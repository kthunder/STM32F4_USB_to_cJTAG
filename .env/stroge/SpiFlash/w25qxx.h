#ifndef __W25QXX_H
#define __W25QXX_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

  uint32_t W25Qx_Init(void);
  uint8_t W25Qx_ReadSR(uint32_t regNum);
  uint8_t W25Qx_ReadID(void);
  uint32_t W25Qx_Read(uint32_t addr, uint8_t *pBuffer, uint32_t Size);
  uint32_t W25Qx_WritePage(uint32_t addr, uint8_t *pBuffer, uint32_t Size);
  uint32_t W25Qx_WriteNoCheck(uint32_t addr, uint8_t *pBuffer, uint32_t Size);
  uint32_t W25Qx_EraseSector(uint32_t Address);
  uint32_t W25Qx_EraseBlock(uint32_t Address);
  uint32_t W25Qx_EraseChip(void);

#ifdef __cplusplus
}
#endif

#endif
