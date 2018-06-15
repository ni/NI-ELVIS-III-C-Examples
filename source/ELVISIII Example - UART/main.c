/**
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */
/**
 * Overview:
 * Demonstrates using the UART. Writes a character to the UART bus, prints any
 * returned character to the console.
 *
 * Instructions:
 * 1. Connect UART.TX of the UART to DIO16 on connector A.
 * 2. Connect UART.RX of the UART to DIO17 on connector A.
 * 3. Connect UART.GND of the UART to DGND on connector A.
 * 4. Connect a DC voltage input (+3.3 V) to the UART.
 * 5. Run the program.
 *
 * Output:
 * The program writes the character "H" to the console.
 * Returned character is written to the console if the attached UART device writes to the bus.
 *
 * Note:
 * The Eclipse project defines the preprocessor symbol for the ELVIS III.
 */
#include <stdio.h>
#include <time.h>
#include "UART.h"

#define BaudRate 9600
#define DataBit 8

#if !defined(LoopDuration)
#define LoopDuration    60  // How long to output the signal, in seconds 
#endif

extern ELVISIII_Connector connector_A;

int main(int argc, char **argv)
{
    ELVISIII_Uart uart;

    uint8_t writeData = 'H';
    uint8_t readData = 0;
    int32_t status = 0;

    time_t currentTime;
    time_t finalTime;

    printf("UART:\n");

    // Initialize the UART port structure.
    uart.name = "ASRL1::INSTR";
    uart.defaultRM = 0;
    uart.session = 0;

    // Open the ELVIS III NiFpga Session.
    // This function MUST be called before all other functions. After this call
    // is complete the ELVIS III target will be ready to be used.
    status = NiELVISIIIv10_Open();
    if (NiELVISIIIv10_IsNotSuccess(status))
    {
        return status;
    }

    // Set the UART Enable Flag for one connector.
    Uart_Enable(&connector_A);

    // Opens a UART session.
    status = Uart_Open(&uart, BaudRate, DataBit, Uart_StopBits1_0, Uart_ParityNone);
    if (status < VI_SUCCESS)
    {
        return status;
    }

    // Writes data to a UART port.
    status = Uart_Write(&uart, &writeData, 1);
    if (status < VI_SUCCESS)
    {
        return status;
    }

    // Reads data from a UART port.
    status = Uart_Read(&uart, &readData, 1);
    if (status < VI_SUCCESS)
    {
        return status;
    }

    printf("The received data is %d\n", readData);

    // Clears UART receive buffer.
    status = Uart_Clear(&uart);
    if (status < VI_SUCCESS)
    {
        return status;
    }

    // Closes the UART port.
    status = Uart_Close(&uart);
    if (status < VI_SUCCESS)
    {
        return status;
    }

    // Normally, the main function runs a long running or infinite loop.
    // Keep the program running so that you can measure the output using
    // an external instrument.
    time(&currentTime);
    finalTime = currentTime + LoopDuration;
    while (currentTime < finalTime)
    {
        time(&currentTime);
    }

    // Close the ELVISIII NiFpga Session.
    // This function MUST be called after all other functions.
    status = NiELVISIIIv10_Close();

    return status;
}
