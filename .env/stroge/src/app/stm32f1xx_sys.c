#include "hal_flash.h"
#include "hal_rcc.h"

void SystemInit()
{
    FLASH_Init();
    RCC_Init();
}