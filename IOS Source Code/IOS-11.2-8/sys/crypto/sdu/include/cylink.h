/* $Id: cylink.h,v 1.1.68.1 1996/04/19 14:57:35 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/include/cylink.h,v $
 *------------------------------------------------------------------
 * Cylink Source code: cylink.h
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cylink.h,v $
 * Revision 1.1.68.1  1996/04/19  14:57:35  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/11/28  01:49:35  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:10  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/Common/CYLINK.H 3     3/29/95 12:34p Sduhost 
*
* DESCRIPTION:
*    Common type definitions.  This file is MODERATED, please contact the
*    SecureLAN Software Development Team before modifying.
*
* NOTES:
*    Include in C files which use Cylink type extensions.
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: CYLINK.H 
* 
* *****************  Version 3  *****************
* User: Sduhost      Date: 3/29/95    Time: 12:34p
* Updated in $/SecureLAN/Common
* Added NULL definition so don't have to keep including stdio.h only for
* NULL def.
* 
* *****************  Version 1  *****************
* User: Croyle       Date: 9/16/94    Time: 11:09a
* Created in $/SecureLAN/Common
*******************************************************************************/

#ifndef CYLINK_H
#define CYLINK_H 1

/*
 * For Cisco, define the compilation environment
 */
#define CISCO
#define far
#define near
#define huge
#define _ss
#define _fastcall

/*
 * IOS Includes for SDU code
 */
#include "master.h"
#include "string.h"
#include "globs.h"
#include "types.h"

/*--------------------------------- macros ------------------------------------ */

#define REG		register
#define EQ		==

#ifndef NULL
#define NULL ((void*)0L)
#endif 

#ifndef CISCO /* these are in IOS types.h */
/*------------------------- byte oriented types ------------------------------- */

typedef unsigned long	ulong;
typedef unsigned short	ushort;
typedef unsigned char	uchar;
#endif /* !CISCO */
/*------------------------- bit oriented types -------------------------------- */

typedef ulong	bits32;
typedef ushort	bits16;
typedef uchar	bits8;

/*------------------------- Intel inline assembly macros---------------------- */
# define EnableCPU()		asm sti
# define DisableCPU()	asm cli

/*------------------------- cylink.h END OF FILE ------------------------------ */
#endif
