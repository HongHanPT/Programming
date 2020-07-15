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
  
#include "lcd_st7920_serial.h"


uint8_t startRow, startCol, endRow, endCol; // coordinates of the dirty rectangle
graphic_state_t graphicCheck = DISABLE;
uint8_t image[(128 * 64)/8];

static char arrowPos =1 ,menuPos =1;
static char *buttonEnter, *buttonUp, *buttonDown, *buttonCancel;
static char maxShowedMenu=3;
static lcd_pins_t *lcdPins;
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

static void lcd_send_bit(char bit)
{
	GPIO_PinWrite(LCD_DATA_PORT, LCD_DATA_PIN, bit);         //DATA bit
	GPIO_PinWrite(LCD_CLK_PORT, LCD_CLK_PIN, 1);		//CLK PIN RESET
        delay(1);
	GPIO_PinWrite(LCD_CLK_PORT, LCD_CLK_PIN, 0);		//CLK PIN SET
}

static void lcd_send_byte(char byte)
{
	for (int i=7;i>=0;i--) 
          {
            lcd_send_bit((byte >> i)&0x01);
          }
}


// Send the command to the LCD
void LCD_SendCmd(uint8_t cmd)
{
//        GPIO_PinWrite(LCD_CS_PORT, LCD_CS_PIN, 1);    //CS Pin
	lcd_send_bit(1);
	lcd_send_bit(1);
	lcd_send_bit(1);
	lcd_send_bit(1);
	lcd_send_bit(1);
	lcd_send_bit(0);		//RW bit
	lcd_send_bit(0);		//RS bit
	lcd_send_bit(0);
	//D7:D4
	for (unsigned char i=7;i>=4;i--) 
          {
		lcd_send_bit((cmd >> i)&0x01);
          }
	lcd_send_bit(0);
	lcd_send_bit(0);
	lcd_send_bit(0);
	lcd_send_bit(0);
	//D3:D0
	for (int i=3;i>=0;i--) 
          {
            lcd_send_bit((cmd >> i)&0x01);
          }
	lcd_send_bit(0);
	lcd_send_bit(0);
	lcd_send_bit(0);
	lcd_send_bit(0);
//        GPIO_PinWrite(LCD_CS_PORT, LCD_CS_PIN, 0);    //CS Pin
}

// send the data to the LCD
void LCD_SendData (uint8_t data)
{
//        GPIO_PinWrite(LCD_CS_PORT, LCD_CS_PIN, 1);    //CS Pin
	lcd_send_bit(1);
	lcd_send_bit(1);
	lcd_send_bit(1);
	lcd_send_bit(1);
	lcd_send_bit(1);
	lcd_send_bit(0);		//RW bit
	lcd_send_bit(1);		//RS bit
	lcd_send_bit(0);
	//D7:D4
	for (unsigned char i=7;i>=4;i--) 
          {
		lcd_send_bit((data >> i)&0x01);
          }
	lcd_send_bit(0);
	lcd_send_bit(0);
	lcd_send_bit(0);
	lcd_send_bit(0);
	//D3:D0
	for (int i=3;i>=0;i--) 
          {
            lcd_send_bit((data >> i)&0x01);
          }
	lcd_send_bit(0);
	lcd_send_bit(0);
	lcd_send_bit(0);
	lcd_send_bit(0);
//        GPIO_PinWrite(LCD_CS_PORT, LCD_CS_PIN, 0);    //CS Pin
}

// send the string to the LCD
void LCD_SendString(char* string)
{
	unsigned char i = 0;
	while (string[i])
	{
		LCD_SendData(string[i]);
		i++;
	}
}

/* ENABLE or DISABLE the graphic mode
 * enable =1 --> graphic mode enabled
 */
void LCD_GraphicMode(graphic_state_t state)
{
	if (state == ENABLE)
	{
		// ST7920_SendCmd(0x30);  // 8 bit mode
		// HAL_Delay (1);
		// ST7920_SendCmd(0x34);  // switch to Extended instructions
		// HAL_Delay (1);
		LCD_SendCmd(0x3E);  // enable graphics
		delay(10);
		graphicCheck = ENABLE;  // update the variable
	}

	else if (state == DISABLE)
	{
		LCD_SendCmd(0x30);  // 8 bit mode
		delay(1);
		graphicCheck = DISABLE;  // update the variable
	}
}
// clear screen in any mode
void LCD_Clear()
{
	if (graphicCheck == DISABLE)
	{
		LCD_SendCmd(0x01);
	}
	else if (graphicCheck == ENABLE)
	{
		uint8_t x, y;
		for(y = 0; y < 64; y++)
		{
				if(y < 32)
				{
						LCD_SendCmd(0x80 | y);
						LCD_SendCmd(0x80);
				}
				else
				{
						LCD_SendCmd(0x80 | (y-32));
						LCD_SendCmd(0x88);
				}
				for(x = 0; x < 8; x++)
				{
						LCD_SendData(0);
						LCD_SendData(0);
				}
		}
	}
}

// Draw bitmap on the display
void LCD_DrawBitmap(const unsigned char* graphic)
{
	uint8_t x, y;
	for(y = 0; y < 64; y++)
	{
		if(y < 32)
		{
			for(x = 0; x < 8; x++)				// Draws top half of the screen.
			{					        // In extended instruction mode, vertical and horizontal coordinates must be specified before sending data in.
				LCD_SendCmd(0x80 | y);			// Vertical coordinate of the screen is specified first. (0-31)
				LCD_SendCmd(0x80 | x);			// Then horizontal coordinate of the screen is specified. (0-8)
				LCD_SendData(graphic[2*x + 16*y]);		// Data to the upper byte is sent to the coordinate.
				LCD_SendData(graphic[2*x+1 + 16*y]);	// Data to the lower byte is sent to the coordinate.
			}
		}
		else
		{
			for(x = 0; x < 8; x++)				// Draws bottom half of the screen.
			{						// Actions performed as same as the upper half screen.
				LCD_SendCmd(0x80 | (y-32));		// Vertical coordinate must be scaled back to 0-31 as it is dealing with another half of the screen.
				LCD_SendCmd(0x88 | x);
				LCD_SendData(graphic[2*x + 16*y]);
				LCD_SendData(graphic[2*x+1 + 16*y]);
			}
		}
	}
}

// Update the display with the selected graphics
void LCD_Update(void);



// Initialize the display
//void LCD_Init (lcd_pins_t *_lcdPins)
//{
//    lcdPins = _lcdPins;
//}
void LCD_Init ()
{
    GPIO_PinWrite(LCD_CS_PORT, LCD_CS_PIN, 1);
    GPIO_PinWrite(LCD_CLK_PORT, LCD_CLK_PIN, 0);
}

/* Common functions used
 * in other LCDs also
 */

void LCD_SelectLine(unsigned char line)
{
  if(line == 2) LCD_SendCmd(0x90);
  else if (line ==3) LCD_SendCmd(0x88);
  else if (line ==4) LCD_SendCmd(0x98);
  else if (line == 1) LCD_SendCmd(0x80);
}

void LCD_SetPos(unsigned int x, int y)
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
  LCD_SendCmd(instruction);
}


// Set a pixel on the display
void LCD_SetPixel(uint8_t p_x, uint8_t p_y)
{
	if (p_y < NUM_ROWS && p_x < NUM_COLS)
	{
		uint8_t *p = image + ((p_y * (NUM_COLS/8)) + (p_x/8));
		*p |= 0x80u >> (p_x%8);

		*image = *p;

		// Change the dirty rectangle to account for a pixel being dirty (we assume it was changed)
		if (startRow > p_y) { startRow = p_y; }
		if (endRow <= p_y)  { endRow = p_y + 1; }
		if (startCol > p_x) { startCol = p_x; }
		if (endCol <= p_x)  { endCol = p_x + 1; }
	}

}


// draw line from (X0, Y0) to (X1, Y1)
void LCD_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	int dx = (x1 >= x0) ? x1 - x0 : x0 - x1;
	int dy = (y1 >= y0) ? y1 - y0 : y0 - y1;
	int sx = (x0 < x1) ? 1 : -1;
	int sy = (y0 < y1) ? 1 : -1;
	int err = dx - dy;

	for (;;)
	{
		LCD_SetPixel(x0, y0);
		if (x0 == x1 && y0 == y1) break;
		int e2 = err + err;
		if (e2 > -dy)
		{
			err -= dy;
			x0 += sx;
		}
		if (e2 < dx)
		{
			err += dx;
			y0 += sy;
		}
	}
}


// draw rectangle from (X,Y) w- width, h- height
void LCD_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	/* Check input parameters */
	if (x >= NUM_COLS || y >= NUM_ROWS) 
	{
		/* Return error */
		return;
	}

	/* Check width and height */
	if ((x + w) >= NUM_COLS) {
		w = NUM_COLS - x;
	}
	if ((y + h) >= NUM_ROWS) {
		h = NUM_ROWS - y;
	}

	/* Draw 4 lines */
	LCD_DrawLine(x, y, x + w, y);         /* Top line */
	LCD_DrawLine(x, y + h, x + w, y + h); /* Bottom line */
	LCD_DrawLine(x, y, x, y + h);         /* Left line */
	LCD_DrawLine(x + w, y, x + w, y + h); /* Right line */
}


// draw filled rectangle
void LCD_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	uint8_t i;

	/* Check input parameters */
	if (x >= NUM_COLS || y >= NUM_ROWS)
	{
		/* Return error */
		return;
	}

	/* Check width and height */
	if ((x + w) >= NUM_COLS) {
		w = NUM_COLS - x;
	}
	if ((y + h) >= NUM_ROWS) {
		h = NUM_ROWS - y;
	}

	/* Draw lines */
	for (i = 0; i <= h; i++) {
		/* Draw lines */
		LCD_DrawLine(x, y + i, x + w, y + i);
	}
}



// draw circle with centre (X0, Y0) and radius= radius
void LCD_DrawCircle(uint8_t x0, uint8_t y0, uint8_t radius)
{
	int f = 1 - (int)radius;
	int ddF_x = 1;

	int ddF_y = -2 * (int)radius;
	int x = 0;

	LCD_SetPixel(x0, y0 + radius);
	LCD_SetPixel(x0, y0 - radius);
	LCD_SetPixel(x0 + radius, y0);
	LCD_SetPixel(x0 - radius, y0);

	int y = radius;
	while(x < y)
	{
		if(f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		LCD_SetPixel(x0 + x, y0 + y);
		LCD_SetPixel(x0 - x, y0 + y);
		LCD_SetPixel(x0 + x, y0 - y);
		LCD_SetPixel(x0 - x, y0 - y);
		LCD_SetPixel(x0 + y, y0 + x);
		LCD_SetPixel(x0 - y, y0 + x);
		LCD_SetPixel(x0 + y, y0 - x);
		LCD_SetPixel(x0 - y, y0 - x);
	}
}



// Draw Filled Circle with centre (X0, Y0) and radius= r

void LCD_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	LCD_SetPixel(x0, y0 + r);
	LCD_SetPixel(x0, y0 - r);
	LCD_SetPixel(x0 + r, y0);
	LCD_SetPixel(x0 - r, y0);
	LCD_DrawLine(x0 - r, y0, x0 + r, y0);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		LCD_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y);
		LCD_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y);

		LCD_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x);
		LCD_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x);
	}
}



// Draw Traingle with coordimates (x1, y1), (x2, y2), (x3, y3)
void LCD_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3)
{
	/* Draw lines */
	LCD_DrawLine(x1, y1, x2, y2);
	LCD_DrawLine(x2, y2, x3, y3);
	LCD_DrawLine(x3, y3, x1, y1);
}



// Draw Filled Traingle with coordimates (x1, y1), (x2, y2), (x3, y3)
void LCD_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3)
{
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
	curpixel = 0;

	#define ABS(x)   ((x) > 0 ? (x) : -(x))

	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay){
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++)
	{
		LCD_DrawLine(x, y, x3, y3);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
		}
}

////////////////////////////////////////////////////////////////////////////////
static MenuItem_t *root;

void LCD_SetMaxShowedMenu(char _maxShowedMenu)
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

void LCD_InitSignalControl(char *signalEnter, char *signalUp, char *signalDown, char *signalCacel)
{
	buttonEnter = signalEnter;
	buttonUp = signalUp;
	buttonDown = signalDown;
	buttonCancel = signalCacel;
}

static void LCD_ShowMenu()
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

/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
void LCD_BrowseMenu(MenuItem_t *rootMenu, update_browse_t isUpdateBrowse)
{
  if((root==NULL)|| (isUpdateBrowse == YES_UPDATE)) root = rootMenu;
  if (BUTTON_UP == ON)
  {
      BUTTON_UP = OFF;
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
              if (root ->fnCallback == NULL)
              {
                  root = root ->parent;
              }
              else root ->fnCallback();
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
  
  LCD_ShowMenu();
}

/**
 * @brief    Initializes modbus with according to parameters specified in structure
 * @note
 * 
 * @param    NONE
 * @retval   LCD status 
 */
void LCD_AddSubMenu2Menu(MenuItem_t *root1, MenuItem_t *child1)
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