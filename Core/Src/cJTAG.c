#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include "log.h"
#include "openocd_tool.h"
// #include "driver/gpio.h"
/*******************************SQE*******************************/
/*------------------------PIN OPERATION PORTING----------------------------------*/
// clang-format off
#define PIN_TCK_OUT(bit)       GPIOA->BSRR = (TCKC_Pin << (bit?0:16))
#define PIN_TMS_OUT(bit)       GPIOA->BSRR = (TMSC_Pin << (bit?0:16))
#define PIN_TDI_OUT(bit)       PIN_TMS_OUT(bit)
#define PIN_TDO_IN()           ((GPIOA->IDR & TMSC_Pin)>0)
#define PIN_TMS_INPUT_ENABLE()  GPIOA->MODER &= ~GPIO_MODER_MODE14_0
#define PIN_TMS_INPUT_DISABLE() GPIOA->MODER |= GPIO_MODER_MODER14_0
#define PIN_TMSC_OUT_SIDE_SET(bit)  GPIOA->BSRR = (TCKC_Pin<< 0)|(TMSC_Pin << (bit?0:16))
#define PIN_TMSC_OUT_SIDE_CLR(bit)  GPIOA->BSRR = (TCKC_Pin<<16)|(TMSC_Pin << (bit?0:16))
// clang-format on
char binary_string[64] = {0};
char *to_binary_string(unsigned int value, uint32_t bits)
{
    uint32_t i = 0;
    // 从最高位到最低位逐位检查并填充字符串
    for (i = 0; i < bits; i++)
    {
        binary_string[i] = ((value >> i) & 1) ? '1' : '0';
    }
    // 添加字符串结束符
    binary_string[i] = '\0';

    return binary_string;
}
/*------------------------PIN OPERATION PORTING----------------------------------*/

#define PIN_TCK_SET() PIN_TCK_OUT(1)
#define PIN_TCK_CLR() PIN_TCK_OUT(0)
#define PIN_TMS_SET() PIN_TMS_OUT(1)
#define PIN_TMS_CLR() PIN_TMS_OUT(0)

/**
 * 仿真器在 TCK 下降沿改变 TMS、TDI 电平，被测芯片在 TCK 上升沿捕获输入并更新内部状态。
 * 被测芯片在 TCK 下降沿改变 TDO 电平，仿真器在 TCK 上升沿捕获 TDO 数据。
 */
/*******************************private func*******************************/
#define JTAG_CYCLE_TCK(tms, tdi, tdo) \
    do                                \
    {                                 \
        PIN_TDI_OUT(!tdi);            \
        PIN_TCK_SET();                \
        PIN_TCK_CLR();                \
        PIN_TMS_OUT(tms);             \
        PIN_TCK_SET();                \
        PIN_TMS_INPUT_ENABLE();       \
        PIN_TCK_CLR();                \
        PIN_TCK_SET();                \
        tdo = PIN_TDO_IN();           \
        PIN_TCK_CLR();                \
    } while (0);

#define JTAG_CYCLE_TCK_FAST(tms, tdi, tdo) \
    do                                     \
    {                                      \
        PIN_TMSC_OUT_SIDE_CLR(!tdi);       \
        PIN_TCK_SET();                     \
        PIN_TMSC_OUT_SIDE_CLR(tms);        \
        PIN_TCK_SET();                     \
        PIN_TMS_INPUT_ENABLE();            \
        PIN_TCK_CLR();                     \
        PIN_TCK_SET();                     \
        tdo = PIN_TDO_IN();                \
        PIN_TCK_CLR();                     \
        PIN_TMS_INPUT_DISABLE();           \
    } while (0);

#define JTAG_CYCLE_TMS(tms) \
    do                      \
    {                       \
        PIN_TMS_OUT(tms);   \
        PIN_TCK_SET();      \
        PIN_TCK_CLR();      \
    } while (0);

static void JTAG_sequence_escape(uint32_t n)
{
    bool tms = 1;
    PIN_TCK_SET();
    for (size_t i = 0; i < n; i++)
    {
        PIN_TMS_OUT(tms);
        tms = !tms;
    }
    PIN_TMS_OUT(1);
    PIN_TCK_CLR();
}

static void JTAG_sequence_tms(uint32_t val, uint32_t bit_len)
{
    for (size_t i = 0; i < bit_len; i++)
    {
        bool tms = val >> i & 1;
        JTAG_CYCLE_TMS(tms);
    }
}
/*******************************interface*******************************/
void cJTAG_sequence(uint8_t *ucTMS, uint8_t *ucTDI, uint8_t *ucTDO, uint32_t bits)
{
    bool tms, tdi, tdo;
    uint8_t tmp = 0;
    uint32_t cycle = (bits + 7) / 8;
    uint32_t last_count = bits - (8 * (cycle - 1));
    for (size_t i = 1; i <= cycle; i++)
    {
        tmp = 0;
        for (size_t j = 0; j < (i == cycle ? last_count : 8); j++)
        {
            tms = (*ucTMS >> j) & 1;
            tdi = (*ucTDI >> j) & 1;
            JTAG_CYCLE_TCK_FAST(tms, tdi, tdo);
            // printf("%d ", tdo);
            tmp |= tdo << j;
        }
        ucTMS++;
        ucTDI++;
        *ucTDO++ = tmp;
    }
    // printf("\n");
}

void cJTAG_operation_ir_scan(uint8_t *ir_w, uint8_t *ir_r, uint32_t bits)
{
    if (ir_w == NULL || bits == 0 || bits > 8)
    {
        return;
    }

    uint32_t TMS = 0;
    uint32_t TDI = 0;
    uint32_t TDO = 0;

    TMS = 0x3;
    cJTAG_sequence((uint8_t *)&TMS, (uint8_t *)&TDI, (uint8_t *)&TDO, 4); // to Shift-IR

    TMS = 0;
    ARRAY_SET_BIT(&TMS, bits - 1, 1);
    cJTAG_sequence((uint8_t *)&TMS, ir_w, ir_r, bits); // the last bit scan in exit1-IR

    TMS = 0x1;
    cJTAG_sequence((uint8_t *)&TMS, (uint8_t *)&TDI, (uint8_t *)&TDO, 2); // to Run-Test/Idle
}

void cJTAG_operation_dr_scan(uint8_t *dr_w, uint8_t *dr_r, uint32_t bits)
{
    if (dr_w == NULL || bits == 0)
    {
        return;
    }

    uint8_t ucTMS[0x100] = {0};
    uint32_t TMS = 0;
    uint32_t TDI = 0;
    uint32_t TDO = 0;

    TMS = 0x0;
    cJTAG_sequence((uint8_t *)&TMS, (uint8_t *)&TDI, (uint8_t *)&TDO, 8); // waite busy clk in idle

    TMS = 0x1;
    cJTAG_sequence((uint8_t *)&TMS, (uint8_t *)&TDI, (uint8_t *)&TDO, 3); // to Shift-DR

    ARRAY_SET_BIT(ucTMS, bits - 1, 1);
    cJTAG_sequence((uint8_t *)&ucTMS, dr_w, dr_r, bits); // the last bit scan in exit1-DR

    TMS = 0x1;
    cJTAG_sequence((uint8_t *)&TMS, (uint8_t *)&TDI, (uint8_t *)&TDO, 2); // to Run-Test/Idle
}

void cJtag_active()
{
    JTAG_sequence_escape(10);
    JTAG_sequence_tms(0xFFFF, 24);
    JTAG_sequence_escape(7);
    JTAG_sequence_tms(0x08C, 12);
}

int test(void)
{
    uint32_t ir_w = 0x01;
    uint32_t dr_w = 0;

    uint32_t ir_r = 0;
    uint32_t dr_r = 0;

    cJTAG_operation_ir_scan((uint8_t *)&ir_w, (uint8_t *)&ir_r, 5);
    log_info("IR_r : 0x%02X %s", ir_r, to_binary_string(ir_r, 5));
    cJTAG_operation_dr_scan((uint8_t *)&dr_w, (uint8_t *)&dr_r, 32);
    log_info("DR_r : 0x%08X %s", dr_r, to_binary_string(dr_r, 32));

    return 1;
}