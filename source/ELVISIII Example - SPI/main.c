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
 * 5. Run the program.
 *
 * Output:
 * The program writes the message "Hello World\n" for 60 s. Slave values are
 * written to the console every newline character.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the ELVIS III.
 */
#include <stdio.h>
#include <time.h>
#include "SPI.h"
#include "NiELVISIIIv10.h"

#if !defined(LoopDuration)
#define LoopDuration    60  // How long to output the signal, in seconds 
#endif

extern ELVISIII_Spi bank_A;

int main(int argc, char **argv)
{
    NiFpga_Status status;

    time_t currentTime;
    time_t finalTime;

    char writeMessage[]  = "Hello World\n";
    uint8_t writePos = 0;

    uint16_t readChar;

    char readMessage[100];
    uint8_t readPos = 0;

    printf("SPI:\n");

    // Open the ELVIS III NiFpga Session.
    // This function MUST be called before all other functions. After this call
    // is complete the ELVIS III target will be ready to be used.
    status = NiELVISIIIv10_Open();
    if (NiELVISIIIv10_IsNotSuccess(status))
    {
        return status;
    }

    // Write the value to the System Select Register.
    Spi_Select(&bank_A);

    // Configure the SPI as leading clock phase, low clock polarity, most
    // significant bit first, 8 bits, and with a clock divider of 8X.
    Spi_Configure(&bank_A,
                  Spi_ClockPhase | Spi_ClockPolarity | Spi_DataOrder | Spi_FrameLength | Spi_ClockDivider,
                  Spi_ClockPhaseLeading | Spi_ClockPolarityLow | Spi_DataOrderMsbFirst | Spi_FrameSize8 | Spi_Clock8x);

    // Set the maximum counter value. The counter counts from 0 to 1000.
    // The clock divider was previously set with a clock divider of 8X.
    // The counter increments at 40 MHz / 2 / 8 = 2.5 MHz and the counter
    // counts from 0 to 5624990000. The frequency of the SPI is 2.5 MHz / 62499 = 40 Hz.
    Spi_CounterMaximum(&bank_A, 62499U);

    // Normally, the main function runs a long running or infinite loop.
    // Keep the program running so that you can measure the output using
    // an external instrument.
    time(&currentTime);
    finalTime = currentTime + LoopDuration;
    while (currentTime < finalTime)
    {
        Spi_Transmit(&bank_A, writeMessage[writePos], &readChar);

        // Increment the position in the message. Reset to the beginning after
        // reaching the end of the message. -1 to not write the null terminating
        // character.
        ++writePos;
        if (writePos >= (sizeof writeMessage) - 1)
        {
            writePos = 0;
        }

        // If the character read is not 0, then add to the string. Otherwise,
        // interpret 0 as 'nothing was read'
        if (readChar != 0)
        {
            readMessage[readPos] = (char) readChar;

            // If this was a newline character, then print out the string
            // otherwise increment the position for the next loop iteration.
            if (readMessage[readPos] == '\n')
            {
                readMessage[readPos + 1] = '\0';
                printf("%s", readMessage);
                readPos = 0;
            }
            else if (readPos < (sizeof readMessage) - 1)
            {
                ++readPos;
            }
        }

        time(&currentTime);
    }

    // Close the ELVISIII NiFpga Session.
    // This function MUST be called after all other functions.
    status = NiELVISIIIv10_Close();

    // Returns 0 if successful.
    return status;
}
