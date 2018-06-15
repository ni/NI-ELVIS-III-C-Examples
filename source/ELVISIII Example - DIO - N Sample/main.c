/**
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 *
 * Overview:
 * Demonstrates using the digital input and output in N Sample (DIO - N Sample).
 * Read a groups of values from a DI FIFO (connector A, channel 0).
 * Write a groups of values to a DO FIFO (connector B, channel 0).
 *
 * Instructions:
 * 1. Install an oscilloprobe on one of the Function Generator on NI ELVIS III device.
 *    Use the oscilloprobe to probe DIO0 on connector A. Use DGND as ground.
 * 2. Install an oscilloprobe on one of the Oscilloscope on NI ELVIS III device.
 *    Use the oscilloprobe to probe DIO0 on connector B. Use DGND as ground.
 * 3. Open "Measurements Live" (measurementslive.ni.com) and connect to the NI ELVIS III device.
 * 4. In Instruments tab, open Function and Arbitrary Waveform Generator.
 *    Set a square wave in 250Hz, 4Vpp and 2V DC offset.
 * 5. In Instruments tab, open Oscilloscope and set Normal mode.The Level is 1V and the Type is Analog edge.
 * 6. Run this program.
 *
 * Output:
 * The program reads groups of values from DIO0 on connector A.
 * The program writes groups of values to DIO0 on connector B.
 * The groups of values reading from the DI0 will be printed on the console.
 * The output is maintained for 60 s.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the ELVIS III.
 */
#include <stdio.h>
#include <time.h>
#include "DIO_N_Sample.h"
#include "NiELVISIIIv10.h"

#if !defined(LoopDuration)
#define LoopDuration    60  // How long to output the signal, in seconds 
#endif

//Default FIFO size.
#define FIFO_SIZE 100

extern ELVISIII_Dio connector_A;
extern ELVISIII_Dio connector_B;

int main()
{
    NiFpga_Status status;

    time_t currentTime;
    time_t finalTime;

    uint64_t fxp_buffer_receive[FIFO_SIZE];
    uint64_t fxp_buffer_send[] = {0, 1, 0, 1, 0, 1, 0, 1};
    NiFpga_Bool value[FIFO_SIZE];

    printf("DigitalInputOutput - N Sample:\n");

    // Open the ELVIS III NiFpga Session.
    // This function MUST be called before all other functions. After this call
    // is complete the ELVIS III target will be ready to be used.
    status = NiELVISIIIv10_Open();
    if (NiELVISIIIv10_IsNotSuccess(status))
    {
        return status;
    }

    // Set the Direction of the DIO0 on connector A.
    Di_Direction(&connector_A, Dio_Channel0);

    // Configure the divisor for the DI sample rate on connector A.
    Di_Divisor(&connector_A, 40000000, 1000);

    // Set the DI DMA Enable Flag for connector A.
    Di_Enable(&connector_A);

    // Read fixed-point values from a DI FIFO on connector A.
    Di_ReadFifo(&connector_A,
                TargetToHost_FIFO_FXP_A,
                fxp_buffer_receive,
                FIFO_SIZE,
                NiFpga_InfiniteTimeout,
                NULL);

    // Convert fixed-point values of the FIFO to boolean values.
    // The fixed-point value is an unsigned long long int value.
    ConvertUnsignedLongLongIntToBool(Dio_Channel0, fxp_buffer_receive, FIFO_SIZE, value);

    // Print the values of A/DIO0.
    printf("Channel%d:\n", Dio_Channel0);
    int i;
    for (i = 0; i < FIFO_SIZE; ++i)
    {
        printf("%d ", value[i]);
        if ((i + 1) % 20 == 0)
            printf("\n");
    }
    printf("\n");

    // Set the Direction of the DIO0 on connector B.
    Do_Direction(&connector_B, Dio_Channel0);

    // Configure the divisor for the DO sample rate on connector B.
    Do_Divisor(&connector_B, 40000000, 1000);

    // Set the DO DMA Enable Flag for DIO0 on connector B.
    Do_Enable(&connector_B, Dio_Channel0);

    // Write fixed-point values to a DO FIFO on connector B.
    Do_WriteFifo(&connector_B,
                 HostToTarget_FIFO_FXP_B,
                 fxp_buffer_send,
                 sizeof(fxp_buffer_send)/sizeof(uint64_t),
                 NiFpga_InfiniteTimeout,
                 NULL);

    // Normally, the main function runs a long running or infinite loop.
    // Keep the program running so that you can measure the output using
    // an external instrument.
    time(&currentTime);
    finalTime = currentTime + LoopDuration;
    while (currentTime < finalTime)
    {
        time(&currentTime);
    }

    // Close the ELVISIII NiFpga Session.
    // This function MUST be called after all other functions.
    status = NiELVISIIIv10_Close();

    // Returns 0 if successful.
    return status;
}
