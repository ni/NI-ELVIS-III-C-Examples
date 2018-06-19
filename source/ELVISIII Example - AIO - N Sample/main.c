/**
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 *
 * Overview:
 * Demonstrates using the analog input and output of N Sample (AIO - N Sample).
 * Read a group of values from one analog input channel on bank A.
 * Write a group of values to one analog output channel on bank B.
 *
 * Instructions:
 * 1. Install an oscilloprobe on one of the Function Generator on NI ELVIS III device.
 *    Use the oscilloprobe to probe AI0 on bank A. Use AGND as ground.
 * 2. Install an oscilloprobe on one of the Oscilloscope on NI ELVIS III device.
 *    Use the oscilloprobe to probe AO0 on bank B. Use AGND as ground.
 * 3. Open "Measurements Live" (measurementslive.ni.com) and connect to the NI ELVIS III device.
 * 4. In Instruments tab, open Function and Arbitrary Waveform Generator.
 * 	  Set a square wave in 500Hz, 4Vpp and 0V DC offset.
 * 5. In Instruments tab, open Oscilloscope.
 * 	  Set Normal mode, analog edge type and 1V level.
 * 6. Run this program.
 *
 * Output:
 * The program reads groups of values from AI0 on bank A.
 * The program writes groups of initial values to AO0 on bank B.
 * The output is maintained for 60 s.
 * The group of values reading from the AI0 is written to the console.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the ELVIS III.
 */
#include <stdio.h>
#include <time.h>

#include "AIO_N_Sample.h"
#include "NiELVISIIIv10.h"

#if !defined(LoopDuration)
#define LoopDuration    60  // How long to output the signal, in seconds 
#endif

//Default FIFO size.
#define FIFO_SIZE 100

extern ELVISIII_Aio bank_A;
extern ELVISIII_Aio bank_B;

int main(int argc, char **argv)
{
    NiFpga_Status status;

    double value[FIFO_SIZE];
    uint64_t fxp_buffer_receive[FIFO_SIZE];

    double send[] = {2, 2, 2, 2, -2, -2, -2, -2};
    uint64_t fxp_buffer_send[FIFO_SIZE];

    time_t currentTime;
    time_t finalTime;

    printf("AnalogInputOutput - N Sample:\n");

    // Open the ELVIS III NiFpga Session.
    // This function MUST be called before all other functions. After this call
    // is complete the ELVIS III target will be ready to be used.
    status = NiELVISIIIv10_Open();
    if (NiELVISIIIv10_IsNotSuccess(status))
    {
        return status;
    }

    // Set the number of valid channels on bank A.
    Ai_Counter(&bank_A, 1);

    // Configure the range of the AI channel on bank A.
    Ai_Configure(&bank_A, Ai_Channel0, Ai_Range0);

    // Configure the divisor for the AI sample rate on bank A.
    Ai_Divisor(&bank_A, 40000000, 1000);

    // Set the DMA Enable Flag for bank A.
    Ai_Enable(&bank_A);

    // Read fixed-point values from an AI FIFO on bank A.
    Ai_ReadFifo(&bank_A,
                TargetToHost_FIFO_FXP_A,
                fxp_buffer_receive,
                FIFO_SIZE,
                NiFpga_InfiniteTimeout,
                NULL);

    // Convert fixed-point values of the FIFO to double values.
    // The fixed-point value is an unsigned long long int value.
    ConvertU64ArrayToDoubleArray(fxp_buffer_receive, FIFO_SIZE, value);
	
    // Print out the values of A/AI0.
    printf("Channel%d:\n", Ai_Channel0 - RSE_NUM);
    int i;
    for (i = 0; i < FIFO_SIZE; ++i)
    {
        printf("%f ", value[i]);
        if ((i + 1) % 10 == 0)
            printf("\n");
    }
    printf("\n");

    // Configure the divisor for the AO sample rate on bank B.
    Ao_Divisor(&bank_B, 40000000, 1000);

    // Set the DMA Enable Flag for AO0 on bank B.
    Ao_Enable(&bank_B, Ao_Channel0);

    // Convert double values to fixed-point values of the FIFO.
    // The fixed-point value is an unsigned long long int value.
    ConvertDoubleArrayToU64Array(send, fxp_buffer_send, sizeof(send)/sizeof(uint64_t));
	
    // Write fixed-point values to an AO FIFO on bank B.
    Ao_WriteFifo(&bank_B,
                 HostToTarget_FIFO_FXP_B,
                 fxp_buffer_send,
                 (sizeof(fxp_buffer_send)/sizeof(uint64_t)),
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
