/**
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 *
 * Overview:
 * Demonstrates using the analog input and output (AIO).
 * Writes an initial value on connector A.Reads the write value from connector A.
 * Reads the difference of the initial values of two analog input channels from connector B.
 * Print the value and the difference to the console.
 *
 *
 * Instructions:
 * 1. Connect the AI0 and AO0 on connector A.
 * 2. Connect a DC voltage input (between -5 V and 5 V) to AI1 on connector B.
 * 3. Connect a DC voltage input (between -5 V and 5 V) to AI5 on connector B.
 * 4. Run this program.
 *
 * Output:
 * The program writes the initial value to AO0 on connector A, and reads the write value from AI0.
 * The program reads the difference of the initial voltage on AI1 and AI5 on connector B.
 * The output is maintained for 60 s.
 * The write value and the difference are written to the console.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the ELVIS III.
 */
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include "AIO.h"
#include "NiELVISIIIv10.h"

#if !defined(LoopDuration)
#define LoopDuration    60  // How long to output the signal, in seconds 
#endif

extern ELVISIII_Ai connector_A;
extern ELVISIII_Ai connector_B;
extern ELVISIII_Ao Ao;

int main(int argc, char **argv)
{
    NiFpga_Status status;

    time_t currentTime;
    time_t finalTime;

    printf("AnalogInputOutput:\n");

    // Open the ELVIS III NiFpga Session.
    // This function MUST be called before all other functions. After this call
    // is complete the ELVIS III target will be ready to be used.
    status = NiELVISIIIv10_Open();
    if (NiELVISIIIv10_IsNotSuccess(status))
    {
        return status;
    }

    // Write the value to A/AO0 to set it to the desired value.
    // The channel is on connector A.
    Aio_Write(&Ao, 3, AO_A0_VAL);

    // Set the number of valid channels on connector A.
    Ai_Counter(&connector_A, 12);

    // Configure the range of the analog input channel on connector A.
    Ai_Configure(&connector_A, Ai_Channel0, Ai_Range0);

    // Configure the divisor for the analog sample rate on connector A.
    Ai_Divisor(&connector_A, 40000000, 1000);

    // Sleep before reading.Wait for the value writing to the register.
    sleep (1);

    // Read from AI channel A/AI0.
    // The channel is on connector A.
    double ai_A0 = Aio_Read(&connector_A, Ai_Channel0);

    // Print out the desired logic level of A/AI0.
    printf("ai_A0   = %f\n", ai_A0);

    // Set the number of valid channels on connector B.
    Ai_Counter(&connector_B, 12);

    // Configure the range of the analog input channel on connector B.
    Ai_Configure(&connector_B, Ai_Channel1_5, Ai_Range1);

    // Configure the divisor for the analog sample rate on connector B.
    Ai_Divisor(&connector_B, 40000000, 1000);

    // Read from AI channel B/AI1 and B/AI5.
    // Each channel is on connector B.
    double ai_B1_5 = Aio_Read(&connector_B, Ai_Channel1_5);

    // Print the difference of the values in two channels.
    printf("ai_B1_5 = %f\n", ai_B1_5);

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
