/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"

#include "pin_mux.h"
#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BOARD_LED_GPIO BOARD_LED_RED_GPIO
#define BOARD_LED_GPIO_PIN BOARD_LED_RED_GPIO_PIN

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief delay a while.
 */
void delay(unsigned int time);
bool check_button(void);
void scan_row(unsigned char col);
unsigned char check_row(void);
char check_key(void);
/*******************************************************************************
 * Variables
 ******************************************************************************/
char key[4][3] = {{'5','1','9'},{'7','3','#'},{'4','0','8'},{'6','2','*'}};
volatile uint32_t g_systickCounter;
/*******************************************************************************
 * Code
 ******************************************************************************/
void SysTick_Handler(void)
{
    if (g_systickCounter != 0xFFFFFFFF)
    {
        g_systickCounter++;
    }
}

void SysTick_DelayTicks(uint32_t n)
{
    g_systickCounter = n;
    while (g_systickCounter != 0U)
    {
    }
}

/**
 * @brief 
 * 
 * @param    
 * @retval
 */
void delay(unsigned int time)
{
    volatile uint32_t i = 0;
    for (unsigned int cnt = 0; cnt<=time;cnt++)
    {
      for (i = 0; i < 800; ++i)
      {
          __asm("NOP"); /* delay */
      }
    }
}


/**
 * @brief 
 * 
 * @param    
 * @retval
 */
bool check_button(void)
{
    GPIO_PinWrite(GPIOE, 8U, 0);
    GPIO_PinWrite(GPIOB, 5U, 0);
    GPIO_PinWrite(GPIOC, 3U, 0);
    if((!GPIO_PinRead(GPIOC, 2U))||(!GPIO_PinRead(GPIOD, 6U))||(!GPIO_PinRead(GPIOD, 7U))||(!GPIO_PinRead(GPIOD, 5U)))
      return 1;
    return 0;    
}


/**
 * @brief 
 * 
 * @param    
 * @retval
 */
void scan_col(unsigned char col)
{
    GPIO_PinWrite(GPIOE, 8U, 1);
    GPIO_PinWrite(GPIOB, 5U, 1);
    GPIO_PinWrite(GPIOC, 3U, 1);
    if (col ==1) 
        GPIO_PinWrite(GPIOE, 8U, 0);
     else if (col == 2) 
        GPIO_PinWrite(GPIOB, 5U, 0);
     else if (col == 3)
        GPIO_PinWrite(GPIOC, 3U, 0);
 
}


/**
 * @brief 
 * 
 * @param    
 * @retval
 */
unsigned char check_row(void)
{
    unsigned char row = 0;
    if (!GPIO_PinRead(GPIOC, 2U)) row =1;
    else if (!GPIO_PinRead(GPIOD, 6U)) row =2;
    else if (!GPIO_PinRead(GPIOD, 7U)) row =3;
    else if (!GPIO_PinRead(GPIOD, 5U)) row =4;
    return row;
}


/**
 * @brief 
 * 
 * @param    
 * @retval
 */
char check_key(void)
{
    unsigned char row;
    if (check_button())
    {
      delay(10);
      if (check_button())
      {
        for (unsigned char col = 1; col <=3;col ++)
        {
          
          scan_col(col);
          row = check_row();
          if(row) return key[row-1][col-1];
        }
      }
      else return 0;
    }
    return 0;
}
///////////////////////////////////////////////////////////////////////////////////
#define MAX_BUTTON_COUNT    10      //
#define PRESS_TICK          300	    //
#define HOLD_TICK           200	    //
#define RELEASE_TICK        300     //

#define CURRENT_TICK 10
#define BUTTON_LOGIC_LEVEL 1 //Gia tri doc GPIO

#define SYSTEMCALLBACK void (*fnCallback)(void);

typedef enum tagSTATE
{
    IDLE = 0,
    PRESSING,
    HOLDING,
    RELEASING,
    HOLDED,
}state;

void fnCallbackIDLE(void);
typedef struct tagButton
{
    unsigned int 	nTick;			// Current tick of button.
    char   	keyButton;		// 
//    unsigned char   	last_keyButton;
    bool  	enable;			        //
    char	state;			        //
    void (*fnCallbackIDLE)(void);               // 
    void (*fnCallbackPRESS)(void);
    void (*fnCallbackHOLD)(void);
    void (*fnCallbackRELEASE)(void);


}BUTTON;

void ButtonProcessEvent(BUTTON *g_pButtons, char currentKey)
{
   // char nIndex;
                switch(g_pButtons->state)
                {
                    case IDLE:
                    {
                        // Waiting for One pin being pressed.
                        if(g_pButtons->keyButton==currentKey)
                        {
                            g_pButtons->state = PRESSING;
                            g_pButtons->nTick = g_systickCounter + PRESS_TICK;
                        }
                        break;
                    }
                    case PRESSING:
                    {
                        // waiting for One pin being released.
                        if(g_pButtons->keyButton!=currentKey)
                        {
                            g_pButtons->state = RELEASING;
                            g_pButtons->nTick = g_systickCounter + RELEASE_TICK;
                            /*-------------------------------------------------------return  */
                         if(g_pButtons->fnCallbackPRESS != NULL)
                            {
                                g_pButtons->fnCallbackPRESS(); // Process press event.
                                delay(50);
                            }
                            break;
                        }
                        // Detect hold event.
                        if((g_pButtons->keyButton==currentKey) && g_pButtons->nTick <= g_systickCounter++)
                        {

                            g_pButtons->state = HOLDING;     
                            g_pButtons->nTick = g_systickCounter + HOLD_TICK;
                            break;
                        }
                        break;
                    }
                    case HOLDING:
                    {                   
                       
                        if( g_pButtons->nTick <= g_systickCounter)
                        {
                            if(g_pButtons->fnCallbackHOLD != NULL)
                            {
                                g_pButtons->fnCallbackHOLD(); // Process click event.
                                delay(50);
                            }
                            g_pButtons->state = HOLDED;
                            break;
                        }
                        if(g_pButtons->keyButton!=currentKey)
                        {
                            if(g_pButtons->fnCallbackHOLD != NULL)
                            {
                                g_pButtons->fnCallbackHOLD(); // Process Hold event.
                                delay(50);
                            }
                            g_pButtons->state = RELEASING;
                            g_pButtons->nTick = g_systickCounter + RELEASE_TICK;
                            break;
                        }
                        break; 
                    }
                    case HOLDED:
                      {
                        if(g_pButtons->keyButton!=currentKey)
                        {
                          g_pButtons->state = RELEASING;
                          g_pButtons->nTick = g_systickCounter + RELEASE_TICK;
                          break;
                        }
                        break;
                      }
                    case RELEASING:
                      {
                        if(g_pButtons->nTick <= g_systickCounter)
                        {
                          if(g_pButtons->fnCallbackRELEASE != NULL)
                            {
                                g_pButtons->fnCallbackRELEASE(); // Process Hold event.
                                delay(50);
                            }
                          g_pButtons->state = IDLE;
                          g_pButtons->nTick = 0;
                          break;
                        }
                        if(currentKey)
                        {
                          if(g_pButtons->fnCallbackRELEASE != NULL)
                            {
                                g_pButtons->fnCallbackRELEASE(); // Process Hold event.
                                delay(50);
                            }
                          g_pButtons->state = PRESSING;
                          g_pButtons->nTick = g_systickCounter + PRESS_TICK;
                          break;
                        }
                        break;
                      }
                        break;
                }
}

void eventPress(void);
void eventHold(void);
void eventRelease(void);






/////////////////////////////////////////////////////////////////////////////////
/*
For LCD
*/
const unsigned char Picture_test [] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0xFF, 0xFE,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0,
0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0xE0, 0xE0, 0xE0,
0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03, 0x03, 0x03, 0x03,
0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0x7F,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
void sendCommand(int instruction);
void sendData(int data);
void Init_LCD()
{
	GPIO_PinWrite(GPIOD, 14U, 1);      //CS pin
	GPIO_PinWrite(GPIOD, 13U, 0);      //Clk pin
        delay(50);   //wait for >40 ms
}

void GraphicMode ()   
{
		sendCommand(0x30);  // 8 bit mode
		delay (1000);
		sendCommand(0x34);  // switch to Extended instructions
		delay (1000);
		sendCommand(0x36);  // enable graphics
		delay (1000);
}
void DrawBitmap(const unsigned char* graphic)
{
	uint8_t x, y;
	for(y = 0; y < 64; y++)
	{
		if(y < 32)
		{
			for(x = 0; x < 8; x++)				// Draws top half of the screen.
			{					        // In extended instruction mode, vertical and horizontal coordinates must be specified before sending data in.
				sendCommand(0x80 | y);			// Vertical coordinate of the screen is specified first. (0-31)
				sendCommand(0x80 | x);			// Then horizontal coordinate of the screen is specified. (0-8)
				sendData(graphic[2*x + 16*y]);		// Data to the upper byte is sent to the coordinate.
				sendData(graphic[2*x+1 + 16*y]);	// Data to the lower byte is sent to the coordinate.
			}
		}
		else
		{
			for(x = 0; x < 8; x++)				// Draws bottom half of the screen.
			{						// Actions performed as same as the upper half screen.
				sendCommand(0x80 | (y-32));		// Vertical coordinate must be scaled back to 0-31 as it is dealing with another half of the screen.
				sendCommand(0x88 | x);
				sendData(graphic[2*x + 16*y]);
				sendData(graphic[2*x+1 + 16*y]);
			}
		}

	}
}

void send_1(void)
{
	GPIO_PinWrite(GPIOD, 15U, 1);
	GPIO_PinWrite(GPIOD, 13U, 1);
        delay(1);
	GPIO_PinWrite(GPIOD, 13U, 0);
}

void send_0(void)
{
	GPIO_PinWrite(GPIOD, 15U, 0);
	GPIO_PinWrite(GPIOD, 13U, 1);
        delay(1);
	GPIO_PinWrite(GPIOD, 13U, 0);
}

char read_bit(void)
{
	char value_bit;
	GPIO_PinWrite(GPIOD, 13U, 1);
	value_bit = GPIO_PinRead(GPIOD, 15U);
	GPIO_PinWrite(GPIOD, 13U, 0);
	return value_bit;
}


void sendCommand(int instruction)
{
	send_1();
	send_1();
	send_1();
	send_1();
	send_1();
	send_0();  //RW bit
	send_0();	 //RS bit
	send_0();
	for (unsigned char i=7;i>=4;i--) 
          {
            if ((instruction >> i)&0x01) send_1(); 
            else send_0(); //D7:D4
          }
        send_0();
	send_0();
	send_0();
	send_0();
	for (int i=3;i>=0;i--) 
          {
            if ((instruction >> i)&0x01) send_1(); 
            else send_0(); //D3:D0
          }
	send_0();
	send_0();
	send_0();
	send_0();
	
}

void sendData(int data)
{
	send_1();
	send_1();
	send_1();
	send_1();
	send_1();
	send_0();  //RW bit
	send_1(); //RS bit
	send_0();
	for (unsigned char i=7;i>=4;i--) 
          {
            if ((data >> i)&0x01) send_1(); 
            else send_0(); //D7:D4
          }
	send_0();
	send_0();
	send_0();
	send_0();
	for (int i=3;i>=0;i--) 
          {
            if ((data >> i)&0x01) send_1(); 
            else send_0(); //D3:D0
          }
	send_0();
	send_0();
	send_0();
	send_0();
}

void HOME()
{
    sendCommand(0x02);
}

void CLEAR()
{
    sendCommand(0x01);
}
void CLEAR2()
{
    uint8_t x, y;
    for(y = 0; y < 64; y++)
    {
            if(y < 32)
            {
                    sendCommand(0x80 | y);
                    sendCommand(0x80);
            }
            else
            {
                    sendCommand(0x80 | (y-32));
                    sendCommand(0x88);
            }
            for(x = 0; x < 8; x++)
            {
                    sendData(0);
                    sendData(0);
            }
    }
}

void ENTRY_MODE_SET()
{
    sendCommand(0x04);
}

void Display_Status(unsigned char Type_display)
{
    sendCommand(Type_display);
}

void sendString(char *String)
{
	unsigned char i = 0;
	while (String[i])
	{
		sendData(String[i]);
                //ENTRY_MODE_SET();
		i++;
	}
}
void Graphic_Off()
{
    sendCommand(0x20);
}
void SelectLine(unsigned char line)
{
  if(line == 2) sendCommand(0x90);
  else if (line ==3) sendCommand(0x88);
  else if (line ==4) sendCommand(0x98);
  else if (line == 1) sendCommand(0x80);
}

void setPos(unsigned int x, int y)
{
  int instruction =0;
  if (y>8) y=8;
  if (x>4) x=4;
  if (x == 1)
  {
      instruction = 128 + y;
  }
  else if (x==2)
  {
      instruction = 144 +y;
  }
  else if (x==3)
  {
      instruction = 136 +y;
  }
  else 
  {
      instruction = 152 +y;
  }
  sendCommand(instruction);
}


/*Test state machine*/
void eventPressA(void)
{
    HOME();
    sendData('A');
}
void eventHoldA(void)
{
    HOME();
    sendData('B');
}
/* ------------------------------- */
/*!
 * @brief Main function
 */
int main(void)
{
    /* Define the init structure for the output LED pin*/
    gpio_pin_config_t out_config = {
        kGPIO_DigitalOutput,
        1,
    };
    
    gpio_pin_config_t in_config = {
        kGPIO_DigitalInput,
        1,
    };

    /* Board pin, clock, debug console init */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /* Print a note to terminal. */
    PRINTF("\r\n GPIO Driver example\r\n");

    /* Init output GPIO. */
    GPIO_PinInit(GPIOE, 8U, &out_config);       /*Col1 button*/
    GPIO_PinInit(GPIOB, 5U, &out_config);       /*Col2 button*/
    GPIO_PinInit(GPIOC, 3U, &out_config);       /*Col3 button*/
      
    GPIO_PinInit(GPIOD, 14U, &out_config);      /*CS pin LCD*/
    GPIO_PinInit(GPIOD, 15U, &out_config);      /*DATA pin LCD*/
    GPIO_PinInit(GPIOD, 13U, &out_config);      /*CLK pin LCD*/
    GPIO_PinInit(GPIOA, 9U, &out_config);       /*LED pin LCD*/
    
    /* Init input GPIO. */
    GPIO_PinInit(GPIOC, 2U, &in_config);        /*Row1 button*/
    GPIO_PinInit(GPIOD, 6U, &in_config);        /*Row2 button*/
    GPIO_PinInit(GPIOD, 7U, &in_config);        /*Row3 button*/
    GPIO_PinInit(GPIOD, 5U, &in_config);        /*Row4 button*/
    
    Init_LCD();
    Graphic_Off();
  //  GraphicMode();
    CLEAR();
    //CLEAR2();
    HOME();
    Graphic_Off();
    sendData('0');
    Display_Status(0x0E);
//    DrawBitmap(Picture_test);
    BUTTON buttonA = 
    {
      .keyButton = '1',
      .state = IDLE,		      
      .fnCallbackPRESS = eventPressA,
      .fnCallbackHOLD = eventHoldA,
    };
    
    if (SysTick_Config(SystemCoreClock / 1000U))
    {
        while (1)
        {
        }
    }
    //CLEAR2();
    while (1)
    {
        //CLEAR();
        delay(1);
        unsigned char key1=0;
        key1 = check_key(); 
/*        if (key1)
        {
        CLEAR();
        sendData(key1);
        }*/
       ButtonProcessEvent(&buttonA, key1);
    }
}
