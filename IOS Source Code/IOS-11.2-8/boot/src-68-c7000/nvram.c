/* $Id: nvram.c,v 3.1 1995/11/09 09:10:25 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c7000/nvram.c,v $
 *------------------------------------------------------------------
 * Support for nvram environment variables on the 70x0 RP
 *
 * January 1995, Stig Thormodsrud
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nvram.c,v $
 * Revision 3.1  1995/11/09  09:10:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:17:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.2  1995/06/07 09:05:17  smackie
 * Fold back Arizona_branch into mainline.
 *
 * Revision 1.1.4.1  1995/05/02  23:22:38  sthormod
 * Branch: Arizona_branch
 * Rom monitor support for RP1 flash credit card
 *
 * Revision 1.1  1995/04/23  00:22:49  sthormod
 * Placeholders for Arizona branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Warning to those who would tread here: nvram on the RP can ONLY be
 * accessed 8 bits at a shot.  Therefore beware of using bcopy().  
 * Included below are wrapper routines for long & short access.
 */

#include "../defs.h"
#include "../stdio.h"
#include "../reason.h"
#include "../extern.h"
#include "nvmonvars.h"

struct nvram *nvram = (struct nvram *)ADRSPC_NVRAM_VARS;

char evarinit[] = {
    "BOOTLDR\0\0"
    "BOOT\0\0"
    "CONFIG_FILE\0\0"
    "\0"
};

/* 
 * Forward reference
 */
void movbyte(uchar *, uchar *, long);
boolean verify_nvram(struct nvram *);
ushort nv_read_short(ushort *);
ushort nv_write_short(ushort *, ushort);
ulong nv_read_long(ulong *);
ulong nv_write_long(ulong *, ulong);
int chksumnv(struct nvram *);
short chksum(ushort *, int);
void nvram_reset(void);


#define NVCHKSUMSTART(P)  (&P->magic)
#define NVCHKSUMSIZE(P)   ((long)&P->chksum - (long)&P->magic + sizeof(P->chksum))
#define NVCHKSUM(Q)  (chksum((unsigned short *)NVCHKSUMSTART(Q), NVCHKSUMSIZE(Q)))


inline long nv_get_magic (void)
{
    return (long)nv_read_long(&(nvram->magic));
}

void nvraminit (boolean startup)
{
    if (!verify_nvram(nvram)) {
	printf("Initializing nvram environment variables\n");
	nvram_reset();
    }
    if (startup) {
	reset_autoboot();
    }
}

ulong nv_read_long (ulong *address)
{
    ulong value;
    
    movbyte((uchar *)address, (uchar *)&value, sizeof(ulong));
    return value;
}

ulong nv_write_long (ulong *address, ulong value)
{
    ulong old_value;

    old_value = nv_read_long(address);
    movbyte((uchar *)&value, (uchar *)address, sizeof(ulong));
    return old_value;
}


ushort nv_read_short (ushort *address)
{
    ushort value;

    movbyte((uchar *)address, (uchar *)&value, sizeof(ushort));
    return value;
}

ushort nv_write_short (ushort *address, ushort value)
{
    ushort old_value;

    old_value = nv_read_short(address);
    movbyte((uchar *)&value, (uchar *)address, sizeof(ushort));
    return old_value;
}

void savenv (void)
{
    chksumnv(nvram);
    return;
}

void nvram_reset (void)
{
    ushort cksum;

    bzero((uchar *)nvram, sizeof(nvram));
    movbyte(evarinit, &nvram->evartbl[0], sizeof(evarinit));
    nv_write_long(&nvram->magic, NVMAGIC);
    savenv();
}

boolean nvram_ok (void)
{
    return verify_nvram(nvram);
}

boolean verify_nvram (struct nvram *nvptr)
{
    long magic = nv_get_magic();

    if (magic != NVMAGIC) {
	printf("Bad nvram magic %x\n", magic);
	return FALSE;
    }
    if (NVCHKSUM(nvptr) != 0) {
	printf("\nBad nvram checksum\n");
	return FALSE;
    }
    return TRUE;
}

void movbyte (uchar *base, uchar *dest, long size)
{
    long local_size = 0;

    while (local_size++ < size)
	*dest++ = *base++;
}

/*
 * Checksum the nvram structure
 * Note that the entire structure is not checksummed, only
 * the area from nvptr->magic to nvptr->chksum inclusive
 */
int chksumnv (struct nvram *nvptr)
{
    short checksum = 0;

    /* zero the chksum field in the structure */
    nv_write_short(&nvptr->chksum, checksum);
    /* calculate the checksum */
    checksum = -chksum((unsigned short *)NVCHKSUMSTART(nvptr), NVCHKSUMSIZE(nvptr));
    /* write the checksum */
    nv_write_short(&nvptr->chksum, checksum);    
    /* finally, recheck it to make sure it came out right */
    if (NVCHKSUM(nvptr) != 0)
	return(-1);
    return(0);
}

short chksum (unsigned short *addr, int size)
{
    register short chksum = 0;

    if (size & 1) {
	printf("cannot checksum odd number of bytes\n");
	return(-1);
    }
    while (size) {
	chksum += nv_read_short(addr);
	addr++;
	size -= 2;
    }
    return(chksum);
}
