/**
 * Example for performing Encoder
 *
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */

#include <stdio.h>

/**
 * Include the ELVIS III header file.
 * The target type must be defined in your project, as a stand-alone #define,
 * or when calling the compiler from the command-line.
 */
#include "NiELVISIIIv10.h"
#include "Encoder.h"

/*
 * Declare the ELVIS III NiFpga_Session so that it can be used by any function in
 * this file. The variable is actually defined in NiELVISIIIv10.c.
 *
 * This removes the need to pass the NiELVISIIIv10_session around to every function and
 * only has to be declared when it is being used.
 */
extern NiFpga_Session NiELVISIIIv10_session;

/*
 * Initialize the register addresses of Encoder in connector A.
 */
ELVISIII_Encoder connector_A = {{ENCA_0CNFG, ENCA_1CNFG, ENCA_2CNFG, ENCA_3CNFG, ENCA_4CNFG, ENCA_5CNFG, ENCA_6CNFG, ENCA_7CNFG, ENCA_8CNFG, ENCA_9CNFG},
                                {ENCA_0STAT, ENCA_1STAT, ENCA_2STAT, ENCA_3STAT, ENCA_4STAT, ENCA_5STAT, ENCA_6STAT, ENCA_7STAT, ENCA_8STAT, ENCA_9STAT},
                                {ENCA_0CNTR, ENCA_1CNTR, ENCA_2CNTR, ENCA_3CNTR, ENCA_4CNTR, ENCA_5CNTR, ENCA_6CNTR, ENCA_7CNTR, ENCA_8CNTR, ENCA_9CNTR},
                                 SYSSELECTA};

/*
 * Initialize the register addresses of Encoder in connector B.
 */
ELVISIII_Encoder connector_B = {{ENCB_0CNFG, ENCB_1CNFG, ENCB_2CNFG, ENCB_3CNFG, ENCB_4CNFG, ENCB_5CNFG, ENCB_6CNFG, ENCB_7CNFG, ENCB_8CNFG, ENCB_9CNFG},
                                {ENCB_0STAT, ENCB_1STAT, ENCB_2STAT, ENCB_3STAT, ENCB_4STAT, ENCB_5STAT, ENCB_6STAT, ENCB_7STAT, ENCB_8STAT, ENCB_9STAT},
                                {ENCB_0CNTR, ENCB_1CNTR, ENCB_2CNTR, ENCB_3CNTR, ENCB_4CNTR, ENCB_5CNTR, ENCB_6CNTR, ENCB_7CNTR, ENCB_8CNTR, ENCB_9CNTR},
                                 SYSSELECTB};


/**
 * Sets options for the encoder configuration register.
 *
 * @param[in]  connector  	A struct containing the registers for one connecter.
 * @param[in]  channel		Enum containing 10 kinds of Encoder channels.
 * @param[in]  mask     	Array of flags that indicate which of the configure settings are valid.
 * @param[in]  settings 	Array of flags that indicate the configuration settings.
 */
void Encoder_Configure(ELVISIII_Encoder*         connector,
		               Encoder_Channel           channel,
		               Encoder_ConfigureMask     mask,
                       Encoder_ConfigureSettings settings)
{
	NiFpga_Status status;
	uint8_t cnfgValue;

	/*
	 * Get the current value of the Configuration Register.
	 *
	 * The returned NiFpga_Status value is stored for error checking.
	 */
	status = NiFpga_ReadU8(NiELVISIIIv10_session, connector->cnfg[channel], &cnfgValue);

	/*
	 * Check if there was an error reading from the encoder registers.
	 *
	 * If there was an error then the rest of the function cannot complete
	 * correctly so print an error message to stdout and return from the
	 * function early.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not read from the Encoder Configuration Register!");

	/*
	 * Clear the value of the masked bits in the Configuration Register. This is
	 * done so that the correct value can be set later on.
	 */
	cnfgValue = cnfgValue & (~mask);

	/*
	 * Set the value of the settings in the Configuration Register. If the
	 * value to set is 0 this operation would not work unless the bit was
	 * previously cleared.
	 */
	cnfgValue = cnfgValue | settings;

	/*
	 * Write the new value of the Configuration Register to the device.
	 */
	NiFpga_MergeStatus(&status, NiFpga_WriteU8(NiELVISIIIv10_session, connector->cnfg[channel], cnfgValue));

	/*
	 * Check if there was an error writing to Encoder Configuration Register.
	 *
	 * If there was an error then print an error message to stdout.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the Encoder Configuration Register!");

	return;
}


/**
 * Reads the encoder status, returning the status as bits.
 *
 * @param[in]  connector  	A struct containing the registers for one connecter.
 * @param[in]  channel		Enum containing 10 kinds of Encoder channels.
 *
 * @return the status as a bit field.
 */
uint8_t Encoder_Status(ELVISIII_Encoder* connector, Encoder_Channel channel)
{
	NiFpga_Status status;
	uint8_t statusValue;

	/*
	 * Get the value of the status register.
	 *
	 * The returned NiFpga_Status value is stored for error checking.
	 */
	status = NiFpga_ReadU8(NiELVISIIIv10_session, connector->stat[channel], &statusValue);


	/*
	 * Check if there was an error reading from the encoder register.
	 *
	 * If there was an error then the status is undefined.rest Print an
	 * error message to stdout and return a default status.
	 */
	NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0, "Could not read from the Encoder Status Register!");

	/*
	 *  Return the value of the status.
	 */
	return statusValue;
}


/**
 * Reads the number of steps that the encoder has gone through.
 * The behavior depends on the SignalMode.
 *
 * ENC_QUAD_PHASE:
 * The counter increments when phase A leads phase B and decrements when phase B leads phase A.
 *
 * ENC_SET_AND_DIRECTION:
 * The counter increments when the direction input is low and decrements when the direction input is high.
 *
 * @param[in]  connector  	A struct containing the registers for one connecter.
 * @param[in]  channel		Enum containing 10 kinds of Encoder channels.
 *
 * @return the status as a bit field.
 */
uint32_t Encoder_Counter(ELVISIII_Encoder* connector, Encoder_Channel channel)
{
	NiFpga_Status status;
	uint32_t counterValue;

	/*
	 * Get the value of the counter register.
	 *
	 * The returned NiFpga_Status value is stored for error checking.
	 */
	status = NiFpga_ReadU32(NiELVISIIIv10_session, connector->cntr[channel], &counterValue);

	/*
	 * Check if there was an error reading from the encoder register.
	 *
	 * If there was an error then the value of the counter is undefined
	 * so print an error message to stdout and return 0.
	 */
	NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0, "Could not read from the Encoder Counter Register!");

	/*
	 * Return the value of the counter.
	 */
	return counterValue;
}


/**
 * Write the value to the System Select Register.
 *
 * @param[in]  connector		A struct containing the registers for one connecter.
 * @param[in]  channel			Enum containing 10 kinds of Encoder channels.
 */
void Encoder_Select(ELVISIII_Encoder* connector, Encoder_Channel channel)
{
	NiFpga_Status status;
	uint64_t selectReg;

	/*
	 * Encoder outputs are on pins shared with other onboard devices.
	 * To output on a physical pin, select the Encoder on the appropriate SELECT Register.
	 * See the MUX example for simplified code to enable-disable onboard devices.
	 *
	 * Read the value of the SYSSELECTA/SYSSELECTB Register.
	 */
	status = NiFpga_ReadU64(NiELVISIIIv10_session, connector->sel, &selectReg);

	/*
	 * Check if there was an error reading from the System Select Register.
	 *
	 * If there was an error then print an error message to stdout.
	 */
	NiELVISIIIv10_ReturnValueIfNotSuccess(status, status, "Could not read from the System Select Register!");

	/*
	 * Clear bits of the SYSSELECTA/SYSSELECTB register. This is
     * done so that the correct value can be set later on.
	 */
	selectReg = selectReg & (~(0xf << (channel * 4)));

	/*
	 * Set bit2 of the SYSSELECTA/SYSSELECTB register to enable Encoder functionality.
	 * The functionality of the bit is specified in the documentation.
	 */
	selectReg = selectReg | (0xa << (channel * 4));

	/*
	 * Write the new value to the SYSSELECTA/SYSSELECTB Register.
	 */
	status = NiFpga_WriteU64(NiELVISIIIv10_session, connector->sel, selectReg);

	/*
	 * Check if there was an error reading from the System Select Register.
	 *
	 * If there was an error then print an error message to stdout.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not Write to the System Select Register!");

	return;
}
