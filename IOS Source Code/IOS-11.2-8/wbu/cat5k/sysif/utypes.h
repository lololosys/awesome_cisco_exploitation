/* $Id: utypes.h,v 1.1.4.1 1996/05/09 15:01:55 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/sysif/utypes.h,v $
 *-----------------------------------------------------------------
 * utypes.h -- 
 *
 * November 1994, jkuo
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: utypes.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:55  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:49  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:43:21  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:14:01  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:43  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:57  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.6  1995/10/04  01:07:32  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.5  1995/02/07  21:25:07  sgarcia
 * Added media_type to CardFeatureTable.c/h.
 *
 * Revision 1.4  1995/02/01  22:34:44  vinod
 * Moved some defintions from utypes.h to syn_ser_eeprom.h, added
 * a few duplicate constants.
 *
 * Revision 1.3  1995/01/30  19:08:39  sgarcia
 * Defines for card media type.
 *
 * Revision 1.2  1994/12/24  02:47:16  husain
 * merging
 *
 * Revision 1.1  1994/12/16  23:30:10  husain
 * Moved from 8051/common
 *
* Revision 1.3  1994/12/08  21:49:12  jkuo
* Added a new definition for unsigned char to look like uchar.
*
* Revision 1.2  1994/11/24  02:27:31  husain
* added equates for ATM
*
 * Revision 1.1  1994/11/15  19:53:56  husain
 * moved 8051/common
 *
 * Revision 1.1  1994/11/15  03:16:42  jkuo
 * 	First checked in 8051/mcp.
*
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	_UTYPES_H_
#define _UTYPES_H_

#define	flag(x)		(1 << (x))
#define	flagL(x)	(1L << (x))	/* long word flag */

#ifndef	TRUE
#define	TRUE		1
#define	FALSE		0
#endif

#define	uLong		unsigned long
#define uShort		unsigned short
#define uChar		unsigned char
#define uchar		unsigned char

#define uInt32		uLong
#define uInt16		uShort

#define	BOOLEAN		bit

#ifdef LATER
/* define split uInt16 for indivdual byte access */
struct spint {
  uChar h;
  uChar l;
};

typedef union {
  uInt16	word;
  struct spint	byte;
} BYT_ACC;

/* define split long word for individual word access */
struct splint {
  uInt16 h;
  uInt16 l;
};

typedef union {
  uInt32	lword;
  struct splint	word;
} WRD_ACC;

#endif


#endif

