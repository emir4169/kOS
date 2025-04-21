
#include "drivers/ramfs.h"
#include "kernel.h"
#include "drivers/tty.h"
#define bytes_per_line 16
void ramdisk_stub() {
    printk("RAMDISK STUB\n");
}
ramdisk_fs ramdisk = {0};
void ramfs_dump(ramdisk_fs fs){
    printk("ramfs_dump\n");
    ramdisk_dumpfiles(fs.files, fs.size);
};


void ramdisk_dumpfile(ramdisk_file_t file) {
    // Print file metadata
//    printk("RAMDISK FILE DUMP\n");
    printk("[\nName: %s\n", file.name);
    printk("Size: %d bytes\n", file.size);
    
    // Print hex/ASCII view of data
    printk("Data:\n");
    for (int i = 0; i < file.size; i += bytes_per_line) {
        
        // Hex view (16 bytes per line)
        for (int j = 0; j < bytes_per_line; j++) {
            if (i + j < file.size) {
                printk("%x ", file.data[i + j]);
            }
        }
        
        // ASCII view (print printable characters only)
        printk(" |");
        for (int j = 0; j < bytes_per_line; j++) {
            if (i + j >= file.size) break;
            
            unsigned char c = file.data[i + j];
            printk("%c", (c >= 32 && c <= 126) ? c : '.');
        }
        printk("|\n");
    }
    printk("]\n");
}
void ramdisk_dumpfiles(ramdisk_file_t *files, int count){
    printk("dumping files\n");
    for (int i = 0; i != count; i++)
    {
        ramdisk_dumpfile(files[i]);
    }
};
ramdisk_file_t ramdisk_lookup(ramdisk_fs filesystem, char *filename) {
    if (!filename || !filesystem.files) {
        ramdisk_file_t empty = {0};
        printk("ramdisk_lookup: file name is null or ramdisk is not initialized\n");
        return empty;
    }

    for (int i = 0; i < filesystem.size; i++) {
        if (kstrcmp(filesystem.files[i].name, filename) == true) {
            ramdisk_file_t ret = filesystem.files[i];
            printk("ramdisk_lookup: found file %s\n", ret.name);
            return ret;
        }
    }

    ramdisk_file_t empty = {0};
    printk("ramdisk_lookup: file %s not found\n", filename);
    return empty;
}

ramdisk_fs ramfs_init() {
    // Allocate memory for the filesystem structure
    ramdisk_fs* ramdisk = (ramdisk_fs*) kmalloc(sizeof(ramdisk_fs));
    
    // Create 2 files (adjust count as needed)
    ramdisk->size = 3;
    ramdisk->files = (ramdisk_file_t*) kmalloc(sizeof(ramdisk_file_t) * ramdisk->size);

    // File 1: "test.txt" with "Hello World"
    ramdisk->files[0].name = (char*) kmalloc(kstrlen("test.txt") + 1);
    kmemcpy(ramdisk->files[0].name, "test.txt", kstrlen("test.txt") + 1); // +1 for null terminator

    unsigned char* file1_data = (unsigned char*) kmalloc(12); // "Hello World" + null terminator
    kmemcpy(file1_data, "Hello World", 12);
    ramdisk->files[0].size = 11; // Exclude null terminator
    ramdisk->files[0].data = file1_data;

    // File 2: "hello.bin" with binary data
    ramdisk->files[1].name = (char*) kmalloc(kstrlen("hello.bin") + 1);
    kmemcpy(ramdisk->files[1].name, "hello.bin", kstrlen("hello.bin") + 1);

    unsigned char* file2_data = (unsigned char*) kmalloc(4);
    unsigned char temp_data[] = {0x01, 0x02, 0x03, 0x04};
    kmemcpy(file2_data, temp_data, 4);
    ramdisk->files[1].size = 4;
    ramdisk->files[1].data = file2_data;


    // file3: named "code.bin" with the "ret" instruction

    
    // thank god to https://defuse.ca/online-x86-assembler.htm
    unsigned char _temp_data[] ={ 0xB8, 0x03, 0x00, 0x00, 0x00, 0xBB, 0x09, 0x00, 0x00, 0x00, 0x89, 0xD9, 0xCD, 0x80, 0xC6, 0x03, 0x74, 0xC6, 0x43, 0x01, 0x65, 0xC6, 0x43, 0x02, 0x73, 0xC6, 0x43, 0x03, 0x74, 0xC6, 0x43, 0x04, 0x2E, 0xC6, 0x43, 0x05, 0x74, 0xC6, 0x43, 0x06, 0x78, 0xC6, 0x43, 0x07, 0x74, 0xB8, 0x00, 0x00, 0x00, 0x00, 0xCD, 0x80, 0xC6, 0x42, 0x01, 0x41, 0xB8, 0x01, 0x00, 0x00, 0x00, 0xCD, 0x80, 0xB8, 0x05, 0x00, 0x00, 0x00, 0xC3 }
    ;
    //unsigned char _temp_data[] = { 0xC3 }; // MOV EAX, 0x48; NOP; RET
    unsigned char* file3_data = (unsigned char*) kmalloc(sizeof(_temp_data));
    kmemcpy(file3_data, _temp_data, sizeof(_temp_data));
    ramdisk->files[2].size = sizeof(_temp_data);
    ramdisk->files[2].data = file3_data;
    ramdisk->files[2].name = (char*) kmalloc(kstrlen("code.bin") + 1);
    kmemcpy(ramdisk->files[2].name, "code.bin", kstrlen("code.bin") + 1);
    
    BOOT_LOG("RamDisk initialized.");
    return *ramdisk;
}