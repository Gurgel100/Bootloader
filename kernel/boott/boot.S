.code32
.extern boot
.global _start

#Magicnumber - Erkennungsmerkmal für GRUB
MB_MAGIC = 0x1BADB002
#Flags er soll mem_lower und mem_upper bestimmen
MB_FLAGS = 0x2
#Prüfsumme
MB_CHECKSUM = -(MB_MAGIC + MB_FLAGS)

.section multiboot
.align 4
.int MB_MAGIC
.int MB_FLAGS
.int MB_CHECKSUM

.section .text
_start:
cli
#Stack initilialisieren
mov $(kernel_stack),%esp

#Bildschirm löschen
call clear
mov $(msg_boot),%esi
call print_boot

#Ausgeben, dass Stack initialisiert
mov $(msg_stack_init),%esi
call print_boot

mov $(msg_call_main),%esi
call print_boot

#Addresse der Multiboot-Struktur auf den Stack legen und boot aufrufen (C-Code)
push %ebx
call boot
#falls Rücksprung aus dem Kernel CPU auschalten
_stop:
cli
hlt
#falls es doch noch weiter geht noch mal probieren
jmp _stop


# ###################################################################################
#Funktionen
print_boot:
#Register für Zeichen
push %eax
#Offset
push %ebx
#Register für Zeile und Spalte
push %ecx
xor %bx,%bx
#cl = Spalte
#ch = Zeile
push %esi
mov $(ausgabe),%esi
lodsw
mov %ax,%cx
pop %esi

print_bloop:
#Lade Zeichen aus dem Speicher (Addresse es:esi) nach eax
lodsb
#Überprüfe auf NULL am Ende des Strings
test %al,%al
jz print_bdone
#Überprüfe auf "line feed" (neue Zeile)
cmp $0x0A,%al
je neu_zeile
#Nicht mehr als 24 Zeilen deshalb keine Kontrolle auf Zeilenüberlauf
#Überprüfe auf "carriage return" (auf Anfang der Zeile zurück gehen)
cmp $0x0D,%al
jne pweiter
xor %cl,%cl
pweiter:
push %ax
#Zeile mit 80 multiplizieren (Ergebnis in ax)
mov $0x50,%al
#al mit ch multiplizieren
mul %ch
#Ergebnis mit Spalte addieren
xor %ebx,%ebx
mov %cl,%bl
add %ax,%bx
pop %ax
shl $0x1,%bx
#Berechnung der Addresse (0xB8000 + Offset)
add $0xB8000,%ebx
mov %ebx,%edi
#Weisse Zeichen auf schwarzem Hintergrund
mov $0x0F,%ah
#Speichere das Zeichen in ax nach es:edi
stosw
#Nächstes Zeichen
inc %cl
#Überprüfe auf neue Zeile
cmp $0x50,%cl
jne print_bloop
xor %cl,%cl
neu_zeile:
inc %ch
#nie mehr als 25 Zeilen-->keine Kontrolle
jmp print_bloop
print_bdone:
mov $(ausgabe),%edi
mov %cx,%ax
stosw
pop %ecx
pop %ebx
pop %eax
ret


clear:
push %eax
push %ecx
mov $0x0B8000,%edi
xor %ax,%ax
mov $0x7D0,%ecx
rep stosw
xor %ax,%ax
mov $(ausgabe),%edi
stosw
pop %ecx
pop %eax
ret
#.space 10485760
#Platz für den späteren Kernel, der an Addresse 0x100000 geladen wird und er vielleicht grösser
#als 1MB sein könnte (vorausschauen). Kann beliebig verändert werden.

.section .data
ausgabe:
.space 16
.section .rodata
#Strings
msg_stack_init:	.string "Stack initialisiert\n\r"
msg_boot:		.string "Boote...\n\r"
msg_call_main:	.string "C-Bootloader laden\n\r"

# 8 kB Stack fuer den Kernel. Das Label steht hinter dem freien Speicher,
# weil der Stack nach unten waechst
.section .bss
.space 8192
kernel_stack:
