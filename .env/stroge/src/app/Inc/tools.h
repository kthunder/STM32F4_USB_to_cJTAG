#ifndef __TOOLS_H
#define __TOOLS_H

#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"

#define GET_BIT(value, bit) ((value) & (1 << (bit)))  // 读取指定位
#define CPL_BIT(value, bit) ((value) ^= (1 << (bit))) // 取反指定位

// #define SET_BIT(value,bit) ((value)|= (1<<(bit)))   //把某个位置1
// #define CLEAR_BIT(value,bit) ((value)&=~(1<<(bit))) //把某个位置0

void delay_ms(uint32_t ms);

uint32_t HAL_GetTick();

static inline void Active_Soft_EXIT(IRQn_Type nIRQ)
{
    NVIC_EnableIRQ(nIRQ);
    SET_BIT(SCB->CCR, SCB_CCR_USERSETMPEND_Msk);
    WRITE_REG(NVIC->STIR, nIRQ);
}

#define assert_param(expr) \
    ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
static inline void assert_failed(uint8_t *file, uint32_t line)
{
#ifndef __clang__
    printf("Wrong parameters value: file %s on line %lu\n", file, line);
#else
    printf("Wrong parameters value: file %s on line %u\n", file, line);
#endif
    while (1)
    {
    }
}

#endif