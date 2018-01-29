#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "osfmk/mach/machine.h"
#include "EXTERNAL_HEADERS/mach-o/loader.h"
#include <string.h>

int main()
{
    unsigned int code_bytes = 640 * 1024;
    uint8_t* mem = (uint8_t*) mmap(
            NULL,
            code_bytes,
            PROT_READ | PROT_WRITE | PROT_EXEC,
            MAP_ANONYMOUS | MAP_PRIVATE,
            0,
            0);
    uint64_t offset = 0;
    uint64_t data_offset = 0;
    uint64_t ncmds = 0;
    uint64_t vmaddr = 0x0000000100000000;
    uint64_t vmsize = 0x0000000000001000;

    struct mach_header_64 *header = (struct mach_header_64*)mem;
    header->magic = MH_MAGIC_64;
    header->cputype = (CPU_ARCH_ABI64 | CPU_TYPE_X86);
    header->cpusubtype = CPU_SUBTYPE_LIB64 | CPU_SUBTYPE_X86_64_ALL;
    header->ncmds = 0;
    header->sizeofcmds = 0;
    header->filetype = MH_EXECUTE;
    header->flags = MH_PIE | MH_TWOLEVEL | MH_PRELOAD;

    offset += sizeof(struct mach_header_64);

    struct segment_command_64 *zero = (struct segment_command_64*)(mem + offset);
    zero->cmd = LC_SEGMENT_64;
    zero->cmdsize = sizeof(struct segment_command_64);
    zero->vmsize = vmaddr;
    strcpy(zero->segname, SEG_PAGEZERO);
    offset += sizeof(struct segment_command_64);
    ncmds += 1;

    struct segment_command_64 *text = (struct segment_command_64*)(mem + offset);
    text->cmd = LC_SEGMENT_64;
    text->cmdsize = sizeof(struct segment_command_64);
    strncpy(text->segname, SEG_TEXT, 16);
    text->vmaddr = vmaddr;
    text->vmsize = vmsize;
    text->fileoff = 0;
    text->filesize = 4096;
    text->maxprot = 0x7;
    text->initprot = 0x5;
    uint32_t nsects = 0;
    text->nsects = nsects;
    text->flags = 0;

    offset += sizeof(struct segment_command_64);
    ncmds += 1;

    uint8_t prg[] = {
        0x48, 0xc7, 0xc0, 0x04, 0x00, 0x00, 0x02,
        0x48, 0xc7, 0xc7, 0x01, 0x00, 0x00, 0x00,
        0x48, 0x8d, 0x35, 0x19, 0x00, 0x00, 0x00,
        0x48, 0xc7, 0xc2, 0x0d, 0x00, 0x00, 0x00,
        0x0f, 0x05,
        0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x02,
        0x48, 0xc7, 0xc7, 0x00, 0x00, 0x00, 0x00,
        0x0f, 0x05,
        0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64, 0x0a, 0x00,
    };


    size_t prgsize = sizeof(prg);

    struct section_64 *text_sect = (struct section_64*)(mem + offset);
    strncpy(text_sect->sectname, SECT_TEXT, 16);
    strncpy(text_sect->segname, SEG_TEXT, 16);
    text_sect->addr = 0;
    text_sect->size = prgsize;
    text_sect->offset = 0;
    text_sect->align = 0;
    text_sect->flags = 0x80000400;

    offset += sizeof(struct section_64);
    data_offset += text_sect->size;
    nsects += 1;

    text->nsects = nsects;
    text->cmdsize += (sizeof(struct section_64) * nsects);

    struct segment_command_64 *linkedit = (struct segment_command_64*)(mem+offset);
    linkedit->cmd = LC_SEGMENT_64;
    linkedit->cmdsize = sizeof(struct segment_command_64);
    strncpy(linkedit->segname, SEG_LINKEDIT, 16);
    linkedit->vmaddr = 0;
    linkedit->vmsize = 4096;
    linkedit->fileoff = 4096;
    linkedit->filesize = 152;
    linkedit->maxprot = 7;
    linkedit->initprot = 1;

    offset += sizeof(struct segment_command_64);
    data_offset += 152;
    ncmds += 1;

    struct dyld_info_command *dyld_info_only = (struct dyld_info_command*)(mem+offset);
    dyld_info_only->cmd = LC_DYLD_INFO_ONLY;
    dyld_info_only->cmdsize = sizeof(struct dyld_info_command);
    dyld_info_only->export_off = 0x2000;
    dyld_info_only->export_size = 0x30;

    offset += sizeof(struct dyld_info_command);
    ncmds += 1;

    struct symtab_command *symtab = (struct symtab_command*)(mem + offset);
    symtab->cmd = LC_SYMTAB;
    symtab->cmdsize = sizeof(struct symtab_command);
    symtab->symoff = 0x2030;
    symtab->nsyms = 0;
    symtab->stroff = 0x2030;
    symtab->strsize = 0;

    offset += sizeof(struct symtab_command);
    ncmds += 1;

    struct dysymtab_command *dysymtab = (struct dysymtab_command*)(mem + offset);
    dysymtab->cmd = LC_DYSYMTAB;
    dysymtab->cmdsize = sizeof(struct dysymtab_command);
    dysymtab->nextdefsym = 0;
    dysymtab->iundefsym = 0;
    dysymtab->nundefsym = 0;

    offset += sizeof(struct dysymtab_command);
    ncmds += 1;

    struct dylinker_command* dylinker = (struct dylinker_command*)(mem + offset);
    dylinker->cmd = LC_LOAD_DYLINKER;
    dylinker->cmdsize = sizeof(struct dylinker_command) + 20;
    union lc_str name = {0x0c};
    dylinker->name = name;
    offset += sizeof(struct dylinker_command);
    strncpy((char*)(mem+offset), "/usr/lib/dyld", 20);

    offset += 20;
    ncmds += 1;

    struct entry_point_command *entry = (struct entry_point_command*)(mem + offset);
    entry->cmd = LC_MAIN;
    entry->cmdsize = sizeof(struct entry_point_command);
    entry->entryoff = 4016;
    entry->stacksize = 0;

    offset += sizeof(struct entry_point_command);
    ncmds += 1;

    struct dylib_command *dylib = (struct dylib_command*)(mem + offset);
    dylib->cmd = LC_LOAD_DYLIB;
    dylib->cmdsize = 56;
    union lc_str libname = {24};
    struct dylib lib = {
        .name = libname,
        .timestamp = 2,
        .current_version = 81148930,
        .compatibility_version = 65536
    };
    dylib->dylib = lib;

    offset += sizeof(struct dylib_command);

    strncpy((char*)mem+offset, "/usr/lib/libSystem.B.dylib", 32);

    offset += 32;
    ncmds += 1;

    header->ncmds = ncmds;
    header->sizeofcmds = offset - sizeof(struct mach_header_64);

    if (offset < 0xfb0) {
        offset += (0xfb0 - offset);
    }

    text_sect->addr = vmaddr + offset;
    text_sect->offset = offset;

    memcpy((mem+offset), prg, sizeof(prg));
    offset += text_sect->size;

    linkedit->fileoff = 0x2000;
    linkedit->vmaddr = vmaddr + linkedit->fileoff;


    char* filename = "foo";
    FILE *fp = fopen(filename, "wb");
    fwrite(mem, 1, code_bytes, fp);
    fclose(fp);
    chmod(filename, 0777);
    return 0;
}
