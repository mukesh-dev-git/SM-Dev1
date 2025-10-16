/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : wio_e5_commands.h
  * @brief          : Wio-E5 AT Command Reference
  ******************************************************************************
  * @attention
  *
  * Common AT commands for Seeed Studio Wio-E5 LoRaWAN module
  * 
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef INC_WIO_E5_COMMANDS_H_
#define INC_WIO_E5_COMMANDS_H_

/* Basic AT Commands */
#define WIO_CMD_TEST            "AT"                    // Test command
#define WIO_CMD_RESET           "AT+RESET"              // Reset module
#define WIO_CMD_VER             "AT+VER"                // Get firmware version
#define WIO_CMD_HELP            "AT+?"                  // Get help

/* LoRaWAN Configuration Commands */
#define WIO_CMD_MODE_LWOTAA     "AT+MODE=LWOTAA"        // Set OTAA mode
#define WIO_CMD_MODE_LWABP      "AT+MODE=LWABP"         // Set ABP mode
#define WIO_CMD_MODE_TEST       "AT+MODE=TEST"          // Set test mode

/* Device Identification */
#define WIO_CMD_ID              "AT+ID"                 // Get all IDs
#define WIO_CMD_ID_DEVEUI       "AT+ID=DevEui"          // Get DevEUI
#define WIO_CMD_ID_APPEUI       "AT+ID=AppEui"          // Get AppEUI
#define WIO_CMD_ID_DEVADDR      "AT+ID=DevAddr"         // Get DevAddr

/* LoRaWAN Keys (for OTAA) */
// Set AppEUI: AT+ID=AppEui,"0000000000000000"
// Set AppKey: AT+KEY=APPKEY,"00000000000000000000000000000000"

/* LoRaWAN Keys (for ABP) */
// Set Network Session Key: AT+KEY=NWKSKEY,"00000000000000000000000000000000"
// Set App Session Key: AT+KEY=APPSKEY,"00000000000000000000000000000000"

/* LoRaWAN Parameters */
#define WIO_CMD_DR              "AT+DR"                 // Get/Set Data Rate
#define WIO_CMD_DR_SCHEME       "AT+DR=SCHEME"          // Get frequency scheme/band
#define WIO_CMD_CH              "AT+CH"                 // Get/Set Channel
#define WIO_CMD_CH_NUM          "AT+CH=NUM"             // Get number of channels
#define WIO_CMD_POWER           "AT+POWER"              // Get/Set TX Power
#define WIO_CMD_PORT            "AT+PORT"               // Get/Set Port
#define WIO_CMD_CLASS           "AT+CLASS"              // Get/Set Class (A/B/C)
#define WIO_CMD_LW_VER          "AT+LW=VER"             // Get LoRaWAN version and region

/* LoRaWAN Network Commands */
#define WIO_CMD_JOIN            "AT+JOIN"               // Join network
#define WIO_CMD_MSG             "AT+MSG"                // Send confirmed message
#define WIO_CMD_CMSG            "AT+CMSG"               // Send unconfirmed message
#define WIO_CMD_MSGHEX          "AT+MSGHEX"             // Send hex message

/* Low Power and Sleep */
#define WIO_CMD_LOWPOWER        "AT+LOWPOWER"           // Enable low power
#define WIO_CMD_SLEEP           "AT+SLEEP"              // Enter sleep mode

/* LoRa P2P Mode Commands */
#define WIO_CMD_TEST_RFCFG      "AT+TEST=RFCFG"         // RF configuration
#define WIO_CMD_TEST_TXLRPKT    "AT+TEST=TXLRPKT"       // TX LoRa packet
#define WIO_CMD_TEST_RXLRPKT    "AT+TEST=RXLRPKT"       // RX LoRa packet

/* GPIO and ADC */
#define WIO_CMD_GPIO            "AT+GPIO"               // GPIO control
#define WIO_CMD_ADC             "AT+ADC"                // ADC reading

/* Frequency Band / Region Commands */
// Get frequency band/scheme:     AT+DR=SCHEME
// Get LoRaWAN version and region: AT+LW=VER
// Get channel info:               AT+CH
// Get number of channels:         AT+CH=NUM

/* Common Frequency Bands */
// EU868  - Europe (863-870 MHz)
// US915  - North America (902-928 MHz)
// AU915  - Australia (915-928 MHz)
// AS923  - Asia (915-928 MHz)
// KR920  - Korea (920-923 MHz)
// IN865  - India (865-867 MHz)
// CN470  - China (470-510 MHz)

/* Example Usage Strings */

// Check frequency band/region
// AT+DR=SCHEME

// Check LoRaWAN version and region
// AT+LW=VER

// Get all channel configuration
// AT+CH

// Join network (after setting keys)
// AT+JOIN

// Send confirmed message "Hello"
// AT+MSG="Hello"

// Send hex message
// AT+MSGHEX="48656C6C6F"  // "Hello" in hex

// Set Data Rate to DR5
// AT+DR=5

// Set AppEUI
// AT+ID=AppEui,"0000000000000000"

// Set AppKey
// AT+KEY=APPKEY,"00000000000000000000000000000000"

#endif /* INC_WIO_E5_COMMANDS_H_ */

