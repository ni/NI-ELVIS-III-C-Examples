/**
 * Configuration for Analog Input Interrupt Request (IRQ)
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
#include "AIIRQ.h"


/*
 * Define some normal macros to ensure the input is within limitation.
 */
#if !defined(THRESHOLD_MAX)
#define THRESHOLD_MAX 5
#endif

#if !defined(THRESHOLD_MIN)
#define THRESHOLD_MIN 0
#endif

#if !defined(HYSTERESIS_MAX)
#define HYSTERESIS_MAX 1
#endif

#if !defined(HYSTERESIS_MIN)
#define HYSTERESIS_MIN 0
#endif


/*
 * Declare the ELVIS III NiFpga_Session so that it can be used by any function in
 * this file. The variable is actually defined in NiELVISIIIv10.c.
 *
 * This removes the need to pass the NiELVISIIIv10_session around to every function and
 * only has to be declared when it is being used.
 */
extern NiFpga_Session NiELVISIIIv10_session;

/*
 * Initialize the register addresses of AIIRQ in connector A.
 */
ELVISIII_IrqAi connector_A = {AIACNFG, AIACNTR, AIACNT, AIAVALRDY, {99764, 99768}, {IRQAI_A_0NO, IRQAI_A_1NO}, {99700, 99696}, {99708, 99712}, IRQAI_ACNFG};


/*
 * Convert double value to unsigned int value.
 *
 * @param[in]  value    Double Value.
 *
 * @return an unsigned int value to represent a fixed-point.
 */
unsigned int ConvertDoubleToUnsignedInt(double value)
{
    return (unsigned int)(value * pow(2, (AIIRQ_WordLength - AIIRQ_IntegerWordLength)));
}


/**
 * Set the number of valid channels on the connector.
 *
 * @param[in]  connector    A struct containing the registers for one connecter.
 * @param[in]  counter        The number of valid channels on the connector.
 */
void Ai_Counter(ELVISIII_IrqAi* connector, uint8_t counter)
{
    NiFpga_Status status;
    bool flag = true;
    uint8_t Counter = 0;

    /*
     * Write the counter value to the AI Counter Register.
     *
     * The returned NiFpga_Status value is stored for error checking.
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
         *
         * The returned NiFpga_Status value is stored for error checking.
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
 * Configure the range of the analog input channel.
 *
 * @param[in]  connector    A struct containing the registers for one connecter.
 * @param[in]  channel        Enum containing 2 kinds of channels (AI0, AI1).
 * @param[in]  range        Enum containing 4 kinds of ranges (±10 V, ±5 V, ±2 V, ±1 V).
 */
void Ai_Configure(ELVISIII_IrqAi* connector, Ai_Channel channel, Ai_Range range)
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
     *
     * The returned NiFpga_Status value is stored for error checking.
     */
    status = NiFpga_ReadArrayU8(NiELVISIIIv10_session, connector->cnfg, Config, RSE_NUM + DIFF_NUM);

    /*
     * Check if there was an error writing to the read register.
     *
     * If there was an error then print an error message to stdout and return.
     */
    NiELVISIIIv10_ReturnValueIfNotSuccess(status, 0.0, "Could not read from the AI Configuration Register!");

    /*
     * Generate AI Channel Selection Bit and AI Range Bit in the AI configuration array.
     */
    Config[Channel - RSE_NUM] = Channel | Range;

    /*
     * Write the configuration values to the AI Configuration Register.
     *
     * The returned NiFpga_Status value is stored for error checking.
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
     *
     * The returned NiFpga_Status value is stored for error checking.
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
         *
         * The returned NiFpga_Status value is stored for error checking.
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
         *
         * The returned NiFpga_Status value is stored for error checking.
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

    return;
}


/**
 * Configure the divisor for the analog sample rate.The default onboard clock rate of FPGA is 40 MHz.
 *
 * @param[in]  connector        A struct containing the registers for one connecter.
 * @param[in]  ClockRate        The onboard clock rate of FPGA.
 * @param[in]  SampleRate        The analog sample rate.
 */
void Ai_Divisor(ELVISIII_IrqAi* connector, uint32_t ClockRate, uint32_t SampleRate)
{
    NiFpga_Status status;
    bool flag = true;
    uint32_t Divisor = 0;

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

    /*
     * Generate the divisor, cast this value directly to a unsigned 32-bit value.
     */
    uint32_t divisor = (uint32_t)(ClockRate / SampleRate);

    /*
     * Write the divisor value to the AI Divisor Register.
     *
     * The returned NiFpga_Status value is stored for error checking.
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
         *
         * The returned NiFpga_Status value is stored for error checking.
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
 * Reserve the interrupt from FPGA and configure AI IRQ.
 *
 * @param[in]  irqChannel   A structure containing the registers and settings for a particular analog IRQ I/O to modify.
 * @param[in]  irqContext   IRQ context under which you reserve the IRQ.
 * @param[in]  irqNumber    The IRQ number (IRQNO_MIN - IRQNO_MAX).
 * @param[in]  threshold    The triggering voltage threshold (THRESHOLD_MIN - THRESHOLD_MAX).
 * @param[in]  hysteresis   The hysteresis (HYSTERESIS_MIN - HYSTERESIS_MAX) helps present multiple triggers due to noise.
 * @param[in]  type         Trigger type.
 *
 * @return  the configuration status.
 */
int32_t Irq_RegisterAiIrq(ELVISIII_IrqAi*    connector,
                          NiFpga_IrqContext* irqContext,
                          uint8_t            irqNumber,
                          double             threshold,
                          double             hysteresis,
                          Irq_Ai_Type        type)
{
    NiFpga_Bool status;

    uint8_t  cnfgValue;
    uint32_t Threshold;
    uint32_t Hysteresis;

    /*
     * Reserve an IRQ context. IRQ contexts are single-threaded; only one thread
     * can wait with a particular context at any given time. To minimize jitter
     * when first waiting on IRQs, reserve as many contexts as the application requires.
     * If a context is successfully reserved, you must unreserve it later.
     * Otherwise a memory leak will occur.
     */
    status = NiFpga_ReserveIrqContext(NiELVISIIIv10_session, irqContext);

    /*
     * Check if there was an error when you reserve an IRQ.
     *
     * If there was an error, print an error message to stdout and return configuration status.
     */
    NiELVISIIIv10_ReturnStatusIfNotSuccess(status, "A required NiFpga_IrqContext was not reserved.");

    /*
     * Limit the IRQ number within a range,
     * if the entered value is out of range, print an error message.
     */
    if (irqNumber > IRQNO_MAX || irqNumber < IRQNO_MIN)
    {
        printf("The specified IRQ Number is out of range.\n");
        return NiELVISIIIv10_Status_IrqNumberNotUsable;
    }

    /*
     * Check if the IRQ number or channel value already existed in the resource list,
     * return configuration status, and print an error message.
     */
    status = Irq_CheckReserved(connector->aiChannel, irqNumber);
    if (status == NiELVISIIIv10_Status_IrqNumberNotUsable)
    {
        printf("You have already registered an interrupt with the same interrupt number.\n");
        return status;
    }
    else if (status == NiELVISIIIv10_Status_IrqChannelNotUsable)
    {
        printf("You have already registered an interrupt with the same channel name.\n");
        return status;
    }

    /*
     * Write the value to the AI IRQ number register.
     *
     * The returned NiFpga_Status value is stored for error checking.
     */
    status = NiFpga_WriteU8(NiELVISIIIv10_session, connector->aiIrqNumber[connector->aiChannel], irqNumber);

    /*
     * Check if there was an error when you wrote to the AI IRQ Number Register.
     *
     * If there was an error, print an error message to stdout and return configuration status.
     */
    NiELVISIIIv10_ReturnStatusIfNotSuccess(status, "Could not write to AI IRQ Number Register!");

    /*
     * Coerce the threshold within THRESHOLD_MIN to THRESHOLD_MAX,
     * and coerce the hysteresis within HYSTERESIS_MIN to HYSTERESIS_MAX.
     */
    if (threshold > THRESHOLD_MAX)
    {
        threshold = THRESHOLD_MAX;
    }
    else if (threshold < THRESHOLD_MIN)
    {
        threshold = THRESHOLD_MIN;
    }
    if (hysteresis > HYSTERESIS_MAX)
    {
        hysteresis = HYSTERESIS_MAX;
    }
    else if (hysteresis < HYSTERESIS_MIN)
    {
        hysteresis = HYSTERESIS_MIN;
    }

    /*
     * Turn the threshold value from double to the fixed-point value.
     */
    Threshold = ConvertDoubleToUnsignedInt(threshold);

    /*
     * Write the value to the AI Threshold Register.
     *
     * The returned NiFpga_Status value is stored for error checking.
     */
    status = NiFpga_WriteU32(NiELVISIIIv10_session, connector->aiThreshold[connector->aiChannel], Threshold);

    /*
     * Check if there was an error when you wrote to the AI Threshold Register.
     *
     * If there was an error, print an error message to stdout and return configuration status.
     */
    NiELVISIIIv10_ReturnStatusIfNotSuccess(status, "Could not write to AI Threshold Register!");

    /*
     * Turn the hysteresis value from double to the fixed-point value.
     */
    Hysteresis = ConvertDoubleToUnsignedInt(hysteresis);

    /*
     * Write the value to the AI hysteresis register.
     *
     * The returned NiFpga_Status value is stored for error checking.
     */
    status = NiFpga_WriteU32(NiELVISIIIv10_session, connector->aiHysteresis[connector->aiChannel], Hysteresis);

    /*
     * Check if there was an error when you wrote to the AI threshold register.
     *
     * If there was an error, print an error message to stdout and return configuration status.
     */
    NiELVISIIIv10_ReturnStatusIfNotSuccess(status, "Could not write to AI Hysteresis Register!");

    /*
     * Get the current value of the AI configure register.
     *
     * The returned NiFpga_Status value is stored for error checking.
     */
    status = NiFpga_ReadU8(NiELVISIIIv10_session, connector->aiIrqConfigure, &cnfgValue);

    /*
     * Check if there was an error when you wrote to the AI threshold register.
     *
     * If there was an error, print an error message to stdout and return configuration status.
     */
    NiELVISIIIv10_ReturnStatusIfNotSuccess(status, "Could not read from the AI Configuration Register!");

    /*
     * Configure the IRQ triggered-type for the particular analog IRQ I/O.
     */
    if (connector->aiChannel == Irq_Ai_A0)
    {
        /*
         * Clear the value of the masked bits in the AI Configuration Register. This is
         * done so that the correct value can be set later on.
         */
        cnfgValue = cnfgValue & (~Irq_Ai_A0_Enable) & (~Irq_Ai_A0_Type);

        /*
         * Configure the value of the settings in the AI Configuration Register. If the
         * value to set is 0, this operation would not work unless the bit was
         * previously cleared. This is done so the triggered type is configured.
         */
        if (type == Irq_Ai_RisingEdge)
        {
            cnfgValue = cnfgValue | Irq_Ai_A0_Enable | Irq_Ai_A0_Type;
        }
        else if (type == Irq_Ai_FallingEdge)
        {
            cnfgValue = cnfgValue | Irq_Ai_A0_Enable;
        }
    }
    else if (connector->aiChannel == Irq_Ai_A1)
    {
        /*
         * Clear the value of the masked bits in the AI Configuration Register. This is
         * done so that the correct value can be set later on.
         */
        cnfgValue = cnfgValue & (~Irq_Ai_A1_Enable) & (~Irq_Ai_A1_Type);

        /*
         * Configure the value of the settings in the AI Configuration Register. If the
         * value to set is 0, this operation would not work unless the bit was
         * previously cleared. This is done so the triggered type is configured.
         */
        if (type == Irq_Ai_RisingEdge)
        {
            cnfgValue = cnfgValue | Irq_Ai_A1_Enable | Irq_Ai_A1_Type;
        }
        else if (type == Irq_Ai_FallingEdge)
        {
            cnfgValue = cnfgValue | Irq_Ai_A1_Enable;
        }
    }

    /*
     * Write the new value of the AI configure register to the device.
     *
     * The returned NiFpga_Status value is stored for error checking.
     */
    status = NiFpga_WriteU8(NiELVISIIIv10_session, connector->aiIrqConfigure, cnfgValue);

    /*
     * Check if there was an error when you wrote to the AI Threshold Register.
     *
     * If there was an error, print an error message to stdout and return configuration status.
     */
    NiELVISIIIv10_ReturnStatusIfNotSuccess(status, "Could not write to the AI Configuration Register!");

    /*
     * Add the channel value and IRQ number in the list.
     */
    Irq_AddReserved(connector->aiChannel, irqNumber);

    return NiELVISIIIv10_Status_Success;
}


/**
 * Clear the AI IRQ configuration.
 *
 * @param[in]  irqChannel       A structure containing the registers and settings for a particular analog IRQ I/O to modify.
 * @param[in]  irqContext       IRQ context under which to clear IRQ.
 * @param[in]  irqNumber        IRQ number that you need to clear.
 *
 * @return the configuration status.
 */
int32_t Irq_UnregisterAiIrq(ELVISIII_IrqAi*   connector,
                            NiFpga_IrqContext irqContext,
                            uint8_t           irqNumber)
{
    NiFpga_Bool status;

    uint8_t cnfgValue;

    /*
     * Limit the IRQ number within a range,
     * if the entered value is out of range, print an error message.
     */
    if (irqNumber > IRQNO_MAX || irqNumber < IRQNO_MIN)
    {
        printf("The specified IRQ Number is out of range.\n");
        return NiELVISIIIv10_Status_IrqNumberNotUsable;
    }

    /*
     * Check if the specified IRQ resource is registered.
     */
    status = Irq_CheckReserved(connector->aiChannel, irqNumber);
    if (status == NiELVISIIIv10_Status_Success)
    {
        /*
         * Did not find the resource in the list.
         */
        printf("You didn't register an interrupt with this IRQ number.\n");
        return NiELVISIIIv10_Status_Success;
    }

    /*
     * Get the current value of the AI Configuration Register.
     *
     * The returned NiFpga_Status value is stored for error checking.
     */
    status = NiFpga_ReadU8(NiELVISIIIv10_session, connector->aiIrqConfigure, &cnfgValue);

    /*
     * Check if there was an error when you wrote to the AI Threshold Register.
     *
     * If there was an error, print an error message to stdout and return configuration status.
     */
    NiELVISIIIv10_ReturnStatusIfNotSuccess(status, "Could not read from the AI Configuration Register!");

    /*
     * Disable the specified channel.
     */
    if (connector->aiChannel == Irq_Ai_A0)
    {
        /*
         * Clear the value of the masked bits in the AI Configuration Register. This is
         * done so AI0 is disabled.
         */
        cnfgValue = cnfgValue & (~Irq_Ai_A0_Enable);
    }
    else if (connector->aiChannel == Irq_Ai_A1)
    {
        /*
         * Clear the value of the masked bits in the AI Configuration Register. This is
         * done so AI1 is disabled.
         */
        cnfgValue = cnfgValue & (~Irq_Ai_A1_Enable);
    }


    /*
     * Write the new value of the AI Configuration Register to the device.
     *
     * The returned NiFpga_Status value is stored for error checking.
     */
    status = NiFpga_WriteU8(NiELVISIIIv10_session, connector->aiIrqConfigure, cnfgValue);

    /*
     * Check if there was an error when you wrote to the AI Threshold Register.
     *
     * If there was an error, print an error message to stdout and return configuration status.
     */
    NiELVISIIIv10_ReturnStatusIfNotSuccess(status, "Could not write to the AI Configuration Register!");


    /*
     * Remove the reserved resource in the list.
     *
     * The returned NiFpga_Status value is stored for error checking.
     */
    status = Irq_RemoveReserved(irqNumber);
    /*
     * Check if there was an error when you wrote to the AI Threshold Register.
     *
     * If there was an error, print an error message to stdout and return configuration status.
     */
    NiELVISIIIv10_ReturnStatusIfNotSuccess(status, "Could not release the IRQ resource!");

    /*
     * Unreserve an IRQ context obtained from Irq_ReserveIrqContext.
     * The returned NiFpga_Status value is stored for error checking.
     */
    status = NiFpga_UnreserveIrqContext(NiELVISIIIv10_session, irqContext);

    /*
     * Check if there was an error when you wrote to the AI Threshold Register.
     *
     * If there was an error, print an error message to stdout and return configuration status.
     */
    NiELVISIIIv10_ReturnStatusIfNotSuccess(status, "A required NiFpga_IrqContext was not unreserved.");

    return NiELVISIIIv10_Status_Success;
}
