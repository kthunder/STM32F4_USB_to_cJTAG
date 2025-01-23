#ifndef __HAL_SPI_H
#define __HAL_SPI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f4xx.h"

    uint32_t SPI_Init(SPI_TypeDef *SPIx);
    uint32_t SPI_TransmitReceive(SPI_TypeDef *SPIx, uint8_t *pTxBuffer, uint8_t *pRxBuffer, uint32_t len);
    uint32_t SPI_Transmit(SPI_TypeDef *SPIx, uint8_t *pTxBuffer, uint32_t len);
    uint32_t SPI_Receive(SPI_TypeDef *SPIx,  uint8_t *pRxBuffer, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif