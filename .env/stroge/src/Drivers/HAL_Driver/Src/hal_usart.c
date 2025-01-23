#include "stm32f4xx.h"
#include "hal_usart.h"
#include "hal_def.h"

void USART_Init(USART_TypeDef *USARTx)
{
    USARTx->BRR = 0x2D9; // 设置波特率

    SET_BIT(USARTx->CR1, USART_CR1_UE);  // 使能USART
    CLEAR_BIT(USARTx->CR1, USART_CR1_M); // 一个起始位，8个数据位，n个停止位
    SET_BIT(USARTx->CR1, USART_CR1_TE);  // TX使能
    SET_BIT(USARTx->CR1, USART_CR1_RE);  // RX使能
}

uint32_t USART_Transmit(USART_TypeDef *USARTx, uint8_t *pData, uint16_t nSize)
{
    while (nSize > 0)
    {
        while (!READ_BIT(USARTx->SR, USART_SR_TXE))
            ;
        USARTx->DR = *pData++;
        nSize--;
        while (!READ_BIT(USARTx->SR, USART_SR_TC))
            ;
    }
    return nSize;
}

// todo
uint32_t USART_Receive(USART_TypeDef *USARTx, uint8_t *pData, uint16_t nSize)
{
    UNUSED(USARTx);
    UNUSED(pData);
    UNUSED(nSize);
    return 0;
}