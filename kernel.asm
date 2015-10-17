[BITS 16]
[ORG 0x8000]
 
xor ax, ax ; set ax to 0 = first segment
mov ss, ax
mov ds, ax
mov es, ax

cli

%include "a20.asm"

enter_pmode:
    lgdt [gdtr]
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 08h:far_jump

[BITS 32]
far_jump:
    mov ax, 10h
    mov ds, ax
    mov ss, ax
    mov esp, 090000h
    mov byte [ds:0B8000h], 'P'
    mov byte [ds:0B8001h], 1Bh

hang:
    jmp hang
 
gdt_null: ; the first element is the so called null descriptor
    dd 0   ; it is not referenced by the processor
    dd 0

gdt_code: ; usually we want at least one segment descriptor for code
    dw 0ffffh ; segment length bits 0-15
    dw 0 ; segment base byte 0,1
    db 0 ; segment base byte 2
    db 10011010b ; access rights
    db 11001111b ; bit 7-4: 4 flag bits: granularity, default operation
                 ; size bit, 2 bits available for OS
                 ; bit 3-0: segment length bits 16-19
    db 0         ; segment base byte 3

gdt_data: ; usually we want at least one segment descriptor for data
    dw 0ffffh
    dw 0
    db 0
    db 10010010b
    db 11001111b
    db 0
gdt_end:

gdtr:
    dw gdt_end - gdt_null - 1 ; 2 bytes are the size of the GDT in bytes - 1
    dd gdt_null ; 4 bytes are the address of the GDT start
