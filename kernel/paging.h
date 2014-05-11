/*
 * paging.h
 *
 *  Created on: 01.07.2012
 *      Author: pascal
 */

#ifndef PAGING_H_
#define PAGING_H_

#include "stdint.h"

/*Pagingstrukturen
 *PML4 = Page-Map-Level-4 Table
 *PML4E = Page-Map-Level-4 Entry
 *PDP = Page-Directory-Pointer Table
 *PDPE = Page-Directory-Pointer Entry
 *PD = Page-Directory Table
 *PDE = Page-Directory Entry
 *PT = Page Table
 *PTE = Page Table Entry
 */

#define MAP				10		//Anzahl der zu mappenden MBs. Mehrfaches von 2MB, d.h. 10*2MB werden gemappt; Max 1GB
#define PAGE_ENTRIES	512

typedef struct{
		uint64_t PML4E[PAGE_ENTRIES];
}PML4_t;

typedef struct{
		uint64_t PDPE[PAGE_ENTRIES];
}PDP_t;

typedef struct{
		uint64_t PDE[PAGE_ENTRIES];
}PD_t;

typedef struct{
		uint64_t PTE[PAGE_ENTRIES];
}PT_t;

void setPML4Entry(uint16_t i, PML4_t *PML4, uint8_t Present, uint8_t RW, uint8_t US, uint8_t PWT,
		uint8_t PCD, uint8_t A, uint16_t AVL, uint8_t NX, uint32_t Address);
void setPDPEntry(uint16_t i, PDP_t *PDP, uint8_t Present, uint8_t RW, uint8_t US, uint8_t PWT,
		uint8_t PCD, uint8_t A, uint16_t AVL, uint8_t NX, uint32_t Address);
void setPDEntry(uint16_t i, PD_t *PD, uint8_t Present, uint8_t RW, uint8_t US, uint8_t PWT,
		uint8_t PCD, uint8_t A, uint16_t AVL, uint8_t NX, uint32_t Address);
void setPTEntry(uint16_t i, PT_t *PT, uint8_t Present, uint8_t RW, uint8_t US, uint8_t PWT,
		uint8_t PCD, uint8_t A, uint8_t D, uint8_t G, uint16_t AVL,
		uint8_t PAT, uint8_t NX, uint32_t Address);

#endif /* PAGING_H_ */
