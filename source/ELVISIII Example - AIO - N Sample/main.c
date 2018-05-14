/**
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */

#include <stdio.h>
#include <time.h>

#include "AIO_N_Sample.h"
#include "NiELVISIIIv10.h"

#if !defined(LoopDuration)
#define LoopDuration    60  /* How long to output the signal, in seconds */
#endif

//Default FIFO size.
#define FIFO_SIZE 100

extern ELVISIII_Aio connector_A;
extern ELVISIII_Aio connector_B;

/**
 * Overview:
 * Demonstrates using the analog input and output in N Sample (AIO - N Sample).
 * Read a groups of values from a AI FIFO (connector A, channel 0).
 * Write a groups of values to a AO FIFO (connector B, channel 0).
 *
 * Instructions:
 * 1. Use an oscilloprobe to connect FGen (one side) and AI0, AGND (another side) on connector A.
 * 2. Use an oscilloprobe to connect Oscilloscope (one side) and AO0, AGND (another side) on connector B.
 * 3. Open FGen and set a square wave in 500Hz, 4Vpp and 0V DC offset.
 * 4. Open Oscilloscope and set Normal mode. The Level is 1V and the Type is Analog edge.
 * 5. Run this program.
 *
 * Output:
 * The program reads groups of values from a AI FIFO in channel 0, connector A.
 * The program writes groups of values to a AO FIFO in channel 0, connector B.
 * The groups of values reading from the AI FIFO will be printed on the console.
 * The output is maintained for 60 s.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the ELVIS III.
 */
int main(int argc, char **argv)
{
	NiFpga_Status status;

	double val[FIFO_SIZE];
	uint64_t fxp_buffer_recv[FIFO_SIZE];

	double send[] = {2, 2, 2, 2, -2, -2, -2, -2};
	uint64_t fxp_buffer_send[FIFO_SIZE];

	time_t currentTime;
	time_t finalTime;

	printf("AnalogInputOutput - N Sample:\n");

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
	 * Set the AI Counter Register.
	 */
	Ai_Counter(&connector_A, 1);

	/*
	 * Set the AI Configuration Register.
	 */
	Ai_Configure(&connector_A, Ai_Channel0, Ai_Range0);

	/*
	 * Set the AI Divisor Register.
	 */
	Ai_Divisor(&connector_A, 40000000, 1000);

	/*
	 * Set the AI DMA Enable Register.
	 */
	Ai_Enable(&connector_A);

	/*
	 * Read FXP from a AI FIFO.
	 */
	Ai_ReadFifo(&connector_A,
			    TargetToHost_FIFO_FXP_A,
				fxp_buffer_recv,
				FIFO_SIZE,
				NiFpga_InfiniteTimeout,
				NULL);

	/*
	 * Get groups of values from one channel.
	 */
	Ai_GetVal(fxp_buffer_recv, FIFO_SIZE, val);

	/*
	 * Print the values of AIO0.
	 */
	printf("Channel%d:\n", Ai_Channel0 - RSE_NUM);
	int i;
	for (i = 0; i < FIFO_SIZE; ++i)
	{
		printf("%f ", val[i]);
		if ((i + 1) % 10 == 0)
			printf("\n");
	}
	printf("\n");

	/*
	 * Set the AO Divisor Register.
	 */
	Ao_Divisor(&connector_B, 40000000, 1000);

	/**
	 * Set the AO DMA Enable Register
	 */
	Ao_Enable(&connector_B, Ao_Channel0);

	/*
	 * Turn groups of values from double to Fix Point.
	 */
	Ao_SetVal(send, fxp_buffer_send, sizeof(send)/sizeof(uint64_t));

	/*
	 * Read FXP from a AI FIFO.
	 */
	Ao_WriteFifo(&connector_B,
				 HostToTarget_FIFO_FXP_B,
				 fxp_buffer_send,
				 (sizeof(fxp_buffer_send)/sizeof(uint64_t)),
				 NiFpga_InfiniteTimeout,
				 NULL);

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
