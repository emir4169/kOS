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

#include "kernel.h"
#include "drivers/tty.h"
#include "drivers/ramfs.h"

/* Init syscall table */
static syscall_t syscall_entries[10] = {
    __write,
    __open,
    __print,
    __malloc,
    __free
};

/* Unused for now */
 static void 
__attribute__((naked, used)) syscall_push_regs() 
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
static void 
__attribute__((naked, used)) syscall_pop_regs() 
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
static void 
syscall_cb(i_register_t registers) 
{
    KASSERT_PANIC(registers.eax > (SYSCALL_MAX - 1), "Invalid syscall!");

    // lookup syscall from table and call
    switch (registers.eax) {
        case 0:
            printk("syscall: write\n");
            break;
        case 1:
            printk("syscall: open\n");
            break;
        case 2:
            printk("syscall: print\n");
            break;
        case 3:
            printk("syscall: malloc\n");
            break;
        case 4:
            printk("syscall: free\n");
            break;
        default:
            printk("syscall: unknown\n");
            break;
    }
    syscall_entries[registers.eax](&registers);
}

/* 
    __write,
    __open,
    __print,
    __malloc,
    __free
 * Begin syscall definitions
 */

/* write(), writes to a file descriptor (fd) */
static void 
__write(i_register_t* registers)
{   
    // old ass stuff
    // get syscall parameters from registers struct
    //int fd = registers->ebx;
    //char* buffer = (char*) registers->ecx;
    //size_t n = registers->edx;

    // check for standard file descriptors

    // arguments:
    // edx = filename
    // ecx = length of new file data
    // ebx = address to new file data
    // returns:
    // ecx = did we write? FF = yes, 00 = no
    // edx = the address to the file data
    char* buffer = (char*) registers->ebx;
    size_t n = registers->ecx;
    // write the data to the file
    //ramdisk_write_file(ramdisk, (void*) registers->ebx, buffer, n);
    char* filename = (char*) registers->edx;

    // print stuff
    printk("raw filename just in case: %x\n", filename);
    printk("__write: writing %d bytes to file %s\n", n, filename);
    printk("__write: data address: %x\n", buffer);
    // check if the file exists in the ramdisk
    ramdisk_file_t file = ramdisk_lookup(ramdisk, "test.txt");
        // file not found
    KASSERT_PANIC(file.data == NULL, "File not found! (__write)");
    // yoo theres a file. write time!
    kfree(file.data);
    file.data = kmalloc(n);
    file.size = n;
    kmemcpy(file.data, buffer, n);

    asm volatile(
        "movl %0, %%ebx\n"
        : /* no output */
        : "r"((uint32_t)file.data)
        : "%ebx"
    );

}

/* open(), writes to a file descriptor (fd) */
static void 
__open(i_register_t* registers)
{
    // arguments:
    // ebx = address to filename
    // ecx = length of filename
    // returns:
    // ecx = file length
    // edx = the address to the files data
    // ebx = the address to the files name

    char* filename = (char*) registers->ebx;
    size_t filename_length = registers->ecx;
    // check if the file exists in the ramdisk
    ramdisk_file_t file = ramdisk_lookup(ramdisk, filename);
        // file not found
    KASSERT_PANIC(file.data == NULL, "File not found!");
    // file found, set registers
    registers->ebx = &file.data; // address to the file data
    registers->ecx = file.size; // size of the file
    registers->edx = &file.name; // address to the file name
    // print the file name to the terminal
    printk("Opened file: %s\n", file.name);
    // print the file size to the terminal
    printk("File size: %d bytes\n", file.size);


    printk("syscall open()\n");
}
// print(), prints ecx bytes of data starting from ebx to the terminal with printk()
static void 
__print(i_register_t* registers)
{
    char* buffer = (char*) registers->ebx;
    size_t n = registers->ecx;
    if (n > 0) {
        char output[n + 1]; // create a buffer with the specified size plus one for null-terminator
        kmemcpy(output, buffer, n); // copy data to the buffer
        output[n] = '\0'; // null-terminate the string
        printk("%s", output); // print the data to the terminal
    }
}
// malloc(), allocates memory of size ebx and returns the address in ebx
static void
__malloc(i_register_t* registers)
{
    size_t size = registers->ebx;
    void* ptr = kmalloc(size);
    // use inline asm to set ebx
    asm volatile(
        "movl %0, %%ebx\n"
        : /* no output */
        : "r"((uint32_t)ptr)
        : "%ebx"
    );
    registers->ebx = (uint32_t)ptr;
}

static void
__free(i_register_t* registers)
{
    kfree(registers->ebx);
}
/* Init syscall callbacks */
void 
syscall_init() 
{
    register_interrupt_handler(128, syscall_cb); 
}
