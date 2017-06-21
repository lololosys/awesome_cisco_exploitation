/* $Id: czip_aout_main.c,v 3.3.52.1 1996/03/18 19:11:06 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/czip/czip_aout_main.c,v $
 *------------------------------------------------------------------
 * czip_aout_main.c - Main module for self decompressing images
 *		      (AOUT executables)
 *
 * December 1994, Michael Beesley
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: czip_aout_main.c,v $
 * Revision 3.3.52.1  1996/03/18  19:11:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.2  1996/03/07  08:42:12  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.20.1  1996/02/20  13:44:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/03  17:56:00  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.2  1995/11/17  08:58:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:15:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:22:13  hampton
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
 * The entry code, (in czip_68k.S) saves the rom monitor
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
#include "cisco_a.out.h"
#include "../os/sum_magic_number.h"
#include "../dev/monitor1.h"
#include "../czip/czip.h"
#include "../../boot/cpu.h"
#include "../../boot/emt.h"

/*
 * Externals for routines and labels in asm files
 */
extern void copy_and_launch(uchar *, uchar *, int, void *, int, ulong);
extern void cache_flush(void);

extern uchar edata[];
extern uchar pic_code_end[];
extern uchar _end[];

/*
 * Globals. Must be initialized as the bss section is unusable
 * untill the compressed image has been copied to upper memory.
 */
ulong memsize = 0;
boolean debug = 0;
int cpu_type = 0;
int ciscopro_image = TRUE;	/* Allow czip to run on all platforms */
ulong *monstack = 0;

/*
 * cz_reload_system : Return to the monitor. Software forced reload
 */
void cz_reload_system (void)
{
    for (;;) {
	crashpoint();
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
static const unsigned char hexstr[] = "0123456789ABCDEF";

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
    ulong *ptr, *current, magic, comp_size, uncomp_size, comp_checksum;
    ulong memory_needed, sum, *dest, post_uncomp_size, uncomp_checksum;
    ulong memory_in_use;
    struct exec *exec_ptr;
    uchar *cptr, *src;
    void (*fptr)(uchar *, uchar *, int, void *, int, ulong *);
    
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
	cz_reload_system();
    }
    if (!monstack) {
	putstring("\nError : monitor stack pointer is zero\n");
	cz_reload_system();
    }

    if (debug) {
	puthex("Stack pointer       : ", stack);
	puthex("monstack            : ", (ulong)monstack);
    }

    /*
     * Get some sizes. The glue utility sticks some magic at
     * the end of the unzip executables data section.
     */
    ptr = (ulong *)edata;
    magic = *ptr++;
    if (magic != MAGIC_VALUE) {
	putstring("\nError : glue magic numbers do not correspond\n");
	cz_reload_system();
    }
    uncomp_size = *ptr++;
    comp_size = *ptr++;
    comp_checksum = *ptr++;
    uncomp_checksum = *ptr++;

    /*
     * Make sure there is enough memory.
     * memory_in_use = unzip code + cziped image.
     * uncomp_size   = uncompressed size of cziped image.
     * CZIP_BUF_BYTES= buffers required for unzip
     * STACK_BYTES   = stack required by unzip code
     * COPY_CODE_BUF = position independent copy code size
     */
    memory_in_use = (ulong)_end - (ulong)RAMBASE;
    memory_needed = memory_in_use + uncomp_size + 
                    STACK_BYTES + COPY_CODE_BUF +
                    CZIP_BUF_BYTES;

    if (debug) {
	puthex("memsize             : ", memsize);
	puthex("uncomp_size         : ", uncomp_size);
	puthex("comp_size           : ", comp_size);
	puthex("comp_checksum       : ", comp_checksum);
	puthex("uncomp_checksum     : ", uncomp_checksum);
	puthex("memory in use	    : ", memory_in_use);
	puthex("total memory needed : ", memory_needed);
    }

    if (memory_needed > memsize) {
	putstring("\nError : memory requirements exceed available memory\n");
	puthex("Memory required     : ", memory_needed);
	cz_reload_system();
    }

    /*
     * Check the checksum
     */
    nwords = comp_size / sizeof(ulong);
    sum = 0;
    current = ptr;
    while (nwords--) {
	ADD_AROUND(*current++, sum);
    }
    if (sum != comp_checksum) {
	puthex("\nError : compressed image checksum is incorrect ", sum);
	puthex("        Expected a checksum of ", comp_checksum);
	cz_reload_system();
    }

    /*
     * Copy the compressed router image to high memory
     */
    dest = (ulong *)(RAMBASE + memsize - STACK_BYTES - COPY_CODE_BUF - 
		     comp_size);
    dest = (ulong *)((ulong)dest & 0xfffffffc);
    if (dest < ((ulong *)_end + CZIP_BUF_BYTES + END_BUF_BYTES)) {
	putstring("\nError : decompression will overlap itself\n");
	cz_reload_system();
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
	cz_reload_system();
    }
    putstring(" [OK]\n");

    /*
     * Check that the decompressed image is the same size
     * as the original image
     */
    if (post_uncomp_size != uncomp_size) {
	putstring("\nError : pre and post compression image sizes disagree\n");
	cz_reload_system();
    }

    /*
     * Check that the decompressed image checksum checks out
     */
    nwords = uncomp_size / sizeof(ulong);
    sum = 0;
    current = (ulong *)cptr;

    while (nwords--) {
	ADD_AROUND(*current++, sum);
    }
    if (sum != uncomp_checksum) {
	puthex("\nError : uncompressed image checksum is incorrect ", sum);
	puthex("        Expected a checksum of ", uncomp_checksum);
	cz_reload_system();
    }

    /*
     * Check that the start address in the decompressed aout
     * header is the same as out start address
     */
    exec_ptr = (struct exec *)(cptr);
    if (exec_ptr->a_entry != (unsigned long)(&_start)) {
	putstring("\nError : image entry point does not equal to _start\n");
	cz_reload_system();
    }

    /*
     * Relocate the copy_and_launch() code to its high memory buffer
     */
    dest = (ulong *)(RAMBASE + memsize - STACK_BYTES - COPY_CODE_BUF);
    size = (ulong)pic_code_end - (ulong)copy_and_launch;
    bcopy(copy_and_launch, dest, size);

    /*
     * Flush the caches
     */
    cache_flush();

    /*
     * Call the copy_and_launch() code to move the image
     */
    fptr = (void (*)(uchar *, uchar *, int, void *, int, ulong *))(dest);
    src = (uchar *)((ulong)cptr + sizeof(struct exec));

    if (debug) {
	puthex("source for copy_and_launch : ", (ulong)src);
	puthex("entry for copy_and_launch  : ", (ulong)(exec_ptr->a_entry));
	puthex("entry pointer              : ", (ulong)(exec_ptr));
	puthex("cpu type                   : ", cpu_type);
	puthex("uncomp_size                : ", uncomp_size);
	puthex("monstack                   : ", (ulong)monstack);
    }

    (*fptr)(src, (uchar *)(exec_ptr->a_entry), uncomp_size,
	    (void *)(exec_ptr->a_entry), cpu_type, monstack);

    /*
     * This should never return
     */
    putstring("\nError : copy_and_launch() returned\n");
    for (;;) {
	cz_reload_system();
    }
    return(0);
}

/* end of file */
