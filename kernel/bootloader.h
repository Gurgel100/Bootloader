/*
 * bootloader.h
 *
 *  Created on: 24.06.2012
 *      Author: pascal
 */

#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_
#define GRAFIKSPEICHER 0xB8000

#include "multiboot.h"
#include "elf.h"
#include "stdint.h"
#include "gdt.h"
#include "paging.h"

typedef uint32_t size_t;

void boot(multiboot_structure *MultibootStruktur);
void print(char *ausgabe);
void clearDisp(void);
void scrollScreen(int Anzahl);
char *IntToStr(int32_t Zahl);
void *memcpy(void *to, const void *from, size_t size);
size_t strlen(const char *cs);

#endif /* BOOTLOADER_H_ */
