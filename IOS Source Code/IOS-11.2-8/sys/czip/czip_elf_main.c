/* $Id: czip_elf_main.c,v 3.3.52.1 1996/03/18 19:11:07 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/czip/czip_elf_main.c,v $
 *------------------------------------------------------------------
 * czip_elf_main.c - Main module for self decompressing images
 *		     (ELF executables)
 *
 * May 1995, Michael Beesley
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: czip_elf_main.c,v $
 * Revision 3.3.52.1  1996/03/18  19:11:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.2  1996/03/07  08:42:14  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.20.1  1996/02/20  13:44:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/03  17:56:00  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.2  1995/11/17  08:58:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:15:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/23  18:33:37  mbeesley
 * CSCdi36332:  zipped r4k bootstrap images do not work
 *
 * Revision 2.1  1995/06/07  20:22:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Possible improvements :
 *
 * 1) bcopy :
 * bcopy is presently not very fancy although it does handle overlapping
 * addresses. However it is slow and does everything as bytes. It could
 * be sped up.
 *
 * 2) copy_and_launch :
 * It also does a slow byte-by-byte move of the image. This could be sped up.
 *
 */

/*
 * Flow of the program is following:
 * 
 * 1) Self decompressing image loaded into memory
 *
 * The rom monitor will load and position the self compressing image
 * at its link address in memory (either from a flash boot or a netboot)
 *
 * 2) Decompressing image initialization
 *
 * The entry code, (in czip_4k.S) saves the rom monitor
 * stack pointer, queries the rom monitor for main memory size
 * and relocates the stack to upper memory. The main routine
 * then checks to ensure that there is enough memory for the decompression.
 * The space required is calculated as the sum of program text,
 * compressed image size, stack space, copy_and_luanch() pic buffer,
 * and compression buffers.
 * 
 * 3) Checksum and move router image to high memory
 *
 * The embedded compressed router image is checksummed to ensure
 * no corruption has occured. It is then moved to high memory
 * and the bss section of the self extracting image is initialized
 *
 * 3) Decompress the router image
 * 
 * The router image is decompressed into memory at the end of the unzip
 * executables bss space. The decompressed image will grow upwards in 
 * memory. The size of the reulting uncompreesed image is checked to ensure
 * that decompression was completely successful. It is also checksummed
 * to ensure no corruption.
 *
 * 4) Copy the copy_and_launch() code relocation
 * 
 * The copy_and_launch() code is copied from unzip text space
 * to the memory buffer reserved for in in high memory. The caches are
 * then flushed.
 *
 * 5) Relocate compressed image and call
 *
 * copy_and_launch() is called (in the high memory buffer). It
 * moves the decompressed router image to the correct address,
 * flushes the caches, installs the original rom monitor stack
 * pointer and calls the image entry point
 *
 */

/*
 * Note : The bss segment is unusable by the unzip program until
 * after the compressed router image has been copied to high memory.
 */

#include "master.h"
#include "config_register.h"
#include "elf.h"

#include "../os/sum_magic_number.h"
#include "../dev/monitor1.h"
#include "../czip/czip.h"
#include "../os/gdb.h"
#include "../../boot/cpu.h"
#include "../../boot/emt.h"

/*
 * Typedefs
 */
typedef struct section_type_ {
    ulong source;
    ulong dest;
    ulong bytes;
} section_type;

typedef struct image_info_t_ {
    ulong entry_point;
    ulong section_count;
    ulong monstack;
    ulong monra;
    ulong param0;
    ulong param1;
    ulong param2;
    ulong param3;
    ulong reg_s0;
    ulong reg_s1;
    ulong reg_s2;
    ulong reg_s3;
    ulong reg_s4;
    ulong reg_s5;
    ulong reg_s6;
    ulong reg_s7;
    ulong reg_s8;
} image_info_t;

/*
 * Definitions
 */
#define MAX_SECTIONS	16

/*
 * Externals for routines and labels in asm files
 */
extern void copy_and_launch(section_type *, int, void *, ulong *);
extern void __start(void);

extern uchar pic_code_end[];
extern uchar _end[];

/*
 * Globals. Must be initialized as the bss section is unusable
 * until the compressed image has been copied to upper memory.
 */
ulong memsize = 0;
boolean debug = 0;
int cpu_type = 0;
ulong *monstack = 0;
int ciscopro_image = TRUE;      /* Allow czip to run on all platforms */
ulong monra = 0;
ulong param0 = 0;
ulong param1 = 0;
ulong param2 = 0;
ulong param3 = 0;
ulong reg_s0 = 0;
ulong reg_s1 = 0;
ulong reg_s2 = 0;
ulong reg_s3 = 0;
ulong reg_s4 = 0;
ulong reg_s5 = 0;
ulong reg_s6 = 0;
ulong reg_s7 = 0;
ulong reg_s8 = 0;

/*
 * cz_reload_system : Return to the monitor. Software forced reload
 */
void cz_reload_system (int reason)
{
    for (;;) {
	mon_cpu_exception(SIGRELOAD, reason, 0);
    }
}

/*
 * putstring : Print a string to the console
 */
void putstring (char *str)
{
    while (*str) {
	if (*str == '\n') {
	    emt_call(EMT_PUTCHAR, '\r');
	}
	emt_call(EMT_PUTCHAR, *str++);
    }
}

/*
 * puthex :
 */
static char hexstr[] = "0123456789ABCDEF";

void puthex(char *s, ulong hex)
{
    char buf[12];
    int i, j;

    putstring(s);
    buf[0] = '0';
    buf[1] = 'x';
    for (i=0; i<8; i++) {
	j = (hex >> ((8-(i+1)) * 4)) & 0x0f;
	buf[i+2] = hexstr[j];
    }
    buf[10] = '\n';
    buf[11] = 0;
    putstring(buf);
}
	
/*
 * bcopy : direction sensitive byte copy
 */
void bcopy (void const *src, void *dest, int bytes)
{
    uchar *sptr, *dptr;
    int i;

    if (src < dest) {

	/*
	 * Destination is higher. So copy backwards
	 */
	sptr = (uchar *)((ulong)src + bytes);
	dptr = (uchar *)((ulong)dest + bytes);
	for (i=0; i<bytes; i++) {
	    dptr--;
	    sptr--;
	    *dptr = *sptr;
	}
    } else {
	
	/* 
	 * Source is higher than destination. So copy forward
	 */
	sptr = (uchar *)src;
	dptr = (uchar *)dest;
	for (i=0; i<bytes; i++) {
	    *dptr++ = *sptr++;
	}
    }
}

/*
 * error : Issue an error message and sets the error flag
 */
void error (char *s)
{
    putstring("\nError : Uncompression of the image failed.\n");
    putstring(s);
    exit_code = CZIP_ERROR;
}

/*
 * __main : C++ junk
 */
void __main(void)
{}

/*
 * main - beginning of the world
 */
boolean main (boolean arg)
{
    ulong stack = (ulong)arg;
    int i, nwords, size;
    ushort config;
    ulong *ptr, magic, comp_size, uncomp_size, comp_checksum;
    ulong memory_needed, sum, *dest, post_uncomp_size, uncomp_checksum;
    uchar *cptr, *src, *bufp;
    void (*fptr)(section_type *, image_info_t *);
    Elf32_Ehdr *elf_hdr;
    Elf32_Shdr *sptr;
    image_info_t image_info;
    section_type sec_array[MAX_SECTIONS];
    
    /*
     * Read the config register and see if we are in diagnostic mode
     */
    i = mon_boot_confreg();
    if (i == -1) { 				/* old roms, can not do it */
	config = (ushort) ~mon_getconfig();
    } else {
	config = (ushort)i;
    }
    debug = (config & CFG_DIAGNOSTIC);

    /*
     * Get cpu_type from rom monitor
     */
    cpu_type = mon_proctype() & ~CPU_CISCOPRO_MASK;

    /*
     * Check our memory size and monstack
     */
    if (!memsize) {
	putstring("\nError : memory size reported as zero\n");
	cz_reload_system(1);
    }
    if (!monstack) {
	putstring("\nError : monitor stack pointer is zero\n");
	cz_reload_system(2);
    }
    if (debug) {
	puthex("Stack pointer       : ", stack);
	puthex("monstack            : ", (ulong)monstack);
	puthex("monra               : ", monra);
    }

    /*
     * Get some sizes. The glue utility sticks some magic at
     * the end of the unzip executables data section.
     */
    ptr = (ulong *)edata;
    magic = *ptr++;
    if (debug) {
	puthex("edata : ", (ulong)edata);
	puthex("magic : ", magic);
    }
    if (magic != MAGIC_VALUE) {
	putstring("\nError : glue magic numbers do not correspond\n");
	cz_reload_system(3);
    }
    uncomp_size = *ptr++;
    comp_size = *ptr++;
    comp_checksum = *ptr++;
    uncomp_checksum = *ptr++;

    if (debug) {
	puthex("memsize             : ", memsize);
	puthex("uncomp_size         : ", uncomp_size);
	puthex("comp_size           : ", comp_size);
	puthex("comp_checksum       : ", comp_checksum);
	puthex("uncomp_checksum     : ", uncomp_checksum);
    }

    /*
     * Make sure there is enough memory
     */
    memory_needed = uncomp_size + STACK_BYTES + COPY_CODE_BUF +
	(ulong)_end - R4K_K0BASE + CZIP_BUF_BYTES ;
    if (memory_needed > memsize) {
	putstring("\nError : memory requirements exceed available memory\n");
        puthex("Memory required     : ", memory_needed);
	cz_reload_system(4);
    }

    /*
     * Check the checksum
     */
    nwords = comp_size / sizeof(ulong);
    sum = 0;
    bufp = (uchar*) ptr;
    while (nwords--) {
	ADD_AROUND(*bufp++, sum);
    }
    if (sum != comp_checksum) {
	puthex("\nError : compressed image checksum is incorrect ", sum);
	puthex("        Expected a checksum of ", comp_checksum);
	cz_reload_system(5);
    }

    /*
     * Copy the compressed router image to high memory
     */
    dest = (ulong *)(R4K_K0BASE + memsize - STACK_BYTES - COPY_CODE_BUF - 
		     comp_size);
    dest = (ulong *)((ulong)dest & 0xfffffffc);
    if (dest < ((ulong *)_end + CZIP_BUF_BYTES + END_BUF_BYTES)) {
	putstring("\nError : decompression will overlap itself\n");
	cz_reload_system(6);
    }
    bcopy(ptr, dest, comp_size);

    /*
     * Zero the bss segment
     */
    for (cptr=(uchar *)edata; cptr < (uchar *)_end; ) {
	*cptr++ = 0;
    }

    /*
     * Uncompress the router image
     */
    src = (char *)dest;
    cptr = (char *)(((ulong)_end + END_BUF_BYTES) & 0xfffffffc);
    putstring("Self decompressing the image : ");
    uncompress(src, cptr, comp_size, &post_uncomp_size);
    if (exit_code != CZIP_OK) {
	putstring("\nError : zip decompress failed\n");
	cz_reload_system(7);
    }
    putstring(" [OK]\n");

    /*
     * Check that the decompressed image is the same size
     * as the original image
     */
    if (post_uncomp_size != uncomp_size) {
	putstring("\nError : pre and post compression image sizes disagree\n");
	cz_reload_system(8);
    }

    /*
     * Check that the decompressed image checksum checks out
     */
    nwords = uncomp_size / sizeof(ulong);
    sum = 0;
    bufp = (uchar *)cptr;

    while (nwords--) {
	ADD_AROUND(*bufp++, sum);
    }
    if (sum != uncomp_checksum) {
	puthex("\nError : uncompressed image checksum is incorrect ", sum);
	puthex("        Expected a checksum of ", uncomp_checksum);
	cz_reload_system(9);
    }

    /*
     * Check that the start address in the decompressed elf executable
     * header is the same as the start address of the self-decompressor
     */
    elf_hdr = (Elf32_Ehdr *)(cptr);
    if (elf_hdr->e_entry != (unsigned long)(&__start)) {
	putstring("\nError : image entry point does not equal to __start\n");
	cz_reload_system(10);
    }

    /*
     * Transfer the section entries from the elf executable
     * to the array on the stack for copy_and_launch() to work on.
     */
    for (i=0; i<MAX_SECTIONS; i++) {
	sec_array[i].source = 0;
	sec_array[i].dest = 0;
	sec_array[i].bytes = 0;
    }
    sptr = (Elf32_Shdr *)((ulong)cptr + elf_hdr->e_shoff);
    for (i=0; i<elf_hdr->e_shnum; i++) {
	sec_array[i].source = (ulong)cptr + sptr->sh_offset;
	sec_array[i].dest = sptr->sh_addr;
	sec_array[i].bytes = sptr->sh_size;
	sptr += 1;
    }

    /*
     * Relocate the copy_and_launch() code to its high memory buffer
     */
    dest = (ulong *)(R4K_K0BASE + memsize - STACK_BYTES - COPY_CODE_BUF);
    size = (ulong)pic_code_end - (ulong)copy_and_launch;
    bcopy(copy_and_launch, dest, size);

    /*
     * Flush the caches
     */
    cache_flush();

    /*
     * Call the copy_and_launch() code to move the image
     */
    image_info.entry_point = elf_hdr->e_entry;
    image_info.section_count = elf_hdr->e_shnum;
    image_info.monstack = (ulong)monstack;
    image_info.monra = monra;
    image_info.param0 = param0;
    image_info.param1 = param1;
    image_info.param2 = param2;
    image_info.param3 = param3;
    image_info.reg_s0 = reg_s0;
    image_info.reg_s1 = reg_s1;
    image_info.reg_s2 = reg_s2;
    image_info.reg_s3 = reg_s3;
    image_info.reg_s4 = reg_s4;
    image_info.reg_s5 = reg_s5;
    image_info.reg_s6 = reg_s6;
    image_info.reg_s7 = reg_s7;
    image_info.reg_s8 = reg_s8;

    fptr = (void (*)(section_type *, image_info_t *))(dest);

    if (debug) {
	puthex("cpu type                   : ", cpu_type);
	puthex("uncomp_size                : ", uncomp_size);
	puthex("monstack                   : ", (ulong)monstack);
	
        puthex("\nimage_info.entry_point   = ", image_info.entry_point);
        puthex("image_info.section_count = ", image_info.section_count);
        puthex("image_info.monstack      = ", image_info.monstack);
	puthex("image_info.monra         = ", image_info.monra);
        puthex("image_info.param0        = ", image_info.param0);
        puthex("image_info.param1        = ", image_info.param1);
        puthex("image_info.param2        = ", image_info.param2);
        puthex("image_info.param3        = ", image_info.param3);

	for (i=0; i<image_info.section_count; i++) {
	    puthex("Section Index = ", i);
	    puthex("    source    = ", sec_array[i].source);
	    puthex("    dest      = ", sec_array[i].dest);
	    puthex("    bytes     = ", sec_array[i].bytes);
	}
    }

    /*
     * Call copy_and_launch()
     */
    (*fptr)(&sec_array[0], &image_info);

    /*
     * This should never return
     */
    putstring("\nError : copy_and_launch() returned\n");
    for (;;) {
	cz_reload_system(11);
    }
    return(0);
}

/* end of file */
