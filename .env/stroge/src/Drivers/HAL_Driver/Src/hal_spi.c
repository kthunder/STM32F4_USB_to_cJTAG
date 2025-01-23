#include "stm32f4xx.h"
#include "hal_spi.h"
#include "hal_def.h"

/***
 * SPI初始化
 * 1. 设置波特率
 * 2. 设置极性相位
 * 3. 设置DFF位，定义数据帧格式
 * 4. 配置LSB或者MSB
 * 5. 设置FRF位
 */
uint32_t SPI_Init(SPI_TypeDef *SPIx)
{
    SET_BIT(SPIx->CR1, SPI_CR1_CPHA);
    SET_BIT(SPIx->CR1, SPI_CR1_CPOL);
    SET_BIT(SPIx->CR1, SPI_CR1_MSTR);
    CLEAR_BIT(SPIx->CR1, SPI_CR1_LSBFIRST);
    // SET_BIT(SPIx->CR1, SPI_CR1_SSM);
    CLEAR_BIT(SPIx->CR1, SPI_CR1_BIDIMODE);
    WRITE_BIT(SPIx->CR1, SPI_CR1_BR, 6);

    CLEAR_BIT(SPIx->CR2, SPI_CR2_FRF);
    SET_BIT(SPIx->CR2, SPI_CR2_SSOE);
    SET_BIT(SPIx->CR1, SPI_CR1_SPE);

    return HAL_OK;
}

uint32_t SPI_TransmitReceive(SPI_TypeDef *SPIx, uint8_t *pTxBuffer, uint8_t *pRxBuffer, uint32_t len)
{
    SET_BIT(SPIx->CR1, SPI_CR1_SPE);
    // CLEAR_BIT(SPIx->CR1, SPI_CR1_SSI)
    while (len)
    {
        WAITE_LOW(SPIx->SR, SPI_SR_BSY);

        WAITE_HIGH(SPIx->SR, SPI_SR_TXE);
        WRITE_REG(SPIx->DR, *(pTxBuffer++));

        WAITE_HIGH(SPIx->SR, SPI_SR_RXNE);
        *(pRxBuffer++) = (uint8_t)READ_REG(SPIx->DR);

        WAITE_LOW(SPIx->SR, SPI_SR_BSY);

        len--;
    }
    // SET_BIT(SPIx->CR1, SPI_CR1_SSI);
    CLEAR_BIT(SPIx->CR1, SPI_CR1_SPE);
    return HAL_OK;
}

uint32_t SPI_Transmit(SPI_TypeDef *SPIx, uint8_t *pTxBuffer, uint32_t len)
{
    uint8_t tmp = 0;
    SET_BIT(SPIx->CR1, SPI_CR1_SPE);
    // CLEAR_BIT(SPIx->CR1, SPI_CR1_SSI)
    while (len)
    {
        WAITE_LOW(SPIx->SR, SPI_SR_BSY);

        WAITE_HIGH(SPIx->SR, SPI_SR_TXE);
        WRITE_REG(SPIx->DR, *(pTxBuffer++));

        WAITE_HIGH(SPIx->SR, SPI_SR_RXNE);
        tmp = (uint8_t)READ_REG(SPIx->DR);

        WAITE_LOW(SPIx->SR, SPI_SR_BSY);

        len--;
    }
    // SET_BIT(SPIx->CR1, SPI_CR1_SSI);
    CLEAR_BIT(SPIx->CR1, SPI_CR1_SPE);

    UNUSED(tmp);
    return HAL_OK;
}

uint32_t SPI_Receive(SPI_TypeDef *SPIx, uint8_t *pRxBuffer, uint32_t len)
{
    SET_BIT(SPIx->CR1, SPI_CR1_SPE);
    // CLEAR_BIT(SPIx->CR1, SPI_CR1_SSI)
    while (len)
    {
        WAITE_LOW(SPIx->SR, SPI_SR_BSY);

        WAITE_HIGH(SPIx->SR, SPI_SR_TXE);
        WRITE_REG(SPIx->DR, 0xFF);

        WAITE_HIGH(SPIx->SR, SPI_SR_RXNE);
        *(pRxBuffer++) = (uint8_t)READ_REG(SPIx->DR);

        WAITE_LOW(SPIx->SR, SPI_SR_BSY);

        len--;
    }
    // SET_BIT(SPIx->CR1, SPI_CR1_SSI);
    CLEAR_BIT(SPIx->CR1, SPI_CR1_SPE);
    return HAL_OK;
}