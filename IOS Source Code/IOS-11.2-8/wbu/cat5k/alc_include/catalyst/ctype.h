/* $Id: ctype.h,v 1.1.4.1 1996/05/09 14:55:46 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/catalyst/ctype.h,v $
 *------------------------------------------------------------------
 * ctype.h -- stand-alone version of ctype.h for use in the PROM
 *            environ. It does not require a lookup table like the
 *            system version does.
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * Copyright (c) 1993 Xlnt Designs Inc
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ctype.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:46  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:15  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:14  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:24  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:44:05  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:44  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
@(#)ctype.h	1.1 10/18/93 10/22/93 Copyright (c) 1993 Xlnt Designs Inc 
*/
/* This is a stand-alone version of ctype.h for use in the PROM
 * environ. It does not require a lookup table like the system
 * version does.
 */
#ifndef _CTYPE_H_
#define _CTYPE_H_

#ifndef TRUE
#define TRUE 1
#define FALSE 0
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
