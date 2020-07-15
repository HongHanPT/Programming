/** @file    GLCD.c
  * @version 1.0
  * @created 15th July 2020
  * @author  HongHan
  * @brief   
  *          This file provides firmware functions to manage the following functionalities of the LCD.
  *          + Initialization and de-initialization functions
  *             ++ 
  *          + Operation functions
  *             ++
  *          + Control functions
  *             ++
  *          + State functions
  *             ++
  */

#ifndef __KEYPAD_H__
#define __KEYPAD_H__

#include "fsl_gpio.h"
#include "MKE15Z7.h"

#define MAX_BUTTON_COUNT    10      //
#define PRESS_TICK          300	    //
#define HOLD_TICK           200	    //
#define RELEASE_TICK        100     //

#define        PORT_COL1       GPIOE// keypad->keypad_col1->port
#define        PORT_COL2       GPIOB// keypad->keypad_col2->port
#define        PORT_COL3       GPIOC// keypad->keypad_col3->port
#define        PORT_ROW1       GPIOC// keypad->keypad_row1->port
#define        PORT_ROW2       GPIOD// keypad->keypad_row2->port
#define        PORT_ROW3       GPIOD// keypad->keypad_row3->port
#define        PORT_ROW4       GPIOD// keypad->keypad_row3->port

#define        PIN_COL1        8U//keypad->keypad_col1->pin
#define        PIN_COL2        5U//keypad->keypad_col2->pin
#define        PIN_COL3        3U//keypad->keypad_col3->pin
#define        PIN_ROW1        2U//keypad->keypad_row1->pin
#define        PIN_ROW2        6U//keypad->keypad_row2->pin
#define        PIN_ROW3        7U//keypad->keypad_row3->pin
#define        PIN_ROW4        5U//keypad->keypad_row3->pin


typedef enum
{
    IDLE = 0,
    PRESSING,
    HOLDING,
    RELEASING,
    HOLDED,
}state_button_t;

typedef enum
{
    NoPress = 0,
    IsPress,
}state_press_button_t;

typedef struct tagButton
{
    unsigned int 	nTick;			// Current tick of button.
    unsigned char   	keyButton;		// 
    unsigned char   	last_keyButton;
    char	state;			        //
    void (*fnCallbackIDLE)(unsigned char key);               // 
    void (*fnCallbackPRESS)(unsigned char key);
    void (*fnCallbackHOLD)(unsigned char key);
    void (*fnCallbackRELEASE)(unsigned char key);
}button_t;

typedef struct{
  GPIO_Type *port;
  uint32_t  pin;
}keypad_line_t;

typedef struct{
  keypad_line_t *keypad_col1;
  keypad_line_t *keypad_col2;
  keypad_line_t *keypad_col3;
  keypad_line_t *keypad_row1;
  keypad_line_t *keypad_row2;
  keypad_line_t *keypad_row3;
  keypad_line_t *keypad_row4;
}keypad_t;

static const unsigned char Key[4][3] = {{'5','1','9'},{'7','3','#'},{'4','0','8'},{'6','2','*'}};

void KEYPAD_Init();
char KEYPAD_CheckKey(void);
void KEYPAD_ProcessButtonEvent(button_t *g_pButtons);


#endif /* __KEYPAD_H__*/