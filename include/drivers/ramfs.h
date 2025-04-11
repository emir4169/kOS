#include "kernel.h"
typedef struct ramdisk_file_t
{
    char *name;
    int size;
    unsigned char *data;
} ramdisk_file_t;


typedef struct ramdisk_fs
{
    ramdisk_file_t* files;
    int size;
} ramdisk_fs;
void ramdisk_dumpfile(ramdisk_file_t file);
void ramdisk_dumpfiles(ramdisk_file_t *files, int count);
ramdisk_file_t ramdisk_lookup(ramdisk_fs filesystem, char *filename);
void ramfs_dump(ramdisk_fs fs);
ramdisk_fs ramfs_init();
extern ramdisk_fs ramdisk;