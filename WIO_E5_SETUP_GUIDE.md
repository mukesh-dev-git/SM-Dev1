# Wio-E5 LoRaWAN Module - Quick Setup Guide

## 📋 Hardware Connections

### STM32L432KC ↔ Wio-E5-LE-Mini

| Wio-E5 Pin | STM32 Pin | Function |
|------------|-----------|----------|
| UART1_TX   | PB7 (USART1_RX) | Serial RX |
| UART1_RX   | PB6 (USART1_TX) | Serial TX |
| VCC        | 3.3V | Power |
| GND        | GND | Ground |

### STM32L432KC ↔ 16x2 I2C LCD

| LCD Pin | STM32 Pin | Function |
|---------|-----------|----------|
| SCL     | PA9 (I2C1_SCL) | I2C Clock |
| SDA     | PA10 (I2C1_SDA) | I2C Data |
| VCC     | 5V | Power |
| GND     | GND | Ground |

---

## 🚀 What the Code Does

### Startup Sequence:
1. **Initializes LCD** and displays "Wio-E5 Module Initializing..."
2. **Sends `AT`** command to test Wio-E5 connection → displays response
3. **Sends `AT+VER`** to get firmware version → displays on LCD
4. **Sends `AT+ID`** to get Device EUI → displays on LCD

### Main Loop:
- Toggles LED (PB3) to show system is running
- Sends `AT` command every 5 seconds
- Displays response on LCD

---

## 🛠️ How to Use Different AT Commands

### Include the command reference:
```c
#include "wio_e5_commands.h"
```

### Example 1: Get Device Information
```c
// Get DevEUI
WioE5_SendCommand(WIO_CMD_ID_DEVEUI);
WioE5_ReceiveResponse(1000);
LCD_DisplayResponse(wio_rx_buffer);

// Get Version
WioE5_SendCommand(WIO_CMD_VER);
WioE5_ReceiveResponse(1000);
LCD_DisplayResponse(wio_rx_buffer);
```

### Example 2: Configure for LoRaWAN OTAA
```c
// Set OTAA mode
WioE5_SendCommand(WIO_CMD_MODE_LWOTAA);
WioE5_ReceiveResponse(1000);

// Set AppEUI (replace with your AppEUI)
WioE5_SendCommand("AT+ID=AppEui,\"0000000000000001\"");
WioE5_ReceiveResponse(1000);

// Set AppKey (replace with your AppKey)
WioE5_SendCommand("AT+KEY=APPKEY,\"00112233445566778899AABBCCDDEEFF\"");
WioE5_ReceiveResponse(1000);

// Join network
WioE5_SendCommand(WIO_CMD_JOIN);
WioE5_ReceiveResponse(10000);  // Join can take up to 10 seconds
```

### Example 3: Send LoRaWAN Message
```c
// Send text message (confirmed)
WioE5_SendCommand("AT+MSG=\"Hello from STM32\"");
WioE5_ReceiveResponse(5000);
LCD_DisplayResponse(wio_rx_buffer);

// Send hex message (unconfirmed)
WioE5_SendCommand("AT+CMSG=\"48656C6C6F\"");  // "Hello" in hex
WioE5_ReceiveResponse(5000);
```

### Example 4: Configure LoRa Parameters
```c
// Set Data Rate to DR5
WioE5_SendCommand("AT+DR=5");
WioE5_ReceiveResponse(1000);

// Set TX Power to 14 dBm
WioE5_SendCommand("AT+POWER=14");
WioE5_ReceiveResponse(1000);

// Set Class C
WioE5_SendCommand("AT+CLASS=C");
WioE5_ReceiveResponse(1000);
```

---

## 📝 Common AT Commands Reference

| Command | Description | Example |
|---------|-------------|---------|
| `AT` | Test connection | `AT` |
| `AT+RESET` | Reset module | `AT+RESET` |
| `AT+VER` | Get firmware version | `AT+VER` |
| `AT+ID` | Get all device IDs | `AT+ID` |
| `AT+MODE=LWOTAA` | Set OTAA mode | `AT+MODE=LWOTAA` |
| `AT+MODE=LWABP` | Set ABP mode | `AT+MODE=LWABP` |
| `AT+JOIN` | Join LoRaWAN network | `AT+JOIN` |
| `AT+MSG="text"` | Send confirmed message | `AT+MSG="Hello"` |
| `AT+CMSG="text"` | Send unconfirmed msg | `AT+CMSG="Data"` |
| `AT+DR=X` | Set data rate (0-7) | `AT+DR=5` |
| `AT+POWER=X` | Set TX power (dBm) | `AT+POWER=14` |
| `AT+CLASS=X` | Set class (A/B/C) | `AT+CLASS=A` |

---

## 🔧 Customizing the Code

### Change AT Command in Main Loop:
Edit the main loop in `main.c`:

```c
while (1)
{
    // Toggle LED
    HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
    
    // Your custom command here
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_Print("Custom CMD...");
    
    WioE5_SendCommand("AT+DR");  // Example: Get data rate
    WioE5_ReceiveResponse(1000);
    
    LCD_DisplayResponse(wio_rx_buffer);
    
    HAL_Delay(5000);  // Adjust delay as needed
}
```

### Send Data from Sensor:
```c
char command[64];
float temperature = 25.5;  // Example sensor reading

// Format command with sensor data
sprintf(command, "AT+MSG=\"Temp:%.1fC\"", temperature);

WioE5_SendCommand(command);
WioE5_ReceiveResponse(5000);
```

---

## 🐛 Troubleshooting

### No Response on LCD:
1. Check UART connections (TX→RX, RX→TX)
2. Verify Wio-E5 is powered (3.3V)
3. Check baud rate is 9600 (default for Wio-E5)
4. Try changing I2C LCD address in `i2c_lcd.h` to `0x3F`

### LCD Shows "No Response":
1. Wio-E5 may not be responding - check power
2. Increase timeout in `WioE5_ReceiveResponse(2000)`
3. Verify TX/RX pins are correct (PB6=TX, PB7=RX)

### Wio-E5 Not Joining Network:
1. Verify AppEUI and AppKey are correct
2. Check if you're in LoRaWAN coverage area
3. Ensure module is in OTAA mode: `AT+MODE=LWOTAA`
4. Reset module: `AT+RESET` then try again

### Wrong Characters on LCD:
1. Adjust LCD contrast potentiometer on I2C backpack
2. Verify I2C connections (SDA/SCL)
3. Check I2C address (0x27 or 0x3F)

---

## 🌐 LoRaWAN Integration

### The Things Network (TTN) Setup:
1. Register device on TTN console
2. Copy **DevEUI** from Wio-E5: `AT+ID=DevEui`
3. Copy **AppEUI** and **AppKey** from TTN to Wio-E5:
   ```
   AT+ID=AppEui,"YOUR_APPEUI_HERE"
   AT+KEY=APPKEY,"YOUR_APPKEY_HERE"
   ```
4. Join network: `AT+JOIN`
5. Send test message: `AT+MSG="Hello TTN"`

### Helium Network Setup:
Similar to TTN, configure DevEUI, AppEUI, and AppKey

---

## 📊 Expected Responses

| Command | Expected Response |
|---------|------------------|
| `AT` | `+AT: OK` |
| `AT+VER` | `+VER: 2.0.1` (version number) |
| `AT+ID` | `+ID: DevAddr, XX:XX:XX:XX` |
| `AT+JOIN` | `+JOIN: Start` then `+JOIN: NORMAL` or `+JOIN: Done` |
| `AT+MSG="Hi"` | `+MSG: Start` then `+MSG: Done` |

---

## 🔋 Power Optimization

To enable low power mode:
```c
WioE5_SendCommand("AT+LOWPOWER");
WioE5_ReceiveResponse(1000);
```

---

## 📌 Pin Summary

| Pin | Function | Connected To |
|-----|----------|--------------|
| PB6 | USART1_TX | Wio-E5 RX |
| PB7 | USART1_RX | Wio-E5 TX |
| PA9 | I2C1_SCL | LCD SCL |
| PA10 | I2C1_SDA | LCD SDA |
| PA2 | USART2_TX | Debug/VCP |
| PA15 | USART2_RX | Debug/VCP |
| PB3 | GPIO_Output | LED (LD3) |

---

## ✅ Next Steps

1. **Test Basic Communication**: Flash the code and verify AT commands work
2. **Configure LoRaWAN**: Set your network credentials
3. **Join Network**: Use `AT+JOIN` command
4. **Send Messages**: Test with `AT+MSG` or `AT+CMSG`
5. **Integrate Sensors**: Read sensor data and transmit via LoRaWAN

---

## 📚 Resources

- [Wio-E5 AT Command Guide](https://files.seeedstudio.com/products/317990687/res/LoRa-E5%20AT%20Command%20Specification_V1.0%20.pdf)
- [STM32L432KC Datasheet](https://www.st.com/resource/en/datasheet/stm32l432kc.pdf)
- [The Things Network Docs](https://www.thethingsnetwork.org/docs/)

---

**Happy LoRaWAN Development! 🎉**

