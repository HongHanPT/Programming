/** @file    Menu.c
  * @version 1.0
  * @created 16th July 2020
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
  
#ifndef __MENU_TREE_H__
#define __MENU_TREE_H__
  
/*********************************************************************************
 * INCLUDE
 */
#include "lcd_st7920_serial.h"



/*********************************************************************************
 * DEFINE
 */
 
#define SIGNAL_ENTER					(*signalEnter)
#define SIGNAL_UP				        (*signalUp)
#define SIGNAL_DOWN					(*signalDown)
#define SIGNAL_CANCEL					(*signalCancel)
#define MAX_SHOWED_MENU					(*maxShowedMenu)

#define GIA_TRI_NHO_HON(giaTri1,giaTri2)                (giaTri1<giaTri2?giaTri1:giaTri2)

/*********************************************************************************
 * TYPEDEFS
 */
 
typedef struct  _MenuItem_t MenuItem_t;
struct _MenuItem_t
{
  char *name;
  char size;               
  MenuItem_t *parent;
  MenuItem_t *child;
  MenuItem_t *brother;
  void(*fnCallback)();
  //MenuItem_t av[];
};

typedef enum{
	OFF = 0U,
	ON,
}signal_state_t;

typedef enum{
	NO_UPDATE =0U,
	YES_UPDATE,
}update_browse_t;

/*********************************************************************************
 * EXPORTED FUNCTION
 */

void MENU_SetMaxShowedMenu(char _maxShowedMenu);

void MENU_InitSignalControl(char *signalEnter, char *signalUp, char *signalDown, char *signalCacel);

void MENU_BrowseMenu(MenuItem_t *rootMenu, update_browse_t isUpdateBrowse);

void MENU_AddSubMenu2Menu(MenuItem_t *root1, MenuItem_t *child1);
#endif  /* __MENU_TREE_H__ */

