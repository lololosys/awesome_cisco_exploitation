/* $Id: basic_types.h,v 1.1.4.1 1996/05/09 14:54:28 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cominc/basic_types.h,v $
 *-----------------------------------------------------------------
 * basic_types.h -- Defines basic types used 
 *
 * Ratinder P. S. Ahuja
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: basic_types.h,v $
 * Revision 1.1.4.1  1996/05/09  14:54:28  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:53  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:35  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:09:56  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:53  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:24  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *------------------------------------------------------------------
 * basic_types.h -- Defines basic types used 
 *
 *------------------------------------------------------------------
 *
 *  FILE:  basic_types.h
 *
 *	CONTENTS:
 *
 *	TAB STOP = 4
 *	Defines the primitive data types
 *
 *	OWNER:
 *
 *		Ratinder P. S. Ahuja
 *
*/

#ifndef _BASICT_
#define _BASICT_

#ifdef UNIX
#include	<stdio.h>
#include	<malloc.h>
#endif

/*
**
**	Basic data types
**
*/

#ifdef LANGUAGE_C

typedef char   		INT8;
typedef unsigned char   UINT8;
typedef short  		INT16;
typedef unsigned short  UINT16;
typedef long  		INT32;
typedef unsigned long  	UINT32;

typedef UINT16		tVCI;
typedef UINT16		tVPI;
typedef UINT16		tVPC;
typedef UINT16		tLECID;
typedef UINT16		tCOLOR;
typedef UINT32		tQOS;

#define	ACP_OK		0
#define ACP_ERR		1
#define NULLP		(void *)0

typedef struct mac_addr {
	
	UINT8	maddr[6];

} tMACADDR, *tpMACADDR;

typedef UINT32	tBOOLEAN ;

#ifndef	TRUE
#define	TRUE	1
#endif

#ifndef	FALSE
#define	FALSE	0
#endif

#ifndef	SYNALC		/* Do NOT do this for IOS-related builds. */
#ifndef	NULL
#define	NULL	NULLP
#endif
#endif

#endif


/*
**
**	Define sizes of various queues 
**
*/

#include	"param.h"

#endif /* _BASICT_H_ */
