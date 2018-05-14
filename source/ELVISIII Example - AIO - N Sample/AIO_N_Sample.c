/**
 * Example for performing Basic Analog Input and Output - N Sample
 *
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

/**
 * Include the ELVIS III header file.
 * The target type must be defined in your project, as a stand-alone #define,
 * or when calling the compiler from the command-line.
 */
#include "NiELVISIIIv10.h"
#include "AIO_N_Sample.h"

/*
 * Declare the ELVIS III NiFpga_Session so that it can be used by any function in
 * this file. The variable is actually defined in NiELVISIIIv10.c.
 *
 * This removes the need to pass the NiELVISIIIv10_session around to every function and
 * only has to be declared when it is being used.
 */
extern NiFpga_Session NiELVISIIIv10_session;

/*
 * Initialize the register addresses of AI in connector A
 */
ELVISIII_Aio connector_A = {AIACNFG, AIACNTR, AOADMA_CNTR, AIACNT, AIADMA_ENA, 99558};

/*
 * Initialize the register addresses of AI in connector B
 */
ELVISIII_Aio connector_B = {AIBCNFG, AIBCNTR, AOADMA_CNTR, AIBCNT, AIBDMA_ENA, 99566};


/**
 * Set the AI Counter Register.
 *
 * @param[in]  connector	Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  counter		The number of valid channels.
 */
void Ai_Counter(ELVISIII_Aio* connector, uint8_t counter)
{
	NiFpga_Status status;

	/*
	 * Write the counter value to the AI Counter Register.
	 */
	status = NiFpga_WriteU8(NiELVISIIIv10_session, connector->cnt, counter);

	/*
	 * Check if there was an error writing to the write register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the AI Counter Register!");

	return;
}


/**
 * Set the AI Configuration Register.
 *
 * @param[in]  connector	Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  channel		Enum containing 12 kinds of channels (8 RSE + 4 DIFF).
 * @param[in]  range		Enum containing 4 kinds of ranges (±10 V, ±5 V, ±2 V, ±1 V).
 */
void Ai_Configure(ELVISIII_Aio* connector, Ai_Channel channel, Ai_Range range)
{
	NiFpga_Status status;

	bool rdy;
	bool flag = true;
	int i = 0;

	unsigned short int Channel = channel;
	unsigned short int Range = range;

	uint8_t  Config[RSE_NUM + DIFF_NUM] = {0};
	uint8_t *ConfigValue = (uint8_t*)calloc(sizeof(uint8_t), RSE_NUM + DIFF_NUM);

	uint8_t Counter = 0;

	/*
	 * Get the initial values from the AI Configuration Register.
	 */
	status = NiFpga_ReadArrayU8(NiELVISIIIv10_session, connector->cnfg, Config, RSE_NUM + DIFF_NUM);

	/*
	 * Check if there was an error writing to the read register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the AI Configuration Register!");

	/*
	 * Change the proper element in the configuration array.
	 */
	if ((Channel >> 3) == 1)
	{//RSE mode
		Config[Channel - RSE_NUM] = Channel | Range;
	}
	else
	{//DIFF mode
		Config[Channel + RSE_NUM] = Channel | Range;
	}

	/*
	 * Write the configuration values to the AI Configuration Register.
	 */
	status = NiFpga_WriteArrayU8(NiELVISIIIv10_session, connector->cnfg, (const uint8_t*)(Config), RSE_NUM + DIFF_NUM);

	/*
	 * Check if there was an error writing to the write register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the AI Configuration Register!");

	/*
	 * Read the value from the AI Counter Register.
	 */
	status = NiFpga_ReadU8(NiELVISIIIv10_session, connector->cnt, &Counter);

	/*
	 * Check if there was an error reading from the register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not read from the AI Counter Register!");

	/*
	 * Get the value from the AI Configuration Register to check whether the configuration writing process is OK.
	 *
	 * The returned NiFpga_Status value is stored for error checking.
	 */
	while(flag)
	{
		rdy = true;

		/*
		 * Get the values from the AI Configuration Register.
		 */
		status = NiFpga_ReadArrayU8(NiELVISIIIv10_session, connector->cnfg, ConfigValue, RSE_NUM + DIFF_NUM);

		/*
		 * Check if there was an error writing to the read register.
		 *
		 * If there was an error then print an error message to stdout and return.
		 */
		NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the AI Configuration Register!");

		/*
		 * Waiting to finish reading from AI Configuration Register.
		 */
		for(i = 0; i < Counter; ++i)
		{
			if (ConfigValue[i] != Config[i])
			{
				rdy = false;
				break;
			}
		}
		if (rdy == true)
			flag = false;
	}
	free(ConfigValue);

	/*
	 * We need a 1s-delay in order to wait until all the numbers can be read from the AI FIFO correctly.
	 * If you remove the sleep(), the first number reading from the AI FIFO may be incorrect.
	 */
	sleep(1);

	return;
}


/**
 * Set the AI Divisor Register.
 *
 * @param[in]  connector	Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  ClockRate	Onboard clock rate of FPGA.
 * @param[in]  SampleRate	The Sample Rate.
 */
void Ai_Divisor(ELVISIII_Aio* connector, uint32_t ClockRate, uint32_t SampleRate)
{
	NiFpga_Status status;

	/*
	 * The range of the sample rate is from MIN_SAMPLE_RATE to MAX_SAMPLE_RATE.
	 */
	if (SampleRate > MAX_SAMPLE_RATE)
	{
		SampleRate = MAX_SAMPLE_RATE;
	}

	if (SampleRate < MIN_SAMPLE_RATE)
	{
		SampleRate = MIN_SAMPLE_RATE;
	}

	uint32_t divisor = (uint32_t)(ClockRate / SampleRate);

	/*
	 * Write the divisor value to the AI Divisor Register.
	 */
	status = NiFpga_WriteU32(NiELVISIIIv10_session, connector->ai_cntr, divisor);

	/*
	 * Check if there was an error writing to the write register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the AI Divisor Register!");

	return;
}


/**
 * Set the AI DMA Enable Register.
 *
 * @param[in]  connector	Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 */
void Ai_Enable(ELVISIII_Aio* connector)
{
	NiFpga_Status status;

	/*
	 * Write the NiFpga_True to the AI DMA Enable Register.
	 */
	status = NiFpga_WriteBool(NiELVISIIIv10_session, connector->ai_enable, NiFpga_True);

	/*
	 * Check if there was an error writing to the write register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the AI DMA Enable Register!");

	return;
}


/**
 * Read groups of values from a AI FIFO.
 *
 * @param[in]  connector			Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  fifo					AI target-to-host fifo, which can be seen in AIO_N_Sample.h.
 * @param[in]  fxp_buffer_recv		Array used to read from AI FIFO.
 * @param[in]  fifo_size			The size of AI FIFO.
 * @param[in]  timeout				The default value is NiFpga_InfiniteTimeout.
 * @param[in]  elementsRemaining	The default value is NULL.
 */
void Ai_ReadFifo(ELVISIII_Aio*          connector,
		         TargetToHost_FIFO_FXP fifo,
			 	 uint64_t*             fxp_buffer_recv,
			 	 size_t                fifo_size,
			 	 uint32_t              timeout,
			 	 size_t*               elementsRemaining)
{
	NiFpga_Status status;

	/*
	 * Read Groups of FXP from a AI FIFO.
	 *
	 * ------------------------------------------
	 * Item:             | Default value:
	 * ------------------------------------------
	 * fifo_size         | 100
	 * ------------------------------------------
	 * timeout           | NiFpga_InfiniteTimeout
	 * ------------------------------------------
	 * elementsRemaining | NULL.
	 * ------------------------------------------
	 */
	status = NiFpga_ReadFifoU64(NiELVISIIIv10_session,
								fifo,
								fxp_buffer_recv,
								fifo_size,
								timeout,
								elementsRemaining);

	/*
	 * Check if there was an error reading from register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the AI FIFO!");

	return;
}


/**
 * Get groups of values from one channel.
 *
 * @param[in]  fxp_buffer_recv	Array used to read from AI FIFO.
 * @param[in]  fifo_size		The size of AI FIFO.
 * @param[in]  val				The values get from one channel.
 */
void Ai_GetVal(uint64_t *fxp_buffer_recv, size_t fifo_size, double *val)
{
	int i;
	uint32_t temp;
	for (i = 0; i < fifo_size; ++i)
	{
		temp = (uint32_t)fxp_buffer_recv[i];
		if (temp & (1 << (AI_WordLength - 1)))
		{//negative
			temp = ~temp;
			++temp;
			val[i] = (double)(temp / pow(2, (AI_WordLength - AI_IntegerWordLength)) * (-1));
		}
		else
		{//positive
			val[i] = (double)(temp / pow(2, (AI_WordLength - AI_IntegerWordLength)));
		}
	}

	return;
}


/**
 * Set the AI Divisor Register.
 *
 * @param[in]  connector	Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  ClockRate	Onboard clock rate of FPGA.
 * @param[in]  SampleRate	The Sample Rate.
 */
void Ao_Divisor(ELVISIII_Aio* connector, uint32_t ClockRate, uint32_t SampleRate)
{
	NiFpga_Status status;

	/*
	 * The range of the sample rate is from MIN_SAMPLE_RATE to MAX_SAMPLE_RATE.
	 */
	if (SampleRate > MAX_SAMPLE_RATE)
	{
		SampleRate = MAX_SAMPLE_RATE;
	}

	if (SampleRate < MIN_SAMPLE_RATE)
	{
		SampleRate = MIN_SAMPLE_RATE;
	}

	uint32_t divisor = (uint32_t)(ClockRate / SampleRate);

	/*
	 * Write the divisor value to the AI Divisor Register.
	 */
	status = NiFpga_WriteU32(NiELVISIIIv10_session, connector->ao_cntr, divisor);

	/*
	 * Check if there was an error writing to the write register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the AO Divisor Register!");

	return;
}


/**
 * Set the AO DMA Enable Register
 *
 * @param[in]  connector	Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  channel		Enum containing 2 kinds of channels.
 */
void Ao_Enable(ELVISIII_Aio* connector, Ao_Channel channel)
{
	NiFpga_Status status;
	uint8_t bit = channel;

	uint8_t enable = 0;

	/*
	 * Get the initial values from the AO DMA Enable Register.
	 */
	status = NiFpga_ReadU8(NiELVISIIIv10_session, connector->ao_enable, &enable);

	/*
	 * Check if there was an error writing to the read register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the AO DMA Enable Register!");

	/*
	 * Clear the proper bit.
	 */
	enable = enable & ~bit;

	/*
	 * Set the proper bit.
	 */
	enable = enable | bit;

	/*
	 * Write the value to the AO DMA Enable Register.
	 */
	status = NiFpga_WriteU8(NiELVISIIIv10_session, connector->ao_enable, enable);

	/*
	 * Check if there was an error writing to the register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not write to the AO DMA Enable Register!");

	return;
}


/**
 * Turn groups of values from double to Fix Point.
 *
 * @param[in]  val				The values get from one channel.
 * @param[in]  fxp_buffer_send	Array used to write to AO FIFO.
 * @param[in]  fifo_size		The size of AO FIFO.
 */
void Ao_SetVal(double *val, uint64_t *fxp_buffer_send, size_t fifo_size)
{
	int i;
	uint32_t temp = 0;

	for (i = 0; i < fifo_size; ++i)
	{
		if (val[i] < 0)
		{//negative
			temp = (uint32_t)(val[i] * (-1) * pow(2, (AO_WordLength - AO_IntegerWordLength)));
			--temp;
			temp = ~temp;
			fxp_buffer_send[i] = (uint64_t)(0xFFFFFFFF00000000 + temp);
		}
		else
		{//positive
			fxp_buffer_send[i] = (uint64_t)(val[i] * pow(2, (AO_WordLength - AO_IntegerWordLength)));
		}
	}

	return;
}


/**
 * Write FXP to a AO FIFO.
 *
 * @param[in]  connector			Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  fifo					AO host-to-target fifo, which can be seen in AIO_N_Sample.h.
 * @param[in]  fxp_buffer_send		Array used to write to AO FIFO.
 * @param[in]  fifo_size			The size of AO FIFO
 * @param[in]  timeout				The default value is NiFpga_InfiniteTimeout.
 * @param[in]  elementsRemaining	The default value is NULL.
 */
void Ao_WriteFifo(ELVISIII_Aio*         connector,
		          HostToTarget_FIFO_FXP fifo,
			 	  const uint64_t*       fxp_buffer_send,
			 	  size_t                fifo_size,
			 	  uint32_t              timeout,
			 	  size_t*               elementsRemaining)
{
	NiFpga_Status status;

	/*
	 * Read Groups of FXP from a AI FIFO.
	 *
	 * ------------------------------------------
	 * Item:             | Default value:
	 * ------------------------------------------
	 * FIFO_SIZE         | 100
	 * ------------------------------------------
	 * timeout           | NiFpga_InfiniteTimeout
	 * ------------------------------------------
	 * elementsRemaining | NULL.
	 * ------------------------------------------
	 */
	status = NiFpga_WriteFifoU64(NiELVISIIIv10_session,
								fifo,
								fxp_buffer_send,
								fifo_size,
								timeout,
								elementsRemaining);

	/*
	 * Check if there was an error reading from register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the AO FIFO!");

	return;
}
