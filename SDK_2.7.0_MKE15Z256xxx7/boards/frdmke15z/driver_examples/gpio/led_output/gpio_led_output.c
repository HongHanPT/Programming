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

#include "GLCD.h"
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
unsigned char key[4][3] = {{'5','1','9'},{'7','3','#'},{'4','0','8'},{'6','2','*'}};
volatile uint32_t g_systickCounter=0;
char selected =0;
char PRESS1, PRESS2, HOLD1, HOLD2, PRESS0;
/*******************************************************************************
 * Code
 ******************************************************************************/
void SysTick_Handler(void)
{
    if (g_systickCounter != 0xFFFFFFFF)
    {
        g_systickCounter++;
    }
    else g_systickCounter = 0;
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
#define RELEASE_TICK        100     //

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
    unsigned char   	keyButton;		// 
    unsigned char   	last_keyButton;
    char	state;			        //
    void (*fnCallbackIDLE)(unsigned char key);               // 
    void (*fnCallbackPRESS)(unsigned char key);
    void (*fnCallbackHOLD)(unsigned char key);
    void (*fnCallbackRELEASE)(unsigned char key);


}BUTTON;

void ButtonProcessEvent(BUTTON *g_pButtons)
{
   // char nIndex;
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



/////////////////////////////////////////////////////////////////////////////////
/*
For LCD
*/
char UP, DOWN, ENTER, CANCEL;
/* ----------------------------------------
Hide 14/07/2020


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
//		sendCommand(0x30);  // 8 bit mode
//		delay (10);
//		sendCommand(0x34);  // switch to Extended instructions
//		delay (10);
		sendCommand(0x3E);  // enable graphics
		delay (10);
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
    sendCommand(0x20);                  //Off Graphic Mode
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




*Menu tree-----------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////
typedef struct Tree{
  char *text;
  char press1;
  char press2;
  char hold1;
  char hold2;
  char press0;
}MenuEntry;
char x, y=1, M = 3;
typedef struct  Mia_t MenuItem_t;
struct Mia_t
{
  char *name;
  char number;
  MenuItem_t *parent;
  MenuItem_t *child;
  MenuItem_t *brother;
  void(*fnCallback);
  //MenuItem_t av[];
};
MenuItem_t *root;


void show_menu_new()
{
  HOME();
  sendString("        ");
  HOME();
  sendString(root->name);
  for(unsigned char i=2; i<=M+1; i++)
    {
      SelectLine(i);
      sendData(' ');
    }
  if(root->child!=NULL)
  {
    SelectLine(vitri+1);
    sendData(0x1A);
    MenuItem_t *cusor1;
    cusor1 = root->child;
    char i;
    if (y-vitri +1 ==1) 
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
        for(char i =1; i< y-vitri +1;i++)
        {
          cusor1 = cusor1 -> brother; 
        }
        i=1;
    }
    while(cusor1!= NULL&& i<=M)
        {
          
          i++;
          setPos(i,1);
          sendString("              ");
          setPos(i,1);
          sendString(cusor1->name);//Lan luot hien ten cua cac menu con
          cusor1 = cusor1->brother;
        }
    for (char j=i; j<M+1;j++)   //Xoa nhung dong k dung den
        {
          setPos(j+1,1);
          sendString("              ");
        }
  }
}


void browse_new()
{
  show_menu_new();
  if (UP == 1)
  {
    UP=0;
    if(y>1)
    {
      y--;
      vitri--;
      if (y >1) vitri =2;
    }
  }
  if (DOWN == 1)
  {
    DOWN = 0;
//    if (y ==1) 
//    {
//      if(root ->child -> brother !=NULL)
//      {
//        y++;
//        vitri++;
//      }
//    }
//    else if (y>1)
    {
      MenuItem_t *cusor2;
      cusor2 = root -> child;
      for(unsigned char i=1;i<y; i++)
      {
        cusor2 = cusor2->brother;
      }
      if(cusor2 -> brother !=NULL)
      {
        y++;
        vitri++;
        if (y<cusor2->parent->number) vitri=M-1;
      }
    }
  }
  if (ENTER == 1)
  {
    ENTER =0;
    if (root ->child!= NULL)
    {
      root = root -> child;
      for(unsigned char i=1;i<y; i++)
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
        y =1;
        vitri =1;
      }
    }
  }
  if (CANCEL == 1)
  {
    CANCEL = 0;
    if(root -> parent!=NULL)
    {
      root = root -> parent;
    }
    y=1;
    vitri =1;
  }
}

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
    p= root1 -> child;
    while(p->brother!=NULL)
    {
      p= p-> brother;
    }
    p->brother = child1;
    child1 -> parent = root1;
  }
  root1->number ++;

}

MenuEntry Menu[]=
{
  {"Main", 1, 2, 3, 4, 0},                //0
  {"Sub1", 5, 5, 1, 1, 0},                //1
  {"Sub2", 2, 2, 2, 2, 0},                //2
  {"Sub3", 3, 3, 3, 3, 0},                //3
  {"Sub4", 4, 4, 4, 4, 0},                 //4
  {"Sub1_1",5, 5, 5, 5, 0}
};

void ShowMenu()
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

void BrowseMenu()
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
    
}
 */                       
/*End Menu tree*/

/*Test state machine*/
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
  }
    if (key == '0')
  {
    DOWN = 1;
  }
      if (key == '8')
  {
    UP = 1;
  }
  //browse_new();
}
void eventHoldA(unsigned char key)
{
  /* 09/07 */
  if(key)
  {
   HOME();
   sendData(key);
   sendData('+');
//    sendCommand(0x80||y);
//    sendCommand(0x80||x);
   //sendData(0xF8);
//    x++;
//    y++;
   // DrawBitmap(Untitled);
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
}
/* ------------------------------- */
/*!
 * @brief Main function
 */

MenuItem_t Main = {.name= "Main", .number =0}; 
MenuItem_t Menu1 = {.name= "Menu1", .number =0}; 
MenuItem_t Menu2 = {.name= "Menu2", .number =0}; 
MenuItem_t Menu11 = {.name= "A", .number =0}; 
MenuItem_t Menu12 = {.name= "B", .number =0}; 
MenuItem_t Menu13 = {.name= "C", .number =0}; 
MenuItem_t Menu14 = {.name= "D", .number =0}; 
MenuItem_t Menu21 = {.name= "C", .number =0}; 
MenuItem_t Menu22 = {.name= "D", .number =0}; 
MenuItem_t Menu111 = {.name= "E", .number =0}; 
MenuItem_t Menu121 = {.name= "F", .number =0};
MenuItem_t Menu122 = {.name= "G", .number =0}; 
//MenuItem_t *root;

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
    
    Init();
    Graphic_Off();
    sendData('0');
    CLEAR();
    Add(&Main, &Menu1);
   // uint8_t a = sizeof(Main);
    Add(&Main, &Menu2);
    Add(&Menu1, &Menu11);
    Add(&Menu1, &Menu12);
    Add(&Menu1, &Menu13);
    Add(&Menu1, &Menu14);
    Add(&Menu2, &Menu21);
    Add(&Menu2, &Menu22);
    Add(&Menu11, (MenuItem_t *)&Menu111);
    Add(&Menu12, (MenuItem_t *)&Menu121);
    Add(&Menu12, (MenuItem_t *)&Menu122);
    initSignalControl(&ENTER, &UP, &DOWN, &CANCEL);
    setMaxShowedMenu(3);
 //   GraphicMode();

    //CLEAR2();
//    HOME();
//    sendData('0');
    Display(DISPLAY_ON);
//    DrawBitmap(Untitled);
    BUTTON buttonA = 
    {
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
    //root = &Main;
//    sendCommand(0xA0);
//    sendCommand(0xA0);
    //CLEAR2();
//    browse_new();
    while (1)
    {
        //CLEAR();

       delay(1);
//        unsigned char key1=0;
       buttonA.keyButton = 0;
       buttonA.keyButton = check_key(); 
       ButtonProcessEvent(&buttonA);
//       if(check_key())
//       {
         browse_new(&Main);
//       }
       //BrowseMenu();
    }
}
