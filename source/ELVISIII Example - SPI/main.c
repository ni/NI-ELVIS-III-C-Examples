/**
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 *
 * Overview:
 * Demonstrates using the SPI. Writes a message to the SPI bus, prints any
 * returned bytes to the console.
 *
 * Instructions:
 * 1. Connect SPI.CLK of the SPI slave to DIO5 on bank A.
 * 2. Connect SPI.MISO of the SPI slave to DIO6 on bank A.
 * 3. Connect SPI.MOSI of the SPI slave to DIO7 on bank A.
 * 4. Connect SPI.GND of the SPI slave to DGND on bank A.
 * 5. Connect SPI.CS of the SPI slave to DIO0 on bank A.
 * 6. Run the program.
 *
 * Output:
 * The program writes 0x80, which specifies the address to read from, to the
 * SPI device. The bit 7 refers to the writing/reading bit. We want to read
 * from the address 0x00, which means we have to set the bit 7 of 0x00 to 1.
 * Then the program reads back a value from the 0x00 register of the SPI device.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the NI ELVIS III.
 */
#include <stdio.h>
#include <time.h>
#include "SPI.h"
#include "DIO.h"
#include "NiELVISIIIv10.h"

#if !defined(LoopDuration)
#define LoopDuration    60  // How long to output the signal, in seconds 
#endif

extern ELVISIII_Dio bank_A;
extern ELVISIII_Spi spi_bank_A;

int main(int argc, char **argv)
{
    NiFpga_Status status;

    uint16_t data[1] = {0x80};

    uint16_t readChar;

    printf("SPI:\n");

    // Open the ELVIS III NiFpga Session.
    // This function MUST be called before all other functions. After this call
    // is complete the ELVIS III target will be ready to be used.
    status = NiELVISIIIv10_Open();
    if (NiELVISIIIv10_IsNotSuccess(status))
    {
        return status;
    }

    // Configure the SPI as leading clock phase, low clock polarity, most
    // significant bit first, 8 bits, and with a clock divider of 8X.
    Spi_Configure(&spi_bank_A,
                  Spi_ClockPhase | Spi_ClockPolarity | Spi_DataOrder | Spi_FrameLength | Spi_ClockDivider,
                  Spi_ClockPhaseTrailing | Spi_ClockPolarityHigh | Spi_DataOrderMsbFirst | Spi_FrameSize8 | Spi_Clock1x);

    // Set the maximum counter value. See NI_ELVIS_III_Shipping_Personality_Reference
    // for more details.
    Spi_CounterMaximum(&spi_bank_A, 127);

    // Write the value to the System Select Register.
    Spi_Select(&spi_bank_A);

    // Write the initial value to channel DIO0 which set the SPI.CS to low.
    Dio_WriteBit(&bank_A, false, Dio_Channel0);

    // Start transmit beween the slave device and ELVIS III
    readChar = Spi_Transmit(&spi_bank_A, data[0]);
    printf("%i\n",  (int) readChar);

    // Write the initial value to channel DIO0 which set the SPI.CS to high.
    Dio_WriteBit(&bank_A, true, Dio_Channel0);

    // Close the ELVISIII NiFpga Session.
    // This function MUST be called after all other functions.
    status = NiELVISIIIv10_Close();

    // Returns 0 if successful.
    return status;
}
