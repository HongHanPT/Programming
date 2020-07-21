#include "e2prom.h"

static void E2PROM_InitPins(void)
{
    /* Clock Control: 0x01u */
    CLOCK_EnableClock(kCLOCK_PortD);

    const port_pin_config_t portd8_config = {/* Internal pull-up resistor is enabled */
                                                   kPORT_PullUp,
                                                   /* Passive filter is disabled */
                                                   kPORT_PassiveFilterDisable,
                                                   /* Low drive strength is configured */
                                                   kPORT_LowDriveStrength,
                                                   /* Pin is configured as LPI2C1_SDA */
                                                   kPORT_MuxAlt2,
                                                   /* Pin Control Register fields [15:0] are not locked */
                                                   kPORT_UnlockRegister};
    /* PORTA2 (pin 73) is configured as LPI2C0_SDA */
    PORT_SetPinConfig(PORTD, 8U, &portd8_config);

    const port_pin_config_t portd9_config = {/* Internal pull-up resistor is enabled */
                                                   kPORT_PullUp,
                                                   /* Passive filter is disabled */
                                                   kPORT_PassiveFilterDisable,
                                                   /* Low drive strength is configured */
                                                   kPORT_LowDriveStrength,
                                                   /* Pin is configured as LPI2C1_SCL */
                                                   kPORT_MuxAlt2,
                                                   /* Pin Control Register fields [15:0] are not locked */
                                                   kPORT_UnlockRegister};
    /* PORTD9 (pin 72) is configured as LPI2C0_SCL */
    PORT_SetPinConfig(PORTD, 9U, &portd9_config);
}

static void E2PROM_ConfigI2C(void)
{
    lpi2c_master_config_t masterConfig; 
    CLOCK_SetIpSrc(kCLOCK_Lpi2c1, kCLOCK_IpSrcFircAsync);  //Chua biet lam gi

    /*
     * masterConfig.debugEnable = false;
     * masterConfig.ignoreAck = false;
     * masterConfig.pinConfig = kLPI2C_2PinOpenDrain;
     * masterConfig.baudRate_Hz = 100000U;
     * masterConfig.busIdleTimeout_ns = 0;
     * masterConfig.pinLowTimeout_ns = 0;
     * masterConfig.sdaGlitchFilterWidth_ns = 0;
     * masterConfig.sclGlitchFilterWidth_ns = 0;
     */
    LPI2C_MasterGetDefaultConfig(&masterConfig);        //Khai bao mac dinh cua I2C, co the thay doi thong so, nhu ben duoi

    masterConfig.baudRate_Hz = I2C_BAUDRATE1;            // Thay doi toc do BAURATE

    LPI2C_MasterInit(I2C_BASEADDR, &masterConfig, LPI2C_CLOCK_FREQUENCY);   //Khoi tao I2C
}

void E2PROM_Init(void)
{
	E2PROM_InitPins();
	E2PROM_ConfigI2C();
}

bool E2PROM_WriteE2prom0Reg(uint8_t regHighAddr, uint8_t regLowAddr, uint8_t value)
{
    if (kStatus_Success == LPI2C_MasterStart(I2C_BASEADDR, E2PROM0_ADDR, kLPI2C_Write))
    {
        /* subAddress = 0x01, data = g_master_txBuff - write to slave.
          start + slaveaddress(w) + regHigh_Address + regLowAddress + length of data buffer + data buffer + stop*/
        LPI2C_MasterSend(I2C_BASEADDR, &regHighAddr, 1);
        
        LPI2C_MasterSend(I2C_BASEADDR, &regLowAddr, 1);
        
        uint8_t g_master_txBuff[1];
        g_master_txBuff[0] = value;
        LPI2C_MasterSend(I2C_BASEADDR, g_master_txBuff, 1);

        LPI2C_MasterStop(I2C_BASEADDR);
    }
    return true;
}
bool E2PROM_ReadE2prom0Regs(uint8_t regHighAddr, uint8_t regLowAddr, uint8_t *rxBuff, uint32_t rxSize)
{
    if (kStatus_Success == LPI2C_MasterStart(I2C_BASEADDR, E2PROM0_ADDR, kLPI2C_Write))
    {

        LPI2C_MasterSend(I2C_BASEADDR, &regHighAddr, 1);
        
        LPI2C_MasterSend(I2C_BASEADDR, &regLowAddr, 1);

        LPI2C_MasterRepeatedStart(I2C_BASEADDR, E2PROM0_ADDR, kLPI2C_Read);

        LPI2C_MasterReceive(I2C_BASEADDR, rxBuff, rxSize);

        LPI2C_MasterStop(I2C_BASEADDR);
    }
    else return false;
    return true;
}

bool E2PROM_WriteE2prom1Reg(uint8_t regHighAddr, uint8_t regLowAddr, uint8_t value)
{
    if (kStatus_Success == LPI2C_MasterStart(I2C_BASEADDR, E2PROM1_ADDR, kLPI2C_Write))
    {
        /* subAddress = 0x01, data = g_master_txBuff - write to slave.
          start + slaveaddress(w) + regHigh_Address + regLowAddress + length of data buffer + data buffer + stop*/
        LPI2C_MasterSend(I2C_BASEADDR, &regHighAddr, 1);
        
        LPI2C_MasterSend(I2C_BASEADDR, &regLowAddr, 1);
        
        uint8_t g_master_txBuff[1];
        g_master_txBuff[0] = value;
        LPI2C_MasterSend(I2C_BASEADDR, g_master_txBuff, 1);

        LPI2C_MasterStop(I2C_BASEADDR);
    }
    return true;
}
bool E2PROM_ReadE2prom1Regs(uint8_t regHighAddr, uint8_t regLowAddr, uint8_t *rxBuff, uint32_t rxSize)
{
    if (kStatus_Success == LPI2C_MasterStart(I2C_BASEADDR, E2PROM1_ADDR, kLPI2C_Write))
    {

        LPI2C_MasterSend(I2C_BASEADDR, &regHighAddr, 1);
        
        LPI2C_MasterSend(I2C_BASEADDR, &regLowAddr, 1);

        LPI2C_MasterRepeatedStart(I2C_BASEADDR, E2PROM1_ADDR, kLPI2C_Read);

        LPI2C_MasterReceive(I2C_BASEADDR, rxBuff, rxSize);

        LPI2C_MasterStop(I2C_BASEADDR);
    }
    else return false;
    return true;
}