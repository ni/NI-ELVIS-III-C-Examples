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
 * Specify which DIO channel.
 *
 * Di_Channel0: 0
 * Di_Channel1: 1
 * ...
 * Di_Channel20: 19
 */
typedef enum
{
	Di_Channel0  = 0,
	Di_Channel1  = 1,
	Di_Channel2  = 2,
	Di_Channel3  = 3,
	Di_Channel4  = 4,
	Di_Channel5  = 5,
	Di_Channel6  = 6,
	Di_Channel7  = 7,
	Di_Channel8  = 8,
	Di_Channel9  = 9,
	Di_Channel10 = 10,
	Di_Channel11 = 11,
	Di_Channel12 = 12,
	Di_Channel13 = 13,
	Di_Channel14 = 14,
	Di_Channel15 = 15,
	Di_Channel16 = 16,
	Di_Channel17 = 17,
	Di_Channel18 = 18,
	Di_Channel19 = 19,
} Di_Channel;

/**
 * Registers and settings for a particular DIO.
 * DI and DO share the same structure.
 * Explanation of each Registers is written below.
 */
typedef struct
{
	uint32_t dir;         /**< DIO Direction Register */
	uint32_t in;          /**< DI Value Register */
	uint32_t out;         /**< DO Value Register */
} ELVISIII_Dio;

/**
 * Read the value from one channel.
 */
NiFpga_Bool Dio_ReadBit(ELVISIII_Dio* connector, Di_Channel channel);

/**
 * Write the value into one channel.
 */
void Dio_WriteBit(ELVISIII_Dio* connector, NiFpga_Bool value, Di_Channel channel);

#if NiFpga_Cpp
}
#endif

#endif /* DIO_h_ */
