/*************************************************************************************
 * Memory layout for STM32 linking
 * 
 * Defines where to access the flash and sram for linking purposes. Here's the overall
 * layout (from https://github.com/trebisky/stm32f103/blob/master/blink1/blink.lds).
 *
 * 0x00000000 - 0x07ffffff - aliased to flash or sys memory depending on BOOT jumpers.
 * 0x08000000 - 0x0800ffff - Flash.
 * 0x1ffff000 - 0x1ffff7ff - Boot firmware in system memory.
 * 0x1ffff800 - 0x1fffffff - Option bytes.
 * 0x20000000 - 0x20004fff - SRAM.
 * 0x40000000 - 0x40023400 - Peripherals
 */
 
MEMORY {
   flash(RX)  : ORIGIN = 0x00000000, LENGTH = 64K
   sram(WAIL) : ORIGIN = 0x20000000, LENGTH = 20K
}

SECTIONS {
   .text : { *(.text*) } > flash
   .bss  : { *(.bss*) } > sram
}
