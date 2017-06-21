/* $Id: flash2.c,v 1.1.4.1 1996/05/09 14:54:20 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cbiflash/flash2.c,v $
 *------------------------------------------------------------------
 * flash2.c
 * This file contains routines to reprogram 28F008 FLASH ROMs.
 * They accept data in packets, and remember which flash address
 * was last programmed. Incoming packets are assumed to be in the
 * proper sequence.
 *
 * March 1995, rfa
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash2.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:20  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:45  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:25  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:09:46  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:45  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:16  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.2.6.1  1996/01/23  08:49:28  rlowe
 * Suppress excessive trace and propeller output from code image
 * download and flash burn routines.
 *
 * CSCdi46640:  Excessive console output from ACP flash driver.
 * Branch: ATM21_branch
 *
 * Revision 1.2  1995/10/04  01:07:50  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.1  1995/07/27  09:02:17  rlowe
 * Restructure flash driver so that existing APIs and functionality are
 * preserved, while allowing "printf" and "yield_cpu" to be passed in to
 * the flash driver as function pointers.  This allows the exploder
 * (which uses the flash driver) to be truly independent of the Boot/Kernel
 * context (the caller provides "printf" and "yield_cpu" instead of linking
 * them in directly).
 *
 * Revision 1.6  1995/06/08  21:28:12  rfa
 * New update flash drivers
 *
 * Revision 1.4  1995/06/02  02:24:12  rlowe
 * Make CBI/flash (target) code portable between NMP and ACP.
 *
 * Revision 1.3  1995/05/30  21:15:38  rfa
 * updated flash.c to handle new driver interface
 *
 * Revision 1.2  1995/05/24  00:15:41  rfa
 * New AMD flash driver and FLASH device interface
 *
 * Revision 1.1  1995/05/18  00:59:27  rfa
 * Moved from nmpflash/syndnld
 *
 * Revision 1.8  1995/05/10  17:10:14  rfa
 * New startup sequence in mcpdnld.c and all printfs ifdef'd DEBUG
 *
 * Revision 1.7  1995/04/19  02:32:10  rfa
 * Temporarily removed the flash type ID string.
 *
 * Revision 1.6  1995/04/17  21:40:03  rfa
 * To support prompt bug fix
 *
 * Revision 1.5  1995/04/17  20:35:48  rfa
 * printf() cleanup
 *
 * Revision 1.4  1995/04/17  16:57:44  rfa
 * New support
 *
 * Revision 1.3  1995/04/07  17:24:29  rfa
 * support for new erase commands
 *
 * Revision 1.2  1995/03/23  15:40:38  rfa
 * Bug fixes
 *
 * Revision 1.1  1995/03/16  16:59:24  rfa
 * Moved from nmpboot/system
 *
 * Revision 1.10  1995/03/15  16:38:59  rfa
 * Removed old stuff.
 *
 * Revision 1.9  1995/03/09  18:34:52  rfa
 * These files reflect changes necessary to support the Combined Binary
 * Linked image downloads.
 *
 * exploder.c: parses downloaded image and records each module to flash.
 * extract.c: parses flash and copies module to ram for execution.
 * libcom.c: common functions for use by extract and exploder.
 * oldflash.c: the old flash driver.
 * flash.c: the new flash driver.
 *
 * Revision 1.4  1995/01/19  04:02:53  umahajan
 * Fixed setting of FLASH_WR_ENB bit before writing to flash.
 *
 * Revision 1.3  1995/01/16  22:22:34  jlockhar
 * NMPFLASH code freeze.
 *

 *------------------------------------------------------------------
 * $Endlog$
 */

#include "cbi_flsh_plat.h"
#include "flash.h"


extern fdev_t intelflash, amdflash;

/*
 * Null terminated array of pointers to individual
 * flash device structures maintained within the
 * source file of each flash driver.
 */
static fdev_t *devtab[] =
{
	&intelflash, &amdflash, (fdev_t *)0
};

static fdev_t *flashdevice = (fdev_t *)0;	/* Uninitialized dev pointer */


int load2(block, p, len, pPrt, pYield)
   int block;
   u_long *p;
   int len;
   int (*pPrt)() ;
   void (*pYield)() ;
{
	int nblocks;
	fdev_t **devtabp;

	if (!flashdevice)
	{
		for (devtabp = devtab; *devtabp; ++devtabp)
		{
			if ((*(*devtabp)->query)(*devtabp, pPrt) == (*devtabp)->id)
			{
				flashdevice = *devtabp;
				(*flashdevice->alert)(flashdevice, pPrt);
				break;
			}
		}
		if (!*devtabp)
		{
			if (pPrt)
			{
				(*pPrt)("*** FLASH DEVICE NOT DETECTED ");
				(*pPrt)("OR UNSUPPORTED ***\n");
			}
			return(0);
		}
	}
	wrenable();
	(*flashdevice->reset)(*devtabp, pPrt);

	/*
	 * Erase the flash memory from the caller sepcified block
	 * number to the number of blocks that will be written to
	 * as a result of writin 'len' bytes of data.
	 */
	nblocks = (len / FBLKSIZ) + ((len % FBLKSIZ) ? 1 : 0);
	if (erase2(block, nblocks, pPrt, pYield) != ENOERR)
	{
		if (pPrt)
			(*pPrt)("Erase failed\n");
		CBIF_RecordPgmFailure () ;
		wrdisable();
		return(0);
	}
	if (pPrt)
		(*pPrt)("Erase done\n");
	wrenable();
	if (pPrt)
		(*pPrt)("Programming Flash: ");
	while (nblocks--)
	{
		if (record2(block, p, (len / FBLKSIZ ? FBLKSIZ : len), pPrt, pYield) == EPROG)
			CBIF_RecordPgmFailure () ;
		len -= FBLKSIZ;
		p = (u_long *)(((u_long)p) + FBLKSIZ);
		++block;
	}
	if (pPrt)
		(*pPrt)(" Flash Programming Complete\n");
	(*flashdevice->reset)(flashdevice, pPrt);
	wrdisable();
	return(1);
}

/*
 * Sets up programming variables.
 */
int erase2(block, count, pPrt, pYield)
   int block;
   int count;
   int (*pPrt)() ;
   void (*pYield)() ;
{
	u_long *segment;
	int limit;
	fdev_t **devtabp;

	if (!flashdevice)
	{
		for (devtabp = devtab; *devtabp; ++devtabp)
		{
			if ((*(*devtabp)->query)(*devtabp, pPrt) == (*devtabp)->id)
			{
				flashdevice = *devtabp;
				(*flashdevice->alert)(flashdevice, pPrt);
				break;
			}
		}
		if (!*devtabp)
		{
			if (pPrt)
				(*pPrt)("*** NO FLASH DRIVER OR DEVICE FOUND ***\n");
			return(0);
		}
	}

	wrenable();
	/*
	 * Clear out our 64kbyte block per each
	 * of the 4 devices in parallel.
	 */
#ifdef DEBUG
	if (pPrt)
		(*pPrt)("erase(b=%d, c=%d): block(s):", block, count);
#else /* DEBUG */
	if (pPrt)
		(*pPrt)("Flash erase in progress ... ");
#endif /* DEBUG */
	for (limit = block + count; (block < 16) && (block < limit); ++block)
	{
		segment = (u_long *)(FLASH_START + (block * FBLKSIZ));
		if ((*flashdevice->erase)(flashdevice, segment, pPrt, pYield) != ENOERR)
		{
			if (pPrt)
				(*pPrt)("FATAL: erase failed on block %d", block);
			wrdisable();
			return(EERASE);
		}
#ifdef DEBUG
		if (pPrt)
			(*pPrt)(" %d", block);
#endif /* DEBUG */
	}
#ifdef DEBUG
	if (pPrt)
		(*pPrt)(" ");
#endif /* DEBUG */
	wrdisable();
	return(ENOERR);
}

/*
 * This routine takes a buffer and programs it into the flashes.
 */
int record2(block, p, len, pPrt, pYield)
   int block;
   u_long *p;
   int len;
   int (*pPrt)() ;
   void (*pYield)() ;
{
	static char propeller[] = "|/-\\";
	u_long *segment, *base;
	u_long status;
	int timer;

	if (pYield)
		(*pYield)() ;
	/*
	 * Program longs - 4 bytes into 4 devices in parallel.
	 */
	base = segment = (u_long *)(FLASH_START + (block * FBLKSIZ));
	for (len = (len + sizeof(long) - 1) / sizeof(long); len; --len)
	{
		/*
		 * 'segment' is the destination within FLASH.
		 * '*p' is the value within RAM to write.
		 * Check if all FF's then no need to program.
		 */
		if (*p != 0xFFFFFFFFL &&
		    (*flashdevice->program)(flashdevice, segment, *p, pPrt) != ENOERR)
		{
			if (pPrt)
				(*pPrt)("\ns=%#x *p=0x%08x ", segment, *p);
			return(EPROG); 
		}
		if (!(*((long *)&segment) & 0xFFFL))
		{
			if ((pPrt) && (FLSH_SHOW_PROPELLER))
			    (*pPrt)("%c%c",
				propeller[(((u_long)segment) >> 12) & 3L], 8);
			if (pYield)
				(*pYield)() ;
		}
		++segment, ++p;
	}
	return(ENOERR);
}
