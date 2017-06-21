/* $Id: dev_flash.c,v 3.1.58.1 1996/03/18 18:34:46 gstovall Exp $
 * $Source: /release/111/cvs/Xboot/src-68-c7000/dev_flash.c,v $
 *------------------------------------------------------------------
 * Routines for reading RP1 internal flash and simple file system
 *
 * December 1994, Szewei Ju
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dev_flash.c,v $
 * Revision 3.1.58.1  1996/03/18  18:34:46  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.26.1  1996/03/16  06:10:29  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.1  1995/11/09  09:10:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.2  1995/06/07 09:05:06  smackie
 * Fold back Arizona_branch into mainline.
 *
 * Revision 1.1.4.1  1995/05/02  23:22:25  sthormod
 * Branch: Arizona_branch
 * Rom monitor support for RP1 flash credit card
 *
 * Revision 1.1  1995/04/23  00:22:29  sthormod
 * Placeholders for Arizona branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../defs.h"
#include "../stdio.h"
#include "../reason.h"
#include "../extern.h"
#include "dev_flash.h"
#include "../../sys/hes/flash_rp1.h"
#include "../../sys/dev/flash_dvr_spec.h"


ulong flash_align (ulong addr)
{
    return ((addr + (FLASH_WIDTH-1)) & ~(FLASH_WIDTH-1));
}

inline void flash_print_dirent (flash_fhdr *file)
{
    int i;

    i = printf("%-10d bytes (%#x)", file->ffh_len, file->ffh_len);
    while (i++ < 30) putchar(' ');  /* pad with spaces */
    printf("0x%8x  ", file->ffh_checksum);
    printf("%s", file->ffh_name);
    if (!(file->ffh_flags & FFH_FLAGS_DELETED)) printf(" (deleted)");
    putchar('\n');
}

void show_flash (boolean all)
{
    flash_header header;
    flash_fhdr   file_header;
    ulong addr;

    addr = FLASH_BASE;
    flash_long_copy(addr, (uchar *)&header, sizeof(flash_header), FALSE);

    if (header.fh_magic != FLASH_HDR_MAGIC) {
	printf("Bad flash magic %x, assuming invalid file system\n", 
	       header.fh_magic);
	return;
    }

    puts("         File size           Checksum     File name\n");
    addr += sizeof(flash_header);
    do {
	if (addr+sizeof(flash_fhdr) > FLASH_BASE + header.fhx_size) {
	    break;
	}
	flash_long_copy(addr, (uchar *)&file_header,
			sizeof(flash_fhdr), FALSE);
	if (file_header.ffh_magic != FLASH_FHDR_MAGIC) {
	    break;
	} 
	if (all || file_header.ffh_flags & FFH_FLAGS_DELETED)
	    flash_print_dirent(&file_header);

	addr += sizeof(flash_fhdr) + file_header.ffh_len;
	addr = flash_align(addr);
    } while (addr < (FLASH_BASE + header.fhx_size));
}


char *flash_find_file (uchar *filename)
{
    extern char common_dirent[];
    flash_header header;
    flash_fhdr   *file_header = (flash_fhdr *)&common_dirent;
    ulong addr, filecnt;
    uchar *name, *dirent;
    boolean first_file;
    boolean match;

    name = filename;
    first_file = match = FALSE;
    filecnt = 0;

    /*
     * use 1st valid file when no file is specified
     */
    if (!name || *name == '\0') 
	first_file = TRUE;

    addr = FLASH_BASE;
    flash_long_copy(addr, (uchar *)&header, sizeof(flash_header), FALSE);

    if (header.fh_magic != FLASH_HDR_MAGIC) {
	return FALSE;
    }

    bzero(file_header, sizeof(flash_fhdr));
    while (addr < (FLASH_BASE + header.fhx_size)) {
	addr += sizeof(flash_fhdr) + file_header->ffh_len;
	addr = flash_align(addr);

	/*
	 * check that we're not about to jump over the edge
	 */
	if ((addr + sizeof(flash_fhdr)) > (FLASH_BASE + header.fhx_size)) {
	    break;
	}

	flash_long_copy(addr, (uchar *)file_header,
			sizeof(flash_fhdr), FALSE);

	/*
	 * crude way of finding end of file system
	 */
	if (file_header->ffh_magic != FLASH_FHDR_MAGIC) {
	    break;
	} 

	/*
	 * skip deleted files
	 */
	if (!(file_header->ffh_flags & FFH_FLAGS_DELETED)) {
	    continue;
	}
	
	/*
	 * Skip it if we know it's a config file
	 */
	if (!(file_header->ffh_flags & FFH_FLAGS_SYS_CONFIG)) {
	    continue;
	}
		    
	if (first_file && filecnt == 0) 
	    match = TRUE;
	
	if (!match && strcmp(name, file_header->ffh_name) == 0) 
	    match = TRUE;
	
	if (match) {
	    dirent = (uchar *)file_header;
	    return dirent;
	}
	filecnt++;
    } 

    return NULL;
}

ulong flash_file_size (flash_fhdr *de)
{
    if (!de)
	return 0;

    if (de->ffh_magic != FLASH_FHDR_MAGIC) 
	return 0;
    
    return de->ffh_len;
}

ushort flash_file_cksum (flash_fhdr *de)
{
   if (!de)
	return 0;

   if (de->ffh_magic != FLASH_FHDR_MAGIC) 
       return 0;

    return de->ffh_checksum;
}


char *flash_file_name (flash_fhdr *de)
{
    if (!de)
	return NULL;

    if (de->ffh_magic != FLASH_FHDR_MAGIC) 
	return NULL;

    return de->ffh_name;
}

boolean flash_copy_to_ram (uchar *de, ulong ram_addr, ulong size)
{
    volatile ulong *p1, *p2;
    ulong i, words;
    ulong flash_addr;
    flash_fhdr *ffh = (flash_fhdr *)de;
    ushort cksum;
    
    if (ffh->ffh_magic != FLASH_FHDR_MAGIC) {
	return FALSE;
    } 

    flash_addr = FLASH_BASE + (ulong)ffh->ffh_addr;

    flash_long_copy(flash_addr, (uchar *)ram_addr, size, TRUE);

    cksum = 0;
    cksum = update_cksum(cksum, (uchar *)ram_addr, size);
    cksum = ~cksum;
    if (cksum != ffh->ffh_checksum) {
	printf("\nChecksum mismatch, 0x%x != 0x%x\n", 
	       cksum, ffh->ffh_checksum);
	return FALSE;
    }

    return TRUE;
}

void flash_long_copy (ulong addr, uchar *dest, ulong size, boolean verbose)
{
    ulong *p1, *p2;
    ulong words;
    ulong i;

    p1 = (ulong *)addr;
    p2 = (ulong *)dest;
    words = size/4;
    if (size%4)
	words++;
    *p1 = FLASH_RESET;
    delay(10);

    for (i = 0; words > 0; words--) {
        if ((ulong)p1 % 0x10000 == 0) {
            *p1 = FLASH_RESET;
            delay(10);
        }
	if (i++ == (FC_READ_LEN/4)) {
	    if (verbose) putchar('R');
	    i = 0;
	}
	*p2++ = *p1++;
    }
}

inline ushort add_to_cksum(ushort new_word, ushort old_cksum)
{
    ulong work;                       /* work value for end-around add */

    work = old_cksum + new_word;      /* add new word to checksum */
    if (work & ~0xFFFF) 
        work++;	                      /* end-around carry */   
    return(work & 0xFFFF);
}

ushort update_cksum(ushort cksum, uchar *wp, ulong i)
{
    if ((ulong)wp & 0x01 ) {
        cksum = add_to_cksum(*wp++, cksum);
        i--;
    } 

    for ( ; i > 1; i -= sizeof(ushort)) 
	cksum = add_to_cksum(*((ushort *)wp)++, cksum);
    
    if (i == 1) 
	cksum = add_to_cksum(*wp << 8, cksum);
    return(cksum);
}
