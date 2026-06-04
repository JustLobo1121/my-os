[bits 32]
global gdt_flush
global tss_flush
global switch_to_user_mode

gdt_flush:
    mov eax, [esp+4]
    lgdt [eax]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax    
    jmp 0x08:.flush
.flush:
    ret

tss_flush:
    mov ax, 0x2B 
    ltr ax
    ret

switch_to_user_mode:
    cli
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    mov eax, esp
    push 0x23
    push eax
    pushfd
    pop eax
    or eax, 0x200
    push eax
    push 0x1B
    push .user_jump
    iretd
    
.user_jump:
    ret