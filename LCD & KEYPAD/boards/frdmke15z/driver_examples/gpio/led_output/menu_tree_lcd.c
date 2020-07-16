/** @file    GLCD.c
  * @version 1.0
  * @created 16th July 2020
  * @author  HongHan
  * @brief   
  *          This file provides firmware functions to manage the following functionalities of the Menu Tree.
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

#include "menu_tree_lcd.h"


/*********************************************************************************
 *  VARIBLES
 */
 
static char arrowPos =1 ,menuPos =1;
static char *signalEnter, *signalUp, *signalDown, *signalCancel;
static char maxShowedMenu=3;  
static MenuItem_t *root;


/*********************************************************************************
 * EXPORTED FUNCTION
 */
 
//static void delay(unsigned int time)
//{
//    volatile uint32_t i = 0;
//    for (unsigned int cnt = 0; cnt<=time;cnt++)
//    {
//      for (i = 0; i < 100; ++i)
//      {
//          __asm("NOP"); /* delay */
//      }
//    }
//}


/**
 * @brief    Show on the display
 * @note
 * 
 * @param    None
 * @retval   None
 */
static void MENU_ShowMenu()
{
  for(unsigned char i=1; i<=maxShowedMenu; i++)
   {
      LCD_SelectLine(i);
      LCD_SendData(' ');
   }
  if(root->child!=NULL)
  {
      MenuItem_t *cusor1;
      cusor1 = root->child;
      char i;
      if (menuPos-arrowPos +1 ==1) 
      {
          //LCD_SetPos(1,1);
          LCD_SelectLine(1);
          LCD_SendString("                ");
          //LCD_SetPos(1,1);
          LCD_SelectLine(1);
          LCD_SendData(' ');
          LCD_SendString(cusor1->name);
          cusor1 = cusor1->brother;
          i=1;
      }
      else
      {
          for(char i =1; i< menuPos-arrowPos +1;i++)
          {
            cusor1 = cusor1 -> brother; 
          }
          i=0;
      }
      while(cusor1!= NULL&& i< maxShowedMenu)
      {    
          i++;
          //LCD_SetPos(i,1);
          LCD_SelectLine(i);
          LCD_SendString("                ");
          //LCD_SetPos(i,1);
          LCD_SelectLine(i);
          LCD_SendData(' ');
          LCD_SendString(cusor1->name);//Lan luot hien ten cua M sub_menu from (vi tri hien tai - vi tri mui ten)
          cusor1 = cusor1->brother;
      }
      i++;
      //Hien thi mui ten
      LCD_SelectLine(arrowPos);
      LCD_SendData(0x1A);
      for (char j=i; j<=maxShowedMenu;j++)   //Xoa nhung dong k dung den
      {
          LCD_SelectLine(j);
          LCD_SendString("                ");
      }
  }
}

/*********************************************************************************
 * EXPORTED FUNCTION
 */

/**
 * @brief    Set the maximum number of lines displayed
 * @note     
 * 
 * @param    _maxShowedMenu	Maximum line is displayed
 * @retval   None
 */
void MENU_SetMaxShowedMenu(char _maxShowedMenu)
{
	maxShowedMenu = _maxShowedMenu;
}

/**
 * @brief    Initializes Signals to Control Menu Tree
 * @note
 * 
 * @param    _signalEnter	Signal to select menu
 * @param    _signalUp		Signal to roll up menu
 * @param    _signalDown	Signal to roll down menu
 * @param    _signalCacel	Signal to back previous menu
 * @retval   None
 */
void MENU_InitSignalControl(char *_signalEnter, char *_signalUp, char *_signalDown, char *_signalCacel)
{
	signalEnter = _signalEnter;
	signalUp = _signalUp;
	signalDown = _signalDown;
	signalCancel = _signalCacel;
}


/**
 * @brief    Browse Menu from rootMenu
 * @note	 
 * 
 * @param    rootMenu
 * @param    isUpdateBrowse
 * @retval   None
 */
void MENU_BrowseMenu(MenuItem_t *rootMenu, update_browse_t isUpdateBrowse)
{
  if((root==NULL)|| (isUpdateBrowse == YES_UPDATE)) 
  {
      root = rootMenu;
      menuPos=1;
      arrowPos=1;
  }
  if (SIGNAL_UP == ON)
  {
      SIGNAL_UP = OFF;
      if(menuPos>1)
      {
          menuPos--;
          if(arrowPos>2)
          {
            arrowPos--;
          }
          if (menuPos == 1) arrowPos =1;
      }
      else
      {
          menuPos= root ->size;
          arrowPos = GIA_TRI_NHO_HON(root ->size,maxShowedMenu);
      }
  }
  if (SIGNAL_DOWN == ON)
  {
      SIGNAL_DOWN = OFF;
      {
          MenuItem_t *cusor2;
          cusor2 = root -> child;
          for(unsigned char i=1;i<menuPos; i++)
          {
            cusor2 = cusor2->brother;
          }
          if(cusor2 -> brother !=NULL)
          {
            menuPos++;
            if (arrowPos <maxShowedMenu-1)
            {
              arrowPos++;
            }
            
            if (menuPos==cusor2->parent->size) 
            {
              arrowPos=GIA_TRI_NHO_HON(root ->size,maxShowedMenu);
            }
          }
          else 
          {
            menuPos= 1;
            arrowPos = 1;
          }
      }
  }
  
  if (SIGNAL_ENTER == ON)
  {
      SIGNAL_ENTER =OFF;
      if (root ->child!= NULL)
      {
          root = root -> child;
          for(unsigned char i=1;i<menuPos; i++)
          {
            root = root->brother;
          }
          if (root -> child ==NULL)
          {
              if (root ->fnCallback == NULL)
              {
                  root = root ->parent;
              }
              else root ->fnCallback();
          }
          else 
          {
            if (root ->fnCallback != NULL) 
            {
              root ->fnCallback();
            }
            menuPos =1;
            arrowPos =1;
          }
      }
  }
  
  if (SIGNAL_CANCEL == ON)
  {
      SIGNAL_CANCEL = OFF;
      if(root -> parent!=NULL)
      {
        root = root -> parent;
      }
      menuPos=1;
      arrowPos =1;
  }
  MENU_ShowMenu();
  
}

/**
 * @brief    Add SubMenu to Menu
 * @note
 * 
 * @param    root1 		Menu pointer
 * @param    child1		SubMenu pointer
 * @retval   None
 */
void MENU_AddSubMenu2Menu(MenuItem_t *root1, MenuItem_t *child1)
{
  if(root1 -> child == NULL)
  {
    root1 ->   child = child1;
    child1 -> parent = root1;
  }
  
  else
  {
    MenuItem_t *p;
    p = root1 -> child;
    while(p->brother!=NULL)
    {
      p = p-> brother;
    }
    p->brother = child1;
    child1 -> parent = root1;
  }
  root1->size ++;

}