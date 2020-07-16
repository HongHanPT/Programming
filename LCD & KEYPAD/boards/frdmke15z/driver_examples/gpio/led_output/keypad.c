/** @file    GLCD.c
  * @version 1.0
  * @created 15th July 2020
  * @author  HongHan
  * @brief   
  *          This file provides firmware functions to manage the following functionalities of the Keypad.
  *          + Initialization and de-initialization functions
  *             ++ 
  *          + Operation functions
  *             ++
  *          + Control functions
  *             ++
  *          + State functions
  *             ++
  */

/*********************************************************************************
 * INCLUDE
 */

#include "keypad.h"

/*********************************************************************************
 *  VARIBLES
 */

static volatile uint32_t g_systickCounter=0;
keypad_t *keypad;

/*********************************************************************************
 * EXPORTED FUNCTION
 */

void SysTick_Handler(void)
{
    if (g_systickCounter != 0xFFFFFFFF)
    {
        g_systickCounter++;
    }
    else g_systickCounter = 0;
}


static void delay(unsigned int time)
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
 * @brief       Initialize the keypad pin
 * 
 * @param       None
 * @retval      None
 */
void KEYPAD_Init(keypad_t *_keypad)
{
      keypad = _keypad;
//    keypad -> keypad_col1 -> port = _keypad -> keypad_col1 -> port;
//    keypad -> keypad_col1 -> pin =  keypad -> keypad_col1 -> pin;
//    
//    keypad -> keypad_col2 -> port = _keypad -> keypad_col2 -> port;
//    keypad -> keypad_col2 -> pin =  _keypad -> keypad_col2 -> pin;
//    
//    keypad -> keypad_col3 -> port = _keypad -> keypad_col3 -> port;
//    keypad -> keypad_col3 -> pin =  _keypad -> keypad_col3 -> pin;
//      
//    keypad -> keypad_row1 -> port = _keypad -> keypad_row1 -> port;
//    keypad -> keypad_row1 -> pin =  _keypad -> keypad_row1 -> pin;
//      
//    keypad -> keypad_row2 -> port = _keypad -> keypad_row2 -> port;
//    keypad -> keypad_row2 -> pin =  _keypad -> keypad_row2 -> pin;
//      
//    keypad -> keypad_row3 -> port = _keypad -> keypad_row3 -> port;
//    keypad -> keypad_row4 -> pin =  keypad -> keypad_row4 -> pin;
//      
//    keypad -> keypad_row4 -> port = _keypad -> keypad_row4 -> port;
//    keypad -> keypad_row4 -> pin =  _keypad -> keypad_row4 -> pin;
}

/**
 * @brief       Check keypad have pressed button
 * 
 * @param       None
 * @retval      state_press_button_t
 */
static state_press_button_t keypad_CheckButton(void)
{
    GPIO_PinWrite(PORT_COL1, PIN_COL1, 0);        //Col1
    GPIO_PinWrite(PORT_COL2, PIN_COL2, 0);        //Col2
    GPIO_PinWrite(PORT_COL3, PIN_COL3, 0);        //Col3
    if((!GPIO_PinRead(PORT_ROW1, PIN_ROW1))||(!GPIO_PinRead(PORT_ROW2, PIN_ROW2))||(!GPIO_PinRead(PORT_ROW3, PIN_ROW3))||(!GPIO_PinRead(PORT_ROW4, PIN_ROW4)))
      return IsPress;
    return NoPress;    
}


/**
 * @brief       Scan column of keypad
 * 
 * @param       col Column scaned
 * @retval      None
 */
static void keypad_ScanCol(unsigned char col)
{
    GPIO_PinWrite(PORT_COL1, PIN_COL1, 1);        //Col1
    GPIO_PinWrite(PORT_COL2, PIN_COL2, 1);        //Col2
    GPIO_PinWrite(PORT_COL3, PIN_COL3, 1);        //Col3
    if (col ==1) 
        GPIO_PinWrite(PORT_COL1, PIN_COL1, 0);        //Col1
     else if (col == 2) 
        GPIO_PinWrite(PORT_COL2, PIN_COL2, 0);        //Col2
     else if (col == 3)
        GPIO_PinWrite(PORT_COL3, PIN_COL3, 0);        //Col3
 
}


/**
 * @brief       Check row of keypad
 * 
 * @param       None
 * @retval      Row have button pressed
 */
static unsigned char keypad_CheckRow(void)
{
    unsigned char row = 0;
    if (!GPIO_PinRead(PORT_ROW1, PIN_ROW1)) row =1;
    else if (!GPIO_PinRead(PORT_ROW2, PIN_ROW2)) row =2;
    else if (!GPIO_PinRead(PORT_ROW3, PIN_ROW3)) row =3;
    else if (!GPIO_PinRead(PORT_ROW4, PIN_ROW4)) row =4;
    return row;
}


/**
 * @brief       Check button is pressed
 * 
 * @param       None
 * @retval      Button pressed name
 */
char KEYPAD_CheckKey(void)
{
    unsigned char row;
    if (keypad_CheckButton())
    {
      delay(10);
      if (keypad_CheckButton())
      {
        for (unsigned char col = 1; col <=3;col ++)
        {
          
          keypad_ScanCol(col);
          row = keypad_CheckRow();
          if(row) return Key[row-1][col-1];
        }
      }
      else return 0;
    }
    return 0;
}

/**
 * @brief       Process state machine of keypad
 * 
 * @param       g_pButtons  button pointer
 * @retval      None
 */
void KEYPAD_ProcessButtonEvent(button_t *g_pButtons)
{
      switch(g_pButtons->state)
      {
          case IDLE:
          {
              // Waiting for One pin being pressed.
              if(g_pButtons->keyButton)
              {
                  g_pButtons->state = PRESSING;
                  g_pButtons->nTick = g_systickCounter + PRESS_TICK;
                  g_pButtons->last_keyButton = g_pButtons->keyButton;
              }
              break;
          }
          case PRESSING:
          {
              // waiting for One pin being released.
              if(g_pButtons->keyButton==0)
              {
                  g_pButtons->state = RELEASING;
                  g_pButtons->nTick = g_systickCounter + RELEASE_TICK;
                  /*-------------------------------------------------------return  */
                  if(g_pButtons->fnCallbackPRESS != NULL)
                    {
                      g_pButtons->fnCallbackPRESS(g_pButtons->last_keyButton); // Process press event.
                      delay(10);
                    }                           
                  break;
              }
              else if(g_pButtons->keyButton!=g_pButtons->last_keyButton)
              {
                  g_pButtons->state = PRESSING;
                  g_pButtons->nTick = g_systickCounter + PRESS_TICK;
                  if(g_pButtons->fnCallbackPRESS != NULL)
                    {
                      g_pButtons->fnCallbackPRESS(g_pButtons->last_keyButton); // Process press event.
                      delay(10);
                    }
                  g_pButtons->last_keyButton = g_pButtons->keyButton;
                  break;
              }
              
              // Detect hold event.
              if((g_pButtons->keyButton==g_pButtons->last_keyButton) && (g_pButtons->nTick <= g_systickCounter))
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
                      g_pButtons->fnCallbackHOLD(g_pButtons->last_keyButton); // Process Hold event.
                      delay(10);
                  }
                  g_pButtons->state = HOLDED;
                  break;
              }
              if(g_pButtons->keyButton==0)
              {
                  g_pButtons->state = RELEASING;
                  g_pButtons->nTick = g_systickCounter + RELEASE_TICK;
                  
                  if(g_pButtons->fnCallbackHOLD != NULL)
                    {
                      g_pButtons->fnCallbackHOLD(g_pButtons->keyButton); // Process Hold event.
                      delay(10);
                    }                           
                  break;
              }
              else if(g_pButtons->keyButton!=g_pButtons->last_keyButton)
              {
                  g_pButtons->state = PRESSING;
                  g_pButtons->nTick = g_systickCounter + PRESS_TICK;
                  if(g_pButtons->fnCallbackHOLD != NULL)
                    {
                      g_pButtons->fnCallbackHOLD(g_pButtons->last_keyButton); // Process HOLD event.
                      delay(10);
                    }
                  g_pButtons->last_keyButton = g_pButtons->keyButton;
                  break;
              }
             
              break; 
          }
          case HOLDED:
          {
              if(g_pButtons->keyButton == 0)
              {
                g_pButtons->state = RELEASING;
                g_pButtons->nTick = g_systickCounter + RELEASE_TICK;
                break;
              }
              else if(g_pButtons->keyButton!=g_pButtons->last_keyButton)
              {
                  g_pButtons->state = PRESSING;
                  g_pButtons->nTick = g_systickCounter + PRESS_TICK;
                  if(g_pButtons->fnCallbackHOLD != NULL)
                    {
                      g_pButtons->fnCallbackHOLD(g_pButtons->last_keyButton); // Process HOLD event.
                      delay(10);
                    }
                  g_pButtons->last_keyButton = g_pButtons->keyButton;
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
                      g_pButtons->fnCallbackRELEASE(g_pButtons->last_keyButton); // Process Hold event.
                      delay(10);
                }
                g_pButtons->state = IDLE;
                break;
              }
              if(g_pButtons->keyButton != 0)
              {
                if(g_pButtons->fnCallbackRELEASE != NULL)
                {
                      g_pButtons->fnCallbackRELEASE(g_pButtons->last_keyButton); // Process Release event.
                      delay(10);
                }
                g_pButtons->state = PRESSING;
                g_pButtons->nTick = g_systickCounter + PRESS_TICK;
                g_pButtons->last_keyButton = g_pButtons->keyButton;
                break;
              }
              break;
           }
           break;
      }
}