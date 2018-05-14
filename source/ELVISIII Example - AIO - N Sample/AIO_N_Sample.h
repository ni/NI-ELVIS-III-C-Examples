/**
 * AIO_N_Sample.h
 *
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */

#ifndef AIO_N_Sample_h_
#define AIO_N_Sample_h_

#include <stdlib.h>
#include <math.h>
#include "NiELVISIIIv10.h"

//AI channel selection number (RSE mode)
#define RSE_NUM 8

//AI channel selection number (DIFF mode)
#define DIFF_NUM 4

//AI_Fxp
#define AI_WordLength 24
#define AI_IntegerWordLength 5

//AO_Fxp
#define AO_WordLength 20
#define AO_IntegerWordLength 5

//Maximum and minimum sample rate.
#define MAX_SAMPLE_RATE 8000000
#define MIN_SAMPLE_RATE 1000

#if NiFpga_Cpp
extern "C" {
#endif

//AO FIFO
typedef enum
{
	HostToTarget_FIFO_FXP_B = 4,
	HostToTarget_FIFO_FXP_A = 5,
} HostToTarget_FIFO_FXP;


//AI FIFO
typedef enum
{
	TargetToHost_FIFO_FXP_B = 6,
	TargetToHost_FIFO_FXP_A = 7,
} TargetToHost_FIFO_FXP;


/**
 * Specify which AI channel to configure.
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
 * Specify which AO channel to configure.
 */
typedef enum
{
	Ao_Channel0 = 0b01,
	Ao_Channel1 = 0b10,
}Ao_Channel;


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
 * Registers and settings for a particular AIO N Sample.
 * Explanation of each Registers is written below.
 */
typedef struct
{
	uint32_t cnfg;                          /**< AI Configuration Register */

	uint32_t ai_cntr;                       /**< AI Divisor Register */
	uint32_t ao_cntr;                       /**< AO Divisor Register */

	uint32_t cnt;                           /**< AI Counter Register */

	uint32_t ai_enable;                     /**< AI DMA Enable Register */
	uint32_t ao_enable;                     /**< AO DMA Enable Register */
} ELVISIII_Aio;


/**
 * Set the AI Counter Register.
 */
void Ai_Counter(ELVISIII_Aio* connector, uint8_t counter);


/**
 * Set the AI Configuration Register.
 */
void Ai_Configure(ELVISIII_Aio* connector, Ai_Channel channel, Ai_Range range);


/**
 * Set the AI Divisor Register.
 */
void Ai_Divisor(ELVISIII_Aio* connector, uint32_t ClockRate, uint32_t SampleRate);


/**
 * Set the AI DMA Enable Register.
 */
void Ai_Enable(ELVISIII_Aio* connector);


/**
 * Read groups of AI values from a AI FIFO.
 */
void Ai_ReadFifo(ELVISIII_Aio*         connector,
		         TargetToHost_FIFO_FXP fifo,
			 	 uint64_t*             fxp_buffer_recv,
			 	 size_t                fifo_size,
			 	 uint32_t              timeout,
			 	 size_t*               elementsRemaining);


/**
 * Get groups of values from one channel.
 */
void Ai_GetVal(uint64_t *fxp_buffer_recv, size_t fifo_size, double *val);


/**
 * Set the AO Divisor Register.
 */
void Ao_Divisor(ELVISIII_Aio* connector, uint32_t ClockRate, uint32_t SampleRate);


/**
 * Set the AO DMA Enable Register.
 */
void Ao_Enable(ELVISIII_Aio* connector, Ao_Channel channel);


/**
 * Turn groups of values from double to Fix Point.
 */
void Ao_SetVal(double *val, uint64_t *fxp_buffer_send, size_t fifo_size);


/**
 * Write groups of AO values to a AO FIFO.
 */
void Ao_WriteFifo(ELVISIII_Aio*         connector,
		          HostToTarget_FIFO_FXP fifo,
			 	  const uint64_t*       fxp_buffer_recv,
			 	  size_t                fifo_size,
			 	  uint32_t              timeout,
			 	  size_t*               elementsRemaining);


#if NiFpga_Cpp
}
#endif

#endif /* AIO_N_Sample_h_ */
