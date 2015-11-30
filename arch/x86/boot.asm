section .multiboot
align 4
    dd 1BADB002h
    dd 0003h ; set align boot modules to page boundaries, supply memory map
    dd -(1BADB002h + 0003h)

KERNEL_VIRTUAL_BASE equ 0xC0000000
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22)
 
section .data
align 0x1000
page_dir:
    dd 0x00000083
    times (KERNEL_PAGE_NUMBER - 1) dd 0
    dd 0x00000083
    times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0

section .bootstrap_stack nobits
stack_bottom:
    resb 16384 ; 16 KiB
stack_top:

section .text
extern kernel_main
global _kernel_start
_kernel_start:
    mov ecx, (page_dir - KERNEL_VIRTUAL_BASE)
    mov cr3, ecx
 
    mov ecx, cr4
    or ecx, 0x00000010
    mov cr4, ecx
 
    mov ecx, cr0
    or ecx, 0x80000000
    mov cr0, ecx
 
    lea ecx, [higher_half]
    jmp ecx

higher_half:
    xor eax, eax
    xor ebx, ebx
    ; set a self reference to the page directory for easy editing
    mov eax, (page_dir - KERNEL_VIRTUAL_BASE) ; fetch the address of the page dir
    mov ebx, eax
    xchg bx, bx
    add eax, 0xffc ; last page dir entry desired
    ; and ebx, 0xfffff000 ; set the lower 12 bits correctly for page entry
    or ebx, 0x003 ; set the lower 12 bits correctly for page entry
    mov dword [eax], ebx

    mov esp, stack_top
    call kernel_main

    cli
    hlt
.lhang:
    jmp .lhang
