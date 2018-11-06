/**
 * Example for performing SPI
 *
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */

#include <stdio.h>
#include <unistd.h>

/**
 * Include the ELVIS III header file.
 * The target type must be defined in your project, as a stand-alone #define,
 * or when calling the compiler from the command-line.
 */
#include "NiELVISIIIv10.h"
#include "SPI.h"

/*
 * Declare the ELVIS III NiFpga_Session so that it can be used by any function in
 * this file. The variable is actually defined in NiELVISIIIv10.c.
 *
 * This removes the need to pass the NiELVISIIIv10_session around to every function and
 * only has to be declared when it is being used.
 */
extern NiFpga_Session NiELVISIIIv10_session;

// Initialize the register addresses of SPI in bank A.
ELVISIII_Spi spi_bank_A = {SPIACNFG, SPIACNT, SPIAGO, SPIASTAT, SPIADATO, SPIADATI, SYSSELECTA};

// Initialize the register addresses of SPI in bank B.
ELVISIII_Spi spi_bank_B = {SPIBCNFG, SPIBCNT, SPIBGO, SPIBSTAT, SPIBDATO, SPIBDATI, SYSSELECTB};

// Resources for the new thread.
typedef struct
{
    NiFpga_IrqContext irqContext;      // IRQ context reserved by Irq_ReserveContext()
    NiFpga_Bool       irqThreadRdy;    // IRQ thread ready flag
    uint8_t           irqNumber;       // IRQ number value
} ThreadResource;

/**
 * Sets options for the SPI configuration register.
 *
 * @param[in]  bank         A struct containing the registers on the SPI channel to modify.
 * @param[in]  mask         Array of flags that indicate which of the configure settings are valid.
 * @param[in]  settings     Array of flags that indicate the configuration settings.
 */
void Spi_Configure(ELVISIII_Spi* bank, Spi_ConfigureMask mask, Spi_ConfigureSettings settings)
{
    NiFpga_Status status;
    uint16_t cnfgValue;

    // Get the current value of the Configuration Register.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_ReadU16(NiELVISIIIv10_session, bank->cnfg, &cnfgValue);

    // Check if there was an error reading from the SPI registers.
    // If there was an error then the rest of the function cannot complete
    // correctly so print an error message to stdout and return from the function early.
    NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not read from the SPI Configuration Register!");

    // Clear the value of the masked bits in the Configuration Register. This is
    // done so that the correct value can be set later on.
    cnfgValue = cnfgValue & ~mask;

    // Set the value of the settings bits in the Configuration Register. If the
    // value to set is 0 this operation would not work unless the bit was
    // previously cleared.
    cnfgValue = cnfgValue | settings;

    // Write the new value of the Configuration Register to the device.
    status = NiFpga_WriteU16(NiELVISIIIv10_session, bank->cnfg, cnfgValue);

    // Check if there was an error writing to SPI Configuration Register.
    // If there was an error then print an error message to stdout.
    NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the SPI Configuration Register!");

    return;
}

/**
 * Sets the maximum counter value for the SPI. This, along with the clock
 * divider value set by Spi_Configure sets the speed of the SPI transmission.
 *
 * SPI frequency  = f_SPI / divisor /  2 * (maximum counter + 1)
 *
 * where f_SPI = 80 MHz.
 *
 * @param[in]  bank          Only bank_A or bank_B can be used unless you know the addresses of certain registers.
 * @param[in]  counterMax    The maximum counter value.
 */
void Spi_CounterMaximum(ELVISIII_Spi* bank, uint16_t counterMax)
{
    NiFpga_Status status;

    // Write the value to the SPI Counter Register.
    status = NiFpga_WriteU16(NiELVISIIIv10_session, bank->cnt, counterMax);

    // Check if there was an error writing to SPI Counter Register.
    // If there was an error then print an error message to stdout.
    NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the SPI Counter Register!");

    return;
}

/**
 * Transmits data.
 *
 * @param[in]  bank        A struct containing the registers on the SPI channel to modify.
 * @param[in]  dataOut     Data to output.
 * @param[in]  dataIn      Data to input.
 */
uint16_t Spi_Transmit(ELVISIII_Spi* bank, uint16_t dataOut)
{
    NiFpga_Status status;

    // Set the value of the data to output.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_WriteU16(NiELVISIIIv10_session, bank->dato, dataOut);

    // Check if there was an error writing to the SPI Execute Register.
    // If there was an error then the rest of the function cannot complete
    // correctly so print an error message to stdout and return from the
    // function early.
    NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the SPI Execute Register!");

    // Start the data transmission.
    // NiFpga_MergeStatus is used to propagate any errors from previous
    // function calls. Errors are not anticipated so error checking is not done
    // after every NiFpga function call but only at specific points.
    NiFpga_MergeStatus(&status, NiFpga_WriteBool(NiELVISIIIv10_session, bank->go, NiFpga_True));

    ThreadResource irqThread;
    if(bank[0].cnfg == spi_bank_A.cnfg)
    {
        irqThread.irqNumber = 27;
    } 
    else
    {
        irqThread.irqNumber = 26;
    }
    irqThread.irqThreadRdy = NiFpga_True;
    status = NiFpga_ReserveIrqContext(NiELVISIIIv10_session, &irqThread.irqContext);
    NiELVISIIIv10_ReturnIfNotSuccess(status, "A required NiFpga_IrqContext was not reserved.");
    uint32_t irqAssert = 0;
    status = NiFpga_WaitOnIrqs(NiELVISIIIv10_session,
                               irqThread.irqContext,
                               1 << irqThread.irqNumber,
                               1000,  			/* break into infinite 100ms timeouts */
                               &irqAssert,
                               NULL);
	// If an IRQ was asserted.
    if (irqAssert & (1 << irqThread.irqNumber))
    {
        Irq_Acknowledge(irqAssert);
    }

    // Get the value of the data Input Register.
    // The returned NiFpga_Status value is stored for error checking.
    uint16_t readChar;
    status = NiFpga_ReadU16(NiELVISIIIv10_session, bank->dati, &readChar);

    // Check if there was an error reading from the SPI registers.
    // If there was an error then print an error message to stdout.
    NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not read from the SPI Data In Register!");

    return readChar;
}

/**
 * Write the value to the System Select Register.
 *
 * @param[in]  bank      A struct containing the registers on the SPI channel to modify.
 */
void Spi_Select(ELVISIII_Spi* bank)
{
    NiFpga_Status status;
    uint64_t selectReg;

    // SPI connections are on pins shared with other onboard devices. To use
    // on a physical pin, select the SPI on the appropriate SELECT register.
    // Read the value of the SYSSELECTA register.
    status = NiFpga_ReadU64(NiELVISIIIv10_session, bank->sel, &selectReg);

    NiELVISIIIv10_ReturnValueIfNotSuccess(status, status, "Could not read from the System Select Register!");

    // No need to clear the value of the bits in the SYSSELECTA/SYSSELECTB register
    // because SPI select is all 1.
    // Set bit10:15 of the SYSSELECTA/SYSSELECTB register to enable SPI functionality.
    // The functionality of the bit is specified in the documentation.
    selectReg = selectReg | (uint64_t)0b111111 << 10;

    // Write the updated value of the SYSSELECTA register.
    status = NiFpga_WriteU64(NiELVISIIIv10_session, bank->sel, selectReg);

    // Check if there was an error reading from the System Select Register.
    // If there was an error then print an error message to stdout.
    NiELVISIIIv10_ReturnValueIfNotSuccess(status, status, "Could not write to the System Select Register!");

    return;
}
