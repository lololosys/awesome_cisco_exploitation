/* $Id: flashbt.ld,v 1.1.4.1 1996/05/09 15:01:28 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/obj-68-alc/flashbt.ld,v $
 *------------------------------------------------------------------
 * flashbt.ld -- linker script for ACP flash boot.
 *
 * November 1994, Rick Lowe
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flashbt.ld,v $
 * Revision 1.1.4.1  1996/05/09  15:01:28  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:24  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:52  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:35  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:29  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
# Revision 1.1  1996/03/11  07:44:44  rlowe
# Placeholders for California.
#
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

SECTIONS
{
	.text :				/* Could start at abs 0 if needed. */
	{
		CREATE_OBJECT_SYMBOLS
		_btext = . ;
		*(.text)		/* All text. */
		. = ALIGN (4) ;
		_etext = . ;
	}

	.data . : AT ( ADDR (.text) + SIZEOF (.text) )
	{
		_bdata = . ;
		*(.data)		/* All data. */
		. = ALIGN (4) ;
		_edata = . ;
	}

	_ImageSize = SIZEOF (.text) + SIZEOF (.data) ;
	_EndImage  = ADDR (.text) + _ImageSize ;

	.bss ALIGN (0x400) :
	{
		_bbss = . ;
		*(.bss)			/* All bss. */
		*(COMMON)		/* Any non-bss (?!) un-init data. */
		. = ALIGN (4) ;
		_ebss = . ;
	}
}
