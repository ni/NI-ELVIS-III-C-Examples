/**
 * I2C.h
 *
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */

#ifndef I2C_h_
#define I2C_h_

#include "NiELVISIIIv10.h"

#if NiFpga_Cpp
extern "C" {
#endif

// I2C configuration register options.
typedef enum
{
    I2c_Disabled = 0x00,
    I2c_Enabled = 0x01
} I2c_ConfigureSettings;

// I2C control options.
typedef enum
{
    I2c_Rx_Tx = 1,   // 0b00000001 
    I2c_Start = 2,   // 0b00000010 
    I2c_Stop = 4,    // 0b00000100 
    I2c_Ack = 8      // 0b00001000 
} I2c_ControlBits;

// I2C status options.
typedef enum
{
    I2c_Busy = 1,         // 0b00000001 
    I2c_Error = 2,        // 0b00000010 
    I2c_Address_Nak = 4,  // 0b00000100 
    I2c_Data_Nak = 8,     // 0b00001000 
    I2c_In_Use = 16,      // 0b00010000 
    I2c_Bus_Busy = 32     // 0b00100000 
} I2c_StatusBits;

// Registers for a particular I2C.
typedef struct
{
    uint32_t addr;     // I2C slave address register
    uint32_t cnfg;     // I2C configuration register
    uint32_t cntl;     // I2C control register
    uint32_t cntr;     // I2C counter register
    uint32_t stat;     // I2C status register
    uint32_t dato;     // I2C data out register
    uint32_t dati;        // I2C data in register
    uint32_t go;          // I2C execution register
    uint32_t sel;      // System Select Register 
} ELVISIII_I2c;

// Configure the I2C block.
void I2c_Configure(ELVISIII_I2c* connector, I2c_ConfigureSettings settings);

// Set the speed of the I2C block.
void I2c_Counter(ELVISIII_I2c* connector, uint8_t speed);

// Write a series of bytes to the I2C channel.
void I2c_Write(ELVISIII_I2c* connector, uint8_t address, uint8_t* data, uint32_t numBytes);

// Read a series of bytes from the I2C channel.
void I2c_Read(ELVISIII_I2c* connector, uint8_t address, uint8_t* data, uint32_t numBytes);

// Write the value to the System Select Register.
void I2c_Select(ELVISIII_I2c* connector);

#if NiFpga_Cpp
}
#endif

#endif // I2C_h_ 
