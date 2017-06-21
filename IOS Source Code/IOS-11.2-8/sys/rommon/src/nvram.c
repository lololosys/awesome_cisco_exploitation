/* $Id: nvram.c,v 3.2.58.1 1996/03/21 23:27:44 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/nvram.c,v $
 *------------------------------------------------------------------
 * nvram.c
 *
 * March 1993, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Routines for the maintenence of ENV non-volatile RAM.
 *------------------------------------------------------------------
 * $Log: nvram.c,v $
 * Revision 3.2.58.1  1996/03/21  23:27:44  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1995/12/05  23:12:20  mbeesley
 * Branch: ELC_branch
 * Add complete support for discontigous NVRAM.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:51  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:42:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** Routines for the maintenence of ENV non-volatile RAM.
*/
#include "monitor.h"
#include "eeprom.h"
#include "confreg.h"
#include "signal_4k.h"

extern int netflashbooted;

struct nvram nvram;  /* temporarily */
short chksum(unsigned short *addr, int size);

static char evarinit[] = {
    "PS1\0rommon ! > \0\0"
};

static char aliasinit[] = {
  "r\0repeat\0h\0history\0?\0help\0b\0boot\0ls\0dir\0i\0reset\0k\0stack\0\0"
};

void
nvraminit(void)
{
    struct nvram *nvptr = NVRAM;

    /* read entire nvram structure into core */
    movbyte((unsigned char *)NVRAM_SAV, (unsigned char *)nvptr, 
	    sizeof(struct nvram));
    if(nvramvalid()) {
	nvptr->testarea = 0x0;
	DIAGFLAG = 0x0;
    } else {
	printf("\nWarning: monitor nvram area is corrupt ... "
	       "using default values\n");
	filbyte((unsigned char *)nvptr, sizeof(struct nvram), 0);
	nvptr->magic = NVMAGIC;
	MEMOP_TRIGGER = SCOPE_TRIGGER_ADDR;
	movbyte(aliasinit,ALIAS_TABLE,sizeof(aliasinit));
	movbyte(evarinit,nvptr->evartbl,sizeof(evarinit));
    }
    if(NV_VALID_CONFREG) nvptr->confreg = (NVRAM_SAV)->confreg;
    else nvptr->confreg = DEFAULT_CONFIG;
}

int
sync(int argc, char *argv[])
{
    if(savenv() < 0) return(1);
    /* does the user wish to save the config reg too */
    if(argc > 1 && (strcmp(argv[1], "-c") == 0)) {
	NV_WR_CONFREG((NVRAM)->confreg);
    }
    return(0);
}


/*
** Checksum the nvram structure
** Note that the entire structure is not checksummed, only
** the area from nvptr->magic to nvptr->chksum inclusive
** The following defines are for the next 3 routines
*/
#define NVCHKSUMSTART(P)  (&P->magic)
#define NVCHKSUMSIZE(P)   ((long)&P->chksum - (long)&P->magic + sizeof(P->chksum))

int
chksumnv(struct nvram *nvptr)
{
    short checksum = 0;

    /* zero the chksum field in the structure */
    if(nvwrite( &checksum, &nvptr->chksum, sizeof(nvptr->chksum)) < 0)
	return(-1);
    /* calculate the checksum */
    checksum = -chksum((unsigned short *)NVCHKSUMSTART(nvptr), NVCHKSUMSIZE(nvptr));
    /* write the checksum */
    if(nvwrite( &checksum, &nvptr->chksum, sizeof(nvptr->chksum)) < 0)
	return(-1);
    /* finally, recheck it to make sure it came out right */
    if(chksum((unsigned short *)NVCHKSUMSTART(nvptr), NVCHKSUMSIZE(nvptr)) != 0)
	return(-1);
    return(0);
}

/*
** Check the checksummed portion of NVRAM
*/
int
nvramvalid(void)
{
    struct nvram *nvptr = NVRAM_SAV;

#ifdef DEBUG
    printf("NVRAM magic number at location %#x is %#x\n",
	   &nvptr->magic, nvptr->magic);
#endif
    if(nvptr->magic == NVMAGIC &&
       (chksum((unsigned short *)NVCHKSUMSTART(nvptr), NVCHKSUMSIZE(nvptr)) == 0))
	return(1);
    return(0);
}

/*
** Save only the checksummed portion of the nvram structure
*/
int
savenv(void)
{
    struct nvram *nvptr = NVRAM_SAV;

    if(chksumnv(NVRAM) < 0) {
	puts("environment checksum failed\n");
	return(-1);
    }
    if(nvwrite(NVCHKSUMSTART(NVRAM), NVCHKSUMSTART(nvptr), NVCHKSUMSIZE(nvptr)) < 0) {
	puts("environment write to NVRAM failed\n");
	return(-1);
    }
    if(!nvramvalid()) {
	puts("environment checksum in NVRAM failed\n");
	return(-1);
    }
    return(0);
}

short
chksum(unsigned short *addr, int size)
{
    register short chksum = 0;

    if(size & 1) {
	printf("cannot checksum odd number of bytes\n");
	return(-1);
    }
    while(size) {
	chksum += *addr++;
	size -= 2;
    }
#ifdef DEBUG
    printf("checksum is: %#x\n", chksum);
#endif
    return(chksum);
}

/*
** nv_read_config:
** Return the config register in actual NVRAM
*/
ushort nv_read_config (void)
{
    return((NVRAM_SAV)->confreg);
}

/*
** nv_read_short:
** Read a signed short from NVRAM
*/
short nv_read_short (short *ptr)
{
    return(*ptr);
}

/*
** nv_read_ushort:
** Read an unsigned short from NVRAM
*/
ushort nv_read_ushort (ushort *ptr)
{
    return(*ptr);
}

/*
** nv_read_ptr:
** Read a pointer from NVRAM
*/
void *nv_read_ptr (ulong *ptr)
{
    return((void *)(*ptr));
}

/*
** This NVRAM write utility should work with NVRAM and
** EEPROM.  The routine nvread() was created to waste time
** between reads.
**
** The Xicor x28c256 EEPROM alows the writing of 64 locations
** at a time before polling for completion.  This "block write"
** must be within a 64 byte page boundary.
**
** For Sapphire we are using a part that has a block write size
** of 32 bytes.  This is not a great performance hit and will
** work in all cases, so this will be the [new] standard.
*/

unsigned char nvread (volatile unsigned char *location)
{
    return(*location);
}

nvwrite(source, dest, size)
    unsigned char *source, *dest;
{
    unsigned char data, *blkend, *end;
    int i;
    int retval = 0;
    SIG_PF sigsav;

    end = dest + size;
    /*
    ** This routine must not be interrupted by the user
    */
    sigsav = signal(SIGINT, SIG_IGN);  /* ignore user interrupts */
    WR_ENA_NVRAM;
    while(dest < end) {
	blkend = dest + EEPROM_BLOCKSIZ;
	/* make sure we don't cross block boundaries */
	blkend = (unsigned char *)((long)blkend & ~(EEPROM_BLOCKSIZ-1));
	if(blkend > end) blkend = end;
	while(dest < blkend) {  /* copy a block */
	    data = *source++;
	    *dest++ = data;
	}
	dest--;    /* compensate for above auto-increment */
	for(i=0; i<EEPROM_MAXLOOPS; i++)
	    if(nvread(dest) == data) break;
	if(*dest != data) {
	    retval = -1;
	    break;
	}
	dest++;    /* compensate for above compensation */
    }
    WR_PROT_NVRAM;
    signal(SIGINT, sigsav);
    return(retval);
}


