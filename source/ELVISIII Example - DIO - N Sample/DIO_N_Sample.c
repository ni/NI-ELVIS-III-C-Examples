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
 * Initialize the register addresses of DIO in connector A.
 */
ELVISIII_Dio connector_A = {DIADMA_ENA, 98328, DIADMA_CNTR, DOADMA_CNTR, 98304};

/*
 * Initialize the register addresses of DIO in connector B.
 */
ELVISIII_Dio connector_B = {DIBDMA_ENA, 99508, DIBDMA_CNTR, DOBDMA_CNTR, 99532};

/**
 * Set the Direction of the DIO channel as an input.
 * The DIO channels are accessed in 8-bit banks where each bit in the bank
 * corresponds to one of the DIO channels. In general:
 *    DIO0 = bit0   (1st channel on the DIO 7:0 bank)
 *    DIO1 = bit1   (2nd channel on the DIO 7:0 bank)
 *       ...
 *    DIO7 = bit7   (last channel on the DIO 7:0 bank)
 *    DIO8 = bit0   (1st channel on the DIO 15:8 bank)
 *      ...
 *    DIO15 - bit7  (last channel on the DIO 15:8 bank)
 *
 * A DIO channel can be either an input or an output depending on the value
 * of the DIR register. A value of 0 makes the channel an input, a value of 1
 * sets the channel as an output.
 *
 * @param[in]  connector	A struct containing the registers for one connecter.
 * @param[in]  channel  	Enum containing 20 kinds of channels (DIO0 - DIO19).
 */
void Di_Direction(ELVISIII_Dio* connector, Dio_Channel channel)
{
	NiFpga_Status status;

	uint8_t  bit      = channel;
	uint32_t dirValue = 0;
	uint32_t dirMask  = 0;

	/*
	 * Get the value of the DI Direction Register.
	 *
	 * The returned NiFpga_Status value is stored for error checking.
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
	 *
	 * The returned NiFpga_Status value is stored for error checking.
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
 * Configure the divisor for the DI sample rate.The default onboard clock rate of FPGA is 40 MHz.
 *
 * @param[in]  connector		A struct containing the registers for one connecter.
 * @param[in]  ClockRate		The onboard clock rate of FPGA.
 * @param[in]  SampleRate		The Sample Rate.
 */
void Di_Divisor(ELVISIII_Dio* connector, uint32_t ClockRate, uint32_t SampleRate)
{
	NiFpga_Status status;
	bool flag = true;
	uint16_t Divisor = 0;

	/*
	 * Control the range of the sample rate from MIN_SAMPLE_RATE to MAX_SAMPLE_RATE.
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
	 * Write the divisor value to the DI Divisor Register.
	 *
	 * The returned NiFpga_Status value is stored for error checking.
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
		 * Read the value from the DI Divisor Register.
		 *
		 * The returned NiFpga_Status value is stored for error checking.
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
 * Set the DMA Enable Flag for one connector.The flag controls whether the DMA is enabled for a specific connector.
 *
 * @param[in]  connector	A struct containing the registers for one connecter.
 */
void Di_Enable(ELVISIII_Dio* connector)
{
	NiFpga_Status status;

	/*
	 * Write the new value to the DI DMA Enable Register.
	 *
	 * The returned NiFpga_Status value is stored for error checking.
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
 * @param[in]  connector					A struct containing the registers for one connecter.
 * @param[in]  fifo							DI target-to-host FIFO from which to read
 * @param[in]  fxp_buffer_receive			groups of values in an DI FIFO, get from one channel.
 * @param[in]  fifo_size					The size of DI FIFO.
 * @param[in]  timeout						timeout in milliseconds, or NiFpga_InfiniteTimeout
 * @param[in]  elementsRemaining			if non-NULL, outputs the number of elements
 *                          				remaining in the host memory part of the DMA FIFO
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
void Di_ReadFifo(ELVISIII_Dio*         connector,
				 TargetToHost_FIFO_FXP fifo,
			 	 uint64_t*             fxp_buffer_receive,
			 	 size_t                fifo_size,
			 	 uint32_t              timeout,
			 	 size_t*               elementsRemaining)
{
	NiFpga_Status status;

	/*
	 * Read Groups of fixed-point values from a DI FIFO.
	 *
	 * The returned NiFpga_Status value is stored for error checking.
	 */
	status = NiFpga_ReadFifoU64(NiELVISIIIv10_session,
								fifo,
								fxp_buffer_receive,
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
 * Convert unsigned long long int values of the fixed-points in the FIFO to boolean values.
 *
 * @param[in]  channel  				Enum containing 20 kinds of channels (DIO0 - DIO19).
 * @param[in]  fxp_buffer_receive		groups of fixed-point values get from one channel.
 * 										The fixed-point value is an unsigned long long int value.
 * @param[in]  fifo_size				The size of DI FIFO.
 * @param[in]  value					groups of boolean value
 */
void ConvertUnsignedLongLongIntToBool(Dio_Channel channel, uint64_t* fxp_buffer_receive, size_t fifo_size, NiFpga_Bool value[])
{
	uint8_t bit = channel;
	int i;

	/*
	 * Get the proper bit.
	 */
	for(i = 0; i < fifo_size; ++i)
	{
		value[i] = (NiFpga_Bool)((fxp_buffer_receive[i] & (1 << bit)) >> bit);
	}

	return;
}

/**
 * Set the Direction of the DIO channel as an output.
 * The DIO channels are accessed in 8-bit banks where each bit in the bank
 * corresponds to one of the DIO channels. In general:
 *    DIO0 = bit0   (1st channel on the DIO 7:0 bank)
 *    DIO1 = bit1   (2nd channel on the DIO 7:0 bank)
 *       ...
 *    DIO7 = bit7   (last channel on the DIO 7:0 bank)
 *    DIO8 = bit0   (1st channel on the DIO 15:8 bank)
 *      ...
 *    DIO15 - bit7  (last channel on the DIO 15:8 bank)
 *
 * A DIO channel can be either an input or an output depending on the value
 * of the DIR register. A value of 0 makes the channel an input, a value of 1
 * sets the channel as an output.
 *
 * @param[in]  connector	A struct containing the registers for one connecter.
 * @param[in]  channel  	Enum containing 20 kinds of channels (DIO0 - DIO19).
 */
void Do_Direction(ELVISIII_Dio* connector, Dio_Channel channel)
{
	NiFpga_Status status;

	uint32_t dirValue;
	uint8_t bit = channel;

	/*
	 * Get the value from the DIO Direction Register.
	 *
	 * The returned NiFpga_Status value is stored for error checking.
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
	 *
	 * The returned NiFpga_Status value is stored for error checking.
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
 * Configure the divisor for the DO sample rate.The default onboard clock rate of FPGA is 40 MHz.
 *
 * @param[in]  connector		A struct containing the registers for one connecter.
 * @param[in]  ClockRate		The onboard clock rate of FPGA.
 * @param[in]  SampleRate		The Sample Rate.
 */
void Do_Divisor(ELVISIII_Dio* connector, uint32_t ClockRate, uint32_t SampleRate)
{
	NiFpga_Status status;
	bool flag = true;
	uint16_t Divisor = 0;

	/*
	 * Control the range of the sample rate from MIN_SAMPLE_RATE to MAX_SAMPLE_RATE.
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
	 * Write the divisor value to the DO Divisor Register.
	 *
	 * The returned NiFpga_Status value is stored for error checking.
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
		 * Read the value from the DO Divisor Register.
		 *
		 * The returned NiFpga_Status value is stored for error checking.
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
 * Set the DMA Enable value for a DO channel.The value controls
 * whether the DMA is enabled for a specific digital output channel.
 *
 * @param[in]  connector	A struct containing the registers for one connecter.
 * @param[in]  channel  	Enum containing 20 kinds of channels (DIO0 - DIO19).
 */
void Do_Enable(ELVISIII_Dio* connector, Dio_Channel channel)
{
	NiFpga_Status status;

	uint8_t bit = channel;
	uint32_t config;

	/*
	 * Read the value from the DO DMA Enable Register.
	 *
	 * The returned NiFpga_Status value is stored for error checking.
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
	 * Write the new value to the DO DMA Enable Register.
	 *
	 * The returned NiFpga_Status value is stored for error checking.
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
 * Write groups of values to a DO FIFO.
 *
 * @param[in]  connector				A struct containing the registers for one connecter.
 * @param[in]  fifo						DO host-to-target FIFO from which to write
 * @param[in]  fxp_buffer_send			groups of values to be written.
 * @param[in]  fifo_size				The size of DO FIFO.
 * @param[in]  timeout					timeout in milliseconds, or NiFpga_InfiniteTimeout
 * @param[in]  elementsRemaining		if non-NULL, outputs the number of elements
 *                          			remaining in the host memory part of the DMA FIFO
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
void Do_WriteFifo(ELVISIII_Dio* 		connector,
		          HostToTarget_FIFO_FXP fifo,
			 	  const uint64_t*       fxp_buffer_send,
			 	  size_t                fifo_size,
			 	  uint32_t              timeout,
			 	  size_t*               elementsRemaining)
{
	NiFpga_Status status;

	/*
	 * Write Groups of fix point values to a DO FIFO.
	 *
	 * The returned NiFpga_Status value is stored for error checking.
	 *
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

