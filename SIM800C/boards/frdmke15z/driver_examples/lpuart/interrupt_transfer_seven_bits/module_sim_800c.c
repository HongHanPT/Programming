#include "module_sim_800c.h"
static volatile uint32_t g_systickCounter=0;
static lpuart_handle_t g_lpuartHandle;
static uint8_t g_rxBuffer[1] = {0};
static lpuart_transfer_t receiveXfer;
static uint8_t ringBuffer[70] = "";

void SysTick_Handler(void)
{
    if (g_systickCounter != 0xFFFFFFFF)
    {
        g_systickCounter++;
    }
    else g_systickCounter = 0;
}

static void LPUART_UserCallback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData)
{
    userData = userData;
    static uint8_t stringIndex=0;
    if (kStatus_LPUART_TxIdle == status)
    {

    }

    if (kStatus_LPUART_RxIdle== status)
    {
        if(g_systickCounter>50)
        {
                  stringIndex=0;
                  for (uint8_t i= 0; i<70; i++)
                  {
                       ringBuffer[i] = NULL;
                  }
        }
        ringBuffer[stringIndex++] = *receiveXfer.data;
        g_systickCounter = 0;
        LPUART_TransferReceiveNonBlocking(GSM_LPUART, &g_lpuartHandle, &receiveXfer, NULL);

    }
}

static void initPins(void)
{    
    CLOCK_EnableClock(kCLOCK_PortD);
    
    CLOCK_EnableClock(kCLOCK_PortE);
    
    /* PORTD17 (pin 20) is configured as LPUART0_RX */
    PORT_SetPinMux(PORTD, 17U, kPORT_MuxAlt3);

    /* PORTE12 (pin 19) is configured as LPUART0_TX */
    PORT_SetPinMux(PORTE, 12U, kPORT_MuxAlt3);    
}

void initSim800C()
{
	initPins();
	//BOARD_BootClockRUN();
	CLOCK_SetIpSrc(kCLOCK_Lpuart2, kCLOCK_IpSrcSysOscAsync);
	
	lpuart_config_t config;    /*
     * config.baudRate_Bps = 115200U;
     * config.parityMode = kLPUART_ParityDisabled;
     * config.stopBitCount = kLPUART_OneStopBit;
     * config.txFifoWatermark = 0;
     * config.rxFifoWatermark = 0;
     * config.enableTx = false;
     * config.enableRx = false;
     */
    LPUART_GetDefaultConfig(&config);
    config.enableTx      = true;
    config.enableRx      = true;
    config.isMsb         = false;    
	LPUART_Init(GSM_LPUART, &config, LPUART_CLK_FREQ);    
	LPUART_TransferCreateHandle(GSM_LPUART, &g_lpuartHandle, LPUART_UserCallback, NULL);
	
	receiveXfer.data     = g_rxBuffer;
    receiveXfer.dataSize = 1;
	LPUART_TransferReceiveNonBlocking(GSM_LPUART, &g_lpuartHandle, &receiveXfer, NULL);
}

void getString(uint8_t* _string)
{
  for (uint8_t i= 0; i<70; i++)
  {
    _string[i] = ringBuffer[i];
  }
}