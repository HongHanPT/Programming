/** @file    GLCD.c
  * @version 1.0
  * @date	 17th July 2020
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
/*********************************************************************************
 * INCLUDE
 */
#ifndef _GLCD_H_
#define _GLCD_H_

#include "fsl_gpio.h"
#include "MKE15Z7.h"
   
   
/*********************************************************************************
 * DEFINE
 */

/**
 * @defgroup  Function code 
 * @brief     Function code of request master
 */
#define BUTTON_ENTER					(*buttonEnter)
#define BUTTON_UP					(*buttonUp)
#define BUTTON_DOWN					(*buttonDown)
#define BUTTON_CANCEL					 (*buttonCancel)
#define MAX_SHOWED_MENU					 (*maxShowedMenu)
   
   
/*********************************************************************************
 * MACRO
 */


/*********************************************************************************
 * TYPEDEFS
 */
   
   
// /**
//  * @brief  Modbus state enumeration
//  */
//typedef struct Tree{
//  char *text;
//  char press1;
//  char press2;
//  char hold1;
//  char hold2;
//  char press0;
//}MenuEntry;

 /**
  * @brief  Modbus state enumeration
  */
typedef struct  Mia_t MenuItem_t;
struct Mia_t
{
  char *name;         /* */
  char number;                  /* */
  MenuItem_t *parent;
  MenuItem_t *child;
  MenuItem_t *brother;
  void(*fnCallback);
  //MenuItem_t av[];
};

 /**
  * @brief  Modbus state enumeration
  */
typedef enum{
	LCD_OK      = 0x00U,                                      /*!< Eerything OK                      */
	LCD_ERROR   = 0x01U,                                      /*!<                                   */
	LCD_TIMEOUT = 0x02U,                                      /*!<                                   */
}LCD_state_t;

 /**
  * @brief  Modbus state enumeration
  */
typedef enum{
	OFF = 0U,
	ON,
}state_t;

 /**
  * @brief  Modbus state enumeration
  */
typedef enum{
	DISPLAY_ON  = 0x0CU,
	DISPLAY_OFF = 0x0BU,
	CURSOR_ON   = 0x0AU,
	CURSOR_OFF  = 0x0DU,
	BLINK_ON	   = 0x01U,
	BLINK_OFF   = 0x0EU,
	
}display_t;


/*********************************************************************************
 * EXPORTED FUNCTION
 */

void Init();
void initSignalControl(char *signalEnter, char *signalUp, char *signalDown, char *signalCacel);
void delay(unsigned int time);
void setMaxShowedMenu(char _maxShowedMenu);
void send_1(void);
void send_0(void);
char read_bit(void);
void sendCommand(int instruction);
void sendData(int data);
void sendString(char *String);
void DrawBitmap(const unsigned char* graphic);
void GraphicMode ();
void HOME();
void CLEAR();
void CLEAR2();
void ENTRY_MODE_SET();
void Display(display_t Type_display);
void sendString(char *String);
void Graphic_Off();
void setPos(unsigned int x, int y);
void show_menu_new();
void browse_new(MenuItem_t *rootMenu);
void Add(MenuItem_t *root1, MenuItem_t *child1);
void ShowMenu();
void BrowseMenu();
#endif /* _GLCD_H_ */