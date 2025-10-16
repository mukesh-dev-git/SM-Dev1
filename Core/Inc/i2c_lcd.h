/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : i2c_lcd.h
  * @brief          : Header for I2C LCD (HD44780 with PCF8574 I2C backpack)
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef INC_I2C_LCD_H_
#define INC_I2C_LCD_H_

#include "stm32l4xx_hal.h"

/* LCD I2C address - common addresses are 0x27 or 0x3F */
#define LCD_I2C_ADDR    0x27 << 1  // Left shift for HAL I2C functions

/* LCD dimensions */
#define LCD_ROWS        2
#define LCD_COLS        16

/* Commands */
#define LCD_CLEAR       0x01
#define LCD_HOME        0x02
#define LCD_ENTRY_MODE  0x04
#define LCD_DISPLAY_CTL 0x08
#define LCD_CURSOR_SHIFT 0x10
#define LCD_FUNCTION_SET 0x20
#define LCD_SETCGRAM    0x40
#define LCD_SETDDRAM    0x80

/* Display entry mode */
#define LCD_ENTRY_RIGHT 0x00
#define LCD_ENTRY_LEFT  0x02
#define LCD_ENTRY_SHIFT_INC 0x01
#define LCD_ENTRY_SHIFT_DEC 0x00

/* Display on/off control */
#define LCD_DISPLAY_ON  0x04
#define LCD_DISPLAY_OFF 0x00
#define LCD_CURSOR_ON   0x02
#define LCD_CURSOR_OFF  0x00
#define LCD_BLINK_ON    0x01
#define LCD_BLINK_OFF   0x00

/* Function set */
#define LCD_8BIT_MODE   0x10
#define LCD_4BIT_MODE   0x00
#define LCD_2_LINE      0x08
#define LCD_1_LINE      0x00
#define LCD_5x10_DOTS   0x04
#define LCD_5x8_DOTS    0x00

/* Backlight */
#define LCD_BACKLIGHT   0x08
#define LCD_NOBACKLIGHT 0x00

/* Enable bit */
#define LCD_EN          0x04  // Enable bit
#define LCD_RW          0x02  // Read/Write bit
#define LCD_RS          0x01  // Register select bit

/* Function prototypes */
void LCD_Init(I2C_HandleTypeDef *hi2c);
void LCD_Clear(void);
void LCD_Home(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_Print(char *str);
void LCD_PrintChar(char c);
void LCD_Backlight(uint8_t state);
void LCD_Display(uint8_t state);

#endif /* INC_I2C_LCD_H_ */

