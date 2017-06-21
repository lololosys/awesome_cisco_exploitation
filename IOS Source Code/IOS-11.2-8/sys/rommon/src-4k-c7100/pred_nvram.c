/* $Id: pred_nvram.c,v 3.1.64.2 1996/07/18 17:48:58 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-c7100/pred_nvram.c,v $
 *------------------------------------------------------------------
 * pred_nvram.c - Predator Monitor NVRAM support
 *
 * November 1995, Michael Beesley
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pred_nvram.c,v $
 * Revision 3.1.64.2  1996/07/18  17:48:58  mbeesley
 * CSCdi63193:  Need support for rev2 gt64010CSCdi63193: Teach the rom
 * monitor
 * about the incompatibilities between rev1 and rev2 gt64010s :
 *         o Addressing flip for bytes accessed thru PCI IO space
 *         o Internal PCI config register addressing differences
 * Branch: California_branch
 *
 * Revision 3.1.64.1  1996/03/21  23:31:43  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.3  1996/02/26  18:43:32  szhang
 * CSCdi49349:  Predator ROM monitor needs to support packed NVRAM
 * Branch: ELC_branch
 *
 * Revision 3.1.2.2  1995/12/08  05:00:05  mbeesley
 * Branch: ELC_branch
 * Setup first 5 TLB entiries, init PCI, PCMCIA and ralib. Clean up
 * exception/interrupt handling, fix NVRAM handling.
 *
 * Revision 3.1.2.1  1995/12/05  23:12:29  mbeesley
 * Branch: ELC_branch
 * Add complete support for discontigous NVRAM.
 *
 * Revision 3.1  1995/12/05  17:20:51  mbeesley
 * Add placeholder file for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Routines for the maintenence of ENV non-volatile RAM on Predator
 */
#include "monitor.h"
#include "eeprom.h"
#include "confreg.h"
#include "signal_4k.h"
#include "proto.h"
#include "pred_plat_defs.h"

/*
 * Local storage
 */
struct nvram nvram;
static char evarinit[] = {
    "PS1\0rommon ! > \0\0"
};
static char aliasinit[] = {
  "r\0repeat\0h\0history\0?\0help\0b\0boot\0ls\0dir\0i\0reset\0k\0stack\0\0"
};

/*
 * Macro for deciding if an address is in NVRAM range or not
 */
#define NVRAM_ADDR_RANGE(addr) 				\
    (((ulong)addr >= KSEG1_ADRSPC_NVRAM) &&		\
     ((ulong)addr < KSEG1_ADRSPC_NVRAM_END))

#define C7100_NVRAM_BYTE_GAP 	1

/*
 * c7100_packed_nvram:
 * Return TRUE if we are running with packed NVRAM
 */
static boolean c7100_packed_nvram (void)
{
    return(KSEG1_PRED_FPGA->io_ctrl_reg & NVRAM_PACKED);
}

/*
 * c7100_nvram_ptr:
 * Return the address of a byte on NVRAM
 */
static volatile uchar *c7100_nvram_ptr (int offset)
{
    if (c7100_packed_nvram()) {
	return((volatile uchar *)(KSEG1_ADRSPC_NVRAM + offset));
    } else {
	return((volatile uchar *)(KSEG1_ADRSPC_NVRAM + (offset << 2)));
    }
}


/*
 * nv_read_ushort:
 * Read a ushort from NVRAM
 */
ushort nv_read_ushort (ushort *mem_ptr)
{
    volatile uchar *c_ptr;
    volatile ulong *l_ptr;
    ulong offset;
    ushort val;

    offset = (ulong)mem_ptr - KSEG1_ADRSPC_NVRAM;

    if (c7100_packed_nvram()) {
	c_ptr = c7100_nvram_ptr(offset);
	val = (*c_ptr << 8);            /* msb */
	c_ptr += C7100_NVRAM_BYTE_GAP;
	val |= (*c_ptr);
    } else {
	l_ptr = (volatile ulong *)c7100_nvram_ptr(offset);
	val = (*l_ptr & 0xff) << 8;    /* msb */
	l_ptr += C7100_NVRAM_BYTE_GAP;
	val |= (*l_ptr & 0xff);	       /* lsb */
    }
    return(val);
}

/*
 * nv_read_short:
 * Read a short from NVRAM
 */
short nv_read_short (short *mem_ptr)
{
    volatile uchar *c_ptr;
    volatile ulong *l_ptr;
    ulong offset;
    short val;

    offset = (ulong)mem_ptr - KSEG1_ADRSPC_NVRAM;
    
    if (c7100_packed_nvram()) {
	c_ptr = c7100_nvram_ptr(offset);
	val = (*c_ptr << 8);
	c_ptr += C7100_NVRAM_BYTE_GAP;
	val |= (*c_ptr);
    } else {
	l_ptr = (volatile ulong *)c7100_nvram_ptr(offset);
	val = (*l_ptr & 0xff) << 8;		/* msb */
	l_ptr += C7100_NVRAM_BYTE_GAP;
	val |= (*l_ptr & 0xff);		        /* lsb */
    }
    return(val);
}

/*
 * nv_read_ptr:
 * Read a pointer from NVRAM
 */
void *nv_read_ptr (void *mem_ptr)
{
    volatile uchar *c_ptr;
    volatile ulong *l_ptr;
    ulong offset;
    ulong val, i;

    offset = (ulong)mem_ptr - KSEG1_ADRSPC_NVRAM;

    if (c7100_packed_nvram()) {
	c_ptr = c7100_nvram_ptr(offset);
	for (i=0, val=0; i<4; i++) {
	    val = (val << 8) | (*c_ptr);
	    c_ptr += C7100_NVRAM_BYTE_GAP;
	}
    } else {
	l_ptr = (volatile ulong *)c7100_nvram_ptr(offset);
	for (i=0, val=0; i<4; i++) {
	    val = (val << 8) | (*l_ptr & 0xff);
	    l_ptr += C7100_NVRAM_BYTE_GAP;
	}
    }

    return((void *)val);
}

/*
 * chksum:
 * Run a checksum on some memory. If the address is in NVRAM range,
 * use the nv_read_ushort() routine.
 */
static short chksum (unsigned short *addr, int size)
{
    short temp_chksum = 0;

    if (size & 1) {
	printf("cannot checksum odd number of bytes\n");
	return(-1);
    }

    if (NVRAM_ADDR_RANGE(addr)) {
	while (size) {
	    temp_chksum += nv_read_ushort(addr++);
	    size -= 2;
	}
	return(temp_chksum);
    } else {
	while (size) {
	    temp_chksum += *addr++;
	    size -= 2;
	}
	return(temp_chksum);
    }
}

/*
 * nv_read_magic:
 * Read "magic" element of real NVRAM structure
 */
static long nv_read_magic (void)
{
    volatile uchar *c_ptr;
    volatile ulong *l_ptr;
    ulong offset;
    long magic, i;

    offset = (ulong)(&(NVRAM_SAV)->magic) - KSEG1_ADRSPC_NVRAM;

    if (c7100_packed_nvram()) {
	c_ptr = c7100_nvram_ptr(offset);
	for (i=0, magic=0; i<4; i++) {
	    magic = (magic << 8) | (*c_ptr);
	    c_ptr += C7100_NVRAM_BYTE_GAP;
	}
    } else {
	l_ptr = (volatile ulong *)c7100_nvram_ptr(offset);
	for (i=0, magic=0; i<4; i++) {
	    magic = (magic << 8) | (*l_ptr & 0xff);
	    l_ptr += C7100_NVRAM_BYTE_GAP;
	}
    }

    return(magic);
}

/*
 * nv_valid_confreg:
 * Return TRUE of the config register is valid. It is valid if the
 * magic is set to NVMAGIC and the config register is the 1's
 * complement of the reversed config register.
 */
int nv_valid_confreg (void)
{
    ushort cr, rcr;
    long magic;

    magic = nv_read_magic();
    if (magic != NVMAGIC) {
	return(0);
    }

    cr = nv_read_ushort(&(NVRAM_SAV)->confreg);
    rcr = nv_read_ushort(&(NVRAM_SAV)->rconfreg);
    if ((ushort)(~cr) != rcr) {
	return(0);
    }
    return(1);
}

/*
 * nv_read_config:
 * Return the config register saved in NVRAM
 */
ushort nv_read_config (void)
{
    return(nv_read_ushort(&(NVRAM_SAV)->confreg));
}

/*
 * Write the confreg value to NVRAM.  Do *not* update the
 * in-core copy.
 * The tmp_nv structure herein mirrors the relevant portion of the
 * actual nvram structure.
 */
void nv_wr_confreg (unsigned short val)
{
    struct {
	unsigned short confreg;
	unsigned short rconfreg;
	long magic;
    } tmp_nv;

    tmp_nv.confreg = val;
    tmp_nv.rconfreg = ~val;
    tmp_nv.magic = NVMAGIC;
    nvwrite((unsigned char *)&tmp_nv,
	    (unsigned char *)&(NVRAM_SAV)->confreg, sizeof(tmp_nv));
}

/*
 * nvraminit:
 * Initialize NVRAM support
 */
void nvraminit (void)
{
    struct nvram *nvptr = NVRAM;
    ushort *s_ptr, *d_ptr;
    int i;

    /* 
     * read entire nvram structure into core 
     */
    d_ptr = (ushort *)nvptr;
    s_ptr = (ushort *)(NVRAM_SAV);
    for (i=0; i<sizeof(struct nvram); i+=2) {
	*d_ptr = nv_read_ushort(s_ptr);
	d_ptr += 1;
	s_ptr += 1;
    }

    if (nvramvalid()) {
	nvptr->testarea = 0x0;
	DIAGFLAG = 0x0;
    } else {
	printf("\nWarning: monitor nvram area is corrupt ... "
	       "using default values\n");
	filbyte((unsigned char *)nvptr, sizeof(struct nvram), 0);
	nvptr->magic = NVMAGIC;
	MEMOP_TRIGGER = SCOPE_TRIGGER_ADDR;
	movbyte(aliasinit, ALIAS_TABLE, sizeof(aliasinit));
	movbyte(evarinit, nvptr->evartbl, sizeof(evarinit));
    }

    if (nv_valid_confreg()) {
	nvptr->confreg = nv_read_ushort(&(NVRAM_SAV)->confreg);
    } else {
	nvptr->confreg = DEFAULT_CONFIG;
    }
}

/*
 * sync:
 * Save monitor environment into NVRAM
 */
int sync (int argc, char *argv[])
{
    if (savenv() < 0) {
	return(1);
    }
    /* 
     * Does the user wish to save the config reg too ? 
     */
    if (argc > 1 && (strcmp(argv[1], "-c") == 0)) {
	nv_wr_confreg((NVRAM)->confreg);
    }
    return(0);
}

/*
 * Checksum the nvram structure
 * Note that the entire structure is not checksummed, only
 * the area from nvptr->magic to nvptr->chksum inclusive
 * The following defines are for the next 3 routines
 */
#define NVCHKSUMSTART(P)  (&P->magic)
#define NVCHKSUMSIZE(P)   \
    ((long)&P->chksum - (long)&P->magic + sizeof(P->chksum))

int chksumnv (struct nvram *nvptr)
{
    short checksum = 0;

    /* 
     * zero the chksum field in the structure 
     */
    if (nvwrite(&checksum, &nvptr->chksum, sizeof(nvptr->chksum)) < 0) {
	return(-1);
    }

    /* 
     * calculate the checksum 
     */
    checksum = -chksum((unsigned short *)NVCHKSUMSTART(nvptr), 
		       NVCHKSUMSIZE(nvptr));
    /* 
     * write the checksum 
     */
    if (nvwrite(&checksum, &nvptr->chksum, sizeof(nvptr->chksum)) < 0) {
	return(-1);
    }

    /* 
     * finally, recheck it to make sure it came out right 
     */
    if (chksum((unsigned short *)NVCHKSUMSTART(nvptr), 
	       NVCHKSUMSIZE(nvptr)) != 0) {
	return(-1);
    }
    return(0);
}

/*
 * nvramvalid:
 * Check the checksummed portion of NVRAM
 */
int nvramvalid (void)
{
    struct nvram *nvptr = NVRAM_SAV;
    long magic;

    magic = nv_read_magic();
    if ((magic == NVMAGIC) &&
	(chksum((unsigned short *)NVCHKSUMSTART(nvptr), 
		NVCHKSUMSIZE(nvptr)) == 0)) {
	return(1);
    }
    return(0);
}

/*
 * savenv:
 * Save only the checksummed portion of the nvram structure
 */
int savenv (void)
{
    struct nvram *nvptr = NVRAM_SAV;

    if (chksumnv(NVRAM) < 0) {
	puts("environment checksum failed\n");
	return(-1);
    }
    if (nvwrite(NVCHKSUMSTART(NVRAM), NVCHKSUMSTART(nvptr), 
		NVCHKSUMSIZE(nvptr)) < 0) {
	puts("environment write to NVRAM failed\n");
	return(-1);
    }
    if (!nvramvalid()) {
	puts("environment checksum in NVRAM failed\n");
	return(-1);
    }
    return(0);
}

/*
 * nv_write_byte:
 * Write one byte to the actual NVRAM
 */
static void nv_write_byte (uchar *mem_ptr, uchar value)
{
    volatile uchar *c_ptr;
    volatile ulong *l_ptr;
    ulong offset;

    offset = (ulong)mem_ptr - KSEG1_ADRSPC_NVRAM;

    if (c7100_packed_nvram()) {
	c_ptr = c7100_nvram_ptr(offset);
	*c_ptr = value;
    } else {
	l_ptr = (volatile ulong *)c7100_nvram_ptr(offset);
	*l_ptr = value;
    }
}

/*
 * nvwrite:
 * Write to NVRAM. Can be used to write to incore copy as well as
 * to actual NVRAM. Source should never be the actual NVRAM.
 */
int nvwrite (uchar *source, uchar *dest, int size)
{
    int i;
    SIG_PF sigsav;

    if (NVRAM_ADDR_RANGE(source)) {
	puts("Error: nvwrite() called with source in NVRAM");
	return(-1);
    }

    /*
     * This routine must not be interrupted by the user
     */
    sigsav = signal(SIGINT, SIG_IGN);  /* ignore user interrupts */
    if (NVRAM_ADDR_RANGE(dest)) {
	/*
	 * Writing to the real NVRAM
	 */
	for (i=0; i<size; i++) {
	    nv_write_byte(dest++, *source++);
	}
    } else {
	/*
	 * Writing to some incore copy. Just use movbyte
	 */
	movbyte(source, dest, size);
    }

    /*
     * Restore signal and return success
     */
    signal(SIGINT, sigsav);
    return(0);
}

/* end of file */



