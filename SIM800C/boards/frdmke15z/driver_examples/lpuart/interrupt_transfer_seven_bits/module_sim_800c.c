#include "module_sim_800c.h"
static volatile uint32_t g_systickCounter=0;
static volatile uint32_t g_systickCounterString=0;
static lpuart_handle_t g_lpuartHandle;
static uint8_t g_rxBuffer[1] = {0};
static lpuart_transfer_t receiveXfer;
static uint8_t ringBuffer[256] = "";
static sim_t *g_pSim;

void SysTick_Handler(void)
{
    if (g_systickCounter != 0xFFFFFFFF)
    {
        g_systickCounter++;
        g_systickCounterString++;
    }
    else 
    {
      g_systickCounter = 0;
      g_systickCounterString=0;
    }
}

void delay(unsigned int time)
{
    volatile uint32_t i = 0;
    for (unsigned int cnt = 0; cnt<=time;cnt++)
    {
      for (i = 0; i < 100; ++i)
      {
          __asm("NOP"); /* delay */
      }
    }
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
        if(g_systickCounter>40)
        {
                  stringIndex=0;
                  for (uint8_t i= 0; i <100; i++)
                  {
                       ringBuffer[i] = NULL;
                  }
        }

        ringBuffer[stringIndex++] = *receiveXfer.data;
        if(stringIndex+1==256) stringIndex=0;     // 29/07
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

void initSim800C(sim_t *_g_pSim)
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
    g_pSim = _g_pSim;
}

void getString(uint8_t* _string)
{
  g_systickCounterString=0;
  while(g_systickCounterString<200);

  for (uint8_t i= 0; i<100; i++)
  {
    _string[i] = ringBuffer[i];
  }

}

////////////////////////////////////////
/* State Machine */
void processInit()
{
    switch(g_pSim -> stateInit)
    {   
    
    case IDLE_INIT_PRROCESS:   break;
    case CHECK_SIM: 
          
          SIM_SEND_COMMAND("AT+CPIN=?\r\n");
         // delay(2000);
          getString(g_pSim -> respone);
          if(findString("ERROR",g_pSim -> respone)== ISFINDED)
          {
              g_pSim -> isCheckedSim = true; 
          }
          else
          {
              g_pSim -> isCheckedSim = 1;
          if(g_pSim -> fnSimCallBack!= NULL)
          {
              g_pSim -> fnSimCallBack(g_pSim -> state, g_pSim -> stateInit);
          }
          }
          break;
    case READ_MODULE: 
          if(g_pSim -> isCheckedSim == 1)
          {
              if(g_pSim -> fnSimCallBack!= NULL)
              {
                  g_pSim -> fnSimCallBack(g_pSim -> state, g_pSim -> stateInit);
              }
              g_pSim ->stateInit = IDLE_INIT_PRROCESS;
          }
          else
              g_pSim ->stateInit = CHECK_SIM;
          break;
    case READ_SIM:
          if(g_pSim -> isCheckedSim == 1)
          {
              if(g_pSim -> fnSimCallBack!= NULL)
              {
                  g_pSim -> fnSimCallBack(g_pSim -> state, g_pSim -> stateInit);
              }
              g_pSim ->stateInit = IDLE_INIT_PRROCESS;
          }
          else
              g_pSim -> stateInit = CHECK_SIM;
          break;
    case SET_MIC_GAIN: 
		  if(g_pSim -> isCheckedSim == 1)
		  {
			  if(g_pSim -> fnSimCallBack!= NULL)
			  {
				  g_pSim -> fnSimCallBack(g_pSim -> state, g_pSim -> stateInit);
			  } 
			  g_pSim -> state = IDLE;
		  }
		  else
			  g_pSim -> state = INIT;
		  break;
    }
}

void processSIM()
{
    switch (g_pSim ->state)
    {
    case IDLE: break;
    case RESET: 
          //g_systickCounter2 =0;
          if(g_pSim -> fnSimCallBack!= NULL)
              {
                  g_pSim -> fnSimCallBack(g_pSim -> state, g_pSim -> stateInit);
              } 
              g_pSim -> state = IDLE;       
          break;
    case INIT:  
           processInit();
          break;

    case GET_CSQ: 
          if(g_pSim -> isCheckedSim == 1)
          {
              if(g_pSim -> fnSimCallBack!= NULL)
              {
                  g_pSim -> fnSimCallBack(g_pSim -> state, g_pSim -> stateInit);
              }
          }
          else
              g_pSim -> state = INIT;
          break;
    }
}