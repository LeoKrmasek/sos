a20_enable:
    call a20_stop_if_active

a20_bios:
    mov ax, 2401h
    int 15h

    call a20_stop_if_active

a20_keyboard:
    call a20_wait_inbuf ; disable the keyboard
    mov al, 0adh
    out 64h, al
    
    call a20_wait_inbuf ; tell the controller we want to read data
    mov al, 0d0h
    out 64h, al
    
    call a20_wait_outbuf ; read the P2 port provided by the controller
    in al, 60h
    push ax

    call a20_wait_inbuf ; tell the controller we want to write data
    mov al, 0d1h
    out 64h, al
    
    call a20_wait_inbuf ; write the new P2 port with A20 line active
    pop ax
    or al, 2
    out 60h, al

    call a20_wait_inbuf ; re-enable the keyboard
    mov al, 0aeh
    out 64h, al

    call a20_wait_inbuf

    call a20_stop_if_active_loop

a20_fast:
    in al, 92h
    test al, 2
    jnz a20_done ; A20 Fast Gate is already activated
    or al, 2
    and al, 0feh
    out 92h, al
    
    call a20_stop_if_active_loop

    jmp a20_done ; give up, no other methods there
                 ; could jump to another location and display an error

a20_wait_inbuf:
    in al, 64h
    test al, 2
    jnz a20_wait_inbuf
    ret

a20_wait_outbuf:
    in al, 64h,
    test al, 1
    jz a20_wait_outbuf
    ret

a20_active:
    pushf
    push ds
    push es
    push di
    push si

    xor ax, ax ; ax = 0
    mov es, ax

    not ax; ax = 0xffff
    mov ds, ax
    
    mov di, 0500h
    mov si, 0510h

    mov al, byte [es:di] ; save the old values from memory
    push ax
    
    mov al, byte [ds:si]
    push ax

    mov byte [es:di], 00h ; write 0x00 to one and 0xff to the other location
    mov byte [ds:si], 0ffh
    
    cmp byte [es:di], 0ffh ; check if the address we set to 0x00 was
                          ; set to 0xff later, then we have only 20 bit
                          ; addresses
    
    pop ax ; restore the bytes we set before
    mov byte [ds:si], al
    
    pop ax
    mov byte [es:di], al
    
    mov ax, 0
    je a20_active_end
    mov ax, 1

a20_active_end:
    pop si
    pop di
    pop es
    pop ds
    popf
    ret

a20_stop_if_active:
    call a20_active
    test ax, ax ; check if all bits are 0, then ZF = 1
    jnz a20_done
    ret

a20_stop_if_active_loop: ; try in a loop if a20 is active for k times
    mov bx, 0ffh
a20_stop_if_active_loop_iterator:
    dec bx
    call a20_stop_if_active
    test ax, ax
    jnz a20_done

    test bx, bx ; check if bx 0
    jnz a20_stop_if_active_loop_iterator

    ret
    

a20_done:
