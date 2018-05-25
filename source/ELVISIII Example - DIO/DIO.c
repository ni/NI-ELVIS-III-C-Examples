/**
 * Example for performing Basic Digital Input and Output
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
#include "DIO.h"

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
ELVISIII_Dio connector_A = {98304, 98312, 98320};

/*
 * Initialize the register addresses of DI in connector B.
 */
ELVISIII_Dio connector_B = {99532, 99524, 99516};

/**
 * Read the value from one channel.
 *
 * @param[in]  connector	Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  channel  	Enum containing 20 kinds of channels (DIO0 - DIO19).
 *
 * @return the logical value of the voltage on the channel.
 *
 * Note:
 * 1. The first parameter can be either &connector_A or &connector_B (Both are initialized in AIO.h) unless you know certain addresses of AI Registers.
 * 2. The second parameter only have 20 choices which can be seen in AIO.h (enum DiChannel).
 */
NiFpga_Bool Dio_ReadBit(ELVISIII_Dio* connector, Di_Channel channel)
{
	NiFpga_Status status;

	uint8_t  bit      = channel;
	uint32_t dirValue = 0;
	uint32_t dirMask  = 0;
	uint32_t inValue  = 0;

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

	/*
	 * Get the value of the DI Value Register.
	 *
	 * NiFpga_MergeStatus is used to propagate any errors from previous function calls.
	 * Errors are not anticipated so error checking is not done after every NiFpga function call but only at specific points.
	 */
	NiFpga_MergeStatus(&status, NiFpga_ReadU32(NiELVISIIIv10_session, connector->in, &inValue));

	/*
	 * Check if there was an error writing to or reading from the DI Registers.
	 *
	 * If there was an error then the rest of the function cannot complete correctly so print an error message to stdout and return from the function early.
	 */
	NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the DI Value Register!");

	/*
	 * Isolate the value of the relevant bit.
	 */
	inValue = inValue & (1 << bit);

	/*
	 * If inValue > 0 then return NiFpga_True otherwise return NiFpga_False.
	 */
	return ((inValue > 0) ? NiFpga_True : NiFpga_False);
}


/**
 * Write the value into one channel.
 *
 * @param[in]  connector	Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  value  		the value used to write into the DO Value Register.
 * @param[in]  channel  	Enum containing 20 kinds of channels (DIO0 - DIO19).
 *
 * Note:
 * 1. The first parameter can be either &connector_A or &connector_B (Both are initialized in AIO.h) unless you know certain addresses of AI Registers.
 * 2. The second parameter is either true or false.
 * 3. The third parameter only have 20 choices which can be seen in AIO.h (enum DiChannel).
 */
void Dio_WriteBit(ELVISIII_Dio* connector, NiFpga_Bool value, Di_Channel channel)
{
	NiFpga_Status status;

	uint32_t dirValue;
	uint32_t outValue;
	uint8_t bit = channel;

	/*
	 * Get the value from the DO Value Register.
	 */
	status = NiFpga_ReadU32(NiELVISIIIv10_session, connector->out, &outValue);

	/*
	 * Get the value from the DI Direction register.
	 *
	 * NiFpga_MergeStatus is used to propagate any errors from previous function calls.
	 * Errors are not anticipated so error checking is not done after every NiFpga function call but only at specific points.
	 */
	NiFpga_MergeStatus(&status, NiFpga_ReadU32(NiELVISIIIv10_session, connector->dir, &dirValue));

	/*
	 * Check if there was an error reading from the DIO registers.
	 *
	 * If there was an error then the rest of the function cannot complete correctly so print an error message to stdout and return from the function early.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not read from the DI Direction Register!");

	/*
	 * Set the appropriate bit in the direction register to turn the channel into an output.
	 * DIO0 = bit0, DIO1 = bit1, etc.
	 */
	dirValue = dirValue | (1 << bit);

	/*
	 * Clear the value of the appropriate bit in the output register.
	 * This is done so that the correct value can be set later on.
	 */
	outValue = outValue & ~(1 << bit);

	/*
	 * Set the value of the appropriate bit in the output register.
	 * If the value to set is 0 this operation would not work unless the bit was previously cleared.
	 */
	outValue = outValue | (value << bit);

	/*
	 * Write the new value of the output register to the device.
	 */
	NiFpga_MergeStatus(&status, NiFpga_WriteU32(NiELVISIIIv10_session, connector->out, outValue));

	/*
	 * Write the new value of the direction register to the device to ensure that the DIO channel is set as an output channel.
	 */
	NiFpga_MergeStatus(&status, NiFpga_WriteU32(NiELVISIIIv10_session, connector->dir, dirValue));

	/*
	 * Check if there was an error writing to DIO channel registers.
	 *
	 * If there was an error then print an error message to stdout.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the DO Value/Direction Register!");

	return;
}