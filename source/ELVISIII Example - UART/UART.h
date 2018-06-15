/**
 * UART.h
 *
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */

#ifndef UART_h_
#define UART_h_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "visa.h"
#include "NiELVISIIIv10.h"

// UART parity configurations.
typedef enum
{
    Uart_ParityNone  = 0,
    Uart_ParityOdd   = 1,
    Uart_ParityEven  = 2,
    Uart_ParityMark  = 3,
    Uart_ParitySpace = 4
} Uart_Parity;

// UART stop bit configurations.
typedef enum
{
    Uart_StopBits1_0 = 10,    // 1.0 stop bits 
    Uart_StopBits1_5 = 15,    // 1.5 stop bits 
    Uart_StopBits2_0 = 20     // 2.0 stop bits 
} Uart_StopBits;

// Port information for a particular UART.
typedef struct
{
    const char* name;         // Resource name on UART port
    ViSession   defaultRM;    // Default resource manager session
    ViSession   session;      // ViSession reference
} ELVISIII_Uart;

// Registers and settings for a particular UART.
// Explanation of each Registers is written below.
typedef struct
{
    uint32_t enable;         // UART Enable Register 
    uint32_t stat;           // UART Status Register 
    uint32_t console;        // UART Console Enable Register 
} ELVISIII_Connector;

#ifdef __cplusplus
extern "C" {
#endif

// Set the UART Enable Flag for one connector.
void Uart_Enable(ELVISIII_Connector* connector);

// Opens a UART session.
int32_t Uart_Open(ELVISIII_Uart*      port,
                  const uint32_t      baud,
                  const uint8_t       dataBits,
                  const Uart_StopBits stopBits,
                  const Uart_Parity   parity);

// Closes a UART session.
int32_t Uart_Close(ELVISIII_Uart* port);

// Reads binary data from a UART port. If the number of bytes in the UART
// receive FIFO is less than number bytes to read, this function blocks until either
// all the bytes are available to be removed from the FIFO, or until a timeoutoccurs.
int32_t Uart_Read(ELVISIII_Uart* port,
                  uint8_t* const data,
                  const size_t   nData);

// Writes data to a UART transmit FIFO. If not enough space is available in the
// FIFO this function blocks until sufficient space is available or a timeout
// occurs.
int32_t Uart_Write(ELVISIII_Uart* port,
                   const uint8_t* const data,
                   const size_t   nData);

// Clears UART receive buffer.
int32_t Uart_Clear(ELVISIII_Uart* port);

#ifdef __cplusplus
}
#endif

#endif // UART_h_ 
