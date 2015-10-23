section .text
global int_handler
int_handler:
    mov ax, 0x10
    mov gs, ax
    mov dword [gs:0xB8000],') : '
    hlt
