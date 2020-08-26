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
 
/*********************************************************************************
 * INCLUDE
 */

#include "lcd_st7920_serial.h"


/*********************************************************************************
 *  VARIBLES
 */

uint8_t startRow, startCol, endRow, endCol; // coordinates of the dirty rectangle
graphic_state_t graphicCheck = DISABLE;
uint8_t image[(128 * 64)/8];
//static lcd_pins_t *lcdPins;
extern const uint8_t g_vnfont_8x15[];
extern uint16_t g_vnfontFindPosition(uint16_t c);
/*********************************************************************************
 * EXPORTED FUNCTION
 */

static void delay(unsigned int time)
{
    volatile uint32_t i = 0;
    for (unsigned int cnt = 0; cnt<=time;cnt++)
    {
      for (i = 0; i < 20; ++i)
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

//static void lcd_send_byte(char byte)
//{
//	for (int i=7;i>=0;i--) 
//          {
//            lcd_send_bit((byte >> i)&0x01);
//          }
//}

/**
 * @brief    Send the command to the LCD
 * @note
 * 
 * @param    cmd Command
 */
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

/**
 * @brief    Send the data to the LCD
 * @note
 * 
 * @param    data Data
 */
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


/**
 * @brief    Send the string to the LCD
 * @note
 * 
 * @param    string String pointer
 */
void LCD_SendString(char* string)
{
	unsigned char i = 0;
	while (string[i])
	{
		LCD_SendData(string[i]);
		i++;
	}
}


/**
 * @brief    ENABLE or DISABLE the graphic mode
 * @note     state = ENABLE --> graphic mode enabled
 * 
 * @param    state graphic mode state
 */
void LCD_GraphicMode(graphic_state_t state)
{
	if (state == ENABLE)
	{
		LCD_SendCmd(0x30);  // 8 bit mode
		delay (1);
		LCD_SendCmd(0x34);  // switch to Extended instructions
		delay (1);
		LCD_SendCmd(0x3E);  // enable graphics
		delay(30);
		graphicCheck = ENABLE;  // update the variable
	}

	else if (state == DISABLE)
	{
//                LCD_SendCmd(0x3E);
//                delay(1);
		LCD_SendCmd(0x30);  // 8 bit mode
		delay(5);
		graphicCheck = DISABLE;  // update the variable
	}
}

/**
 * @brief    Clear screen in any mode
 * @note
 *
 */
void LCD_Clear()
{
	if (graphicCheck == DISABLE)
	{
		LCD_SendCmd(0x01);
                delay(100);
	}
	else if (graphicCheck == ENABLE)
	{
                for(uint16_t i=0;  i < (128 * 64)/8; i++)
                {
                  image[i]=0x00;
                  //memset
                }
                LCD_Update();
                
	}
        
}


/**
 * @brief    Draw bitmap on the display
 * @note
 * 
 * @param    garaphic The pointer of the graphic array is converted to the format bitmap
 */
void LCD_DrawBitmap(const unsigned char* graphic)
{
	uint8_t x, y;
	for(y = 0; y < 64; y++)
	{
		if(y < 32)
		{
			for(x = 0; x < 8; x++)				// Draws top half of the screen.
			{					        // In extended instruction mode, vertical and horizontal coordinates must be specified before sending data in.
				LCD_SendCmd(0x80|y);			// Vertical coordinate of the screen is specified first. (0-31)
				LCD_SendCmd(0x80|x);			// Then horizontal coordinate of the screen is specified. (0-8)
				LCD_SendData(graphic[2*x + 16*y]);		// Data to the upper byte is sent to the coordinate.
				LCD_SendData(graphic[2*x+1 + 16*y]);	// Data to the lower byte is sent to the coordinate.
			}
                        //delay(5);
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
                        //delay(5);
		}
	}
}

/**
 * @brief    Update the display with the selected graphics
 * @note
 * 
 * @param    None
 * @retval   None
 */
void LCD_Update(void)
{
    LCD_DrawBitmap(image);
}

void LCD_SetFont(sFONT *fonts)
{

}


/**
 * @brief    Write to RAM
 * @note     Xpos < 128
             Ypos < 64
 * 
 * @param    Xpos, Yos Coordinate to draw char
 * @retval   None
 */
void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, uint8_t width, uint8_t *c)
{
  uint16_t _Xpos = Xpos, _Ypos = Ypos-1;
  uint8_t value;
	for (uint8_t _py = 0; _py < 8; _py++)  //Dong thu py
        {
          for (uint8_t i=0; i< width; i++)
          {
            value = ((c[i])>> _py)&0x01;  //Lay lan luot gia tri bit o cot i, tren dong py
            _Xpos= Xpos + i;
            if(value){
              setBit(image, _Xpos, _Ypos);}
            else clearBit(image, _Xpos, _Ypos);
            
            value = ((uint8_t)(c[i+width])>>_py)&0x01;  //Lay lan luot gia tri bit o cot i, tren dong py
            if(value){
              setBit(image, _Xpos, _Ypos+8);}
            else clearBit(image, _Xpos, _Ypos+8);
            
          }
          _Ypos++;
        }
 // LCD_Update();
}

/**
 * @brief    Write to RAM
 * @note     Line<4 
 * @param    
 * @retval   None
 */
void LCD_DisplayStringLine(uint8_t Line, uint16_t *ptr)
{
    unsigned char index = 0;
    uint16_t postionChar=0;
    uint8_t widthChar=0;
    uint16_t _Xpos = 1, _Ypos = (Line-1)*16+1;
//    uint8_t *tempArray=ptr;
    while(ptr[index])
    {
      postionChar = g_vnfontFindPosition(*ptr);
      widthChar = g_vnfont_8x15[postionChar];
//      for(uint8_t index=0; index < 2*widthChar; index++)
//      {
//        *(tempArray+index) =1;
//        //*(tempArray+index) = *(g_vnfont_8x15 + postionChar + 1+ index);
//      }
      LCD_DrawChar(_Xpos, _Ypos, widthChar, (uint8_t*)g_vnfont_8x15+postionChar+1);
      _Xpos += widthChar+1;
      ptr++;
    }
   // LCD_Update();
    //delay(1000);
}
void LCD_DisplayStringLineWithPosition(uint16_t Line, uint16_t ColPos, uint8_t *ptr)
{
    unsigned char index = 0;
    uint8_t *tempArray;
    uint16_t postionChar;
    uint8_t widthChar;
    uint16_t _Xpos = ColPos, _Ypos = (Line-1)*16+1;
    while(ptr[index])
    {
      postionChar = g_vnfontFindPosition(*ptr);
      widthChar = g_vnfont_8x15[postionChar];
      for(uint8_t index=0; index < 2*widthChar; index++)
      {
        *(tempArray + index) = g_vnfont_8x15[postionChar + 1+ index];
      }
      LCD_DrawChar(_Xpos, _Ypos, widthChar, tempArray);
      _Xpos += widthChar;
      ptr++;
    }
    LCD_Update();
}

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

/**
 * @brief    Select display line
 * @note
 * 
 * @param    line Display line
 * @retval   None
 */
void LCD_SelectLine(unsigned char line)
{
  if(line == 2) LCD_SendCmd(0x90);
  else if (line ==3) LCD_SendCmd(0x88);
  else if (line ==4) LCD_SendCmd(0x98);
  else if (line == 1) LCD_SendCmd(0x80);
}

/**
 * @brief    Select cursor possition
 * @note     Cusor for one 16x16 character or two 8x16 half-height characters
 * 
 * @param    x  possition x
 * @param    y  possition y
 * @retval   None
 */
void LCD_SetPos(unsigned int x, int y)
{
  int instruction =0;
  if (x>8) x=8;
  if (y>4) y=4;
  if (y == 1)
  {
      instruction = 128 + x;
  }
  else if (y==2)
  {
      instruction = 144 +x;
  }
  else if (y==3)
  {
      instruction = 136 +x;
  }
  else 
  {
      instruction = 152 +x;
  }
  LCD_SendCmd(instruction);
}


/**
 * @brief    Set a pixel on the display
 * @note     p_x < NUM_COLS = 128
 *           p_y < NUM_ROWS = 64
 * 
 * @param    p_x   coordinates x
 * @param    p_y   coordinates y
 * @retval   None
 */
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


/**
 * @brief    draw line from (X0, Y0) to (X1, Y1)
 * @note
 * 
 * @param    x0         coordinates x of 1st point
 * @param    y0         coordinates y of 1st point
 * @param    x1         coordinates x of 2nd point
 * @param    y1         coordinates y of 2nd point
 * @retval   None
 */
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

/**
 * @brief    draw rectangle
 * @note
 * 
 * @param    x         coordinates x of start point
 * @param    y         coordinates y of start point
 * @param    w          width of rectangle
 * @param    h         height of rectangle
 * @retval   None
 */
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

/**
 * @brief    draw filled rectangle
 * @note
 * 
 * @param    x         coordinates x of start point
 * @param    y         coordinates y of start point
 * @param    w         width of rectangle
 * @param    h         height of rectangle
 * @retval   None
 */
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


/**
 * @brief    draw circle
 * @note
 * 
 * @param    x0         coordinates x of centre point
 * @param    y0         coordinates y of centre point
 * @param    radius          radius
 * @retval   None
 */
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


/**
 * @brief    draw filled circle
 * @note
 * 
 * @param    x0         coordinates x of centre point
 * @param    y0         coordinates y of centre point
 * @param    radius     radius
 * @retval   None
 */
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


/**
 * @brief    Draw Traingle
 * @note
 * 
 * @param    x1         coordinates x of 1st point
 * @param    y1         coordinates y of 1st point
 * @param    x2         coordinates x of 2nd point
 * @param    y2         coordinates y of 2nd point
 * @param    x3         coordinates x of 3rd point
 * @param    y3         coordinates y of 3rd point
 * @retval   None
 */
void LCD_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3)
{
	/* Draw lines */
	LCD_DrawLine(x1, y1, x2, y2);
	LCD_DrawLine(x2, y2, x3, y3);
	LCD_DrawLine(x3, y3, x1, y1);
}


/**
 * @brief    Draw Filled Traingle
 * @note
 * 
 * @param    x1         coordinates x of 1st point
 * @param    y1         coordinates y of 1st point
 * @param    x2         coordinates x of 2nd point
 * @param    y2         coordinates y of 2nd point
 * @param    x3         coordinates x of 3rd point
 * @param    y3         coordinates y of 3rd point
 * @retval   None
 */
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