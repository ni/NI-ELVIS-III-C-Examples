/**
 * Example for performing PWM
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
#include "PWM.h"

/*
 * Declare the ELVIS III NiFpga_Session so that it can be used by any function in
 * this file. The variable is actually defined in NiELVISIIIv10.c.
 *
 * This removes the need to pass the NiELVISIIIv10_session around to every function and
 * only has to be declared when it is being used.
 */
extern NiFpga_Session NiELVISIIIv10_session;

/*
 * Initialize the register addresses of PWM in connector A.
 */
ELVISIII_Pwm connector_A = {{PWMA_0CNFG, PWMA_1CNFG, PWMA_2CNFG, PWMA_3CNFG, PWMA_4CNFG, PWMA_5CNFG, PWMA_6CNFG, PWMA_7CNFG, PWMA_8CNFG, PWMA_9CNFG,
		                     PWMA_10CNFG, PWMA_11CNFG, PWMA_12CNFG, PWMA_13CNFG, PWMA_14CNFG, PWMA_15CNFG, PWMA_16CNFG, PWMA_17CNFG, PWMA_18CNFG, PWMA_19CNFG},
		                    {PWMA_0CS, PWMA_1CS, PWMA_2CS, PWMA_3CS, PWMA_4CS, PWMA_5CS, PWMA_6CS, PWMA_7CS, PWMA_8CS, PWMA_9CS,
				             PWMA_10CS, PWMA_11CS, PWMA_12CS, PWMA_13CS, PWMA_14CS, PWMA_15CS, PWMA_16CS, PWMA_17CS, PWMA_18CS, PWMA_19CS},
				            {PWMA_0MAX, PWMA_1MAX, PWMA_2MAX, PWMA_3MAX, PWMA_4MAX, PWMA_5MAX, PWMA_6MAX, PWMA_7MAX, PWMA_8MAX, PWMA_9MAX,
						     PWMA_10MAX, PWMA_11MAX, PWMA_12MAX, PWMA_13MAX, PWMA_14MAX, PWMA_15MAX, PWMA_16MAX, PWMA_17MAX, PWMA_18MAX, PWMA_19MAX},
						    {PWMA_0CMP, PWMA_1CMP, PWMA_2CMP, PWMA_3CMP, PWMA_4CMP, PWMA_5CMP, PWMA_6CMP, PWMA_7CMP, PWMA_8CMP, PWMA_9CMP,
						     PWMA_10CMP, PWMA_11CMP, PWMA_12CMP, PWMA_13CMP, PWMA_14CMP, PWMA_15CMP, PWMA_16CMP, PWMA_17CMP, PWMA_18CMP, PWMA_19CMP},
						    {PWMA_0CNTR, PWMA_1CNTR, PWMA_2CNTR, PWMA_3CNTR, PWMA_4CNTR, PWMA_5CNTR, PWMA_6CNTR, PWMA_7CNTR, PWMA_8CNTR, PWMA_9CNTR,
						     PWMA_10CNTR, PWMA_11CNTR, PWMA_12CNTR, PWMA_13CNTR, PWMA_14CNTR, PWMA_15CNTR, PWMA_16CNTR, PWMA_17CNTR, PWMA_18CNTR, PWMA_19CNTR},
                             SYSSELECTA};

/*
 * Initialize the register addresses of PWM in connector B.
 */
ELVISIII_Pwm connector_B = {{PWMB_0CNFG, PWMB_1CNFG, PWMB_2CNFG, PWMB_3CNFG, PWMB_4CNFG, PWMB_5CNFG, PWMB_6CNFG, PWMB_7CNFG, PWMB_8CNFG, PWMB_9CNFG,
		                     PWMB_10CNFG, PWMB_11CNFG, PWMB_12CNFG, PWMB_13CNFG, PWMB_14CNFG, PWMB_15CNFG, PWMB_16CNFG, PWMB_17CNFG, PWMB_18CNFG, PWMB_19CNFG},
		                    {PWMB_0CS, PWMB_1CS, PWMB_2CS, PWMB_3CS, PWMB_4CS, PWMB_5CS, PWMB_6CS, PWMB_7CS, PWMB_8CS, PWMB_9CS,
				             PWMB_10CS, PWMB_11CS, PWMB_12CS, PWMB_13CS, PWMB_14CS, PWMB_15CS, PWMB_16CS, PWMB_17CS, PWMB_18CS, PWMB_19CS},
				            {PWMB_0MAX, PWMB_1MAX, PWMB_2MAX, PWMB_3MAX, PWMB_4MAX, PWMB_5MAX, PWMB_6MAX, PWMB_7MAX, PWMB_8MAX, PWMB_9MAX,
						     PWMB_10MAX, PWMB_11MAX, PWMB_12MAX, PWMB_13MAX, PWMB_14MAX, PWMB_15MAX, PWMB_16MAX, PWMB_17MAX, PWMB_18MAX, PWMB_19MAX},
						    {PWMB_0CMP, PWMB_1CMP, PWMB_2CMP, PWMB_3CMP, PWMB_4CMP, PWMB_5CMP, PWMB_6CMP, PWMB_7CMP, PWMB_8CMP, PWMB_9CMP,
						     PWMB_10CMP, PWMB_11CMP, PWMB_12CMP, PWMB_13CMP, PWMB_14CMP, PWMB_15CMP, PWMB_16CMP, PWMB_17CMP, PWMB_18CMP, PWMB_19CMP},
						    {PWMB_0CNTR, PWMB_1CNTR, PWMB_2CNTR, PWMB_3CNTR, PWMB_4CNTR, PWMB_5CNTR, PWMB_6CNTR, PWMB_7CNTR, PWMB_8CNTR, PWMB_9CNTR,
						     PWMB_10CNTR, PWMB_11CNTR, PWMB_12CNTR, PWMB_13CNTR, PWMB_14CNTR, PWMB_15CNTR, PWMB_16CNTR, PWMB_17CNTR, PWMB_18CNTR, PWMB_19CNTR},
                             SYSSELECTB};


/**
 * Sets options for the PWM configuration register.
 *
 * Pwm_Inverted inverts output high and low. Equivalent to 100% - <duty cycle>.
 *
 * Pwm_Disabled disables PWM output
 * Pwm_Enabled enabled PWM output
 *
 * @param[in]  connector  	Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  channel		Enum containing 20 kinds of PWM channels.
 * @param[in]  mask     	Array of flags that indicate which of the configure settings are valid.
 *  						This can be one or more of the following values: Pwm_Invert, Pwm_Mode.
 * @param[in]  settings 	Array of flags that indicate the configuration settings.
 *                      	If mask sets Pwm_Invert, set either Pwm_NotInverted or Pwm_Inverted.
 *                      	If mask sets Pwm_Mode, set one of Pwm_Disabled, Pwm_Enabled.
 */
void Pwm_Configure(ELVISIII_Pwm* connector, Pwm_Channel channel, Pwm_ConfigureMask mask, Pwm_ConfigureSettings settings)
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
	 * Check if there was an error reading from the PWM registers.
	 *
	 * If there was an error then the rest of the function cannot complete
	 * correctly so print an error message to stdout and return from the
	 * function early.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not read from the PWM Configuration Register!");

	/*
	 * Clear the value of the masked bits in the Configuration Register. This is
	 * done so that the correct value can be set later on.
	 */
	cnfgValue = cnfgValue & (~mask);

	/*
	 * Set the value of the settings bits in the Configuration Register. If the
	 * value to set is 0 this operation would not work unless the bit was
	 * previously cleared.
	 */
	cnfgValue = cnfgValue | settings;

	/*
	 * Write the new value of the configure register to the device.
	 */
	NiFpga_MergeStatus(&status, NiFpga_WriteU8(NiELVISIIIv10_session, connector->cnfg[channel], cnfgValue));

	/*
	 * Check if there was an error writing to PWM Configuration Register.
	 *
	 * If there was an error then print an error message to stdout.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the PWM Configuration Register!");

	return;
}


/**
 * Sets the PWM clock divider. The PWM clock increments/decrements at this
 * frequency. This, along with the value set by Pwm_CounterMaximum sets the
 * frequency of the PWM waveform.
 *
 * The frequency is equal to the ELVIS III clock frequency (f_clk) divided by this clock divider.
 * The default ELVIS III FPGA clock frequency is 40 MHz.
 *
 * | divider  | clock          	| default clock |
 * ----------------------------------------------
 * | Pwm_OffF | Off (no clock) 	| Off           |
 * | Pwm_1X   | f_clk / 1      	| 40 MHz        |
 * | Pwm_2X   | f_clk / 2      	| 20 MHz        |
 * | Pwm_4X   | f_clk / 4      	| 10 MHz        |
 * | Pwm_8X   | f_clk / 8      	| 5 MHz         |
 * | Pwm_16X  | f_clk / 16     	| 2.5 MHz       |
 * | Pwm_32X  | f_clk / 32     	| 1.25 MHz      |
 * | Pwm_64X  | f_clk / 64     	| 625 kHz       |
 *
 * @param[in]  connector	Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  channel		Enum containing 20 kinds of PWM channels.
 * @param[in]  divider  	The PWM waveform clock divider.
 */
void Pwm_ClockSelect(ELVISIII_Pwm* connector, Pwm_Channel channel, Pwm_ClockDivider divider)
{
	NiFpga_Status status;

	/*
	 * Write the new value of the Clock Select Register to the device.
	 */
	status = NiFpga_WriteU8(NiELVISIIIv10_session, connector->cs[channel], divider);

	/*
	 * Check if there was an error writing to the PWM Clock Select Register.
	 *
	 * If there was an error then print an error message to stdout.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the PWM Clock Select Register!");

	return;
}


/**
 * Sets the maximum counter value for the PWM.
 *
 * The behavior of the counter depends on waveform set by Pwm_Waveform.
 *
 * Pwm_Disabled:
 * The value is ignored
 *
 * Pwm_Enabled:
 * The PWM counter counts to MAX, then resets to 0
 *
 * @param[in]  connector	Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  channel		Enum containing 20 kinds of PWM channels.
 * @param[in]  counterMax  	The maximum counter value.
 */
void Pwm_CounterMaximum(ELVISIII_Pwm* connector, Pwm_Channel channel, uint16_t counterMax)
{
	NiFpga_Status status;

	/*
	 * Write the new value to the PWM Maximum Count Register.
	 */
	status = NiFpga_WriteU16(NiELVISIIIv10_session, connector->max[channel], counterMax);

	/*
	 * Check if there was an error writing to the PWM Maximum Count Register.
	 *
	 * If there was an error then print an error message to stdout.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the PWM Maximum Count Register!");

	return;
}


/**
 * Sets the comparison counter value for the PWM.
 * The behavior of the output depends on waveform set by Pwm_Configure.
 * This, along with the value set by Pwm_CounterMaximum, sets the duty cycle.
 *
 * Pwm_Disabled:
 * The value is ignored
 *
 * Pwm_Enabled and Pwm_NotInverged:
 * Clears the output when CNTR = CMP
 *
 * Pwm_Enabled and Pwm_Inverted:
 * Sets the output when CNTR = CMP
 *
 * @param[in]  connector		Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  channel			Enum containing 20 kinds of PWM channels.
 * @param[in]  counterCompare  	The comparison counter value.
 */
void Pwm_CounterCompare(ELVISIII_Pwm* connector, Pwm_Channel channel, uint16_t counterCompare)
{
	NiFpga_Status status;

	/*
	 * Write the new value of the PWM Compare Register to the device.
	 */
	status = NiFpga_WriteU16(NiELVISIIIv10_session, connector->cmp[channel], counterCompare);

	/*
	 * Check if there was an error writing to the PWM Compare Register.
	 *
	 * If there was an error then print an error message to stdout.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the PWM Compare Register!");

	return;
}


/**
 * Gets the current value of the PWM counter.
 * The behavior of the counter depends on the waveform set by Pwm_Configure and the maximum counter value set by Pwm_CounterMaximum.\
 *
 * @param[in]  connector		Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  channel			Enum containing 20 kinds of PWM channels.
 *
 * @return the counter value.
 */
uint16_t Pwm_Counter(ELVISIII_Pwm* connector, Pwm_Channel channel)
{
	NiFpga_Status status;
	uint16_t cntrValue;

	/*
	 * Get the value from the PWM Counter Register.
	 */
	status = NiFpga_ReadU16(NiELVISIIIv10_session, connector->cntr[channel], &cntrValue);

	/*
	 * Check if there was an error writing to the PWM Counter Register.
	 *
	 * If there was an error then print an error message to stdout.
	 */
	NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0, "Could not read from the PWM counter register!");

	return cntrValue;
}


/**
 * Write the value to the System Select Register.
 *
 * @param[in]  connector		Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  channel			Enum containing 20 kinds of PWM channels.
 */
void Pwm_Select(ELVISIII_Pwm* connector, Pwm_Channel channel)
{
	NiFpga_Status status;
	uint64_t selectReg;

	/*
	 * PWM outputs are on pins shared with other onboard devices.
	 * To output on a physical pin, select the PWM on the appropriate SELECT Register.
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
	 * Clear proper bits.
	 */
	selectReg = selectReg & (~(0b11 << (channel * 2)));

	/*
	 * Set bit2 of the SYSSELECTA/SYSSELECTB register to enable Encoder functionality.
	 * The functionality of the bit is specified in the documentation.
	 */
	selectReg = selectReg | (0b01 << (channel * 2));

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
