/*
 * Copyright (c) 2020 rxi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "log.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAX_CALLBACKS 32

typedef struct
{
    log_LogFn fn;
    void *udata;
    int level;
} Callback;

// 当前log模块配置结构体
static struct
{
    void *udata;
    log_LockFn lock; // 锁函数
    int level;       // level 用于保存当前的 log 等级，等级大于 level 的 log 才会被输出到标准输出。
    bool quiet;      // quiet 用于打开、关闭 log 输出。
    Callback callbacks[MAX_CALLBACKS];
} Log_ConfigData = {
    .level = LOG_INFO};

static const char *level_strings[] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

#ifdef LOG_USE_COLOR
// #include "SEGGER_RTT.h"
// #define RTT_CTRL_TEXT_BLACK           "\x1B[2;30m"
// #define RTT_CTRL_TEXT_RED             "\x1B[2;31m"
// #define RTT_CTRL_TEXT_GREEN           "\x1B[2;32m"
// #define RTT_CTRL_TEXT_YELLOW          "\x1B[2;33m"
// #define RTT_CTRL_TEXT_BLUE            "\x1B[2;34m"
// #define RTT_CTRL_TEXT_MAGENTA         "\x1B[2;35m"
// #define RTT_CTRL_TEXT_CYAN            "\x1B[2;36m"
// #define RTT_CTRL_TEXT_WHITE           "\x1B[2;37m"
static const char *level_colors[] = {
    // RTT_CTRL_TEXT_WHITE, "\x1b[36m", RTT_CTRL_TEXT_GREEN, "\x1b[33m", "\x1b[31m", RTT_CTRL_TEXT_RED
    "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"};
#endif

// 获取文件名（去掉路径部分）
const char* get_filename(const char* path) {
    const char* filename = strrchr(path, '/'); // 查找最后一个 '/' 字符
    if (filename) {
        return filename + 1; // 返回 '/' 之后的内容
    }
    return path; // 如果没有路径分隔符，直接返回原路径
}

static void stdout_callback(log_Event *ev)
{
    // 时间信息字符串
    char time_string[16];
    time_string[strftime(time_string, sizeof(time_string), "%H:%M:%S", ev->time)] = '\0';
#ifdef LOG_USE_COLOR
    fprintf(ev->udata, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ", time_string, level_colors[ev->level], level_strings[ev->level], get_filename(ev->file_name), ev->line);
    // fprintf(ev->udata, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ", time_string, level_colors[ev->level], level_strings[ev->level], ev->file_name, ev->line);
#else
    // fprintf(ev->udata, "%s %-5s %s:%d: ", time_string, level_strings[ev->level], ev->file_name, ev->line);
    // 时间
    // fprintf(ev->udata, "%s ", time_string);
    // 日志等级
    fprintf(ev->udata, "%-5s ", level_strings[ev->level]);
    // 文件名和行号
    fprintf(ev->udata, "%s:%d: ", ev->file_name, ev->line);
#endif
    vfprintf(ev->udata, ev->fmt, ev->ap);
    fprintf(ev->udata, "\n");
    fflush(ev->udata);
}
#if 0
static void file_callback(log_Event* ev)
{
    char buf[64];
    buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ev->time)] = '\0';
    fprintf(ev->udata, "%s %-5s %s:%d: ", buf, level_strings[ev->level], ev->file_name, ev->line);
    vfprintf(ev->udata, ev->fmt, ev->ap);
    fprintf(ev->udata, "\n");
    fflush(ev->udata);
}
#endif
static void lock(void)
{
    if (Log_ConfigData.lock)
    {
        Log_ConfigData.lock(true, Log_ConfigData.udata);
    }
}

static void unlock(void)
{
    if (Log_ConfigData.lock)
    {
        Log_ConfigData.lock(false, Log_ConfigData.udata);
    }
}

void log_set_level(int level)
{
    Log_ConfigData.level = level;
}

void log_set_quiet(bool enable)
{
    Log_ConfigData.quiet = enable;
}

// 添加callback，用于增加日志输出方式
int log_add_callback(log_LogFn fn, void *udata, int level)
{
    for (int i = 0; i < MAX_CALLBACKS && !Log_ConfigData.callbacks[i].fn; i++)
    {
        Log_ConfigData.callbacks[i] = (Callback){fn, udata, level};
        return 0;
    }
    return -1;
}

// 日志输出函数
void log_log(int level, const char *file_name, int line, const char *fmt, ...)
{
    time_t t = osKernelGetTickCount()/1000;

    log_Event ev = {
        .fmt = fmt,
        .file_name = file_name,
        .time = localtime(&t),
        .udata = stdout,
        .line = line,
        .level = level,
    };
    if (ev.level >= Log_ConfigData.level)
    {
        va_start(ev.ap, fmt);
        stdout_callback(&ev);
        va_end(ev.ap);
    }

    lock();
    // callback输出
    for (int i = 0; i < MAX_CALLBACKS && Log_ConfigData.callbacks[i].fn; i++)
    {
        Callback *cb = &Log_ConfigData.callbacks[i];
        if (!Log_ConfigData.quiet && level >= cb->level)
        {
            ev.udata = cb->udata;
            va_start(ev.ap, fmt);
            cb->fn(&ev);
            va_end(ev.ap);
        }
    }
    unlock();
}

void log_hex(char * ucInfo, uint8_t * ucData, uint32_t len)
{
    log_info("%s : ", ucInfo);
    for (size_t i = 0; i < len; i++)
    {
        printf("%02X", ucData[i]);
        if ((i + 1 == len) || ((i + 1) % 20 == 0))
            printf("\n");
    }
}

#ifdef TEST_LOG
int main(int argc, char *argv[])
{
    // FILE* fp = fopen("./log_info.txt", "ab");
    // if (fp == NULL)
    //     return -1;
    // log_add_callback(file_callback, fp, LOG_INFO);

    log_trace("log_trace");
    log_debug("log_debug");
    log_info("log_info");
    log_warn("log_warn");
    log_error("log_error");
    log_fatal("log_fatal");

    // fclose(fp);
    return 0;
}
#endif