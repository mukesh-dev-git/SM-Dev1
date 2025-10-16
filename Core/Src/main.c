/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c_lcd.h"
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
// Wio-E5 Communication buffers
char wio_rx_buffer[256];        // Buffer for receiving data from Wio-E5
char wio_tx_buffer[128];        // Buffer for transmitting data to Wio-E5
uint16_t wio_rx_index = 0;      // Current position in RX buffer
uint8_t wio_response_ready = 0; // Flag indicating response is complete
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void WioE5_SendCommand(char* cmd);
void WioE5_ReceiveResponse(uint32_t timeout_ms);
void WioE5_ClearBuffer(void);
void LCD_DisplayResponse(char* response);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  
  // Add delay for stable power-up
  HAL_Delay(100);
  
  // Initialize LCD
  LCD_Init(&hi2c1);
  HAL_Delay(500);  // Give LCD time to initialize
  
  // Test LCD with simple message first
  LCD_Clear();
  HAL_Delay(100);
  
  LCD_SetCursor(0, 0);
  LCD_Print("LCD Init OK");
  HAL_Delay(1000);
  
  // Blink LED to show we're alive
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
  HAL_Delay(200);
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
  HAL_Delay(200);
  
  // Display startup message
  LCD_Clear();
  HAL_Delay(100);
  
  LCD_SetCursor(0, 0);
  LCD_Print("Wio-E5 Test");
  HAL_Delay(100);
  
  LCD_SetCursor(1, 0);
  LCD_Print("Starting...");
  HAL_Delay(2000);
  
  // ========== TEST 1: Basic AT Command ==========
  // Blink LED before each test
  HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
  
  LCD_Clear();
  HAL_Delay(50);
  
  LCD_SetCursor(0, 0);
  LCD_Print("Test 1: AT");
  HAL_Delay(1000);
  
  // Send AT command
  WioE5_SendCommand("AT");
  HAL_Delay(100);
  WioE5_ReceiveResponse(1000);
  
  // Show result
  LCD_Clear();
  HAL_Delay(50);
  
  LCD_SetCursor(0, 0);
  LCD_Print("Response:");
  HAL_Delay(50);
  
  LCD_SetCursor(1, 0);
  if (strlen(wio_rx_buffer) > 0) {
    LCD_Print("RX OK!");
  } else {
    LCD_Print("No Response");
  }
  HAL_Delay(3000);
  
  // Display actual response
  LCD_DisplayResponse(wio_rx_buffer);
  HAL_Delay(3000);
  
  // ========== TEST 2: Get Version ==========
  LCD_Clear();
  LCD_SetCursor(0, 0);
  LCD_Print("Test 2: VER");
  HAL_Delay(1000);
  
  WioE5_SendCommand("AT+VER");
  WioE5_ReceiveResponse(1000);
  
  LCD_DisplayResponse(wio_rx_buffer);
  HAL_Delay(3000);
  
  // ========== TEST 3: Get Device ID ==========
  LCD_Clear();
  LCD_SetCursor(0, 0);
  LCD_Print("Test 3: ID");
  HAL_Delay(1000);
  
  WioE5_SendCommand("AT+ID");
  WioE5_ReceiveResponse(1000);
  
  LCD_DisplayResponse(wio_rx_buffer);
  HAL_Delay(3000);
  
  // ========== TEST 4: Check Current Power ==========
  LCD_Clear();
  LCD_SetCursor(0, 0);
  LCD_Print("Test 4: POWER");
  HAL_Delay(1000);
  
  WioE5_SendCommand("AT+POWER");
  WioE5_ReceiveResponse(1000);
  
  LCD_DisplayResponse(wio_rx_buffer);
  HAL_Delay(3000);
  
  // ========== TEST 5: Set Power to 14 dBm ==========
  LCD_Clear();
  LCD_SetCursor(0, 0);
  LCD_Print("Set Power 14dBm");
  HAL_Delay(1000);
  
  WioE5_SendCommand("AT+POWER=14");
  WioE5_ReceiveResponse(1000);
  
  LCD_DisplayResponse(wio_rx_buffer);
  HAL_Delay(3000);
  
  // ========== TEST 6: Verify Power Setting ==========
  LCD_Clear();
  LCD_SetCursor(0, 0);
  LCD_Print("Verify Power");
  HAL_Delay(1000);
  
  WioE5_SendCommand("AT+POWER");
  WioE5_ReceiveResponse(1000);
  
  LCD_DisplayResponse(wio_rx_buffer);
  HAL_Delay(3000);
  
  // ========== TEST 7: Check Frequency Band ==========
  LCD_Clear();
  LCD_SetCursor(0, 0);
  LCD_Print("Test 7: BAND");
  HAL_Delay(1000);
  
  WioE5_SendCommand("AT+DR=SCHEME");
  WioE5_ReceiveResponse(1000);
  
  LCD_DisplayResponse(wio_rx_buffer);
  HAL_Delay(3000);
  
  // ========== TEST 7B: LoRaWAN Version/Region ==========
  LCD_Clear();
  LCD_SetCursor(0, 0);
  LCD_Print("LoRaWAN Region");
  HAL_Delay(1000);
  
  WioE5_SendCommand("AT+LW=VER");
  WioE5_ReceiveResponse(1000);
  
  LCD_DisplayResponse(wio_rx_buffer);
  HAL_Delay(3000);
  
  // ========== TEST 7C: Check Mode ==========
  LCD_Clear();
  LCD_SetCursor(0, 0);
  LCD_Print("Test 7C: MODE");
  HAL_Delay(1000);
  
  WioE5_SendCommand("AT+MODE");
  WioE5_ReceiveResponse(1000);
  
  LCD_DisplayResponse(wio_rx_buffer);
  HAL_Delay(3000);
  
  // ========== TEST 8: Set to LWOTAA Mode ==========
  LCD_Clear();
  LCD_SetCursor(0, 0);
  LCD_Print("Set LWOTAA Mode");
  HAL_Delay(1000);
  
  WioE5_SendCommand("AT+MODE=LWOTAA");
  WioE5_ReceiveResponse(1000);
  
  LCD_DisplayResponse(wio_rx_buffer);
  HAL_Delay(3000);
  
  // ========== TEST 9: Try JOIN (will fail without config) ==========
  LCD_Clear();
  LCD_SetCursor(0, 0);
  LCD_Print("Test JOIN");
  LCD_SetCursor(1, 0);
  LCD_Print("(May fail)");
  HAL_Delay(2000);
  
  WioE5_SendCommand("AT+JOIN");
  WioE5_ReceiveResponse(10000);  // Wait up to 10 seconds for join
  
  LCD_DisplayResponse(wio_rx_buffer);
  HAL_Delay(4000);
  
  // ========== TEST COMPLETE ==========
  LCD_Clear();
  LCD_SetCursor(0, 0);
  LCD_Print("Test Complete!");
  LCD_SetCursor(1, 0);
  LCD_Print("TX/RX Working");
  HAL_Delay(3000);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    
    // Toggle LED to show we're running
    HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
    
    // Cycle through different test commands
    
    // Command 1: AT (Communication Check)
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_Print("Loop: AT");
    WioE5_SendCommand("AT");
    WioE5_ReceiveResponse(1000);
    LCD_DisplayResponse(wio_rx_buffer);
    HAL_Delay(3000);
    
    // Command 2: Check Power Level
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_Print("Loop: POWER");
    WioE5_SendCommand("AT+POWER");
    WioE5_ReceiveResponse(1000);
    LCD_DisplayResponse(wio_rx_buffer);
    HAL_Delay(3000);
    
    // Command 3: Check Mode
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_Print("Loop: MODE");
    WioE5_SendCommand("AT+MODE");
    WioE5_ReceiveResponse(1000);
    LCD_DisplayResponse(wio_rx_buffer);
    HAL_Delay(3000);
    
    // Command 4: Check Frequency Band
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_Print("Loop: BAND");
    WioE5_SendCommand("AT+DR=SCHEME");
    WioE5_ReceiveResponse(1000);
    LCD_DisplayResponse(wio_rx_buffer);
    HAL_Delay(3000);
    
    // Show cycle complete
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_Print("Cycle Complete");
    LCD_SetCursor(1, 0);
    LCD_Print("Restarting...");
    HAL_Delay(2000);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00B07CB4;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LD3_Pin */
  GPIO_InitStruct.Pin = LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief  Send AT command to Wio-E5 module
  * @param  cmd: AT command string (without \r\n)
  * @retval None
  */
void WioE5_SendCommand(char* cmd)
{
    // Clear previous response
    WioE5_ClearBuffer();
    
    // Prepare command with CR+LF
    sprintf(wio_tx_buffer, "%s\r\n", cmd);
    
    // Send command via UART1
    HAL_UART_Transmit(&huart1, (uint8_t*)wio_tx_buffer, strlen(wio_tx_buffer), 1000);
    
    // Small delay to ensure command is sent
    HAL_Delay(10);
}

/**
  * @brief  Receive response from Wio-E5 module
  * @param  timeout_ms: Timeout in milliseconds
  * @retval None
  */
void WioE5_ReceiveResponse(uint32_t timeout_ms)
{
    uint8_t rx_byte;
    uint32_t start_tick = HAL_GetTick();
    
    wio_response_ready = 0;
    
    while ((HAL_GetTick() - start_tick) < timeout_ms)
    {
        // Try to receive one byte
        if (HAL_UART_Receive(&huart1, &rx_byte, 1, 10) == HAL_OK)
        {
            // Store byte in buffer
            if (wio_rx_index < sizeof(wio_rx_buffer) - 1)
            {
                wio_rx_buffer[wio_rx_index++] = rx_byte;
                
                // Check for end of response (usually ends with \r\n or OK/ERROR)
                if (rx_byte == '\n')
                {
                    // Null terminate
                    wio_rx_buffer[wio_rx_index] = '\0';
                    wio_response_ready = 1;
                    
                    // Check if we got a complete response (OK or ERROR)
                    if (strstr(wio_rx_buffer, "OK") || 
                        strstr(wio_rx_buffer, "ERROR") ||
                        strstr(wio_rx_buffer, "+"))
                    {
                        // Wait a bit more for any additional data
                        HAL_Delay(50);
                        break;
                    }
                }
            }
        }
    }
    
    // Ensure null termination
    wio_rx_buffer[wio_rx_index] = '\0';
}

/**
  * @brief  Clear Wio-E5 receive buffer
  * @retval None
  */
void WioE5_ClearBuffer(void)
{
    memset(wio_rx_buffer, 0, sizeof(wio_rx_buffer));
    memset(wio_tx_buffer, 0, sizeof(wio_tx_buffer));
    wio_rx_index = 0;
    wio_response_ready = 0;
}

/**
  * @brief  Display Wio-E5 response on LCD (parse and format)
  * @param  response: Response string from Wio-E5
  * @retval None
  */
void LCD_DisplayResponse(char* response)
{
    char line1[17] = {0};
    char line2[17] = {0};
    int len;
    
    LCD_Clear();
    
    // Parse response - remove \r\n and format for 16x2 display
    if (strlen(response) == 0)
    {
        LCD_SetCursor(0, 0);
        LCD_Print("No Response");
        return;
    }
    
    // Copy and clean up response
    char cleaned[256] = {0};
    int clean_idx = 0;
    
    for (int i = 0; i < strlen(response) && clean_idx < 255; i++)
    {
        if (response[i] != '\r' && response[i] != '\n')
        {
            cleaned[clean_idx++] = response[i];
        }
        else if (clean_idx > 0 && cleaned[clean_idx-1] != ' ')
        {
            cleaned[clean_idx++] = ' '; // Replace newlines with space
        }
    }
    cleaned[clean_idx] = '\0';
    
    // Display first 16 chars on line 1
    len = strlen(cleaned);
    if (len > 0)
    {
        strncpy(line1, cleaned, 16);
        line1[16] = '\0';
        LCD_SetCursor(0, 0);
        LCD_Print(line1);
    }
    
    // Display next 16 chars on line 2
    if (len > 16)
    {
        strncpy(line2, cleaned + 16, 16);
        line2[16] = '\0';
        LCD_SetCursor(1, 0);
        LCD_Print(line2);
    }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
