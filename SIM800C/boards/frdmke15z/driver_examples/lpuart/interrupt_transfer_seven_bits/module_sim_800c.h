
#ifndef __MODULE_SIM_800C_H__
#define __MODULE_SIM_800C_H__

#include "fsl_lpuart.h"
#include "fsl_common.h"
#include "fsl_port.h"
#include "clock_config.h"

#define GSM_LPUART LPUART2
#define LPUART_CLK_FREQ CLOCK_GetFreq(kCLOCK_ScgSysOscClk)
#define SIM_SEND_COMMAND(x) LPUART_WriteBlocking(GSM_LPUART, (uint8_t*) x, sizeof(x)-1)


void getString(uint8_t* _string);
void initSim800C(void);

#endif /* __MODULE_SIM_800C_H__ */