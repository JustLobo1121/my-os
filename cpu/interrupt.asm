[bits 32]

global isr0
[extern isr0_handler]
global isr13
[extern isr13_handler]
global isr14
[extern isr14_handler]
global isr32
[extern timer_handler]
[extern schedule]
global isr33
[extern keyboard_handler]
global isr43
global isr44
[extern mouse_handler]
global isr46
global isr128
[extern isr128_handler]

isr0:
    cli
    pusha
    call isr0_handler
    jmp $

isr13:
    cli
    pusha
    call isr13_handler
    jmp $

isr14:
    cli
    pusha
    call isr14_handler
    jmp $

isr32:
    pusha
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call timer_handler
    push esp
    call schedule
    add esp, 4
    mov esp, eax
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa
    iretd

isr33:
    pusha
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call keyboard_handler

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa
    iretd

isr43:
    pusha
    mov al, 0x20
    out 0xA0, al 
    out 0x20, al 
    popa
    iretd

isr44:
    pusha
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call mouse_handler

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa
    iretd

isr46:
    pusha
    mov al, 0x20
    out 0xA0, al 
    out 0x20, al 
    popa
    iretd

isr128:
    pusha
    call isr128_handler
    popa
    iretd