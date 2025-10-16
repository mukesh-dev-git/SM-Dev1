# Industrial Energy Meter Architecture Guide
## STM32L432KC + Wio-E5 LE-Mini

---

## 🎯 **Project Overview**

**Goal:** Develop an industrial-grade energy meter with secure LoRaWAN communication

**Key Requirements:**
- ✅ Accurate energy measurement
- ✅ Secure data transmission (LoRaWAN)
- ✅ Data integrity and reliability
- ✅ Low power consumption
- ✅ Remote monitoring capability
- ✅ Compliance with industry standards

---

## 🔒 **Phase 1: Secure LoRaWAN Configuration (IMMEDIATE PRIORITY)**

### **1.1 LoRaWAN Security Fundamentals**

#### **OTAA vs ABP:**
- ✅ **Use OTAA (Over-The-Air Activation)** - More secure
- ❌ Avoid ABP (Activation By Personalization) - Keys can be compromised

#### **Required Security Keys:**
1. **DevEUI** - Device unique identifier (64-bit)
2. **AppEUI** - Application identifier (64-bit)
3. **AppKey** - Application key (128-bit, AES encryption)

### **1.2 Implementation Steps:**

```c
// Configure for OTAA mode
WioE5_SendCommand("AT+MODE=LWOTAA");

// Get your DevEUI (unique per device)
WioE5_SendCommand("AT+ID=DevEui");
// Response: +ID: DevEui, 2C:F7:F1:20:32:30:31:39

// Set AppEUI (from your LoRaWAN network provider)
WioE5_SendCommand("AT+ID=AppEui,\"70B3D57ED0000000\"");

// Set AppKey (from your LoRaWAN network provider)
WioE5_SendCommand("AT+KEY=APPKEY,\"2B7E151628AED2A6ABF7158809CF4F3C\"");

// Join network
WioE5_SendCommand("AT+JOIN");
// Wait for: +JOIN: Network joined
```

### **1.3 Data Encryption:**
- All LoRaWAN data is **AES-128 encrypted** automatically
- Network and application session keys derived from AppKey
- End-to-end encryption between device and application server

### **1.4 Message Integrity:**
- Use **confirmed messages** for critical data (energy readings)
- Implement **sequence numbers** to detect replay attacks
- Add **CRC/checksum** to payload

---

## 📊 **Phase 2: Energy Measurement System**

### **2.1 Required Hardware Components:**

#### **Current Sensing:**
- **ACS712** (Hall effect, 5A/20A/30A variants)
- **ZMCT103C** (Current transformer, 5A/10A/20A)
- **INA219** (High-side current sensor, I2C)
- **ADE7753** (Energy metering IC, SPI) ⭐ Recommended

#### **Voltage Sensing:**
- **ZMPT101B** (AC voltage sensor, 250V)
- **Voltage divider** with precision resistors
- **Isolation amplifier** (for safety)

#### **Power Supply:**
- **HLK-PM01** (5V/3.3V isolated AC-DC)
- **Hi-Link HLK-5M05** (5V, 1A isolated)

### **2.2 Connection Diagram:**

```
Energy Meter System Architecture:

[AC Line] ──┬─→ [Voltage Sensor (ZMPT101B)] ──→ ADC1 (PA0)
            │
            └─→ [Current Sensor (ACS712)]   ──→ ADC2 (PA1)
            
[Isolated PSU] ──→ 3.3V + 5V for system

STM32L432KC:
├─ ADC1/ADC2      : Energy measurement
├─ I2C1 (PA9/10)  : LCD display
├─ UART1 (PB6/7)  : Wio-E5 communication
├─ RTC            : Timestamp
├─ Flash/EEPROM   : Data logging
└─ GPIO           : Status LEDs
```

### **2.3 Measurement Parameters:**

Calculate and transmit:
1. **Voltage (V)** - RMS voltage
2. **Current (A)** - RMS current
3. **Power (W)** - Real power
4. **Energy (kWh)** - Cumulative energy
5. **Power Factor** - cos(φ)
6. **Frequency (Hz)** - Line frequency

### **2.4 Sampling Requirements:**

```c
// For 50Hz/60Hz AC:
#define SAMPLES_PER_CYCLE   128      // Sample points per AC cycle
#define SAMPLING_FREQ       6400     // Hz (128 * 50Hz)
#define ADC_RESOLUTION      12       // 12-bit ADC

// For accurate RMS calculation, sample at least 100 points per cycle
```

---

## 🏭 **Phase 3: Industry-Grade Features**

### **3.1 Essential Features:**

#### **A. Data Logging**
```c
typedef struct {
    uint32_t timestamp;        // Unix timestamp
    float voltage_rms;         // RMS voltage
    float current_rms;         // RMS current
    float power_active;        // Active power (W)
    float power_reactive;      // Reactive power (VAR)
    float energy_total;        // Total energy (kWh)
    float power_factor;        // Power factor
    uint8_t status_flags;      // Error/warning flags
    uint16_t crc;              // Data integrity check
} EnergyRecord_t;
```

**Storage:**
- Use STM32 internal flash (256KB)
- External EEPROM (AT24C256, 32KB) via I2C
- Circular buffer for last 1000 readings
- Store to flash every 15 minutes

#### **B. Real-Time Clock (RTC)**
```c
// Enable LSE (32.768kHz crystal) for accurate timekeeping
// Already configured in your clock setup

// Add RTC functions:
- Get_Timestamp()      // Current Unix time
- Set_RTC_Time()       // Sync with network time
- Schedule_Readings()  // Timed measurements
```

#### **C. Watchdog Timer**
```c
// Independent Watchdog (IWDG) for system reliability
// Enable in CubeMX: IWDG with 4-second timeout

void IWDG_Refresh(void) {
    HAL_IWDG_Refresh(&hiwdg);
}

// Refresh in main loop to prevent reset
```

#### **D. Error Detection & Handling**
```c
typedef enum {
    ERROR_NONE = 0x00,
    ERROR_OVERVOLTAGE = 0x01,
    ERROR_OVERCURRENT = 0x02,
    ERROR_FREQUENCY = 0x04,
    ERROR_LORAWAN_FAIL = 0x08,
    ERROR_SENSOR_FAIL = 0x10,
    ERROR_FLASH_FAIL = 0x20,
    WARNING_LOW_PF = 0x40,
    WARNING_BATTERY_LOW = 0x80
} ErrorStatus_t;
```

#### **E. Calibration System**
```c
typedef struct {
    float voltage_gain;        // Calibration multiplier
    float voltage_offset;      // Calibration offset
    float current_gain;
    float current_offset;
    uint32_t calibration_date;
    uint16_t crc;
} CalibrationData_t;

// Store in flash, load at startup
// Implement calibration mode via button/command
```

---

### **3.2 Advanced Features:**

#### **A. Tamper Detection**
- Magnetic tamper detection (Hall sensor)
- Cover open detection (switch)
- Current reversal detection
- Power outage detection with supercap backup

#### **B. Load Profiling**
- Record power consumption patterns
- Detect appliance signatures
- Peak demand tracking
- Time-of-use (TOU) metering

#### **C. Power Quality Monitoring**
- Harmonics analysis (THD)
- Voltage sag/swell detection
- Transient detection
- Phase angle measurement

#### **D. Multi-Tariff Support**
- Peak/off-peak rates
- Seasonal rates
- Demand charges
- Configurable via LoRaWAN downlink

#### **E. Demand Response**
- Remote disconnect/reconnect relay
- Load limiting
- Price signal response
- Emergency curtailment

---

## 📡 **Phase 4: LoRaWAN Communication Strategy**

### **4.1 Message Structure:**

```c
// Payload optimization for LoRaWAN (max 51 bytes for DR0)

typedef struct __attribute__((packed)) {
    uint8_t  msg_type;           // 1 byte: Message type identifier
    uint32_t timestamp;          // 4 bytes: Unix timestamp
    uint16_t voltage;            // 2 bytes: Voltage * 10 (230.5V = 2305)
    uint16_t current;            // 2 bytes: Current * 100 (5.23A = 523)
    uint32_t power;              // 4 bytes: Power in watts
    uint32_t energy;             // 4 bytes: Energy in Wh
    uint8_t  power_factor;       // 1 byte: PF * 100 (0.95 = 95)
    uint8_t  frequency;          // 1 byte: Freq - 45 (50Hz = 5)
    uint8_t  status;             // 1 byte: Error/warning flags
    uint16_t crc16;              // 2 bytes: CRC for integrity
    // Total: 22 bytes
} LoRaWAN_EnergyPayload_t;
```

### **4.2 Transmission Strategy:**

```c
// Adaptive Data Rate (ADR)
WioE5_SendCommand("AT+ADR=ON");  // Enable ADR

// Confirmed vs Unconfirmed messages
// Use confirmed for critical data (energy totals)
// Use unconfirmed for frequent updates (power)

// Confirmed message (critical data):
WioE5_SendCommand("AT+MSG=\"01234567890ABCDEF\"");  // Waits for ACK

// Unconfirmed message (frequent data):
WioE5_SendCommand("AT+CMSG=\"01234567890ABCDEF\""); // No ACK
```

### **4.3 Transmission Schedule:**

```c
// Optimize for battery life vs data freshness

// Example schedule:
- Every 15 minutes: Full energy report (confirmed)
- Every hour: Detailed statistics
- On alarm: Immediate alert (confirmed)
- On demand: Respond to downlink requests

// Fair Access Policy (FAP) compliance:
// EU868: Max 1% duty cycle (36 seconds/hour)
// US915: No duty cycle limit, but gateway limits apply
```

### **4.4 Downlink Commands:**

```c
// Implement command parsing from LoRaWAN downlink
typedef enum {
    CMD_GET_STATUS = 0x01,
    CMD_GET_ENERGY = 0x02,
    CMD_RESET_ENERGY = 0x03,
    CMD_SET_INTERVAL = 0x04,
    CMD_CALIBRATE = 0x05,
    CMD_DISCONNECT = 0x06,
    CMD_FIRMWARE_UPDATE = 0x07
} DownlinkCommand_t;

void Parse_Downlink(uint8_t* payload, uint8_t length) {
    if (length < 1) return;
    
    switch(payload[0]) {
        case CMD_GET_STATUS:
            Send_Status_Report();
            break;
        case CMD_RESET_ENERGY:
            Reset_Energy_Counter();
            break;
        // ... handle other commands
    }
}
```

---

## ⚡ **Phase 5: Power Management**

### **5.1 Low Power Modes:**

```c
// STM32L432 is optimized for low power

// Operating modes:
// - Run mode: 100 µA/MHz @ 3.3V
// - Sleep mode: 50 µA/MHz
// - Stop mode: 1.0 µA with RTC
// - Standby: 0.3 µA

// Strategy for energy meter:
// 1. ADC sampling: Run mode
// 2. Between samples: Sleep mode
// 3. Between transmissions: Stop mode with RTC wakeup
```

### **5.2 Power Supply:**

```c
// Dual power supply design:
// Primary: AC line-powered (always on)
// Backup: Supercapacitor or battery (power failure)

// Detect power failure:
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == POWER_FAIL_Pin) {
        // AC power lost!
        Save_Critical_Data();      // Save to flash
        Send_Power_Failure_Alert(); // Send LoRaWAN alert
        Enter_Backup_Mode();       // Switch to battery
    }
}
```

---

## 🔐 **Phase 6: Security Enhancements**

### **6.1 Secure Boot:**
- Enable STM32 read protection (Level 1)
- Implement firmware signature verification
- Encrypted firmware updates via LoRaWAN

### **6.2 Key Storage:**
```c
// Store sensitive data in flash with protection
// Use STM32 internal flash pages with write protection

// Never hardcode keys in source!
// Load from secure storage at runtime
```

### **6.3 Communication Security:**
```c
// LoRaWAN provides:
// ✅ AES-128 encryption
// ✅ Frame counter (prevent replay)
// ✅ Message integrity check (MIC)

// Additional measures:
// - Unique DevEUI per device
// - Rotate AppKey periodically
// - Monitor for anomalies
```

### **6.4 Physical Security:**
```c
// Tamper detection
#define TAMPER_DETECT_Pin GPIO_PIN_8
#define COVER_OPEN_Pin    GPIO_PIN_9

if (HAL_GPIO_ReadPin(GPIOA, TAMPER_DETECT_Pin) == GPIO_PIN_SET) {
    // Tamper detected!
    Log_Tamper_Event();
    Send_Tamper_Alert();
    Lock_Device();  // Disable until reset
}
```

---

## 📈 **Phase 7: Calibration & Accuracy**

### **7.1 Calibration Procedure:**

```c
// Multi-point calibration for accuracy

typedef struct {
    float voltage_points[3];    // [low, mid, high]
    float current_points[3];
    float voltage_corrections[3];
    float current_corrections[3];
} CalibrationTable_t;

// Calibration process:
// 1. Apply known voltage (e.g., 230V from calibrated source)
// 2. Measure ADC value
// 3. Calculate correction factor
// 4. Store in flash
// 5. Repeat for current

// Accuracy target: ±1% for Class 1 meter (IEC 62053)
```

### **7.2 Temperature Compensation:**
```c
// ADC and sensors drift with temperature
// Measure temperature and apply correction

float Get_Temperature_Compensated_Voltage(float raw_voltage) {
    float temp = Read_MCU_Temperature();
    float temp_coefficient = 0.0002;  // 0.02% per °C
    float compensation = 1.0 + (temp - 25.0) * temp_coefficient;
    return raw_voltage * compensation;
}
```

---

## 📋 **Phase 8: Compliance & Standards**

### **8.1 Metering Standards:**

#### **IEC Standards:**
- **IEC 62053-21** - Static meters for AC active energy (Class 1 & 2)
- **IEC 62052-11** - General requirements for meters
- **IEC 62056** - DLMS/COSEM protocol (if needed)

#### **Accuracy Classes:**
- **Class 0.5** - ±0.5% error (high-end meters)
- **Class 1** - ±1% error (residential/commercial)
- **Class 2** - ±2% error (basic meters)

### **8.2 LoRaWAN Compliance:**
- LoRaWAN 1.0.3 or 1.1 specification
- Regional frequency plan compliance (EU868/US915/etc.)
- LoRa Alliance certification (optional but recommended)

### **8.3 Safety Standards:**
- **IEC 61010** - Safety requirements for electrical equipment
- **UL 61010** - North American safety standard
- Proper isolation (2.5kV minimum)
- Overcurrent/overvoltage protection

### **8.4 EMC Compliance:**
- **EN 61326** - EMC requirements for measurement equipment
- ESD protection (IEC 61000-4-2)
- RF immunity testing

---

## 🏗️ **Phase 9: Software Architecture**

### **9.1 Modular Code Structure:**

```
Project Structure:
├── Core/
│   ├── Src/
│   │   ├── main.c                  // Main application
│   │   ├── energy_meter.c          // Energy measurement module
│   │   ├── lorawan_comm.c          // LoRaWAN communication
│   │   ├── data_logger.c           // Data logging to flash
│   │   ├── calibration.c           // Calibration functions
│   │   ├── security.c              // Security & encryption
│   │   └── error_handler.c         // Error detection
│   └── Inc/
│       ├── energy_meter.h
│       ├── lorawan_comm.h
│       ├── data_logger.h
│       └── config.h                // System configuration
```

### **9.2 State Machine:**

```c
typedef enum {
    STATE_INIT,
    STATE_CALIBRATE,
    STATE_MEASURE,
    STATE_TRANSMIT,
    STATE_SLEEP,
    STATE_ERROR
} SystemState_t;

void State_Machine(void) {
    switch (current_state) {
        case STATE_INIT:
            Initialize_System();
            current_state = STATE_CALIBRATE;
            break;
            
        case STATE_CALIBRATE:
            Load_Calibration();
            current_state = STATE_MEASURE;
            break;
            
        case STATE_MEASURE:
            Sample_Energy();
            Calculate_Parameters();
            if (time_to_transmit) {
                current_state = STATE_TRANSMIT;
            }
            break;
            
        case STATE_TRANSMIT:
            Send_LoRaWAN_Data();
            current_state = STATE_SLEEP;
            break;
            
        case STATE_SLEEP:
            Enter_Low_Power_Mode();
            // Wake up timer triggers next cycle
            current_state = STATE_MEASURE;
            break;
            
        case STATE_ERROR:
            Handle_Error();
            break;
    }
}
```

---

## 🚀 **Implementation Roadmap**

### **Phase 1 (Week 1-2): Foundation**
- [ ] Implement secure LoRaWAN (OTAA)
- [ ] Register device on network (TTN/Helium/Private)
- [ ] Test bi-directional communication
- [ ] Implement basic error handling

### **Phase 2 (Week 3-4): Energy Measurement**
- [ ] Connect voltage sensor (ZMPT101B)
- [ ] Connect current sensor (ACS712)
- [ ] Implement ADC sampling (DMA)
- [ ] Calculate RMS values
- [ ] Display on LCD

### **Phase 3 (Week 5-6): Data Processing**
- [ ] Calculate power, energy, PF
- [ ] Implement calibration
- [ ] Add data logging to flash
- [ ] Implement RTC for timestamps

### **Phase 4 (Week 7-8): Communication**
- [ ] Optimize LoRaWAN payload
- [ ] Implement transmission schedule
- [ ] Add downlink command handling
- [ ] Test confirmed/unconfirmed messages

### **Phase 5 (Week 9-10): Reliability**
- [ ] Add watchdog timer
- [ ] Implement error detection
- [ ] Add tamper detection
- [ ] Power failure handling

### **Phase 6 (Week 11-12): Optimization**
- [ ] Power optimization (low power modes)
- [ ] Temperature compensation
- [ ] Long-term stability testing
- [ ] Field testing

### **Phase 7 (Week 13-14): Polish & Certification**
- [ ] Final calibration
- [ ] Accuracy testing
- [ ] Documentation
- [ ] Compliance testing prep

---

## 📊 **Key Performance Indicators (KPIs)**

### **Measurement Accuracy:**
- Voltage: ±1% (Class 1)
- Current: ±1% (Class 1)
- Energy: ±1% (Class 1)

### **Communication Reliability:**
- Message delivery rate: >99%
- Join success rate: >95%
- Average latency: <5 seconds

### **Power Consumption:**
- Active measurement: <50mA @ 3.3V
- Sleep mode: <1mA
- Battery life: >10 years (with primary AC power)

### **Data Storage:**
- Minimum 30 days of 15-minute interval data
- Tamper events logged
- Power failure events logged

---

## 🔧 **Required Tools & Equipment**

### **Development:**
- STM32CubeIDE
- STM32CubeProgrammer
- Serial terminal (PuTTY/TeraTerm)
- Logic analyzer (optional)

### **Calibration:**
- Precision voltage source
- Precision current source
- True RMS multimeter (calibrated)
- Power analyzer (optional)

### **Testing:**
- LoRaWAN gateway (for testing)
- AC power source (variable)
- Load bank (resistive/inductive)
- Oscilloscope

---

## 💰 **Cost Estimation**

### **Per Unit (Production Volume: 1000+):**
- STM32L432KC: $3-4
- Wio-E5 Mini: $8-10
- Voltage sensor: $2-3
- Current sensor: $2-3
- PCB: $2-5
- Enclosure: $3-5
- **Total BOM: ~$20-30**

### **Certification Costs:**
- LoRa Alliance certification: $1,000-5,000
- Safety certification (UL/CE): $5,000-15,000
- Metering accuracy certification: $2,000-10,000

---

**Next Steps:** Shall I create the initial code modules for energy measurement and secure LoRaWAN communication?

