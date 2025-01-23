#include "stdint.h"
#include "openocd_tool.h"

#define HEX__(n) 0x##n##LU

#define B8__(x) \
    ((((x) & 0x0000000FLU) ? (1 << 0) : 0) + (((x) & 0x000000F0LU) ? (1 << 1) : 0) + (((x) & 0x00000F00LU) ? (1 << 2) : 0) + (((x) & 0x0000F000LU) ? (1 << 3) : 0) + (((x) & 0x000F0000LU) ? (1 << 4) : 0) + (((x) & 0x00F00000LU) ? (1 << 5) : 0) + (((x) & 0x0F000000LU) ? (1 << 6) : 0) + (((x) & 0xF0000000LU) ? (1 << 7) : 0))
#define B8(bits, count) {((uint8_t)B8__(HEX__(bits))), (count)}
static const struct tms_sequences short_tms_seqs[6][6] = {
    /* [from_state_ndx][to_state_ndx] */
    /* this is the table submitted by Jeff Williams on 3/30/2009 with this comment:

    OK, I added Peter's version of the state table, and it works OK for
    me on MC1322x. I've recreated the jlink portion of patch with this
    new state table. His changes to my state table are pretty minor in
    terms of total transitions, but Peter feels that his version fixes
    some long-standing problems.
    Jeff

    I added the bit count into the table, reduced RESET column to 7 bits from 8.
    Dick

    state specific comments:
    ------------------------
    *->RESET		tried the 5 bit reset and it gave me problems, 7 bits seems to
                    work better on ARM9 with ft2232 driver.  (Dick)

    RESET->DRSHIFT add 1 extra clock cycles in the RESET state before advancing.
                    needed on ARM9 with ft2232 driver.  (Dick)
                    (For a total of *THREE* extra clocks in RESET; NOP.)

    RESET->IRSHIFT add 1 extra clock cycles in the RESET state before advancing.
                    needed on ARM9 with ft2232 driver.  (Dick)
                    (For a total of *TWO* extra clocks in RESET; NOP.)

    RESET->*		always adds one or more clocks in the target state,
                    which should be NOPS; except shift states which (as
                    noted above) add those clocks in RESET.

    The X-to-X transitions always add clocks; from *SHIFT, they go
    via IDLE and thus *DO HAVE SIDE EFFECTS* (capture and update).
*/

    /* to state: */
    /*	RESET		IDLE			DRSHIFT			DRPAUSE			IRSHIFT			IRPAUSE */        /* from state: */
    {B8(1111111, 7), B8(0000000, 7), B8(0010111, 7), B8(0001010, 7), B8(0011011, 7), B8(0010110, 7)}, /* RESET */
    {B8(1111111, 7), B8(0000000, 7), B8(001, 3), B8(0101, 4), B8(0011, 4), B8(01011, 5)},             /* IDLE */
    {B8(1111111, 7), B8(011, 3), B8(00111, 5), B8(01, 2), B8(001111, 6), B8(0101111, 7)},             /* DRSHIFT */
    {B8(1111111, 7), B8(011, 3), B8(01, 2), B8(0, 1), B8(001111, 6), B8(0101111, 7)},                 /* DRPAUSE */
    {B8(1111111, 7), B8(011, 3), B8(00111, 5), B8(010111, 6), B8(001111, 6), B8(01, 2)},              /* IRSHIFT */
    {B8(1111111, 7), B8(011, 3), B8(00111, 5), B8(010111, 6), B8(01, 2), B8(0, 1)}                    /* IRPAUSE */
};
typedef const struct tms_sequences tms_table[6][6];

static tms_table *tms_seqs = &short_tms_seqs;
int tap_move_ndx(tap_state_t astate)
{
    /* given a stable state, return the index into the tms_seqs[]
     * array within tap_get_tms_path()
     */

    int ndx = 0;

    switch (astate)
    {
    case TAP_RESET:
        ndx = 0;
        break;
    case TAP_IDLE:
        ndx = 1;
        break;
    case TAP_DRSHIFT:
        ndx = 2;
        break;
    case TAP_DRPAUSE:
        ndx = 3;
        break;
    case TAP_IRSHIFT:
        ndx = 4;
        break;
    case TAP_IRPAUSE:
        ndx = 5;
        break;
    default:
        // LOG_ERROR("FATAL: unstable state \"%s\" in tap_move_ndx()",
        // 		tap_state_name(astate));
        // exit(1);
        break;
    }

    return ndx;
}
int tap_get_tms_path(tap_state_t from, tap_state_t to)
{
    return (*tms_seqs)[tap_move_ndx(from)][tap_move_ndx(to)].bits;
}

int tap_get_tms_path_len(tap_state_t from, tap_state_t to)
{
    return (*tms_seqs)[tap_move_ndx(from)][tap_move_ndx(to)].bit_count;
}
