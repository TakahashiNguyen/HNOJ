int main()
{
    __asm__ volatile(R"(
        subq $32, %rsp

        movq $0, %r8

    .L_read_int_char:
        movq $0, %rax
        movq $0, %rdi
        movq %rsp, %rsi
        movq $1, %rdx
        syscall

        cmpq $0, %rax 
        jle .L_done_read

        movzbq (%rsp), %rax
        
        cmpb $10, %al
        je .L_done_read

        cmpb $48, %al
        jl .L_read_int_char
        cmpb $57, %al
        jg .L_read_int_char

        subq $48, %rax
        imulq $10, %r8              
        addq %rax, %r8

        jmp .L_read_int_char
    .L_done_read:
        subq $32, %rsp              # Cấp phát 32 bytes trên Stack làm bộ nhớ đệm chuỗi xuất
        
        movb $10, 31(%rsp)          # Đặt ký tự xuống dòng '\n' ở cuối vùng nhớ đệm
        leaq 31(%rsp), %rsi         # %rsi làm con trỏ chạy, bắt đầu từ vị trí ký tự '\n'
        
        movq %r8, %rax              # Nạp số nguyên cần chia vào %rax
        movq $10, %rcx              # %rcx đóng vai trò là bộ chia cố định (chia cho 10)

    .L_convert_loop:
        decq %rsi                   # Dịch con trỏ lùi về phía trước 1 byte trên Stack
        xorq %rdx, %rdx             # BẮT BUỘC: Xóa sạch %rdx về 0 trước khi thực hiện lệnh chia
        divq %rcx                   # Thực hiện: %rax / 10
                                    # Kết quả: Thương số nằm ở %rax, Số dư nằm ở %rdx
        
        addq $48, %rdx              # Chuyển số dư (0-9) thành ký tự ASCII số ('0'-'9')
        movb %dl, (%rsi)            # Cất ký tự ASCII này vào vị trí con trỏ %rsi đang đứng
        
        testq %rax, %rax            # Kiểm tra xem thương số (%rax) đã bằng 0 chưa?
        jnz .L_convert_loop         # Nếu thương chưa bằng 0, tiếp tục chia để lấy chữ số tiếp theo

        # Tính toán độ dài (độ dài chuỗi = Địa chỉ kết thúc - Địa chỉ bắt đầu %rsi)
        leaq 32(%rsp), %rdx
        subq %rsi, %rdx             # Độ dài chuỗi hợp lệ nằm trong %rdx

        # Gọi sys_write(1, %rsi, %rdx) để bắn toàn bộ chuỗi số ra màn hình
        movq $1, %rax               # syscall số 1 (sys_write)
        movq $1, %rdi               # stdout (1)
        syscall                     # Thực hiện in chuỗi số nguyên!

        addq $32, %rsp              # Dọn dẹp 32 bytes bộ nhớ đệm xuất của Stack
    )");

    return 0;
}