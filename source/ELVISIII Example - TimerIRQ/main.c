/**
 * Copyright (c) 2018,
 * National Instruments.
 * All rights reserved.
 *
 * Overview:
 * Demonstrates how to use the timer IRQ. Once the timer IRQ occurs (5 s), print
 * the IRQ number, triggered times and main loop count number in the console.
 * The timer IRQ will be triggered only once in this example.
 * The output is maintained for 60 s.
 *
 * Instructions:
 * Run this program and observe the console.
 *
 * Output:
 * IRQ0, triggered times and main loop count number are shown in the console,
 * The output is maintained for 60 s.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the ELVIS III.
 */
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "TimerIRQ.h"

#if !defined(LoopDuration)
#define LoopDuration          60   // How long to monitor the signal, in seconds 
#endif

#if !defined(LoopSteps)
#define LoopSteps             3    // How long to step between printing, in seconds 
#endif

extern ELVISIII_IrqTimer IrqTimer;

// Resources for the new thread.
typedef struct
{
    NiFpga_IrqContext irqContext;      // IRQ context reserved by Irq_ReserveContext() 
    NiFpga_Bool irqThreadRdy;           // IRQ thread ready flag 
} ThreadResource;

int main(int argc, char **argv)
{
    int32_t status;

    ThreadResource irqThread0;

    pthread_t thread;

    time_t currentTime;
    time_t finalTime;
    time_t printTime;

    // Configure the timer IRQ and set the time interval. The IRQ occurs after the time interval.
    const uint32_t timeoutValue = 5000000;

    printf("Timer IRQ:\n");

    // Open the NiELVISIIIv10 NiFpga Session.
    // You must use this function before using all the other functions.
    // After you finish using this function, the NI NiELVISIIIv10 target is ready to be used.
    status = NiELVISIIIv10_Open();
    if (NiELVISIIIv10_IsNotSuccess(status))
    {
        return status;
    }

    // Configure the timer IRQ. The Time IRQ occurs only once after Timer_IrqConfigure().
    // If you want to trigger the timer IRQ repeatedly, use this function every time you trigger the IRQ.
    // Or you can put the IRQ in a loop.
    // Return a status message to indicate if the configuration is successful. The error code is defined in IRQConfigure.h.
    status = Irq_RegisterTimerIrq(&IrqTimer,
                                  &irqThread0.irqContext,
                                  timeoutValue);

    // Terminate the process if it is unsuccessful.
    if (status != NiELVISIIIv10_Status_Success)
    {
        printf("CONFIGURE ERROR: %d, Configuration of Timer IRQ failed.", status);
        return status;
    }

    // Set the indicator to allow the new thread.
    irqThread0.irqThreadRdy = NiFpga_True;

    // Create new threads to catch the specified IRQ numbers.
    // Different IRQs should have different corresponding threads.
    status = pthread_create(&thread, NULL, Timer_Irq_Thread, &irqThread0);
    if (status != NiELVISIIIv10_Status_Success)
    {
        printf("CONFIGURE ERROR: %d, Failed to create a new thread!", status);
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

        // Don't print every loop iteration. 
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

    // Disable timer interrupt, so you can configure this I/O next time.
    // Every IrqConfigure() function should have its corresponding clear function,
    // and their parameters should also match.
    status = Irq_UnregisterTimerIrq(&IrqTimer, irqThread0.irqContext);
    if (status != NiELVISIIIv10_Status_Success)
    {
        printf("CONFIGURE ERROR: %d\n", status);
        printf("Clear configuration of Timer IRQ failed.");
        return status;
    }

    // Close the NiELVISIIIv10 NiFpga Session.
    // You must use this function after using all the other functions.
    status = NiELVISIIIv10_Close();

    // Returns 0 if successful.
    return status;
}

void *Timer_Irq_Thread(void* resource)
{
    ThreadResource* threadResource = (ThreadResource*) resource;
    while (1)
    {
        uint32_t irqAssert = 0;
        static uint32_t irqCount = 0;

        // Stop the calling thread, wait until a selected IRQ is asserted.
        Irq_Wait(threadResource->irqContext,
                 TIMERIRQNO,
                 &irqAssert,
                 (NiFpga_Bool*) &(threadResource->irqThreadRdy));

        // If an IRQ was asserted.
        if (irqAssert & (1 << TIMERIRQNO))
        {
            printf("IRQ%d,%d\n", TIMERIRQNO, ++irqCount);

            // Acknowledge the IRQ(s) when assertion is done.
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
