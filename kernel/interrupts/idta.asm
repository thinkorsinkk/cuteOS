section .text
extern handle_interrupt
%define vector_has_error(v)  ((v) == 8 || ((v) >= 10 && (v) <= 14) || (v) == 17 || (v) == 21)
%assign vector 0
%rep 256
stub%+vector:
    %if !vector_has_error(vector)
    push 0
    %endif
    push vector
    jmp trap_common
%assign vector vector+1
%endrep

section .data 
global trap_stubs
trap_stubs:
%assign vector 0
%rep 256
    dq stub%+vector
%assign vector vector+1
%endrep

section .text
trap_common:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    mov r15, rsp
    cld
    and rsp, ~0xf
    mov rdi, r15
    call handle_interrupt ; in C
    mov rsp, r15
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
    add rsp, 16
    iretq