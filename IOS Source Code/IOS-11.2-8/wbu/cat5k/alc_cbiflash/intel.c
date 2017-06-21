/* $Id: intel.c,v 1.1.4.1 1996/05/09 14:54:22 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cbiflash/intel.c,v $
 *------------------------------------------------------------------
 * intel.c
 * This file contains routines to reprogram 28F008 FLASH ROMs.
 * They accept data in packets, and remember which flash address
 * was last programmed. Incoming packets are assumed to be in the
 * proper sequence.
 *
 * May 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: intel.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:22  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:46  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:27  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:09:47  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:46  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:17  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.8.8.1  1996/02/16  23:53:36  rlowe
 * Remove references to Catalyst 5000 from flash drivers.
 *
 * Revision 1.8  1995/10/04  01:07:51  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.7  1995/07/27  09:02:18  rlowe
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
 * Revision 1.4  1995/06/02  02:24:15  rlowe
 * Make CBI/flash (target) code portable between NMP and ACP.
 *
 * Revision 1.3  1995/05/30  21:39:39  sgarcia
 * Added last initializer to device structure
 *
 * Revision 1.2  1995/05/30  21:15:16  rfa
 * New intel driver
 *
 * Revision 1.1  1995/05/24  00:15:40  rfa
 * New AMD flash driver and FLASH device interface
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

#define FORWARD	/**/

FORWARD static u_char intelquery(fdev_t *, int (*)());
FORWARD static void intelreset(fdev_t *, int (*)());
FORWARD static int intelerase(fdev_t *, volatile u_long *, int (*)(), void (*)());
FORWARD static int intelprogram(fdev_t *, volatile u_long *, u_long, int (*)());
FORWARD static void intelalert(fdev_t *, int (*)());

#define INTELID		0x89			/* Intel Flash */

/*
 * This is the driver specific minor device structure support.
 */
typedef struct intel
{
	u_short		flags;
	char		*name;		/* Device name */
	int		nblks;		/* Number of blocks */
	long		csize;		/* Chip size in bytes */
	u_char		code;		/* Chip code */
} intel_t;

#define INTEL_T(f,nam,x,z,c)	{ (f), #nam, (x), ##z##L, (c) }

intel_t inteldevs[] =
{
	INTEL_T(0, INTEL28F001BX-T, 0, 131072, 0xA2),
	INTEL_T(0, INTEL28F008, 0, 131072, 0xA2),
	{ 0, (char *)0, 0, 0L, (u_char)0 }
};

fdev_t intelflash =
{
	FDEV_T(F_4MB, INTELID, 0, intel)
};

/*
 * Commands for the INTEL Flash RAM
 */
#define RESET		0xFFFFFFFF
#define RID		0x90909090
#define RSTATUS		0x70707070
#define CSTATUS		0x50505050
#define ERASE		0x20202020		/* Erase setup */
#define SUSPEND		0xB0B0B0B0
#define WRITE		0x40404040		/* Write setup */
#define ALTWRITE	0x10101010
#define CONFIRM		0xD0D0D0D0		/* Erase confirmation */
#define RESUME		0xD0D0D0D0

#define SR_VPPLOW	0x08080808
#define SR_WRITE	0x10101010
#define SR_ERASE	0x20202020
#define SR_CMDSEQ	(SR_ERASE | SR_WRITE)
#define SR_SUSP		0x40404040
#define SR_READY	0x80808080

static u_char intelquery(devp, pPrt)
   fdev_t *devp;
   int (*pPrt)() ;
{
	u_long id;
	u_long code;

	wrenable();
	*((u_long *)FLASH_START) = RID;
	usec10();			/* Recovery time before read */
	id = *((u_long *)FLASH_START);
	usec10();			/* Recovery time before read */
	code = *((u_long *)(FLASH_START | (1 << 2)));
	usec10();			/* Recovery time before read */
#ifdef COMMENT
	if (pPrt)
		(*pPrt)("intelquery: id=0x%08x code=0x%08x\n", id, code);
#endif
	wrdisable();
	return((u_char)id);
}

static void intelreset(devp, pPrt)
   fdev_t *devp;
   int (*pPrt)() ;
{
	*((u_long *)FLASH_START) = RESET;
}

/*
 * This routine erases the contents of a flash sector (64kbyte per device).  
 * This is a straightforward implementation of the flash erase algorithm 
 * given in the 28F008 Flash documentation.  
 *
 * NOTE: 64kbyte sector from each of the 4 flash
 *       devices will be erased in parallel.
 */
static int intelerase(devp, segment, pPrt, pYield)
   fdev_t *devp;
   volatile u_long *segment;
   int (*pPrt)() ;
   void (*pYield)() ;
{
	register int timer, clock;
	u_long status;
	int dev;

	*segment = ERASE;
	*segment = CONFIRM;
	/*
	 * The intel spec gives a max time of 10 sec 
	 * in which the block erase should be over.
	 */
	for(timer = 7; timer; --timer)
	{
#ifdef BOOT
		/* 1.6 sec delay loop */
		for(clock = 150 /* 1600 */; clock; --clock)
#else /* BOOT */
		for(clock = 1600; clock; --clock)
#endif /* BOOT */
		{
			msec(1);
			if (pYield)
				(*pYield)() ;
			/* (*suspend)(); */
		}
		/*
		 * Write Read status Register Command.
		 */
		*segment = RSTATUS; 
		status = *segment;
		if ((status & SR_READY) == SR_READY)
			break;
#ifdef DEBUG
		if (pPrt)
			(*pPrt)(".");
#endif /* DEBUG */
	}
	/*
	 * Clear the Status Register in all 4 Flash devices.
	 */
	*segment = CSTATUS; 	
	if (!(status & (SR_SUSP | SR_ERASE | SR_WRITE | SR_VPPLOW)))
		return(ENOERR);
	for (dev = 0; dev < sizeof(long); ++dev)
	{
		u_char devstat;
#ifdef NEWSTUFF
#define ltoc(l)		(u_char)((l) & 0xFFL)

		devstat = ltoc(status >> (dev * 8));
		if (devstat & ltoc(SR_VPPLOW))
			if (pPrt)
			    (*pPrt)("ERROR: dev %d: Vpp Low Detect\n", dev);
		if ((devstat & ltoc(SR_CMDSEQ)) == ltoc(SR_CMDSEQ))
			if (pPrt)
			    (*pPrt)("ERROR: dev %d: bad command sequence\n", dev);
		if ((devstat & ltoc(SR_CMDSEQ)) == ltoc(SR_WRITE))
			if (pPrt)
			    (*pPrt)("ERROR: dev %d: byte write failed\n", dev);
		if ((devstat & ltoc(SR_CMDSEQ)) == ltoc(SR_ERASE))
			if (pPrt)
			    (*pPrt)("ERROR: dev %d: block erase failed\n", dev);
		if (devstat & ltoc(SR_SUSP))
			if (pPrt)
			    (*pPrt)("ERROR: dev %d: erase is suspended\n", dev);
#endif /* NEWSTUFF */
		devstat = (u_char)((status >> (dev * 8)) & 0x000000FFL);
		if (devstat & SR_VPPLOW)
			if (pPrt)
			    (*pPrt)("ERROR: dev %d: Vpp Low Detect\n", dev);
		if ((devstat & SR_CMDSEQ) == SR_CMDSEQ)
			if (pPrt)
			    (*pPrt)("ERROR: dev %d: bad command sequence\n", dev);
		if ((devstat & SR_CMDSEQ) == SR_WRITE)
			if (pPrt)
			    (*pPrt)("ERROR: dev %d: byte write failed\n", dev);
		if ((devstat & SR_CMDSEQ) == SR_ERASE)
			if (pPrt)
			    (*pPrt)("ERROR: dev %d: block erase failed\n", dev);
		if (devstat & SR_SUSP)
			if (pPrt)
			    (*pPrt)("ERROR: dev %d: erase is suspended\n", dev);
	}
	return(EERASE);
}

/*
 * This routine programs a long int at a particular address; it is a simple
 * implentation of the programming algorithm given in the Intel 28F008 
 * Flash documentation. Note one byte is programmed into each of the 4
 * Flash devices.
 */
static int intelprogram(devp, addr, val, pPrt)
   fdev_t *devp;
   volatile u_long *addr;
   u_long val;
   int (*pPrt)() ;
{
	u_long status;
	int timer;

	*addr = WRITE;
	*addr = val;
	for (timer = 0; timer < 4 ; ++timer)
	{
		/*
		 * The intel spec gives a max time of 32usec 
		 * in which the programming should be over.
		 */
		usec10();
		/*
		 * Write Read status Register Command.
		 */
		*addr = RSTATUS; 
		status = *addr;
		if ((status & SR_READY) == SR_READY)
			break;		/* All 4 devices are done */
	}
	/*
	 * Clear the Status Register in all 4 Flash devices.
	 */
	*addr = CSTATUS; 	
	return(status & (SR_WRITE | SR_VPPLOW) ? EPROG : ENOERR);
}

static void intelalert(devp, pPrt)
   fdev_t *devp;
   int (*pPrt)() ;
{
	u_char id;
	u_char *base;

	if (pPrt)
	{
		(*pPrt)("FLASH:\n\n");
		(*pPrt)("Type		Address		    Location\n");

		if (devp->id == INTELID)
		{
		    (*pPrt)("Intel 28F008    %08x            NMP (P3) 4MB SIM\n\n",
				FLASH_START);
		}
	}
}
