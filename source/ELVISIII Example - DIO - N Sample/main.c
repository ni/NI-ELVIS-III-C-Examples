/**
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */

#include <stdio.h>
#include <time.h>
#include "DIO_N_Sample.h"
#include "NiELVISIIIv10.h"

#if !defined(LoopDuration)
#define LoopDuration    60  /* How long to output the signal, in seconds */
#endif

//Default FIFO size.
#define FIFO_SIZE 100

extern ELVISIII_Dio connector_A;
extern ELVISIII_Dio connector_B;

/**
 * Overview:
 * Demonstrates using the digital input and output in N Sample (DIO - N Sample).
 * Read a groups of values from a DI FIFO (connector A, channel 0).
 * Write a groups of values to a DO FIFO (connector B, channel 0).
 *
 * Instructions:
 * 1. Use an oscilloprobe to connect FGen (one side) and DIO0, DGND (another side) on connector A.
 * 2. Use an oscilloprobe to connect Oscilloscope (one side) and DIO0, DGND (another side) on connector B.
 * 3. Open FGen and set a square wave in 250Hz, 4Vpp and 2V DC offset.
 * 4. Open Oscilloscope and set Normal mode. The Level is 1V and the Type is Analog edge.
 * 5. Run this program.
 *
 * Output:
 * The program reads groups of values from a DI FIFO in channel 0, connector A.
 * The program writes groups of values to a DO FIFO in channel 0, connector B.
 * The groups of values reading from the DI FIFO will be printed on the console.
 * The output is maintained for 60 s.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the ELVIS III.
 */
int main()
{
	NiFpga_Status status;

	time_t currentTime;
	time_t finalTime;

	uint64_t fxp_buffer_recv[FIFO_SIZE];
	uint64_t fxp_buffer_send[] = {0, 1, 0, 1, 0, 1, 0, 1};
	NiFpga_Bool val[FIFO_SIZE];

	printf("DigitalInputOutput - N Sample:\n");

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
	 * Set the DIO Direction Register.
	 */
	Di_Direction(&connector_A, Di_Channel0);

	/*
	 * Set the DI Divisor Register.
	 */
	Di_Divisor(&connector_A, 40000000, 1000);

	/*
	 * Configure the DI DMA Enable Register.
	 */
	Di_Enable(&connector_A);

	/*
	 * Read FXP from a DI FIFO.
	 */
	Di_ReadFifo(&connector_A,
				TargetToHost_FIFO_FXP_A,
				fxp_buffer_recv,
				FIFO_SIZE,
				NiFpga_InfiniteTimeout,
				NULL);

	/*
	 * Get groups of values from one channel.
	 */
	Di_GetVal(Di_Channel0, fxp_buffer_recv, FIFO_SIZE, val);

	/*
	 * Print the values of DIO0.
	 */
	printf("Channel%d:\n", Di_Channel0);
	int i;
	for (i = 0; i < FIFO_SIZE; ++i)
	{
		printf("%d ", val[i]);
		if ((i + 1) % 20 == 0)
			printf("\n");
	}
	printf("\n");

	/*
	 * Set the DIO Direction Register.
	 */
	Do_Direction(&connector_B, Di_Channel0);

	/*
	 * Set the DO Divisor Register.
	 */
	Do_Divisor(&connector_B, 40000000, 1000);

	/*
	 * Configure the DO DMA Enable Register.
	 */
	Do_Enable(&connector_B, Di_Channel0);

	/*
	 * Write FXP to a DO FIFO.
	 */
	Do_WriteFifo(&connector_B,
				 HostToTarget_FIFO_FXP_B,
				 fxp_buffer_send,
				 sizeof(fxp_buffer_send)/sizeof(uint64_t),
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
