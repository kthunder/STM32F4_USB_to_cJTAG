#include "stm32f4xx.h"
#include <stdio.h>
#define SECTION_DATA(sec) __attribute__((section(sec)))

__WEAK void Default_Handler(void)
{
    __ASM("b .");
}

#define FUNC_ALIAS(func_from, func_to) void func_to(void) __attribute__((alias(#func_from), weak))

FUNC_ALIAS(Default_Handler, NMI_Handler);
FUNC_ALIAS(Default_Handler, HardFault_Handler);
FUNC_ALIAS(Default_Handler, MemManage_Handler);
FUNC_ALIAS(Default_Handler, BusFault_Handler);
FUNC_ALIAS(Default_Handler, UsageFault_Handler);
FUNC_ALIAS(Default_Handler, SVC_Handler);
FUNC_ALIAS(Default_Handler, DebugMon_Handler);
FUNC_ALIAS(Default_Handler, PendSV_Handler);
FUNC_ALIAS(Default_Handler, SysTick_Handler);
FUNC_ALIAS(Default_Handler, WWDG_IRQHandler);
FUNC_ALIAS(Default_Handler, PVD_IRQHandler);
FUNC_ALIAS(Default_Handler, TAMP_STAMP_IRQHandler);
FUNC_ALIAS(Default_Handler, RTC_WKUP_IRQHandler);
FUNC_ALIAS(Default_Handler, FLASH_IRQHandler);
FUNC_ALIAS(Default_Handler, RCC_IRQHandler);
FUNC_ALIAS(Default_Handler, EXTI0_IRQHandler);
FUNC_ALIAS(Default_Handler, EXTI1_IRQHandler);
FUNC_ALIAS(Default_Handler, EXTI2_IRQHandler);
FUNC_ALIAS(Default_Handler, EXTI3_IRQHandler);
FUNC_ALIAS(Default_Handler, EXTI4_IRQHandler);
FUNC_ALIAS(Default_Handler, DMA1_Stream0_IRQHandler);
FUNC_ALIAS(Default_Handler, DMA1_Stream1_IRQHandler);
FUNC_ALIAS(Default_Handler, DMA1_Stream2_IRQHandler);
FUNC_ALIAS(Default_Handler, DMA1_Stream3_IRQHandler);
FUNC_ALIAS(Default_Handler, DMA1_Stream4_IRQHandler);
FUNC_ALIAS(Default_Handler, DMA1_Stream5_IRQHandler);
FUNC_ALIAS(Default_Handler, DMA1_Stream6_IRQHandler);
FUNC_ALIAS(Default_Handler, ADC_IRQHandler);
FUNC_ALIAS(Default_Handler, EXTI9_5_IRQHandler);
FUNC_ALIAS(Default_Handler, TIM1_BRK_TIM9_IRQHandler);
FUNC_ALIAS(Default_Handler, TIM1_UP_TIM10_IRQHandler);
FUNC_ALIAS(Default_Handler, TIM1_TRG_COM_TIM11_IRQHandler);
FUNC_ALIAS(Default_Handler, TIM1_CC_IRQHandler);
FUNC_ALIAS(Default_Handler, TIM2_IRQHandler);
FUNC_ALIAS(Default_Handler, TIM3_IRQHandler);
FUNC_ALIAS(Default_Handler, TIM4_IRQHandler);
FUNC_ALIAS(Default_Handler, I2C1_EV_IRQHandler);
FUNC_ALIAS(Default_Handler, I2C1_ER_IRQHandler);
FUNC_ALIAS(Default_Handler, I2C2_EV_IRQHandler);
FUNC_ALIAS(Default_Handler, I2C2_ER_IRQHandler);
FUNC_ALIAS(Default_Handler, SPI1_IRQHandler);
FUNC_ALIAS(Default_Handler, SPI2_IRQHandler);
FUNC_ALIAS(Default_Handler, USART1_IRQHandler);
FUNC_ALIAS(Default_Handler, USART2_IRQHandler);
FUNC_ALIAS(Default_Handler, EXTI15_10_IRQHandler);
FUNC_ALIAS(Default_Handler, RTC_Alarm_IRQHandler);
FUNC_ALIAS(Default_Handler, OTG_FS_WKUP_IRQHandler);
FUNC_ALIAS(Default_Handler, DMA1_Stream7_IRQHandler);
FUNC_ALIAS(Default_Handler, SDIO_IRQHandler);
FUNC_ALIAS(Default_Handler, TIM5_IRQHandler);
FUNC_ALIAS(Default_Handler, SPI3_IRQHandler);
FUNC_ALIAS(Default_Handler, DMA2_Stream0_IRQHandler);
FUNC_ALIAS(Default_Handler, DMA2_Stream1_IRQHandler);
FUNC_ALIAS(Default_Handler, DMA2_Stream2_IRQHandler);
FUNC_ALIAS(Default_Handler, DMA2_Stream3_IRQHandler);
FUNC_ALIAS(Default_Handler, DMA2_Stream4_IRQHandler);
FUNC_ALIAS(Default_Handler, OTG_FS_IRQHandler);
FUNC_ALIAS(Default_Handler, DMA2_Stream5_IRQHandler);
FUNC_ALIAS(Default_Handler, DMA2_Stream6_IRQHandler);
FUNC_ALIAS(Default_Handler, DMA2_Stream7_IRQHandler);
FUNC_ALIAS(Default_Handler, USART6_IRQHandler);
FUNC_ALIAS(Default_Handler, I2C3_EV_IRQHandler);
FUNC_ALIAS(Default_Handler, I2C3_ER_IRQHandler);
FUNC_ALIAS(Default_Handler, FPU_IRQHandler);
FUNC_ALIAS(Default_Handler, SPI4_IRQHandler);

extern uint8_t _estack[];
extern uint8_t _sidata[];
extern uint8_t _sdata[];
extern uint8_t _edata[];
extern uint8_t _sbss[];
extern uint8_t _ebss[];

static void __memset(uint8_t *dist, uint8_t val, uint32_t len)
{
    while (len--)
        *dist++ = val;
}
static void __memcpy(uint8_t *dist, uint8_t *src, uint32_t len)
{
    while (len--)
        *dist++ = *src++;
}

extern int main();

void Reset_Handler(void)
{
    __set_MSP((uint32_t)_estack);
    __memset(_sbss, 0, _ebss - _sbss);
    __memcpy(_sdata, _sidata, _edata - _sdata);
    SystemInit();
    main();
}

SECTION_DATA(".isr_vector")
// typedef void (*IRQ_HandlerType)(void);
__USED static void (*vector[256])(void) = {
    (void (*)(void))_estack,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0,
    0,
    0,
    0,
    SVC_Handler,
    DebugMon_Handler,
    0,
    PendSV_Handler,
    SysTick_Handler,
    WWDG_IRQHandler,               /* Window WatchDog              */
    PVD_IRQHandler,                /* PVD through EXTI Line detection */
    TAMP_STAMP_IRQHandler,         /* Tamper and TimeStamps through the EXTI line */
    RTC_WKUP_IRQHandler,           /* RTC Wakeup through the EXTI line */
    FLASH_IRQHandler,              /* FLASH                        */
    RCC_IRQHandler,                /* RCC                          */
    EXTI0_IRQHandler,              /* EXTI Line0                   */
    EXTI1_IRQHandler,              /* EXTI Line1                   */
    EXTI2_IRQHandler,              /* EXTI Line2                   */
    EXTI3_IRQHandler,              /* EXTI Line3                   */
    EXTI4_IRQHandler,              /* EXTI Line4                   */
    DMA1_Stream0_IRQHandler,       /* DMA1 Stream 0                */
    DMA1_Stream1_IRQHandler,       /* DMA1 Stream 1                */
    DMA1_Stream2_IRQHandler,       /* DMA1 Stream 2                */
    DMA1_Stream3_IRQHandler,       /* DMA1 Stream 3                */
    DMA1_Stream4_IRQHandler,       /* DMA1 Stream 4                */
    DMA1_Stream5_IRQHandler,       /* DMA1 Stream 5                */
    DMA1_Stream6_IRQHandler,       /* DMA1 Stream 6                */
    ADC_IRQHandler,                /* ADC1                         */
    0,                             /* Reserved                     */
    0,                             /* Reserved                     */
    0,                             /* Reserved                     */
    0,                             /* Reserved                     */
    EXTI9_5_IRQHandler,            /* External Line[9:5]s          */
    TIM1_BRK_TIM9_IRQHandler,      /* TIM1 Break and TIM9          */
    TIM1_UP_TIM10_IRQHandler,      /* TIM1 Update and TIM10        */
    TIM1_TRG_COM_TIM11_IRQHandler, /* TIM1 Trigger and Commutation and TIM11 */
    TIM1_CC_IRQHandler,            /* TIM1 Capture Compare         */
    TIM2_IRQHandler,               /* TIM2                         */
    TIM3_IRQHandler,               /* TIM3                         */
    TIM4_IRQHandler,               /* TIM4                         */
    I2C1_EV_IRQHandler,            /* I2C1 Event                   */
    I2C1_ER_IRQHandler,            /* I2C1 Error                   */
    I2C2_EV_IRQHandler,            /* I2C2 Event                   */
    I2C2_ER_IRQHandler,            /* I2C2 Error                   */
    SPI1_IRQHandler,               /* SPI1                         */
    SPI2_IRQHandler,               /* SPI2                         */
    USART1_IRQHandler,             /* USART1                       */
    USART2_IRQHandler,             /* USART2                       */
    0,                             /* Reserved                     */
    EXTI15_10_IRQHandler,          /* External Line[15:10]s        */
    RTC_Alarm_IRQHandler,          /* RTC Alarm (A and B) through EXTI Line */
    OTG_FS_WKUP_IRQHandler,        /* USB OTG FS Wakeup through EXTI line */
    0,                             /* Reserved     				        */
    0,                             /* Reserved       			        */
    0,                             /* Reserved 					          */
    0,                             /* Reserved                     */
    DMA1_Stream7_IRQHandler,       /* DMA1 Stream7                 */
    0,                             /* Reserved                     */
    SDIO_IRQHandler,               /* SDIO                         */
    TIM5_IRQHandler,               /* TIM5                         */
    SPI3_IRQHandler,               /* SPI3                         */
    0,                             /* Reserved                     */
    0,                             /* Reserved                     */
    0,                             /* Reserved                     */
    0,                             /* Reserved                     */
    DMA2_Stream0_IRQHandler,       /* DMA2 Stream 0                */
    DMA2_Stream1_IRQHandler,       /* DMA2 Stream 1                */
    DMA2_Stream2_IRQHandler,       /* DMA2 Stream 2                */
    DMA2_Stream3_IRQHandler,       /* DMA2 Stream 3                */
    DMA2_Stream4_IRQHandler,       /* DMA2 Stream 4                */
    0,                             /* Reserved                     */
    0,                             /* Reserved                     */
    0,                             /* Reserved                     */
    0,                             /* Reserved                     */
    0,                             /* Reserved                     */
    0,                             /* Reserved                     */
    OTG_FS_IRQHandler,             /* USB OTG FS                   */
    DMA2_Stream5_IRQHandler,       /* DMA2 Stream 5                */
    DMA2_Stream6_IRQHandler,       /* DMA2 Stream 6                */
    DMA2_Stream7_IRQHandler,       /* DMA2 Stream 7                */
    USART6_IRQHandler,             /* USART6                       */
    I2C3_EV_IRQHandler,            /* I2C3 event                   */
    I2C3_ER_IRQHandler,            /* I2C3 error                   */
    0,                             /* Reserved                     */
    0,                             /* Reserved                     */
    0,                             /* Reserved                     */
    0,                             /* Reserved                     */
    0,                             /* Reserved                     */
    0,                             /* Reserved                     */
    0,                             /* Reserved                     */
    FPU_IRQHandler,                /* FPU                          */
    0,                             /* Reserved                     */
    0,                             /* Reserved                     */
    SPI4_IRQHandler                /* SPI4                         */
};
