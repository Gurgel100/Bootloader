/*
 * elf.c
 *
 *  Created on: 25.06.2012
 *      Author: pascal
 */
#include "elf.h"
#include "bootloader.h"

char elfCheck(elf_header *ELFHeader)
{
	//zuerst überprüfen wir auf den Magic-String
	if(ELFHeader->e_ident.ei_magic != ELF_MAGIC) return 2;
	//jetzt überprüfen wir auf den Dateityp
	if(ELFHeader->e_type != ELF_ET_EXEC) return 3;
	//jetzt überprüfen wir die Architektur (sollte x86 sein)
	if(ELFHeader->e_machine != ELF_EM_X86_64) return 4;
	//jetzt überprüfen wir die Klasse (wir suchen eine 64-Bit-Datei)
	if(ELFHeader->e_ident.ei_class != ELF_CLASS_64) return 5;
	//Daten
	if(ELFHeader->e_ident.ei_data != ELF_DATA_2LSB) return 6;
	//und jetzt noch die Version
	if(ELFHeader->e_version != ELF_EV_CURRENT) return 7;
	//alles OK
	return -1;
}

uint32_t getElfContent(elf_header *ElfHeader)	//gibt die Adresse zurück
{
	elf_program_header_entry *ProgramHeader =
			(elf_program_header_entry*)(uint32_t)((char*)ElfHeader) + ElfHeader->e_phoff;
	return (uint32_t)(((char*)ElfHeader) + ProgramHeader->p_offset);
}

uint32_t getElfAddress(elf_header *ElfHeader)	//gibt die virtuelle Adresse zurück
{
	elf_program_header_entry *ProgramHeader =
			(elf_program_header_entry*)(uint32_t)((char*)ElfHeader + ElfHeader->e_phoff);
	return (uint32_t)ProgramHeader->p_vaddr;
}

uint32_t getElfEntryAddress(elf_header *ElfHeader)	//gibt die Einsprungsadresse zurück
{
	return (uint32_t)ElfHeader->e_entry;
}

uint32_t getElfLength(elf_header *ElfHeader)	//gibt die Länge des ersten Segments aus
{
	elf_program_header_entry *ProgramHeader =
			(elf_program_header_entry*)(uint32_t)((char*)ElfHeader + ElfHeader->e_phoff);
	return (uint32_t)ProgramHeader->p_filesz;
}

char elfLade(void *Datei, uint16_t Segment)
{
	elf_header *Header = Datei;
	char *Speicher;
	char *Ziel;

	elf_program_header_entry *ProgramHeader = ((void*)Header) + Header->e_phoff;

	register int i;
	for(i = 0; i < Header->e_phnum; i++)
	{
		//Wenn kein ladbares Segment, dann Springe zum nächsten Segment
		if(ProgramHeader[i].p_type != ELF_PT_LOAD) continue;

		Ziel = (char*)(uintptr_t)ProgramHeader[i].p_vaddr;								//Zieladdresse (virtuell)
		Speicher = (char*)(Datei + ((uintptr_t)ProgramHeader[i].p_offset));	//Position der Datei im Speicher

		//Segment in den Speicher laden und eventuell mit nullen auffüllen
		/*asm volatile(
				"push %%ds;"
				"push %%es;"

				"mov $0x10,%%ax;"
				"mov %%ax,%%ds;"

				"mov %2,%%ax;"
				"mov %%ax,%%es;"
				//for-Schleife
				"xor %%eax,%%eax;"
				"mov %3,%%ecx;"
				"dec %%ecx;"
				".2:"
				"lodsb;"	//Lade das was an Addresse ds:esi steht nach al
				"stosb;"	//Speichere den Inhalt von al nach es:edi
				"loop .2;"

				//Müssen noch nullen angehängt werden?
				"mov %4,%%eax;"
				"cmp %3,%%eax;"
				"jnb .4;"
				"sub %3,%%eax;"
				"mov %%eax,%%ecx;"
				"xor %%eax,%%eax;"
				".3:"
				"stosb;"
				"loop .3;"

				".4:"
				"pop %%es;"
				"pop %%ds;"
				: :"S" (Speicher), "D" (Ziel), "m" (Segment), "m" (ProgramHeader[i].p_filesz), "m" (ProgramHeader[i].p_memsz) :"%ecx", "%eax"
		);	*/		//0				1			2				3								4
		elf64_xword filesz = ProgramHeader[i].p_filesz;
		elf64_xword memsz = ProgramHeader[i].p_memsz;
		register uint64_t j;
		for(j = 0; j < filesz; j++)
		{
			Ziel[j] = Speicher[j];
		}

		//Wenn nötig mit Nullen auffüllen
		for(j = filesz; j < memsz; j++)
			Ziel[j] = 0;
	}
	return 0;
}
