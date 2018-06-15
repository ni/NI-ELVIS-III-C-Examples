/**
 * DIIRQ.h
 *
 * Copyright (c) 2018,
 * National Instruments.
 * All rights reserved.
 */

#ifndef DIIRQ_h_
#define DIIRQ_h_

#include "IRQConfigure.h"

//DIIRQ channel selection number
#define DIIRQ_NUM 4

#if NiFpga_Cpp
extern "C" {
#endif

// Flags that indicate whether the DI IRQ pin is triggered by the rising or falling edge.
typedef enum
{
    Irq_Dio_RisingEdge,      //*< IRQ is triggered by the rising edge 
    Irq_Dio_FallingEdge,     //*< IRQ is triggered by the falling edge 
    Irq_Dio_Edge,            //*< IRQ is triggered by both the rising and falling edge 
} Irq_Dio_Type;

// Flags that enable DI IRQ.
typedef enum
{
    Irq_Dio_A0_Enable = 0x01,          //*< The DI0 IRQ enable option is set 
    Irq_Dio_A1_Enable = 0x02,          //*< The DI1 IRQ enable option is set 
    Irq_Dio_A2_Enable = 0x04,          //*< The DI2 IRQ enable option is set 
    Irq_Dio_A3_Enable = 0x08,          //*< The DI3 IRQ enable option is set 
} IrqDI_EnableSettings;

// Flags that set trigger types of DI IRQ
typedef enum
{
    Irq_Dio_A0_RisingEdge  = 0x0001,          //*< DI0 is triggered by the rising edge 
    Irq_Dio_A0_FallingEdge = 0x0100,          //*< DI0 is triggered by the falling edge 
    Irq_Dio_A0_Edge        = 0x0101,          //*< DI0 is triggered by both the rising and falling edge 

    Irq_Dio_A1_RisingEdge  = 0x0002,          //*< DI1 is triggered by the rising edge 
    Irq_Dio_A1_FallingEdge = 0x0200,          //*< DI1 is triggered by the falling edge 
    Irq_Dio_A1_Edge        = 0x0202,          //*< DI1 is triggered by both the rising and falling edge 

    Irq_Dio_A2_RisingEdge  = 0x0004,          //*< DI2 is triggered by the rising edge 
    Irq_Dio_A2_FallingEdge = 0x0400,          //*< DI2 is triggered by the falling edge 
    Irq_Dio_A2_Edge        = 0x0404,          //*< DI2 is triggered by both the rising and falling edge 

    Irq_Dio_A3_RisingEdge  = 0x0008,          //*< DI3 is triggered by the rising edge 
    Irq_Dio_A3_FallingEdge = 0x0800,          //*< DI3 is triggered by the falling edge 
    Irq_Dio_A3_Edge        = 0x0808,          //*< DI3 is triggered by both the rising and falling edge 
} IrqDi_TriggerSettings;

// Registers and settings for a particular digital IRQ I/O.
// Explanation of each registers is written below.
typedef struct
{
    uint32_t dioCount[DIIRQ_NUM];        //*< DI IRQ count register 
    uint32_t dioIrqNumber[DIIRQ_NUM];    //*< DI IRQ number register 
    uint32_t dioIrqEnable;               //*< DI IRQ enable register 
    uint32_t dioIrqRisingEdge;           //*< DI IRQ rising edge-triggered register 
    uint32_t dioIrqFallingEdge;          //*< DI IRQ falling edge-triggered register 

    Irq_Channel dioChannel;              //*< DI IRQ supported I/O 
} ELVISIII_IrqDi;

// Configure the DI IRQ.
int32_t Irq_RegisterDiIrq(ELVISIII_IrqDi*    connector,
                          NiFpga_IrqContext* irqContext,
                          uint8_t            irqNumber,
                          uint32_t           count,
                          Irq_Dio_Type       type);

// Clear the DI IRQ configuration setting.
int32_t Irq_UnregisterDiIrq(ELVISIII_IrqDi*   connector,
                            NiFpga_IrqContext irqContext,
                            uint8_t           irqNumber);

#if NiFpga_Cpp
}
#endif

#endif // DIIRQ_h_ 
