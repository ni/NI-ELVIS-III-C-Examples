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

// Initialize the register addresses for a particular analog Input on bank A.
ELVISIII_Aio bank_A = {AIACNFG, AIACNTR, AOADMA_CNTR, AIACNT, AIADMA_ENA, 99558};

// Initialize the register addresses for a particular analog Input on bank B.
ELVISIII_Aio bank_B = {AIBCNFG, AIBCNTR, AOADMA_CNTR, AIBCNT, AIBDMA_ENA, 99566};

/**
 * Set the number of valid channels.
 *
 * @param[in]  bank    A struct containing the registers for one connecter.
 * @param[in]  counter        The number of valid channels on the bank.
 */
void Ai_Counter(ELVISIII_Aio* bank, uint8_t counter)
{
    NiFpga_Status status;

    // Write the counter value to the AI Counter Register.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_WriteU8(NiELVISIIIv10_session, bank->cnt, counter);

    // Check if there was an error writing to the write register.
    // If there was an error then print an error message to stdout and return.
    NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the AI Counter Register!");

    return;
}

/**
 * Set the AI Configuration Register and configure the range of the analog input channel.
 *
 * @param[in]  bank    A struct containing the registers for one connecter.
 * @param[in]  channel        Enum containing 12 kinds of channels (8 RSE + 4 DIFF).
 * @param[in]  range        Enum containing 4 kinds of ranges (±10 V, ±5 V, ±2 V, ±1 V).
 */
void Ai_Configure(ELVISIII_Aio* bank, Ai_Channel channel, Ai_Range range)
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

    // Get the initial values from the AI Configuration Register.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_ReadArrayU8(NiELVISIIIv10_session, bank->cnfg, Config, RSE_NUM + DIFF_NUM);

    // Check if there was an error writing to the read register.
    // If there was an error then print an error message to stdout and return.
    NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the AI Configuration Register!");

    // Generate AI Channel Selection Bit and AI Range Bit.For the AI Configuration Register, cast the value to a unsigned 8-bit value.
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
    status = NiFpga_WriteArrayU8(NiELVISIIIv10_session, bank->cnfg, (const uint8_t*)(Config), RSE_NUM + DIFF_NUM);

    // Check if there was an error writing to the write register.
    // If there was an error then print an error message to stdout and return.
    NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the AI Configuration Register!");

    // Read the value from the AI Counter Register.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_ReadU8(NiELVISIIIv10_session, bank->cnt, &Counter);

    // Check if there was an error reading from the register.
    // If there was an error then print an error message to stdout and return.
    NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not read from the AI Counter Register!");

    // Get the value from the AI Configuration Register to check whether the configuration writing process is OK.
    // The returned NiFpga_Status value is stored for error checking.
    while(flag)
    {
        rdy = true;

        // Get the values from the AI Configuration Register.
        // The returned NiFpga_Status value is stored for error checking.
        status = NiFpga_ReadArrayU8(NiELVISIIIv10_session, bank->cnfg, ConfigValue, RSE_NUM + DIFF_NUM);

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

    // Waiting for reading all the numbers from AI FIFO.
    sleep(1);

    return;
}

/**
 * Configure the divisor for the AI sample rate.The default onboard clock rate of FPGA is 40 MHz.
 *
 * @param[in]  bank    A struct containing the registers for one connecter.
 * @param[in]  ClockRate    The onboard clock rate of FPGA.
 * @param[in]  SampleRate    The analog sample rate.
 */
void Ai_Divisor(ELVISIII_Aio* bank, uint32_t ClockRate, uint32_t SampleRate)
{
    NiFpga_Status status;

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
    status = NiFpga_WriteU32(NiELVISIIIv10_session, bank->ai_cntr, divisor);

    // Check if there was an error writing to the write register.
    // If there was an error then print an error message to stdout and return.
    NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the AI Divisor Register!");

    return;
}

/**
 * Set the DMA Enable Flag for one bank.The flag controls whether the DMA is enabled for a specific bank.
 *
 * @param[in]  bank    A struct containing the registers for one connecter.
 */
void Ai_Enable(ELVISIII_Aio* bank)
{
    NiFpga_Status status;

    // Write the NiFpga_True to the AI DMA Enable Register.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_WriteBool(NiELVISIIIv10_session, bank->ai_enable, NiFpga_True);

    // Check if there was an error writing to the write register.
    // If there was an error then print an error message to stdout and return.
    NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the AI DMA Enable Register!");

    return;

}

/**
 * Read groups of values from an AI FIFO.
 *
 * @param[in]  bank                    A struct containing the registers for one connecter.
 * @param[in]  fifo                            AI target-to-host FIFO from which to read
 * @param[in]  fxp_buffer_receive            groups of values in an AI FIFO, get from one channel.
 * @param[in]  fifo_size                    The size of the AI FIFO.
 * @param[in]  timeout                        timeout in milliseconds, or NiFpga_InfiniteTimeout
 * @param[in]  elementsRemaining            if non-NULL, outputs the number of elements
 *                                          remaining in the host memory part of the DMA FIFO
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
void Ai_ReadFifo(ELVISIII_Aio*                  bank,
                 TargetToHost_FIFO_FXP     fifo,
                  uint64_t*                         fxp_buffer_receive,
                  size_t                            fifo_size,
                  uint32_t                          timeout,
                  size_t*                           elementsRemaining)
{
    NiFpga_Status status;

    // Get Groups of fixed-point values from an AI FIFO.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_ReadFifoU64(NiELVISIIIv10_session,
                                fifo,
                                fxp_buffer_receive,
                                fifo_size,
                                timeout,
                                elementsRemaining);

    // Check if there was an error reading from register.
    // If there was an error then print an error message to stdout and return.
    NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the AI FIFO!");

    return;
}

/**
 * Convert unsigned long long int values of the fixed-point in the FIFO to double values.
 *
 * @param[in]  fxp_buffer_receive            groups of fixed-point values get from one channel.
 *                                             The fixed-point value is an unsigned long long int value.
 * @param[in]  fifo_size                    The size of the AI FIFO.
 * @param[in]  value                        Double value.
 */
void ConvertUint64ArrayToDoubleArray(uint64_t *fxp_buffer_receive, size_t fifo_size, double *value)
{
    int i;
    uint32_t temp;
    for (i = 0; i < fifo_size; ++i)
    {
        temp = (uint32_t)fxp_buffer_receive[i];
        if (temp & (1 << (AI_WordLength - 1)))
        {
        //Unsigned value is negative
            temp = ~temp;
            ++temp;
            value[i] = (double)(temp / pow(2, (AI_WordLength - AI_IntegerWordLength)) * (-1));
        }
        else
        {
        //Unsigned value is positive
            value[i] = (double)(temp / pow(2, (AI_WordLength - AI_IntegerWordLength)));
        }
    }

    return;
}

/**
 * Configure the divisor for the AO sample rate.The default onboard clock rate of FPGA is 40 MHz.
 *
 * @param[in]  bank    A struct containing the registers for one connecter.
 * @param[in]  ClockRate    The onboard clock rate of FPGA.
 * @param[in]  SampleRate    The analog sample rate.
 */
void Ao_Divisor(ELVISIII_Aio* bank, uint32_t ClockRate, uint32_t SampleRate)
{
    NiFpga_Status status;

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

    // Write the divisor value to the AO Divisor Register.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_WriteU32(NiELVISIIIv10_session, bank->ao_cntr, divisor);

    // Check if there was an error writing to the write register.
    // If there was an error then print an error message to stdout and return.
    NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the AO Divisor Register!");

    return;
}

/**
 * Set the DMA Enable value for an analog output channel.The value controls
 * whether the DMA is enabled for a specific analog output channel.
 *
 * @param[in]  bank    A struct containing the registers for one connecter.
 * @param[in]  channel        Enum containing 2 kinds of channels.
 */
void Ao_Enable(ELVISIII_Aio* bank, Ao_Channel channel)
{
    NiFpga_Status status;
    uint8_t bit = channel;

    uint8_t enable = 0;

    // Get the initial values from the AO DMA Enable Register.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_ReadU8(NiELVISIIIv10_session, bank->ao_enable, &enable);

    // Check if there was an error writing to the read register.
    // If there was an error then print an error message to stdout and return.
    NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the AO DMA Enable Register!");

    // Clear the value of the bits in the DMA Enable register. This is
    // done so that the correct value can be set later on.
    enable = enable & ~bit;

    // Set the value of the bits in the DMA Enable register. If the
    // value to set is 0 this operation would not work unless the bit was
    // previously cleared.
    enable = enable | bit;

    // Write the value to the AO DMA Enable Register.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_WriteU8(NiELVISIIIv10_session, bank->ao_enable, enable);

    // Check if there was an error writing to the register.
    // If there was an error then print an error message to stdout and return.
    NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not write to the AO DMA Enable Register!");

    return;
}

/**
 * Convert double values to unsigned long long int values to represent the fixed-point in the FIFO.
 *
 * @param[in]  value                    Double value
 * @param[in]  fxp_buffer_send            groups of the fixed-point values to be written.
 *                                         The fixed-point value is an unsigned long long int value.
 * @param[in]  fifo_size                The size of the AO FIFO.
 */
void ConvertDoubleArrayToUint64Array(double *value, uint64_t *fxp_buffer_send, size_t fifo_size)
{
    int i;
    uint32_t temp = 0;

    for (i = 0; i < fifo_size; ++i)
    {
        if (value[i] < 0)
        {
        //Value is negative.
            temp = (uint32_t)(value[i] * (-1) * pow(2, (AO_WordLength - AO_IntegerWordLength)));
            --temp;
            temp = ~temp;
            fxp_buffer_send[i] = (uint64_t)(0xFFFFFFFF00000000 + temp);
        }
        else
        {
        //Value is positive.
            fxp_buffer_send[i] = (uint64_t)(value[i] * pow(2, (AO_WordLength - AO_IntegerWordLength)));
        }
    }

    return;
}

/**
 * Read groups of values to an AO FIFO.
 *
 * @param[in]  bank                A struct containing the registers for one connecter.
 * @param[in]  fifo                        AO host-to-target FIFO from which to write
 * @param[in]  fxp_buffer_send            groups of values to be written.
 * @param[in]  fifo_size                The size of the AO FIFO
 * @param[in]  timeout                    timeout in milliseconds, or NiFpga_InfiniteTimeout
 * @param[in]  elementsRemaining        if non-NULL, outputs the number of elements
 *                                      remaining in the host memory part of the DMA FIFO
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
void Ao_WriteFifo(ELVISIII_Aio*                 bank,
                  HostToTarget_FIFO_FXP     fifo,
                   const uint64_t*               fxp_buffer_send,
                   size_t                        fifo_size,
                   uint32_t                      timeout,
                   size_t*                       elementsRemaining)
{
    NiFpga_Status status;

    // Write Groups of fixed-point values to an AO FIFO.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_WriteFifoU64(NiELVISIIIv10_session,
                                fifo,
                                fxp_buffer_send,
                                fifo_size,
                                timeout,
                                elementsRemaining);

    // Check if there was an error reading from register.
    // If there was an error then print an error message to stdout and return.
    NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the AO FIFO!");

    return;
}
