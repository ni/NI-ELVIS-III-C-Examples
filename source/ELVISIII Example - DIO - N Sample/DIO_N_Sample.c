/**
 * Example for performing Basic Digital Input and Output - N Sample
 *
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdbool.h>

/**
 * Include the ELVIS III header file.
 * The target type must be defined in your project, as a stand-alone #define,
 * or when calling the compiler from the command-line.
 */
#include "NiELVISIIIv10.h"
#include "DIO_N_Sample.h"

/*
 * Declare the ELVIS III NiFpga_Session so that it can be used by any function in
 * this file. The variable is actually defined in NiELVISIIIv10.c.
 *
 * This removes the need to pass the NiELVISIIIv10_session around to every function and
 * only has to be declared when it is being used.
 */
extern NiFpga_Session NiELVISIIIv10_session;

/*
 * Initialize the register addresses of DI in connector A.
 */
ELVISIII_Dio connector_A = {DIADMA_ENA, 98328, DIADMA_CNTR, DOADMA_CNTR, 98304};

/*
 * Initialize the register addresses of DI in connector B.
 */
ELVISIII_Dio connector_B = {DIBDMA_ENA, 99508, DIBDMA_CNTR, DOBDMA_CNTR, 99532};


/**
 * Set the DIO Direction Register.
 *
 * @param[in]  connector	Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  channel  	Enum containing 20 kinds of channels (DIO0 - DIO19).
 */
void Di_Direction(ELVISIII_Dio* connector, Di_Channel channel)
{
	NiFpga_Status status;

	uint8_t  bit      = channel;
	uint32_t dirValue = 0;
	uint32_t dirMask  = 0;

	/*
	 * Get the value of the DI Direction Register.
	 */
	status = NiFpga_ReadU32(NiELVISIIIv10_session, connector->dir, &dirValue);

	/*
	 * Check if there was an error writing to the read register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the DI Direction Register!");

	/*
	 * Clear the appropriate bit in the direction register to turn the channel into an input (0).
	 * DIO0 = bit0, DIO1 = bit1, etc.
	 */
	dirMask = 1 << bit;
	dirMask = ~dirMask;
	dirValue = dirValue & dirMask;

	/*
	 * Write the new value to the DI Direction Register to ensure that the proper bit is turned into an input.
	 */
	status = NiFpga_WriteU32(NiELVISIIIv10_session, connector->dir, dirValue);

	/*
	 * Check if there was an error writing to the write register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the DI Direction Register!");

	return;
}


/**
 * Set the DI Divisor Register.
 *
 * @param[in]  connector		Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  ClockRate		Onboard clock rate of FPGA.
 * @param[in]  SampleRate		The Sample Rate.
 */
void Di_Divisor(ELVISIII_Dio* connector, uint32_t ClockRate, uint32_t SampleRate)
{
	NiFpga_Status status;
	bool flag = true;
	uint16_t Divisor = 0;

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

	uint16_t divisor = (uint16_t)(ClockRate / SampleRate);

	/*
	 * Write the new value to the DI DMA Enable Register.
	 */
	status = NiFpga_WriteU16(NiELVISIIIv10_session, connector->di_cntr, divisor);

	/*
	 * Check if there was an error writing to the write register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the DI Counter Register!");

	/*
	 * Make sure that the Value has been written into the proper Register.
	 */
	while (flag)
	{
		/*
		 * Read the value from the DI DMA Enable Register.
		 */
		status = NiFpga_ReadU16(NiELVISIIIv10_session, connector->di_cntr, &Divisor);

		/*
		 * Check if there was an error reading from the register.
		 *
		 * If there was an error then print an error message to stdout and return.
		 */
		NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the DI Counter Register!");

		if (Divisor == divisor)
		{
			flag = false;
		}
	}

	return;
}


/**
 * Configure the DI DMA Enable Register.
 *
 * @param[in]  connector	Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 */
void Di_Enable(ELVISIII_Dio* connector)
{
	NiFpga_Status status;

	/*
	 * Write the new value to the DI DMA Enable Register.
	 */
	status = NiFpga_WriteBool(NiELVISIIIv10_session, connector->di_enable, NiFpga_True);

	/*
	 * Check if there was an error writing to the write register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the DI DMA Enable Register!");

	return;
}


/**
 * Read groups of values from a DI FIFO.
 *
 * @param[in]  connector			Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  fifo					DI target-to-host fifo, which can be seen in DIO_N_Sample.h.
 * @param[in]  fxp_buffer_recv		Array used to read from DI FIFO.
 * @param[in]  fifo_size			The size of DI FIFO.
 * @param[in]  timeout				The default value is NiFpga_InfiniteTimeout.
 * @param[in]  elementsRemaining	The default value is NULL.
 */
void Di_ReadFifo(ELVISIII_Dio*         connector,
				 TargetToHost_FIFO_FXP fifo,
			 	 uint64_t*             fxp_buffer_recv,
			 	 size_t                fifo_size,
			 	 uint32_t              timeout,
			 	 size_t*               elementsRemaining)
{
	NiFpga_Status status;

	/*
	 * Read Groups of FXP from a DI FIFO.
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
	NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the DI FIFO!");

	return;
}


/**
 * Get groups of values from one channel.
 *
 * @param[in]  channel  		Enum containing 20 kinds of channels (DIO0 - DIO19).
 * @param[in]  fxp_buffer_recv	Array used to read from AI FIFO.
 * @param[in]  fifo_size		The size of AI FIFO.
 * @param[in]  val				The values get from one channel.
 */
void Di_GetVal(Di_Channel channel, uint64_t* fxp_buffer_recv, size_t fifo_size, NiFpga_Bool val[])
{
	uint8_t bit = channel;
	int i;

	/*
	 * Get the proper bit.
	 */
	for(i = 0; i < fifo_size; ++i)
	{
		val[i] = (NiFpga_Bool)((fxp_buffer_recv[i] & (1 << bit)) >> bit);
	}

	return;
}


/**
 * Set the DIO Direction Register.
 *
 * @param[in]  connector	Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  channel  	Enum containing 20 kinds of channels (DIO0 - DIO19).
 */
void Do_Direction(ELVISIII_Dio* connector, Di_Channel channel)
{
	NiFpga_Status status;

	uint32_t dirValue;
	uint8_t bit = channel;

	/*
	 * Get the value from the DIO Direction Register.
	 */
	status = NiFpga_ReadU32(NiELVISIIIv10_session, connector->dir, &dirValue);

	/*
	 * Check if there was an error reading from the DIO Direction Register.
	 *
	 * If there was an error then the rest of the function cannot complete correctly so print an error message to stdout and return from the function early.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not read from the DIO Direction Register!");

	/*
	 * Set the appropriate bit in the direction register to turn the channel into an output.
	 * DIO0 = bit0, DIO1 = bit1, etc.
	 */
	dirValue = dirValue | (1 << bit);

	/*
	 * Write the value to the DIO Direction Register.
	 */
	status = NiFpga_WriteU32(NiELVISIIIv10_session, connector->dir, dirValue);

	/*
	 * Check if there was an error writing to the DIO Direction Register.
	 *
	 * If there was an error then the rest of the function cannot complete correctly so print an error message to stdout and return from the function early.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not read from the DIO Direction Register!");

	return;
}


/**
 * Set the DO Divisor Register.
 *
 * @param[in]  connector		Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  ClockRate		Onboard clock rate of FPGA.
 * @param[in]  SampleRate		The Sample Rate.
 */
void Do_Divisor(ELVISIII_Dio* connector, uint32_t ClockRate, uint32_t SampleRate)
{
	NiFpga_Status status;
	bool flag = true;
	uint16_t Divisor = 0;

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

	uint16_t divisor = (uint16_t)(ClockRate / SampleRate);

	/*
	 * Write the new value to the DI DMA Enable Register.
	 */
	status = NiFpga_WriteU16(NiELVISIIIv10_session, connector->do_cntr, divisor);

	/*
	 * Check if there was an error writing to the write register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the DO Counter Register!");

	/*
	 * Make sure that the Value has been written into the proper Register.
	 */
	while (flag)
	{
		/*
		 * Read the value from the DI DMA Enable Register.
		 */
		status = NiFpga_ReadU16(NiELVISIIIv10_session, connector->do_cntr, &Divisor);

		/*
		 * Check if there was an error reading from the register.
		 *
		 * If there was an error then print an error message to stdout and return.
		 */
		NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the DO Counter Register!");

		if (Divisor == divisor)
		{
			flag = false;
		}
	}

	return;
}


/**
 * Configure the DO DMA Enable Register.
 *
 * @param[in]  connector	Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  channel  	Enum containing 20 kinds of channels (DIO0 - DIO19).
 */
void Do_Enable(ELVISIII_Dio* connector, Di_Channel channel)
{
	NiFpga_Status status;

	uint8_t bit = channel;
	uint32_t config;

	/*
	 * Read the value from the DO DMA Enable Register.
	 */
	status = NiFpga_ReadU32(NiELVISIIIv10_session, connector->do_enable, &config);

	/*
	 * Check if there was an error reading from the DO register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not read from the DO DMA Enable Register!");

	/*
	 * Set the appropriate bit in the DO DMA Enable Register to turn the channel into an output.
	 * DIO0 = bit0, DIO1 = bit1, etc.
	 */
	config = config | (1 << bit);

	/*
	 * Write the new value to the DI DMA Enable Register.
	 */
	status = NiFpga_WriteU32(NiELVISIIIv10_session, connector->do_enable, config);

	/*
	 * Check if there was an error writing to the write register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the DO DMA Enable Register!");

	return;
}


/**
 * Write FXP to a DO FIFO.
 *
 * @param[in]  connector			Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  fifo					DO host-to-target fifo, which can be seen in DIO_N_Sample.h.
 * @param[in]  fxp_buffer_send		Array used to write to DO FIFO.
 * @param[in]  fifo_size			The size of DO FIFO.
 * @param[in]  timeout				The default value is NiFpga_InfiniteTimeout.
 * @param[in]  elementsRemaining	The default value is NULL.
 */
void Do_WriteFifo(ELVISIII_Dio* 		connector,
		          HostToTarget_FIFO_FXP fifo,
			 	  const uint64_t*       fxp_buffer_send,
			 	  size_t                fifo_size,
			 	  uint32_t              timeout,
			 	  size_t*               elementsRemaining)
{
	NiFpga_Status status;

	/*
	 * Write Groups of FXP to a DO FIFO.
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
	status = NiFpga_WriteFifoU64(NiELVISIIIv10_session,
								 fifo,
								 fxp_buffer_send,
								 fifo_size,
								 timeout,
								 elementsRemaining);

	/*
	 * Check if there was an error writing to register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not write to the DO FIFO!");

	return;
}

