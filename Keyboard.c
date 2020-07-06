/********************************************************************************
Product: MSP430 FrameWork
Module: Button
Created: 5/18/2015, by KIENLTB
Description: Button framework
********************************************************************************/
/*-----------------------------------------------------------------------------*/
/* Header inclusions */
/*-----------------------------------------------------------------------------*/
#include "system.h"
#include "button.h"
#include "timer.h"
#include "msp430g2553.h"
/*-----------------------------------------------------------------------------*/
/* Data type definitions */
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/
/* Constant definitions  */
/*-----------------------------------------------------------------------------*/
#define MAX_BUTTON_COUNT    10      //
#define CLICK_TICK          200	    //
#define DOUBLE_CLICK_TICK   200	    //
#define HOLD_TICK           500

//Logic level

#define LOW     0x00
#define HIGH    0x01
/*-----------------------------------------------------------------------------*/
/* Macro definitions  */
/*-----------------------------------------------------------------------------*/
#define BUTTON_LOGIC_LEVEL(nIndex) ((GetPins(g_pButtons[nIndex]->gpioPin) & g_pButtons[nIndex]->gpioPin) != 0 )
#define TIMER_TIME_MASK     0x7FFF
#define TIMER_TYPE_LONG	    0x8000

#define CURRENT_TICK    (GetTickCount()   & TIMER_TIME_MASK)
#define CURRENT_SECOND  (GetSecondCount() & TIMER_TIME_MASK)
/*-----------------------------------------------------------------------------*/
/* Global variables  */
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/* Function prototypes  */
/*-----------------------------------------------------------------------------*/
VOID ButtonInit(PBUTTON button, WORD GPIO_PIN, SYSTEMCALLBACK onIDLE, SYSTEMCALLBACK onClick, SYSTEMCALLBACK onDoubleClick, SYSTEMCALLBACK onHold);
BOOL ButtonStart(PBUTTON pBtnRegister);
VOID ButtonCancel(BUTTON btnCancel);
VOID ButtonProcessEvent(VOID);
/*-----------------------------------------------------------------------------*/
/* Function implementations */
/*-----------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------
Function:   ButtonInit
Purpose:    Initiazation Button and register callback to handle events (event can be NULL)
Parameters: \GPIO_PIN: the GPIO that connect to button
            \onIDLE: Callback function handle IDLE event state.
            \onClick: Callback function handle Click event.
            \onDoubleClick: Callback function handle double click event.
Return:     VOID
--------------------------------------------------------------------------------*/

VOID ButtonInit(PBUTTON button,                 \
                WORD GPIO_PIN,                  \
                SYSTEMCALLBACK onIDLE,          \
                SYSTEMCALLBACK onClick,         \
                SYSTEMCALLBACK onDoubleClick,   \
                SYSTEMCALLBACK onHold)
{
    button->fnCallbackIDLE = onIDLE;
    button->fnCallbackClick = onClick;
    button->fnCallbackDoubleClick = onDoubleClick;
    button->fnCallbackHold = onHold;

    /* 	Set GPIO as INPUT, enable pullup resistor*/
    button->gpioPin = GPIO_PIN;
    SetPinDirs(0,button->gpioPin);                  // Set an Input
    SetPullUpDownResisterPins(button->gpioPin, 0);  // Set pullup register

    /*	Setup param*/
    button->nTick = 0;
    button->state = IDLE;
    button->enable = 0;
}
/*--------------------------------------------------------------------------------
Function:   ButtonStart
Purpose:    Enable Button, register button to array g_pButtons[] and start listion event
Parameters: PBUTTON that need to start.
Return:     0 if success, 1 if fail
--------------------------------------------------------------------------------*/

BOOL ButtonStart(PBUTTON pBtnRegister)
{
    BYTE nIndex;
    /*	Register to g_pButtons to process*/
    for( nIndex = 0; nIndex < MAX_BUTTON_COUNT; nIndex++)
    {
        if(g_pButtons[nIndex] == NULL || g_pButtons[nIndex]->gpioPin == pBtnRegister->gpioPin)
        {
            g_pButtons[nIndex] = pBtnRegister;

            /*	Reset param  of IDEL state	*/
            pBtnRegister->nTick = 0;
            pBtnRegister->state = IDLE;
            pBtnRegister->enable = 1;

            return 0;
        }

    }
    return 1;
}
/*--------------------------------------------------------------------------------
Function:   ButtonCancel
Purpose:    Detroy Button by delete from g_pButtons[]
Parameters: BUTTON that need to cancel
Return:     VOID
--------------------------------------------------------------------------------*/

VOID ButtonCancel(BUTTON btnCancel)
{
    BYTE nIndex;
    for( nIndex = 0; nIndex < MAX_BUTTON_COUNT; nIndex++)
    {
        if(g_pButtons[nIndex]->gpioPin  == btnCancel.gpioPin)
        {
            g_pButtons[nIndex] = NULL;
        }
    }
}
/*--------------------------------------------------------------------------------
Function:   ButtonProcessEvent
Purpose:    sweep whole g_pButtons[] and check the button event to process.
Parameters: VOID
Return:     VOID
--------------------------------------------------------------------------------*/

VOID ButtonProcessEvent(VOID)
{
    BYTE nIndex;
    for( nIndex = 0; nIndex < MAX_BUTTON_COUNT; nIndex++)
    {
        if(g_pButtons[nIndex] != NULL && g_pButtons[nIndex]->enable == 1)
        {
            switch(g_pButtons[nIndex]->state)
            {
                case IDLE:
                {
                    // Waiting for One pin being pressed.
                    if(BUTTON_LOGIC_LEVEL(nIndex) == LOW)
                    {
                        g_pButtons[nIndex]->state = WAIT_BUTTON_UP;
                        g_pButtons[nIndex]->nTick = CURRENT_TICK + HOLD_TICK;
                    }
                    break;
                }
                case WAIT_BUTTON_UP:
                {
                    // waiting for One pin being released.
                    if(BUTTON_LOGIC_LEVEL(nIndex) == HIGH && g_pButtons[nIndex]->nTick > CURRENT_TICK)
                    {
                        g_pButtons[nIndex]->state = WAIT_CLICK_TIMEOUT;
                        g_pButtons[nIndex]->nTick = CURRENT_TICK + CLICK_TICK;
                        break;
                    }
                    // Detect hold event.
                    if(BUTTON_LOGIC_LEVEL(nIndex) == HIGH && g_pButtons[nIndex]->nTick <= CURRENT_TICK)
                    {

                        if(g_pButtons[nIndex]->fnCallbackHold != NULL)
                        {
                            g_pButtons[nIndex]->fnCallbackHold(NULL); // Process hold event.
                            __delay_cycles(500000);
                        }
                        g_pButtons[nIndex]->state = IDLE;
                        g_pButtons[nIndex]->nTick = 0;
                        break;
                    }
                    break;
                }
                case WAIT_CLICK_TIMEOUT:
                {
                    // Waiting for One pin being pressed the second time or timeout.
                    if(BUTTON_LOGIC_LEVEL(nIndex) == LOW && g_pButtons[nIndex]->nTick > CURRENT_TICK)
                    {
                        g_pButtons[nIndex]->state = WAIT_DCLICK_TIMEOUT;
                        g_pButtons[nIndex]->nTick = CURRENT_TICK + DOUBLE_CLICK_TICK;
                        break;
                     }
                    // Detect Click event
                    if( g_pButtons[nIndex]->nTick <= CURRENT_TICK)
                    {
                        if(g_pButtons[nIndex]->fnCallbackClick != NULL)
                        {
                            g_pButtons[nIndex]->fnCallbackClick(NULL); // Process click event.
                            __delay_cycles(500000);
                        }
                        g_pButtons[nIndex]->state = IDLE;
                        g_pButtons[nIndex]->nTick = 0;
                        break;
                    }
                    break;
                }
                case WAIT_DCLICK_TIMEOUT:
                {
                    // If logic Level = HIGH (release Button) or timer out => call the callback function to handle event.
                    // Change state to IDLE
                    if(BUTTON_LOGIC_LEVEL(nIndex) == HIGH)
                    {
                        if(g_pButtons[nIndex]->fnCallbackDoubleClick != NULL)
                        {
                            g_pButtons[nIndex]->fnCallbackDoubleClick(NULL); // Process click event.
                            __delay_cycles(500000);
                        }
                        g_pButtons[nIndex]->state = IDLE;
                        g_pButtons[nIndex]->nTick = 0;
                        break;
                    }
                    break;
                }
            }

        }
    }
}