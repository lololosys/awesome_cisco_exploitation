/* $Id: nv_68k.c,v 3.3.60.2 1996/05/17 11:18:54 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/hes/nv_68k.c,v $
 *------------------------------------------------------------------
 * nv_68k.c - CPU dependent support for non-volatile configuration memory
 *
 * 30-January-1987, Kirk Lougheed
 *
 * Copyright (c) 1987-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nv_68k.c,v $
 * Revision 3.3.60.2  1996/05/17  11:18:54  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.48.2  1996/05/02  22:04:05  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.48.1  1996/04/03  14:31:27  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.60.1  1996/03/18  19:43:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  09:40:26  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/01/24  21:48:00  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  09:19:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:29:11  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:41:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:43:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "config.h"
#include "../hes/if_mci.h"
#include "subsys.h"
#include "../os/nv.h"

#define TESTPATTERN  0x89ABCDEFL

hwidbtype *nv_idbp;
boolean nv_writeflag;


/*
 * nv_init
 * Generic routine to initialize NVRAM support.
 * On AGS we do out initialization in the driver code.
 */

void nv_init (void)
{
    return;
}

/*
 * nv_hes_init
 * Initialize non-volatile memory support.
 * If Multibus memory is present, use it.
 * Only if Multibus memory is absent do we look for MCI applique memory.
 */
static void nv_hes_init (subsystype *subsys)
{
    leveltype status;
    mcitype *mci;
    mciregtype *regptr;
    ulong i, first, second;

    nvptr = NULL;		/* make sure unlocked */
    /*
     * Probe Multibus memory for an NV RAM card.  Size it.
     */
    i = 0;
    status = raise_interrupt_level(ALL_DISABLE);
    while (touch_device(&((uchar *)NVBASE)[i]) == TRUE) {
	i += 2;
	if (i >= NVMAXBYTES)
	    break;
    }
    nvsize = i;
    if (nvsize)
	nv_writeflag = FALSE;
    reset_interrupt_level(status);

    /*
     * Now that we have looked for Multibus Memory, go out and start
     * scanning MCI cards to see if there is an MC or MC+ applique with
     * good NV RAM.
     *
     * This is done by reading the first long of data, saving it, and
     * then writing a test pattern.  If we get our test pattern back,
     * then there is a non-volatile memory applique. Be careful to write
     * back the original data that was just clobbered in the test.  We
     * don't yet size memory.
     */
    nv_idbp = NULL;
    for (mci = (mcitype *) mciQ.qhead; mci; mci = mci->next) {
	regptr = mci->regptr;
	mci_memx_select(regptr, MCI_NVRAM_ADDRESS);
	first = regptr->memxlong;
	mci_memx_select(regptr, MCI_NVRAM_ADDRESS);
	regptr->memxlong = TESTPATTERN;
	mci_memx_select(regptr, MCI_NVRAM_ADDRESS);
	second = regptr->memxlong;
	if (second == TESTPATTERN) {
	    mci_memx_select(regptr, MCI_NVRAM_ADDRESS);
	    regptr->memxlong = first;
	    break;
	}
    }
    if (mci) {
	/*
	 * Find an interface to own this memory. However, don't use mci
	 * nvram if we have multibus memory already available.
	 */
	for (i = 0; i < mci->interface_count; i++)
	    if (mci->interface_list[i])
		break;
	if (!nvsize) {
	    nv_writeflag = FALSE;
	    nvsize = MCI_NVRAM_SIZE; 
	    nv_idbp = mci->interface_list[i];
	}
	flash_nv_idbp = mci->interface_list[i];
	return;
    }
}

/*
 * nv_getptr
 * Return pointer to base of non-volatile memory
 */

nvtype *nv_getptr (void)
{
    uchar *ptr;
    nvtype *nv;

    if (nvptr) {  /* Check lock */
	printf("\nNon-Volatile memory is in use");
	return(NULL);
    }
    /*
     * For Multibus memory, return base address
     */
    if (!nv_idbp) {
	if (nvsize) {
	    if (nv_check_getptr((nvtype *)NVBASE)) {
		return((nvtype *)NVBASE);
	    } else {
		return(NULL);
	    }
	}
	else {
	    return NULL;
	}
    }

    /*
     * For MCI applique memory, return a pointer to some shadow memory.
     */
    ptr = malloc(nvsize);
    if (!ptr) {
	printf(nomemory);
	return(NULL);
    }
    mci_mc_read_buffer(nv_idbp, MCI_NVRAM_ADDRESS, ptr, nvsize);
    nv = (nvtype *) ptr;
    if ((nv->magic == NVMAGIC) && !ipcrc((ushort *)nv, nvsize)) {
        /*
         * Good config in NVRAM. Adjust textbase to point to
         * our shadow NVRAM. Then recalculate checksum since
         * we are changing textbase.
         */
	nv->textbase += (ulong) ptr;	/* adjust to real pointer */
	nv->checksum = 0;
	nv->checksum = ipcrc((ushort *)nv, nvsize); /* fix checksum */
    }
    else {
        /*
         * Corrupted config in NVRAM. Adjust textbase anyway
         * since we have a shadow NVRAM.
         */
        nv->textbase += (ulong) ptr;
    }
    if (nv_check_getptr(nv)) {
	return(nv);
    } else {
	free(ptr);
	return(NULL);
    } 
}

/*
 * nv_badptr
 * Given a correctly checksummed portion of non-volatile memory, examine
 * the data structure components more closely for sanity.  Bad things happen
 * if, for example, we follow a CSC1 pointer on a CSC2 processor.
 */

boolean nv_badptr (nvtype *ptr)
{
    if (nv_idbp)			/* don't check if MCI applique */
	return(FALSE);
    if (ptr->version == NVCOMPRESSVERSION) {
	return(FALSE);
    } else {
	if (((long)ptr->textbase < NVBASE)||((long)ptr->textbase > NVBASE+nvsize))
	    return(TRUE);
	if (((long)ptr->textptr < NVBASE)||((long)ptr->textptr > NVBASE+nvsize))
	    return(TRUE);
	if (ptr->textsize > nvsize)
	    return(TRUE);
    }
    return(FALSE);
}

/*
 * nv_done
 * Handles writing configuration data to memory.
 * Special handling for MCI applique.  Does nothing for Multibus NVRAM.
 */

void nv_done (nvtype *ptr)
{
    if (nv_idbp) {
	if (nv_check_done(ptr) && nv_writeflag) {
	    ptr->textbase -= (ulong)nvptr;	 /* make into offset */
	    ptr->checksum = 0;
	    ptr->checksum = ipcrc((ushort *)ptr, nvsize);
	    mci_mc_write_buffer(nv_idbp, ptr, MCI_NVRAM_ADDRESS, nvsize);
	}
	nv_writedisable(ptr);
	free(ptr);
    } else {
	nv_check_done(ptr);
	nv_writedisable(ptr);
    }
	
    if (ptr == nvptr)		/* Free lock */
	nvptr = NULL;
}

/*
 * nv_writeenable
 * Get memory ready to be written
 */

void nv_writeenable (nvtype *ptr)
{
    nv_writeflag = TRUE;
}

/*
 * nv_writeenable
 * Done writing memory, do hardware dependent actions.
 */

void nv_writedisable (nvtype *ptr)
{
    nv_writeflag = FALSE;
}

/*
 * nv_write_summary - Write the configuration summary
 *
 * This routine is used to write the configuration summary software
 * configuration register.
 */

void nv_write_summary (boolean eraseflag)
{
}

/*
 * nv_bcopy:
 * Used to write text directly to NVRAM
 */
void
nv_bcopy(void const *src, void *dst, int length)
{
    bcopy(src, dst, length);
}


/*
 * NVRAM subsystem header
 *
 * The AGS has really weird places to stick NVRAM and FLASH.
 * It can be on MCI interfaces, or the ENVM card.
 * Oh, yes, and in multibus memory.
 */

#define NVRAM_MAJVERSION   1
#define NVRAM_MINVERSION   0
#define NVRAM_EDITVERSION  1

SUBSYS_HEADER(nvram, NVRAM_MAJVERSION, NVRAM_MINVERSION, NVRAM_EDITVERSION,
	      nv_hes_init, SUBSYS_CLASS_DRIVER,
	      "seq: cbus, mci, envm",
	      NULL);
