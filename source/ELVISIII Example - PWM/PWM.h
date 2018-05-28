/**
 * PWM.h
 *
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */

#ifndef PWM_h_
#define PWM_h_

#include "NiELVISIIIv10.h"

//PWM channel selection number
#define PWM_NUM 20

#if NiFpga_Cpp
extern "C" {
#endif

/**
 * Specify the numbers of PWM channels.
 *
 * Pwm_Channel0: 0
 * Pwm_Channel1: 1
 * ...
 * Pwm_Channel19: 19
 */
typedef enum
{
	Pwm_Channel0  = 0,
	Pwm_Channel1  = 1,
	Pwm_Channel2  = 2,
	Pwm_Channel3  = 3,
	Pwm_Channel4  = 4,
	Pwm_Channel5  = 5,
	Pwm_Channel6  = 6,
	Pwm_Channel7  = 7,
	Pwm_Channel8  = 8,
	Pwm_Channel9  = 9,
	Pwm_Channel10 = 10,
	Pwm_Channel11 = 11,
	Pwm_Channel12 = 12,
	Pwm_Channel13 = 13,
	Pwm_Channel14 = 14,
	Pwm_Channel15 = 15,
	Pwm_Channel16 = 16,
	Pwm_Channel17 = 17,
	Pwm_Channel18 = 18,
	Pwm_Channel19 = 19,
} Pwm_Channel;


/**
 * Flags that indicate which of the PWM configure functions are set.
 */
typedef enum
{
    Pwm_Invert = 0x01,  /**< The waveform inversion option is set */
    Pwm_Mode = 0x04     /**< The waveform mode option is set */
} Pwm_ConfigureMask;


/**
 * PWM configure register (waveform generation) options.
 */
typedef enum
{
    Pwm_NotInverted = 0x00,     /**< PWM signal is not inverted */
    Pwm_Inverted = 0x01,        /**< PWM signal is inverted */
    Pwm_Disabled = 0x00,        /**< No PWM generation */
    Pwm_Enabled = 0x04,         /**< Asymmetric PWM generation */
} Pwm_ConfigureSettings;


/**
 * Clock divider options.
 */
typedef enum
{
    Pwm_Off = 0x00,     /**< No clock */
    Pwm_1x = 0x01,      /**< 1x (f_clk) */
    Pwm_2x = 0x02,      /**< 2x (f_clk / 2) */
    Pwm_4x = 0x03,      /**< 4x (f_clk / 4) */
    Pwm_8x = 0x04,      /**< 8x (f_clk / 8) */
    Pwm_16x = 0x05,     /**< 16x (f_clk / 16) */
    Pwm_32x = 0x06,     /**< 32x (f_clk / 32) */
    Pwm_64x = 0x07      /**< 64x (f_clk / 64) */
} Pwm_ClockDivider;


/**
 * Registers and settings for a particular PWM.
 * Explanation of each Registers is written below.
 */
typedef struct
{
	uint32_t cnfg[PWM_NUM];     /**< Counter Configuration Register */
	uint32_t cs[PWM_NUM];       /**< Selects Speed of the Clock Register */
	uint32_t max[PWM_NUM];      /**< Maximum Value of the Counter Register */
	uint32_t cmp[PWM_NUM];      /**< Output Compare Value Register */
	uint32_t cntr[PWM_NUM];     /**< Current Value of the Counter Register */
	uint32_t sel;               /**< System Select Register */
} ELVISIII_Pwm;


/**
 * Sets the PWM waveform and and invert configuration options.
 */
void Pwm_Configure(ELVISIII_Pwm* connector, Pwm_Channel channel, Pwm_ConfigureMask mask, Pwm_ConfigureSettings settings);


/**
 * Sets the PWM clock select divider, that is, the frequency of the PWM internal clock.
 */
void Pwm_ClockSelect(ELVISIII_Pwm* connector, Pwm_Channel channel, Pwm_ClockDivider divider);


/**
 * Sets the maximum value of the PWM counter.
 */
void Pwm_CounterMaximum(ELVISIII_Pwm* connector, Pwm_Channel channel, uint16_t counterMax);


/**
 * Sets the period of the PWM counter with output.
 */
void Pwm_CounterCompare(ELVISIII_Pwm* connector, Pwm_Channel channel, uint16_t counterCompare);


/**
 * Gets the value of the PWM counter.
 */
uint16_t Pwm_Counter(ELVISIII_Pwm* connector, Pwm_Channel channel);


/**
 * Write the value to the System Select Register.
 */
void Pwm_Select(ELVISIII_Pwm* connector, Pwm_Channel channel);


#if NiFpga_Cpp
}
#endif

#endif /* PWM_h_ */
