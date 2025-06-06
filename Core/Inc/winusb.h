#ifndef WINUSB_H
#define WINUSB_H

#include <stdint.h>

void winusb_init(uint8_t busid, uintptr_t reg_base);

void usbd_winusb_write(uint8_t * buffer, uint32_t len);

#endif