#ifndef __HAL_GPIO_H
#define __HAL_GPIO_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f4xx.h"
    // 1.GPIO_Mode_AIN          模拟输入
    // 2.GPIO_Mode_IN_FLOATING  浮空输入
    // 3.GPIO_Mode_IPD          下拉输入
    // 4.GPIO_Mode_IPU          上拉输入
    // 5.GPIO_Mode_Out_OD       开漏输出
    // 6.GPIO_Mode_Out_PP       推挽输出
    // 7.GPIO_Mode_AF_OD        复用开漏输出
    // 8.GPIO_Mode_AF_PP        复用推挽输出
    // 介绍及应用场景 : http://www.openedv.com/posts/list/32730.htm

    // GPIO state def
    typedef enum
    {
        GPIO_PIN_RESET = 0,
        GPIO_PIN_SET = 1
    } GPIO_PinState;
    typedef enum
    {
        GPIO_PUPD_NONE = 0,
        GPIO_PUPD_PULL_UP = 1,
        GPIO_PUPD_PULL_DOWN = 2,
        GPIO_PUPD_RESERVE = 3,
    } GPIO_Pupd;
    typedef enum
    {
        GPIO_MODE_IN = 0,
        GPIO_MODE_OUT = 1,
        GPIO_MODE_AF_OUT = 2,
        GPIO_MODE_ANALOG = 3,
    } GPIO_Mode;
    typedef enum
    {
        GPIO_OTYPE_PP = 0,
        GPIO_OTYPE_OD = 1,
    } GPIO_Otype;
    typedef enum
    {
        GPIO_AF_0 = 0,
        GPIO_AF_1,
        GPIO_AF_2,
        GPIO_AF_3,
        GPIO_AF_4,
        GPIO_AF_5,
        GPIO_AF_6,
        GPIO_AF_7,
        GPIO_AF_8,
        GPIO_AF_9,
        GPIO_AF_10,
        GPIO_AF_11,
        GPIO_AF_12,
        GPIO_AF_13,
        GPIO_AF_14,
        GPIO_AF_15,
    } GPIO_AF;
    // GPIO speed def
    typedef enum
    {
        GPIO_SPEED_FREQ_LOW = 0,      /*!< IO works at 2 MHz, please refer to the product datasheet */
        GPIO_SPEED_FREQ_MEDIUM = 1,   /*!< range 12,5 MHz to 50 MHz, please refer to the product datasheet */
        GPIO_SPEED_FREQ_HIGH = 2,     /*!< range 25 MHz to 100 MHz, please refer to the product datasheet  */
        GPIO_SPEED_FREQ_VERY_HIGH = 3 /*!< range 50 MHz to 200 MHz, please refer to the product datasheet  */
    } GPIO_Speed;
    // GPIO pin def
    typedef enum
    {
        GPIO_Pin_0 = (1u << 0),
        GPIO_Pin_1 = (1u << 1),
        GPIO_Pin_2 = (1u << 2),
        GPIO_Pin_3 = (1u << 3),
        GPIO_Pin_4 = (1u << 4),
        GPIO_Pin_5 = (1u << 5),
        GPIO_Pin_6 = (1u << 6),
        GPIO_Pin_7 = (1u << 7),
        GPIO_Pin_8 = (1u << 8),
        GPIO_Pin_9 = (1u << 9),
        GPIO_Pin_10 = (1u << 10),
        GPIO_Pin_11 = (1u << 11),
        GPIO_Pin_12 = (1u << 12),
        GPIO_Pin_13 = (1u << 13),
        GPIO_Pin_14 = (1u << 14),
        GPIO_Pin_15 = (1u << 15),
        GPIO_Pin_16 = (1u << 16),
        GPIO_Pin_All = (0xFFFFu),
    } GPIO_Pin;

// GPIO AF def
#define GPIO_AF0_RTC_50Hz ((uint8_t)0x00)   /* RTC_50Hz Alternate Function mapping                       */
#define GPIO_AF0_MCO ((uint8_t)0x00)        /* MCO (MCO1 and MCO2) Alternate Function mapping            */
#define GPIO_AF0_TAMPER ((uint8_t)0x00)     /* TAMPER (TAMPER_1 and TAMPER_2) Alternate Function mapping */
#define GPIO_AF0_SWJ ((uint8_t)0x00)        /* SWJ (SWD and JTAG) Alternate Function mapping             */
#define GPIO_AF0_TRACE ((uint8_t)0x00)      /* TRACE Alternate Function mapping                          */
#define GPIO_AF1_TIM1 ((uint8_t)0x01)       /* TIM1 Alternate Function mapping */
#define GPIO_AF1_TIM2 ((uint8_t)0x01)       /* TIM2 Alternate Function mapping */
#define GPIO_AF2_TIM3 ((uint8_t)0x02)       /* TIM3 Alternate Function mapping */
#define GPIO_AF2_TIM4 ((uint8_t)0x02)       /* TIM4 Alternate Function mapping */
#define GPIO_AF2_TIM5 ((uint8_t)0x02)       /* TIM5 Alternate Function mapping */
#define GPIO_AF3_TIM8 ((uint8_t)0x03)       /* TIM8 Alternate Function mapping  */
#define GPIO_AF3_TIM9 ((uint8_t)0x03)       /* TIM9 Alternate Function mapping  */
#define GPIO_AF3_TIM10 ((uint8_t)0x03)      /* TIM10 Alternate Function mapping */
#define GPIO_AF3_TIM11 ((uint8_t)0x03)      /* TIM11 Alternate Function mapping */
#define GPIO_AF4_I2C1 ((uint8_t)0x04)       /* I2C1 Alternate Function mapping */
#define GPIO_AF4_I2C2 ((uint8_t)0x04)       /* I2C2 Alternate Function mapping */
#define GPIO_AF4_I2C3 ((uint8_t)0x04)       /* I2C3 Alternate Function mapping */
#define GPIO_AF5_SPI1 ((uint8_t)0x05)       /* SPI1 Alternate Function mapping        */
#define GPIO_AF5_SPI2 ((uint8_t)0x05)       /* SPI2/I2S2 Alternate Function mapping   */
#define GPIO_AF5_I2S3ext ((uint8_t)0x05)    /* I2S3ext_SD Alternate Function mapping  */
#define GPIO_AF6_SPI3 ((uint8_t)0x06)       /* SPI3/I2S3 Alternate Function mapping  */
#define GPIO_AF6_I2S2ext ((uint8_t)0x06)    /* I2S2ext_SD Alternate Function mapping */
#define GPIO_AF7_USART1 ((uint8_t)0x07)     /* USART1 Alternate Function mapping     */
#define GPIO_AF7_USART2 ((uint8_t)0x07)     /* USART2 Alternate Function mapping     */
#define GPIO_AF7_USART3 ((uint8_t)0x07)     /* USART3 Alternate Function mapping     */
#define GPIO_AF7_I2S3ext ((uint8_t)0x07)    /* I2S3ext_SD Alternate Function mapping */
#define GPIO_AF8_UART4 ((uint8_t)0x08)      /* UART4 Alternate Function mapping  */
#define GPIO_AF8_UART5 ((uint8_t)0x08)      /* UART5 Alternate Function mapping  */
#define GPIO_AF8_USART6 ((uint8_t)0x08)     /* USART6 Alternate Function mapping */
#define GPIO_AF9_CAN1 ((uint8_t)0x09)       /* CAN1 Alternate Function mapping  */
#define GPIO_AF9_CAN2 ((uint8_t)0x09)       /* CAN2 Alternate Function mapping  */
#define GPIO_AF9_TIM12 ((uint8_t)0x09)      /* TIM12 Alternate Function mapping */
#define GPIO_AF9_TIM13 ((uint8_t)0x09)      /* TIM13 Alternate Function mapping */
#define GPIO_AF9_TIM14 ((uint8_t)0x09)      /* TIM14 Alternate Function mapping */
#define GPIO_AF10_OTG_FS ((uint8_t)0x0A)    /* OTG_FS Alternate Function mapping */
#define GPIO_AF10_OTG_HS ((uint8_t)0x0A)    /* OTG_HS Alternate Function mapping */
#define GPIO_AF11_ETH ((uint8_t)0x0B)       /* ETHERNET Alternate Function mapping */
#define GPIO_AF12_FSMC ((uint8_t)0x0C)      /* FSMC Alternate Function mapping                     */
#define GPIO_AF12_OTG_HS_FS ((uint8_t)0x0C) /* OTG HS configured in FS, Alternate Function mapping */
#define GPIO_AF12_SDIO ((uint8_t)0x0C)      /* SDIO Alternate Function mapping                     */
#define GPIO_AF13_DCMI ((uint8_t)0x0D)      /* DCMI Alternate Function mapping */
#define GPIO_AF15_EVENTOUT ((uint8_t)0x0F)

    typedef struct
    {
        GPIO_Pin pin;
        GPIO_Mode mode;
        GPIO_Otype otype;
        GPIO_Speed speed;
        GPIO_Pupd pupd;
        GPIO_AF af;
    } GPIO_InitTypeDef;

    void GPIO_SetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

    void GPIO_ResetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

    void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

    void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *config);

#ifdef __cplusplus
}
#endif

#endif