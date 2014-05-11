/*
 * elf.h
 *
 *  Created on: 25.06.2012
 *      Author: pascal
 *
 *  Nach dem Tutorial "http://www.lowlevel.eu/wiki/ELF-Tutorial"
 */

#ifndef ELF_H_
#define ELF_H_

#include "stdint.h"

typedef uint64_t	elf64_addr;
typedef uint16_t 	elf64_half;
typedef uint64_t	elf64_off;
typedef int32_t		elf64_sword;
typedef uint32_t	elf64_word;
typedef uint64_t	elf64_xword;
typedef int64_t		elf64_sxword;
typedef uint8_t		elf64_uchar;


#define ELF_MAG0	0x7F	//Datei-Identifikation
#define ELF_MAG1	'E'
#define ELF_MAG2	'L'
#define ELF_MAG3	'F'
#define	ELF_MAGIC	(ELF_MAG0 | ((ELF_MAG1 & 0xFFLL) << 8) | ((ELF_MAG2 & 0xFFLL) << 16) | ((ELF_MAG3 & 0xFFLL) << 24))

#define	ELF_CLASS_NONE	0x00	//Datei-Klasse
#define	ELF_CLASS_32	0x01	//32-Bit Datei
#define	ELF_CLASS_64	0x02	//64-Bit Datei

#define	ELF_DATA_NONE	0x00	//Prozessorspezifische Datenkodierung
#define	ELF_DATA_2LSB	0x01	//Little Endian
#define	ELF_DATA_2MSB	0x02	//Big Endian

typedef struct
{
		uint32_t	ei_magic;		// ELF-Magic Number
		uint8_t		ei_class;		// 32 oder 64 bit?
		uint8_t		ei_data;		// Little oder Big Endian?
		uint8_t		ei_version;		// dasselbe wie ELF_HEADER.e_version
		uint8_t		ei_osabi;		//OS/ABI Identifikation
		uint8_t		ei_abiversion;	//ABI Version
		uint8_t		ei_pad;			// reserved (zero)
		uint16_t	ei_pad2;		// reserved (zero)
		uint32_t	ei_pad3;		//reserviert (NULL)
		//uint8_t		ei_nident;		// ?
}elf_ident_header;


#define	ELF_ET_NONE		0x0000		// kein Typ
#define	ELF_ET_REL		0x0001		// relocatable
#define	ELF_ET_EXEC		0x0002		// ausführbar
#define	ELF_ET_DYN		0x0003		// Shared-Object-File
#define	ELF_ET_CORE		0x0004		// Corefile
#define	ELF_ET_LOPROC	0xFF00		// Processor-specific
#define	ELF_ET_HIPROC	0x00FF		// Processor-specific

#define	ELF_EM_NONE		0x0000		// kein Typ
#define	ELF_EM_M32		0x0001		// AT&T WE 32100
#define	ELF_EM_SPARC	0x0002		// SPARC
#define	ELF_EM_386		0x0003		// Intel 80386
#define	ELF_EM_68K		0x0004		// Motorola 68000
#define	ELF_EM_88K		0x0005		// Motorola 88000
#define	ELF_EM_860		0x0007		// Intel 80860
#define	ELF_EM_MIPS		0x0008		// MIPS RS3000
#define	ELF_EM_X86_64	0x003e		// AMD X86-64

#define	ELF_EV_NONE		0x00000000	// ungültige Version
#define	ELF_EV_CURRENT	0x00000001	// aktuelle Version

typedef struct
{
		elf_ident_header	e_ident;	// IDENT-HEADER (siehe oben)
		elf64_half			e_type; 	// Typ der ELF-Datei
		elf64_half			e_machine;	// Prozessortyp
		elf64_word			e_version;	// ELF-Version
		elf64_addr			e_entry;	// Virtuelle addresse des Einstiegspunkt des Codes
		elf64_off			e_phoff;	// Offset des Programm-Headers (ist 0 wenn nicht vorhanden)
		elf64_off			e_shoff;	// Offset des Section-Headers (ist 0 wenn nicht vorhanden)
		elf64_word			e_flags;	// processor-specific flags
		elf64_half			e_ehsize;	// Grösse des ELF-header
		elf64_half			e_phentsize;// Grösse eines Programm-Header Eintrags
		elf64_half			e_phnum;	// Anzahl Einträge im Programm-Header (ist 0 wenn nicht vorhanden)
		elf64_half			e_shentsize;// size of one section-header entry
		elf64_half			e_shnum;	// Anzahl Einträge im Section-Header (ist 0 wenn nicht vorhanden)
		elf64_half			e_shstrndex;// tells us which entry of the section-header is linked to the String-Table
}elf_header;


#define	ELF_PT_NULL		0x00000000	// ungültiges Segment
#define	ELF_PT_LOAD		0x00000001	// ladbares Segment
#define	ELF_PT_DYNAMIC	0x00000002	// dynamisch linkbares Segment
#define	ELF_PT_INTERP	0x00000003	// position of a zero-terminated string, which tells the interpreter
#define	ELF_PT_NOTE		0x00000004	// Universelles Segment
#define	ELF_PT_SHLIB	0x00000005	// shared lib (reserviert)
#define	ELF_PT_PHDIR	0x00000006	// gibt Position und Grösse des Programm-Headers an
#define	ELF_PT_LOPROC	0x70000000	// reserved
#define	ELF_PT_HIPROC	0x7FFFFFFF	// reserved

#define	ELF_PF_X		0x00000001;	// ausführbares Segment
#define	ELF_PF_W		0x00000002;	// schreibbares Segment
#define	ELF_PF_R		0x00000004;	// lesbares Segment

typedef struct
{
		elf64_word	p_type;			// Typ des Segments (siehe oben)
		elf64_word	p_flags;		// Flags
		elf64_off	p_offset;		// Dateioffset des Segments
		elf64_addr	p_vaddr;		// Virtuelle Addresse, an die das Segment kopiert werden soll
		elf64_addr	p_paddr;		// Physikalische Addresse (meist irrelevant)
		elf64_xword	p_filesz;		// Grösse des Segments in der Datei
		elf64_xword	p_memsz;		// Grösse des Segments, die es im Speicher haben soll
		elf64_xword	p_align;		// Alignment. if zero or one, then no alignment is needed, otherwise the alignment has to be a power of two
}elf_program_header_entry;


//Funktionen
char elfCheck(elf_header *ELFHeader);	//Par.: Zeiger auf ELF-Header
uintptr_t getElfContent(elf_header *ElfHeader);
uint32_t getElfAddress(elf_header *ElfHeader);
uint32_t getElfEntryAddress(elf_header *ElfHeader);
uint32_t getElfLength(elf_header *ElfHeader);
char elfLade(void *Datei, uint16_t Segment);	//Par.: Datei = Addresse der Datei im Speicher; Segment = Segment in das kopiert werden soll (GDT)

#endif /* ELF_H_ */
