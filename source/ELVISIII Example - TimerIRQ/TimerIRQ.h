/**
 * TimerIRQ.h
 *
 * Copyright (c) 2018,
 * National Instruments.
 * All rights reserved.
 */

#ifndef TimerIRQ_h_
#define TimerIRQ_h_

#include "IRQConfigure.h"

#if !defined(TIMERIRQNO)
#define TIMERIRQNO  0
#endif

#if NiFpga_Cpp
extern "C" {
#endif

// Registers and settings for timer IRQ I/O.
// Explanation of each Registers is written below.
typedef struct
{
    uint32_t timerRead;         // Timer IRQ Read Register 
    uint32_t timerWrite;        // Timer IRQ Write Register 
    uint32_t timerSet;          // Timer IRQ Set Time Register 

    Irq_Channel timerChannel;   // Timer IRQ supported I/O 
} ELVISIII_IrqTimer;

// Configure the timer IRQ.
int32_t Irq_RegisterTimerIrq(ELVISIII_IrqTimer* irqChannel,
                             NiFpga_IrqContext* irqContext,
                             uint32_t           timeout);

// Clear the timer IRQ setting.
int32_t Irq_UnregisterTimerIrq(ELVISIII_IrqTimer* irqChannel,
                               NiFpga_IrqContext  irqContext);

#if NiFpga_Cpp
}
#endif

#endif // TimerIRQ_h_ 
