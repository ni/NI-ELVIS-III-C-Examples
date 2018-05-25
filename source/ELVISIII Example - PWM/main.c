/**
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */

#include <stdio.h>
#include <time.h>
#include "PWM.h"
#include "NiELVISIIIv10.h"

#if !defined(LoopDuration)
#define LoopDuration    60  /* How long to output the signal, in seconds */
#endif

extern ELVISIII_Pwm connector_A;


/**
 * Overview:
 * Demonstrates using the PWM.
 * Generates a PWM signal from PWM 0 on connector A.
 *
 * Instructions:
 * 1. Connect an oscilloscope to the PWM 0 pin on connector A.
 * 2. Run this program.
 *
 * Output:
 * The program generates a 50% duty cycle signal at 10 kHz for 60 s.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the ELVIS III.
 */
int main(int argc, char **argv)
{
	NiFpga_Status status;

	time_t currentTime;
	time_t finalTime;

	printf("PWM:\n");

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
	 * Set the waveform, enabling the PWM onboard device.
	 */
	Pwm_Configure(&connector_A, Pwm_Channel0, Pwm_Invert | Pwm_Mode, Pwm_NotInverted | Pwm_Enabled);

	/*
	 * Set the clock divider. The internal PWM counter will increments at
	 * f_clk / 4
	 *
	 * where:
	 *  f_clk = the frequency of the myRIO FPGA clock (40 MHz default)
	 */
	Pwm_ClockSelect(&connector_A, Pwm_Channel0, Pwm_4x);

	/*
	 * Set the maximum counter value. The counter counts from 0 to 1000.
	 *
	 * The counter increments at 40 MHz / 4 = 10 MHz and the counter counts
	 * from 0 to 1000. The frequency of the PWM waveform is 10 MHz / 1000
	 * = 10 kHz.
	 */
	Pwm_CounterMaximum(&connector_A, Pwm_Channel0, 1000);

	/*
	 * Set the comparison counter value. The PWM counter counts from 0 to 1000
	 * and outputs from 0 to the comparison value (500).
	 *
	 * The duty cycle is 500 / 1000 = 50%.
	 */
	Pwm_CounterCompare(&connector_A, Pwm_Channel0, 500);

	/**
	 * Write the value to the System Select Register.
	 */
	Pwm_Select(&connector_A, Pwm_Channel0);

	/*
	 * Print the counter value.
	 */
	printf("%d\n", Pwm_Counter(&connector_A, Pwm_Channel0));

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