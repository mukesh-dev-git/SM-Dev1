# Secure LoRaWAN Configuration - Quick Start

## 🔒 **Immediate Steps for Secure Communication**

---

## **Step 1: Register Your Device on LoRaWAN Network**

### **Option A: The Things Network (TTN) - Free**

1. **Create Account:**
   - Go to [console.thethingsnetwork.org](https://console.thethingsnetwork.org)
   - Sign up for free account

2. **Create Application:**
   ```
   Console → Applications → Create Application
   - Application ID: energy-meter-001
   - Description: Energy Meter Network
   ```

3. **Register Device:**
   ```
   Application → End Devices → Add End Device
   
   Device Configuration:
   ├─ Activation mode: OTAA (recommended)
   ├─ LoRaWAN version: 1.0.3
   ├─ Regional Parameters: Select your region (EU868/US915)
   └─ Frequency plan: Based on your Wio-E5 band
   ```

4. **Get Your Credentials:**
   ```
   After registration, you'll see:
   
   DevEUI:  2CF7F120323031390000000000000001  (unique to device)
   AppEUI:  70B3D57ED0000000000000000000      (from application)
   AppKey:  2B7E151628AED2A6ABF7158809CF4F3C  (for encryption)
   ```

### **Option B: Helium Network - Paid**

1. Create account on [console.helium.com](https://console.helium.com)
2. Purchase Data Credits
3. Register device (similar process to TTN)

### **Option C: Private LoRaWAN Network**

If you have your own ChirpStack/LoRa Server:
- Follow your network administrator's registration process

---

## **Step 2: Configure Wio-E5 Module**

### **2.1 Get Your Device EUI:**

```c
// In your STM32 code:
WioE5_SendCommand("AT+ID");
WioE5_ReceiveResponse(2000);

// Response will show:
// +ID: DevAddr, 26:01:1D:87
// +ID: DevEui, 2C:F7:F1:20:32:30:31:39
// +ID: AppEui, 00:00:00:00:00:00:00:00

// Copy the DevEui - you'll need it for registration
```

**LCD will display:** Your unique DevEUI

---

### **2.2 Configure OTAA Mode:**

Add this to your `main.c` initialization:

```c
/* USER CODE BEGIN 2 */

// ... after LCD init ...

// ========== SECURE LORAWAN CONFIGURATION ==========
LCD_Clear();
LCD_SetCursor(0, 0);
LCD_Print("LoRaWAN Config");
HAL_Delay(1000);

// Step 1: Set to OTAA mode (most secure)
LCD_SetCursor(1, 0);
LCD_Print("Set OTAA...");
WioE5_SendCommand("AT+MODE=LWOTAA");
WioE5_ReceiveResponse(1000);
HAL_Delay(1000);

// Step 2: Set AppEUI (replace with YOUR AppEUI from TTN/Helium)
LCD_Clear();
LCD_SetCursor(0, 0);
LCD_Print("Set AppEUI...");
WioE5_SendCommand("AT+ID=AppEui,\"70B3D57ED0000000\"");
WioE5_ReceiveResponse(1000);
HAL_Delay(1000);

// Step 3: Set AppKey (replace with YOUR AppKey from TTN/Helium)
LCD_Clear();
LCD_SetCursor(0, 0);
LCD_Print("Set AppKey...");
WioE5_SendCommand("AT+KEY=APPKEY,\"2B7E151628AED2A6ABF7158809CF4F3C\"");
WioE5_ReceiveResponse(1000);
HAL_Delay(1000);

// Step 4: Set Data Rate (DR5 recommended for fast response)
LCD_Clear();
LCD_SetCursor(0, 0);
LCD_Print("Set Data Rate");
WioE5_SendCommand("AT+DR=5");  // DR5 for EU868
WioE5_ReceiveResponse(1000);
HAL_Delay(1000);

// Step 5: Enable ADR (Adaptive Data Rate)
LCD_Clear();
LCD_SetCursor(0, 0);
LCD_Print("Enable ADR...");
WioE5_SendCommand("AT+ADR=ON");
WioE5_ReceiveResponse(1000);
HAL_Delay(1000);

// Step 6: Set Class A (default, most power efficient)
LCD_Clear();
LCD_SetCursor(0, 0);
LCD_Print("Set Class A");
WioE5_SendCommand("AT+CLASS=A");
WioE5_ReceiveResponse(1000);
HAL_Delay(1000);

// Configuration complete!
LCD_Clear();
LCD_SetCursor(0, 0);
LCD_Print("Config Complete!");
HAL_Delay(2000);

/* USER CODE END 2 */
```

---

### **2.3 Join Network:**

```c
/* USER CODE BEGIN 2 */

// ... after configuration ...

// ========== JOIN LORAWAN NETWORK ==========
LCD_Clear();
LCD_SetCursor(0, 0);
LCD_Print("Joining Network");
LCD_SetCursor(1, 0);
LCD_Print("Please wait...");

// Attempt to join (can take 5-10 seconds)
WioE5_SendCommand("AT+JOIN");
WioE5_ReceiveResponse(15000);  // Wait up to 15 seconds

// Check if joined successfully
if (strstr(wio_rx_buffer, "joined") || strstr(wio_rx_buffer, "Done")) {
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_Print("JOIN SUCCESS!");
    LCD_SetCursor(1, 0);
    LCD_Print("Network Active");
    
    // Blink LED rapidly to indicate success
    for(int i=0; i<10; i++) {
        HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
        HAL_Delay(100);
    }
} else {
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_Print("JOIN FAILED!");
    LCD_SetCursor(1, 0);
    LCD_Print("Check Config");
    
    // Blink LED slowly to indicate failure
    for(int i=0; i<5; i++) {
        HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
        HAL_Delay(500);
    }
}

HAL_Delay(3000);

/* USER CODE END 2 */
```

---

## **Step 3: Send Test Message**

```c
/* USER CODE BEGIN 2 */

// ... after successful join ...

// ========== SEND TEST MESSAGE ==========
LCD_Clear();
LCD_SetCursor(0, 0);
LCD_Print("Sending Data...");

// Prepare test payload
char test_data[] = "Hello from Energy Meter!";

// Convert to hex string for MSGHEX command
char hex_payload[100];
sprintf(hex_payload, "AT+MSGHEX=\"");
for(int i = 0; i < strlen(test_data); i++) {
    sprintf(hex_payload + strlen(hex_payload), "%02X", test_data[i]);
}
strcat(hex_payload, "\"");

// Send confirmed message (waits for acknowledgment)
WioE5_SendCommand(hex_payload);
WioE5_ReceiveResponse(10000);

// Check if message sent successfully
if (strstr(wio_rx_buffer, "Done") || strstr(wio_rx_buffer, "OK")) {
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_Print("Message Sent!");
    LCD_SetCursor(1, 0);
    LCD_Print("Check Console");
} else {
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_Print("Send Failed");
    LCD_DisplayResponse(wio_rx_buffer);
}

HAL_Delay(3000);

/* USER CODE END 2 */
```

---

## **Step 4: Verify on Network Console**

### **The Things Network:**
1. Go to your application
2. Click on your device
3. Go to "Live Data" tab
4. You should see your message appear!

**Expected Output:**
```
Uplink Message:
├─ Time: 2025-01-16 13:45:23
├─ FCnt: 1
├─ FPort: 1
├─ Payload: 48 65 6C 6C 6F 20 66 72 6F 6D 20 45 6E 65 72 67 79 20 4D 65 74 65 72 21
└─ Decoded: "Hello from Energy Meter!"
```

---

## **Step 5: Send Energy Meter Data**

### **5.1 Create Data Structure:**

```c
/* USER CODE BEGIN PV */

// Energy meter data structure
typedef struct __attribute__((packed)) {
    uint8_t  msg_type;      // 0x01 = energy data
    uint32_t timestamp;     // Unix timestamp
    uint16_t voltage;       // Voltage * 10 (e.g., 230.5V = 2305)
    uint16_t current;       // Current * 100 (e.g., 5.23A = 523)
    uint32_t power;         // Power in watts
    uint32_t energy;        // Energy in Wh
    uint8_t  power_factor;  // PF * 100 (e.g., 0.95 = 95)
    uint8_t  status;        // Status flags
} EnergyPayload_t;

EnergyPayload_t energy_data;

/* USER CODE END PV */
```

### **5.2 Populate and Send:**

```c
/* USER CODE BEGIN 3 */

// In your main loop:

// Simulate energy meter readings (replace with actual sensor readings)
energy_data.msg_type = 0x01;
energy_data.timestamp = HAL_GetTick() / 1000;  // Seconds since boot
energy_data.voltage = 2305;      // 230.5V
energy_data.current = 523;       // 5.23A
energy_data.power = 1205;        // 1205W
energy_data.energy = 15234;      // 15.234 kWh
energy_data.power_factor = 95;   // 0.95
energy_data.status = 0x00;       // No errors

// Convert to hex string
char hex_cmd[150];
sprintf(hex_cmd, "AT+MSGHEX=\"");

uint8_t* data_ptr = (uint8_t*)&energy_data;
for(int i = 0; i < sizeof(EnergyPayload_t); i++) {
    sprintf(hex_cmd + strlen(hex_cmd), "%02X", data_ptr[i]);
}
strcat(hex_cmd, "\"");

// Send data
LCD_Clear();
LCD_SetCursor(0, 0);
LCD_Print("Sending Energy");
LCD_SetCursor(1, 0);
LCD_Print("Data...");

WioE5_SendCommand(hex_cmd);
WioE5_ReceiveResponse(10000);

LCD_Clear();
LCD_SetCursor(0, 0);
if (strstr(wio_rx_buffer, "Done")) {
    LCD_Print("Data Sent OK");
} else {
    LCD_Print("Send Failed");
}

HAL_Delay(2000);

// Wait 15 minutes before next transmission
HAL_Delay(900000);  // 15 minutes

/* USER CODE END 3 */
```

---

## **Security Best Practices**

### ✅ **DO:**
1. Use OTAA (not ABP)
2. Store AppKey securely (not in plain text)
3. Use confirmed messages for critical data
4. Implement message sequence numbers
5. Monitor for failed transmissions
6. Rotate keys periodically (if supported)

### ❌ **DON'T:**
1. Don't commit keys to Git/version control
2. Don't use ABP in production
3. Don't ignore frame counter mismatches
4. Don't transmit more than necessary (duty cycle)
5. Don't hardcode sensitive data

---

## **Troubleshooting JOIN Failures**

### **Issue: +JOIN: Join failed**

**Check:**
1. ✅ DevEUI registered on network
2. ✅ AppEUI matches network
3. ✅ AppKey is correct (no typos!)
4. ✅ Frequency band matches (EU868/US915)
5. ✅ Gateway in range (check network coverage)
6. ✅ Antenna connected to Wio-E5

**Try:**
```c
// Reset module and retry
WioE5_SendCommand("AT+RESET");
HAL_Delay(3000);

// Reconfigure and join
WioE5_SendCommand("AT+JOIN");
```

### **Issue: No gateways receive**

**Solutions:**
1. Check gateway location: [TTN Mapper](https://ttnmapper.org)
2. Increase TX power:
   ```c
   WioE5_SendCommand("AT+POWER=14");  // Max power (14 dBm for EU868)
   ```
3. Verify antenna is connected
4. Move closer to window/outside

---

## **Network Provider Comparison**

| Feature | The Things Network | Helium | Private Network |
|---------|-------------------|---------|-----------------|
| **Cost** | Free | Pay per device/DC | Setup cost |
| **Coverage** | Community gateways | Global (growing) | Your gateways |
| **Reliability** | Varies | Good | Full control |
| **Best For** | Testing, POC | Production | Enterprise |
| **Support** | Community | Commercial | Self-managed |

---

## **Next Steps After Successful Join**

1. ✅ **Test message delivery** - Send test payloads
2. ✅ **Verify console reception** - Check network console
3. ✅ **Implement energy measurement** - Add real sensors
4. ✅ **Optimize payload** - Minimize data size
5. ✅ **Add error handling** - Handle failed transmissions
6. ✅ **Implement data logging** - Store data locally
7. ✅ **Setup downlink commands** - Remote configuration

---

## **Complete Example Code**

I can create a complete `lorawan_security.c` module with all these functions. Would you like me to:

1. Create modular code for LoRaWAN communication?
2. Add energy measurement module (ADC + calculations)?
3. Implement data logging to flash?
4. Create downlink command handler?

Let me know which module to create next!

