#include "w25qxx.h"
#include "stm32f4xx.h"
#include "hal_flash.h"
#include "hal_gpio.h"
#include "hal_spi.h"
#include "log.h"
#include "tools.h"

// Block 64K
// Sector 4K
// Page 256B
// status :
// s0 : BUSY Erase/Write In Progress
// s1 : WEL Write Enable Latch
// s2 : BP0 Block Protect Bits
// s3 : BP1 Block Protect Bits
// s4 : BP2 Block Protect Bits
// s5 : TB Top/Bottom Block Protect
// s6 : SEC Sector/Block Protect Bit
// s7 : CMP Complement Protect

// W25X16读写指令表
#define W25X_WriteEnable 0x06
#define W25X_WriteDisable 0x04

#define W25X_ReadStatusReg1 0x05
#define W25X_WriteStatusReg1 0x01
#define W25X_ReadStatusReg2 0x35
#define W25X_WriteStatusReg2 0x31
#define W25X_ReadStatusReg3 0x15
#define W25X_WriteStatusReg3 0x11

#define W25X_ReadData 0x03
#define W25X_FastReadData 0x0B
#define W25X_FastReadDual 0x3B

#define W25X_PageProgram 0x02

#define W25X_BlockErase 0xD8
#define W25X_SectorErase 0x20
#define W25X_ChipErase 0xC7

#define W25X_PowerDown 0xB9
#define W25X_ReleasePowerDown 0xAB

#define W25X_DeviceID 0xAB
#define W25X_ManufactDeviceID 0x90
#define W25X_JedecDeviceID 0x9F

#define ENABLE_CS() GPIO_ResetBits(GPIOB, GPIO_Pin_0)
#define DISABLE_CS() GPIO_SetBits(GPIOB, GPIO_Pin_0)

static uint32_t W25Qx_WriteEnable(void)
{
    uint8_t cmd = W25X_WriteEnable;
    ENABLE_CS();
    SPI_Transmit(SPI1, &cmd, 1);
    DISABLE_CS();
    return 0;
}

// static uint32_t W25Qx_WriteDisable(void)
// {
//     uint8_t cmd = W25X_WriteDisable;
//     ENABLE_CS();
//     SPI_Transmit(SPI1, &cmd, 1);
//     DISABLE_CS();
//     return 0;
// }

static void W25Qx_WaiteBusy(void)
{
    while (W25Qx_ReadSR(1) & (1 << 7))
        ;
}

uint32_t W25Qx_Init(void)
{
    GPIO_InitTypeDef spi1_config = {
        .pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_3,
        .mode = GPIO_MODE_AF_OUT,
        .otype = GPIO_OTYPE_PP,
        .speed = GPIO_SPEED_FREQ_MEDIUM,
        .af = GPIO_AF_5,
        .pupd = GPIO_PUPD_NONE,
    };
    GPIO_InitTypeDef fls_cs = {
        .pin = GPIO_Pin_0,
        .mode = GPIO_MODE_OUT,
        .otype = GPIO_OTYPE_PP,
        .speed = GPIO_SPEED_FREQ_MEDIUM,
        .af = GPIO_AF_0,
        .pupd = GPIO_PUPD_PULL_UP,
    };
    GPIO_Init(GPIOB, &spi1_config);
    GPIO_Init(GPIOB, &fls_cs);

    SPI_Init(SPI1);
    return 0;
}

uint8_t W25Qx_ReadSR(uint32_t regNum)
{
    uint8_t cmd = 0;
    switch (regNum)
    {
    case 1:
        cmd = W25X_ReadStatusReg1;
        break;
    case 2:
        cmd = W25X_ReadStatusReg2;
        break;
    case 3:
        cmd = W25X_ReadStatusReg3;
        break;
    default:
        return 1;
        break;
    };
    ENABLE_CS();
    SPI_Transmit(SPI1, &cmd, 1);
    SPI_Receive(SPI1, &cmd, 1);
    DISABLE_CS();
    return cmd;
}

uint8_t W25Qx_ReadID()
{
    uint8_t ucCmd[4] = {W25X_DeviceID};
    ENABLE_CS();
    SPI_Transmit(SPI1, ucCmd, 1);
    SPI_Receive(SPI1, ucCmd, 4);
    DISABLE_CS();
    return ucCmd[3];
}

uint32_t W25Qx_Read(uint32_t addr, uint8_t *pBuffer, uint32_t Size)
{
    uint8_t ucCmd[4] = {W25X_ReadData, (uint8_t)(addr >> 16), (uint8_t)(addr >> 8), (uint8_t)(addr)};
    ENABLE_CS();
    SPI_Transmit(SPI1, ucCmd, 4);
    SPI_Receive(SPI1, pBuffer, Size);
    DISABLE_CS();
    return 0;
}

uint32_t W25Qx_EraseSector(uint32_t addr)
{
    W25Qx_WriteEnable();
    W25Qx_WaiteBusy();
    uint8_t ucCmd[4] = {W25X_SectorErase, (uint8_t)(addr >> 16), (uint8_t)(addr >> 8), (uint8_t)(addr)};
    ENABLE_CS();
    SPI_Transmit(SPI1, ucCmd, 4);
    DISABLE_CS();
    W25Qx_WaiteBusy();
    return 0;
}

uint32_t W25Qx_EraseBlock(uint32_t addr)
{
    W25Qx_WriteEnable();
    W25Qx_WaiteBusy();
    uint8_t ucCmd[4] = {W25X_BlockErase, (uint8_t)(addr >> 16), (uint8_t)(addr >> 8), (uint8_t)(addr)};
    ENABLE_CS();
    SPI_Transmit(SPI1, ucCmd, 4);
    DISABLE_CS();
    W25Qx_WaiteBusy();
    return 0;
}

uint32_t W25Qx_EraseChip()
{
    W25Qx_WriteEnable();
    W25Qx_WaiteBusy();
    uint8_t ucCmd[1] = {W25X_ChipErase};
    ENABLE_CS();
    SPI_Transmit(SPI1, ucCmd, 1);
    DISABLE_CS();
    W25Qx_WaiteBusy();
    return 0;
}

uint32_t W25Qx_WritePage(uint32_t addr, uint8_t *pBuffer, uint32_t Size)
{
    log_debug(__func__);
    log_debug("addr 0x%X", addr);
    log_debug("pBuffer 0x%p", pBuffer);
    log_debug("Size 0x%X", Size);

    W25Qx_WriteEnable();
    uint8_t ucCmd[4] = {W25X_PageProgram, (uint8_t)(addr >> 16), (uint8_t)(addr >> 8), (uint8_t)(addr)};
    ENABLE_CS();
    SPI_Transmit(SPI1, ucCmd, 4);
    SPI_Transmit(SPI1, pBuffer, Size);
    DISABLE_CS();
    W25Qx_WaiteBusy();
    return 0;
}

uint32_t W25Qx_WriteNoCheck(uint32_t addr, uint8_t *pBuffer, uint32_t Size)
{
    uint32_t nPageRemain = 0x100 - addr % 0x100;
    nPageRemain = nPageRemain >= Size ? Size : nPageRemain;

    while (Size > 0)
    {
        W25Qx_WritePage(addr, pBuffer, nPageRemain);
        Size -= nPageRemain;
        addr += nPageRemain;
        pBuffer += nPageRemain;
        nPageRemain = Size > 0x100 ? 0x100 : Size;
    }

    return 0;
}