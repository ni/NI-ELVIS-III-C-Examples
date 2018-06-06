/**
 * AIO.h
 *
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */

#ifndef AIO_h_
#define AIO_h_

#include <stdlib.h>
#include <math.h>

#include "NiELVISIIIv10.h"

//AI channel selection number (RSE mode)
#define RSE_NUM 8

//AI channel selection number (DIFF mode)
#define DIFF_NUM 4

//AI Fixed-point
#define AI_WordLength 26
#define AI_IntegerWordLength 5

//AO Fixed-point
#define AO_WordLength 20
#define AO_IntegerWordLength 5

//Maximum and minimum sample rate.
#define MAX_SAMPLE_RATE 8000000
#define MIN_SAMPLE_RATE 1000

#if NiFpga_Cpp
extern "C" {
#endif

/**
 * Specify the numbers of AI channels.
 * The analog input channels have RSE mode and DIFF mode.
 * RSE mode corresponding to one particular analog input channel.
 * DIFF mode corresponding to the difference of two analog input channels.
 *
 * RSE (the fourth bit is 1):
 * Ai_Channel 0 : 1 000b
 * Ai_Channel 1 : 1 001b
 * ...
 * Ai_Channel 7 : 1 111b
 *
 * DIFF (the fourth bit is 0):
 * Ai_Channel 0, 4: 0 000b
 * Ai_Channel 1, 5: 0 001b
 * Ai_Channel 2, 6: 0 010b
 * Ai_Channel 3, 7: 0 011b
 */
typedef enum
{
	//RSE mode
	Ai_Channel0 = 0x08,
	Ai_Channel1 = 0x09,
	Ai_Channel2 = 0x0a,
	Ai_Channel3 = 0x0b,
	Ai_Channel4 = 0x0c,
	Ai_Channel5 = 0x0d,
	Ai_Channel6 = 0x0e,
	Ai_Channel7 = 0x0f,

	//DIFF mode
	Ai_Channel0_4 = 0x00,
	Ai_Channel1_5 = 0x01,
	Ai_Channel2_6 = 0x02,
	Ai_Channel3_7 = 0x03,
} Ai_Channel;

/**
 * Specify the voltage range of the AI channel.
 *
 * ±10 V: 00b
 * ±5 V: 01b
 * ±2 V: 10b
 * ±1 V: 11b
 */
typedef enum
{
	Ai_Range0 = 0x00,
	Ai_Range1 = 0x10,
	Ai_Range2 = 0x20,
	Ai_Range3 = 0x30,
} Ai_Range;

/**
 * Specify the AO Value Register addresses.
 */
typedef enum
{
	AO_A0_VAL = 99540,
	AO_A1_VAL = 99536,
	AO_B0_VAL = 99548,
	AO_B1_VAL = 99544,
} Ao_ValueRegister;

/**
 * Registers for a particular analog input.
 */
typedef struct
{
	uint32_t cnfg;                          /**< AI Configuration Register */
	uint32_t cntr;                          /**< AI Divisor Register */
	uint32_t cnt;                           /**< AI Counter Register */
	uint32_t rdy;                           /**< AI Ready Register */
	uint32_t val[RSE_NUM + DIFF_NUM];       /**< AIO Value Register */
} ELVISIII_Ai;

/**
 * Registers for a particular analog output.
 */
typedef struct
{
	uint32_t go;        /**< AO Start Register */
	uint32_t stat;      /**< AO Status Register */
} ELVISIII_Ao;


/**
 * Convert unsigned int value of a fixed-point to double value.
 */
double ConvertUnsignedIntToDouble(unsigned int value);


/**
 * Convert double value to unsigned int value to represent a fixed-point.
 */
unsigned int ConvertDoubleToUnsignedInt(double value);


/**
 * Set the number of valid analog input channels.
 */
void Ai_Counter(ELVISIII_Ai* connector, uint8_t counter);


/**
 * Set the AI configuration options.
 */
void Ai_Configure(ELVISIII_Ai* connector, Ai_Channel channel, Ai_Range range);


/**
 * Generate the divisor for sample rate.
 */
void Ai_Divisor(ELVISIII_Ai* connector, uint32_t ClockRate, uint32_t SampleRate);


/**
 * Read the value from one channel.
 */
double Aio_Read(ELVISIII_Ai* connector, Ai_Channel channel);

/**
 * Write the value to one AO Value Register.
 */
void Aio_Write(ELVISIII_Ao* Ao, double value, Ao_ValueRegister ValueRegister);


#if NiFpga_Cpp
}
#endif

#endif /* AIO_h_ */
