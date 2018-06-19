/**
 * Example of using UART.
 *
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */

#include "UART.h"
#include "visa.h"

#define VisaDefaultTimeout 1000

/*
 * Declare the ELVIS III NiFpga_Session so that it can be used by any function in
 * this file. The variable is actually defined in NiELVISIIIv10.c.
 *
 * This removes the need to pass the NiELVISIIIv10_session around to every function and
 * only has to be declared when it is being used.
 */
extern NiFpga_Session NiELVISIIIv10_session;

// Initialize the register addresses of UART in bank A.
ELVISIII_Bank bank_A = {UARTAENA, UARTASTAT, CONSOLEENA};

// Initialize the register addresses of UART in bank B.
ELVISIII_Bank bank_B = {UARTBENA, UARTBSTAT, CONSOLEENA};

/**
 *  Set the UART Enable Flag for one bank.
 *
 * @param[in]  bank      A struct containing the registers for one connecter.
 */
void Uart_Enable(ELVISIII_Bank* bank)
{
    NiFpga_Status status;
    NiFpga_Bool Enable;
    bool flag = true;

    // Read the value from the Console Enable Register.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_ReadBool(NiELVISIIIv10_session, bank->console, &Enable);

    // Check if there was an error reading from the Console Enable Register.
    // If there was an error then print an error message to stdout.
    NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not read from the Console Enable Register!");

    if (Enable == NiFpga_True)
    {
        // Write the value to the Console Enable Register.
        // The returned NiFpga_Status value is stored for error checking.
        status = NiFpga_WriteBool(NiELVISIIIv10_session, bank->console, NiFpga_False);

        // Check if there was an error writing to the Console Enable Register.
        // If there was an error then print an error message to stdout.
        NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the Console Enable Register!");
    }

    // Write the value to the UART Enable Register.
    // The returned NiFpga_Status value is stored for error checking.
    status = NiFpga_WriteBool(NiELVISIIIv10_session, bank->enable, NiFpga_True);

    // Check if there was an error writing to the UART Enable Register.
    // If there was an error then print an error message to stdout.
    NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not write to the UART Enable Register!");

    while(flag)
    {
        // Read the value from the UART Enable Register.
        NiFpga_ReadBool(NiELVISIIIv10_session, bank->enable, &Enable);

        // Check if there was an error reading from the UART Enable Register.
        // If there was an error then print an error message to stdout.
        NiELVISIIIv10_ReturnIfNotSuccess(status, "Could not read from the UART Enable Register!");

        if (Enable == NiFpga_True)
        {
            flag = false;
        }
    }

    return;
}

/**
 * Opens a UART session on an VISA implemented port.
 *
 * @param[in]  port            UART port information.
 * @param[in]  baud            Baud rate (bps).
 * @param[in]  dataBits        Number of bits per frame.
 * @param[in]  stopBits        Stop bit configuration.
 * @param[in]  parity          Parity configuration.
 *
 * @return     int32_t         Error/success status
 */
int32_t Uart_Open(ELVISIII_Uart*      port,
                  const uint32_t      baud,
                  const uint8_t       dataBits,
                  const Uart_StopBits stopBits,
                  const Uart_Parity   parity)
{
    int32_t status = VI_SUCCESS;

    // Open VISA resource manager.
    if (!port->defaultRM)
    {
        status = viOpenDefaultRM(&port->defaultRM);
        if (status < VI_SUCCESS || !port->defaultRM)
        {
            return status;
        }
    }

    // Open serial resource.
    if (!port->session)
    {
        status = viOpen(port->defaultRM, (ViRsrc)port->name, VI_NULL, VI_NULL, &port->session);
        if (status < VI_SUCCESS || !port->session)
        {
            return status;
        }
    }

    // Set timeout.
    status = viSetAttribute(port->session, VI_ATTR_TMO_VALUE, VisaDefaultTimeout);
    if (status < VI_SUCCESS)
    {
        return status;
    }

    // Set baud rate.
    status = viSetAttribute(port->session, VI_ATTR_ASRL_BAUD, baud);
    if (status < VI_SUCCESS)
    {
        return status;
    }

    // Set data bits.
    status = viSetAttribute(port->session, VI_ATTR_ASRL_DATA_BITS, dataBits);
    if (status < VI_SUCCESS)
    {
        return status;
    }

    // Set stop bits.
    status = viSetAttribute(port->session, VI_ATTR_ASRL_STOP_BITS, stopBits);
    if (status < VI_SUCCESS)
    {
        return status;
    }

    // Set parity.
    status = viSetAttribute(port->session, VI_ATTR_ASRL_PARITY, parity);
    if (status < VI_SUCCESS)
    {
        return status;
    }

    // Set termination character.
    status = viSetAttribute(port->session, VI_ATTR_TERMCHAR_EN, VI_FALSE);
    if (status < VI_SUCCESS)
    {
        return status;
    }

    return status;
}

/**
 * Closes a UART session on an FPGA implemented port.
 *
 * @param[in]  port    Port to close
 *
 * @return     int32_t Error/success status
 */
int32_t Uart_Close(ELVISIII_Uart* port)
{
    int32_t status = VI_SUCCESS;

    if (port->session)
    {
        status = viClose(port->session);
        port->session = 0;
    }
    if (status < VI_SUCCESS)
    {
        return status;
    }

    if (port->defaultRM)
    {
        status = viClose(port->defaultRM);
        port->defaultRM = 0;
    }

    return status;
}

/**
 * Reads binary data from a UART port. If the number of bytes in the UART
 * receive FIFO is less than number bytes to read, this function blocks until either
 * all the bytes are available to be removed from the FIFO, or until a timeout
 * occurs.
 *
 * @param[in]   port    Port to access
 * @param[out]  data    Buffer to receive read data
 * @param[in]   nData   Number of bytes to read
 *
 * @return      int32_t Error/success status
 */
int32_t Uart_Read(ELVISIII_Uart* port, uint8_t* const data, const size_t nData)
{
    int32_t status = VI_SUCCESS;
    ViUInt32 numberRead = 0;

    status = viRead(port->session, (ViBuf)data, (ViUInt32)nData, &numberRead);

    return status;
}

/**
 * Writes data to a UART transmit FIFO. If not enough space is available in the
 * FIFO, this function blocks until sufficient space is available or a timeout
 * occurs.
 *
 * @param[in]  port        UART port to access
 * @param[in]  data        Array of data to write
 * @param[in]  nData       Size of data array
 *
 * @return     int32_t     Error/success status
 */
int32_t Uart_Write(ELVISIII_Uart* port, const uint8_t* const data, const size_t nData)
{
    int32_t status = VI_SUCCESS;
    ViUInt32 numberWritten = 0;

    status = viWrite(port->session, (ViBuf)data, (ViUInt32)nData, &numberWritten);

    return status;
}

/**
 * Clears UART receive buffer.
 *
 * @param[in]  port        UART port to access
 *
 * @return     int32_t  Error/success status
 */
int32_t Uart_Clear(ELVISIII_Uart* port)
{
    int32_t status = VI_SUCCESS;
    int32_t oldTimeout = 0;
    uint8_t read[0xFF] = {0};
    ViUInt32 nRead = 0;

    // Get timeout.
    status = viGetAttribute(port->session, VI_ATTR_TMO_VALUE, &oldTimeout);
    if (status < VI_SUCCESS)
    {
        return status;
    }

    // Set timeout to 1 ms.
    status = viSetAttribute(port->session, VI_ATTR_TMO_VALUE, 1);
    if (status < VI_SUCCESS)
    {
        return status;
    }

    // Read input buffer, up to 255 bytes at a time, until timeout occurs.
    while((status = viRead(port->session, (ViBuf)read, (ViUInt32)0xFF, &nRead)) >= VI_SUCCESS);

    if (status == VI_ERROR_TMO)
    {
        // Timeout expected.
        status = VI_SUCCESS;
    }

    // Reset timeout to previous value.
    status = viSetAttribute(port->session, VI_ATTR_TMO_VALUE, oldTimeout);
    if (status < VI_SUCCESS)
    {
        return status;
    }

    return status;
}
