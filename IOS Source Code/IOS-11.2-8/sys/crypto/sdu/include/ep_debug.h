/* $Id: ep_debug.h,v 1.1.68.1 1996/04/19 14:57:38 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/include/ep_debug.h,v $
 *------------------------------------------------------------------
 * Cylink Source code: ep_debug.h
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ep_debug.h,v $
 * Revision 1.1.68.1  1996/04/19  14:57:38  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/11/28  01:49:37  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:11  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Include/EP_DEBUG.H 8     1/03/95 1:48p Croyle 
*
* DESCRIPTION:	Entity Protection Debug definitions and declarations
*
* NOTES:		strings and macros for supporting debug and testing of the
*				EP module are defined in this file.  This file must be included
*				in all EP module files since some of the debugging macros are
*				redefined as void (;) when debug is turned off (i.e., the debug
*				#defines are commented out - see below).
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: EP_DEBUG.H 
* 
* *****************  Version 8  *****************
* User: Croyle       Date: 1/03/95    Time: 1:48p
* Updated in $/SecureLAN/SDU/Include
* Modified naming convention for all Croyle files.
* 
* *****************  Version 7  *****************
* User: Sduhost      Date: 3/16/94    Time: 3:04p
* Updated in $/SecureLAN/SDU/Include
* Final Update for Integration 1C+.
* 
* *****************  Version 6  *****************
* User: Croyle       Date: 11/01/94   Time: 4:00p
* Updated in $/SecureLAN/SDU/Include
* Moved Debug strings to DEBUG.C.  Now posts to an IQueue for printing
* via task.
* 
* *****************  Version 5  *****************
* User: Sduhost      Date: 2/20/94    Time: 4:27p
* Updated in $/SecureLAN/SDU/Include
* PE integration completed
* 
* *****************  Version 4  *****************
* User: Croyle       Date: 10/20/94   Time: 3:55p
* Updated in $/SecureLAN/SDU/Include
* Accounting for changes to PA interface
* 
* *****************  Version 3  *****************
* User: Croyle       Date: 10/15/94   Time: 1:35p
* Updated in $/SecureLAN/SDU/Include
* Header expansion macro wasn't terminated with a $ properly
* 
* *****************  Version 2  *****************
* User: Croyle       Date: 10/14/94   Time: 1:45p
* Updated in $/SecureLAN/SDU/Include
* Added support for compiling test program when no Cy module exists (but
* able to #define out the test Cy interace and link with real Cy).
*
* *****************  Version 1  *****************
* User: Croyle       Date: 10/13/94   Time: 10:34a
* Created in $/SecureLAN/SDU/Include
* This header file provides MACROs which assist in debugging the EP
* software, primarily the operation of ep_frame().  This file defines
* macros which bypass PA and RTXC support so that the standalone test
* program, ep-test.c, can be executed.
*******************************************************************************/

#ifndef EP_DEBUG_H
#define EP_DEBUG_H

/*-------------------------- DEBUG CONFIGURATION MACROS ----------------------- */

/* NOTE:	These are the macros which control the amount of debug support */
/*			to provide.  Comment out all #defines in this section to turn */
/*			off all debugging support. */

/*#define NO_PA   		 		// dummys out PA function calls */
/*#define NO_CY			 		// dummys out CY function call */
/*#define NO_RTXC 		 		// dummys out RTXC calls */
/*#define CS_BYPASS				// creates a dummy CIB on the fly */

/*-------------------------- FUNCTIONAL BYPASS MACROS ------------------------- */

/* RTXC releated macros */

#ifdef  NO_RTXC
#define TICKS			ushort
#define CLKTICK			1
#define CLKRATE			1
#define KS_inqtime()	1000
#define KS_delay(x,t)	;
#endif

/* PA related macros */

#ifdef NO_PA
#define computeHashValue(x,y) 		(1?(ushort)x:(ushort)x)
#define epReleaseBuffer(p)
#endif

/* CY related macros */

#ifdef NO_CY
#define cy_frame(f)  													\
{	if ((Pa2EpFrameControl*)f->receivedPath == UPE_SIDE)				\
		epReleaseBuffer((Pa2EpFrameControl*)f->descriptorLink);			\
	else if (netTransmitFrame((Cy2TxFrameControl*)f)==REJECT_TXREQ)     \
		epReleaseBuffer((Pa2EpFrameControl*)f->descriptorLink);			\
}
#endif

#endif
/*------------------------- ep_debug.h END OF FILE ---------------------------- */
