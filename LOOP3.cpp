int main()
{
    __asm__ volatile(R"(
        call index
        jmp end

    index:
        movq $0, %r8
        movq $0, %r9

    .L_exec:
        call read_int

        call is_even

        cmpq $0, %r12
        je .L_case_odd

        incq %r8
        jmp .L_check_r1
    .L_case_odd:
        decq %r8

    .L_check_r1:
        incq %r9

        cmpq $0, %r8
        jne .L_exec

        shrq $1, %r9

        movq %r9, %r12

        call print_int

        ret

    is_even:
        testq $1, %r12          

        setz %al                

        movq %rax, %r12

        ret

    read_int:
        subq $16, %rsp              # Bộ nhớ đệm đọc 1 byte trên Stack
        movq $0, %r12               # Xóa sạch %r12 để tích lũy số mới

    .L_read_loop:
        movq $0, %rax               # sys_read
        movq $0, %rdi               # stdin
        movq %rsp, %rsi
        movq $1, %rdx
        syscall

        cmpq $0, %rax
        jle .L_read_done

        movzbq (%rsp), %rax
        cmpb $10, %al
        je .L_read_done

        cmpb $48, %al
        jl .L_read_loop
        cmpb $57, %al
        jg .L_read_loop

        subq $48, %rax
        imulq $10, %r12
        addq %rax, %r12
        jmp .L_read_loop

    .L_read_done:
        addq $16, %rsp
        ret

    print_int:
        subq $32, %rsp
        movb $10, 31(%rsp)
        leaq 31(%rsp), %rsi
        
        movq %r12, %rax
        movq $10, %rcx

    .L_convert_loop:
        decq %rsi
        xorq %rdx, %rdx
        divq %rcx
        addq $48, %rdx
        movb %dl, (%rsi)
        testq %rax, %rax
        jnz .L_convert_loop

        # Tính độ dài chuỗi
        leaq 32(%rsp), %rdx
        subq %rsi, %rdx

        movq $1, %rax               # sys_write
        movq $1, %rdi               # stdout
        syscall

        addq $32, %rsp
        ret

    end:
    )");

    return 0;
}