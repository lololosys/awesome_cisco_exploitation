/* $Id: debug.h,v 1.1.68.1 1996/04/19 14:57:36 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/include/debug.h,v $
 *------------------------------------------------------------------
 * Cylink Source code: debug.h
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: debug.h,v $
 * Revision 1.1.68.1  1996/04/19  14:57:36  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.2  1995/12/12  23:12:11  bew
 * Branch: IosSec_branch
 * Better formatting of debug messages.
 *
 * Revision 1.1.2.1  1995/11/28  01:49:36  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:09  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Include/DEBUG.H 4     12/16/94 10:37a Croyle 
*
* DESCRIPTION:	Header file for debugger task
*
* NOTES:		Contains extern declarations for debugging strings
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: DEBUG.H 
* 
* *****************  Version 4  *****************
* User: Croyle       Date: 12/16/94   Time: 10:37a
* Updated in $/SecureLAN/SDU/Include
* Removed EP and CY string constants
* 
* *****************  Version 3  *****************
* User: Sduhost      Date: 3/16/94    Time: 3:04p
* Updated in $/SecureLAN/SDU/Include
* Final Update for Integration 1C+.
*
* *****************  Version 2  *****************
* User: Croyle       Date: 11/02/94   Time: 11:45a
* Updated in $/SecureLAN/SDU/Include
*
* *****************  Version 1  *****************
* User: Croyle       Date: 11/01/94   Time: 4:01p
* Created in $/SecureLAN/SDU/Include
*******************************************************************************/

#ifndef	DEBUG_H
#define	DEBUG_H

/*****************************************************************************
							 INCLUDE FILES
*****************************************************************************/

#include "../crypto/sdu/include/iqueue.h"

/*******************************************************************************
							GLOBAL MACROS
*******************************************************************************/

#ifndef CISCO
#define DEBUG_MSG			/* comment out this line to ignore iDEBUG statements */
#endif

/* interrupt layer macro to post a string to debug task for printf'ing */

#ifdef DEBUG_MSG
#define DEBUG(str)				\
{       						\
	asm cli;					\
	IQ_PUTnc(iq_msg_out,str);	\
	asm sti;					\
}
#else
#ifdef CISCO
extern boolean crypto_sm_debug;
#define DEBUG(str) 				\
{								\
	if (crypto_sm_debug) {		\
		buginf("\nCRYPTO-SDU: %s",str);	\
	}							\
}
#define DEBUG_WITH_ARG(str, arg) \
{ \
	if (crypto_sm_debug) {		\
		buginf("\nCRYPTO-SDU:");	\
		buginf(str, arg);	\
	}							\
}
#else /* !CISCO */
#define DEBUG(str)
#endif /* !CISCO */
#endif

/*****************************************************************************
								 PUBLIC GLOBAL
*****************************************************************************/

/* queues used to pass string pointers */

extern IQueue iq_msg_out;		/* queue used by interrupt layer */

/* CS debug strings */

extern const char *cs100,*cs200,*cs300,*cs400;

#endif
/*------------------------ END OF FILE:  debug.h  ----------------------------- */

