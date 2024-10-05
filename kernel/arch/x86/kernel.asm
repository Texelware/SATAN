[BITS 32]
section .multiboot2_header
MULTIBOOT_AOUT_KLUDGE    equ  1 << 16
                              ;FLAGS[16] indicates to GRUB we are not
                              ;an ELF executable and the fields
                              ;header address, load address, load end address;
                              ;bss end address and entry address will be available
                              ;in Multiboot header
MULTIBOOT_ALIGN          equ  1<<0   ; align loaded modules on page boundaries
MULTIBOOT_MEMINFO        equ  1<<1   ; provide memory map

; HEADER_FLAGS   equ  MULTIBOOT_ALIGN|MULTIBOOT_MEMINFO

HEADER_MAGIC     equ  0xE85250D6                                    ;multiboot2 magic number
HEADER_ARCH      equ  0                                             ;arch (0 - i386 protected)
HEADER_LENGTH    equ  (header_end - header_start)                   ;length of the header
HEADER_CHECKSUM  equ  0x100000000 - (HEADER_MAGIC + HEADER_ARCH + HEADER_LENGTH) ;checksum

header_start:
    dd HEADER_MAGIC
    dd HEADER_ARCH                                         
    dd HEADER_LENGTH
    dd HEADER_CHECKSUM

    dw  0  ;type=0 for end tag
    dw  0  ;flags=0
    dd  8  ;size=8
header_end:

section .rodata
; GDT
gdt_start:
gdt_null:
    dd 0x0
    dd 0x0

; offset 0x8
gdt_code:     ; CS SHOULD POINT TO THIS
    dw 0xffff ; Segment limit first 0-15 bits
    dw 0      ; Base first 0-15 bits
    db 0      ; Base 16-23 bits
    db 0x9a   ; Access byte
    db 11001111b ; High 4 bit flags and the low 4 bit flags
    db 0        ; Base 24-31 bits

; offset 0x10
gdt_data:      ; DS, SS, ES, FS, GS
    dw 0xffff ; Segment limit first 0-15 bits
    dw 0      ; Base first 0-15 bits
    db 0      ; Base 16-23 bits
    db 0x92   ; Access byte
    db 11001111b ; High 4 bit flags and the low 4 bit flags
    db 0        ; Base 24-31 bits

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start-1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

section .text
extern kernel_main
global _start
_start:
    lgdt[gdt_descriptor]

    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp CODE_SEG:after_gdt

after_gdt:
    mov ebp, 0x00200000
    mov esp, ebp

    ; Enable the A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Remap the master PIC
    mov al, 00010001b
    out 0x20, al ; Tell master PIC

    mov al, 0x20 ; Interrupt 0x20 is where master ISR should start
    out 0x21, al

    mov al, 00000001b
    out 0x21, al
    ; End remap of the master PIC

    call kernel_main

    jmp $
