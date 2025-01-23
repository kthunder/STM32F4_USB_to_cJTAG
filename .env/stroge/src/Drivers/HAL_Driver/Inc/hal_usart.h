#ifndef __HAL_USART_H
#define __HAL_USART_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f4xx.h"

    void USART_Init(USART_TypeDef *USARTx);

    uint32_t USART_Transmit(USART_TypeDef *USARTx, uint8_t *pData, uint16_t nSize);
    uint32_t USART_Receive(USART_TypeDef *USARTx, uint8_t *pData, uint16_t nSize);

#ifdef __cplusplus
}
#endif

#endif
