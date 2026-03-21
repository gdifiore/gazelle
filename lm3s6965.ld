/*******************************************************************************
 * File: lm3s6965.ld
 *
 * Description:
 *   Linker script for the LM3S6965EVB (ARM Cortex-M3).
 *   256 KB flash at 0x00000000, 64 KB SRAM at 0x20000000.
 *
 * Author: Gabe DiFiore
 ******************************************************************************/

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 256K
    SRAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

/* Stack grows down from top of SRAM */
_estack = ORIGIN(SRAM) + LENGTH(SRAM);

SECTIONS
{
    /* Vector table and code in flash */
    .text :
    {
        KEEP(*(.vectors))
        *(.text*)
        *(.rodata*)
    } > FLASH

    /* .data: initialised variables — stored in flash, copied to SRAM at boot */
    .data :
    {
        _sdata = .;
        *(.data*)
        _edata = .;
    } > SRAM AT > FLASH

    /* Load address of .data in flash (used by Reset_Handler) */
    _sidata = LOADADDR(.data);

    /* .bss: zero-initialised variables in SRAM */
    .bss :
    {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > SRAM
}
