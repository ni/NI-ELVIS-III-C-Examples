/**
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */

#include <stdio.h>
#include <time.h>
#include "Encoder.h"
#include "NiELVISIIIv10.h"

#if !defined(LoopDuration)
#define LoopDuration    60  /* How long to output the signal, in seconds */
#endif

#if !defined(LoopStep)
#define LoopStep        5   /* How long to monitor the signal, in seconds */
#endif

extern ELVISIII_Encoder connector_A;


/**
 * Overview:
 * Demonstrates using the encoder. Reads a step and direction signal from the
 * encoder on connector A. Prints the values to the console.
 *
 * Instructions:
 * 1. Connect ENC.A of the encoder to DIO0 on connector A.
 * 2. Connect ENC.B of the encoder to DIO1 on connector A.
 * 3. Connect GND of the encoder to DGND on connector A.
 * 4. Connect a DC voltage input (+5 V) to the encoder.
 * 2. Run this program.
 *
 * Output:
 * The program reads the encoder for 60 s. Encoder values are written to the
 * console every 5 s.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the ELVIS III.
 */
int main(int argc, char **argv)
{
	NiFpga_Status status;

	time_t currentTime;
	time_t printTime;
	time_t finalTime;

	uint32_t steps;
	const char* direction;

	printf("Encoder:\n");

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

	/**
	 * Write the value to the Encoder Channel 0 on connector A.
	 */
	Encoder_Select(&connector_A, Encoder_Channel0);

	/*
	 * Enable the encoder and configure to read step and direction signals.
	 */
	Encoder_Configure(&connector_A, Encoder_Channel0,
			          Encoder_Enable | Encoder_SignalMode,
			          Encoder_Enabled | Encoder_StepDirection);

	/*
	 * Normally, the main function runs a long running or infinite loop.
	 * Keep the program running so that you can measure the output using
	 * an external instrument.
	 */
	time(&currentTime);
	finalTime = currentTime + LoopDuration;
	printTime = currentTime;
	while (currentTime < finalTime)
	{
		time(&currentTime);

		/* Don't print every loop iteration. */
		if (currentTime > printTime)
		{
			steps = Encoder_Counter(&connector_A, Encoder_Channel0);
			if ((Encoder_Status(&connector_A, Encoder_Channel0) & Encoder_StDirection) == Encoder_Incrementing)
			{
				direction = "incrementing";
			}
			else
			{
				direction = "decrementing";
			}

			printf("Steps %d, Direction: %s\n", steps, direction);

			printTime += LoopStep;
		}
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
