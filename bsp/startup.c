/*******************************************************************************
 * File: startup.c
 *
 * Description:
 *   Cortex-M3 startup code for the LM3S6965EVB.
 *   Defines the interrupt vector table and Reset_Handler, which copies
 *   .data from flash to SRAM and zeroes .bss before calling main().
 *
 * Author: Gabe DiFiore
 ******************************************************************************/

#include <stdint.h>

/* Symbols provided by the linker script */
extern uint32_t _sidata; /* Load address of .data in flash */
extern uint32_t _sdata;  /* Start of .data in SRAM         */
extern uint32_t _edata;  /* End of .data in SRAM           */
extern uint32_t _sbss;   /* Start of .bss                  */
extern uint32_t _ebss;   /* End of .bss                    */
extern uint32_t _estack; /* Top of stack                   */

int main(void);

void Default_Handler(void);

/* Weak aliases — drivers override by defining the real handler */
void NMI_Handler(void)       __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)       __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)    __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void UART0_Handler(void)     __attribute__((weak, alias("Default_Handler")));

void Reset_Handler(void)
{
    /* Copy .data from flash to SRAM */
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata)
        *dst++ = *src++;

    /* Zero .bss */
    dst = &_sbss;
    while (dst < &_ebss)
        *dst++ = 0;

    main();

    /* Should never reach here */
    while (1)
        ;
}

void Default_Handler(void)
{
    while (1)
        ;
}

/* Vector table — placed at the very start of flash by the linker script */
typedef void (*irq_handler_t)(void);

__attribute__((section(".vectors"), used))
const irq_handler_t vector_table[] = {
    (irq_handler_t)&_estack, /*  0: Initial stack pointer */
    Reset_Handler,            /*  1: Reset                 */
    NMI_Handler,              /*  2: NMI                   */
    HardFault_Handler,        /*  3: Hard Fault            */
    Default_Handler,          /*  4: MemManage             */
    Default_Handler,          /*  5: BusFault              */
    Default_Handler,          /*  6: UsageFault            */
    0,                        /*  7: Reserved              */
    0,                        /*  8: Reserved              */
    0,                        /*  9: Reserved              */
    0,                        /* 10: Reserved              */
    SVC_Handler,              /* 11: SVCall                */
    Default_Handler,          /* 12: DebugMon              */
    0,                        /* 13: Reserved              */
    PendSV_Handler,           /* 14: PendSV                */
    SysTick_Handler,          /* 15: SysTick               */
    /* Peripheral IRQs (LM3S6965) */
    Default_Handler,          /* IRQ0: GPIO Port A         */
    Default_Handler,          /* IRQ1: GPIO Port B         */
    Default_Handler,          /* IRQ2: GPIO Port C         */
    Default_Handler,          /* IRQ3: GPIO Port D         */
    Default_Handler,          /* IRQ4: GPIO Port E         */
    UART0_Handler,            /* IRQ5: UART0               */
};
