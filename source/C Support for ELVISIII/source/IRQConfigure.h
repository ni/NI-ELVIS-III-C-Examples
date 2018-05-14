/**
 * IRQConfigure.h
 *
 * Copyright (c) 2018,
 * National Instruments Corporation.
 * All rights reserved.
 */

#ifndef IRQConfigure_h_
#define IRQConfigure_h_

#include "NiELVISIIIv10.h"
#include <malloc.h>

/**
 * No errors or warnings.
 */
static const int32_t NiELVISIIIv10_Status_Success = 0;

/**
 * The specified channel is already registered with another interrupt.
 * Specify a different channel or unregister the interrupt with the same channel name.
 */
static const int32_t NiELVISIIIv10_Status_IrqChannelNotUsable = -363024;

/**
 * The specified IRQ Number is out of range or
 * already registered an interrupt with the same interrupt number.
 */
static const int32_t NiELVISIIIv10_Status_IrqNumberNotUsable = -363025;

#if NiFpga_Cpp
extern "C" {
#endif

/*
 * Flags that indicate which of the IRQ IOs are supported.
 */
typedef enum
{
    Irq_Ai_A0    = 0,     /* AI Interrupt on AI0 from Connector A */
    Irq_Ai_A1    = 1,     /* AI Interrupt on AI1 from Connector A */

    Irq_Dio_A0   = 2,     /* DI Interrupt on DI0 from Connector A */
    Irq_Dio_A1   = 3,     /* DI Interrupt on DI1 from Connector A */
    Irq_Dio_A2   = 4,     /* DI Interrupt on DI2 from Connector A */
    Irq_Dio_A3   = 5,     /* DI Interrupt on DI3 from Connector A */

    Irq_Button_0 = 6,     /* Button Interrupt */

    Irq_Timer_0  = 7,     /* Timer Interrupt*/
} Irq_Channel;

/**
 * Linker structure to store the reserved IRQ resources.
 */
typedef struct ReservedIrq
{
    Irq_Channel         channel;            /* Channel supported by IRQ */
    uint8_t             number;             /* IRQ number */
    struct ReservedIrq* pNext;              /* Pointer to next node */
} ReservedIrq, *ReservedIrqPointer;

/**
 * Check whether the channel and IRQ number are already reserved.
 */
int32_t Irq_CheckReserved(Irq_Channel channel, uint8_t irqNumber);

/**
  * Add a new node into the reserved IRQ resource list.
 */
int32_t Irq_AddReserved(Irq_Channel channel, uint8_t irqNumber);

/**
 * Remove the node in the resource list.
 */
int32_t Irq_RemoveReserved(uint8_t irqNumber);

/**
 * Wait until the specified IRQ number occurred or ready signal arrived.
 */
void Irq_Wait(NiFpga_IrqContext irqContext,
		      NiFpga_Irq        irqNumber,
		      uint32_t*         irqAssert,
		      NiFpga_Bool*      continueWaiting);

/**
 * Acknowledge the IRQ(s).
 */
void Irq_Acknowledge(uint32_t irqAssert);

#if NiFpga_Cpp
}
#endif

#endif /* IRQConfigure_h_ */
