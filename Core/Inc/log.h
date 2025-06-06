/**
 * Copyright (c) 2020 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

#define LOG_VERSION "0.1.0"
#define LOG_USE_COLOR

#ifdef E902
#define sys_time64() (*(uint64_t *)0xE000BFF8)
#define sys_time32() (*(uint32_t *)0xE000BFF8)
#elif STM32F401xC
#define sys_time64() (0)
#define sys_time32() (0)
#else
#define sys_time64() (0)
#define sys_time32() (0)
#endif

// 一条 log 信息对应一个 log_Event。暴露这个数据结构是为了用户可以编写自己的 log 打印函数 log_LogFn 以输出 log。
typedef struct
{
    va_list ap;
    const char *fmt;
    const char *file_name;
    struct tm *time;
    void *udata;
    int line;
    int level;
} log_Event;

// 定义函数指针类型 log_LogFn : void (*)(log_Event *ev);
typedef void (*log_LogFn)(log_Event *ev);
// 定义函数指针类型 log_LockFn : void (*)(bool lock, void *udata);
typedef void (*log_LockFn)(bool lock, void *udata);

// 日志等级枚举
enum
{
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
};

#define SAFE_NAME(__NAME) __##__LINE__##__NAME

#define measure_time(...)                            \
    ({                                               \
        int64_t SAFE_NAME(StartTime) = sys_time64(); \
        __VA_ARGS__;                                 \
        sys_time64() - SAFE_NAME(StartTime);         \
    })

#define measure_time_p(...)                                                  \
    ({                                                                       \
        int64_t SAFE_NAME(StartTime) = sys_time64();                         \
        __VA_ARGS__;                                                         \
        int64_t SAFE_NAME(UseTime) = sys_time64() - SAFE_NAME(StartTime);    \
        printf("Use : %d ms\r\n", (uint32_t)(SAFE_NAME(UseTime) / 8000000)); \
        SAFE_NAME(UseTime);                                                  \
    })

/*******************************API*******************************/
// 提供了一个打印相关的 API
#define log_trace(...) log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...) log_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) log_log(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)
/*******************************extend API*******************************/
// 控制和功能扩展。
// void log_set_lock(log_LockFn fn, void *udata);
void log_set_level(int level);
void log_set_quiet(bool enable);
int log_add_callback(log_LogFn fn, void *udata, int level);
// 输出
void log_log(int level, const char *file, int line, const char *fmt, ...);
void log_hex(char *ucInfo, uint8_t *ucData, uint32_t len);

#endif
