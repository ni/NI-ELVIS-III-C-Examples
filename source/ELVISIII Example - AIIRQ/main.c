/**
 * Copyright (c) 2018,
 * National Instruments.
 * All rights reserved.
 *
 * Overview:
 * Demonstrates how to use the AI IRQ. Once the AI IRQ occurs,
 * print the IRQ number, trigger times and main loop count number in the console.
 * The output is maintained for 60 s.
 *
 * Instructions:
 * 1. Connect a signal, such as sine wave, to AI0 on bank A.
 * 2. Run this program and observe the console.
 *
 * Output:
 * The IRQ1, trigger times and main loop count number are shown in the console,
 * the output is maintained for 60 s.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the NI ELVIS III.
 */
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "AIIRQ.h"

#if !defined(LoopDuration)
#define LoopDuration          60   // How long to monitor the signal, in seconds
#endif

#if !defined(LoopSteps)
#define LoopSteps             3    // How long to step between printing, in seconds
#endif

extern ELVISIII_IrqAi bank_A;

// Resources for the new thread.
typedef struct
{
    NiFpga_IrqContext irqContext;      // IRQ context reserved by Irq_ReserveContext()
    NiFpga_Bool       irqThreadRdy;    // IRQ thread ready flag
    uint8_t           irqNumber;       // IRQ number value
} ThreadResource;

int main(int argc, char **argv)
{
    int32_t status;

    ThreadResource irqThread0;

    pthread_t thread;

    time_t currentTime;
    time_t finalTime;
    time_t printTime;

    // Configure the AI IRQ number, threshold, hysteresis, and trigger type.
    const uint8_t IrqNumberConfigure = 1;
    const double ThresholdConfigure = 4.0;
    const double HysteresisConfigure = 0.02;
    const Irq_Ai_Type TriggerTypeConfigure = Irq_Ai_RisingEdge;

    printf("Analog Input IRQ:\n");

    // Specify the AI IRQ supported I/O.
    bank_A.aiChannel = Irq_Ai_A0;

    // Initiate the IRQ number resource of the new thread.
    irqThread0.irqNumber = IrqNumberConfigure;

    // Open the ELVIS III NiFpga Session.
    // This function MUST be called before all other functions. After this call
    // is complete the ELVIS III target will be ready to be used.
    status = NiELVISIIIv10_Open();
    if (NiELVISIIIv10_IsNotSuccess(status))
    {
        return status;
    }

    // Set the number of valid channels on bank A.
    Ai_Counter(&bank_A, 2);

    // Configure the range of the A/AI0 on bank A.
    Ai_Configure(&bank_A, Ai_Channel0, Ai_Range0);

    // Configure the divisor for the analog sample rate on bank A.
    Ai_Divisor(&bank_A, 40000000, 1000);

    // Configure the AI0 IRQ and return a status message to indicate if the configuration is successful,
    // the error code is defined in IRQConfigure.h.
    status = Irq_RegisterAiIrq(&bank_A,
                               &(irqThread0.irqContext),
                               IrqNumberConfigure,
                               ThresholdConfigure,
                               HysteresisConfigure,
                               TriggerTypeConfigure);

    // Terminate the process if it is unsuccessful.
    if (status != NiELVISIIIv10_Status_Success)
    {
        printf("CONFIGURE ERROR: %d. Configuration of AI IRQ failed.", status);
        return status;
    }

    // Set the indicator to allow the new thread.
    irqThread0.irqThreadRdy = NiFpga_True;

    // Create new threads to catch the specified IRQ numbers.
    // Different IRQs should have different corresponding threads.
    status = pthread_create(&thread, NULL, AI_Irq_Thread, &irqThread0);
    if (status != NiELVISIIIv10_Status_Success)
    {
        printf("CONFIGURE ERROR: %d\n, Failed to create a new thread!", status);
        return status;
    }

    // Normally, the main function runs a long running or infinite loop.
    // Read the console output for 60 seconds so that you can recognize the
    // explanation and loop times.
    time(&currentTime);
    finalTime = currentTime + LoopDuration;
    printTime = currentTime;
    while (currentTime < finalTime)
    {
        static uint32_t loopCount = 0;
        time(&currentTime);

        // Do not print every loop iteration.
        if (currentTime > printTime)
        {
            printf("main loop,%d\n", ++loopCount);

            printTime += LoopSteps;
        }
    }

    // Set the indicator to end the new thread.
    irqThread0.irqThreadRdy = NiFpga_False;

    // Wait for the end of the IRQ thread.
    pthread_join(thread, NULL);

    // Disable AI0, so you can configure this I/O next time.
    // Every IrqConfigure() function should have its corresponding clear function,
    // and their parameters should also match.
    status = Irq_UnregisterAiIrq(&bank_A,
                                 irqThread0.irqContext,
                                 IrqNumberConfigure);

    if (status != NiELVISIIIv10_Status_Success)
    {
        printf("CONFIGURE ERROR: %d\n, Clear configuration of AI IRQ failed.", status);
        return status;
    }

    // Close the ELVISIII NiFpga Session.
    // This function MUST be called after all other functions.
    status = NiELVISIIIv10_Close();

    // Returns 0 if successful.
    return status;
}

void *AI_Irq_Thread(void* resource)
{
    ThreadResource* threadResource = (ThreadResource*) resource;
    while (1)
    {
        uint32_t irqAssert = 0;
        static uint32_t irqCount = 0;

        // Stop the calling thread, wait until a selected IRQ is asserted.
        Irq_Wait(threadResource->irqContext,
                 threadResource->irqNumber,
                 &irqAssert,
                 (NiFpga_Bool*) &(threadResource->irqThreadRdy));

        // If an IRQ was asserted.
        if (irqAssert & (1 << threadResource->irqNumber))
        {
            printf("IRQ%d,%d\n", threadResource->irqNumber, ++irqCount);

            // Acknowledge the IRQ(s) when the assertion is done.
            Irq_Acknowledge(irqAssert);
        }

        // Check the indicator to see if the new thread is stopped.
        if (!(threadResource->irqThreadRdy))
        {
            printf("The IRQ thread ends.\n");
            break;
        }
    }

    // Exit the new thread.
    pthread_exit(NULL);

    return NULL;
}
