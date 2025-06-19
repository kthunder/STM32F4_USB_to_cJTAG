#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include "log.h"
/*******************************SQE*******************************/
/*------------------------PIN OPERATION PORTING----------------------------------*/
// clang-format off
#define PIN_TCK_OUT(bit)       TCKC_GPIO_Port->BSRR = (TCKC_Pin << (bit?0:16));__NOP();
#define PIN_TMS_OUT(bit)       TMSC_GPIO_Port->BSRR = (TMSC_Pin << (bit?0:16));__NOP();
#define PIN_TDI_OUT(bit)       PIN_TMS_OUT(bit);__NOP();
#define PIN_TDO_IN()           ((TMSC_GPIO_Port->IDR & TMSC_Pin)>0);__NOP();
#define PIN_TMS_INPUT_ENABLE()  TMSC_GPIO_Port->MODER &= ~GPIO_MODER_MODE2_0;__NOP();
#define PIN_TMS_INPUT_DISABLE() TMSC_GPIO_Port->MODER |= GPIO_MODER_MODER2_0;__NOP();
#define PIN_TMSC_OUT_SIDE_SET(bit)  TMSC_GPIO_Port->BSRR = (TCKC_Pin<< 0)|(TMSC_Pin << (bit?0:16));__NOP();
#define PIN_TMSC_OUT_SIDE_CLR(bit)  TMSC_GPIO_Port->BSRR = (TCKC_Pin<<16)|(TMSC_Pin << (bit?0:16));__NOP();
// clang-format on
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
void cJTAG_seq(uint32_t bits, uint8_t *ucTDI, uint8_t *ucTDO)
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
            tms = ((i == cycle) && (j == (last_count-1)))? 1 : 0;
            tdi = (*ucTDI >> j) & 1;
            JTAG_CYCLE_TCK_FAST(tms, tdi, tdo);
            // printf("%d ", tdo);
            tmp |= tdo << j;
        }
        ucTDI++;
        *ucTDO++ = tmp;
    }

    /*******************************log*******************************/
    // switch (bits)
    // {
    // case 5:
    //     printf("IR shift\n");
    //     printf("    out %d bits,[0x%X] %s\n", bits, *(uint32_t *)ucTDI, to_binary_string(*(uint8_t *)ucTDI, bits));
    //     printf("    in  %d bits,[0x%X] %s\n", bits, *(uint32_t *)ucTDO, to_binary_string(*(uint8_t *)ucTDO, bits));
    //     break;
    // case 32:
    //     printf("DR shift\n");
    //     printf("    out %d bits,[0x%X] %s\n", bits, *(uint32_t *)ucTDI, to_binary_string(*(uint32_t *)ucTDI, bits));
    //     printf("    in  %d bits,[0x%X] %s\n", bits, *(uint32_t *)ucTDO, to_binary_string(*(uint32_t *)ucTDO, bits));
    //     break;
    // case 44:
    //     // printf("DR shift\n");
    //     // printf("    out %d bits,[%X %X] %s\n", bits, *(uint32_t *)out_data, *(uint32_t *)(out_data + 4), to_binary_string(*(uint32_t *)out_data, bits));
    //     // printf("    in  %d bits,[%X %X] %s\n", bits, *(uint32_t *)in_data, *(uint32_t *)(in_data + 4), to_binary_string(*(uint32_t *)in_data, bits));
    //     typedef struct
    //     {
    //         uint64_t op : 2;
    //         uint64_t data : 32;
    //         uint64_t addr : 10;
    //     } DMI_TypeDef;

    //     DMI_TypeDef *d = (DMI_TypeDef *)ucTDI;
    //     printf("---- DMI SCAN ----:\n");
    //     printf("DMI shift out op : 0x%X \n", (uint32_t)d->op);
    //     printf("            data: 0x%08X\n", (uint32_t)d->data);
    //     printf("            addr: 0x%02X\n", (uint32_t)d->addr);
    //     d = (DMI_TypeDef *)ucTDO;
    //     printf("DMI shift in  op : 0x%X \n", (uint32_t)d->op);
    //     printf("            data: 0x%08X\n", (uint32_t)d->data);
    //     printf("            addr: 0x%02X\n", (uint32_t)d->addr);
    //     /* code */
    //     break;

    // default:
    //     break;
    // }
}

void cJTAG_tms(uint32_t bits, uint8_t* ucTMS)
{
    bool tms, tdo;
    for (size_t i = 0; i < bits; i++) {
        tms = (ucTMS[i/8] >> (i%8)) & 1;
        JTAG_CYCLE_TCK_FAST(tms, 1, tdo);
    }
    (void)tdo;
}

void cJTAG_operation_ir_scan(uint8_t *ir_w, uint8_t *ir_r, uint32_t bits)
{
    if (ir_w == NULL || bits == 0 || bits > 8)
    {
        return;
    }

    uint32_t TMS = 0;

    TMS = 0x3;
    cJTAG_tms(4, (uint8_t *)&TMS); // to Shift-IR

    cJTAG_seq(bits, ir_w, ir_r); // the last bit scan in exit1-IR

    TMS = 0x1;
    cJTAG_tms(2, (uint8_t *)&TMS); // to Run-Test/Idle
}

void cJTAG_operation_dr_scan(uint8_t *dr_w, uint8_t *dr_r, uint32_t bits)
{
    if (dr_w == NULL || bits == 0)
    {
        return;
    }

    uint32_t TMS = 0;

    TMS = 0x0;
    cJTAG_tms(8, (uint8_t *)&TMS); // waite busy clk in idle

    TMS = 0x1;
    cJTAG_tms(3, (uint8_t *)&TMS); // to Shift-DR

    cJTAG_seq(bits, dr_w, dr_r); // the last bit scan in exit1-DR

    TMS = 0x1;
    cJTAG_tms(2, (uint8_t *)&TMS); // to Run-Test/Idle
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
    printf("IR_r : 0x%02X %s\n", ir_r, to_binary_string(ir_r, 5));
    cJTAG_operation_dr_scan((uint8_t *)&dr_w, (uint8_t *)&dr_r, 32);
    printf("DR_r : 0x%08X %s\n", dr_r, to_binary_string(dr_r, 32));

    return 1;
}