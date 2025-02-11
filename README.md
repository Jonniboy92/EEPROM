# EEPROM

## 📜 Description

This library enables communication with an external I2C EEPROM module. It allows data storage and retrieval to maintain persistent information between microcontroller restarts.

## 📌 Features

- **Determine module parameters (example code) Module size and page space
- **Reading data
- **Writing data

## 🛠 EEPROM module parameters / initialization

When initializing the EEPROM module, you must specify its parameters.
If these are unknown, you can read this using the “FindModulParameter” example.

Example:

  #define EEPROM_Module_Space      0x7FFF // Exp.: 256Kbit * 1024 bit / 8 bit/byte == 0x7FFF bytes
  #define EEPROM_Number_of_Modul   0      // Number of EEPROM Module (if you have more than one module)
  #define EEPROM_Pages             64     // Page Space (See Datasheet of used Module)

  eeprom EEPROM(EEPROM_Module_Space,EEPROM_Pages,EEPROM_Number_of_Modul);

## 🚀 Usage

### FindModulParameter

With this example, the EEPROM module parameters can be read and checked. It helps to determine the characteristics of the EEPROM module used.

### TestPutAndGet

This example demonstrates reading and writing data in EEPROM. It shows how values ​​can be saved (put) and then read out again (get).
