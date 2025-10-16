# Wio-E5 Frequency Band Detection Guide

## 🌍 **Understanding LoRaWAN Frequency Bands**

LoRaWAN operates on different frequency bands depending on your geographic region. Using the wrong band is **illegal** and won't work with local gateways.

---

## 📡 **AT Commands to Check Frequency Band**

### **Method 1: Check Frequency Scheme (Recommended)**
```
AT+DR=SCHEME
```

**Expected Response:**
```
+DR: EU868
+DR: SCHEME
```
or
```
+DR: US915
+DR: SCHEME
```

This shows your current regional frequency plan.

---

### **Method 2: Check LoRaWAN Version and Region**
```
AT+LW=VER
```

**Expected Response:**
```
+LW: VER, V1.0.4
+LW: REGION, EU868
```
or
```
+LW: VER, V1.0.4
+LW: REGION, US915
```

This shows both LoRaWAN version and active region.

---

### **Method 3: Check Channel Configuration**
```
AT+CH
```

**Expected Response (EU868 example):**
```
+CH: 0, 868100000, DR0:DR5
+CH: 1, 868300000, DR0:DR5
+CH: 2, 868500000, DR0:DR5
```

**Expected Response (US915 example):**
```
+CH: 0, 902300000, DR0:DR3
+CH: 1, 902500000, DR0:DR3
+CH: 2, 902700000, DR0:DR3
...
+CH: 63, 927500000, DR4
```

Channel frequencies reveal the band:
- **860-870 MHz** → EU868, IN865
- **470-510 MHz** → CN470
- **902-928 MHz** → US915, AU915, AS923
- **920-925 MHz** → KR920, AS923

---

### **Method 4: Check Number of Channels**
```
AT+CH=NUM
```

**Expected Response:**
```
+CH: NUM, 3-16  (EU868 typically has 8-16 channels)
+CH: NUM, 72-96 (US915 typically has 72 channels)
```

---

## 🌐 **Common LoRaWAN Frequency Bands**

| Band | Region | Frequency Range | Channels | Data Rates |
|------|--------|-----------------|----------|------------|
| **EU868** | Europe, Middle East, Africa | 863-870 MHz | 8-16 | DR0-DR7 |
| **US915** | USA, Canada, South America | 902-928 MHz | 64-72 | DR0-DR4 |
| **AU915** | Australia, New Zealand | 915-928 MHz | 64-72 | DR0-DR6 |
| **AS923** | Asia (except China/Korea) | 915-928 MHz | 8-16 | DR0-DR7 |
| **KR920** | South Korea | 920-923 MHz | 8 | DR0-DR5 |
| **IN865** | India | 865-867 MHz | 8 | DR0-DR7 |
| **CN470** | China | 470-510 MHz | 96 | DR0-DR5 |

---

## 🔍 **How to Identify Your Module's Band**

### **Quick Test Sequence:**

```c
// In your STM32 code, add these commands:

// Get frequency scheme
WioE5_SendCommand("AT+DR=SCHEME");
WioE5_ReceiveResponse(1000);
LCD_DisplayResponse(wio_rx_buffer);

// Get LoRaWAN region
WioE5_SendCommand("AT+LW=VER");
WioE5_ReceiveResponse(1000);
LCD_DisplayResponse(wio_rx_buffer);

// Get first channel frequency
WioE5_SendCommand("AT+CH=0");
WioE5_ReceiveResponse(1000);
LCD_DisplayResponse(wio_rx_buffer);
```

---

## 📦 **Wio-E5-LE-Mini Variants**

Seeed Studio sells different variants:

| Model | Frequency Band | Region |
|-------|----------------|--------|
| **Wio-E5 Mini (STM32WLE5JC)** | EU868 | Europe |
| **Wio-E5 Mini US915** | US915 | Americas |
| **Wio-E5 Mini AU915** | AU915 | Australia |

**Note:** The frequency band is **firmware-dependent** and typically **cannot be changed** without reflashing the module.

---

## 🔧 **Your Test Results**

Your updated code will now show:

### **Test 7: Frequency Band**
```
Test 7: BAND
+DR: EU868        (or US915, AU915, etc.)
+DR: SCHEME
```

### **Test 7B: LoRaWAN Region**
```
LoRaWAN Region
+LW: VER, V1.0.4
+LW: REGION, EU868
```

### **Test 7C: Mode**
```
Test 7C: MODE
+MODE: LWOTAA
```

---

## ⚠️ **Important Notes**

### **1. Regional Compliance:**
- Using the **wrong frequency band is illegal** in most countries
- Always use the band designed for your region
- Check local regulations before transmitting

### **2. Gateway Compatibility:**
- Your Wio-E5 must match your gateway's frequency
- TTN/Helium gateways are region-specific
- EU868 module won't work with US915 gateways

### **3. Data Rates:**
- Different bands have different data rate capabilities
- US915: DR0-DR4 (SF12-SF7)
- EU868: DR0-DR7 (SF12-SF7, FSK)

---

## 🛠️ **If You Need to Change Frequency Band**

### **Option 1: Check if Module Supports Multiple Bands**
Some modules can switch bands:
```
AT+DR=SCHEME?  // List supported schemes
```

If supported, you can switch:
```
AT+DR=SCHEME,US915  // Switch to US915
AT+DR=SCHEME,EU868  // Switch to EU868
```

### **Option 2: Firmware Update**
If your module only supports one band, you'll need to:
1. Download appropriate firmware from Seeed Studio
2. Use STM32CubeProgrammer to reflash
3. Connect to BOOT0/RESET pins on Wio-E5
4. Flash new firmware with desired band

**⚠️ Warning:** Firmware update requires additional hardware and is risky!

---

## 📊 **Channel Configuration Examples**

### **EU868 Typical Channels:**
```
+CH: 0, 868100000, DR0:DR5  // 868.1 MHz
+CH: 1, 868300000, DR0:DR5  // 868.3 MHz
+CH: 2, 868500000, DR0:DR5  // 868.5 MHz
+CH: 3, 867100000, DR0:DR5  // 867.1 MHz
+CH: 4, 867300000, DR0:DR5  // 867.3 MHz
+CH: 5, 867500000, DR0:DR5  // 867.5 MHz
+CH: 6, 867700000, DR0:DR5  // 867.7 MHz
+CH: 7, 867900000, DR0:DR5  // 867.9 MHz
```

### **US915 Typical Channels (64 uplink channels):**
```
+CH: 0-7,   902.3-903.7 MHz  (125 kHz channels)
+CH: 8-15,  903.9-905.3 MHz
+CH: 16-23, 905.5-906.9 MHz
...
+CH: 64-71, 903.0-914.2 MHz  (500 kHz channels)
```

---

## 🧪 **Testing Your Band Configuration**

### **Step 1: Identify Band**
```c
WioE5_SendCommand("AT+DR=SCHEME");
WioE5_ReceiveResponse(1000);
// Check LCD response: EU868, US915, etc.
```

### **Step 2: Verify Region**
```c
WioE5_SendCommand("AT+LW=VER");
WioE5_ReceiveResponse(1000);
// Confirm region matches your location
```

### **Step 3: Check Channels**
```c
WioE5_SendCommand("AT+CH");
WioE5_ReceiveResponse(2000);
// Verify frequency range matches expected band
```

### **Step 4: Verify Gateway Match**
- Log into your LoRaWAN network (TTN, Helium)
- Check gateway frequency band
- Ensure module band matches gateway band

---

## ✅ **Quick Reference: What Should I Use?**

| Your Location | Use Band | Gateway Type |
|--------------|----------|--------------|
| Europe, UK, Middle East | **EU868** | EU868 gateways |
| USA, Canada, Mexico | **US915** | US915 gateways |
| Australia, New Zealand | **AU915** | AU915 gateways |
| Japan, Singapore, Taiwan | **AS923** | AS923 gateways |
| South Korea | **KR920** | KR920 gateways |
| India | **IN865** | IN865 gateways |
| China | **CN470** | CN470 gateways |

---

## 🔍 **Advanced: Decode Channel Numbers**

### **For US915:**
- Channels 0-63: 125 kHz uplink channels
- Channels 64-71: 500 kHz uplink channels
- Sub-bands (8 groups of 8 channels + 1 500kHz)

### **For EU868:**
- Channels 0-2: Mandatory (868.1, 868.3, 868.5 MHz)
- Channels 3+: Optional, added dynamically

---

## 📝 **Example Output from Your STM32**

When you run the updated code, expect to see:

```
Test 7: BAND
+DR: EU868 +DR: 
SCHEME

LoRaWAN Region
+LW: VER, V1.0.4
 +LW: REGION, EU

Test 7C: MODE
+MODE: LWOTAA

...

Loop: BAND
+DR: EU868 +DR: 
SCHEME
```

---

## 🌍 **Regulatory Compliance**

Remember:
- 📡 Each region has **legal frequency restrictions**
- 🚫 Transmitting on wrong band is **illegal**
- ✅ Always use **certified modules** for your region
- 📋 Check **local regulations** (FCC, CE, etc.)

---

## 🆘 **Troubleshooting**

### **Command Returns Error:**
```
+ERROR: -1
```
**Solution:** Your firmware may not support `AT+DR=SCHEME`. Try `AT+LW=VER` instead.

### **No Channels Shown:**
```
+CH: ERROR
```
**Solution:** Module not initialized. Send `AT+JOIN` first or check mode.

### **Wrong Band for Location:**
**Solution:** You need a different Wio-E5 module or firmware. Contact Seeed Studio for correct variant.

---

**Your code is now updated to detect and display the frequency band! Build and test to see your module's configuration.** 🎉

