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
  
#ifndef __ST7920_SERIAL_H__
#define __ST7920_SERIAL_H__

/*********************************************************************************
 * INCLUDE
 */
#include "fsl_gpio.h"

#define NUM_ROWS                                        64
#define	NUM_COLS                                        128

#define LCD_CS_PORT                    GPIOD//                lcdPins->pinCS->port
#define LCD_DATA_PORT                  GPIOD//                lcdPins->pinDATA->port
#define LCD_CLK_PORT                   GPIOD//               lcdPins->pinCLK->port    
    
#define LCD_CS_PIN                     14U//                lcdPins->pinCS->pin
#define LCD_DATA_PIN                   15U//                lcdPins->pinDATA->pin
#define LCD_CLK_PIN                    13U//               lcdPins->pinCLK->pin

/*********************************************************************************
 * TYPEDEFS
 */

typedef struct{
  GPIO_Type *port;
  uint32_t  pin;
}lcd_pin_t;

typedef struct{
  lcd_pin_t     *pinCS;
  lcd_pin_t     *pinDATA;
  lcd_pin_t     *pinCLK;
}lcd_pins_t;

typedef enum{
	DISABLE = 0,                                        /*!<                       */
	ENABLE = 1,                                         /*!<                       */
}graphic_state_t;

/*********************************************************************************
 * EXPORTED FUNCTION
 */

// Send the command to the LCD
void LCD_SendCmd(uint8_t cmd);

// send the data to the LCD
void LCD_SendData (uint8_t data);

// send the string to the LCD
void LCD_SendString(char* string);

/* ENABLE or DISABLE the graphic mode
 * enable =1 --> graphic mode enabled
 */
void LCD_GraphicMode (graphic_state_t state);

// clear screen in any mode
void LCD_Clear();

// Draw bitmap on the display
void LCD_DrawBitmap(const unsigned char* graphic);

// Update the display with the selected graphics
void LCD_Update(void);

// Initialize the display
//void LCD_Init (lcd_pins_t *_lcdPins);
void LCD_Init (void);

/* Common functions used
 * in other LCDs also
 */
void LCD_SelectLine(unsigned char line);

void LCD_SetPos(unsigned int x, int y);

// Set a pixel on the display
void LCD_SetPixel(uint8_t x, uint8_t y);

// draw line from (X0, Y0) to (X1, Y1)
void LCD_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

// draw rectangle from (X,Y) w- width, h- height
void LCD_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

// draw filled rectangle
void LCD_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

// draw circle with centre (X0, Y0) and radius= radius
void LCD_DrawCircle(uint8_t x0, uint8_t y0, uint8_t radius);

// Draw Filled Circle with centre (X0, Y0) and radius= r
void LCD_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r);

// Draw Traingle with coordimates (x1, y1), (x2, y2), (x3, y3)
void LCD_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3);

// Draw Filled Traingle with coordimates (x1, y1), (x2, y2), (x3, y3)
void LCD_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3);
#endif  /* __ST7920_SERIAL_H__ */