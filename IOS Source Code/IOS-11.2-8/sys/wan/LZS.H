/* $Id: LZS.H,v 3.2.62.2 1996/05/17 12:16:11 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/wan/LZS.H,v $
 *------------------------------------------------------------------
 * LZS.H -- Include files for stac compression engine
 *
 * March 1995, Steve Turadek
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: LZS.H,v $
 * Revision 3.2.62.2  1996/05/17  12:16:11  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/04/03  22:11:05  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.62.1  1996/03/29  06:28:35  turadek
 * CSCdi52904:  stac libraries need to be brought to current released level
 * Branch: California_branch
 * Latest and greatest released stac code.
 *
 * Revision 3.2  1995/11/17  18:00:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:47:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:34:58  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/01  06:23:17  turadek
 * CSCdi39487:  stac compression broken for all encaps
 * fixes two stac library bugs (one data-dependent, one for anti-
 * compression feature.)  also adds enhanced x25 compression debug support.
 *
 * Revision 2.1  1995/06/07  23:16:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*----------------------------------------------------------------------*/

/* LZS221-C Version 4, Release 2, Beta-1								*/

/*----------------------------------------------------------------------*/

/* LZS.H - Function prototypes and constants for LZS221-C				*/

/* 		   Data Compression Software - a STAC(R) LZS(TM) algorithm		*/

/*																		*/

/*  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	*/

/*  !!                                                              !!	*/

/*  !!  NOTE:                                                       !!	*/

/*  !!  The contents of this document constitute                    !!	*/

/*  !!           CONFIDENTIAL INFORMATION                           !!	*/

/*  !!  		(C) Stac Electronics 1995.							!!	*/

/*  !!																!!	*/

/*  !!      Including one or more U.S. Patents:						!!	*/

/*  !!		No. 4701745, 5016009, 5126739 and 5146221 and			!!	*/

/*  !!                  other pending patents.              	    !!  */

/*  !!                                                              !!	*/

/*  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	*/

/*																		*/

/*----------------------------------------------------------------------*/

/*																		*/

/* Copyright 1988-95 Stac Electronics, Carlsbad, California.			*/

/* All rights reserved.  This code is Stac confidential property, and	*/

/* the algorithms and methods used herein may not be disclosed to any	*/

/* party without express written consent from Stac Electronics.			*/

/*																		*/

/*----------------------------------------------------------------------*/

/*	All Stac product names are trademarks or registered trademarks		*/

/*	of Stac Electronics.												*/

/*----------------------------------------------------------------------*/



/************************************************************************

*	$Workfile:   lzs.h  $

*	$Revision: 3.2.62.2 $

*	$Date: 1996/05/17 12:16:11 $

*	$Author: ppearce $

*

*	$Log: LZS.H,v $
 * Revision 3.2.62.2  1996/05/17  12:16:11  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/04/03  22:11:05  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.62.1  1996/03/29  06:28:35  turadek
 * CSCdi52904:  stac libraries need to be brought to current released level
 * Branch: California_branch
 * Latest and greatest released stac code.
 *
 * Revision 3.3  1996/03/28  19:55:13  turadek
 * CSCdi52904:  stac libraries need to be brought to current released level
 * Latest  and greatest released Stac code.
 *

 * 

 *    Rev 1.9   20 Oct 1995 15:58:48   TIMB

 * Bumped rev#.

 * 

 *    Rev 1.8   31 Aug 1995 10:01:12   TIMB

 * Update revision #

 * 

 *    Rev 1.7   17 Aug 1995 16:19:54   TIMB

 * 4.23

 * Bumped Rev #

 * 

 *    Rev 1.6   Jul 13 1995 11:55:48   RFRIEND

 * Steve Mickelsen's bugs fixed:

 * 1)  0 Byte Source Flush in lzsc.c

 * 2) 15:1 max compression ratio in lzsc.c

 * 3) No EOC on Dest Flush in lzsc.c

* 

*    Rev 1.0   Apr 24 1995 15:07:14   BOBL

*  

* 

*************************************************************************/



#if !defined (_Lzs_h)

#define _Lzs_h

/* #define _SAVE_COMPRESS_POINTERS_  */



/*----------------------------------------------------------------------*/

/*                                                              		*/

/*   The following variable types must have the following sizes:		*/

/*	unsigned char  =  8 bits											*/

/*       unsigned short = 16 bits										*/

/*       unsigned long  = 32 bits										*/

/*																		*/

/*----------------------------------------------------------------------*/



/*----------------------------------------------------------------------*/

/* Symbolic Constants - see PRS-0026 Revision 40 data sheet for details */

/*----------------------------------------------------------------------*/

#define LZS_DEST_EXHAUSTED		2

#define LZS_DEST_FLUSH 			2

#define LZS_DEST_MIN 			16U

#define LZS_END_MARKER 			4

#define LZS_FLUSHED 			4

#define LZS_INVALID 			0

#define LZS_MAX_OVERRUN 		16U

#define LZS_PERFORMANCE_MODE_0 	0

#define LZS_PERFORMANCE_MODE_1 	8

#define LZS_PERFORMANCE_MODE_2 	16

#define LZS_SAVE_HISTORY 		4

#define LZS_SOURCE_EXHAUSTED 	1U

#define LZS_SOURCE_FLUSH 		1

#define LZS_UNCOMPRESSED		2



/*----------------------------------------------------------------------*/

/* Other constants														*/

/*----------------------------------------------------------------------*/

#define LZS_LITTLE_END			1

#define LZS_BIG_END				2



/*----------------------------------------------------------------------*/

/* Compile Time Options - See data sheet for details					*/

/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/

/* LZS_FAR																*/

/*	This constant must always be defined.  It defaults to a blank.		*/

/*	Add your definition to the end of the line, if required.			*/

/*	Ex: #define LZR_FAR __far											*/

/*----------------------------------------------------------------------*/

#if !defined (LZS_FAR)

#define LZS_FAR

#endif



/*----------------------------------------------------------------------*/

/* LZS_SMALL_BUFFERS													*/

/*	If desired, add the definition after the #if statement.				*/

/*	Ex: #define LZS_SMALL_BUFFERS										*/

/*----------------------------------------------------------------------*/

#if !defined (LZS_SMALL_BUFFERS)

#endif



/*----------------------------------------------------------------------*/

/* LZS_FAST																*/

/*	If desired, add the definition after the #if statement.				*/

/*	Ex: #define LZS_FAST												*/

/*----------------------------------------------------------------------*/

#if !defined (LZS_FAST)

#endif



/*----------------------------------------------------------------------*/

/* LZS_BYTE_ORDER														*/

/*	This constant must always be defined.  It defaults to 				*/

/*	LZS_LITTLE_END.  Change this to LZS_BIG_END if required.			*/

/*	Ex: #define LZS_BYTE_ORDER LZS_BIG_END								*/

/*----------------------------------------------------------------------*/

#if !defined (LZS_BYTE_ORDER)

/* changed by turadek@cisco */
/* #define LZS_BYTE_ORDER LZS_LITTLE_END */
#define LZS_BYTE_ORDER LZS_BIG_END

#endif



/*----------------------------------------------------------------------*/

/* LZS_ALIGNED															*/

/*	If desired, add the definition after the #if statement.				*/

/*	Ex: #define LZS_ALIGNED												*/

/*----------------------------------------------------------------------*/

#if !defined (LZS_ALIGNED)

/* defined by turadek@cisco */
#define LZS_ALIGNED

#endif



/*----------------------------------------------------------------------*/

/* Function Prototypes - see data sheet for details.					*/

/*----------------------------------------------------------------------*/



unsigned short LZS_FAR LZS_SizeOfCompressionHistory(void);



unsigned short LZS_FAR LZS_InitCompressionHistory(void LZS_FAR * history);



unsigned short LZS_FAR LZS_Compress(

	unsigned char LZS_FAR * LZS_FAR *source,

	unsigned char LZS_FAR * LZS_FAR *dest,

	unsigned long LZS_FAR           *sourceCnt,

	unsigned long LZS_FAR           *destCnt,

	void 	      LZS_FAR           *history,

	unsigned short                   flags,

	unsigned short                   performance

);



unsigned short LZS_FAR LZS_SizeOfDecompressionHistory(void);



unsigned short LZS_FAR LZS_InitDecompressionHistory(void LZS_FAR * history);



unsigned short LZS_FAR LZS_Decompress(

	unsigned char LZS_FAR * LZS_FAR *source,

	unsigned char LZS_FAR * LZS_FAR *dest,

	unsigned long LZS_FAR           *sourceCnt,

	unsigned long LZS_FAR           *destCnt,

	void 	      LZS_FAR           *history,

	unsigned short                   flags

);



#endif



/*------------------------- end of LZS.H -------------------------------*/

