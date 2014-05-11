/*
 * multiboot.h
 *
 *  Created on: 25.06.2012
 *      Author: pascal
 */

#ifndef MULTIBOOT_H_
#define MULTIBOOT_H_

#include "stdint.h"

typedef struct
{
		int32_t	mod_start;	//Startaddresse des Moduls
		int32_t	mod_end;	//Endaddresse des Moduls
		int32_t	name;		//Name des Moduls (0 terminierter ASCII-String)
		int32_t	reserved;	//Reserviert (0)
}mods_addr;

typedef struct
{													//Gültig wenn mbs_flags[x] gesetzt
		unsigned int	mbs_flags;					//immer
		unsigned int	mbs_mem_lower;				//0
		unsigned int	mbs_mem_upper;				//0
		unsigned int	mbs_bootdevice;				//1
		unsigned int	mbs_cmdline;				//2
		unsigned int	mbs_mods_count;				//3
		mods_addr*	mbs_mods_addr;					//3
		unsigned long long int	mbs_syms;			//4 oder 5
		unsigned int	mbs_mmap_length;			//6
		unsigned int	mbs_mmap_addr;				//6
		unsigned int	mbs_drives_length;			//7
		unsigned int	mbs_drives_addr;			//7
		unsigned int	mbs_config_table;			//8
		unsigned int	mbs_boot_loader_name;		//9
		unsigned int	mbs_apm_table;				//10
		unsigned int	mbs_vbe_control_info;		//11
		unsigned int	mbs_vbe_mode_info;			//11
		unsigned short int	mbs_vbe_mode;			//11
		unsigned short int	mbs_vbe_interface_seg;	//11
		unsigned short int	mbs_vbe_interface_off;	//11
		unsigned short int	mbs_vbe_interface_len;	//11
}multiboot_structure;

#endif /* MULTIBOOT_H_ */
