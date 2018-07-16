/**
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 *
 * Overview:
 * Demonstrates using the digital input and output in N samples mode (AIO - N Samples).
 * Reads a groups of values from one digital input channel (bank A, channel 0).
 * Writes a groups of values to one digital output channel (bank B, channel 0).
 *
 * Instructions:
 * 1. Install an oscilloprobe on one of the Function Generator channels on the NI ELVIS III.
 *    Use the oscilloprobe to probe DIO0 on bank A. Use DGND as ground.
 * 2. Install an oscilloprobe on one of the Oscilloscope channels on the NI ELVIS III.
 *    Use the oscilloprobe to probe DIO0 on bank B. Use DGND as ground.
 * 3. Open Measurements Live (measurementslive.ni.com) and connect to the NI ELVIS III.
 * 4. On the Instruments tab, launch the Function and Arbitrary Waveform Generator.
 *    Set a square wave of 250Hz, 4Vpp and 2V DC offset.
 * 5. On the Instruments tab, launch the Oscilloscope.
 *    Set the trigger mode as Normal, the trigger type as Analog edge, and the trigger level as 1V.
 * 6. Run this program.
 *
 * Output:
 * The program reads groups of values from DIO0 on bank A.
 * The program writes groups of values to DIO0 on bank B.
 * The groups of values read from DI0 will be printed on the console.
 * The output is maintained for 60 s.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the NI ELVIS III.
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

extern ELVISIII_Dio bank_A;
extern ELVISIII_Dio bank_B;

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

    // Set the Direction of the DIO0 on bank A.
    Di_Direction(&bank_A, Dio_Channel0);

    // Configure the divisor for the DI sample rate on bank A.
    Di_Divisor(&bank_A, 40000000, 1000);

    // Set the DI DMA Enable Flag for bank A.
    Di_Enable(&bank_A);

    // Read fixed-point values from a DI FIFO on bank A.
    Di_ReadFifo(&bank_A,
                TargetToHost_FIFO_FXP_A,
                fxp_buffer_receive,
                FIFO_SIZE,
                NiFpga_InfiniteTimeout,
                NULL);

    // Convert fixed-point values of the FIFO to boolean values.
    // The fixed-point value is an unsigned long long int value.
    ConvertU64ArrayToBoolArray(Dio_Channel0, fxp_buffer_receive, FIFO_SIZE, value);

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

    // Set the Direction of the DIO0 on bank B.
    Do_Direction(&bank_B, Dio_Channel0);

    // Configure the divisor for the DO sample rate on bank B.
    Do_Divisor(&bank_B, 40000000, 1000);

    // Set the DO DMA Enable Flag for DIO0 on bank B.
    Do_Enable(&bank_B, Dio_Channel0);

    // Write fixed-point values to a DO FIFO on bank B.
    Do_WriteFifo(&bank_B,
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
