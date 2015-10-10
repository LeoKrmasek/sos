[BITS 16]
[ORG 0x7C00]
 
xor ax, ax ; set ax to 0 = first segment
mov ss, ax
mov ds, ax
mov es, ax
 
disk_reset:
    xor ah, ah ; ah=0, reset drive head
    int 13h
    jc disk_reset
 
disk_read:
    mov ah, 0x02 ; with interrupt 3h: reading!
    mov al, 10 ; read 10 sectors
    mov cl, 2  ; from sector 2
    mov bx, 0x8000 ; save data to location 0x8000
 
    mov ch, 0
    mov dh, 0
    int 13h
    jc disk_read
 
    jmp 0x0000:0x8000   ; jump to the new program, hooray
 
times 510-($-$$) db 0
dw 0xaa55
