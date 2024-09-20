[BITS 32]

section .asm

global enable_paging
global paging_load_directory
global paging_set_table_entry

enable_paging:
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    ret

paging_is_enabled:
    mov eax, cr0
    and eax, 0x80000000
    shr eax, 31
    ret

paging_load_directory:
    push ebp
    mov ebp, esp
    mov eax, [ebp+8]
    mov cr3, eax
    pop ebp
    ret

paging_set_table_entry:
    push ebp
    mov ebp, esp
    push ebx

    mov eax, [ebp+8]  ; Directory address
    mov ebx, [ebp+12] ; Table offset
    mov ecx, [ebp+16] ; Value

    mov edx, cr0
    and edx, ~0x80000000
    mov cr0, edx

    mov eax, [eax]
    and eax, 0xfffff000
    mov [eax+ebx], ecx

    or edx, 0x80000000
    mov cr0, edx

    pop ebx
    pop ebp
    ret
