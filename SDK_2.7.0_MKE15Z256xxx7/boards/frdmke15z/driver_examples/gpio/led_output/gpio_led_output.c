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

#include  <stdio.h>

#include "lcd_st7920_serial.h"
#include "keypad.h"
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

/*******************************************************************************
 * Variables
 ******************************************************************************/
char selected =0;
char PRESS1, PRESS2, HOLD1, HOLD2, PRESS0;
/*******************************************************************************
 * Code
 ******************************************************************************/


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
      for (i = 0; i < 100; ++i)
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

char UP, DOWN, ENTER, ENTER2, CANCEL;


/*Function CallBack*/
void eventPressA(unsigned char key);
void eventHoldA(unsigned char key);
void fn1(void);
/* ------------------------------- */
/*!
 * @brief Main function
 */

MenuItem_t Main = {.name= "Main", .size =0}; 
MenuItem_t Menu1 = {.name= "Tong doanh thu", .size =0}; 
MenuItem_t Menu2 = {.name= "Menh gia nhan", .size =0}; 
MenuItem_t Menu3 = {.name= "Menh gia tra", .size =0}; 

MenuItem_t Menu11 = {.name= "Doanh thu ngay", .size =0}; 
MenuItem_t Menu12 = {.name= "Doanh thu thang", .size =0}; 
MenuItem_t Menu13 = {.name= "Doanh thu nam", .size =0}; 

MenuItem_t Menu21 = {.name= "Gia tri lon nhat", .size =0}; 
MenuItem_t Menu22 = {.name= "Gia tri nho nhat", .size =0}; 
MenuItem_t Menu23 = {.name= "Chon menh gia", .size =0};

MenuItem_t Menu31 = {.name= "Gia tri tra", .size =0}; 
MenuItem_t Menu32 = {.name= "Bat/Tat", .size =0}; 
MenuItem_t Menu33 = {.name= "Empty", .size =0};
MenuItem_t Menu34 = {.name= "Full", .size =0};

MenuItem_t Menu111 = {.name= "Thread1", .size =0, .fnCallback = fn1}; 
MenuItem_t Menu121 = {.name= "Thread2", .size =0};
MenuItem_t Menu131 = {.name= "Thread3", .size =0}; 

MenuItem_t Menu211 = {.name= "Thread4", .size =0}; 
MenuItem_t Menu221 = {.name= "Thread5", .size =0};
MenuItem_t Menu231 = {.name= "Thread6", .size =0};

MenuItem_t Menu311 = {.name= "Thread7", .size =0}; 
MenuItem_t Menu321 = {.name= "Thread8", .size =0};
MenuItem_t Menu331 = {.name= "Thread9", .size =0};
MenuItem_t Menu341 = {.name= "Thread10", .size =0};

void fn1(void)
{
  LCD_Clear();
  if (ENTER2 ==1) 
  {
      LCD_BrowseMenu(&Main, YES_UPDATE);
      ENTER2 =0;
  }
}
//MenuItem_t *root;
button_t buttonA = 
{
  .state = IDLE,		      
  .fnCallbackPRESS = eventPressA,
  .fnCallbackHOLD = eventHoldA,
};
    

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
    
//    lcd_pins_t LCD_PINS;
//    //LCD_PINS.pinCS -> port  = GPIOD;              
//    //LCD_PINS.pinCS ->pin    = 14U;
//    LCD_PINS.pinDATA ->port = GPIOD;
//    LCD_PINS.pinDATA ->pin  = 15U;
//    LCD_PINS.pinCLK -> port  = GPIOD;
//    LCD_PINS.pinCLK ->pin   = 13U;
//    
//    keypad_t KEYPAD_PINS;
//    KEYPAD_PINS.keypad_col1->port = GPIOE;
//    KEYPAD_PINS.keypad_col1->pin  = 8U;
//    KEYPAD_PINS.keypad_col2->port = GPIOB;
//    KEYPAD_PINS.keypad_col2->pin  = 5U;
//    KEYPAD_PINS.keypad_col3->port = GPIOC;
//    KEYPAD_PINS.keypad_col3->pin  = 3U;
//    KEYPAD_PINS.keypad_row1->port = GPIOC;
//    KEYPAD_PINS.keypad_row1->pin  = 2U;
//    KEYPAD_PINS.keypad_row2->port = GPIOD;
//    KEYPAD_PINS.keypad_row2->pin  = 6U;
//    KEYPAD_PINS.keypad_row3->port = GPIOD;
//    KEYPAD_PINS.keypad_row3->pin  = 7U;
//    KEYPAD_PINS.keypad_row4->port = GPIOD;
//    KEYPAD_PINS.keypad_row4->pin  = 5U;

    
//    {.pinCS = {.port = GPIOD, .pin= 14U}, .pinDATA = {.port = GPIOD, .pin = 15U}, .pinCLK = {.port = GPIOD, .pin = 13}};
    LCD_Init();
    //KEYPAD_Init();
    LCD_GraphicMode(DISABLE);
    LCD_SendData(0x1A);
    LCD_Clear();
    LCD_AddSubMenu2Menu(&Main, &Menu1);
    LCD_AddSubMenu2Menu(&Main, &Menu2);
    LCD_AddSubMenu2Menu(&Main, &Menu3);
    
    LCD_AddSubMenu2Menu(&Menu1, &Menu11);
    LCD_AddSubMenu2Menu(&Menu1, &Menu12);
    LCD_AddSubMenu2Menu(&Menu1, &Menu13);
    
    LCD_AddSubMenu2Menu(&Menu2, &Menu21);
    LCD_AddSubMenu2Menu(&Menu2, &Menu22);
    LCD_AddSubMenu2Menu(&Menu2, &Menu23);
    
    LCD_AddSubMenu2Menu(&Menu3, &Menu31);
    LCD_AddSubMenu2Menu(&Menu3, &Menu32);
    LCD_AddSubMenu2Menu(&Menu3, &Menu33);
    LCD_AddSubMenu2Menu(&Menu3, &Menu34);
    
    LCD_AddSubMenu2Menu(&Menu11, &Menu111);
    LCD_AddSubMenu2Menu(&Menu12, &Menu121);
    LCD_AddSubMenu2Menu(&Menu13, &Menu131);
    
    LCD_AddSubMenu2Menu(&Menu21, &Menu211);
    LCD_AddSubMenu2Menu(&Menu22, &Menu221);
    LCD_AddSubMenu2Menu(&Menu231, &Menu231);
    
    LCD_AddSubMenu2Menu(&Menu31, &Menu311);
    LCD_AddSubMenu2Menu(&Menu32, &Menu321);
    LCD_AddSubMenu2Menu(&Menu33, &Menu331);
    LCD_AddSubMenu2Menu(&Menu33, &Menu341);
    
    LCD_InitSignalControl(&ENTER, &UP, &DOWN, &CANCEL);
    LCD_SetMaxShowedMenu(3);
 //   GraphicMode();

    //CLEAR2();
//    HOME();
//    sendData('0');
//    Display(DISPLAY_ON);
//    DrawBitmap(Untitled);

    if (SysTick_Config(SystemCoreClock / 1000U))
    {
        while (1)
        {
        }
    }

    LCD_BrowseMenu(&Main, YES_UPDATE);
    while (1)
    {
        //CLEAR();
       delay(100);
       buttonA.keyButton = 0;
       buttonA.keyButton = KEYPAD_CheckKey(); 
       unsigned char a = 0;
       a = buttonA.last_keyButton;
       KEYPAD_ProcessButtonEvent(&buttonA);
//       if(a)
//       {
//       LCD_BrowseMenu(&Main);
//       }
    }
}

void eventPressA(unsigned char key)
{
/* 09/07 */
/*  if(key)
    {
      Graphic_Off();
      CLEAR();
      HOME();
      sendData(key);
      sendData('-');
    }*/
 /////////////////////////
 /* 10/07   morning*/
/*  if (key == '1')
  {
    PRESS1 = 1;
  }
  if (key == '2')
  {
    PRESS2 = 1;
  }
    if (key == '0')
  {
    PRESS0 = 1;
  }*/
  
  /* Afternoon 10/07*/
  //browse_new();
  if (key == '7')
  {
    CANCEL = 1;
  }
  if (key == '#')
  {
    ENTER = 1;
    ENTER2 =1;
  }
    if (key == '0')
  {
    DOWN = 1;
  }
      if (key == '8')
  {
    UP = 1;
  }
  LCD_BrowseMenu(&Main, NO_UPDATE);
  printf("a");
}

void eventHoldA(unsigned char key)
{
  /* 09/07 */
  if(key)
  {
   //HOME();
   LCD_SendData(key);
   LCD_SendData('+');
  }
   /////////////////////////
 /* 10/07*/
  if (key == '1')
  {
    HOLD1 = 1;
  }
  if (key == '2')
  {
    HOLD2 = 1;
  }
  //15/07
  printf("a");
}