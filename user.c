#include "user.h"

extern char __stack_top[];

// Handle application exit:
__attribute__((noreturn)) void exit(void)
{
    syscall(SYS_EXIT, 0, 0, 0);
    for (;;)
        ; // Just in case!
}

void putchar(char ch)
{
    syscall(SYS_PUTCHAR, ch, 0, 0);
}

int getchar(void)
{
    return syscall(SYS_GETCHAR, 0, 0, 0);
}

// The execution of the application starts from the "start" function.
// Similar to the kernel's boot process, it sets up the stack pointer and
// calls the application's "main" function:
__attribute__((section(".text.start")))
__attribute__((naked)) void
start(void)
{
    __asm__ __volatile__(
        "mv sp, %[stack_top] \n"
        "call main           \n"
        "call exit           \n" ::[stack_top] "r"(__stack_top));
}

// Syscall main entry point:
//
// The "syscall" function sets the system call number in the "a3" register and
// the system call arguments in the "a0" to "a2" registers, then executes
// the "ecall" instruction.
//
// The "ecall" instruction is a special instruction used to delegate processing 
// to the kernel.
//
// When the "ecall" instruction is executed, an exception handler is called,
// and control is transferred to the kernel.
//
// The return value from the kernel is set in the a0 register.
int syscall(int sysno, int arg0, int arg1, int arg2)
{
    register int a0 __asm__("a0") = arg0;
    register int a1 __asm__("a1") = arg1;
    register int a2 __asm__("a2") = arg2;
    register int a3 __asm__("a3") = sysno;

    __asm__ __volatile__("ecall"
                         : "=r"(a0)
                         : "r"(a0), "r"(a1), "r"(a2), "r"(a3)
                         : "memory");

    return a0;
}