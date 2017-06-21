/* $Id: flash.c,v 1.1.4.1 1996/05/09 14:54:19 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cbiflash/flash.c,v $
 *------------------------------------------------------------------
 * flash.c
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
 * $Log: flash.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:19  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:44  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:24  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:09:45  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:44  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:15  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.8  1995/10/04  01:07:50  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.7  1995/07/27  09:02:16  rlowe
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


extern int printf () ;


/*
 *	These routines are now just wrappers for the real ones,
 *	which take "printf" as an additional argument.  This
 *	allows a run-time binding of the appropriate "printf".
 */

/*
 *	Erase flash and write data to it.
 */
int load(block, p, len)
   int block;
   u_long *p;
   int len;
{
	return (load2 (block, p, len, printf, yield_cpu)) ;
}


/*
 *	Sets up programming variables.
 */
int erase(block, count)
   int block;
   int count;
{
	return (erase2 (block, count, printf, yield_cpu)) ;
}


/*
 *	This routine takes a buffer and programs it into the flashes.
 */
int record(block, p, len)
   int block;
   u_long *p;
   int len;
{
	return (record2 (block, p, len, printf, yield_cpu)) ;
}
