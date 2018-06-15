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

// Initialize the register addresses for the analog input on connector A.
ELVISIII_Ai connector_A = {AIACNFG, AIACNTR, AIACNT, AIAVALRDY, {99764, 99768, 99772, 99776, 99780, 99784, 99788, 99792, 99796, 99800, 99804, 99808}};

// Initialize the register addresses for the analog input on connector B.
ELVISIII_Ai connector_B = {AIBCNFG, AIBCNTR, AIBCNT, AIBVALRDY, {99812, 99816, 99820, 99824, 99828, 99832, 99836, 99840, 99844, 99848, 99852, 99856}};

// Initialize the register addresses for analog Output on connector A.
ELVISIII_Ao Ao = {AOSYSGO, AOSYSSTAT};

/**
 * Convert unsigned int value to double value.
 *
 * @param[in]  value	Unsigned int value of a fixed-point.
 *
 * @return a double value.
 */
double ConvertUnsignedIntToDouble(unsigned int value)
{
    if (value & (1 << (AI_WordLength - 1)))
    {
        //value is negative
        value = ~value;
        ++value;
        return (double)((-1) * value / pow(2, (AI_WordLength - AI_IntegerWordLength)) - 32);
    }
    else
    {
        //value is positive
        return (double)(value / pow(2, (AI_WordLength - AI_IntegerWordLength)));
    }
}

/**
 * Convert double value to unsigned int value.
 *
 * @param[in]  value    Double Value.
 *
 * @return an unsigned int value to represent a fixed-point.
 */
unsigned int ConvertDoubleToUnsignedInt(double value)
{
    if (value < 0)
    {
        //value is negative
        unsigned int result = value * (-1) * pow(2, (AO_WordLength - AO_IntegerWordLength));
        --result;
        result = ~result;
        return result;
    }
    else
    {
        //value is positive
        return (unsigned int)(value * pow(2, (AO_WordLength - AO_IntegerWordLength)));
    }
}

/**
 * Set the number of valid channels on the connector.
 *
 * @param[in]  connector    A struct containing the registers for one connecter.
 * @param[in]  counter      The number of valid channels on the connector.
 */
void Ai_Counter(ELVISIII_Ai* connector, uint8_t counter)
{
    NiFpga_Status status;
    bool flag = true;
    uint8_t Counter = 0;

    // Write the counter value to the AI Counter Register.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_WriteU8(NiELVISIIIv10_session, connector->cnt, counter);

    // Check if there was an error writing to the write register.
    // If there was an error then print an error message to stdout and return.
    NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the AI Counter Register!");

    // Ensure that the value was written into the right register.
    while (flag)
    {
        // Get the value from the AI Counter Register.
        // The returned NiFpga_Status value is stored for error checking.
        status = NiFpga_ReadU8(NiELVISIIIv10_session, connector->cnt, &Counter);

        // Check if there was an error reading from the register.
        // If there was an error then print an error message to stdout and return.
        NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not read from the AI Counter Register!");

        if (Counter == counter)
        {
            flag = false;
        }
    }

    return;
}

/**
 * Configure the range of the analog input channel.
 *
 * @param[in]  connector    A struct containing the registers for one connecter.
 * @param[in]  channel      Enum containing 12 kinds of channels (8 RSE + 4 DIFF).
 * @param[in]  range        Enum containing 4 kinds of ranges (±10 V, ±5 V, ±2 V, ±1 V).
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

    // Get the initial values from the AI Configuration Register.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_ReadArrayU8(NiELVISIIIv10_session, connector->cnfg, Config, RSE_NUM + DIFF_NUM);

    // Check if there was an error writing to the read register.
    // If there was an error then print an error message to stdout and return.
    NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the AI Configuration Register!");

    // Generate AI Channel Selection Bit and AI Range Bit, for the AI Configuration Register.
    if ((Channel >> 3) == 1)
    {
        //RSE mode
        Config[Channel - RSE_NUM] = Channel | Range;
    }
    else
    {
        //DIFF mode
        Config[Channel + RSE_NUM] = Channel | Range;
    }

    // Write the configuration values to the AI Configuration Register.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_WriteArrayU8(NiELVISIIIv10_session, connector->cnfg, (const uint8_t*)(Config), RSE_NUM + DIFF_NUM);

    // Check if there was an error writing to the write register.
    // If there was an error then print an error message to stdout and return.
    NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the AI Configuration Register!");

    // Get the value from the AI Counter Register.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_ReadU8(NiELVISIIIv10_session, connector->cnt, &Counter);

    // Check if there was an error reading from the register.
    // If there was an error then print an error message to stdout and return.
    NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not read from the AI Counter Register!");

    // Check if there was an error in the configuration writing process.
    // The returned NiFpga_Status value is stored for error checking.
    while(flag)
    {
        // Get the value from the AI Ready Register .
        // The returned NiFpga_Status value is stored for error checking.
        status = NiFpga_ReadBool(NiELVISIIIv10_session, connector->rdy, &rdy);

        // Check if there was an error writing to the read register.
        // If there was an error then print an error message to stdout and return.
        NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the AI Ready Register!");

        // Get the values from the AI Configuration Register.
        // The returned NiFpga_Status value is stored for error checking.
        status = NiFpga_ReadArrayU8(NiELVISIIIv10_session, connector->cnfg, ConfigValue, Counter);

        // Check if there was an error writing to the read register.
        // If there was an error then print an error message to stdout and return.
        NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the AI Configuration Register!");

        // Waiting to finish reading from AI Configuration Register.
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
 * Configure the divisor for the analog sample rate.The default onboard clock rate of FPGA is 40 MHz.
 *
 * @param[in]  connector        A struct containing the registers for one connecter.
 * @param[in]  ClockRate        The onboard clock rate of FPGA.
 * @param[in]  SampleRate       The analog sample rate.
 */
void Ai_Divisor(ELVISIII_Ai* connector, uint32_t ClockRate, uint32_t SampleRate)
{
    NiFpga_Status status;
    bool flag = true;
    uint32_t Divisor = 0;

    // Control the range of the sample rate from MIN_SAMPLE_RATE to MAX_SAMPLE_RATE.
    if (SampleRate > MAX_SAMPLE_RATE)
    {
        SampleRate = MAX_SAMPLE_RATE;
    }

    if (SampleRate < MIN_SAMPLE_RATE)
    {
        SampleRate = MIN_SAMPLE_RATE;
    }

    // Generate the divisor, cast this value directly to a unsigned 32-bit value.

    uint32_t divisor = (uint32_t)(ClockRate / SampleRate);

    // Write the divisor value to the AI Divisor Register.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_WriteU32(NiELVISIIIv10_session, connector->cntr, divisor);

    // Check if there was an error writing to the write register.
    // If there was an error then print an error message to stdout and return.
    NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the AI Divisor Register!");

    // Ensure that the value was written into the right register.
    while(flag)
    {
        // Get the value from the AI Divisor Register.
        // The returned NiFpga_Status value is stored for error checking.
        status = NiFpga_ReadU32(NiELVISIIIv10_session, connector->cntr, &Divisor);

        // Check if there was an error reading from the register.
        // If there was an error then print an error message to stdout and return.
        NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the AI Counter Register!");

        if (Divisor == divisor)
        {
            flag = false;
        }
    }

    return;
}

/**
 * Read value from one analog input channel.
 *
 * @param[in]  connector    A struct containing the registers for one connecter.
 * @param[in]  channel      Enum containing 12 kinds of channels (8 RSE + 4 DIFF).
 *
 * @return the voltage value in volts.
 */
double Aio_Read(ELVISIII_Ai* connector, Ai_Channel channel)
{
    NiFpga_Status status;
    unsigned short int Channel = channel;
    uint32_t value = 0;

    // Get the value from certain AI Value Register.
    // The returned NiFpga_Status value is stored for error checking.
    if ((Channel >> 3) == 1)
    {
        //RSE mode
        status = NiFpga_ReadU32(NiELVISIIIv10_session, connector->val[Channel - RSE_NUM], &value);
    }
    else
    {
        //DIFF mode
        status = NiFpga_ReadU32(NiELVISIIIv10_session, connector->val[Channel + RSE_NUM], &value);
    }

    // Check if there was an error writing to the read register.
    // If there was an error then print an error message to stdout and return.
    NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the AI Value Register!");

    return ConvertUnsignedIntToDouble(value);
}

/**
 * Write the value to AO Value Register.
 *
 * @param[in]  Ao               A struct containing the registers for analog output.
 * @param[in]  value            The voltage value to be written
 * @param[in]  ValueRegister    Enum containing addresses for value registers.
 *
 */
void Aio_Write(ELVISIII_Ao* Ao, double value, Ao_ValueRegister ValueRegister)
{
    NiFpga_Status status;

    unsigned int result = ConvertDoubleToUnsignedInt(value);

    NiFpga_Bool stat = false;
    NiFpga_Bool temp = false;

    bool flag = true;

    // Write the value to the AO Value Register.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_WriteU32(NiELVISIIIv10_session, (uint32_t)ValueRegister, (uint32_t)(result));

    // Check if there was an error writing to the write register.
    // If there was an error then print an error message to stdout and return.
    NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the AO Value Register!");

    // Get the value from the AO Status Register.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_ReadBool(NiELVISIIIv10_session, Ao->stat, &stat);

    // Check if there was an error writing to the read register.
    // If there was an error then print an error message to stdout and return.
    NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the AI Status Register!");

    // Write the value to the AO Start Register, the value is true.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_WriteBool(NiELVISIIIv10_session, Ao->go, true);

    // Check if there was an error writing to the write register.
    // If there was an error then print an error message to stdout and return.
    NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the AO Start Register!");

    // Store the initial status value reading from the AO Status Register.
    temp = stat;

    // Waiting until the status value changes.
    while(flag)
    {
        // Get the value from the AO Status Register.
        status = NiFpga_ReadBool(NiELVISIIIv10_session, Ao->stat, &stat);

        // Check if there was an error writing to the read register.
        // If there was an error then print an error message to stdout and return.
        NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the AI Status Register!");

        if(stat != temp)
        {
            flag = false;
        }
    }

    return;
}
