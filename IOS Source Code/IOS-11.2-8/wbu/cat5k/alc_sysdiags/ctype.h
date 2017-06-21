/* $Id: ctype.h,v 1.1.4.1 1996/05/09 15:00:52 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/ctype.h,v $
 *-----------------------------------------------------------------
 * constant.h -- Constants & Enum Definitions for Diagnostics Driver
 *
 * ?
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * Copyright (c) 1990 Xlnt Designs Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ctype.h,v $
 * Revision 1.1.4.1  1996/05/09  15:00:52  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:50  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:12  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:02  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:45  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:21  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* This is a stand-alone version of ctype.h for use in the PROM
 * environ. It does not require a lookup table like the system
 * version does.
 */
#ifndef _CTYPE_H_
#define _CTYPE_H_

#ifdef TRUE1
#define TRUE 1
#define FALSE 0
#endif

#ifndef YES
#define YES 1
#define NO 0
#endif

#define isalpha(c)	(((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')))
#define isupper(c)	( (c >= 'A') && (c <= 'Z') )
#define islower(c)	( (c >= 'a') && (c <= 'z') )
#define	isdigit(c)	( (c >= '0') && (c <= '9') )
#define	isalnum(c)	(((c>='0')&&(c<='9'))||((c>='A')&&(c<='Z'))||((c>='a')&&(c<='z')))
#define isspace(c)	(( (c >= 0x09) && (c <= 0x0d) ) || (c) == ' ')
#define	isxdigit(c)	( isdigit(c) || ( (c >= 'a') && (c <= 'f') ) || \
	( (c >= 'A') && (c <= 'F') ) )
#define	isprint(c)	( (c >= ' ') && (c <= '~') )
#define	iscntrl(c)	( (c <= 0x01f) )
#define isascii(c)	( !((c) & ~0177))
#define ispunct(c)	(((c >= '!') && (c <= 0x02f)) || ((c>= '{') && (c<= '~')))
#define isgraph(c)	( (c >= '!') && (c <= '~') )
#define	toupper(c)	( (c) - 'a' + 'A' )
#define tolower(c)	( (c) - 'A' + 'a' )
#define toascii(c)	( (c) & 0177 )

#endif
