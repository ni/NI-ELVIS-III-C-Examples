/**
 * AIIRQ.h
 *
 * Copyright (c) 2018,
 * National Instruments.
 * All rights reserved.
 */

#ifndef AIIRQ_h_
#define AIIRQ_h_

#include <math.h>

#include "IRQConfigure.h"

//AI channel selection number (RSE mode)
#define RSE_NUM 8

//AI channel selection number (DIFF mode)
#define DIFF_NUM 4

//AIIRQ channel selection number
#define AIIRQ_NUM 2

//AI_FXP
#define AIIRQ_WordLength 26
#define AIIRQ_IntegerWordLength 5

//Maximum and minimum sample rate.
#define MAX_SAMPLE_RATE 8000000
#define MIN_SAMPLE_RATE 1000

#if NiFpga_Cpp
extern "C" {
#endif

/**
 * Specify which AI channel to configure.
 *
 * Ai_Channel 0 : 1 000b
 * Ai_Channel 1 : 1 001b
 */
typedef enum
{
	Ai_Channel0 = 0x08,
	Ai_Channel1 = 0x09,
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
 * Flags that indicate whether the AI IRQ pin is triggered by the rising or falling edge.
 */
typedef enum
{
    Irq_Ai_RisingEdge,            /**< IRQ is triggered by the rising edge */
    Irq_Ai_FallingEdge            /**< IRQ is triggered by the falling edge */
} Irq_Ai_Type;


/**
 * Flags that indicate and configure AI IRQ.
 */
typedef enum
{
    Irq_Ai_A0_Enable = 0x01,       /**< The AI0 IRQ enable option */
    Irq_Ai_A1_Enable = 0x04,       /**< The AI1 IRQ enable option */

    Irq_Ai_A0_Type = 0x02,         /**< The AI0 IRQ triggered type option */
    Irq_Ai_A1_Type = 0x08          /**< The AI1 IRQ triggered type option */
} Irq_Ai_Configure;


/**
 * Registers and settings for a particular AI IRQ.
 * Explanation of each registers is written below.
 */
typedef struct
{
	uint32_t cnfg;                           /**< AI Configuration Register */
	uint32_t cntr;                           /**< AI Divisor Register */
	uint32_t cnt;                            /**< AI Counter Register */
	uint32_t rdy;                            /**< AI Ready Register */
	uint32_t val[AIIRQ_NUM];        		 /**< AIO Value Register */
	
    uint32_t aiIrqNumber[AIIRQ_NUM];         /**< AI IRQ Number Register */
    uint32_t aiHysteresis[AIIRQ_NUM];        /**< AI IRQ Hysteresis Register */
    uint32_t aiThreshold[AIIRQ_NUM];         /**< AI IRQ Threshold Register */
    uint32_t aiIrqConfigure;                 /**< AI IRQ Enable and Trigger Type Configuration Register */

    Irq_Channel aiChannel;                   /**< AI IRQ supported I/O */
} ELVISIII_IrqAi;


/**
 * Convert double value to unsigned int value to represent a fixed-point.
 */
unsigned int ConvertDoubleToUnsignedInt(double value);


/**
 * Set the number of valid AI channels.
 */
void Ai_Counter(ELVISIII_IrqAi* connector, uint8_t counter);


/**
 * Set the AI configuration options.
 */
void Ai_Configure(ELVISIII_IrqAi* connector, Ai_Channel channel, Ai_Range range);


/**
 * Generate the divisor for sample rate.
 */
void Ai_Divisor(ELVISIII_IrqAi* connector, uint32_t ClockRate, uint32_t SampleRate);


/**
 * Configure the AI IRQ number, threshold and hysteresis and trigger type.
 */
int32_t Irq_RegisterAiIrq(ELVISIII_IrqAi*    connector,
                          NiFpga_IrqContext* irqContext,
                          uint8_t            irqNumber,
                          double             threshold,
                          double             hysteresis,
                          Irq_Ai_Type        type);


/**
 * Disable the IRQ configuration on the specified IRQ channel and number.
 */
int32_t Irq_UnregisterAiIrq(ELVISIII_IrqAi*    connector,
                             NiFpga_IrqContext irqContext,
                             uint8_t           irqNumber);

#if NiFpga_Cpp
}
#endif

#endif /* AIIRQ_h_ */
