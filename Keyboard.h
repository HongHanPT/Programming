/********************************************************************************
Product: MSP430 FrameWork
Module: Button
Author: 5/18/2015, by KIENLTB
Description: Framework for button
********************************************************************************/
#ifndef __BUTTON__
#define __BUTTON__
/*-----------------------------------------------------------------------------*/
/* Header inclusions */
/*-----------------------------------------------------------------------------*/
#include "io.h"
/*-----------------------------------------------------------------------------*/
/* Data type definitions */
/*-----------------------------------------------------------------------------*/
typedef enum tagSTATE
{
    IDLE,
    WAIT_BUTTON_UP,
    WAIT_PRESS_TIMEOUT,
    WAIT_CLICK_TIMEOUT,
    WAIT_DCLICK_TIMEOUT
}state;

typedef struct tagButton
{
    WORD 	nTick;			// Current tick of button.
    WORD 	gpioPin;		// GPIO PIN connect witch button.
    BOOL 	enable;			//
    BYTE	state;			//
    SYSTEMCALLBACK  fnCallbackIDLE;
    SYSTEMCALLBACK  fnCallbackClick;
    SYSTEMCALLBACK  fnCallbackDoubleClick;
    SYSTEMCALLBACK  fnCallbackHold;


}BUTTON, *PBUTTON;
/*-----------------------------------------------------------------------------*/
/* Constant definitions  */
/*-----------------------------------------------------------------------------*/
#define MAX_BUTTON_COUNT    10      // can handle max 10 button, can adjust to meet requirement
#define CLICK_TICK          200	    // 200ms
#define DOUBLE_CLICK_TICK   200	    // 200ms
#define HOLD_TICK           500     // 500ms
/*-----------------------------------------------------------------------------*/
/* Macro definitions  */
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/
/* Global variables  */
/*-----------------------------------------------------------------------------*/
// Array store pointer to button
INTERNAL PBUTTON g_pButtons[MAX_BUTTON_COUNT] = {{NULL}};

/*-----------------------------------------------------------------------------*/
/* Function prototypes  */
/*-----------------------------------------------------------------------------*/

/*
    initialization Button and regist callback function to handle event
*/
VOID ButtonInit(PBUTTON button, WORD GPIO_PIN, SYSTEMCALLBACK onIDLE, SYSTEMCALLBACK onClick, SYSTEMCALLBACK onDoubleClick, SYSTEMCALLBACK onHold);

/*
    Enable Button, regist to array g_pButtons[]and start listion event, recall while button cancel;
*/
BOOL ButtonStart(PBUTTON pBtnRegister);

/*
    Detroy Button, delete from g_pButtons[].
*/
VOID ButtonCancel(BUTTON btnCancel);

/*
Quet mang g_pButtons[] va check cac button da dang ky. Xu ly button .
*/
VOID ButtonProcessEvent(VOID);

#endif // __BUTTON__