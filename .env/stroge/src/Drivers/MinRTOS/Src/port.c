#include "MinRTOS.h"

// 临界段嵌套计数器
uint32_t uxCriticalNesting = 0xaaaaaaaa;
TickType_t xTickCount = 0;

void vPortEnterCritical()
{
    portENABLE_INTERUPTS();
    uxCriticalNesting++;
// todo 检查
}

void vPortExitCritical()
{
    uxCriticalNesting--;
    if (uxCriticalNesting == 0)
    {
        portENABLE_INTERUPTS();
    }
    
}