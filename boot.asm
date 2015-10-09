[BITS 16]
[ORG 0x7C00]
 
xor ax, ax ; set ax to 0 = first segment
mov ss, ax
mov ds, ax
mov es, ax
 
mov si, welcome_msg
call print_string
 
jmp $
 
print_string:
    mov ah, 0eh     ; output with interrupt 10h
.next_char:
    lodsb           ; reads one bytes and saves it to AL
    cmp al, 0       ; was the nullbyte read?
    je .done       ; if yes: finished
    int 10h         ; otherwise: show current character from AL
    jmp .next_char   ; and go on with next char
.done:
    ret
 
welcome_msg db 'Hello World!', 0
 
times 510-($-$$) hlt
dw 0xAA55
