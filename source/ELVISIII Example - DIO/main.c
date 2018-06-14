/**
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */
/**
 * Overview:
 * Demonstrates using the digital input and output (DIO).
 * Write an initial value to DIO1 on connector A.
 * Read this value from DIO0 on connector A.
 * Read an initial value from DIO0 on connector B.
 *
 * Instructions:
 * 1. Connect the DIO0 and DIO1 on connector A.
 * 2. Connect an input (5 V, 3.3 V, DGND) to DIO0 on connector B.
 * 3. Run this program.
 *
 * Output:
 * The program writes the initial value to DIO1 on connector A, and reads the write value from DIO0.
 * The program reads the initial value from DIO0 on connector B.
 * The output is maintained for 60 s.
 * Values are written to the console.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the ELVIS III.
 */
#include <stdio.h>
#include <time.h>
#include "DIO.h"
#include "NiELVISIIIv10.h"

#if !defined(LoopDuration)
#define LoopDuration    60  /* How long to output the signal, in seconds */
#endif

extern ELVISIII_Dio connector_A;
extern ELVISIII_Dio connector_B;


int main(int argc, char **argv)
{
    NiFpga_Status status;

    time_t currentTime;
    time_t finalTime;

    printf("DigitalInputOutput:\n");

    /*
     * Open the ELVIS III NiFpga Session.
     * This function MUST be called before all other functions. After this call
     * is complete the ELVIS III target will be ready to be used.
     */
    status = NiELVISIIIv10_Open();
    if (NiELVISIIIv10_IsNotSuccess(status))
    {
        return status;
    }

    /*
     * Write the initial value to channel DIO1, connector_A.
     */
    Dio_WriteBit(&connector_A, false, Dio_Channel1);

    /*
     * Get the value in channel DIO0, connector_A.
     */
    NiFpga_Bool di_A0 = Dio_ReadBit(&connector_A, Dio_Channel0);

    /*
     * Get the value in channel DIO0, connector_B.
     */
    NiFpga_Bool di_B0 = Dio_ReadBit(&connector_B, Dio_Channel0);

    /*
     * Print out the logic level of each channel.
     */
    printf("di_A0 = %d\n", di_A0);
    printf("di_B0 = %d\n", di_B0);

    /*
     * Normally, the main function runs a long running or infinite loop.
     * Keep the program running so that you can measure the output using
     * an external instrument.
     */
    time(&currentTime);
    finalTime = currentTime + LoopDuration;
    while (currentTime < finalTime)
    {
        time(&currentTime);
    }

    /*
     * Close the ELVISIII NiFpga Session.
     * This function MUST be called after all other functions.
     */
    status = NiELVISIIIv10_Close();

    /*
     * Returns 0 if successful.
     */
    return status;
}
