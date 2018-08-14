/**
 * DIO.h
 *
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */

#ifndef DIO_h_
#define DIO_h_

#include <stdbool.h>
#include "NiELVISIIIv10.h"

#if NiFpga_Cpp
extern "C" {
#endif

/**
 * Specify the numbers of DIO channels.
 *
 * Dio_Channel0: 0
 * Dio_Channel1: 1
 * ...
 * Dio_Channel19: 19
 */
typedef enum
{
    Dio_Channel0  = 0,
    Dio_Channel1  = 1,
    Dio_Channel2  = 2,
    Dio_Channel3  = 3,
    Dio_Channel4  = 4,
    Dio_Channel5  = 5,
    Dio_Channel6  = 6,
    Dio_Channel7  = 7,
    Dio_Channel8  = 8,
    Dio_Channel9  = 9,
    Dio_Channel10 = 10,
    Dio_Channel11 = 11,
    Dio_Channel12 = 12,
    Dio_Channel13 = 13,
    Dio_Channel14 = 14,
    Dio_Channel15 = 15,
    Dio_Channel16 = 16,
    Dio_Channel17 = 17,
    Dio_Channel18 = 18,
    Dio_Channel19 = 19,
} Dio_Channel;

/**
 * Registers and settings for a particular DIO.
 * DI and DO share the same structure.
 * Explanation of each Registers is written below.
 */
typedef struct
{
    uint32_t dir;         // DIO Direction Register 
    uint32_t in;          // DI Value Register 
    uint32_t out;         // DO Value Register
    uint32_t sel;         // System Select Register
} ELVISIII_Dio;

// Read the value from one channel.
NiFpga_Bool Dio_ReadBit(ELVISIII_Dio* bank, Dio_Channel channel);

// Write the value into one channel.
void Dio_WriteBit(ELVISIII_Dio* bank, NiFpga_Bool value, Dio_Channel channel);

#if NiFpga_Cpp
}
#endif

#endif // DIO_h_ 
