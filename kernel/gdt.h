/*
 * gdt.h
 *
 *  Created on: 25.06.2012
 *      Author: pascal
 */

#ifndef GDT_H_
#define GDT_H_

#define GDT_ENTRIES	4

#include "stdint.h"

struct _gdtr
{
		uint16_t	limit;
		void 		*pointer;
}__attribute__((packed)) gdtr;

void setGDTEntry(int i, unsigned int base, unsigned int limit, char access, char flags);
//void GDTInit(uint32_t base);
void GDTInit();

#endif /* GDT_H_ */
