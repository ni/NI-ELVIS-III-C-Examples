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
 * Demonstrates using the analog input and output of N Sample (AIO - N Sample).
 * Read a group of values from one analog input channel on connector A.
 * Write a group of values to one analog output channel on connector B.
 *
 * Instructions:
 * 1. Use an oscilloprobe to connect one Function Generator (one side) and AI0, AGND (another side) on connector A.
 * 2. Use an oscilloprobe to connect one Oscilloscope (one side) and AO0, AGND (another side) on connector B.
 * 3. Open MeasurementsLive website and connect the ELVISIII.
 * 4. In Instruments tab, Open Function and Arbitrary Waveform Generator, set a square wave in 500Hz, 4Vpp and 0V DC offset.
 * 5. In Instruments tab, Open Oscilloscope, set Normal mode, analog edge type and 1V level.
 * 6. Run this program.
 *
 * Output:
 * The program reads groups of values from AI0 on connector A.
 * The program writes groups of initial values to AO0 on connector B.
 * The output is maintained for 60 s.
 * The group of values reading from the AI0 is written to the console.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the ELVIS III.
 */
int main(int argc, char **argv)
{
	NiFpga_Status status;

	double value[FIFO_SIZE];
	uint64_t FixPoint_buffer_receive[FIFO_SIZE];

	double send[] = {2, 2, 2, 2, -2, -2, -2, -2};
	uint64_t FixPoint_buffer_send[FIFO_SIZE];

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
	 * Set the number of valid channels on connector A.
	 */
	Ai_Counter(&connector_A, 1);

	/*
	 * Configure the range of the AI channel on connector A.
	 */
	Ai_Configure(&connector_A, Ai_Channel0, Ai_Range0);

	/*
	 * Configure the divisor for the AI sample rate on connector A.
	 */
	Ai_Divisor(&connector_A, 40000000, 1000);

	/*
	 * Set the DMA Enable Flag for connector A.
	 */
	Ai_Enable(&connector_A);

	/*
	 * Read Fix Points from an AI FIFO on connector A.
	 */
	Ai_ReadFifo(&connector_A,
			    TargetToHost_FIFO_FixPoint_A,
			    FixPoint_buffer_receive,
				FIFO_SIZE,
				NiFpga_InfiniteTimeout,
				NULL);

	/*
	 * Convert Fix Point values of the FIFO to Double values.
	 * The fix point value is an unsigned long long int value.
	 */
	ConvertUnsignedLongLongIntToDouble(FixPoint_buffer_receive, FIFO_SIZE, value);

	/*
	 * Print out the values of A/AI0.
	 */
	printf("Channel%d:\n", Ai_Channel0 - RSE_NUM);
	int i;
	for (i = 0; i < FIFO_SIZE; ++i)
	{
		printf("%f ", value[i]);
		if ((i + 1) % 10 == 0)
			printf("\n");
	}
	printf("\n");

	/*
	 * Configure the divisor for the AO sample rate on connector B.
	 */
	Ao_Divisor(&connector_B, 40000000, 1000);

	/**
	 * Set the DMA Enable Flag for AO0 on connector B.
	 */
	Ao_Enable(&connector_B, Ao_Channel0);

	/*
	 * Convert double values to fix point values of the FIFO.
	 * The fix point value is an unsigned long long int value.
	 */
	ConvertDoubleToUnsignedLongLongInt(send, FixPoint_buffer_send, sizeof(send)/sizeof(uint64_t));

	/*
	 * Write Fix Points to an AO FIFO on connector B.
	 */
	Ao_WriteFifo(&connector_B,
				 HostToTarget_FIFO_FixPoint_B,
				 FixPoint_buffer_send,
				 (sizeof(FixPoint_buffer_send)/sizeof(uint64_t)),
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
