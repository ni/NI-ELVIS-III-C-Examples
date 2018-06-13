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
 * Specify the numbers of DIO channels.
 *
 * Dio_Channel0: 0
 * Dio_Channel1: 1
 * ...
 * Dio_Channel20: 19
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
 * Registers and settings for a particular DIO N Sample.
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
void Di_Direction(ELVISIII_Dio* connector, Dio_Channel channel);


/**
 * Generate the divisor for the DI sample rate.
 */
void Di_Divisor(ELVISIII_Dio* connector, uint32_t ClockRate, uint32_t SampleRate);


/**
 * Set the DI DMA Enable Flag for one connector.
 */
void Di_Enable(ELVISIII_Dio* connector);


/**
 * Read groups of DI values as a DI FIFO from a single channel.
 */
void Di_ReadFifo(ELVISIII_Dio*         connector,
		         TargetToHost_FIFO_FXP fifo,
			 	 uint64_t*             fxp_buffer_receive,
			 	 size_t                fifo_size,
			 	 uint32_t              timeout,
			 	 size_t*               elementsRemaining);


/**
 * Convert fixed-point values of the FIFO to boolean values.
 */
void ConvertUnsignedLongLongIntToBool(Dio_Channel channel, uint64_t* fxp_buffer_receive, size_t fifo_size, NiFpga_Bool value[]);


/**
 * Set the Direction of the DIO channel as an input.
 */
void Do_Direction(ELVISIII_Dio* connector, Dio_Channel channel);


/**
 * Generate the divisor for the DO sample rate.
 */
void Do_Divisor(ELVISIII_Dio* connector, uint32_t ClockRate, uint32_t SampleRate);


/**
 * Set the DMA Enable value for an DO channel.
 */
void Do_Enable(ELVISIII_Dio* connector, Dio_Channel channel);


/**
 * Write groups of DO values as a DO FIFO to a single channel.
 */
void Do_WriteFifo(ELVISIII_Dio* 		connector,
		          HostToTarget_FIFO_FXP fifo,
			 	  const uint64_t*       fxp_buffer_send,
			 	  size_t                fifo_size,
			 	  uint32_t              timeout,
			 	  size_t*               elementsRemaining);


#if NiFpga_Cpp
}
#endif

#endif /* DIO_h_ */
