#include "tools.h"
#include "stm32f4xx.h"
#include "hal_def.h"
#include <stdint.h>
#include <stdio.h>
#include "log.h"

// STM IRQHandler def

void HardFault_Handler()
{
    log_info(__func__);
    while (1)
    {
        ;
    }
}

void SVC_Handler_Fn(uint32_t *pwdSF)
{
    uint32_t svc_number = ((char *)pwdSF[6])[-2];
    uint32_t svc_r0 = pwdSF[0];
    uint32_t svc_r1 = pwdSF[1];
    uint32_t svc_r2 = pwdSF[2];
    uint32_t svc_r3 = pwdSF[3];
    UNUSED(svc_number);
    UNUSED(svc_r0);
    UNUSED(svc_r1);
    UNUSED(svc_r2);
    UNUSED(svc_r3);
}
