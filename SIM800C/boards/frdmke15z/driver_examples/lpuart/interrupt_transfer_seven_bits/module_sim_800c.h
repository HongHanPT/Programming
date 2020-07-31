
#ifndef __MODULE_SIM_800C_H__
#define __MODULE_SIM_800C_H__

#include "fsl_lpuart.h"
#include "fsl_common.h"
#include "fsl_port.h"
#include "clock_config.h"

#include "string_uart.h"

#define GSM_LPUART LPUART2
#define LPUART_CLK_FREQ CLOCK_GetFreq(kCLOCK_ScgSysOscClk)
#define SIM_SEND_COMMAND(x) LPUART_WriteBlocking(GSM_LPUART, (uint8_t*) x, sizeof(x)-1)


typedef enum
{
    IDLE =0,
    RESET,
    INIT,
    GET_CSQ,
}state_sim_t;

typedef enum
{
    IDLE_INIT_PRROCESS = 0,
    CHECK_SIM,
    READ_MODULE,
    READ_SIM,
    SET_MIC_GAIN,
}state_init_t;

typedef struct{
    state_sim_t state;    
    state_init_t stateInit;
    bool isCheckedSim;
    uint8_t respone[256];
    uint8_t str;
    void (*fnSimCallBack)(state_sim_t _state, state_init_t _stateInit);
}sim_t;

void getString(uint8_t* _string);
void initSim800C(sim_t *_g_pSim);
void processInit(void);
void processSIM(void);

#endif /* __MODULE_SIM_800C_H__ */