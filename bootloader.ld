OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)
/*  Bei _start soll die Ausfuehrung losgehen */
ENTRY(_start)

/*
 * Hier wird festgelegt, in welcher Reihenfolge welche Sektionen in die Binary
 * geschrieben werden sollen
 */
SECTIONS
{
    /*
     * . ist die aktuelle Position in der Datei. Wir wollen den Kernel wie gehabt
     * an 10 MB laden, also muessen wir dort die erste Sektion hinlegen
     */
    . = 0xA00000;
    bootloader_start = .;

    /*
     * Der Multiboot-Header muss zuerst kommen (in den ersten 8 kB).
     * Die Standardsektionen einfach hintereinander weg einbinden.
     */
    .text : {
        *(multiboot)
        *(.text)
    }
    .data ALIGN(4096) : {
        *(.data)
    }
    .rodata ALIGN(4096) : {
        *(.rodata)
    }
    .bss ALIGN(4096) : {
        *(.bss)
    }
    bootloader_end = .;
}

