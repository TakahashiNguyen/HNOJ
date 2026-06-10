int main()
{
    __asm__ volatile(R"(
        call index
        jmp end

    index:
        call read_int

        imulq $3, %r12

        call print_int
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