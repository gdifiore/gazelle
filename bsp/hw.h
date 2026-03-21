/*******************************************************************************
 * File: hw.h
 *
 * Description:
 *   Hardware register definitions for the LM3S6965EVB (Cortex-M3).
 *   Covers SysTick, NVIC, SCB, SYSCTL, GPIO Port A, and UART0.
 *
 * Author: Gabe DiFiore
 ******************************************************************************/

#ifndef HW_H_
#define HW_H_

#include <stdint.h>

/*---------------------------------------------------------------------------
 * System clock (LM3S6965: 12 MHz internal oscillator as seen by QEMU)
 *---------------------------------------------------------------------------*/
#define F_CPU 12000000UL

/*---------------------------------------------------------------------------
 * SysTick  (ARM Cortex-M3 core peripheral — base 0xE000E010)
 *---------------------------------------------------------------------------*/
#define SYST_CSR (*(volatile uint32_t *)0xE000E010) /* Control & Status    */
#define SYST_RVR (*(volatile uint32_t *)0xE000E014) /* Reload Value        */
#define SYST_CVR (*(volatile uint32_t *)0xE000E018) /* Current Value       */

#define SYST_CSR_ENABLE    (1u << 0) /* Counter enable                      */
#define SYST_CSR_TICKINT   (1u << 1) /* SysTick exception request enable    */
#define SYST_CSR_CLKSOURCE (1u << 2) /* 1 = processor clock                 */

/*---------------------------------------------------------------------------
 * NVIC  (base 0xE000E100)
 *---------------------------------------------------------------------------*/
#define NVIC_EN0 (*(volatile uint32_t *)0xE000E100) /* IRQ 0-31 enable     */

/*---------------------------------------------------------------------------
 * SCB — System Control Block
 *---------------------------------------------------------------------------*/
#define SCB_ICSR          (*(volatile uint32_t *)0xE000ED04)
#define SCB_ICSR_PENDSVSET (1u << 28)

/*---------------------------------------------------------------------------
 * SYSCTL — System Control  (base 0x400FE000)
 *---------------------------------------------------------------------------*/
#define SYSCTL_RCGC1 (*(volatile uint32_t *)0x400FE104) /* Run-Mode Clock Gating 1 */
#define SYSCTL_RCGC2 (*(volatile uint32_t *)0x400FE108) /* Run-Mode Clock Gating 2 */

#define SYSCTL_RCGC1_UART0 (1u << 0) /* UART0 clock gate  */
#define SYSCTL_RCGC2_GPIOA (1u << 0) /* GPIO Port A clock gate */

/*---------------------------------------------------------------------------
 * GPIO Port A  (base 0x40004000) — PA0 = UART0 RX, PA1 = UART0 TX
 *---------------------------------------------------------------------------*/
#define GPIOA_AFSEL (*(volatile uint32_t *)0x40004420) /* Alternate function select */
#define GPIOA_DEN   (*(volatile uint32_t *)0x4000451C) /* Digital enable            */

/*---------------------------------------------------------------------------
 * UART0  (base 0x4000C000)
 *---------------------------------------------------------------------------*/
#define UART0_DR   (*(volatile uint32_t *)0x4000C000) /* Data register            */
#define UART0_FR   (*(volatile uint32_t *)0x4000C018) /* Flag register            */
#define UART0_IBRD (*(volatile uint32_t *)0x4000C024) /* Integer baud-rate divisor */
#define UART0_FBRD (*(volatile uint32_t *)0x4000C028) /* Fractional baud-rate div */
#define UART0_LCRH (*(volatile uint32_t *)0x4000C02C) /* Line control             */
#define UART0_CTL  (*(volatile uint32_t *)0x4000C030) /* Control                  */
#define UART0_IM   (*(volatile uint32_t *)0x4000C038) /* Interrupt mask           */

/* UART Flag Register bits */
#define UART_FR_TXFE (1u << 7) /* Transmit FIFO empty */
#define UART_FR_RXFF (1u << 6) /* Receive FIFO full   */
#define UART_FR_TXFF (1u << 5) /* Transmit FIFO full  */
#define UART_FR_RXFE (1u << 4) /* Receive FIFO empty  */
#define UART_FR_BUSY (1u << 3) /* UART busy           */

/* UART Line Control bits */
#define UART_LCRH_WLEN_8 (0x3u << 5) /* 8-bit word length */
#define UART_LCRH_FEN    (1u << 4)   /* Enable FIFOs      */

/* UART Control bits */
#define UART_CTL_UARTEN (1u << 0) /* UART enable    */
#define UART_CTL_TXE    (1u << 8) /* Transmit enable */
#define UART_CTL_RXE    (1u << 9) /* Receive enable  */

/* UART Interrupt mask bits */
#define UART_IM_RXIM (1u << 4) /* Receive interrupt mask */

/* UART0 is IRQ5 */
#define UART0_IRQ_BIT (1u << 5)

#endif /* HW_H_ */
