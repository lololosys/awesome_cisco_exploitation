/* $Id: lzsmem.c,v 3.2.62.2 1996/05/17 12:16:38 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/wan/lzsmem.c,v $
 *------------------------------------------------------------------
 * lzsmem.c -- memory routines for stac compression engine
 *
 * March 1995, Steve Turadek
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lzsmem.c,v $
 * Revision 3.2.62.2  1996/05/17  12:16:38  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/04/03  22:13:00  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.62.1  1996/03/29  06:28:33  turadek
 * CSCdi52904:  stac libraries need to be brought to current released level
 * Branch: California_branch
 * Latest and greatest released stac code.
 *
 * Revision 3.2  1995/11/17  18:03:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:49:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:35:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/01  06:23:19  turadek
 * CSCdi39487:  stac compression broken for all encaps
 * fixes two stac library bugs (one data-dependent, one for anti-
 * compression feature.)  also adds enhanced x25 compression debug support.
 *
 * Revision 2.1  1995/06/07  23:18:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*----------------------------------------------------------------------*/

/* LZS221-C Version 4, Release 2, Beta-1								*/

/*----------------------------------------------------------------------*/

/* LZSMEM.C - Buffer copy functions										*/

/*  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	*/

/*  !!                                                              !!	*/

/*  !!  NOTE:                                                       !!	*/

/*  !!  The contents of this document constitute                    !!	*/

/*  !!           CONFIDENTIAL INFORMATION                           !!	*/

/*  !!  		(C) Stac Electronics 1995.							!!	*/

/*  !!																!!	*/

/*  !!      Including one or more U.S. Patents:			    		!!	*/

/*  !!		No. 4701745, 5016009, 5126739 and 5146221 and       	!!	*/

/*  !!                  other pending patents.               	    !!	*/

/*  !!                                                              !!	*/

/*  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	*/

/*																		*/

/*----------------------------------------------------------------------*/

/*																		*/

/* Copyright 1995 Stac Electronics, Carlsbad, California.				*/

/* All rights reserved.  This code is Stac confidential property, and	*/

/* the algorithms and methods used herein may not be disclosed to any	*/

/* party without express written consent from Stac Electronics.			*/

/*								      									*/

/*----------------------------------------------------------------------*/



/************************************************************************

*	$Workfile:   lzsmEM.c  $

*	$Revision: 3.2.62.2 $

*	$Date: 1996/05/17 12:16:38 $

*	$Author: ppearce $

*

*	$Log: lzsmem.c,v $
 * Revision 3.2.62.2  1996/05/17  12:16:38  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/04/03  22:13:00  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.62.1  1996/03/29  06:28:33  turadek
 * CSCdi52904:  stac libraries need to be brought to current released level
 * Branch: California_branch
 * Latest and greatest released stac code.
 *
 * Revision 3.3  1996/03/28  19:55:11  turadek
 * CSCdi52904:  stac libraries need to be brought to current released level
 * Latest  and greatest released Stac code.
 *

 * 

 *    Rev 1.9   20 Oct 1995 15:59:38   TIMB

 * Bumped rev #

 * 

 *    Rev 1.8   31 Aug 1995 09:57:50   TIMB

 * Update rev #

* 

*    Rev 1.7   17 Aug 1995 16:19:26   TIMB

* 4.23

* Bumped Rev #

* 

*    Rev 1.6   Jul 13 1995 11:56:00   RFRIEND

* Steve Mickelsen's bugs fixed:

* 1)  0 Byte Source Flush in lzsc.c

* 2) 15:1 max compression ratio in lzsc.c

* 3) No EOC on Dest Flush in lzsc.c

* 

*    Rev 1.0   Apr 24 1995 15:07:14   BOBL

*  

* 

*************************************************************************/

#include "LZS.H"	/* Compiler, processor Definitions and prototypes 	*/





#define MAX_BLOCK_SIZE_TYPE		long

#define SIZEOF_tOrigByteIndex	4

#define MAX_BUFFR_SIZE_TYPE		long



#if defined (LZS_FAST)

	#define _FAST_

	#define _Small_Blocks_

	#define MAX_ORIG_INDEX OrigCnt

	#undef	LZS_SMALL_BUFFERS

	#define LZS_SMALL_BUFFERS

#endif



#if defined (_BIG_CMPRS_BFR_)

	#define MAX_ORIG_CNT_FAST 57343U

#else

	#define _EARLY_OUT_ 1

	#define MAX_ORIG_CNT_FAST 63487U

#endif



#define MAX_INDEX	~((tOrigByteIndex) 0)



#if !defined MAX_ORIG_INDEX

	#define MAX_ORIG_INDEX \
		((flags & LZS_SOURCE_FLUSH) ? OrigCnt : MAX_INDEX)

#endif



#if defined (_Small_Blocks_)

	#undef MAX_BLOCK_SIZE_TYPE

	#undef SIZEOF_tOrigByteIndex

	#define MAX_BLOCK_SIZE_TYPE 	short

	#define SIZEOF_tOrigByteIndex 	2

#endif



#if defined (LZS_SMALL_BUFFERS)

	#undef MAX_BUFFR_SIZE_TYPE

	#define MAX_BUFFR_SIZE_TYPE 	short

	#define MAX_BUFFER_SIZE			0xFFFFU

#endif



typedef unsigned MAX_BUFFR_SIZE_TYPE tMaxBfrSz;

typedef unsigned MAX_BLOCK_SIZE_TYPE tOrigByteIndex;



#define FP_Off(fp) fp



#define dREG

#define pREG register



/*----------------------------------------------------------------------*/

/* mem_copy_r - Copy n bytes from one (not overlapped) buffer to another*/

/*----------------------------------------------------------------------*/

/* 	Source and Destination must be offset by at least 4 bytes;			*/

/*	Memory model independent.											*/

/*																		*/

/*	Syntax: mem_copy_r (pDestn, pSource, wNbytes)						*/

/*																		*/

/* In:	C calling conventions											*/

/*	void LZS_FAR * pDestn,		Pointer to destination of copy			*/

/*	void LZS_FAR *pSource,		Pointer to source of copy				*/

/*	unsigned short Nbytes,		Number of bytes to copy					*/

/*																		*/

/* Returns:																*/

/*	(unsigned char LZS_FAR *) pDestn + Nbytes							*/

/*																		*/

/*----------------------------------------------------------------------*/



unsigned char LZS_FAR *LZS_FAR mem_copy_r (

	void LZS_FAR   *pDestn,			/* Pointer to destination of copy	*/

	void LZS_FAR   *pSource,        /* Pointer to source of copy		*/

	unsigned short  Nbytes          /* Number of bytes to copy			*/

)

{

	register unsigned char LZS_FAR * pEnd =

		(unsigned char LZS_FAR *) pDestn + Nbytes;



#if defined (LZS_ALIGNED)

	register unsigned long	dwReg;

#endif



	if (Nbytes > 7) {

		/* align source/destination to quad-byte boundary */



		if (((unsigned long)pSource) & 1) {

			*(unsigned char LZS_FAR *) pDestn =

				 *(unsigned char LZS_FAR *) pSource;

			pDestn = ((unsigned char LZS_FAR *) pDestn + 1);

			pSource = ((unsigned char LZS_FAR *) pSource + 1);

		}

		if (((unsigned long)pSource) & 2) {

			*(unsigned char LZS_FAR *) pDestn =

				*(unsigned char LZS_FAR *) pSource;

			*((unsigned char LZS_FAR *) pDestn + 1) =

				*((unsigned char LZS_FAR *) pSource + 1);

			pDestn = ((unsigned char LZS_FAR *) pDestn + 2);

			pSource = ((unsigned char LZS_FAR *) pSource + 2);

		}

		pEnd -= 3;	/* copy till at last boundary */



#if defined (LZS_ALIGNED)

		/* Machine requires short/long accessed be aligned				*/

		/* on even/quad byte boundaries									*/



		if (((unsigned long)pDestn) & 1) {

			while ((unsigned char LZS_FAR *)pDestn < pEnd) {

				dwReg = *(unsigned long LZS_FAR *) pSource;



				#if (LZS_BYTE_ORDER == LZS_LITTLE_END)

					/* out right to left:bytes */

					/* order 	4, 3-2, 1  */



					*(unsigned char LZS_FAR *) pDestn =

						(unsigned char) dwReg;



					pDestn = ((unsigned char LZS_FAR *)

						pDestn + 1);

					dwReg >>= 8;



					*(unsigned short LZS_FAR *) pDestn =

						(unsigned short)dwReg;

					dwReg >>= 16;



					*((unsigned char LZS_FAR *) pDestn+2) =

						(unsigned char) dwReg;

				#else

					/* out right to left:bytes */

					/* order 	1, 2-3, 4  */

					*((unsigned char LZS_FAR *) pDestn+3) =

						(unsigned char) dwReg;



					pDestn = ((unsigned char LZS_FAR *)

						pDestn + 1);

					dwReg >>= 8;



					*(unsigned short LZS_FAR *) pDestn =

						(unsigned short)dwReg;

					dwReg >>= 16;



					*((unsigned char LZS_FAR *) pDestn-1) =

						(unsigned char) dwReg;

				#endif



				pDestn = ((unsigned char LZS_FAR *)

					pDestn + 3);



				pSource = ((unsigned char LZS_FAR *)

					pSource + 4);

			}	/* end of while */

		}

		else if (((unsigned long)pDestn) & 2) {

			/* source & destn are aligned on a short boundary */



			while ((unsigned char LZS_FAR *)pDestn < pEnd) {

				*(unsigned short LZS_FAR *) pDestn =

					*(unsigned short LZS_FAR *) pSource;

				*((unsigned short LZS_FAR *) pDestn + 1) =

					*((unsigned short LZS_FAR *)pSource+1);

				pDestn = ((unsigned short LZS_FAR *)pDestn+2);

				pSource= ((unsigned short LZS_FAR *)pSource+2);

			}	/* end of while */

		}

		else {	/* Both source and destn are aligned */

			/* on a long boundary		     */

			while ((unsigned char LZS_FAR *)pDestn < pEnd) {

				*(unsigned long LZS_FAR *) pDestn =

					*(unsigned long LZS_FAR *)  pSource;

				pDestn = ((unsigned long LZS_FAR *) pDestn+1);

				pSource = ((unsigned long LZS_FAR *)pSource+1);

			}

		}



#else

		while ((unsigned char LZS_FAR *)pDestn < pEnd) {

			*(unsigned long LZS_FAR *) pDestn =

				*(unsigned long LZS_FAR *) pSource;



			pDestn = ((unsigned long LZS_FAR *) pDestn + 1);

			pSource = ((unsigned long LZS_FAR *) pSource + 1);

		}

#endif



		pEnd += 3;	/* restore end of copy */

	}



	/* finish the copy */



	while ((unsigned char LZS_FAR *)pDestn < pEnd) {

		*(unsigned char LZS_FAR *) pDestn =

			*(unsigned char LZS_FAR *) pSource;



		pDestn = ((unsigned char LZS_FAR *) pDestn + 1);

		pSource = ((unsigned char LZS_FAR *) pSource + 1);

	}

	return ((unsigned char LZS_FAR *)pDestn);

}





/*----------------------------------------------------------------------*/

/* copy_prev_string_r - Copy n bytes from one (overlapped)		 		*/

/*			buffer to another											*/

/*----------------------------------------------------------------------*/

/* 	Source and Destination are overlapped. Same segment/selector.		*/

/*																		*/

/*	Memory model independent.											*/

/*																		*/

/*	Syntax: copy_prev_string_r (pDestn, wOffset, wNbytes)						*/

/*																		*/

/* In:	C calling conventions											*/

/*	unsigned char LZS_FAR * pDestn,	Pointer to destination of copy		*/

/*	unsigned short Offset,			Offset from des to src for copy		*/

/*	tMaxBfrSz wNbytes,				Number of bytes to copy				*/

/*									(Max = 64 K) (Min = 3 bytes)		*/

/* Returns:																*/

/*	(unsigned char LZS_FAR *) pDestn + wNbytes							*/

/*																		*/

/*----------------------------------------------------------------------*/



unsigned char LZS_FAR * LZS_FAR copy_prev_string_r (

	unsigned char LZS_FAR *pDestn,	/* ptr to destination of copy		*/

	unsigned short	       Offset,	/* Offset from des to src for copy	*/

	tMaxBfrSz 	       	   wNbytes	/* Number of bytes to copy,			*/

									/* copy, Max = (64K-1)		   		*/

)

{

	unsigned char LZS_FAR *pEnd;

	unsigned char LZS_FAR *pSi;		/* Pointer to string source 		*/

	unsigned long  	       E_A_X;

	unsigned short	       i;



	pSi = pDestn - Offset;

	pEnd = pDestn + wNbytes;		/* Stopping place 					*/



	if (Offset >= 4) {

Off4Plus:							/* offset 4+ can use dword copy		*/



		if (wNbytes & 1) {

			*pDestn++ = *pSi++;		/* copy odd byte 					*/

		}

		if (wNbytes & 2) {



#if defined (LZS_ALIGNED)

			*pDestn = *pSi;

			*(pDestn+1) = *(pSi+1);

#else

			*(unsigned short LZS_FAR *) pDestn =

				*(unsigned short LZS_FAR *) pSi;

#endif



			pDestn += 2;

			pSi += 2;

		}

		while (pDestn < pEnd)   {



#if defined (LZS_ALIGNED)

			*pDestn = *pSi;

			*(pDestn+1) = *(pSi+1);

			*(pDestn+2) = *(pSi+2);

			*(pDestn+3) = *(pSi+3);

#else

			*(unsigned long LZS_FAR *) pDestn =

				*(unsigned long LZS_FAR *) pSi;

#endif



			pDestn += 4;

			pSi += 4;

		}

		return (pDestn);			/* return far ptr to end of string	*/

	}

	if (Offset > 1) {

		if (wNbytes < 3) {

			while ( pDestn < pEnd )

				*pDestn++ = *pSi++;

			return (pDestn);		/* return far ptr to end of string	*/



		}

		wNbytes -= Offset;			/* increase offset to 4				*/



		for (i = Offset; i; i--)

			 *pDestn++ = *pSi++;



		pSi -= Offset;

		goto Off4Plus;

	}



	/* Repeat byte at pDestn - offset (1 ) */



	E_A_X  = (((unsigned long) *pSi) << 8) | *pSi;

	E_A_X |= E_A_X << 16;



#if defined (LZS_ALIGNED)

	/* Machine requires long accesses be aligned on quad byte boundaries*/



	if (wNbytes > 7) {

		/* align destination to quad-byte boundary */



		if (((unsigned long)pDestn) & 1) {

			*pDestn++ = (unsigned char) E_A_X;

		}

		if (((unsigned long)pDestn) & 2) {

			*(unsigned char LZS_FAR *) pDestn =

				(unsigned char) E_A_X;

			*((unsigned char LZS_FAR *) pDestn + 1) =

				(unsigned char) E_A_X;

			pDestn += 2;

		}

		pEnd -= 3;					/* Copy till at last boundary	    */

									/* destn aligned on a long boundary	*/



		while (pDestn < pEnd) {

			*(unsigned long LZS_FAR *) pDestn = E_A_X;

			pDestn += 4;

		}



		pEnd += 3;					/* restore end of copy 				*/

	}



	/* finish the copy	*/

	while (pDestn < pEnd) {

		*pDestn++ = (unsigned char)E_A_X;

	}

#else

	if (wNbytes & 1)

		*pDestn++ = (unsigned char)E_A_X;

		/* out, out odd byte */



	if (wNbytes & 2) {

		*(unsigned short LZS_FAR *) pDestn = (unsigned short) E_A_X;

		pDestn += 2;

	}

	while (pDestn < pEnd)   {

		*(unsigned long LZS_FAR *) pDestn = E_A_X;

		pDestn += 4;

	}

#endif



	return (pDestn); 				/* return far ptr to end of string	*/

}





/*----------------------------------------------------------------------*/

/* init_hash_tbl_r - Sets n bytes to a specified character in a buffer	*/

/*----------------------------------------------------------------------*/

/*	Memory model independent.											*/

/*																		*/

/*	Syntax: init_hash_tbl_r (pDestn, pPattern, wNbytes)					*/

/* In:																	*/

/*	void 	far *pDestn,	    Pointer to destination for set			*/

/*	void 	far *pPattern,	    Pointer to 4 byte pattern to set		*/

/*	unsigned short wNbytes,	    # of bytes to set (multiple of 2)		*/

/*							    (Max = 64k-1)							*/

/*																		*/

/* Returns:	(void)														*/

/*----------------------------------------------------------------------*/



void LZS_FAR init_hash_tbl_r (

	void 	LZS_FAR *pDestn,    /* Pointer to destination for set    	*/

	void 	LZS_FAR *pPattern,  /* Pointer to 4 byte pattern to set  	*/

	unsigned short 	 wNbytes    /* # of bytes to set (multiple of 2) 	*/

)

{

	register unsigned long E_A_X;

	register unsigned char LZS_FAR *pEnd;



	E_A_X = *(unsigned long LZS_FAR *)pPattern;		/* repeat pattern	*/

	pEnd  = (unsigned char LZS_FAR *) pDestn + (wNbytes & 0xFFF8);



	while ( (unsigned char LZS_FAR *)pDestn < pEnd ) {

		*(unsigned long LZS_FAR *) pDestn = E_A_X;

		*((unsigned long LZS_FAR *) pDestn + 1) = E_A_X;

		pDestn = ((unsigned long LZS_FAR *) pDestn + 2);

	}

	if (wNbytes & 4) {			/* out, out odd word					*/

		*(unsigned long LZS_FAR *) pDestn = E_A_X;

		pDestn = ((unsigned long LZS_FAR *) pDestn + 1);

	}

	if (wNbytes & 2) {			/* out, out odd word					*/

		*(unsigned short LZS_FAR *)pDestn = (unsigned short)E_A_X;

	}

	return;

}



/*------------------------ end of LZSMEM.C -----------------------------*/

