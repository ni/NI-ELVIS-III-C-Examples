/**
 * DIO_N_Sample.h
 *
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */

#ifndef DIO_N_Sample_h_
#define DIO_N_Sample_h_

#include <stdbool.h>
#include <math.h>
#include "NiELVISIIIv10.h"

//Maximum and minimum sample rate.
#define MAX_SAMPLE_RATE 8000000
#define MIN_SAMPLE_RATE 1000


#if NiFpga_Cpp
extern "C" {
#endif

//DO FIFO
typedef enum
{
	HostToTarget_FIFO_FXP_B = 0,
	HostToTarget_FIFO_FXP_A = 1,
} HostToTarget_FIFO_FXP;


//DI FIFO
typedef enum
{
	TargetToHost_FIFO_FXP_B = 2,
	TargetToHost_FIFO_FXP_A = 3,
} TargetToHost_FIFO_FXP;


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
	uint32_t di_enable;   /**< DI DMA Enable Register */
	uint32_t do_enable;   /**< DO DMA Enable Register */

	uint32_t di_cntr;     /**< DI Divisor Register */
	uint32_t do_cntr;     /**< DO Divisor Register */

	uint32_t dir;         /**< DIO Direction Register */
} ELVISIII_Dio;


/**
 * Set the DIO Direction Register.
 */
void Di_Direction(ELVISIII_Dio* connector, Di_Channel channel);


/**
 * Set the DI Divisor Register.
 */
void Di_Divisor(ELVISIII_Dio* connector, uint32_t ClockRate, uint32_t SampleRate);


/**
 * Configure the DI DMA Enable Register.
 */
void Di_Enable(ELVISIII_Dio* connector);


/**
 * Read FXP from a DI FIFO.
 */
void Di_ReadFifo(ELVISIII_Dio*         connector,
		         TargetToHost_FIFO_FXP fifo,
			 	 uint64_t*             fxp_buffer_recv,
			 	 size_t                fifo_size,
			 	 uint32_t              timeout,
			 	 size_t*               elementsRemaining);


/**
 * Get groups of values from one channel.
 */
void Di_GetVal(Di_Channel channel, uint64_t* fxp_buffer_recv, size_t fifo_size, NiFpga_Bool val[]);


/**
 * Set the DIO Direction Register.
 */
void Do_Direction(ELVISIII_Dio* connector, Di_Channel channel);


/**
 * Set the DI Divisor Register.
 */
void Do_Divisor(ELVISIII_Dio* connector, uint32_t ClockRate, uint32_t SampleRate);


/**
 * Configure the DO DMA Enable Register.
 */
void Do_Enable(ELVISIII_Dio* connector, Di_Channel channel);


/**
 * Write FXP to a DO FIFO.
 */
void Do_WriteFifo(ELVISIII_Dio* 		connector,
		          HostToTarget_FIFO_FXP fifo,
			 	  const uint64_t*       fxp_buffer_recv,
			 	  size_t                fifo_size,
			 	  uint32_t              timeout,
			 	  size_t*               elementsRemaining);


#if NiFpga_Cpp
}
#endif

#endif /* DIO_h_ */
