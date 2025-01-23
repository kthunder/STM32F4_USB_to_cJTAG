#include "MinRTOS.h"
#include "cmsis_compiler.h"
#include "log.h"
#include "stm32f4xx.h"
#include "hal_gpio.h"
#include "tools.h"
#define configMAX_PRIORITIES 5

void vTaskDelay(const TickType_t xTicksToDelay);

TCB_t *pxCurrentTCB;
List_t pxReadyTaskList[configMAX_PRIORITIES];
portSTACK_TYPE taskStack[512];
portSTACK_TYPE taskStack2[512];
TCB_t task1TCB;
TCB_t task2TCB;
extern TickType_t xTickCount;

static portSTACK_TYPE IdleTaskStack[512];
TCB_t IdleTaskTCB;

/* 在task.h中定义 */
#define taskYIELD() portYIELD()
/* 在portmacro.h中定义 */
/* 中断控制状态寄存器：0xe000ed04
 * Bit 28 PENDSVSET: PendSV 悬起位
 */
#define portNVIC_INT_CTRL_REG (*((volatile uint32_t *)0xe000ed04))
#define portNVIC_PENDSVSET_BIT (1UL << 28UL)

#define portSY_FULL_READ_WRITE (15)

#define portYIELD()                                     \
    {                                                   \
        /* 触发PendSV，产生上下文切换 */      \
        portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT; \
        __asm volatile("dsb"                            \
                       :                                \
                       :                                \
                       : "memory");                     \
        __asm volatile("isb");                          \
    }

void prvInitTaskLists()
{
    for (uint32_t uxPriority = 0; uxPriority < configMAX_PRIORITIES; uxPriority++)
        vListInit(&(pxReadyTaskList[uxPriority]));
}
void xTaskIncremmentTick()
{
    TCB_t *pxTCB = NULL;
    const TickType_t xConstTickCount = xTickCount + 1;
    xTickCount = xConstTickCount;

    List_t list = pxReadyTaskList[0];
    ListItem_t *item = list.pxIndex->pxNext;

    while (item != list.pxIndex)
    {
        pxTCB = item->pvOwner;

        if (pxTCB->xTicksToDelay > 0)
        {
            pxTCB->xTicksToDelay--;
        }

        item = item->pxNext;
    }

    portYIELD();
}
// 1、多寄存器寻址：
// LDMIA R0!,{R1-R4}
// 等效于
//                                ;R1<----[R0]
//                                ;R2<----[R0+4]
//                                ;R3<----[R0+8]
//                                ;R4<----[R0+12]

void SVC_Handler()
{
    // log_info(__func__);
    __asm volatile(
        ".global pxCurrentTCB             \n"
        "    ldr r3, =pxCurrentTCB        \n"
        "    ldr r1, [r3]                 \n"
        "    ldr r0, [r1]                 \n"
        "    ldmia r0!, {r4-r11}          \n"
        "    msr psp, r0                  \n"
        "    isb                          \n"
        "    mov r0, #0                   \n"
        "    msr basepri, r0              \n"
        "    orr r14, #0xD                \n"
        "    bx r14                       \n");
}

void PendSV_Handler()
{
    __asm volatile(
        ".global pxCurrentTCB             \n"
        "    mrs r0, psp                  \n"
        "    isb                          \n"
        "    ldr r3, =pxCurrentTCB        \n"
        "    ldr r2, [r3]                 \n"
        "    stmdb r0!, {r4-r11}          \n"
        "    str r0, [r2]                 \n"
        "    stmdb sp!, {r3, r14}         \n"
        "    mov r0, %0                   \n"
        "    msr basepri, r0              \n"
        "    dsb                          \n"
        "    isb                          \n"
        "    bl vTaskSwitchContext        \n"
        "    mov r0, #0                   \n"
        "    msr basepri, r0              \n"
        "    ldmia sp!, {r3, r14}         \n"
        "    ldr r1, [r3]                 \n"
        "    ldr r0, [r1]                 \n"
        "    ldmia r0!, {r4-r11}          \n"
        "    msr psp, r0                  \n"
        "    isb                          \n"
        "    bx r14                       \n"
        "    nop                          \n"
        :
        : "r"(configMAX_SYSCALL_INTERRUPT_PRIORITY)
        :);
}

void SysTick_Handler()
{
    portDIASBLE_INTERUPTS();

    xTaskIncremmentTick();

    portENABLE_INTERUPTS();
}

void prvStartFirstTask()
{
    log_info(__func__);
    __asm volatile(
        "    cpsie i \n"
        "    cpsie f \n"
        "    dsb     \n"
        "    isb     \n"
        "    svc 0   \n"
        "    nop     \n");
}

static void prvTaskExitError()
{
    while (1)
        ;
}
static void prvIdleTask()
{
    log_info(__func__);
    while (1)
        ;
}

static void prvInitNewTask(TaskFunction_t pxTaskCode,
                           const char *const pcName,
                           const uint32_t ulStackDepth,
                           void *const pvParameters,
                           TaskHandle_t *const pxCreatedTask,
                           TCB_t *const pxNewTCB)
{
    portSTACK_TYPE *pxTopOfStack;

    pxTopOfStack = pxNewTCB->pxStack + (ulStackDepth - 1);
    pxTopOfStack = (portSTACK_TYPE *)((uint32_t)pxTopOfStack & (~((uint32_t)7)));
    for (size_t i = 0; i < 16; i++)
    {
        pxNewTCB->pcTaskName[i] = pcName[i];
        if (pcName[i] == '\0')
        {
            break;
        }
    }
    pxNewTCB->pcTaskName[16 - 1] = '\0';

    vListInitListItem(&(pxNewTCB->xStateListItem));
    pxNewTCB->xStateListItem.pvOwner = pxNewTCB;

    pxNewTCB->pxTopOfStack = pxPortInitStack(pxTopOfStack, pxTaskCode, pvParameters);
    if ((void *)pxCreatedTask != NULL)
    {
        *pxCreatedTask = (TaskHandle_t)pxNewTCB;
    }
}

TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode,
                               const char *const pcName,
                               const uint32_t ulStackDepth,
                               void *const pvParameters,
                               portSTACK_TYPE *const puxStackBuffer,
                               TCB_t *const pxTaskBuffer)
{
    TCB_t *pxNewTCB;
    TaskHandle_t xReturn;

    if ((pxTaskBuffer != NULL) && (puxStackBuffer != NULL))
    {
        pxNewTCB = (TCB_t *)pxTaskBuffer;
        pxNewTCB->pxStack = (portSTACK_TYPE *)puxStackBuffer;

        prvInitNewTask(pxTaskCode, pcName, ulStackDepth, pvParameters, &xReturn, pxNewTCB);
    }
    else
    {
        xReturn = NULL;
    }

    return xReturn;
}

portSTACK_TYPE *pxPortInitStack(portSTACK_TYPE *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters)
{
    pxTopOfStack--;
    *pxTopOfStack = 0x01000000;
    pxTopOfStack--;
    *pxTopOfStack = (portSTACK_TYPE)pxCode & 0xfffffffeUL;
    pxTopOfStack--;
    *pxTopOfStack = (portSTACK_TYPE)prvTaskExitError;
    pxTopOfStack -= 5;
    *pxTopOfStack = (portSTACK_TYPE)pvParameters;

    pxTopOfStack -= 8;
    return pxTopOfStack;
}

void vTaskSwitchContext()
{
    if (pxCurrentTCB == &IdleTaskTCB)
    {
        // log_info("pxCurrentTCB IdleTaskTCB");
        if (task1TCB.xTicksToDelay == 0)
        {
            pxCurrentTCB = &task1TCB;
            // log_info("to task1TCB");
        }
        else if (task2TCB.xTicksToDelay == 0)
        {
            pxCurrentTCB = &task2TCB;
            // log_info("to task2TCB");
        }
        else
        {
            return;
        }
    }
    else
    {

        if (pxCurrentTCB == &task1TCB)
        {
            // log_info("pxCurrentTCB task1TCB");
            if (task2TCB.xTicksToDelay == 0)
            {
                pxCurrentTCB = &task2TCB;
            }
            else if (pxCurrentTCB->xTicksToDelay != 0)
            {
                pxCurrentTCB = &IdleTaskTCB;
            }
            else
            {
                return;
            }
        }
        else if (pxCurrentTCB == &task2TCB)
        {
            {
                // log_info("pxCurrentTCB task2TCB");
                if (task1TCB.xTicksToDelay == 0)
                {
                    pxCurrentTCB = &task1TCB;
                }
                else if (pxCurrentTCB->xTicksToDelay != 0)
                {
                    pxCurrentTCB = &IdleTaskTCB;
                }
                else
                {
                    return;
                }
            }
        }
    }
}

void task1(void *param)
{
    (void)param;
    while (1)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_6);
        vTaskDelay(1000);
        GPIO_ResetBits(GPIOA, GPIO_Pin_6);
        vTaskDelay(1000);
    }
}
void task2(void *param)
{
    (void)param;
    while (1)
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_7);
        vTaskDelay(1000);
        GPIO_SetBits(GPIOA, GPIO_Pin_7);
        vTaskDelay(1000);
    }
}

void vTaskDelay(const TickType_t xTicksToDelay)
{
    TCB_t *pxTCB = NULL;
    pxTCB = pxCurrentTCB;
    pxTCB->xTicksToDelay = xTicksToDelay;
    taskYIELD();
}

#define portNVIC_SYSPRI2_REG (*((volatile uint32_t *)0xe000ed20))

#define portNVIC_PENDSV_PRI (((uint32_t)configKERNEL_INTERRUPT_PRIORITY) << 16UL)
#define portNVIC_SYSTICK_PRI (((uint32_t)configKERNEL_INTERRUPT_PRIORITY) << 24UL)

void vTaskStartScheduler()
{
    portNVIC_SYSPRI2_REG |= portNVIC_PENDSV_PRI;
    portNVIC_SYSPRI2_REG |= portNVIC_SYSTICK_PRI;

    prvInitTaskLists();

    TaskHandle_t handle0 = xTaskCreateStatic(prvIdleTask, "IDLE", 512, NULL, IdleTaskStack, &IdleTaskTCB);
    TaskHandle_t handle1 = xTaskCreateStatic(task1, "TASK1", 512, NULL, taskStack, &task1TCB);
    TaskHandle_t handle2 = xTaskCreateStatic(task2, "TASK2", 512, NULL, taskStack2, &task2TCB);
    (void)handle0;
    (void)handle1;
    (void)handle2;
    vListInsert(&pxReadyTaskList[0], &(task1TCB.xStateListItem));
    vListInsert(&pxReadyTaskList[0], &(task2TCB.xStateListItem));
    vListInsert(&pxReadyTaskList[0], &(IdleTaskTCB.xStateListItem));

    pxCurrentTCB = &task1TCB;
    log_info("main");
    log_info("&task1TCB       %p", &task1TCB);
    log_info("&task2TCB       %p", &task2TCB);
    log_info("&IdleTaskTCB    %p", &IdleTaskTCB);
    log_info("pxCurrentTCB    %p", pxCurrentTCB);
    // 关闭中断，避免systick中断打断SCV中断
    portDIASBLE_INTERUPTS();
    SysTick_Config(168000000 / 1000);
    prvStartFirstTask();
}
