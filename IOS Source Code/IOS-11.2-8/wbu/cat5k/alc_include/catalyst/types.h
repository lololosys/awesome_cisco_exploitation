/* $Id: types.h,v 1.1.4.1 1996/05/09 14:55:49 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/catalyst/types.h,v $
 *------------------------------------------------------------------
 * types.h
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * Copyright (c) 1993 Xlnt Designs Inc
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: types.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:49  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:18  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:18  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:27  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:44:08  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:48  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
@(#)types.h	1.1 10/18/93 10/22/93 Copyright (c) 1993 Xlnt Designs Inc 
*/
/*
						types.h

	Global type definitions for the concentrator software.  The cocentrator
	is a MC68302 processor.  This is a 16 bit microprocessor with a 
	MC68000 core and on-chip I/O support for embedded applications.

	Created 7-27-90
	RSM

*/
#ifndef _TYPES_H_
#define _TYPES_H_
/*
	Basic type definitions for various byte(8 bit), word(16 bit), and 
	long word (32 bit) types.
*/
#ifndef __systypes
#define __systypes
typedef unsigned char 		uChar;		/* Eight unsigned bits			*/
typedef unsigned short		uInt16;		/* 16 unsigned bits				*/
typedef unsigned long		uInt32;		/* 32 unsigned bits				*/
typedef long				Int32;		/* 32 signed bits				*/
typedef short				Int16;		/* 16 signed bits				*/
typedef char *  caddr_t;
typedef void	(*pvf_t)();
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned long   u_int;
typedef unsigned long   u_long;
typedef unsigned short  ushort;     /* sys III compat */



typedef	struct timet {
	uInt32 high;
	uInt32 low;
} time_t;
#endif
typedef long    off_t;

#define NULL	0
#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifdef DEBUG
#define ERRORMSG(string)	printf("%s\n",string);
#else
#define ERRORMSG(string)
#endif

#ifdef DEBUG
#define ERRORNUMMSG(string, num)	printf("%s%d\n",string,num);
#else
#define ERRORNUMMSG(string, num)
#endif

#endif
