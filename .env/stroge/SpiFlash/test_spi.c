#include <stdint.h>
#include <stdio.h>
#include "log.h"
#include "tools.h"
#include "w25qxx.h"
static uint8_t ucTxBuffer[0x1000];
static uint8_t ucRxBuffer[0x1000];

void set_buffer()
{
    for (size_t i = 0; i < 0x1000; i++)
    {
        ucTxBuffer[i] = i;
    }
}

void test_spi()
{
    // uint8_t state = 0;
    // uint8_t flashId = 0;
    set_buffer();
    W25Qx_Init();

    log_info("state1 %02X", W25Qx_ReadSR(1));
    log_info("flashId %02X", W25Qx_ReadID());

    uint32_t addr = 0x100*2;
    uint32_t len = 0x100*3;

    // W25Qx_WritePage(addr, ucTxBuffer, len);
    W25Qx_WriteNoCheck(addr, ucTxBuffer, len);
    // W25Qx_EraseSector(0);
    delay_ms(2);

    W25Qx_Read(addr, ucRxBuffer, len);
    log_hex("ucBuffer", ucRxBuffer, len);
}