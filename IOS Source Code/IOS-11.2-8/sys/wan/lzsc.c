/* $Id: lzsc.c,v 3.2.62.3 1996/08/07 09:04:34 snyder Exp $
 * $Source: /release/112/cvs/Xsys/wan/lzsc.c,v $
 *------------------------------------------------------------------
 * lzsc.c -- Compression side of stac compression engine
 *
 * March 1995, Steve Turadek
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lzsc.c,v $
 * Revision 3.2.62.3  1996/08/07  09:04:34  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/05/17  12:16:32  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/04/03  22:12:56  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.62.1  1996/03/29  06:28:28  turadek
 * CSCdi52904:  stac libraries need to be brought to current released level
 * Branch: California_branch
 * Latest and greatest released stac code.
 *
 * Revision 3.2  1995/11/17  18:03:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:49:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:35:52  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/31  19:19:32  turadek
 * CSCdi39487:  stac compression broken for all encaps
 * fixes 2 stac bugs (one data-dependent, one for 'uncompress')
 * feature. also adds enhanced compression debugging facility.
 *
 * Revision 2.1  1995/06/07  23:18:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*----------------------------------------------------------------------*/

/* LZS221-C Version 4, Release 2, Beta-1								*/

/*----------------------------------------------------------------------*/

/*  LZSC.C - Compress data to Lzs(TM) tokens							*/

/*                                                                      */

/*  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	*/

/*  !!                                                              !!	*/

/*  !!  NOTE:                                                       !!	*/

/*  !!  The contents of this document constitute                    !!	*/

/*  !!           CONFIDENTIAL INFORMATION                           !!	*/

/*  !!  		(C) Stac Electronics 1995.                  		!!	*/

/*  !!					                            				!!	*/

/*  !!      Including one or more U.S. Patents:			    		!!	*/

/*  !!		No. 4701745, 5016009, 5126739 and 5146221 and       	!!	*/

/*  !!                  other pending patents.               	    !!	*/

/*  !!                                                              !!	*/

/*  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	*/

/*																		*/

/*----------------------------------------------------------------------*/

/*																		*/

/* Copyright 1988-95 Stac Electronics, Carlsbad, California.			*/

/* All rights reserved.  This code is Stac confidential property, and	*/

/* the algorithms and methods used herein may not be disclosed to any 	*/

/* party without express written consent from Stac Electronics.         */

/*                                                                      */

/*----------------------------------------------------------------------*/

/* All Stac product names are trademarks or registered trademarks		*/

/* of Stac Electronics.													*/

/*----------------------------------------------------------------------*/



/************************************************************************

*	$Workfile:   lzsc.c  $

*	$Revision: 3.2.62.3 $

*	$Date: 1996/08/07 09:04:34 $

*	$Author: snyder $

*

*	$Log: lzsc.c,v $
 * Revision 3.2.62.3  1996/08/07  09:04:34  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/05/17  12:16:32  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/04/03  22:12:56  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.62.1  1996/03/29  06:28:28  turadek
 * CSCdi52904:  stac libraries need to be brought to current released level
 * Branch: California_branch
 * Latest and greatest released stac code.
 *
 * Revision 3.3  1996/03/28  19:55:07  turadek
 * CSCdi52904:  stac libraries need to be brought to current released level
 * Latest  and greatest released Stac code.
 *

 * 

 *    Rev 1.16   20 Oct 1995 15:04:12   TIMB

 * Fix split EOCD bug

 * Fix numerous dest flush bugs

 * Enforce stickiness of source and dest flush flags by returning LZS_INVALID

 * 

 *    Rev 1.15   11 Oct 1995 13:19:14   TIMB

 * Added enforcement of sticky flush flags

 * Fixed several bugs related to dest flush

 * 

 *    Rev 1.14   05 Sep 1995 09:34:40   TIMB

 * Removed line of code comented out for tidiness.

 * 

 *    Rev 1.13   31 Aug 1995 10:01:32   TIMB

 * Update revision #

 * 

 *    Rev 1.12   29 Aug 1995 16:47:56   TIMB

 * Fixed hash table initialization bug.

 * 

 *    Rev 1.11   22 Aug 1995 17:05:44   TIMB

 * Fixed randomly updated counters, pointers.

 * Fixed case where dest length was saved as 0 causing the saved pointer to be 

 * ignored in favor of the calling pointer.

 * 

 *    Rev 1.10   21 Aug 1995 14:00:10   TIMB

 * Added Steve Mickelsen fix mistakenly deleted.

 * 

 *    Rev 1.9   17 Aug 1995 17:47:44   TIMB

 * 4.23

 * Fixed undeclared var.

 * 

 *    Rev 1.8   17 Aug 1995 17:41:58   TIMB

 * 4.23

 * Merged in changes from BobL to rev 1.6 (ver 4.22)

 * 

 *    Rev 1.7   17 Aug 1995 16:19:06   TIMB

 * 4.23

 * Bumped Rev #

 * 

 *    Rev 1.6   Jul 13 1995 17:11:48   RFRIEND

 * Steve Mickelsen's bugs fixed:

 * 1)  0 Byte Source Flush in lzsc.c

 * 2) 15:1 max compression ratio in lzsc.c

 * 3) No EOC on Dest Flush in lzsc.c

 * 

 * 

 *    Rev 1.6   Jul 13 1995 11:55:52   RFRIEND

 * Steve Mickelsen's bugs fixed:

 * 1)  0 Byte Source Flush in lzsc.c

 * 2) 15:1 max compression ratio in lzsc.c

 * 3) No EOC on Dest Flush in lzsc.c

* 

*    Rev 1.0   Apr 24 1995 15:07:14   BOBL

*  

* 

*************************************************************************/



#include "LZS.H"	/* Compression definitions, structures, prototypes  */



#define MAX_BLOCK_SIZE_TYPE			long

#define SIZEOF_tOrigByteIndex		4

#define MAX_BUFFR_SIZE_TYPE			long



#if defined (LZS_FAST)

	#define _FAST_

	#define _Small_Blocks_

	#define MAX_ORIG_INDEX			OrigCnt

	#undef	LZS_SMALL_BUFFERS

	#define LZS_SMALL_BUFFERS

#endif



#if defined (_BIG_CMPRS_BFR_)

	#define MAX_ORIG_CNT_FAST		57343U

#else

	#define _EARLY_OUT_				1

	#define MAX_ORIG_CNT_FAST		63487U

#endif



#define MAX_INDEX					~((tOrigByteIndex) 0)



#if !defined MAX_ORIG_INDEX

	#define MAX_ORIG_INDEX \
		((flags & LZS_SOURCE_FLUSH) ? OrigCnt : MAX_INDEX)

#endif



#if defined (_Small_Blocks_)

	#undef	MAX_BLOCK_SIZE_TYPE

	#undef	SIZEOF_tOrigByteIndex

	#define MAX_BLOCK_SIZE_TYPE 	short

	#define SIZEOF_tOrigByteIndex 	2

#endif



#if defined (LZS_SMALL_BUFFERS)

	#undef	MAX_BUFFR_SIZE_TYPE

	#define MAX_BUFFR_SIZE_TYPE 	short

	#define MAX_BUFFER_SIZE 		0xFFFFU

#endif



typedef unsigned MAX_BUFFR_SIZE_TYPE tMaxBfrSz;

typedef unsigned MAX_BLOCK_SIZE_TYPE tOrigByteIndex;



#define FP_Off(fp) fp



#define dREG

#define pREG register





#define	MEM_COPY_S(pDest, pSrc, nbytes) (mem_copy_r (pDest, pSrc, nbytes))

#define	INIT_HASH_TBL(pDest, pPattern, nbytes) \
		init_hash_tbl_r (pDest, pPattern, nbytes)



#define LARGE_STR 				25U

#define RAW_TKN_SZ 				9

#define KEY_LENGTH 				2

#define MAX_OFFSET 				0x7FFU

#define CMP_SIGNATURE 			0x4321U

#define SPAN_CMPRS_BFR 			0x80

#define SPLITTING_STRING		0x40

#define SPLITTING_UNIQUE_BYTE	0x20

#define SPLITTING_ENDS			0x10

#define SPAN_ORIG_BFR			0x08

#define ORIG_BLK_START			0x04

#define ENGN_SOURCE_EXHAUSTED 	LZS_SOURCE_EXHAUSTED



#define INIT_HASH				0x02

#define NOT_ORIG_BFR			0x04



#define HASH_TBL_ENTRIES  		2003U

#define HASH_TBL_EVEN 			(HASH_TBL_ENTRIES+1)

#define MAX_LOOK_AHEAD 			4U

#define DUM_DUM_SZ 				(2U*(MAX_OFFSET)+MAX_LOOK_AHEAD)



#define SVD_SOURCE_VALID		1

#define SVD_DEST_VALID			2

#define SVD_INIT				0



#if !defined (LZS_FAR)

	#define LZS_FAR

#endif



struct Look4Match {

	unsigned long  Token;

	unsigned long  SaveBitStream;

	unsigned char LZS_FAR * pParsePt;

	tOrigByteIndex match_list[MAX_OFFSET+1];

	tOrigByteIndex hash_tbl[HASH_TBL_EVEN];

	unsigned char LZS_FAR * pStartUb;

	tOrigByteIndex ParseNdx;

	tOrigByteIndex StartNdx;

	tOrigByteIndex BeginAgain;

	tMaxBfrSz nUnqBytes;

	tMaxBfrSz StrLen;

	unsigned short OrigBytesSaved;

	unsigned short Ret_Code;

	unsigned short CmprsBytesSaved;

	unsigned short wBestOff;

	unsigned short TokenSz;

	unsigned short AvailBits;

	unsigned short Sign;

	unsigned char  BkState;

	unsigned char  EngnState;



#if !defined(_SAVE_COMPRESS_POINTERS_)

	unsigned long		   svd_sourceCnt;

	unsigned char LZS_FAR *svd_source;

	unsigned long		   svd_destCnt;

	unsigned char LZS_FAR *svd_dest;

	unsigned short         svd_flags;

	unsigned char          svd_status;

#endif



#if !defined (_FAST_)

	unsigned char  	PriorOrigBytes[DUM_DUM_SZ];

#endif



};



#define MAX_OVERRUN 	16U

#define DUMDUM_SZ (2*(MAX_OFFSET+1) + MAX_OVERRUN)



struct RestartData {

	tMaxBfrSz	 	len;

	unsigned short 	bits2discard;

	unsigned short 	offset;

	unsigned short	Sign;

	unsigned char	engnState;

};





#define	LZS_STOPPED_AT_TOKEN		0x01

#define	LZS_STOPPED_GETTING_LEN		0x02

#define	SPANNING_CMPRS_BFRS 		0x04

#define	LZS_OVERRUNNN				0x08

#define	TINY_EXPAND_BFR				0x10

#define	LZS_EXPANDING_STR 			0x20

#define	SAVED_CMPRS_BYTES			0x40

#define	LZS_END_OF_CMPRS_DATA		0x80





#define ENGN_STOPPED_AT_TOKEN		LZS_STOPPED_AT_TOKEN

#define	ENGN_STOPPED_GETTING_LEN	LZS_STOPPED_GETTING_LEN

#define	ENGN_PARTIAL_TOKEN			0x04

#define	ENGN_OVERRUNNN 				LZS_OVERRUNNN

#define	ENGN_EXPANDING_STR 			LZS_EXPANDING_STR

#define	ENGN_NEW_BFR             	0x40

#define	ENGN_END_OF_CMPRS_DATA		LZS_END_OF_CMPRS_DATA



unsigned int LZS_FAR Expand_Lzs_Engine(

	unsigned char LZS_FAR * LZS_FAR	*ppCmprsBfr,

	unsigned char LZS_FAR		*pCmprsBfrEnd,

	unsigned char LZS_FAR * LZS_FAR	*ppExpBfr,

	unsigned char LZS_FAR		*pExpBfrOverEnd,

	struct RestartData LZS_FAR 	*pRestartData

);



unsigned int LZS_FAR Expand_Null_Engine(

	unsigned char LZS_FAR * LZS_FAR *ppCmprsBfr,

	unsigned char LZS_FAR		*pCmprsBfrEnd,

	unsigned char LZS_FAR * LZS_FAR *ppExpBfr,

	unsigned char LZS_FAR		*pExpBfrOverEnd,

	struct RestartData LZS_FAR 	*pRestartData

);



static unsigned int LZS_FAR Compress_Lzs_Engine(

	unsigned char LZS_FAR * LZS_FAR *ppCmprsBfr,

	unsigned char LZS_FAR		*pCmprsBfrEnd,

	unsigned char LZS_FAR * LZS_FAR *ppOrigBfr,

	unsigned char LZS_FAR	   	*pOrigEnd,

	struct Look4Match LZS_FAR  	*pWrkSpace,

	unsigned short 			 	wOption,

	unsigned short			 	wSearchDepth

);



unsigned short LZS_FAR LZS_Compress(

	unsigned char LZS_FAR * LZS_FAR *source,

	unsigned char LZS_FAR * LZS_FAR *dest,

	unsigned long LZS_FAR     		*sourceCnt,

	unsigned long LZS_FAR     		*destCnt,

	void          LZS_FAR 			*history,

	unsigned short              	 flags,

	unsigned short              	 performance



);



#if !defined (_Lzs_Mem_R_H)

	#define	_Lzs_Mem_R_H



 	unsigned char LZS_FAR * LZS_FAR mem_copy_r (

		void LZS_FAR * pDestn,

		void LZS_FAR * pSource,

		unsigned short wNbytes

	);



	unsigned char LZS_FAR * LZS_FAR copy_prev_string_r (

		unsigned char LZS_FAR * pDestn,

		unsigned short 	wOffset,

		tMaxBfrSz 		wNbytes

	);



 	void LZS_FAR  init_hash_tbl_r (

		void LZS_FAR * pDestn,

		void LZS_FAR * pPattern,

		unsigned short wNbytes

	);

#endif



static const char copyright[] =

	"Lzs221-C version 4.01\nCopyright 1988-95 Stac Electronics, "

	"Carlsbad, California.\nAll rights reserved.\n"

	"One or more U.S. patents: 4701745, 5016009, 5126739, and 5146221 and\n"

	"other patents pending.";





/*----------------------------------------------------------------------*/

/* LZS_SizeOfCompressHistory -- Returns size of LZS_Compress's 			*/

/*				(call to call) memory block								*/

/*----------------------------------------------------------------------*/

/* LZS_Compress needs a history buffer to survive buffer spanning		*/

/*																		*/

/* In:																	*/

/*																		*/

/* Returns:																*/

/*	(unsigned short) Number of bytes needed for (call to call)			*/

/*		memory block													*/

/*----------------------------------------------------------------------*/



unsigned short LZS_FAR LZS_SizeOfCompressionHistory(void)

{

	return (sizeof (struct Look4Match));

}







/*----------------------------------------------------------------------*/

/* LZS_InitCompressionHistory --										*/

/*----------------------------------------------------------------------*/



unsigned short LZS_FAR LZS_InitCompressionHistory(

	void LZS_FAR *history				/* Ptr to compression history	*/

)

{

	struct Look4Match LZS_FAR *p_temp;  /* Temporary data structure     */



	p_temp = history ;		    		/* Assignment to access members */



	p_temp->OrigBytesSaved = 0;

	p_temp->Sign 	       = CMP_SIGNATURE;

	p_temp->BkState        = copyright[0];

	p_temp->BkState        = INIT_HASH;

	p_temp->ParseNdx       = MAX_INDEX;	/* Pick big init value to cause */

										/* hash table to be invalidated */



#if !defined(_SAVE_COMPRESS_POINTERS_)

	p_temp->svd_status      = SVD_INIT;

#endif



	return 1;

}







/*----------------------------------------------------------------------*/

/* LZS_Compress -- Compresses (original) data to a (compressed)			*/

/*		buffer of LZS tokens											*/

/*----------------------------------------------------------------------*/

/* In:																	*/

/*	source	 			pointer to pointer to Original data buffer		*/

/*	dest	 			pointer to pointer to (compressed) buffer		*/

/*	sourceCnt			pointer to size (in bytes) of Original buffer.	*/

/*	destCnt 			pointer to size (in bytes) of compress buffer.	*/

/*  history				pointer to (call to call) memory block			*/

/*  flags				Memory Control options (bit encoded):			*/

/*	LZS_SAVE_HISTORY	0x04  span original data buffers				*/

/*	LZS_DEST_FLUSH		0x02  Block compress buffers					*/

/*	LZS_SOURCE_FLUSH	0x01  High performance							*/

/*					 		  compression: original data in full Block;	*/

/*																		*/

/*	Performance mode controls sub-string insertion in the search		*/

/*	table																*/

/*       	LZS_PERF_MODE_0	    0X0000   None							*/

/*       	LZS_PERF_MODE_1     0x0008   Last character in string		*/

/*											and next					*/

/*       	LZS_PERF_MODE_2     0x0010   All							*/

/*											performance	Longest match	*/

/*											search limit, try n matches	*/

/*											to find best match 			*/

/*																		*/

/* Returns:																*/

/*	LZS_INVALID					0										*/

/*	LZS_SOURCE_EXHAUSTED		1                       		        */

/*	LZS_DEST_EXHAUSTED			2										*/

/*	LZS_SOURCE_DEST_EXHAUSTED	3										*/

/*	LZS_FLUSHED			  		4		    Control structures reset	*/

/*																		*/

/* Updates:																*/

/*	MaxCount		remaining bytes in the Original data buffer.		*/

/*			    	(Original size- # bytes processed)					*/

/*	pOrigBuffer   	Next original data byte to compress.				*/

/*	CmprsSize		remaining bytes in the compression buffer.			*/

/*		 	 	   	(Original size- # compressed bytes produced)		*/

/*	pCmprsBuffer    Next compressed byte								*/

/*																		*/

/* Error Handling:	Terminates if either source or destination			*/

/*					    counts are exhausted.							*/

/*																		*/

/*----------------------------------------------------------------------*/

/* Compressed data tokens (QIC-122 standard):							*/

/*	0<b8><b7><b6><b5><b4><b3><b2><b1> 		 							*/

/*		- 8-bit byte (not compressed)									*/

/*	String reference: offset to prior occurrence; length of string		*/

/*	11<o7><o6><o5><o4><o3><o2><o1>{len}	 		 						*/

/*		- 7 bit offset													*/

/*	10<o11><o10><o9><o8><o7><o6><o5><o4><o3><o2><o1>{len} 				*/

/*		- 11 bit offset													*/

/*		{len}:															*/

/*			00 = 2 bytes; 01 = 3 bytes; 10 =  4 bytes					*/

/*			11 00 = 5; 11 01 = 6;    11 10 =  7							*/

/*			11 11 0000 = 8; ... 11 11 1110 = 22							*/

/*			11 11 1111 0000 = 23; 11 11 1111 1110 = 37; etc.			*/

/*----------------------------------------------------------------------*/



/*----------------------------------------------------------------------*/

/* Performance Tuning Options											*/

/*----------------------------------------------------------------------*/

/* Using rational original blocks, compress buffer, and expansion 		*/

/* buffer sizes yield the best performance.  The excess processing 		*/

/* involved in spanning buffers, detecting end conditions, and double	*/

/* precision bookkeeping can be avoided by selecting the following:		*/

/*		Original block size:		< 64K;								*/

/*		Compression buffer size:	Original block size					*/

/*				 						+ 1/8 (Original block size) +2	*/

/*		LZS_SOURCE_FLUSH												*/

/*																		*/

/*		Compressed data size:		At least 4+ compressed data size	*/

/*		Expansion buffer size:    	Original block size +  MAX_OVERRUN	*/

/*																		*/

/* The speed and compression ratio of the LZS_Compress and 				*/

/* Compress_LzsR functions can be tuned by adjusting the values of 		*/

/* performance and the LZS_PERFORMANCE_MODE bits of the wOption 		*/

/* parameter.  Search depths can range from 0 to 255.  					*/

/* LZS_PERFORMANCE_MODE (values 0,1 or 2) control the sub-string		*/

/* insertion strategy.													*/

/*																		*/

/* Setting these parameters to the lowest possible values results in 	*/

/* the fastest speed; the highest possible values results in the 		*/

/* greatest compression ratios.											*/

/*																		*/

/* !!!ACTUAL PERFORMANCE DEPENDS ON ACTUAL DATA --						*/

/*	SELECTED BENCHMARK VALUES ARE:										*/

/* Setting:																*/

/* LZS_PERFORMANCE_MODE	 performance  CompressionRatio  Relative Speed	*/

/*			0				0				1.66			3.6			*/

/*							7				1.92			2.8			*/

/*							255             1.94            2.1			*/

/*			1				0				1.79			2.8			*/

/*							7				1.93			2.5			*/

/*							255				2.07			1.6			*/

/*			2				0				1.79			2.4			*/

/*							7				2.15			1.8			*/

/*							255				2.24			1.0			*/

/*																		*/

/*----------------------------------------------------------------------*/



unsigned short LZS_FAR	LZS_Compress(

	unsigned char LZS_FAR * LZS_FAR *source,    /* Source buffer        */

	unsigned char LZS_FAR * LZS_FAR *dest,      /* Dest   buffer        */

	unsigned long LZS_FAR	        *sourceCnt, /* Source count	    	*/

	unsigned long LZS_FAR	        *destCnt,   /* Destination buf size */

	void          LZS_FAR           *history,   /* Compression history  */

	unsigned short 		        	flags,	    /* Special flags	    */

	unsigned short		        	performance /* Performance params   */

)

{

	unsigned char LZS_FAR *pParse_Pt;  /* Pointer to current            */

									   /* Parsing point in original     */

	unsigned char LZS_FAR *pOrig_End;  /* Keep track of remaining bytes */

	unsigned char LZS_FAR *pCmprs_Bfr; /* Pointer to compress buffer    */

	unsigned char LZS_FAR *pCmprsEnd;  /* End of compress buffer        */

	unsigned long 	       Bit_Stream; /* Contain latest valid tokens   */

	tOrigByteIndex 	       start_ndx;  /* Starting parsing point index  */

									   /* Into Original byte stream     */

	struct Look4Match LZS_FAR *p_temp ;



#if !defined (_FAST_)

	tOrigByteIndex			DeltaNdx;   /* # of bytes parsed from start */

#endif



	tMaxBfrSz 	       		OrigCnt;	/* # of data bytes to compress  */



	unsigned short	       	wDataReg;   /* Misc data registers          */

	unsigned short	       	wDataReg2;

#if !defined (_FAST_)

	unsigned short	       	wDataReg3;

#endif



	unsigned short	       ret_code;   	/* Collects return status		*/

	unsigned int	       engn_ret;



#if !defined(_SAVE_COMPRESS_POINTERS_)

	unsigned char LZS_FAR *	call_source		= *source;

	unsigned char LZS_FAR *	call_dest		= *dest;

	unsigned long			call_sourceCnt	= *sourceCnt;

	unsigned long			call_destCnt	= *destCnt;

#endif



	p_temp = history ;		   			/* Structure assignment         */



	ret_code = 0;





	if (flags & (~(  LZS_SAVE_HISTORY

			 | LZS_SOURCE_FLUSH

			 | LZS_DEST_FLUSH

			 | LZS_PERFORMANCE_MODE_1

			 | LZS_PERFORMANCE_MODE_2 ) ))

		return (LZS_INVALID);



	if ( p_temp->Sign != CMP_SIGNATURE)

		return (LZS_INVALID);



#if !defined(_SAVE_COMPRESS_POINTERS_)

	if(p_temp->svd_status & SVD_SOURCE_VALID)	/* source buffer left over	*/

	{

		*sourceCnt = p_temp->svd_sourceCnt;

		*source	   = p_temp->svd_source;

#if 0

		if ((flags & LZS_SOURCE_FLUSH) != 

			(p_temp->svd_flags & LZS_SOURCE_FLUSH))

			return (LZS_INVALID);

#endif

	}

	if(p_temp->svd_status & SVD_DEST_VALID)		/* dest buffer left over	*/

	{

		*destCnt = p_temp->svd_destCnt;

		*dest    = p_temp->svd_dest;

#if 0

		if ((flags & LZS_DEST_FLUSH) != (p_temp->svd_flags & LZS_DEST_FLUSH))

			return (LZS_INVALID);

#endif

	}

#endif



	wDataReg2 = p_temp->BkState;

	pParse_Pt = *source;



#if defined (LZS_SMALL_BUFFERS)

	/* If Buffers are < 64k, Max counts are limited */



	if ((*destCnt > MAX_BUFFER_SIZE) || (*sourceCnt > MAX_BUFFER_SIZE))

		return (LZS_INVALID);

#endif



	OrigCnt = (tMaxBfrSz) *sourceCnt;	/* Bytes to process 			*/



#if defined (_FAST_)

	if (LZS_SOURCE_FLUSH != (flags & (

					      LZS_SOURCE_FLUSH

					    | LZS_SAVE_HISTORY

					    | LZS_DEST_FLUSH   )))

		return (LZS_INVALID);



	if (OrigCnt > MAX_ORIG_CNT_FAST)

		return (LZS_INVALID);

#endif



	if ( (*destCnt < LZS_DEST_MIN) && (!(wDataReg2 & SPAN_ORIG_BFR)) )

		return (LZS_INVALID);



	pCmprs_Bfr = *dest;

	pCmprsEnd = (tMaxBfrSz)(*destCnt - 4) + pCmprs_Bfr;



#if !defined (_FAST_)

	if (flags & LZS_DEST_FLUSH)

		pCmprsEnd -= LZS_DEST_MIN - 4;



	if (wDataReg2 & (  SPAN_CMPRS_BFR

			 | SPLITTING_STRING

			 | SPLITTING_UNIQUE_BYTE

			 | SPLITTING_ENDS )) {

		p_temp->BkState &= ~(  SPAN_CMPRS_BFR

					 | SPLITTING_STRING

					 | SPLITTING_UNIQUE_BYTE

					 | SPLITTING_ENDS);



		Bit_Stream = p_temp->SaveBitStream;

		wDataReg3 = 0;

		/* If splitting eocd, but not splitting string, i.e.

			eocd made it into bitstream and is not stuck in token,

			skip bytes in Bit_Stream which aren't relevant.*/

		if ((wDataReg2 & (SPLITTING_ENDS | SPLITTING_STRING)) == 

				SPLITTING_ENDS)

			wDataReg3 = (p_temp->AvailBits + 2) >> 3;



		for (wDataReg = p_temp->CmprsBytesSaved;

		     wDataReg > wDataReg3;

		     wDataReg--) {

			*pCmprs_Bfr = (unsigned char)(0xFF & (unsigned char)

				(Bit_Stream >> ((wDataReg-1)<<3)));

			pCmprs_Bfr++;

		}



		OrigCnt   = (tMaxBfrSz) p_temp->BeginAgain;

		pParse_Pt = p_temp->pParsePt;  		/* Pick up parser point		*/

					       					/* Point in (save) buffer	*/

		ret_code   = p_temp->Ret_Code;



		if (wDataReg2 & SPLITTING_ENDS) {

			if (wDataReg2 & SPLITTING_STRING) {



				/* Figure bytes to contain split EOCD marker -

				take off 2 bits for dummy length field, add 7 to round up */

				wDataReg = (p_temp->AvailBits - 2 + 7) >> 3;

				p_temp->AvailBits = (32 - p_temp->AvailBits);

				Bit_Stream = p_temp->Token << p_temp->AvailBits;

			goto Out_EocdToken;

			} else {

			goto Recover_From_Splitting_Ends;

			}

		}

		

		goto Restart_Compressor;

	}

#endif



	p_temp->StartNdx = p_temp->ParseNdx;



	if (wDataReg2 & INIT_HASH) {

		start_ndx = p_temp->ParseNdx;



		/* If compressor would overflow original Index counters,		*/

		/* re-init hash table											*/



		if (start_ndx > (MAX_INDEX - MAX_ORIG_INDEX) ) {

			start_ndx = 0;

			p_temp->hash_tbl[0] = 0;



			#if SIZEOF_tOrigByteIndex == 2

				p_temp->hash_tbl[1] = 0;

			#endif



			INIT_HASH_TBL(

			  &p_temp->hash_tbl[4/(sizeof (tOrigByteIndex))],

			  &p_temp->hash_tbl[0],

			  (HASH_TBL_EVEN * (sizeof(tOrigByteIndex))) - 4);

		}



		/* New start index = last index + larger than possible  		*/

		/* match (MAX_OFFSET+1)											*/



		start_ndx += MAX_OFFSET+1u;

		p_temp->StartNdx =

		p_temp->ParseNdx = start_ndx;

		p_temp->OrigBytesSaved = 0;

		p_temp->BkState = 0;

		p_temp->SaveBitStream = 0;

		p_temp->AvailBits = 32;

	}



#if !defined (_FAST_)

	else if (wDataReg2 & SPAN_ORIG_BFR) {

		p_temp->BkState &= ~SPAN_ORIG_BFR;



Insert_In_Save_Bfr:

		p_temp->BkState |= NOT_ORIG_BFR;

		wDataReg  = p_temp->OrigBytesSaved;



		if ( (wDataReg > MAX_OFFSET) &&

			((wDataReg + OrigCnt) > DUM_DUM_SZ)){

			wDataReg -= MAX_OFFSET;

			MEM_COPY_S(&p_temp->PriorOrigBytes[0],

				   &p_temp->PriorOrigBytes[wDataReg],

				   MAX_OFFSET);

			wDataReg = MAX_OFFSET;

		}





		if (OrigCnt > (DUM_DUM_SZ - wDataReg))

			/* Use only what fits in save buffer */

			OrigCnt = (tMaxBfrSz)(DUM_DUM_SZ - wDataReg);



		MEM_COPY_S (&p_temp->PriorOrigBytes[wDataReg],

			    pParse_Pt, (unsigned short)OrigCnt);



		pParse_Pt = &p_temp->PriorOrigBytes[wDataReg];

		p_temp->OrigBytesSaved = wDataReg + (unsigned short)OrigCnt;

	}

#endif  /* End of not FAST code */



	p_temp->pStartUb = pParse_Pt;



#if !defined (_FAST_)

Resume_Compressing:

#endif



	p_temp->EngnState = ORIG_BLK_START;



/*----------------------- Compress data --------------------------------*/



Restart_Compressor:

	pOrig_End = pParse_Pt + OrigCnt;

	engn_ret = Compress_Lzs_Engine(

			&pCmprs_Bfr, pCmprsEnd,

			&pParse_Pt,  pOrig_End,

			(struct Look4Match LZS_FAR *)history,

			flags, performance );



	if ((engn_ret & SPLITTING_STRING) && (ret_code & LZS_SOURCE_EXHAUSTED)

		&& (flags & LZS_SOURCE_FLUSH)) {

		goto SourceFlushEOCD;

	}

		

	if (engn_ret & (  SPLITTING_UNIQUE_BYTE | SPLITTING_STRING

			| SPAN_CMPRS_BFR ))

		goto Save_Cmprs_Info;

	

	if (engn_ret & SPAN_ORIG_BFR)

		goto Span_Orig_Bfr;



	if (!(ret_code & LZS_SOURCE_EXHAUSTED)) {



		#if !defined (_FAST_)

		if (p_temp->BkState & NOT_ORIG_BFR) {



			/* May have more N Original buffer */

			p_temp->BkState &= ~NOT_ORIG_BFR;



			DeltaNdx = p_temp->ParseNdx - p_temp->StartNdx;



			wDataReg  = (unsigned short)(pParse_Pt

					- p_temp->pStartUb);

				/* Number of remaining unique bytes */



			pParse_Pt = *source + (tMaxBfrSz)DeltaNdx ;

			p_temp->pStartUb = pParse_Pt - wDataReg;



			OrigCnt = (tMaxBfrSz) *sourceCnt

					- (tMaxBfrSz)DeltaNdx;



			if (DeltaNdx >= MAX_OFFSET) {

				/* More N Original buffer */

				p_temp->OrigBytesSaved = 0;

				goto Resume_Compressing;

			}

			if (OrigCnt > 4)

				goto Insert_In_Save_Bfr;

		}

		#endif	/* End of not FAST code */



		ret_code |= LZS_SOURCE_EXHAUSTED;

		p_temp->ParseNdx = p_temp->StartNdx + (tMaxBfrSz) *sourceCnt;



		/* Out all remaining bytes */



		pParse_Pt = *source + (tMaxBfrSz) *sourceCnt;

		OrigCnt   = 0;



		/* End code is a zero offset/min len string */



		p_temp->wBestOff  = 0;

		p_temp->StrLen 	 = 2;



		if (!(flags & LZS_SOURCE_FLUSH)) {

			/* Spanning original byte buffers */



			p_temp->BkState |= SPAN_ORIG_BFR;

			/* Not done yet, out Origbytes code is a */

			/*   max offset - big len string 	 */



			p_temp->wBestOff = MAX_OFFSET;

			p_temp->StrLen 	 = 200;

		}

		goto Restart_Compressor;

			/* Flush final dif bytes; append EOCD code */

	}



SourceFlushEOCD:

	pParse_Pt = *source + (tMaxBfrSz) *sourceCnt;

	/* Report all bytes compressed */



	if (engn_ret & SPLITTING_STRING)

	/* Bytes left is one dword plus excess bits take away two for dummy

	length rounded up to nearest byte */

		wDataReg = 4 + ((p_temp->AvailBits - 2 + 7) >> 3);

	else

	/* Bytes left to output is 4 take away available bytes.

	Available bytes is AvailBits + 2 for the 2-bit dummy length field

	rounded down to the next lower byte. */

		wDataReg = 4 - ((p_temp->AvailBits + 2) >> 3);



	if ((pCmprs_Bfr + wDataReg) > (pCmprsEnd + ( (flags & LZS_DEST_FLUSH) ? 16 : 4 ))) {

		engn_ret |=  SPLITTING_ENDS;

		goto Save_Cmprs_Info;

	}



	Bit_Stream = p_temp->SaveBitStream;



	if (engn_ret & SPLITTING_STRING) {

		#if (defined (LZS_ALIGNED) || (LZS_BYTE_ORDER == LZS_LITTLE_END))

		    *(pCmprs_Bfr+3) = (unsigned char) Bit_Stream;



		    /* Low byte last */

		    Bit_Stream >>= 8;

		    *(pCmprs_Bfr+2) = (unsigned char) Bit_Stream;

		    Bit_Stream >>= 8;

		    *(pCmprs_Bfr+1) = (unsigned char) Bit_Stream;

		    Bit_Stream >>= 8;

		    *pCmprs_Bfr     = (unsigned char) Bit_Stream;

		#else

		    * (unsigned long *) pCmprs_Bfr = Bit_Stream;

		#endif



		pCmprs_Bfr += 4;

		wDataReg -= 4;

		p_temp->AvailBits = (32 - p_temp->AvailBits);



		Bit_Stream = (p_temp->AvailBits == 0) ? 

			0 : p_temp->Token << p_temp->AvailBits;

	}



#if !defined (_FAST_)

Out_EocdToken:

#endif



	pCmprsEnd = pCmprs_Bfr + wDataReg;

	wDataReg = 3*8;



	while (pCmprs_Bfr < pCmprsEnd) {

		*pCmprs_Bfr = (unsigned char)(0xFFU & (unsigned char)

			  (Bit_Stream >> wDataReg));

		pCmprs_Bfr++;

		wDataReg -= 8;

	};



#if !defined (_FAST_)

Recover_From_Splitting_Ends:

#endif



	/* Expander SHOULD have a 3-byte look-ahead 		*/

	/* Count after the compressed data 			*/

	/* pCmprs_Buffer += 2+(4 - (history->TokenSz/8)); 	*/

	/* Report min byte compressed data 			*/

	/* EOCD token is 9 bits-string added 11 		*/



	ret_code |= LZS_FLUSHED;



	if (flags & LZS_SAVE_HISTORY) {

		p_temp->SaveBitStream = 0;

		p_temp->AvailBits     = 32;



		/* If out of original data, save enought	  */

		/*   context to survive compress buffer switching */



Span_Orig_Bfr:

#if !defined (_FAST_)

		p_temp->BkState |= SPAN_ORIG_BFR;



		if (p_temp->OrigBytesSaved == 0) {

			wDataReg = ((tOrigByteIndex) *sourceCnt < MAX_OFFSET) ?

				(unsigned short) *sourceCnt : MAX_OFFSET;



		MEM_COPY_S(&p_temp->PriorOrigBytes[0],

			   (*source + (tMaxBfrSz)*sourceCnt) - wDataReg,

			   wDataReg);



		/* Save context for a restart*/



		p_temp->OrigBytesSaved  = wDataReg;

	}



	if (ret_code & LZS_SOURCE_EXHAUSTED) {

		p_temp->ParseNdx = p_temp->StartNdx + *sourceCnt;

		pParse_Pt = *source + (tMaxBfrSz) *sourceCnt;

	}



	/* Process all bytes in original buffer */

#endif



	} else if (flags & LZS_SOURCE_FLUSH) {

		p_temp->BkState |= INIT_HASH;

	}



	goto Report_Results;



	/* Ran out of room for compressed data 				   */

	/*	- Terminate compression stream or 			   */

	/*	- save enough context to survive compress buffer switching */



Save_Cmprs_Info:

	p_temp->BkState  |= engn_ret;

	p_temp->Ret_Code  = ret_code;

	p_temp->pParsePt  = pParse_Pt;

	p_temp->BeginAgain = (tOrigByteIndex)(pOrig_End - pParse_Pt);



	ret_code = LZS_DEST_EXHAUSTED;



#if defined (_FAST_)

	p_temp->BkState |= INIT_HASH;	/* Reset hash for next call */

#else

	if (flags & LZS_DEST_FLUSH) {

		unsigned short 	AvailBits = p_temp->AvailBits;



		Bit_Stream = p_temp->SaveBitStream;



		#if (defined (LZS_ALIGNED) || (LZS_BYTE_ORDER == LZS_LITTLE_END))

		    *(pCmprs_Bfr+3) = (unsigned char) Bit_Stream;



		    /* Low byte last */

		    Bit_Stream >>= 8;

		    *(pCmprs_Bfr+2) = (unsigned char) Bit_Stream;

		    Bit_Stream >>= 8;

		    *(pCmprs_Bfr+1) = (unsigned char) Bit_Stream;

		    Bit_Stream >>= 8;

		    *pCmprs_Bfr     = (unsigned char) Bit_Stream;

		#else

		    * (unsigned long *) pCmprs_Bfr = Bit_Stream;

		#endif



		pCmprs_Bfr += 4;

		AvailBits = (32 - AvailBits);



		Bit_Stream = (AvailBits == 0) ? 0 : p_temp->Token << AvailBits;



		{

			tOrigByteIndex	delta_p = 0;		/* Number of bytes pParsePt must back up */



			/* If in the middle of outputting a length, spit out last nybble*/

			if ((engn_ret & SPLITTING_STRING) && (p_temp->StrLen >= 15)) {

				/* length_remaining is bytes left to output in string.  */

				tMaxBfrSz		length_remaining = p_temp->StrLen - 15;

				/* last_nybble_length is max string length that can be encoded

					by adding a single nybble to the length field. */

			    tMaxBfrSz		last_nybble_length;	

			    

				last_nybble_length = length_remaining;

				if (last_nybble_length > 14)

					last_nybble_length = 14;

			

				if (AvailBits > 4) {

					AvailBits -= 4;

					Bit_Stream |= last_nybble_length << AvailBits;

				} else {

					AvailBits = 4 - AvailBits;

					Bit_Stream |= last_nybble_length >> AvailBits;

					#if (defined (LZS_ALIGNED) || (LZS_BYTE_ORDER == LZS_LITTLE_END))

		    			*(pCmprs_Bfr+3) = (unsigned char) Bit_Stream;



		    			/* Low byte last */

		    			Bit_Stream >>= 8;

		    			*(pCmprs_Bfr+2) = (unsigned char) Bit_Stream;

		    			Bit_Stream >>= 8;

		    			*(pCmprs_Bfr+1) = (unsigned char) Bit_Stream;

		    			Bit_Stream >>= 8;

		    			*pCmprs_Bfr     = (unsigned char) Bit_Stream;

					#else

		    			* (unsigned long *) pCmprs_Bfr = Bit_Stream;

					#endif



					pCmprs_Bfr += 4;

					AvailBits = (32 - AvailBits);



					Bit_Stream = (AvailBits == 0) ? 0 :

			      			last_nybble_length << AvailBits;

				}

				delta_p = length_remaining - last_nybble_length;

			}

			

			/* If in the middle of outputting a bunch of raw bytes,

				back up source ptr to last raw byte output. */

			if (engn_ret & (SPAN_CMPRS_BFR | SPLITTING_UNIQUE_BYTE)) {

				/* pStartUb trails last byte output by one for SPLITTING_UNIQUE_BYTE */

				delta_p += pParse_Pt - (p_temp->pStartUb + 1);

				/* pStartUb trails last byte output by two for SPAN_CMPRS_BFR */

				if (engn_ret & SPAN_CMPRS_BFR)

					delta_p--;

			}



			/* Insert EOCD token */



			if (AvailBits > 9) {

				AvailBits -= 9;

				Bit_Stream |= 0x0180L << AvailBits;

			} else {

				AvailBits = 9 - AvailBits;

				Bit_Stream |= 0x0180L >> AvailBits;

				#if (defined (LZS_ALIGNED) || (LZS_BYTE_ORDER == LZS_LITTLE_END))

		    		*(pCmprs_Bfr+3) = (unsigned char) Bit_Stream;



		    		/* Low byte last */

		    		Bit_Stream >>= 8;

		    		*(pCmprs_Bfr+2) = (unsigned char) Bit_Stream;

		    		Bit_Stream >>= 8;

		    		*(pCmprs_Bfr+1) = (unsigned char) Bit_Stream;

		    		Bit_Stream >>= 8;

		    		*pCmprs_Bfr     = (unsigned char) Bit_Stream;

				#else

		    		* (unsigned long *) pCmprs_Bfr = Bit_Stream;

				#endif



				pCmprs_Bfr += 4;

				AvailBits = (32 - AvailBits);



				Bit_Stream = (AvailBits == 0) ? 0 :

			    		0x0180L << AvailBits;

			}

		

			wDataReg = 4 - (AvailBits >> 3);

			p_temp->BkState = (p_temp->BkState & NOT_ORIG_BFR) | INIT_HASH;

			ret_code |= LZS_FLUSHED;



			if (p_temp->BkState & NOT_ORIG_BFR) {

				if (p_temp->EngnState & (SPLITTING_UNIQUE_BYTE | SPAN_CMPRS_BFR))

					delta_p += p_temp->StrLen;

				pParse_Pt = *source + p_temp->ParseNdx - p_temp->StartNdx;

			}

			pParse_Pt -= delta_p;

		}

		

		p_temp->AvailBits = AvailBits;

		goto Out_EocdToken;

	}



	pParse_Pt = *source;



	Bit_Stream = p_temp->SaveBitStream;

	wDataReg2 = (p_temp->CmprsBytesSaved =

		(unsigned short)(pCmprs_Bfr - pCmprsEnd) );



	for (wDataReg = 3; wDataReg >= wDataReg2; wDataReg--) {

		*pCmprs_Bfr = (unsigned char)(Bit_Stream >> ((wDataReg)<<3));

		pCmprs_Bfr++;

	}

#endif	/* End not FAST code */



Report_Results:



	*destCnt   -= FP_Off(pCmprs_Bfr) - FP_Off(*dest);

	*dest	    = pCmprs_Bfr;

	*sourceCnt -= FP_Off(pParse_Pt) - FP_Off(*source);

	*source   = pParse_Pt;



#if !defined(_SAVE_COMPRESS_POINTERS_)



	/* By default, clear saved counter, ptr information */

	p_temp->svd_status = SVD_INIT;

	

	/* If not flushed or source exhausted, save src cnt/ptr, restore values */

	if(!(ret_code & (LZS_FLUSHED | LZS_SOURCE_EXHAUSTED)))

	{

		/* Save current values */

		p_temp->svd_sourceCnt = *sourceCnt;

		p_temp->svd_source	  = *source;

		p_temp->svd_status    = SVD_SOURCE_VALID;

		p_temp->svd_flags     = (p_temp->svd_flags & ~LZS_SOURCE_FLUSH) 
						| (flags & LZS_SOURCE_FLUSH);



		/* Restore original values */

		*source = call_source;

		*sourceCnt = call_sourceCnt;

	}



	/* If not flushed or dest exhausted, save dst cnt/ptr, restore values */

	if(!(ret_code & (LZS_FLUSHED | LZS_DEST_EXHAUSTED)))

	{

		/* Save current values */

		p_temp->svd_destCnt = *destCnt;

		p_temp->svd_dest	= *dest;

		p_temp->svd_status  |= SVD_DEST_VALID;

		p_temp->svd_flags     = (p_temp->svd_flags & ~LZS_DEST_FLUSH)
						| (flags & LZS_DEST_FLUSH);



		/* Restore original values */

		*dest = call_dest;

		*destCnt = call_destCnt;

	}



#endif



	return (ret_code);

}



/*----------------------------------------------------------------------*/

/*			Begining of the compression engine							*/

/*----------------------------------------------------------------------*/





/*----------------------------------------------------------------------*/

/* Compress_Lzs_Engine - Fast Compressor (original) data to a 			*/

/*				(compressed) buffer of LZS tokens						*/

/*----------------------------------------------------------------------*/

/* In:																	*/

/*	dest	(referenced) pointer to (compressed) buffer					*/

/*	pCmprsBfrEnd    Compress buffer end									*/

/*	ppOrigBfr 	(referenced) pointer to Original data buffer			*/

/*	pOrigBfrEnd     Original data buffer end							*/

/*																		*/

/* Returns:																*/

/*	ENGN_																*/

/*	ENGN_																*/

/*	ENGN_																*/

/*	ENGN_																*/

/*	ENGN_																*/

/* Updates:																*/

/*	pExpandBuffer   Next expansion byte.								*/

/*	pCmprsBuffer    Next compressed byte								*/

/*																		*/

/* Terminates when either source or destination end is encountered.     */

/*----------------------------------------------------------------------*/

/* Compressed data tokens (QIC-122 standard):							*/

/*	0<b8><b7><b6><b5><b4><b3><b2><b1>									*/

/*		- 8-bit byte (not compressed)									*/

/* String reference: offset to prior occurrence; length of string		*/

/*	11<o7><o6><o5><o4><o3><o2><o1>{len}  								*/

/*		-  7 bit offset													*/

/*	10<o11><o10><o9><o8><o7><o6><o5><o4><o3><o2><o1>{len}				*/

/*		- 11 bit offset													*/

/*	{len}:																*/

/*		00 = 2 bytes; 01 = 3 bytes; 10 =  4 bytes						*/

/*		11 00 = 5; 11 01 = 6;    11 10 =  7								*/

/*		11 11 0000 = 8; ... 11 11 1110 = 22								*/

/*		11 11 1111 0000 = 23; 11 11 1111 1110 = 37; etc.				*/

/*																		*/

/*----------------------------------------------------------------------*/



static unsigned int LZS_FAR Compress_Lzs_Engine(

	unsigned char LZS_FAR * LZS_FAR *ppCmprsBfr,

	unsigned char LZS_FAR	        *pCmprsEnd,

	unsigned char LZS_FAR * LZS_FAR *ppOrigBfr,

	unsigned char LZS_FAR	        *pOrigEnd,  /* End of original data */

	struct Look4Match LZS_FAR       *pWrkSpace,

	unsigned short 		         	flags,

	unsigned short		         	performance

)

{

	unsigned long		 BitStream;		/* Contains the latest	    	*/

										/*   valid tokens 	    		*/

	dREG unsigned long 	 token;			/* Holds token(s) 	    		*/

										/*   white it is built 	    	*/

	unsigned char LZS_FAR 	*pCmprsBfr;	/* Ptr to compress buffer   	*/

	pREG unsigned char LZS_FAR *pParsePt;/* Ptr to current parsing   	*/

										/*   point in original byte 	*/

										/*   buffer 		    		*/

	unsigned char LZS_FAR 	*pStartUb; 	/* Ptr to Unique byte start 	*/

	pREG unsigned char LZS_FAR  *pOrigStop; /* Stop compressing here    */

	unsigned char LZS_FAR 	*pDi;	 	/* Misc pointers 	    		*/

	unsigned char LZS_FAR 	*pSi;



	pREG tOrigByteIndex	 parse_ndx;		/* Parsing point index into 	*/

										/*   Original byte stream   	*/

	dREG tOrigByteIndex	 match_ndx; 	/* Possible match index     	*/

	dREG tOrigByteIndex	 Delta2Match;  	/* Offset to match          	*/

	tOrigByteIndex		 i_ndx;	 		/* Misc index reg           	*/



	dREG tMaxBfrSz	 	 wMatchLen; 	/* String match length      	*/

	dREG tMaxBfrSz 		 BytePairsLeft; /* # of pairs of bytes to   	*/

										/* End of buffer	    		*/

	dREG tMaxBfrSz		 MaxReSearch;	/* # of pairs to end of long	*/

										/* token */

	dREG tMaxBfrSz 		 nUnqBytes;

	tMaxBfrSz 		 	 wBestLen;		/* Longest match found      	*/

	unsigned short		 wBestOff;		/*   and its offset  	    	*/

	unsigned short		 avail_bits;	/* Amount of room in	    	*/

										/* BitStream for tokens     	*/

	dREG unsigned short	 wMatchTried;	/* # of 4-byte matches tried	*/



#if !defined (LZS_ALIGNED)

	pREG unsigned short 	 wKeyBytes1_2;

#endif



	dREG unsigned short	 wToken_size;

	unsigned short 		 x_rem;

	unsigned short		 hashTblEntries;/* Hash table size (divisor)	*/

	unsigned int		 stopped4;		/* Return code		    		*/



	hashTblEntries = HASH_TBL_ENTRIES;

	performance  &= 0xFF;			/* Truncate overstated search limit */

	wMatchTried    = performance;	/* Limit searches 	 	    		*/

	pCmprsBfr = pCmprsEnd;			/* Avoid compiler warning 	    	*/

	pCmprsBfr = *ppCmprsBfr;

	pParsePt  = *ppOrigBfr;



	pOrigStop = ((pOrigEnd - pParsePt) < MAX_LOOK_AHEAD) ?

		pParsePt: pOrigEnd - MAX_LOOK_AHEAD;



	token     = pWrkSpace->Token;

	BitStream = pWrkSpace->SaveBitStream;

	pStartUb  = pWrkSpace->pStartUb;

	parse_ndx = pWrkSpace->ParseNdx;

	nUnqBytes = pWrkSpace->nUnqBytes;   /* Needed if compress			*/

										/*   buffer was tiny 			*/

	wBestLen    = pWrkSpace->StrLen;

	wBestOff    = pWrkSpace->wBestOff;

	wToken_size = pWrkSpace->TokenSz;

	avail_bits  = pWrkSpace->AvailBits;



	/* (Re)start compressor */



	if (pWrkSpace->EngnState & ORIG_BLK_START)

		goto LookUpKey;



	/* Flush add end code */



	if (pWrkSpace->EngnState & ENGN_SOURCE_EXHAUSTED)

		goto Encode;



	if (pWrkSpace->EngnState & SPLITTING_UNIQUE_BYTE)

		goto Recover_From_Cmprs_Bfr_Split_Unique_Byte;



	if (pWrkSpace->EngnState & SPAN_CMPRS_BFR)

		goto Recover_From_Cmprs_Bfr_Split;



	goto Recover_From_Cmprs_Bfr_Split_String;



	/* End of Original bytes buffer */



Thats_All:

	stopped4 = ENGN_SOURCE_EXHAUSTED;



Save_Cmprs_State:

	*ppOrigBfr  = pParsePt;		/* Update ptrs and save values 	   */

	*ppCmprsBfr = pCmprsBfr;

	pWrkSpace->Token         = token;     /* Needed if string token    */

					      /*   won't fit 		   */

	pWrkSpace->SaveBitStream = BitStream;

	pWrkSpace->pStartUb      = pStartUb;

	pWrkSpace->ParseNdx      = parse_ndx;

	pWrkSpace->nUnqBytes     = nUnqBytes; /* Needed if compress buffer */

					      /*   too small 		   */

	pWrkSpace->StrLen    = wBestLen;

	pWrkSpace->wBestOff  = wBestOff;

	pWrkSpace->TokenSz   = wToken_size;

	pWrkSpace->AvailBits = avail_bits;

	pWrkSpace->EngnState = (unsigned char) stopped4;

	return (stopped4);



Reset_Match_Tried:

	wMatchTried = performance;



Try_Next_Byte:

	parse_ndx++; pParsePt++;



LookUpKey:

	if (pParsePt >= pOrigStop)

		goto Thats_All;



	#if defined (LZS_ALIGNED)

	    x_rem = ((*pParsePt << 8) | *(pParsePt+1)) % hashTblEntries;



	    /* Get key to matching string */



	#else

	    wKeyBytes1_2 = *(unsigned short LZS_FAR *) pParsePt;



	    /* Quick access to key */



	    x_rem = wKeyBytes1_2 % hashTblEntries;

	#endif



	match_ndx = pWrkSpace->hash_tbl[x_rem];

	pWrkSpace->hash_tbl[x_rem] = parse_ndx;

	pWrkSpace->match_list[(((unsigned short) parse_ndx) & MAX_OFFSET)] =

		match_ndx;



	/* Search list to find key match */



	if ((Delta2Match = parse_ndx - match_ndx) > MAX_OFFSET)

		goto Try_Next_Byte;



	#if defined (LZS_ALIGNED)

	    pSi = pParsePt - (unsigned short) Delta2Match;

	    if ((* pSi    != * pParsePt)  ||

		(*(pSi+1) != *(pParsePt+1)) ){	/* Find a key match */

	#else

		if (*(unsigned short LZS_FAR *)

		(pParsePt - (unsigned short) Delta2Match) != wKeyBytes1_2) {

		/* Find a key match */

	#endif



		if (wMatchTried == 0)

			goto Try_Next_Byte;



Validate_Index: /* Get next in list */

		match_ndx = pWrkSpace->match_list[(((unsigned short)

			match_ndx) & MAX_OFFSET)];



		if ((Delta2Match = parse_ndx - match_ndx) > MAX_OFFSET)

			goto Reset_Match_Tried;



		#if defined (LZS_ALIGNED)

		    pSi = pParsePt - (unsigned short) Delta2Match;

		    if ((* pSi    != * pParsePt)   ||

			(*(pSi+1) != *(pParsePt+1)) ){

			/* Find a key match */

		#else

		    if (*(unsigned short LZS_FAR *)(pParsePt - (unsigned short)

			Delta2Match) != wKeyBytes1_2) {

			/* Find a key match */

		#endif

			wMatchTried--;

			if (wMatchTried == 0) goto Reset_Match_Tried;

				goto Validate_Index;

		}

	}



	wBestLen = KEY_LENGTH;		/* Found a key match */

	wBestOff = (unsigned short) Delta2Match;

	wMatchTried = performance;	/* Limit search */

	goto MatchByte3;



NextNlist:	/* Find a 3-byte match */

	match_ndx = pWrkSpace->match_list[(((unsigned short)

		match_ndx) & MAX_OFFSET)];

	Delta2Match = parse_ndx - match_ndx;



	if ((wMatchTried == 0) || (Delta2Match > MAX_OFFSET))

		goto FoundString;



	wMatchTried--;



MatchByte3:

	pSi = pParsePt - (unsigned short) Delta2Match;



	if ( (*(pParsePt+2) != *(pSi+2)) ||



	#if defined (LZS_ALIGNED)

		(*(pParsePt+1) != *(pSi+1)) ||

		( *pParsePt    !=  *pSi   )

	#else

		(wKeyBytes1_2  !=

		*(unsigned short LZS_FAR *) pSi   )

	#endif

		) goto NextNlist;



	wBestLen++;	/* Found a 3-byte match */

	wBestOff = (unsigned short) Delta2Match;



	/* Search list to find a 4 or more match */



	wMatchTried = performance;	/* Reset search limit */



	#if defined (LZS_ALIGNED)

	    while (

	    (*((pParsePt - (unsigned short)Delta2Match)+3) != *(pParsePt+3)) ||

	    (*((pParsePt - (unsigned short)Delta2Match)+2) != *(pParsePt+2)) ||

	    (*((pParsePt - (unsigned short)Delta2Match)+1) != *(pParsePt+1)) ||

	    ( *(pParsePt - (unsigned short)Delta2Match)    != * pParsePt  )

		) {

	#else

	    while (*(unsigned long LZS_FAR *)

		(pParsePt - (unsigned short) Delta2Match) !=

			*(unsigned long LZS_FAR *) pParsePt) {

		/* Quick access to string */

	#endif



Next4byteMatch:

		if (wMatchTried) {

			match_ndx = pWrkSpace->match_list[(((unsigned short)

				match_ndx) & MAX_OFFSET)];



			/* Get next in list */



			Delta2Match = parse_ndx - match_ndx;

			if (Delta2Match <= MAX_OFFSET) {

				wMatchTried--;

				continue;

			}

		}

		goto FoundString;

	}



	/* Find length of remaining matching bytes 			*/

	/* In:								*/

	/*	pParsePt	current parsing point 			*/

	/*	Delta2Match	offset to 4-byte match in window 	*/

	/* Out:								*/

	/*	MatchLen	length of match (including the key 	*/



	pSi = pParsePt + 4;

	pDi = pSi - (unsigned short) Delta2Match;



	if (* pSi == * pDi) {

		pSi++;	pDi++;

		BytePairsLeft = (tMaxBfrSz)((pOrigEnd - pSi) >> 1);



		if (BytePairsLeft > LARGE_STR)

			MaxReSearch = BytePairsLeft - LARGE_STR;

		else

			MaxReSearch = 0U;



		while (BytePairsLeft) {



		    #if defined (LZS_ALIGNED)

			if (* pSi    != * pDi)

				break;

			if (*(pSi+1) != *(pDi+1))  {

				pSi++;	/* Same 1st byte & different 2nd */

		    #else

			if (*(unsigned short LZS_FAR *)pSi !=

				*(unsigned short LZS_FAR *)pDi) {

				if (*(pSi) == *(pDi)) pSi++;

					/* Same 1st byte & different 2nd */

		    #endif

				break;

			}

			pSi += sizeof(unsigned short);

			pDi += sizeof(unsigned short);

			BytePairsLeft--;

			if (BytePairsLeft < MaxReSearch)

			{

				wMatchTried = 0;		/* no more searches  */

				MaxReSearch = 0;		/* no more true if's */

			}

		}

	}



	wMatchLen = (tMaxBfrSz)(pSi - pParsePt);



	if (wMatchLen > wBestLen) {

		wBestLen = wMatchLen;	/* Longest match found */

		wBestOff = (unsigned short) Delta2Match;

	}



	if (wMatchTried--)

		goto Next4byteMatch;



FoundString:

	/* (Optionally insert sub-strings in the Hash Table */



	if (flags & (LZS_PERFORMANCE_MODE_1 | LZS_PERFORMANCE_MODE_2)) {

		pSi = pDi = pParsePt + (wBestLen-1);

		i_ndx = parse_ndx;



		if (wBestLen > 2) {

			if ((flags & LZS_PERFORMANCE_MODE_2) && (wBestOff>1))

				 pSi = pParsePt+1;

			else i_ndx += (wBestLen-1) - 1;

		}

		do {

			i_ndx++;



			#if defined (LZS_ALIGNED)



			    /* Get key to matching string */



			    x_rem = ((*pSi << 8) | *(pSi+1)) % hashTblEntries;

			#else

			    x_rem = *(unsigned short LZS_FAR *)pSi % hashTblEntries;

			#endif



			pWrkSpace->match_list[(((unsigned short) i_ndx) &

				MAX_OFFSET)] = pWrkSpace->hash_tbl[x_rem];

			pWrkSpace->hash_tbl[x_rem] = i_ndx;

			pSi++;

		} while (pDi >= pSi);

	}

	parse_ndx += wBestLen;	/* Resume parsing after the string */



	/* Encode Unique bytes 						*/

	/* In: 								*/

	/*	pStartUb	pointer to start of unique bytes 	*/

	/*	pParsePt - current parsing point                	*/



Encode:

	if ((nUnqBytes = (tMaxBfrSz)(pParsePt - pStartUb)) != 0) {

		if (nUnqBytes & 0x0001) {     /* Have an odd # of bytes */

			if (avail_bits > RAW_TKN_SZ) {

				avail_bits -= RAW_TKN_SZ;

				BitStream |= ((unsigned long) *pStartUb) <<

					avail_bits;

			}

			else  {

				token = (unsigned long) *pStartUb;

				avail_bits = RAW_TKN_SZ - avail_bits;

				BitStream  |= token >> avail_bits;



				/* Put BitStream in compress buffer */



				#if defined (_EARLY_OUT_)

				    /* Can avoid this test; if 		 */

				    /*   compress data buffer big enough */



				    if (pCmprsBfr > pCmprsEnd) {

					stopped4 =  SPLITTING_UNIQUE_BYTE;

					goto Save_Cmprs_State;

				}

				#endif



				#if (defined (LZS_ALIGNED) || \
						(LZS_BYTE_ORDER == LZS_LITTLE_END))

				    /* Map regs to memory */



				    *(pCmprsBfr+3) = (unsigned char)BitStream;



				    /* Low byte last */

				    BitStream >>=  8;

				    *(pCmprsBfr+2) = (unsigned char)BitStream;

				    BitStream >>=  8;

				    *(pCmprsBfr+1) = (unsigned char)BitStream;

				    BitStream >>=  8;

				    *pCmprsBfr     = (unsigned char)BitStream;

				    /* High byte first */

				#else

					*(unsigned long *) pCmprsBfr = BitStream;

				#endif



				pCmprsBfr += 4;



Recover_From_Cmprs_Bfr_Split_Unique_Byte:

				avail_bits = 32 - avail_bits;

				BitStream = token << avail_bits;



				    /* 80x86 does not handle 32 bit shifts */

				if (avail_bits == 32)

				    /* Not needed if processor supports    */

				    /* 32 bit shift by zeroing result, Use:*/



					BitStream = 0;

			}

			pStartUb++;

		}	/* End of have an odd byte to out */

			/* Out all has 2 token pairs      */



		for (nUnqBytes >>= 1; nUnqBytes > 0; nUnqBytes--) {

			if (avail_bits > 2*RAW_TKN_SZ){

				avail_bits -= 2*RAW_TKN_SZ;

				BitStream |= ( ( ((unsigned long) *pStartUb) <<

					RAW_TKN_SZ)

					 | *(pStartUb+1) ) << avail_bits;

			}

			else {

				token = (((unsigned long) *pStartUb) <<

					RAW_TKN_SZ)

					 | *(pStartUb+1);

				avail_bits  = 2*RAW_TKN_SZ - avail_bits;

				BitStream  |= token >> avail_bits;



				/* Put BitStream in compress buffe */



				#if defined (_EARLY_OUT_)

				    /* Can avoid this test; 		  */

				    /* If compress data buffer big enough */



				if (FP_Off(pCmprsBfr) > FP_Off(pCmprsEnd)) {

					stopped4 = SPAN_CMPRS_BFR;

					goto Save_Cmprs_State;

				}

				#endif



   					#if (defined (LZS_ALIGNED) || \
						(LZS_BYTE_ORDER == LZS_LITTLE_END))

				    /* Map regs to memory */



				    *(pCmprsBfr+3) = (unsigned char) BitStream;

				    /* Low byte last */

				    BitStream >>=  8;

				    *(pCmprsBfr+2) = (unsigned char) BitStream;

				    BitStream >>=  8;

				    *(pCmprsBfr+1) = (unsigned char) BitStream;

				    BitStream >>=  8;

				    *pCmprsBfr     = (unsigned char) BitStream;

				    /* High byte first */

				#else

				    *(unsigned long *) pCmprsBfr = BitStream;

				#endif



				pCmprsBfr += 4;



Recover_From_Cmprs_Bfr_Split:

				avail_bits = 32 - avail_bits;

				BitStream = token << avail_bits;



				if (avail_bits == 32)

					BitStream = 0;

					/* Not needed id processor supports */

					/* 32 bit shift by zeroing result   */

			}

			pStartUb += 2;



		}       /* End of for loop */

	}	/* End of have out unique bytes to out */



	/* Encode string 					*/

	/* In:							*/

	/*	wBestLen = longest match found			*/

	/*      wBestOff = Delta to Match from parsing point	*/



	pStartUb = (pParsePt += wBestLen);



	if (wBestOff & 0xFF80U) {

		token = wBestOff | 0x1000;	/* Construct string token; */

		wToken_size = 11+2;       	/* Offset 11 		   */

	}

	else {

		token = wBestOff | 0x0180;	/* Construct string token; */

		wToken_size = 7+2;		/* Offset 7		   */

	}



	if (wBestLen < 5) {

		token = (token << 2) | (wBestLen-2);	/* len = 2, 3, 4   */

		wToken_size += 2;



		/* Put string token into BitStream */

OutToken:

		if (avail_bits > wToken_size) {

			avail_bits -= wToken_size;

			BitStream |= token << avail_bits;

		}

		else {	/* Split string token */

			avail_bits  = wToken_size - avail_bits;

			BitStream  |= token >> avail_bits;



			/* Put BitStream in compress buffer */



			#if defined (_EARLY_OUT_)

			    /* Can avoid this test; 		  */

			    /* If compress data buffer big enough */



			    if ( FP_Off(pCmprsBfr) > FP_Off(pCmprsEnd) ) {

				stopped4 =  SPLITTING_STRING;

				goto Save_Cmprs_State;

			}

			#endif



    		#if (defined (LZS_ALIGNED) || (LZS_BYTE_ORDER == LZS_LITTLE_END))

			    /* Map regs to memory */

			    *(pCmprsBfr+3) = (unsigned char) BitStream;

			    /* Low byte last */

			    BitStream >>=  8;

			    *(pCmprsBfr+2) = (unsigned char) BitStream;

			    BitStream >>=  8;

			    *(pCmprsBfr+1) = (unsigned char) BitStream;

			    BitStream >>=  8;

			    *pCmprsBfr     = (unsigned char) BitStream;

			    /* High byte first */

			#else

				*(unsigned long *) pCmprsBfr = BitStream;

			#endif



			pCmprsBfr += 4;



Recover_From_Cmprs_Bfr_Split_String:

			avail_bits = 32 - avail_bits;

			BitStream = token << avail_bits;



			if (avail_bits == 32)

				BitStream = 0;

				/* Not needed if processor supports */

				/* 32 bit shift by zeroing defalt, Use: */



		}

		if (wBestLen > 14) {	/* Funish building token */

			wToken_size = 0;

			token = 0;

			goto moreLongString;

		}



		wMatchTried = performance;	/* Limit searches */

		goto LookUpKey;



	}



	if (wBestLen < 8) {

		token = (token << 4) | (0x000C | (wBestLen-5));

			/* len = 5, 6, 7 */



		wToken_size += 4;

		goto OutToken;

	}



	wBestLen += (15-8);

	do {

		token = (token << 4) | 0x000F;

		wToken_size += 4;



		if (wToken_size > 28) {



		#if defined (_FAST_)

			goto OutToken;

		#else

			if (!(pWrkSpace->BkState & SPAN_ORIG_BFR))

				goto OutToken;



			stopped4 = SPAN_ORIG_BFR;

			goto Save_Cmprs_State;

		#endif

		}

moreLongString:

		wBestLen -= 15;

	} while (wBestLen > 14);



	token = (token << 4) | wBestLen;

	wToken_size += 4;

	goto OutToken;

}



/*--------------------- end of LZSC.C ----------------------------------*/

