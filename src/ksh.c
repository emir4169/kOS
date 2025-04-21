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
#include "drivers/keyboard.h"
#include "drivers/tty.h"
#include "drivers/fat.h"
#include "drivers/ramfs.h"
#include "drivers/serial.h"
/* Our keyboard input buffer */
static char* inputbuf;
/* Initialize head pointer for input buffer */
static uint32_t inputbuf_head = 0;

/* Process data in input buffer as command */
static void
process_cmd()
{
    // a temporary shitty way to process commands
    // this should really be a hashmap
    KASSERT_GOTO_SUCCESS(kstrlen(inputbuf) == 0);

    if (kstrcmp(inputbuf, "clear"))
    {
        tty_clear();
    } 
    else if (kstrcmp(inputbuf, "reboot"))
    {
        // explicitly zero out our input buffer
        kmemset(inputbuf, 0, KSH_INPUT_BUF_SIZE);

        // then reboot
        warm_reboot();
    }
    else if (kstrcmp(inputbuf, "dumpt"))
    {
        // dump kernel page table/directory mappings
        pmm_dumpt();
    }
    else if (kstrcmp(inputbuf, "dumpfs"))
    {
        // dump fat bios parameter block
        fat_dump_bs();
    }
    else if (kstrcmp(inputbuf, "dumpramfs"))
    {
        ramfs_dump(ramdisk);
    }
    else if (kstrstr(inputbuf, "run", 3))
    {
        char* filename = inputbuf + 4;
        ramdisk_file_t file = ramdisk_lookup(ramdisk, filename);
        // 0 out eax
        asm volatile
        (
            "mov 0, %%eax\n"
            : /* no output */
            : /* no input */
            : "%eax"
        );
        int (*func)() = ((int (*)())file.data);
        int ret = func();
        printk("Returned:%x\n", ret);

    }
    else if (kstrcmp(inputbuf, "neofetch"))
    {
        // display the dopest shit
        tty_neofetch();
    } else {
        printk("Command not found: %s\n", inputbuf);
    }

success:
    // reset input buffer head
    inputbuf_head = 0;

    // reset our input buffer
    kmemset(inputbuf, 0, KSH_INPUT_BUF_SIZE);
}

/* Key press notification callback */
static void
notify_cb(uint8_t scan, uint8_t pressed)
{
    char c;

    // ensure our input buffer is allocated
    KASSERT_GOTO_FAIL_MSG(
        inputbuf == NULL, "inputbuf is not initialized!\n");

    // ignore key up
    KASSERT_GOTO_FAIL(pressed != 0);

    switch (scan)
    {
        case KEY_BACKSPACE:
            // check that we're not at the start of input buffer
            if (inputbuf_head == 0)
            {
                break;
            }

            // remove last char from input buffer
            inputbuf[--inputbuf_head] = 0;

            write_serial('\b');
            write_serial(' ');
            write_serial('\b');

            // remove char from screen
            tty_putc_relative('\0', -1, 0, TRUE);
            break;
        // tilde key reboots
        case KEY_TILDE:
            warm_reboot();
            break;
        // process keybuf on return
        case KEY_ENTER:
            tty_write("\n");

            // process current command
            process_cmd();

            // write our prompt
            tty_writecolor("> ", VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
            break;
        // otherwise, write ascii
        default:
            // restrict char count
            if (inputbuf_head == (KSH_INPUT_BUF_SIZE - 1))
            {
                break;
            }

            // get ascii
            c = keyboard_scan_to_char(scan);

            // set next char in buffer
            inputbuf[inputbuf_head++] = c;

            // finally, display the char
            tty_putc(c);
            break;
    }

fail:
    return;
}

/* Start kernel shell */
void
ksh_init()
{
    // allocate 256 byte input buffer
    inputbuf = (char*)kmalloc(KSH_INPUT_BUF_SIZE);
    // reset input buffer head
    inputbuf_head = 0;

    // reset our input buffer
    kmemset(inputbuf, 0, KSH_INPUT_BUF_SIZE);
    // set our keypress callback
    keyboard_set_notify_cb(notify_cb);

    // write initial prompt
    tty_writecolor("> ", VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
}
void ksh_poll_serial()
{
    if (serial_received())
    {
        char c = inb(PORT);

        if (inputbuf_head < (KSH_INPUT_BUF_SIZE - 1))
        {
            if (c == '\r' || c == '\n')
            {
                tty_write("\n");
                process_cmd();
                tty_writecolor("> ", VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
            }
            else if (c == '\b' || c == 0x7f)
            {
                if (inputbuf_head > 0)
                {
                    inputbuf[--inputbuf_head] = 0;

                    write_serial('\b');
                    write_serial(' ');
                    write_serial('\b');
                    tty_putc_relative('\0', -1, 0, TRUE);
                }
            }
            else
            {
                inputbuf[inputbuf_head++] = c;
                tty_putc(c);
            }
        }
    }
}

void
ksh_deinit()
{
    kfree(inputbuf);
}
