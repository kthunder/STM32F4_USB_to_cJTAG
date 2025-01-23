#include <stdint.h>
#include "stm32f4xx.h"
#include "hal_gpio.h"

void GPIO_SetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    GPIOx->BSRR = GPIO_Pin;
}

void GPIO_ResetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    GPIOx->BSRR = GPIO_Pin << 16;
}

void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    GPIOx->ODR ^= GPIO_Pin;
}

void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
    for (uint32_t pin_num = 0; (pin_num < 16); pin_num++)
    {
        if (GPIO_Init->pin & (1 << pin_num))
        {
            SET_BIT(GPIOx->MODER, GPIO_Init->mode << (pin_num * 2));
            SET_BIT(GPIOx->OTYPER, GPIO_Init->otype << pin_num);
            SET_BIT(GPIOx->OSPEEDR, GPIO_Init->speed << (pin_num * 2));
            SET_BIT(GPIOx->PUPDR, GPIO_Init->pupd << (pin_num * 2));
            SET_BIT(GPIOx->AFR[pin_num / 8], GPIO_Init->af << ((pin_num % 8) * 4));
        }
    }
}