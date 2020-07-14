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
#include "fsl_gpio.h"
#include "GLCD.h"
#include "debug_macro.h"

/*********************************************************************************
 * DEFINE
 */

/**
 * @defgroup  GPIO PORT 
 * @brief
 */

/**
 * @defgroup  GPIO PIN 
 * @brief
 */



/*********************************************************************************
 * MACRO
 */


/*********************************************************************************
 * TYPEDEFS
 */
 




/*********************************************************************************
 *  VARIBLES
 */
char arrowPos =1 ,menuPos =1;
char *buttonEnter, *buttonUp, *buttonDown, *buttonCancel;
char maxShowedMenu=3;


/*********************************************************************************
 * EXPORTED FUNCTION
 */
 
/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
void Init()
{
	GPIO_PinWrite(GPIOD, 14U, 1);
	GPIO_PinWrite(GPIOD, 13U, 0);
}

/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
void initSignalControl(char *signalEnter, char *signalUp, char *signalDown, char *signalCacel)
{
	buttonEnter = signalEnter;
	buttonUp = signalUp;
	buttonDown = signalDown;
	buttonCancel = signalCacel;
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

void setMaxShowedMenu(char _maxShowedMenu)
{
	maxShowedMenu = _maxShowedMenu;
}
/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
void send_1(void)
{
	GPIO_PinWrite(GPIOD, 15U, 1);
	GPIO_PinWrite(GPIOD, 13U, 1);
        delay(1);
	GPIO_PinWrite(GPIOD, 13U, 0);
}


/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
void send_0(void)
{
	GPIO_PinWrite(GPIOD, 15U, 0);
	GPIO_PinWrite(GPIOD, 13U, 1);
        delay(1);
	GPIO_PinWrite(GPIOD, 13U, 0);
}

/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
char read_bit(void)
{
	char value_bit;
	GPIO_PinWrite(GPIOD, 13U, 1);
	value_bit = GPIO_PinRead(GPIOD, 15U);
	GPIO_PinWrite(GPIOD, 13U, 0);
	return value_bit;
}


/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
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


/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
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

/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
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

/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
void GraphicMode ()   
{
//		sendCommand(0x30);  // 8 bit mode
//		delay (10);
//		sendCommand(0x34);  // switch to Extended instructions
//		delay (10);
		sendCommand(0x3E);  // enable graphics
		delay (10);
}

/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
void HOME()
{
    sendCommand(0x02);
}

/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
void CLEAR()
{
    sendCommand(0x20);                  //Off Graphic Mode
    sendCommand(0x01);
}

/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
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


/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
void ENTRY_MODE_SET()
{
    sendCommand(0x04);
}


/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
void Display(display_t Type_display)
{
    sendCommand(Type_display);
}


/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
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

/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
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



/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
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


/*Menu tree-----------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////

MenuItem_t *root;


/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
void show_menu_new()
{
  HOME();
  sendString("                ");
  HOME();
  sendString(root->name);
  for(unsigned char i=2; i<=maxShowedMenu+1; i++)
    {
      SelectLine(i);
      sendData(' ');
    }
  if(root->child!=NULL)
  {
    SelectLine(arrowPos+1);
    sendData(0x1A);
    MenuItem_t *cusor1;
    cusor1 = root->child;
    char i;
    if (menuPos-arrowPos +1 ==1) 
    {
        setPos(2,1);
        sendString("                ");
        setPos(2,1);
        sendString(cusor1->name);
        cusor1 = cusor1->brother;
        i=2;
    }
    else
    {
        for(char i =1; i< menuPos-arrowPos +1;i++)
        {
          cusor1 = cusor1 -> brother; 
        }
        i=1;
    }
    while(cusor1!= NULL&& i<=maxShowedMenu)
        {
          
          i++;
          setPos(i,1);
          sendString("              ");
          setPos(i,1);
          sendString(cusor1->name);//Lan luot hien ten cua M sub_menu from (vi tri hien tai - vi tri mui ten)
          cusor1 = cusor1->brother;
        }
    for (char j=i; j<maxShowedMenu+1;j++)   //Xoa nhung dong k dung den
        {
          setPos(j+1,1);
          sendString("              ");
        }
  }
}

/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
void browse_new(MenuItem_t *rootMenu)
{
  if(root==NULL) root = rootMenu;
  show_menu_new();
  if (BUTTON_UP == ON)
  {
    BUTTON_UP = OFF;
    if(menuPos>1)
    {
      menuPos--;
      arrowPos--;
      if (menuPos >1) arrowPos =2;
    }
  }
  if (BUTTON_DOWN == ON)
  {
    BUTTON_DOWN = OFF;
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
        arrowPos++;
        if (menuPos<cusor2->parent->number) arrowPos=maxShowedMenu-1;
      }
    }
  }
  if (BUTTON_ENTER == ON)
  {
    BUTTON_ENTER =OFF;
    if (root ->child!= NULL)
    {
      root = root -> child;
      for(unsigned char i=1;i<menuPos; i++)
      {
        root = root->brother;
      }
      if (root -> child ==NULL)
      {
        //root ->fnCallback;
        root = root ->parent;
      }
      else 
      {
        menuPos =1;
        arrowPos =1;
      }
    }
  }
  if (BUTTON_CANCEL == ON)
  {
    BUTTON_CANCEL = OFF;
    if(root -> parent!=NULL)
    {
      root = root -> parent;
    }
    menuPos=1;
    arrowPos =1;
  }
}

/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
void Add(MenuItem_t *root1, MenuItem_t *child1)
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
  root1->number ++;

}


/*
MenuEntry Menu[]=
{
  {"Main", 1, 2, 3, 4, 0},                //0
  {"Sub1", 5, 5, 1, 1, 0},                //1
  {"Sub2", 2, 2, 2, 2, 0},                //2
  {"Sub3", 3, 3, 3, 3, 0},                //3
  {"Sub4", 4, 4, 4, 4, 0},                 //4
  {"Sub1_1",5, 5, 5, 5, 0}
};*/

/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
/*void ShowMenu()
{
  HOME();
  //CLEAR();
  sendString(Menu[selected].text);
  if(selected == 0)
  {
      SelectLine(2);
      sendString("Press OR HOLD");
      SelectLine(3);
      sendString("1 OR 2");
  }
  if(selected == 1)
  {
      SelectLine(2);
      sendString("Pressed1");
      SelectLine(3);
      sendString("Press 0 to back");
  }
  if(selected == 2)
  {
      SelectLine(2);
      sendString("Pressed2");
      SelectLine(3);
      sendString("Press 0 to back");
  }
  if(selected == 3)
  {
      SelectLine(2);
      sendString("Holded1");
      SelectLine(3);
      sendString("Press 0 to back");
  }
  if(selected == 4)
  {
      SelectLine(2);
      sendString("Holded2");
      SelectLine(3);
      sendString("Press 0 to back");
  }
}
*/
/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
/*void BrowseMenu()
{
    ShowMenu();
    if(PRESS1)
    {
      selected = Menu[selected].press1;
      PRESS1 =0;
    }
    if(PRESS2)
    {
      selected = Menu[selected].press2;
      PRESS2=0;
    }
    if(HOLD1)
    {
      selected = Menu[selected].hold1;
      HOLD1 = 0;
    }
    if(HOLD2)
    {
      selected = Menu[selected].hold2;
      HOLD2=0;
    }
    if(PRESS0)
    {
      selected = Menu[selected].press0;
      PRESS0=0;
    }
    
}*/
                        
/*End Menu tree*/