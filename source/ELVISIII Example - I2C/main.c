/**
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 *
 * Overview:
 * Demonstrates using the I2C. Read the temperature from a connected TMP102
 * digital temperature sensor and writes the response to the console.
 *
 * Instructions:
 * 1. Connect a TMP102 digital temperature sensor to the I2C pins on bank A.
 * 2. Connect I2C.SCL  to DIO14 on bank A.
 * 3. Connect I2C.SDA  to DIO15 on bank A.
 * 4. Connect GND of the sensor to DGND on bank A.
 * 4. Connect a DC voltage input (+3.3 V) to the sensor.
 * 2. Run the program.
 *
 * Output:
 * The program writes the read temperature to the console.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the NI ELVIS III.
 */
#include <stdio.h>
#include <time.h>
#include "I2C.h"
#include "NiELVISIIIv10.h"

#if !defined(LoopDuration)
#define LoopDuration    60  // How long to output the signal, in seconds 
#endif

extern ELVISIII_I2c bank_A;

int main(int argc, char **argv)
{
    NiFpga_Status status;

    uint8_t data[2] = {0x00, 0x80};
    int index;
    uint8_t slaveWriteAddress = 0x48;
    uint8_t slaveReadAddress  = 0x48;

    printf("I2C\n");

    // Open the ELVIS III NiFpga Session.
    // This function MUST be called before all other functions. After this call
    // is complete the ELVIS III target will be ready to be used.
    status = NiELVISIIIv10_Open();
    if (NiELVISIIIv10_IsNotSuccess(status))
    {
        return status;
    }

    // Write the value to the System Select Register on bank A.
    I2c_Select(&bank_A);

    // Set the speed of the I2C block.
    I2c_Counter(&bank_A, 187);

    // Configure the I2C block.
    I2c_Configure(&bank_A, I2c_Enabled);

    // Send 2 bytes to the slave device.
    I2c_Write(&bank_A, slaveWriteAddress, data, 2);

    // Read 10 bytes from the slave device.
    I2c_Read(&bank_A, slaveReadAddress, data, 2);

    // Print the data received from the slave.
    printf("Received data:");
    for (index = 0; index < 2; index++)
    {
        printf(" %.2X ", data[index]);
    }
    printf("\n");

    // Close the ELVISIII NiFpga Session.
    // This function MUST be called after all other functions.
    status = NiELVISIIIv10_Close();

    // Returns 0 if successful.
    return status;
}
