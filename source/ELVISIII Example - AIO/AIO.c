/**
 * Example for performing Basic Analog Input and Output
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
#include "AIO.h"

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
ELVISIII_Ai connector_A = {AIACNFG, AIACNTR, AIACNT, AIAVALRDY, {99764, 99768, 99772, 99776, 99780, 99784, 99788, 99792, 99796, 99800, 99804, 99808}};

/*
 * Initialize the register addresses of AI in connector B
 */
ELVISIII_Ai connector_B = {AIBCNFG, AIBCNTR, AIBCNT, AIBVALRDY, {99812, 99816, 99820, 99824, 99828, 99832, 99836, 99840, 99844, 99848, 99852, 99856}};

/*
 * Initialize the register addresses of AO
 */
ELVISIII_Ao Ao = {AOSYSGO, AOSYSSTAT};


/**
 * Fix Point (signed) ----> double
 *
 * @param[in]  value	Fix Point Value presented in the form of unsigned int.
 *
 * @return a double value.
 */
double sFxp2Double(unsigned int value)
{
	if (value & (1 << (AI_WordLength - 1)))
	{//negative
		value = ~value;
		++value;
		return (double)((-1) * value / pow(2, (AI_WordLength - AI_IntegerWordLength)) - 32);
	}
	else
	{//positive
		return (double)(value / pow(2, (AI_WordLength - AI_IntegerWordLength)));
	}
}


/**
 * double ----> Fix Point (signed)
 *
 * @param[in]  value	Double Value.
 *
 * @return an unsigned int value.
 */
unsigned int sDouble2Fxp(double value)
{
	if (value < 0)
	{//negative
		unsigned int result = value * (-1) * pow(2, (AO_WordLength - AO_IntegerWordLength));
		--result;
		result = ~result;
		return result;
	}
	else
	{//positive
		return (unsigned int)(value * pow(2, (AO_WordLength - AO_IntegerWordLength)));
	}
}


/**
 * Set the AI Counter Register.
 *
 * @param[in]  connector	Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  counter		The number of valid channels.
 */
void Ai_Counter(ELVISIII_Ai* connector, uint8_t counter)
{
	NiFpga_Status status;
	bool flag = true;
	uint8_t Counter = 0;

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

	/*
	 * Ensure that the value was written into the proper register.
	 */
	while (flag)
	{
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

		if (Counter == counter)
		{
			flag = false;
		}
	}

	return;
}


/**
 * Set the AI Configuration Register.
 *
 * @param[in]  connector	Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  channel		Enum containing 12 kinds of channels (8 RSE + 4 DIFF).
 * @param[in]  range		Enum containing 4 kinds of ranges (±10 V, ±5 V, ±2 V, ±1 V).
 */
void Ai_Configure(ELVISIII_Ai* connector, Ai_Channel channel, Ai_Range range)
{
	NiFpga_Status status;

	NiFpga_Bool rdy;
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
		/*
		 * Get the value from the AI Ready Register (true or false).
		 */
		status = NiFpga_ReadBool(NiELVISIIIv10_session, connector->rdy, &rdy);

		/*
		 * Check if there was an error writing to the read register.
		 *
		 * If there was an error then print an error message to stdout and return.
		 */
		NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the AI Ready Register!");

		/*
		 * Get the values from the AI Configuration Register.
		 */
		status = NiFpga_ReadArrayU8(NiELVISIIIv10_session, connector->cnfg, ConfigValue, Counter);

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

	return;
}


/**
 * Set the AI Divisor Register.
 *
 * @param[in]  connector		Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  ClockRate		Onboard clock rate of FPGA, the default is 40 MHz.
 * @param[in]  SampleRate		The Sample Rate.
 */
void Ai_Divisor(ELVISIII_Ai* connector, uint32_t ClockRate, uint32_t SampleRate)
{
	NiFpga_Status status;
	bool flag = true;
	uint32_t Divisor = 0;

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
	status = NiFpga_WriteU32(NiELVISIIIv10_session, connector->cntr, divisor);

	/*
	 * Check if there was an error writing to the write register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the AI Divisor Register!");

	/*
	 * Ensure that the value was written into the proper register.
	 */
	while(flag)
	{
		/*
		 * Read the value from the AI Divisor Register.
		 */
		status = NiFpga_ReadU32(NiELVISIIIv10_session, connector->cntr, &Divisor);

		/*
		 * Check if there was an error reading from the register.
		 *
		 * If there was an error then print an error message to stdout and return.
		 */
		NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the AI Counter Register!");

		if (Divisor == divisor)
		{
			flag = false;
		}
	}

	return;
}


/**
 * Read value from one channel in either RSE  or DIFF mode.
 *
 * @param[in]  connector	Only connector_A or connector_B can be used unless you know the addresses of certain registers.
 * @param[in]  channel		Enum containing 12 kinds of channels (8 RSE + 4 DIFF).
 *
 * @return the voltage value in volts.
 */
double Aio_Read(ELVISIII_Ai* connector, Ai_Channel channel)
{
	NiFpga_Status status;
	unsigned short int Channel = channel;
	uint32_t value = 0;

	/*
	 * Get the value from certain AI Value Register.
	 */
	if ((Channel >> 3) == 1)
	{//RSE mode
		status = NiFpga_ReadU32(NiELVISIIIv10_session, connector->val[Channel - RSE_NUM], &value);
	}
	else
	{//DIFF mode
		status = NiFpga_ReadU32(NiELVISIIIv10_session, connector->val[Channel + RSE_NUM], &value);
	}

	/*
	 * Check if there was an error writing to the read register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the AI Value Register!");

	return sFxp2Double(value);
}


/**
 * Write the value to one AO Value Register.
 *
 * @param[in]  Ao		containing addresses of AO registers.
 * @param[in]  value	the value used to write into the AO Value Register.
 * @param[in]  valreg	choose proper AO Value.
 *
 * Note:
 * 1. The first parameter can only be &Ao (Initialized in AIO.h) unless you know certain addresses of AO Registers.
 * 2. The second parameter is between -10 v - 10 v.
 * 3. The third parameter only have 4 choices which can be seen in AIO.h (enum AoValReg).
 */
void Aio_Write(ELVISIII_Ao* Ao, double value, Ao_ValReg valreg)
{
	NiFpga_Status status;

	unsigned int result = sDouble2Fxp(value);

	NiFpga_Bool stat = false;
	NiFpga_Bool temp = false;

	bool flag = true;

	/*
	 * Write the value to the AO Value Register.
	 */
	status = NiFpga_WriteU32(NiELVISIIIv10_session, (uint32_t)valreg, (uint32_t)(result));

	/*
	 * Check if there was an error writing to the write register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the AO Value Register!");

	/*
	 * Get the value from the AO Status Register.
	 */
	status = NiFpga_ReadBool(NiELVISIIIv10_session, Ao->stat, &stat);

	/*
	 * Check if there was an error writing to the read register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the AI Status Register!");

	/*
	 * Write the value to the AO Start Register, the value is true.
	 */
	status = NiFpga_WriteBool(NiELVISIIIv10_session, Ao->go, true);

	/*
	 * Check if there was an error writing to the write register.
	 *
	 * If there was an error then print an error message to stdout and return.
	 */
	NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the AO Start Register!");

	/*
	 * Store the initial status value reading from the AO Status Register.
	 */
	temp = stat;

	/*
	 * Waiting until the status value changes.
	 */
	while(flag)
	{
		/*
		 * Read the value from the AO Status Register.
		 */
		status = NiFpga_ReadBool(NiELVISIIIv10_session, Ao->stat, &stat);

		/*
		 * Check if there was an error writing to the read register.
		 *
		 * If there was an error then print an error message to stdout and return.
		 */
		NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the AI Status Register!");

		if(stat != temp)
			flag = false;
	}

	return;
}
