/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : i2c_lcd.c
  * @brief          : I2C LCD driver for HD44780 with PCF8574 I2C backpack
  ******************************************************************************
  */
/* USER CODE END Header */

#include "i2c_lcd.h"

static I2C_HandleTypeDef *lcd_i2c;
static uint8_t backlight_state = LCD_BACKLIGHT;

/**
  * @brief  Send data to LCD via I2C
  * @param  data: Data to send
  * @retval None
  */
static void LCD_SendInternal(uint8_t data)
{
    HAL_I2C_Master_Transmit(lcd_i2c, LCD_I2C_ADDR, &data, 1, 100);
}

/**
  * @brief  Send 4 bits to LCD
  * @param  data: 4-bit data to send
  * @retval None
  */
static void LCD_Send4Bits(uint8_t data)
{
    uint8_t byte = data | backlight_state;
    
    // Send with enable high
    LCD_SendInternal(byte | LCD_EN);
    HAL_Delay(1);
    
    // Send with enable low
    LCD_SendInternal(byte & ~LCD_EN);
    HAL_Delay(1);
}

/**
  * @brief  Send command or data to LCD
  * @param  data: Data/command byte
  * @param  mode: LCD_RS for data, 0 for command
  * @retval None
  */
static void LCD_SendByte(uint8_t data, uint8_t mode)
{
    uint8_t high_nibble = data & 0xF0;
    uint8_t low_nibble = (data << 4) & 0xF0;
    
    // Send high nibble
    LCD_Send4Bits(high_nibble | mode);
    
    // Send low nibble
    LCD_Send4Bits(low_nibble | mode);
}

/**
  * @brief  Send command to LCD
  * @param  cmd: Command byte
  * @retval None
  */
static void LCD_SendCommand(uint8_t cmd)
{
    LCD_SendByte(cmd, 0);
    if (cmd == LCD_CLEAR || cmd == LCD_HOME)
    {
        HAL_Delay(2);  // Clear and home commands need more time
    }
}

/**
  * @brief  Send data to LCD
  * @param  data: Data byte
  * @retval None
  */
static void LCD_SendData(uint8_t data)
{
    LCD_SendByte(data, LCD_RS);
}

/**
  * @brief  Initialize LCD
  * @param  hi2c: Pointer to I2C handle
  * @retval None
  */
void LCD_Init(I2C_HandleTypeDef *hi2c)
{
    lcd_i2c = hi2c;
    
    // Wait for LCD to power up
    HAL_Delay(50);
    
    // Initialization sequence for 4-bit mode
    LCD_Send4Bits(0x30);  // Function set: 8-bit mode
    HAL_Delay(5);
    
    LCD_Send4Bits(0x30);  // Function set: 8-bit mode
    HAL_Delay(1);
    
    LCD_Send4Bits(0x30);  // Function set: 8-bit mode
    HAL_Delay(1);
    
    LCD_Send4Bits(0x20);  // Function set: 4-bit mode
    HAL_Delay(1);
    
    // Configure LCD
    LCD_SendCommand(LCD_FUNCTION_SET | LCD_4BIT_MODE | LCD_2_LINE | LCD_5x8_DOTS);
    LCD_SendCommand(LCD_DISPLAY_CTL | LCD_DISPLAY_OFF);
    LCD_SendCommand(LCD_CLEAR);
    LCD_SendCommand(LCD_ENTRY_MODE | LCD_ENTRY_LEFT | LCD_ENTRY_SHIFT_DEC);
    LCD_SendCommand(LCD_DISPLAY_CTL | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF);
    
    HAL_Delay(2);
}

/**
  * @brief  Clear LCD display
  * @retval None
  */
void LCD_Clear(void)
{
    LCD_SendCommand(LCD_CLEAR);
}

/**
  * @brief  Return cursor to home position
  * @retval None
  */
void LCD_Home(void)
{
    LCD_SendCommand(LCD_HOME);
}

/**
  * @brief  Set cursor position
  * @param  row: Row number (0 or 1)
  * @param  col: Column number (0-15)
  * @retval None
  */
void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t row_offsets[] = {0x00, 0x40};
    
    if (row >= LCD_ROWS)
    {
        row = LCD_ROWS - 1;
    }
    
    if (col >= LCD_COLS)
    {
        col = LCD_COLS - 1;
    }
    
    LCD_SendCommand(LCD_SETDDRAM | (col + row_offsets[row]));
}

/**
  * @brief  Print string on LCD
  * @param  str: String to print
  * @retval None
  */
void LCD_Print(char *str)
{
    while (*str)
    {
        LCD_SendData(*str++);
    }
}

/**
  * @brief  Print single character on LCD
  * @param  c: Character to print
  * @retval None
  */
void LCD_PrintChar(char c)
{
    LCD_SendData(c);
}

/**
  * @brief  Control LCD backlight
  * @param  state: 1 for ON, 0 for OFF
  * @retval None
  */
void LCD_Backlight(uint8_t state)
{
    if (state)
    {
        backlight_state = LCD_BACKLIGHT;
    }
    else
    {
        backlight_state = LCD_NOBACKLIGHT;
    }
    
    LCD_SendInternal(backlight_state);
}

/**
  * @brief  Control LCD display
  * @param  state: 1 for ON, 0 for OFF
  * @retval None
  */
void LCD_Display(uint8_t state)
{
    if (state)
    {
        LCD_SendCommand(LCD_DISPLAY_CTL | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF);
    }
    else
    {
        LCD_SendCommand(LCD_DISPLAY_CTL | LCD_DISPLAY_OFF);
    }
}

