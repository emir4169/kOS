/**
 * 
 * @copyright Copyright (c) 2024, Kevin Kleiman, All Rights Reserved
 * 
 * This is the kernel for yet another hobbyOS designed and developed by Kevin Kleiman.
 * Feel free to copy, use, edit, etc. anything you see 
 *
 * This was originally designed to try writing a ring0 math library but I soon realized,
 * I should just make a full-blown kernel. It has been a great learning experience and I
 * implore anyone even remotely interested to fork, play around, contribute, whatever
 * you want. 
 *
 * For now, it's pretty barebones and shitty, but hopefully that will change with time.
 * Have fun creating kOS (pronounced "Chaos")
 */

#include "drivers/tty.h"
#include "syscall.h"
#include "kutils.h"
#include "stdio.h"
#include "file.h"

/* Init syscall table */
static syscall_t syscall_entries[10] = {
    __write,
    __open
};

/* Unused for now */
__attribute__((naked, used)) static void syscall_push_regs() 
{
    __asm__ __volatile__(
                         "pushl %eax\n"
                         "pushl %gs\n"
                         "pushl %fs\n"
                         "pushl %es\n"
                         "pushl %ds\n"

                         "pushl %ebp\n"
                         "pushl %edi\n"
                         "pushl %esi\n"
                         "pushl %edx\n"
                         "pushl %ecx\n"
                         "pushl %ebx\n"
                         "pushl %esp\n"
                        );
}

/* Unused for now */
__attribute__((naked, used)) static void syscall_pop_regs() 
{
    __asm__ __volatile__(
                         "addl $4, %esp\n"

                         "popl %ebx\n"
                         "popl %ecx\n"
                         "popl %edx\n"

                         "popl %esi\n"
                         "popl %edi\n"
                         "popl %esp\n"

                         "popl %ds\n"
                         "popl %es\n"
                         "popl %fs\n"
                         "popl %gs\n"

                         "addl $4, %esp\n"
                        );
}

/* Callback for handling all syscalls */
static void syscall_cb(i_register_t registers) 
{
    ASSERT_STRICT(registers.eax > (SYSCALL_MAX - 1), "Invalid syscall!");

    // lookup syscall from table and call
    syscall_entries[registers.eax](&registers);
}

/* 
 * Begin syscall definitions
 */

/* write(), writes to a file descriptor (fd) */
static void __write(i_register_t* registers)
{   
    // get syscall parameters from registers struct
    int fd = registers->ebx;
    char* buffer = (char*) registers->ecx;
    size_t n = registers->edx;

    // check for standard file descriptors
    ASSERT_STRICT(fd == STDIN_FD, "stdin not yet supported.");

    if (fd == STDOUT_FD) tty_write(buffer);            // redirect stdout to tty for now
    else if (fd == STDERR_FD) printk("stderr: Error %s\n");
}

/* open(), writes to a file descriptor (fd) */
void __open(i_register_t* registers)
{
    printk("syscall open()\n");
}

/* Init syscall callbacks */
void syscall_init() 
{
    register_interrupt_handler(128, syscall_cb); 
}
