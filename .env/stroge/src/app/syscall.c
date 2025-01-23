#ifndef __clang__
#include <sys/stat.h>
#endif
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hal_usart.h"

// commom functions
__attribute__((weak)) int __io_getchar(FILE *file)
{
    (void)file;
    // USART_Receive(USART1, &ch, 1);// un support
    return 0;
}

__attribute__((weak)) int __io_putchar(char ch, FILE *file)
{
    (void)file;
    USART_Transmit(USART1, (uint8_t *)&ch, 1);
    // ITM_SendChar(ch);
    return 1;
}

//only for llvm
#if defined(__clang_major__)&&(__clang_major__ == 19)
/* Redirect sdtio as per https://github.com/picolibc/picolibc/blob/main/doc/os.md */
static FILE __stdio = FDEV_SETUP_STREAM(__io_putchar, __io_getchar, NULL, _FDEV_SETUP_RW);
FILE *const stdin = &__stdio;
__strong_reference(stdin, stdout);
__strong_reference(stdin, stderr);
#endif


#ifdef __GNUC__
char *__env[1] = {0};

#ifndef __clang__
#define FUNC_PREFIX(string) _##string
#else
#define FUNC_PREFIX(string) string
#endif

__attribute__((weak)) int FUNC_PREFIX(read)(int file, char *ptr, int len)
{
    for (int DataIdx = 0; DataIdx < len; DataIdx++)
        *ptr++ = __io_getchar((FILE *)file);
    return len;
}

__attribute__((weak)) int FUNC_PREFIX(write)(int file, char *ptr, int len)
{
    for (int  DataIdx = 0; DataIdx < len; DataIdx++)
        __io_putchar(*ptr++, (FILE *)file);
    return len;
}

int FUNC_PREFIX(close)(int file)
{
    (void)file;
    return -1;
}

void _exit(int status)
{
    (void)status;
    while (1)
    {
    } /* Make sure we hang here */
}

int remove(const char *path)
{
    (void)path;
    return -1;
}

void *FUNC_PREFIX(sbrk)(size_t incr)
{
    extern char __heap_start;
    extern char __heap_end;
    (void)incr;
    void *ret = 0;
    static char *heap_top = &__heap_start;
    char *new_heap_top = heap_top + incr;
    if (new_heap_top > &__heap_end)
    {
        return (void *)(-1);
    }
    if (new_heap_top < &__heap_start)
        abort();
    ret = (void *)heap_top;
    heap_top = new_heap_top;
    return ret;
}
#endif

#ifndef __clang__
int _kill(int pid, int sig)
{
    (void)pid;
    (void)sig;
    errno = EINVAL;
    return -1;
}

int _getpid(void)
{
    return 1;
}

int _lseek(int file, int ptr, int dir)
{
    (void)file;
    (void)ptr;
    (void)dir;
    return 0;
}

int _fstat(int file, struct stat *st)
{
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file)
{
    (void)file;
    return 1;
}
#endif
