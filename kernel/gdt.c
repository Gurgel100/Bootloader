/*
 * gdt.c
 *
 *  Created on: 25.06.2012
 *      Author: pascal
 */

#include "gdt.h"
#include "elf.h"

static uint64_t gdt[GDT_ENTRIES];

void setGDTEntry(int i, uint32_t base, uint32_t limit, char access, char flags)
{
	gdt[i] = limit & 0xFFFF;
	gdt[i] |= (base & 0xFFFFFFLL) << 16;
	gdt[i] |= (access & 0xFFLL) << 40;
	gdt[i] |= ((limit >> 16) & 0xFLL) << 48;
	gdt[i] |= (flags & 0xFLL) << 52;
	gdt[i] |= ((base >> 24) & 0xFFLL) << 56;
}

void GDTInit()
{
	setGDTEntry(0, 0, 0, 0, 0);				//NULL-Deskriptor
	setGDTEntry(1, 0, 0xFFFFF, 0x9A, 0xC);	//Codesegment, ausführ- und lesbar, 32-bit
	setGDTEntry(2, 0, 0xFFFFF, 0x92, 0xC);	//Datensegment, les- und schreibbar
	setGDTEntry(3, 0, 0xFFFFF, 0x9A, 0xA);	//Codesegment, ausführ- und lesbar, 64-bit

	gdtr.limit = GDT_ENTRIES *8 -1;
	gdtr.pointer = gdt;
	asm volatile("lgdt %0": :"m"(gdtr));
	asm volatile(
			"mov $0x10,%ax;"
			"mov %ax,%ds;"
			"mov %ax,%es;"
			"mov %ax,%ss;"
			"mov %ax,%fs;"
			"mov %ax,%gs;"
			"ljmp $0x8,$.1;"
			".1:"
	);
}
