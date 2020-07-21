#ifndef __E2PROM_H__
#define __E2PROM_H__


#include "fsl_common.h"
#include "fsl_port.h"
#include "fsl_lpi2c.h"

#define I2C_BASEADDR LPI2C1
#define LPI2C_CLOCK_FREQUENCY CLOCK_GetIpFreq(kCLOCK_Lpi2c1)
#define I2C_BAUDRATE1 100000U

#define E2PROM0_ADDR           (0x50)
#define E2PROM1_ADDR           (0x51)

void E2PROM_Init(void);
bool E2PROM_WriteE2prom0Reg(uint8_t regHighAddr, uint8_t regLowAddr, uint8_t value);
bool E2PROM_ReadE2prom0Regs(uint8_t regHighAddr, uint8_t regLowAddr, uint8_t *rxBuff, uint32_t rxSize);
bool E2PROM_WriteE2prom1Reg(uint8_t regHighAddr, uint8_t regLowAddr, uint8_t value);
bool E2PROM_ReadE2prom1Regs(uint8_t regHighAddr, uint8_t regLowAddr, uint8_t *rxBuff, uint32_t rxSize);

#endif /* __E2PROM_H__ */