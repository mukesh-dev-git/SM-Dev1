# Firmware Development Plan - Before STPM33 Arrives

## 🎯 **Development Strategy**

While waiting for the EVALSTPM33 module, we can develop and test all supporting firmware modules. This will ensure rapid integration once the hardware arrives.

---

## 📦 **Modules to Develop Now (Priority Order)**

### **Priority 1: LoRaWAN Communication Module** ⭐
**Status:** Foundation already working (Wio-E5 responding)  
**Estimated Time:** 3-4 days  
**Why First:** Critical for data transmission, can be tested immediately

**Features to Implement:**
- ✅ Secure OTAA configuration
- ✅ Network join with retry logic
- ✅ Payload encoding/decoding
- ✅ Confirmed vs unconfirmed messages
- ✅ Downlink command parser
- ✅ Connection monitoring
- ✅ Error handling & recovery

**Files to Create:**
- `Core/Inc/lorawan_comm.h`
- `Core/Src/lorawan_comm.c`

**Testing:** Can test with dummy sensor data before STPM33 arrives

---

### **Priority 2: Data Logger Module** ⭐
**Status:** Not started  
**Estimated Time:** 2-3 days  
**Why Second:** Independent of STPM33, useful for any data

**Features to Implement:**
- ✅ Flash memory management
- ✅ Circular buffer for readings
- ✅ Write/read/erase functions
- ✅ Data integrity (CRC)
- ✅ Timestamp each entry
- ✅ Retrieve historical data
- ✅ Flash wear leveling

**Files to Create:**
- `Core/Inc/data_logger.h`
- `Core/Src/data_logger.c`

**Testing:** Can log test data (voltage, current simulated values)

---

### **Priority 3: RTC & Timestamp Module** ⭐
**Status:** RTC configured in CubeMX, not used yet  
**Estimated Time:** 1-2 days  
**Why Third:** Needed for data logging timestamps

**Features to Implement:**
- ✅ RTC initialization
- ✅ Set/get time functions
- ✅ Unix timestamp conversion
- ✅ Alarm configuration
- ✅ Periodic wakeup
- ✅ Time synchronization (from LoRaWAN downlink)

**Files to Create:**
- `Core/Inc/rtc_manager.h`
- `Core/Src/rtc_manager.c`

**Testing:** Can test time keeping, alarm triggers

---

### **Priority 4: Configuration Management** ⭐
**Status:** Not started  
**Estimated Time:** 2 days  
**Why Fourth:** Store calibration, settings, LoRaWAN keys

**Features to Implement:**
- ✅ Configuration structure
- ✅ Save/load from flash
- ✅ Default configuration
- ✅ Factory reset
- ✅ Configuration validation (CRC)
- ✅ Parameter update via LoRaWAN

**Files to Create:**
- `Core/Inc/config_manager.h`
- `Core/Src/config_manager.c`

**Testing:** Can test save/restore with dummy configuration

---

### **Priority 5: Error Handler & Watchdog** ⭐
**Status:** Basic error handler exists  
**Estimated Time:** 1-2 days  
**Why Fifth:** System reliability

**Features to Implement:**
- ✅ Centralized error logging
- ✅ Error code definitions
- ✅ Watchdog timer configuration
- ✅ Auto-recovery mechanisms
- ✅ Error reporting via LoRaWAN
- ✅ LED error codes

**Files to Create:**
- `Core/Inc/error_handler.h`
- `Core/Src/error_handler.c`

**Testing:** Can test error scenarios, watchdog reset

---

### **Priority 6: System Monitor** 
**Status:** Not started  
**Estimated Time:** 1-2 days  
**Why Sixth:** Health monitoring

**Features to Implement:**
- ✅ CPU temperature monitoring
- ✅ Supply voltage monitoring (VDDA)
- ✅ Stack usage monitoring
- ✅ Uptime tracking
- ✅ Reset reason detection
- ✅ Performance metrics

**Files to Create:**
- `Core/Inc/system_monitor.h`
- `Core/Src/system_monitor.c`

**Testing:** Can test all monitoring functions

---

### **Priority 7: Command Interface** 
**Status:** Not started  
**Estimated Time:** 1-2 days  
**Why Seventh:** Remote configuration

**Features to Implement:**
- ✅ Downlink command parser
- ✅ Command execution
- ✅ Response generation
- ✅ Command authentication
- ✅ Command history logging

**Files to Create:**
- `Core/Inc/command_handler.h`
- `Core/Src/command_handler.c`

**Testing:** Can test with simulated downlink messages

---

### **Priority 8: LCD Display Manager** 
**Status:** Basic LCD working  
**Estimated Time:** 1 day  
**Why Eighth:** Better user interface

**Features to Implement:**
- ✅ Screen management (multiple pages)
- ✅ Menu system
- ✅ Animated indicators
- ✅ Status icons
- ✅ Scrolling text
- ✅ Numeric formatting helpers

**Files to Create:**
- `Core/Inc/display_manager.h`
- `Core/Src/display_manager.c`

**Testing:** Can test with dummy data

---

### **Priority 9: State Machine** 
**Status:** Not started  
**Estimated Time:** 1-2 days  
**Why Ninth:** Application flow control

**Features to Implement:**
- ✅ State machine implementation
- ✅ State transitions
- ✅ Event handling
- ✅ Timer-based state changes
- ✅ Power management states

**Files to Create:**
- `Core/Inc/state_machine.h`
- `Core/Src/state_machine.c`

**Testing:** Can test state transitions

---

## 📋 **Detailed Module Specifications**

### **Module 1: LoRaWAN Communication**

```c
// lorawan_comm.h

#ifndef INC_LORAWAN_COMM_H_
#define INC_LORAWAN_COMM_H_

#include "stm32l4xx_hal.h"
#include <stdbool.h>

/* LoRaWAN Configuration */
typedef struct {
    char app_eui[17];           // AppEUI (16 hex chars)
    char app_key[33];           // AppKey (32 hex chars)
    char dev_eui[17];           // DevEUI (read from module)
    uint8_t data_rate;          // DR0-DR7
    bool adr_enabled;           // Adaptive Data Rate
    uint8_t tx_power;           // TX power in dBm
    uint8_t class;              // Class A, B, or C
} LoRaWAN_Config_t;

/* Transmission Status */
typedef enum {
    LORAWAN_NOT_JOINED,
    LORAWAN_JOINING,
    LORAWAN_JOINED,
    LORAWAN_SENDING,
    LORAWAN_TX_OK,
    LORAWAN_TX_FAILED,
    LORAWAN_ERROR
} LoRaWAN_Status_t;

/* Message Types */
typedef enum {
    MSG_TYPE_ENERGY_DATA = 0x01,
    MSG_TYPE_STATUS = 0x02,
    MSG_TYPE_ALERT = 0x03,
    MSG_TYPE_DIAGNOSTIC = 0x04
} Message_Type_t;

/* Function Prototypes */
HAL_StatusTypeDef LoRaWAN_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef LoRaWAN_Configure(LoRaWAN_Config_t *config);
HAL_StatusTypeDef LoRaWAN_Join(uint32_t timeout_ms);
HAL_StatusTypeDef LoRaWAN_SendData(uint8_t *data, uint16_t length, bool confirmed);
HAL_StatusTypeDef LoRaWAN_SendString(char *message, bool confirmed);
HAL_StatusTypeDef LoRaWAN_CheckDownlink(uint8_t *data, uint16_t *length);
LoRaWAN_Status_t LoRaWAN_GetStatus(void);
bool LoRaWAN_IsJoined(void);
HAL_StatusTypeDef LoRaWAN_Reset(void);

#endif /* INC_LORAWAN_COMM_H_ */
```

---

### **Module 2: Data Logger**

```c
// data_logger.h

#ifndef INC_DATA_LOGGER_H_
#define INC_DATA_LOGGER_H_

#include "stm32l4xx_hal.h"

/* Data Record Structure */
typedef struct {
    uint32_t timestamp;         // Unix timestamp
    float voltage;              // Voltage (V)
    float current;              // Current (A)
    float power;                // Power (W)
    float energy;               // Energy (kWh)
    uint8_t status;             // Status flags
    uint16_t crc;               // CRC-16
} DataRecord_t;

/* Logger Configuration */
#define MAX_RECORDS             1000    // Max records in circular buffer
#define FLASH_START_ADDR        0x08040000  // Start of data storage
#define RECORD_SIZE             sizeof(DataRecord_t)

/* Function Prototypes */
HAL_StatusTypeDef DataLogger_Init(void);
HAL_StatusTypeDef DataLogger_Write(DataRecord_t *record);
HAL_StatusTypeDef DataLogger_Read(uint32_t index, DataRecord_t *record);
HAL_StatusTypeDef DataLogger_ReadLast(DataRecord_t *record);
HAL_StatusTypeDef DataLogger_Clear(void);
uint32_t DataLogger_GetRecordCount(void);
HAL_StatusTypeDef DataLogger_GetRecordRange(uint32_t start, uint32_t count, DataRecord_t *records);
HAL_StatusTypeDef DataLogger_ExportToLoRaWAN(void);

#endif /* INC_DATA_LOGGER_H_ */
```

---

### **Module 3: RTC Manager**

```c
// rtc_manager.h

#ifndef INC_RTC_MANAGER_H_
#define INC_RTC_MANAGER_H_

#include "stm32l4xx_hal.h"
#include <time.h>

/* Function Prototypes */
HAL_StatusTypeDef RTC_Init(void);
HAL_StatusTypeDef RTC_SetTime(uint32_t unix_timestamp);
uint32_t RTC_GetTime(void);
HAL_StatusTypeDef RTC_SetAlarm(uint32_t seconds_from_now);
HAL_StatusTypeDef RTC_SetPeriodicWakeup(uint32_t interval_seconds);
void RTC_GetDateTime(struct tm *datetime);
HAL_StatusTypeDef RTC_SyncFromNetwork(uint32_t network_time);

#endif /* INC_RTC_MANAGER_H_ */
```

---

### **Module 4: Configuration Manager**

```c
// config_manager.h

#ifndef INC_CONFIG_MANAGER_H_
#define INC_CONFIG_MANAGER_H_

#include "stm32l4xx_hal.h"

/* System Configuration */
typedef struct {
    // LoRaWAN Settings
    char app_eui[17];
    char app_key[33];
    uint16_t tx_interval;       // Transmission interval (seconds)
    
    // Calibration
    float voltage_gain;
    float voltage_offset;
    float current_gain;
    float current_offset;
    
    // Thresholds
    float overvoltage_threshold;
    float overcurrent_threshold;
    
    // System Settings
    uint8_t device_id;
    uint32_t config_version;
    uint32_t last_update;
    
    // Integrity
    uint16_t crc;
} SystemConfig_t;

/* Function Prototypes */
HAL_StatusTypeDef Config_Init(void);
HAL_StatusTypeDef Config_Load(SystemConfig_t *config);
HAL_StatusTypeDef Config_Save(SystemConfig_t *config);
HAL_StatusTypeDef Config_Reset(void);
HAL_StatusTypeDef Config_Validate(SystemConfig_t *config);
SystemConfig_t* Config_GetDefault(void);

#endif /* INC_CONFIG_MANAGER_H_ */
```

---

## 🗓️ **2-Week Development Schedule**

### **Week 1: Core Communication & Storage**

| Day | Module | Tasks | Hours |
|-----|--------|-------|-------|
| **Mon** | LoRaWAN Comm | Basic functions, OTAA join | 6-8h |
| **Tue** | LoRaWAN Comm | Payload encoding, send/receive | 6-8h |
| **Wed** | LoRaWAN Comm | Error handling, retry logic | 4-6h |
| **Thu** | Data Logger | Flash management, write/read | 6-8h |
| **Fri** | Data Logger | Circular buffer, CRC validation | 4-6h |
| **Sat** | Testing | LoRaWAN + Logger integration | 4h |
| **Sun** | Testing | Long-term stability test | 2h |

**Week 1 Deliverable:** LoRaWAN sending dummy data, flash logging working

---

### **Week 2: System Management & UI**

| Day | Module | Tasks | Hours |
|-----|--------|-------|-------|
| **Mon** | RTC Manager | Time functions, alarms | 4-6h |
| **Tue** | Config Manager | Save/load, validation | 4-6h |
| **Wed** | Error Handler | Error logging, watchdog | 4-6h |
| **Thu** | System Monitor | Health monitoring, diagnostics | 4-6h |
| **Fri** | Display Manager | LCD menus, formatting | 4-6h |
| **Sat** | State Machine | State flow, integration | 4-6h |
| **Sun** | Testing | Full system integration test | 4h |

**Week 2 Deliverable:** Complete firmware framework ready for STPM33

---

## ✅ **What Can Be Tested Now**

### **Without STPM33:**

1. **LoRaWAN Communication** ✅
   - Join network (TTN/Helium)
   - Send dummy energy data
   - Receive downlink commands
   - Monitor on network console

2. **Data Logging** ✅
   - Write test records to flash
   - Read back and verify
   - Test circular buffer wrap-around
   - Verify CRC integrity

3. **RTC Functions** ✅
   - Set/get time
   - Test alarms
   - Periodic wakeup
   - Timestamp accuracy

4. **Configuration** ✅
   - Save settings to flash
   - Load on boot
   - Factory reset
   - CRC validation

5. **System Monitor** ✅
   - CPU temperature
   - Supply voltage
   - Uptime tracking
   - Reset reason

6. **LCD Display** ✅
   - Menu navigation
   - Status screens
   - Animated indicators
   - Data formatting

7. **Error Handling** ✅
   - Watchdog reset
   - Error logging
   - Auto-recovery
   - Error reporting

---

## 🧪 **Test Data Simulation**

### **Create Simulated Sensor Data:**

```c
// For testing before STPM33 arrives

typedef struct {
    float voltage;      // 230V ± 10%
    float current;      // 0-10A
    float power;        // V * I * PF
    float energy;       // Accumulated
    float pf;           // 0.85-1.0
} SimulatedData_t;

void Generate_SimulatedData(SimulatedData_t *data) {
    static float energy_accumulator = 0.0f;
    
    // Simulate realistic values
    data->voltage = 230.0f + (rand() % 20 - 10);  // 220-240V
    data->current = (rand() % 1000) / 100.0f;      // 0-10A
    data->pf = 0.85f + (rand() % 15) / 100.0f;    // 0.85-1.0
    data->power = data->voltage * data->current * data->pf;
    
    // Accumulate energy (Wh)
    energy_accumulator += data->power / 3600.0f;  // Assuming 1-second interval
    data->energy = energy_accumulator;
}
```

---

## 📊 **Integration Readiness**

### **When STPM33 Arrives:**

**Already Complete:**
- ✅ LoRaWAN communication framework
- ✅ Data logging system
- ✅ Configuration management
- ✅ RTC and timestamps
- ✅ Error handling
- ✅ Display management
- ✅ State machine

**To Add (1-2 days):**
- ⏳ STPM33 driver (stpm33.c/h)
- ⏳ Replace simulated data with real measurements
- ⏳ Calibration procedures
- ⏳ Power quality monitoring

**Integration Time:** 1-2 days maximum!

---

## 🎯 **Recommended Starting Point**

### **Today: LoRaWAN Communication Module**

**Reason:** 
- Can test immediately with working Wio-E5
- Critical for final product
- Independent of STPM33
- Visible results (network console)

**Steps:**
1. Register device on TTN/Helium
2. Create `lorawan_comm.c/h` files
3. Implement OTAA join
4. Send test message
5. Verify on network console

**Time:** 1-2 days for full implementation

---

## 📚 **Documentation to Keep**

**Architecture & Design:**
- ✅ ENERGY_METER_ARCHITECTURE.md (general architecture)
- ✅ STPM33_ENERGY_METER_ARCHITECTURE.md (for when hardware arrives)
- ✅ SECURE_LORAWAN_SETUP.md (LoRaWAN setup guide)
- ✅ FREQUENCY_BAND_GUIDE.md (band detection)
- ✅ WIO_E5_SETUP_GUIDE.md (Wio-E5 reference)

**Development:**
- ✅ FIRMWARE_DEVELOPMENT_PLAN.md (this file)

**Deleted:**
- ❌ TEST_RESULTS_GUIDE.md (testing complete)
- ❌ TROUBLESHOOTING_LCD_FREEZE.md (issue resolved)
- ❌ STPM33 driver files (hardware not yet available)

---

## 🚀 **Next Action Items**

### **Immediate (This Week):**

1. **Register LoRaWAN Device** ⏰ 30 min
   - Sign up on TTN or Helium
   - Register device with DevEUI
   - Get AppEUI and AppKey

2. **Create LoRaWAN Module** ⏰ 2 days
   - Implement lorawan_comm.c/h
   - Test OTAA join
   - Send first test message

3. **Create Data Logger** ⏰ 1-2 days
   - Implement flash storage
   - Test with dummy data
   - Verify data integrity

### **Next Week:**

4. **RTC & Config Manager** ⏰ 2 days
5. **Error Handler & Monitor** ⏰ 2 days
6. **Display & State Machine** ⏰ 2 days

---

## 💡 **Benefits of This Approach**

1. **Parallel Development** ⭐
   - Don't wait for STPM33
   - Test each module independently
   - Build confidence in firmware

2. **Risk Reduction** ⭐
   - Identify issues early
   - Proven modules before hardware integration
   - Faster final integration

3. **Learning** ⭐
   - Master LoRaWAN
   - Understand flash management
   - Practice professional firmware structure

4. **Reusability** ⭐
   - Modules can be reused in other projects
   - Clean, modular code
   - Well-documented

---

**Shall I create the LoRaWAN Communication module first? This is the most critical and can be tested immediately!** 🚀

