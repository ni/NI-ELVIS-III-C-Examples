/**
 * Configuration for Button Input Interrupt Request (IRQ)
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
#include "ButtonIRQ.h"

/*
 * Declare the ELVIS III NiFpga_Session so that it can be used by any function in
 * this file. The variable is actually defined in NiELVISIIIv10.c.
 *
 * This removes the need to pass the NiELVISIIIv10_session around to every function and
 * only has to be declared when it is being used.
 */
extern NiFpga_Session NiELVISIIIv10_session;

// Initialize the register addresses of ButtonIRQ.
ELVISIII_IrqButton irqButton = {IRQDI_BTNCNT, IRQDI_BTNNO, IRQDI_BTNENA, IRQDI_BTNRISE, IRQDI_BTNFALL};

/**
 * Reserve the interrupt from FPGA and configure Button IRQ.
 *
 * @param[in]  irqChannel    A struct containing the registers and settings for a particular button IRQ I/O to modify.
 * @param[in]  irqContext   IRQ context under which you reserve the IRQ.
 * @param[in]  irqNumber    The IRQ number (IRQNO_MIN-IRQNO_MAX).
 * @param[in]  count        The incremental times that you want to use to trigger the interrupt.
 * @param[in]  type         The trigger type that you want to use to increment the count.
 *
 * @return the configuration status.
 */
int32_t Irq_RegisterButtonIrq(ELVISIII_IrqButton* irqButton,
                              NiFpga_IrqContext*  irqContext,
                              uint8_t             irqNumber,
                              uint32_t            count,
                              Irq_Button_Type     type)
{
    NiFpga_Bool status;

    // Reserve an IRQ context. IRQ contexts are single-threaded; only one thread
    // can wait with a particular context at any given time. To minimize jitter
    // when first waiting on IRQs, reserve as many contexts as the application requires.
    // If a context is successfully reserved, you must unreserve it later.
    // Otherwise a memory leak will occur.
    status = NiFpga_ReserveIrqContext(NiELVISIIIv10_session, irqContext);

    // Check if there was an error when you reserved an IRQ.
    // If there was an error, print an error message to stdout and return the configuration status.
    NiELVISIIIv10_ReturnStatusIfNotSuccess(status, "A required NiFpga_IrqContext was not reserved.");

    // Limit the IRQ number within a range,
    // if the entered value is out of range, print an error message.
    if (irqNumber > IRQNO_MAX || irqNumber < IRQNO_MIN)
    {
        printf("The specified IRQ Number is out of range.\n");
        return NiELVISIIIv10_Status_IrqNumberNotUsable;
    }

    // Check if the IRQ number or channel value already exists in the resource list,
    // return configuration status, and print an error message.
    status = Irq_CheckReserved(irqButton->btnChannel, irqNumber);
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

    // Write the value to the Button IRQ Number Register.
    status = NiFpga_WriteU8(NiELVISIIIv10_session, irqButton->btnIrqNumber, irqNumber);

    // Check if there was an error when you reserved an IRQ.
    // If there was an error, print an error message to stdout and return the configuration status.
    NiELVISIIIv10_ReturnStatusIfNotSuccess(status, "Could not write to Button IRQ Number Register!");

     // Write the value to the Button IRQ Count Register.
    status = NiFpga_WriteU32(NiELVISIIIv10_session, irqButton->btnCount, count);

    // Check if there was an error when you reserved an IRQ.
    // If there was an error, print an error message to stdout and return the configuration status.
    NiELVISIIIv10_ReturnStatusIfNotSuccess(status, "Could not write to Button IRQ Count Register!");

    // Write the value to the Button IRQ Enable Register.
    status = NiFpga_WriteBool(NiELVISIIIv10_session, irqButton->btnIrqEnable, NiFpga_True);

    // Check if there was an error when you reserved an IRQ.
    // If there was an error, print an error message to stdout and return the configuration status.
    NiELVISIIIv10_ReturnStatusIfNotSuccess(status, "Could not write to IRQ Enable Register!");

    // Configure the IRQ trigger type for the particular button IRQ I/O.
    if (type == Irq_Button_RisingEdge || type == Irq_Button_Edge)
    {
        // Write the value to the Button IRQ Rising Edge-Trigger Register.
        status = NiFpga_WriteBool(NiELVISIIIv10_session, irqButton->btnIrqRisingEdge, NiFpga_True);

        // Check if there was an error when you reserved an IRQ.
        // If there was an error, print an error message to stdout and return the configuration status.
        NiELVISIIIv10_ReturnStatusIfNotSuccess(status, "Could not write to IRQ Rise-Trigger Register!");
    }
    else if (type == Irq_Button_FallingEdge || type == Irq_Button_Edge)
    {
        // Write the value to the Button IRQ falling edge-trigger register.
        status = NiFpga_WriteBool(NiELVISIIIv10_session, irqButton->btnIrqFallingEdge, NiFpga_True);

        // Check if there was an error when you reserved an IRQ.
        // If there was an error, print an error message to stdout and return the configuration status.
        NiELVISIIIv10_ReturnStatusIfNotSuccess(status, "Could not write to IRQ Fall-Trigger Register!");
    }

    // Add the channel value and IRQ number to the list.
    Irq_AddReserved(irqButton->btnChannel, irqNumber);

    return NiELVISIIIv10_Status_Success;
}

/**
 * Unreserve the interrupt from FPGA and disable the particular button IRQ I/O.
 *
 * @param[in]  irqChannel  A structure containing the registers and settings for a particular button IRQ IO to modify.
 * @param[in]  irqContext  IRQ context under which you need to unreserve.
 *
 * @return the configuration status.
 */
int32_t Irq_UnregisterButtonIrq(ELVISIII_IrqButton* irqButton,
                                NiFpga_IrqContext   irqContext,
                                uint8_t             irqNumber)
{
    NiFpga_Bool status;

    // Limit the IRQ number within a range,
    // if the entered value is out of range, print an error message.
    if (irqNumber > IRQNO_MAX || irqNumber < IRQNO_MIN)
    {
        printf("The specified IRQ Number is out of range.\n");
        return NiELVISIIIv10_Status_IrqNumberNotUsable;
    }

    // Check if the specified IRQ resource is registered.
    status = Irq_CheckReserved(irqButton->btnChannel, irqNumber);
    if (status == NiELVISIIIv10_Status_Success)
    {
        // Did not find the resource in the list.
        printf("You didn't register an interrupt with this IRQ number.\n");
        return NiELVISIIIv10_Status_Success;
    }

    // Write the new value of the button Configuration Register to the device.
    status = NiFpga_WriteBool(NiELVISIIIv10_session, irqButton->btnIrqEnable, NiFpga_False);

    // Check if there was an error when you reserved an IRQ.
    // If there was an error, print an error message to stdout and return the Configuration status.
    NiELVISIIIv10_ReturnStatusIfNotSuccess(status, "Could not write to the Button Configuration Register!");

    // Delete the reserved resource in the list.
    status = Irq_RemoveReserved(irqNumber);

    // Check if there was an error when you reserved an IRQ.
    // If there was an error, print an error message to stdout and return the configuration status.
    NiELVISIIIv10_ReturnStatusIfNotSuccess(status, "Could not release the IRQ resource!");

    // Unreserve an IRQ context obtained from Irq_ReserveIrqContext.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_UnreserveIrqContext(NiELVISIIIv10_session, irqContext);

    // Check if there was an error when you reserved an IRQ.
    // If there was an error, print an error message to stdout and return the configuration status.
    NiELVISIIIv10_ReturnStatusIfNotSuccess(status, "A required NiFpga_IrqContext was not unreserved.");

    return NiELVISIIIv10_Status_Success;
}
