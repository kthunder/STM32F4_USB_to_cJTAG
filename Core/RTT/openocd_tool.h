#ifndef OPENOCD_TOOL_H
#define OPENOCD_TOOL_H

#include "stdint.h"

typedef enum tap_state
{
    TAP_INVALID = -1,

    /* Proper ARM recommended numbers */
    TAP_DREXIT2 = 0x0,
    TAP_DREXIT1 = 0x1,
    TAP_DRSHIFT = 0x2,
    TAP_DRPAUSE = 0x3,
    TAP_IRSELECT = 0x4,
    TAP_DRUPDATE = 0x5,
    TAP_DRCAPTURE = 0x6,
    TAP_DRSELECT = 0x7,
    TAP_IREXIT2 = 0x8,
    TAP_IREXIT1 = 0x9,
    TAP_IRSHIFT = 0xa,
    TAP_IRPAUSE = 0xb,
    TAP_IDLE = 0xc,
    TAP_IRUPDATE = 0xd,
    TAP_IRCAPTURE = 0xe,
    TAP_RESET = 0x0f,
} tap_state_t;

struct tms_sequences
{
    uint8_t bits;
    uint8_t bit_count;
};

int tap_get_tms_path(tap_state_t from, tap_state_t to);

int tap_get_tms_path_len(tap_state_t from, tap_state_t to);

#endif