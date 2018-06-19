/**
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 *
 * Overview:
 * Demonstrates using the PWM.
 * Generates a PWM signal from PWM 0 on bank A.
 *
 * Instructions:
 * 1. Connect an oscilloscope to the PWM 0 pin on bank A.
 * 2. Open MeasurementsLive website and connect the ELVISIII.
 * 4. In Instruments tab, Open Oscilloscope, generate a PWM signal.
 * 3. Run this program.
 *
 * Output:
 * The program generates a 25% duty cycle signal at 10 kHz for 60 s.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the ELVIS III.
 */
#include <stdio.h>
#include <time.h>
#include "PWM.h"
#include "NiELVISIIIv10.h"

#if !defined(LoopDuration)
#define LoopDuration    60  // How long to output the signal, in seconds 
#endif

extern ELVISIII_Pwm bank_A;

int main(int argc, char **argv)
{
    NiFpga_Status status;

    time_t currentTime;
    time_t finalTime;

    printf("PWM:\n");

    // Open the ELVIS III NiFpga Session.
    // This function MUST be called before all other functions. After this call
    // is complete the ELVIS III target will be ready to be used.
    status = NiELVISIIIv10_Open();
    if (NiELVISIIIv10_IsNotSuccess(status))
    {
        return status;
    }

    // Set the waveform, enabling the PWM onboard device.
    Pwm_Configure(&bank_A, Pwm_Channel0, Pwm_Invert | Pwm_Mode, Pwm_NotInverted | Pwm_Enabled);

    // Set the clock divider. The internal PWM counter will increments at f_clk / 4
    // where: f_clk = the frequency of the myRIO FPGA clock (40 MHz default)
    Pwm_ClockSelect(&bank_A, Pwm_Channel0, Pwm_4x);

    // Set the maximum counter value. The counter counts from 0 to 1000.
    // The counter increments at 40 MHz / 4 = 10 MHz and the counter counts
    // from 0 to 1000. The frequency of the PWM waveform is 10 MHz / 1000 = 10 kHz.
    Pwm_CounterMaximum(&bank_A, Pwm_Channel0, 1000);

    // Set the comparison counter value. The PWM counter counts from 0 to 1000
    // and outputs from 0 to the comparison value (250).
    // The duty cycle is 250 / 1000 = 25%.
    Pwm_CounterCompare(&bank_A, Pwm_Channel0, 250);

    // Write the value to PWM channel 0 on bank A.
    Pwm_Select(&bank_A, Pwm_Channel0);

    // Print the counter value.
    printf("%d\n", Pwm_Counter(&bank_A, Pwm_Channel0));

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
