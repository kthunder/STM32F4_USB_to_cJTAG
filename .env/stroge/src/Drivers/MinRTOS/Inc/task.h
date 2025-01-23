#ifndef __TASK_H
#define __TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "MinRTOS.h"
// 进入、退出临界区，不带保护
#define taskENTER_CRITICAL() portENTER_CRITICAL()
#define taskEXIT_CRITICAL() portEXIT_CRITICAL()
// 进入、退出临界区，带保护
#define taskENTER_CRITICAL_FROM_ISR() portSET_INTERUPT_MASK_FROM_ISR()
#define taskEXIT_CRITICAL_FROM_ISR(x) portCLEAR_INTERUPT_MASK_FROM_ISR(x)
// 屏蔽、恢复中断，带保护
#define taskSET_INTERUPT_MASK_FROM_ISR() portSET_INTERUPT_MASK_FROM_ISR()
#define taskCLEAR_INTERUPT_MASK_FROM_ISR(x) portCLEAR_INTERUPT_MASK_FROM_ISR(x)

// demo
// /* 在中断场合，临界段可以嵌套 */
// {
//     uint32_t ulReturn;
//     /* 进入临界段，临界段可以嵌套 */
//     ulReturn = taskENTER_CRITICAL_FROM_ISR();
//
//     /* 临界段代码 */
//
//     /* 退出临界段 */
//     taskEXIT_CRITICAL_FROM_ISR( ulReturn );
// }
//
// /* 在非中断场合，临界段不能嵌套 */
// {
//     /* 进入临界段 */
//     taskENTER_CRITICAL();
//
//     /* 临界段代码 */
//
//     /* 退出临界段*/
//     taskEXIT_CRITICAL();
// }

typedef void* TaskHandle_t;

typedef struct tskTaskControlBlock {
    volatile portSTACK_TYPE* pxTopOfStack;

    ListItem_t xStateListItem;

    portSTACK_TYPE* pxStack;

    TickType_t xTicksToDelay;

    char pcTaskName[16];
} TCB_t;

portSTACK_TYPE* pxPortInitStack(portSTACK_TYPE* pxTopOfStack, TaskFunction_t pxCode, void* pvParameters);

TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode,
    const char* const pcName,
    const uint32_t ulStackDepth,
    void* const pvParameters,
    portSTACK_TYPE* const puxStackBuffer,
    TCB_t* const pxTaskBuffer);

void vTaskStartScheduler();

#ifdef __cplusplus
}
#endif

#endif