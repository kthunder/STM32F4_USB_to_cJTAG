#include <stdint.h>
#include <string.h>

#include "stm32f4xx.h"
#include "hal_def.h"
#include "hal_flash.h"

#include "log.h"
#include "tools.h"

#define RDPRTKEY 0x000000A5U
#define KEY1 0x45670123U
#define KEY2 0xCDEF89ABU
#define OPTKEY1 0x08192A3BU
#define OPTKEY2 0x4C5D6E7FU

#define FLASH_SIZE (FLASH_END - FLASH_BASE + 1)

static const uint32_t sector_addr[] = {
    0x08000000, // 16K
    0x08004000, // 16K
    0x08008000, // 16K
    0x0800C000, // 16K
    0x08010000, // 64K
    0x08020000, // 128K
    0x08040000, // 128K
    0x08060000, // 128K
    0x08080000, // 128K
    // 0x080A0000, // 128K
    // 0x080C0000, // 128K
    // 0x080E0000  // 128K
};

/* private func */
static inline HAL_StatusTypeDef FLASH_Unlock_Prv()
{
    HAL_StatusTypeDef status = HAL_OK;

    if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != RESET)
    {
        WRITE_REG(FLASH->KEYR, KEY1);
        WRITE_REG(FLASH->KEYR, KEY2);
    }

    if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != RESET)
    {
        status = HAL_ERROR;
    }

    return status;
}

static inline HAL_StatusTypeDef FLASH_Lock_Prv()
{
    HAL_StatusTypeDef status = HAL_OK;

    if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != SET)
    {
        SET_BIT(FLASH->CR, FLASH_CR_LOCK);
    }

    if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != SET)
    {
        status = HAL_ERROR;
    }

    return status;
}

static inline HAL_StatusTypeDef FLASH_Waite_Idle_Prv(uint32_t Timeout)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint32_t tickstart = HAL_GetTick();

    // waite idle
    while (READ_BIT(FLASH->SR, FLASH_SR_BSY))
    {
        // when timeout do
        if (HAL_GetTick() - tickstart >= Timeout)
        {
            status = HAL_TIMEOUT;
            break;
        }
    }

    return status;
}

static inline HAL_StatusTypeDef FLASH_Validate_Addr_Prv(uint32_t addr, uint32_t len, uint32_t nAlgin)
{
    if ((addr % nAlgin != 0) || (len % nAlgin != 0))
    {
        log_error("not algin %d", nAlgin);
        log_error("addr : 0x%X", addr);
        log_error("len  : 0x%X", len);
        return HAL_ERROR;
    }

    if ((addr + len) > FLASH_SIZE)
    {
        log_error("addr out of bound => addr : 0x%X , len : %d", addr, len);
        return HAL_ERROR;
    }

    return HAL_OK;
}

static inline void FLASH_Progarm_Word_Prv(uint32_t addr, uint32_t data)
{
    log_info(__func__);
    assert_param(FLASH_Validate_Addr_Prv(addr, 4, 4) == HAL_OK);

    log_info("addr : 0x%X, data %d", addr, data);

    // start program
    SET_BIT(FLASH->CR, FLASH_CR_PG);

    // write data
    *(__IO uint32_t *)(FLASH_BASE + addr) = data;

    // write idle
    FLASH_Waite_Idle_Prv(HAL_MAX_DELAY);

    // clear STRT bit ()
    CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
}

static inline uint32_t FLASH_Get_Sector_Num(uint32_t addr)
{
    uint32_t realAddr = addr + FLASH_BASE;
    log_info(__func__);
    log_info("    addr : 0x%X, realAddr : 0x%X", addr, realAddr);
    for (size_t i = 0; i < 8; i++)
    {
        log_debug("    i : %d, min : 0x%X, max : 0x%X", i, sector_addr[i], sector_addr[i + 1]);
        if (realAddr >= sector_addr[i] && realAddr < sector_addr[i + 1])
        {
            log_info("    sector index : %d", i);
            return i;
        }
    }
    log_error("    addr out of range : %d");
    return 0xFF;
}

uint32_t FLASH_Init()
{
    // set program size
    CLEAR_BIT(FLASH->CR, FLASH_CR_PSIZE_0);
    SET_BIT(FLASH->CR, FLASH_CR_PSIZE_1);
    // enable prifetch ins
    SET_BIT(FLASH->ACR, FLASH_ACR_PRFTEN);
    // enable cache
    SET_BIT(FLASH->ACR, FLASH_ACR_ICEN);
    SET_BIT(FLASH->ACR, FLASH_ACR_DCEN);
    // set flash latency
    SET_BIT(FLASH->ACR, FLASH_ACR_LATENCY_2WS);
    return HAL_OK;
}

/* pubilc func */
uint32_t FLASH_Write(uint32_t addr, void *ptr, uint32_t len)
{
    log_info(__func__);
    assert_param(ptr != NULL);
    assert_param(FLASH_Validate_Addr_Prv(addr, len, 4) == HAL_OK);

    if (FLASH_Blank_Check(addr, len))
    {
        log_warn("FLASH_Blank_Check FAILED ADDR:0x%X LEN:%d", addr, len);
        return HAL_ERROR;
    }

    // unlock flash
    FLASH_Unlock_Prv();

    for (uint32_t i = 0; i < len; i += 4)
    {
        FLASH_Progarm_Word_Prv(addr + i, *(uint32_t *)(ptr + i));
    }

    // lock flash
    FLASH_Lock_Prv();

    return HAL_OK;
}

uint32_t FLASH_Read(uint32_t addr, void *ptr, uint32_t len)
{
    log_info(__func__);
    assert_param(ptr != NULL);
    assert_param(FLASH_Validate_Addr_Prv(addr, len, 1) == HAL_OK);

    memcpy(ptr, (const void *)(addr + FLASH_BASE), len);
    return HAL_OK;
}

uint32_t FLASH_Blank_Check(uint32_t addr, uint32_t len)
{
    log_info(__func__);
    assert_param(FLASH_Validate_Addr_Prv(addr, len, 1) == HAL_OK);

    for (uint32_t i = 0; i < len; i++)
    {
        if (*(uint8_t *)(FLASH_BASE + addr + i) != 0xFF)
            return HAL_ERROR;
    }
    return HAL_OK;
}

uint32_t FLASH_Erase(uint32_t addr)
{
    log_info(__func__);
    uint32_t nSector = FLASH_Get_Sector_Num(addr);
    assert_param(nSector != 0xFF);

    // uint32_t sectorStartAddr = sector_addr[nSector];
    log_info("FLASH_Erase sectorStartAddr:0x%X", sector_addr[nSector]);

    // unlock flash
    FLASH_Unlock_Prv();

    // write idle
    FLASH_Waite_Idle_Prv(HAL_MAX_DELAY);

    // start sector erase
    SET_BIT(FLASH->CR, FLASH_CR_SER);

    // select page
    WRITE_BIT(FLASH->CR, FLASH_CR_SNB, nSector);

    // start erase
    SET_BIT(FLASH->CR, FLASH_CR_STRT);

    // write idle
    FLASH_Waite_Idle_Prv(HAL_MAX_DELAY);

    // over page erase
    CLEAR_BIT(FLASH->CR, FLASH_CR_SER);

    // lock flash
    FLASH_Lock_Prv();

    return HAL_OK;
}

uint32_t FLASH_Unit_Test()
{
    log_info(__func__);
    assert_param(FLASH_Get_Sector_Num(0x0) == 0);
    assert_param(FLASH_Get_Sector_Num(0x4000) == 1);
    assert_param(FLASH_Get_Sector_Num(0x8000) == 2);
    assert_param(FLASH_Get_Sector_Num(0xC000) == 3);
    assert_param(FLASH_Get_Sector_Num(0x10000) == 4);
    assert_param(FLASH_Get_Sector_Num(0x20000) == 5);
    assert_param(FLASH_Get_Sector_Num(0x40000) == 6);
    assert_param(FLASH_Get_Sector_Num(0x60000) == 7);
    return 0;
}
