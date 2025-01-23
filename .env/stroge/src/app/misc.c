#include <stdint.h>
#include <stdio.h>

#include "log.h"
#include "tools.h"

#include "hal_flash.h"
#include "hal_gpio.h"
#include "hal_rcc.h"
#include "hal_spi.h"
#include "hal_usart.h"
#include "stm32f4xx.h"

#include "arm_etm.h"
#include "core_cm4.h"

/* private func*/
void LedInit()
{
    GPIO_InitTypeDef led_config = {
        .pin = GPIO_Pin_13,
        .mode = GPIO_MODE_OUT,
        .otype = GPIO_OTYPE_PP,
        .speed = GPIO_SPEED_FREQ_HIGH,
        .pupd = GPIO_PUPD_NONE,
        .af = GPIO_AF_0,
    };
    GPIO_Init(GPIOC, &led_config);
}
void McoInit()
{
    GPIO_InitTypeDef config = {
        .pin = GPIO_Pin_8,
        .mode = GPIO_MODE_AF_OUT,
        .otype = GPIO_OTYPE_PP,
        .speed = GPIO_SPEED_FREQ_HIGH,
        .pupd = GPIO_PUPD_NONE,
        .af = GPIO_AF_0,
    };
    GPIO_Init(GPIOA, &config);
}
void UartInit()
{
    GPIO_InitTypeDef usart1_config = {
        .pin = GPIO_Pin_9,
        .mode = GPIO_MODE_AF_OUT,
        .otype = GPIO_OTYPE_PP,
        .speed = GPIO_SPEED_FREQ_HIGH,
        .pupd = GPIO_PUPD_PULL_UP,
        .af = GPIO_AF_7,
    };
    GPIO_Init(GPIOA, &usart1_config);
    USART_Init(USART1);
}
void ItmInit()
{
    GPIO_InitTypeDef itm_config = {
        .pin = GPIO_Pin_3,
        .mode = GPIO_MODE_AF_OUT,
        .otype = GPIO_OTYPE_PP,
        .speed = GPIO_SPEED_FREQ_HIGH,
        .pupd = GPIO_PUPD_PULL_UP,
        // .af = GPIO_AF_7,
    };
    GPIO_Init(GPIOB, &itm_config);
}

void configure_tracing()
{
    /* STM32 specific configuration to enable the TRACESWO IO pin */
    // RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    // AFIO->MAPR |= (2 << 24); // Disable JTAG to release TRACESWO
    // DBGMCU->CR |= DBGMCU_CR_TRACE_IOEN; // Enable IO trace pins

    // if (!(DBGMCU->CR & DBGMCU_CR_TRACE_IOEN))
    // {
    //     // Some (all?) STM32s don't allow writes to DBGMCU register until
    //     // C_DEBUGEN in CoreDebug->DHCSR is set. This cannot be set by the
    //     // CPU itself, so in practice you need to connect to the CPU with
    //     // a debugger once before resetting it.
    //     return;
    // }

    /* Configure Trace Port Interface Unit */
    // CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Enable access to registers
    // TPI->ACPR = 0; // Trace clock = HCLK/(x+1) = 8MHz = UART 's baudrate
    // The HCLK of F105 is 8MHz so x is 0, and the F103 is 72MHz so x is 8
    // TPI->SPPR = 2; // Pin protocol = NRZ/USART
    TPI->FFCR = 0x102; // TPIU packet framing enabled when bit 2 is set.
                       // You can use 0x100 if you only need DWT/ITM and not ETM.
    // DBGMCU->CR |= DBGMCU_CR_TRACE_IOEN;
    /* Configure PC sampling and exception trace  */
    DWT->CTRL = (1 << DWT_CTRL_CYCTAP_Pos)       // Prescaler for PC sampling/
                                                 // 0 = x64, 1 = x1024
                | (0xa << DWT_CTRL_POSTPRESET_Pos) // Postscaler for PC sampling
                                                 // Divider = value + 1
                // | (1 << DWT_CTRL_PCSAMPLENA_Pos) // Enable PC sampling
                // | (3 << DWT_CTRL_SYNCTAP_Pos)    // Sync packet interval
                                                 // 0 = Off, 1 = Every 2^23 cycles,
                                                 // 2 = Every 2^25, 3 = Every 2^27
                // | (1 << DWT_CTRL_EXCTRCENA_Pos)  // Enable exception trace 
                | (1 << DWT_CTRL_CYCCNTENA_Pos); // Enable cycle counter

    /* Configure instrumentation trace macroblock */
    ITM->LAR = 0xC5ACCE55;
    ITM->TCR = (0x66 << ITM_TCR_TraceBusID_Pos) // Trace bus ID for TPIU
    //  | (1 << ITM_TCR_DWTENA_Pos) // Enable events from DWT
    //  | (1 << ITM_TCR_SYNCENA_Pos) // Enable sync packets
     | (1 << ITM_TCR_ITMENA_Pos); // Main enable for ITM
    // ITM->TER = 0x00000001; // Enable all stimulus ports
    // ITM->TPR = 0x00000001;

    /* Configure embedded trace macroblock */
    ETM->LAR = 0xC5ACCE55;
    ETM_SetupMode();
    ETM->CR = ETM_CR_ETMEN // Enable ETM output port
            | ETM_CR_STALL_PROCESSOR // Stall processor when fifo is full
            | ETM_CR_BRANCH_OUTPUT; // Report all branches
         // | ETM_CR_PORTIZE_8BIT;  // Add this code in F103 to set port_size 21, 6, 5, 4 as 0, 0, 0, 1 for 8Bit.
    ETM->TRACEIDR = 2; // Trace bus ID for TPIU
    ETM->TECR1 = ETM_TECR1_EXCLUDE; // Trace always enabled
    ETM->FFRR = ETM_FFRR_EXCLUDE; // Stalling always enabled
    ETM->FFLR = 24; // Stall when less than N bytes free in FIFO (range 1..24)
                    // Larger values mean less latency in trace, but more stalls.
    ETM->TRIGGER = 0x0000406F; // Add this code in F103 to define the trigger event
    ETM->TEEVR = 0x0000006F;   // Add this code in F103 to  define an event to start/stop
    // Note: we do not enable ETM trace yet, only for specific parts of code.
    ETM_TraceMode(); //en
}

void configure_watchpoint()
{
    /* This is an example of how to configure DWT to monitor a watchpoint.
       The data value is reported when the watchpoint is hit. */

    /* Monitor all accesses to GPIOC (range length 32 bytes) */
    DWT->COMP0 = (uint32_t)GPIOC;
    DWT->MASK0 = 5;
    DWT->FUNCTION0 = (2 << DWT_FUNCTION_FUNCTION_Pos) // Report data and addr on watchpoint hit
                     | (1 << DWT_FUNCTION_EMITRANGE_Pos);

    /* Monitor all accesses to globalCounter (range length 4 bytes) */
    // DWT->COMP1 = (uint32_t)&globalCounter;
    DWT->MASK1 = 2;
    DWT->FUNCTION1 = (3 << DWT_FUNCTION_FUNCTION_Pos); // Report data and PC on watchpoint hit
}

void misc_init()
{
    // uint32_t(*ROM_TABLE)[12] = (uint32_t (*)[12])0xE00FF000;
    // configure_tracing();
    // configure_watchpoint();
    LedInit();
    // McoInit();
    UartInit();
    // ItmInit();
    // test_spi();
}