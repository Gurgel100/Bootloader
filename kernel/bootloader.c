/*
 * kernel.c
 *
 *  Created on: 03.06.2012
 *      Author: pascal
 */
#include "bootloader.h"
char *Hallo = "Hallo Welt!\n\r";
uint8_t Zeile, Spalte;
static int64_t *Speicher;
void boot(multiboot_structure *MBS)
{
	clearDisp();
	print(Hallo);
	if(MBS->mbs_flags & 0b1)
	{
		print("GRUB uebergibt uns die Groesse des Speichers.\n\r");
		print("   Groesse des physikalischen Speichers ist: ");
		print(IntToStr((uint32_t)MBS->mbs_mem_upper + (uint32_t)MBS->mbs_mem_lower));
		print("kb\n\r");
	}
	if(MBS->mbs_flags & 0b1000)
	{
		if(MBS->mbs_mods_count == 1)
		{
			print("Es wurde 1 Modul geladen\n\r");
			print("Addresse des Moduls:\n\r");
			print("   ");
			print(IntToStr(MBS->mbs_mods_addr[0].mod_start));
			print("\n\r");
			print("Name des Moduls: ");
			print((char*)MBS->mbs_mods_addr[0].name);
			print("\n\r");
		}
		else if(MBS->mbs_mods_count > 1)
		{
			print("Es wurden ");
			print(IntToStr(MBS->mbs_mods_count));
			print("Module geladen.\n\r");
			print("Addressen der Module:\n\r");
			register int i;
			for(i = 0; i < MBS->mbs_mods_count; i++)
			{
				print("   ");
				print(IntToStr(MBS->mbs_mods_addr[i].mod_start));
				print("\n\r");
			}
			print("Namen der Module:\n\r");
			for(i = 0; i < MBS->mbs_mods_count; i++)
			{
				print("   ");
				print((char*)MBS->mbs_mods_addr[i].name);
				print("\n\r");
			}
		}
		else
			panic("Es wurde kein Modul geladen.\n\r");
	}
	else
		panic("GRUB hat keine Module geladen.\n\r");

	char Fehler;
	register int i;
	for(i = 0; i < MBS->mbs_mods_count; i++)
	{
		if((Fehler = elfCheck((elf_header*)MBS->mbs_mods_addr[i].mod_start)) == -1)
			break;
		else
		{
			print("Kein Kernel gefunden.\n\r");
			print("Fehlercode: ");
			print(IntToStr(Fehler));
			print("\n\r");
			print("Bootvorgang wird abgebrochen...");
			asm("cli;hlt");
		}
	}
	print("Kernel gefunden.\n\r");
	uint32_t mod = i;

	print("Initialisiere GDT...\n\r");
	//GDTInit(base);
	GDTInit();
	print("GDT initialisiert.\n\r");

	//Wichtige Daten an sicheren Ort kopieren
	static multiboot_structure new_MBS;
	static uint8_t mbsData[1024];	//1 KiB für die Daten
	memcpy(&new_MBS, MBS, sizeof(*MBS));
	size_t index = 0;
	if(MBS->mbs_flags & 0b100)
	{
		new_MBS.mbs_cmdline = memcpy(mbsData, MBS->mbs_cmdline, strlen(MBS->mbs_cmdline) + 1);
		index += strlen(MBS->mbs_cmdline) + 1;
	}
	if(MBS->mbs_flags & 0b1000)
	{
		new_MBS.mbs_mods_addr = memcpy(&mbsData[index], MBS->mbs_mods_addr, MBS->mbs_mods_count * sizeof(*MBS->mbs_mods_addr));
		index += MBS->mbs_mods_count * sizeof(*MBS->mbs_mods_addr);
	}
	if(MBS->mbs_flags & 0b1000000)
	{
		new_MBS.mbs_mmap_addr = memcpy(&mbsData[index], MBS->mbs_mmap_addr, MBS->mbs_mmap_length);
		index += MBS->mbs_mmap_length;
	}

	print("Lade Kernel...\n\r");
	uintptr_t address = (uintptr_t)elfLade(new_MBS.mbs_mods_addr[mod].mod_start, 0x10);

	print("Initialisiere Long Mode...\n\r");
	uint8_t Ergebnis;
	asm volatile(
			"mov $0x80000001,%%eax;"
			"cpuid;"
			"and $0x20000000,%%edx;"
			"test %%edx,%%edx;"
			"jz .1;"

			"mov $0x1,%%eax;"
			"cpuid;"
			"and $0x40,%%edx;"
			"test %%edx,%%edx;"
			"jz .1;"
			"mov $1,%%ax;"
			"mov %%ax,%0;"
			"jmp .2;"
			".1:"
			"mov $0,%%ax;"
			"mov %%ax,%0;"
			".2:"
			:"=m" (Ergebnis) : :"eax", "ebx", "ecx", "edx"
	);
	if(Ergebnis == 0)
	{
		print("Long Mode wird vom Prozessor nicht unterstuetzt.\n\r");
		print("Bitte beachten Sie, dass sie ein zu X86-64 kompatiblen Prozessor benoetigen.\n\r");
		print("Bootvorgang wird abgebrochen...");
		asm("cli;hlt;");
	}
	print("   Long Mode wird unterstuetzt.\n\r");
	print("   Aktiviere PAE (physical address extension)...\n\r");
	asm volatile(
			"mov %%cr4,%%eax;"
			"or $0x20,%%eax;"	//Das 5. Bit im CR4 aktivieren
			"mov %%eax,%%cr4;"
			: : :"eax"
	);
	print("   Initialisiere Paging...\n\r");
	static struct{
			uint64_t speicher[PAGE_ENTRIES];
	}Speicher[3 + MAP]__attribute__((aligned(4096)));
	PML4_t *PML4;
	PDP_t *PDP;
	PD_t *PD;
	PT_t *PT;

	//Adressen für die Tabellen ausfindig machen: müssen 4kb aligned sein
	uintptr_t Adresse = (uintptr_t)Speicher;
	PML4 = (PML4_t*)Adresse;
	PDP = (PDP_t*)(Adresse + 0x1000);
	PD = (PD_t*)(Adresse + 2 * 0x1000);
	PT = (PT_t*)(Adresse + 3 * 0x1000);

	//Erste 16MB des Speichers mappen

	setPML4Entry(0, PML4, 1, 1, 0, 1, 0, 0, 0, 0, (uint64_t)(uint32_t)PDP);
	for(i = 1; i < PAGE_ENTRIES; i++)
		setPML4Entry(i, PML4, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	setPDPEntry(0, PDP, 1, 1, 0, 1, 0, 0, 0, 0, (uint64_t)(uint32_t)PD);
	for(i = 1; i < PAGE_ENTRIES; i++)
		setPDPEntry(i, PDP, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	for(i = 0; i < MAP; i++)
		setPDEntry(i, PD, 1, 1, 0, 1, 0, 0, 0, 0, (uint64_t)(uint32_t)(PT + i));
	for(i = MAP; i < PAGE_ENTRIES; i++)	//Rest mit nullen auffüllen
		setPDEntry(i, PD, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	register int j;
	for(i = 0; i < MAP; i++)
		for(j = 0; j < PAGE_ENTRIES; j++)
			setPTEntry(j, PT + i, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, (j * 4096) + (i * 512 * 4096));

	/*for(i = 0; i < (0xFFFFF / 4096); i++)	//Erstes MB mappen.
		setPTEntry(i, &PT, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, i * 4096);


	if(getElfLength((elf_header*)MBS->mbs_mods_addr[mod].mod_start) <= (0x100000 / 4096))
		for(; i < getElfLength((elf_header*)MBS->mbs_mods_addr[mod].mod_start); i++)
			setPTEntry(i, &PT, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, i * 4096);
	else	//Falls Kernel grösser als ein MB so muss er bis zum Ende gemappt werden.
	{
		for(; i < PAGE_ENTRIES; i++)
			setPTEntry(i, &PT, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, i * 4096);
		register int j;
		for(j = 1; j < ((getElfLength((elf_header*)MBS->mbs_mods_addr[mod].mod_start) - 0x100000) / 0x1FFFFF); j++)
			for(; (i - (j * PAGE_ENTRIES)) < PAGE_ENTRIES; i++)
				setPTEntry(i - (j * PAGE_ENTRIES), &PT, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, i * 4096);
	}*/

	//Adresse der PML4 und PWT-Bit gesetzt
	register uint32_t cr3 = 0 | (((uint32_t)PML4 & 0xFFFFFFE0) | 0x8);//Adresse der PML4 und PWT-Bit gesetzt
	asm volatile(
			"mov %0,%%cr3;"
			: : "a" (cr3)
	);

	print("   Paging initialisiert.\n\r");
	print("   Long Mode aktivieren...\n\r");
	asm volatile(								//LM-Bit setzen = Long Mode aktivieren
			"mov $0xc0000080,%%ecx;"
			"rdmsr;"
			"bts $8,%%eax;"
			"wrmsr;"
			: : :"eax", "ecx", "edx"
	);
	print("Long Mode aktiviert.\n\r");

	print("Aktiviere Paging und starte Kernel...\n\r");
	asm volatile(
			"mov %%cr0,%%eax;"
			"bts $31,%%eax;"
			"mov %%eax,%%cr0;"

			"pushl $0x18;"	//CS und EIP auf den Stack legen damit der Prozessor nach dem ret
			"pushl %0;"		//Befehl dort weitermacht. ljmp $0x18,*%0 funktioniert nicht.
			"lret;"			//So kann man dies überbrücken.
			: :"r" (address), "b" (&new_MBS) :"eax"
	);
	/*asm(
			"mov $0x20,%%ax;"
			"mov %%ax,%%ds;"
			"mov %%ax,%%es;"
			"mov %%ax,%%ss;"
			"mov %%ax,%%fs;"
			"mov %%ax,%%gs;"

			"pushl $0x18;"	//CS und EIP auf den Stack legen damit der Prozessor nach dem ret
			"pushl %0;"		//Befehl dort weitermacht. ljmp $0x18,%0 funktioniert nicht.
			"lret;"			//So kann man dies überbrücken.
			: :"b" (address)
	);*/

	while(1);
}

void print(char *ausgabe)
{
	int i;
	int16_t *gs = (int16_t*)GRAFIKSPEICHER;
	for(i = 0; ausgabe[i] != '\0'; i++)
	{
		switch(ausgabe[i])
		{
			case '\n':
				Zeile++;
			break;
			case '\r':
				Spalte = 0;
			break;
			default:
				//Zeichen in den Grafikspeicher kopieren
				gs[Zeile * 80 + Spalte] = (ausgabe[i] | (0x02 << 8));	//0x02 = Grün auf Schwarz
				Spalte++;
				if(Spalte >= 79)
				{
					Spalte = 0;
					Zeile++;
					if(Zeile >= 24)
					{
						scrollScreen(1);
						Zeile = 23;
					}
				}
		}
	}
}

void scrollScreen(int Anzahl)
{
	int i,zeile, spalte;
	char *gs = (char*)GRAFIKSPEICHER;
	for(i = 0; i < Anzahl; i++)
	{
		memcpy(gs, gs + zeile * 80, 24 * 80);
		/*for(zeile = 0; zeile < 24; zeile++)
			for(spalte = 0; spalte < 80; spalte++)
			{
				gs[zeile * 80 + spalte] = gs[(zeile + 1) * 80 + spalte];
			}*/
		//Letzte Zeile löschen
		zeile = 24 - i;
		for(spalte = 0; spalte < 80; spalte++)
			gs[zeile * 80 + spalte] = 0;
	}
}

void clearDisp()
{
	register int i;
	char *gs = (char*)GRAFIKSPEICHER;
	for(i = 0; i < 4096; i++)
		gs[i] = 0;
	Zeile = Spalte = 0;
}

char *IntToStr(int32_t Zahl)
{
	char *str;
	int len, i, c;
	if(Zahl == 0) return "0";
	len = (Zahl < 0) ? 2 : 1;
	for(i = Zahl; i != 0; i /= 10)
		len++;
	str = (char*)Speicher;
	str[len--] = '\0';
	for(i = Zahl, len--; i != 0;)
	{
		c = (i >= 0) ? (i % 10) : (i % 10) * -1;
		str[len--] = (char)(c + 0x30);	//0x30 = Verschiebung der Zahlen in der ASCII-Tabelle
		i /= 10;
	}
	if(Zahl < 0)
		str[0] = '-';
	return str;
}

void *memcpy(void *to, const void *from, size_t size)
{
	size_t i;
	const char *src = from;
	char *dest = to;
	for(i = 0; i < size; i++)
		dest[i] = src[i];
	return to;
}

void *memset(void *block, int c, size_t n)
{
	unsigned char volatile *i;
	for(i = block; i < block + n; i++)
		*i = (unsigned char)c;
	return block;
}

size_t strlen(const char *cs)
{
	register size_t i;
	for(i = 0; cs[i] != '\0'; i++);
	return i;
}

void panic(const char *string)
{
	print("PANIC: ");
	print(string);
	//CPU anhalten
	asm volatile("cli;hlt;");
}
