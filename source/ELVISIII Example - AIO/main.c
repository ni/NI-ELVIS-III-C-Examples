/**
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */

#include <stdio.h>
#include <time.h>

#include "AIO.h"
#include "NiELVISIIIv10.h"

#if !defined(LoopDuration)
#define LoopDuration    60  /* How long to output the signal, in seconds */
#endif

extern ELVISIII_Ai connector_A;
extern ELVISIII_Ai connector_B;
extern ELVISIII_Ao Ao;

/**
 * Overview:
 * Demonstrates using the analog input and output (AIO).
 * Write an initial value to one AO Value Register.
 * Read this value from one AI Value Register in RSE mode, channel0, range ±10 V, connector A.
 * Read an initial value from one AI Value Register in DIFF mode, channel1_5, range ±5 V, connector B.
 *
 * Instructions:
 * 1. Connect the AI0 and AO0 on connector A.
 * 2. Connect a DC voltage input (between -5 V and 5 V) to AI1 and a DC voltage input (between -5 V and 5 V) to AI5 on connector B.
 * 3. Run this program.
 *
 * Output:
 * The program reads 2 values in voltages from proper channels on both connector A and connector B.
 * The output is maintained for 60 s.
 * Values and difference are written to the console.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the ELVIS III.
 */
int main(int argc, char **argv)
{
	NiFpga_Status status;

	time_t currentTime;
	time_t finalTime;

	printf("AnalogInputOutput:\n");

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
	 * Write the value to the proper AO Value Register.
	 */
	Aio_Write(&Ao, 1.5, AO_A0_VAL);

	/*
	 * Set the AI Counter Register.
	 */
	Ai_Counter(&connector_A, 12);

	/*
	 * Set the AI Configuration Register.
	 */
	Ai_Configure(&connector_A, Ai_Channel0, Ai_Range0);

	/*
	 * Set the AI Divisor Register.
	 */
	Ai_Divisor(&connector_A, 40000000, 1000);

	/*
	 * Get the Value from AI Value Register in RSE mode, channel0, range ±10 V, connector_A.
	 */
	double ai_A0 = Aio_Read(&connector_A, Ai_Channel0);

	/*
	 * Set the AI Counter Register.
	 */
	Ai_Counter(&connector_B, 12);

	/*
	 * Set the AI Configuration Register.
	 */
	Ai_Configure(&connector_B, Ai_Channel1_5, Ai_Range1);

	/*
	 * Set the AI Divisor Register.
	 */
	Ai_Divisor(&connector_B, 40000000, 1000);

	/*
	 * Get the Value from AI Value Register in DIFF mode, channel1_5, range ±5 V, connector_B.
	 */
	double ai_B1_5 = Aio_Read(&connector_B, Ai_Channel1_5);

	/*
	 * Print the value you get.
	 */
	printf("ai_A0   = %f\n", ai_A0);
	printf("ai_B1_5 = %f\n", ai_B1_5);

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
