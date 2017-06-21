/* $Id: lzsd.c,v 3.2.62.3 1996/05/17 12:16:35 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/wan/lzsd.c,v $
 *------------------------------------------------------------------
 * lzsd.c -- Decompression side of stac compression engine
 *
 * March 1995, Steve Turadek
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lzsd.c,v $
 * Revision 3.2.62.3  1996/05/17  12:16:35  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/05/06  00:09:14  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.2.62.2  1996/04/29  10:09:26  mdb
 * The 96q1 gcc is much more particular about flagging potentially
 * uninitialized variables [CSCdi56015].
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/29  06:28:31  turadek
 * CSCdi52904:  stac libraries need to be brought to current released level
 * Branch: California_branch
 * Latest and greatest released stac code.
 *
 * Revision 3.2  1995/11/17  18:03:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:49:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:35:55  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/01  06:23:18  turadek
 * CSCdi39487:  stac compression broken for all encaps
 * fixes two stac library bugs (one data-dependent, one for anti-
 * compression feature.)  also adds enhanced x25 compression debug support.
 *
 * Revision 2.1  1995/06/07  23:18:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*----------------------------------------------------------------------*/

/* LZS221-C Version 4, Release 2, Beta-1								*/

/*----------------------------------------------------------------------*/

/*  LZSD.C - Expand Lzs(TM) tokens - Bookkeeper Layer		   			*/

/*																		*/

/*  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  */

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

/* party without express written consent from Stac Electronics.			*/

/*																		*/

/*----------------------------------------------------------------------*/

/* All Stac product names are trademarks or registered trademarks		*/

/* of Stac Electronics.													*/

/*----------------------------------------------------------------------*/



/************************************************************************

*	$Workfile:   lzsd.c  $

*	$Revision: 3.2.62.3 $

*	$Date: 1996/05/17 12:16:35 $

*	$Author: ppearce $

*

*	$Log: lzsd.c,v $
 * Revision 3.2.62.3  1996/05/17  12:16:35  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/05/06  00:09:14  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.2.62.2  1996/04/29  10:09:26  mdb
 * The 96q1 gcc is much more particular about flagging potentially
 * uninitialized variables [CSCdi56015].
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/29  06:28:31  turadek
 * CSCdi52904:  stac libraries need to be brought to current released level
 * Branch: California_branch
 * Latest and greatest released stac code.
 *
 * Revision 3.3  1996/03/28  19:55:09  turadek
 * CSCdi52904:  stac libraries need to be brought to current released level
 * Latest  and greatest released Stac code.
 *

 * 

 *    Rev 1.13   20 Oct 1995 15:03:10   TIMB

 * Added fixes for LZS_UNCOMPRESSED mode with srccount != dstcount.

 * Due to lack of testing, now returns LZS_INVALID if srccount != dstcount

 * in LZS_UNCOMPRESSED mode.

 * 

 *    Rev 1.12   11 Oct 1995 12:00:32   TIMB

 * Fixed decompress with simultaneous source, dest exhaust and eocd.

 * 

 *    Rev 1.11   31 Aug 1995 09:52:42   TIMB

 * Added logic to save history on 0 byte destination count.

* 

*    Rev 1.10   29 Aug 1995 16:54:00   TIMB

* Added flags to denote saved count, ptr validity rather than count=0

* Made SAVED_CMPRS_BYTES flag valid when calling in LZS_UNCOMPRESS mode

* Clear SAVED_CMPRS_BYTES flag when leaving SPANNING_CMPRS state

* 

*    Rev 1.8   21 Aug 1995 14:03:00   TIMB

* Preserve values of ptrs, offsets in argument list

* Added test for overrun in low source count case

* 

*    Rev 1.7   17 Aug 1995 16:20:04   TIMB

* 4.23

* Bumped Rev #

* 

*    Rev 1.6   Jul 13 1995 11:55:56   RFRIEND

* Steve Mickelsen's bugs fixed:

* 1)  0 Byte Source Flush in lzsc.c

* 2) 15:1 max compression ratio in lzsc.c

* 3) No EOC on Dest Flush in lzsc.c

* 

*    Rev 1.0   Apr 24 1995 15:07:14   BOBL

*  

* 

*************************************************************************/

#include "LZS.H"



#define MAX_BLOCK_SIZE_TYPE long

#define SIZEOF_tOrigByteIndex 4

#define MAX_BUFFR_SIZE_TYPE long



#if defined (LZS_FAST)

	#define _FAST_

	#define _Small_Blocks_

	#define MAX_ORIG_INDEX OrigCnt

	#undef LZS_SMALL_BUFFERS

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

	#define MAX_BUFFER_SIZE 		0xFFFFU

#endif



typedef unsigned MAX_BUFFR_SIZE_TYPE tMaxBfrSz;

typedef unsigned MAX_BLOCK_SIZE_TYPE tOrigByteIndex;



#define FP_Off(fp) fp



#define dREG

#define pREG register





#define	MEM_COPY_S(pDest, pSrc, nbytes) (mem_copy_r (pDest, pSrc, nbytes))

#define	COPY_PREV_STRING(pDest, wOffset, nbytes) \
		copy_prev_string_r (pDest, wOffset, nbytes)



#define COPY_TO_STRING_END COPY_PREV_STRING



#define RAW_TKN_SZ 				9

#define EOCD_TKN_SZ 			(2+7)

#define KEY_LENGTH 				2

#define MAX_OFFSET		 	 	0x7FFU



#define SPAN_CMPRS_BFR 			0x80

#define SPLITTING_STRING		0x40

#define SPLITTING_UNIQUE_BYTE	0x20

#define SPLITTING_ENDS			0x10

#define SPAN_ORIG_BFR			0x08

#define ORIG_BLK_START			0x04

#define ENGN_SOURCE_EXHAUSTED 	LZS_SOURCE_EXHAUSTED



#define INIT_HASH				0x02

#define NOT_ORIG_BFR			0x04



#define MAX_LOOK_AHEAD 			4U

#define DUM_DUM_SZ	 			(2U*(MAX_OFFSET)+MAX_LOOK_AHEAD)



#if !defined (LZS_FAR)

	#define LZS_FAR

#endif



#define MAX_OVERRUN 			16U



#define DUMDUM_SZ 				(2*(MAX_OFFSET+1) + MAX_OVERRUN)



struct RestartData {

	tMaxBfrSz      len;

	unsigned short bits2discard;

	unsigned short offset;

	unsigned short Sign;

	unsigned char  engnState;

};



struct SaveData {

	unsigned char LZS_FAR *pTinyBfrBrk;

	unsigned char LZS_FAR *pExpBfrBase;

	unsigned short	       CmpByteSaved;

	unsigned short 	       ExpByteSaved;

	struct RestartData     restart;

	unsigned char	       fState;



#if !defined(_SAVE_DECOMPRESS_POINTERS_)

	unsigned long		   svd_sourceCnt;

	unsigned char LZS_FAR *svd_source;

	unsigned long		   svd_destCnt;

	unsigned char LZS_FAR *svd_dest;

	unsigned char          svd_flags;

#endif



#if !defined (_FAST_)

	unsigned char SaveCmpBytes[7];

	unsigned char SaveExpBytes[DUMDUM_SZ];

#endif

};



#define	LZS_STOPPED_AT_TOKEN	0x01

#define	LZS_STOPPED_GETTING_LEN	0x02

#define	SPANNING_CMPRS_BFRS 	0x04

#define	LZS_OVERRUNNN			0x08

#define	TINY_EXPAND_BFR			0x10

#define	LZS_EXPANDING_STR 		0x20

#define	SAVED_CMPRS_BYTES		0x40

#define	LZS_END_OF_CMPRS_DATA	0x80



#define ENGN_STOPPED_AT_TOKEN	LZS_STOPPED_AT_TOKEN

#define	ENGN_STOPPED_GETTING_LEN LZS_STOPPED_GETTING_LEN

#define	ENGN_PARTIAL_TOKEN      0x04

#define ENGN_OVERRUNNN 			LZS_OVERRUNNN

#define	ENGN_EXPANDING_STR 		LZS_EXPANDING_STR

#define	ENGN_NEW_BFR            0x40

#define	ENGN_END_OF_CMPRS_DATA	LZS_END_OF_CMPRS_DATA



#define SVD_SOURCE_VALID		0x01

#define SVD_DEST_VALID			0x02

#define SVD_INIT				0



unsigned int LZS_FAR Expand_Lzs_Engine(

	unsigned char LZS_FAR * LZS_FAR *ppCmprsBfr,

	unsigned char LZS_FAR		*pCmprsBfrEnd,

	unsigned char LZS_FAR * LZS_FAR *ppExpBfr,

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



#define tExpndWrkSpcPtr struct SaveData



#ifndef	_Lzs_Mem_R_H

	#define	_Lzs_Mem_R_H

	unsigned char LZS_FAR * LZS_FAR mem_copy_r (

		void LZS_FAR 	*pDestn,

		void LZS_FAR 	*pSource,

		unsigned short   wNbytes

	);



	unsigned char LZS_FAR * LZS_FAR copy_prev_string_r (

		unsigned char LZS_FAR *pDestn,

		unsigned short 	       wOffset,

		tMaxBfrSz    	       wNbytes

	);

#endif



static unsigned int EocdToken(

	unsigned char LZS_FAR	  **ppCmprsBfr,

	unsigned char LZS_FAR	   *pCmprsBfrEnd,

	struct RestartData LZS_FAR *pRestartData

);



unsigned short LZS_FAR LZS_Decompress(

	unsigned char LZS_FAR * LZS_FAR *source,

	unsigned char LZS_FAR * LZS_FAR *dest,

	unsigned long LZS_FAR           *sourceCnt,

	unsigned long LZS_FAR           *destCnt,

	void          LZS_FAR           *history,

	unsigned short                   flags

);



#define EXP_SIGNATURE 0x1234



/*----------------------------------------------------------------------*/

/* Init_Expand_Data --  Initialize Expansion variables					*/

/*----------------------------------------------------------------------*/



static void Init_Expand_Data(

	struct RestartData LZS_FAR *pRestartData

)

{

	pRestartData->bits2discard 	= 0;

	pRestartData->Sign 	   		= EXP_SIGNATURE;

	pRestartData->engnState    	= ENGN_NEW_BFR;

	return;

}



/*----------------------------------------------------------------------*/

/* LZS_InitDecompressionHistory -- Initialize Expansion Save data area  */

/*----------------------------------------------------------------------*/



unsigned short LZS_FAR LZS_InitDecompressionHistory(

	void LZS_FAR *history	/* Pointer to decompression history */

)

{

	struct SaveData LZS_FAR *p_temp ;



	p_temp = history ;



	Init_Expand_Data( (struct RestartData LZS_FAR *)

		(&(*(struct SaveData LZS_FAR *)p_temp).restart) );



#if !defined (_FAST_)

	p_temp->pTinyBfrBrk = &p_temp->SaveExpBytes[0];

#endif



	p_temp->ExpByteSaved = (p_temp->CmpByteSaved = 0);

	p_temp->fState = 0;



#if !defined(_SAVE_DECOMPRESS_POINTERS_)

	p_temp->svd_flags = SVD_INIT;

#endif



	return 1;

}



/*----------------------------------------------------------------------*/

/* LZS_SizeOf_DecompressHistory -- Returns size of the data save buffer	*/

/*	needed by LZS_Decompress to survive buffer spanning. 				*/

/*----------------------------------------------------------------------*/

/* In:                                                                  */

/*                                                                      */

/* Returns:                                                             */

/*	(unsigned short) Number of bytes needed for data save buffer    	*/

/*                                                                      */

/*----------------------------------------------------------------------*/



unsigned short LZS_FAR LZS_SizeOfDecompressionHistory (void)

{

	return (sizeof (struct SaveData));

}



/*----------------------------------------------------------------------*/

/*		      Begining of the Expansion engine							*/

/*----------------------------------------------------------------------*/



/*----------------------------------------------------------------------*/

/* Expand_Lzs_engine -- Expands from a (compressed) buffer of LZS		*/

/*						 tokens to data									*/

/*----------------------------------------------------------------------*/

/*	Uses DoublewordRegister decoder										*/

/* In:																	*/

/*	ppCmprsBuffer 	(referenced) pointer to (compressed) buffer			*/

/*	pCmprsBfrEnd    Compress buffer end                             	*/

/*	ppExpBfr	 	(referenced) pointer to Expansion buffer			*/

/*	pExpBfrEnd		Expansion buffer end								*/

/*	pRestartData	pointer to RestartData                          	*/

/*																		*/

/* Returns:																*/

/*	ENGN_STOPPED_AT_TOKEN		Hit end of compressed buffer			*/

/*									during token fetch					*/

/*	ENGN_OVERRUNNN				Too close to expand buffer end.			*/

/*	ENGN_STOPPED_GETTING_LEN  	Decoding length field when				*/

/*									compress buffer emptied				*/

/*	ENGN_EXPANDING_STRING   	Expanded string filled expand			*/

/*									buffer - need more room				*/

/*	ENGN_END_OF_COMPRESS_DATA	Found End of Compressed Data			*/

/*									Token								*/

/* Updates:																*/

/*	pExpandBuffer   Next expansion byte.								*/

/*	pCmprsBuffer    Next compressed byte								*/

/*																		*/

/*	*** = Not used if _FAST_											*/

/* Terminates when either source or destination end is encountered.		*/

/*																		*/

/* Compressed data tokens (QIC-122 standard):							*/

/* 	0<b8><b7><b6><b5><b4><b3><b2><b1>									*/

/*		- 8-bit byte (not compressed)									*/

/*     String reference: offset to prior occurrence; length of string	*/

/*	11<o7><o6><o5><o4><o3><o2><o1>{len}	 		  						*/

/*		- 7 bit offset													*/

/*	10<o11><o10><o9><o8><o7><o6><o5><o4><o3><o2><o1>{len}				*/

/*		- 11 bit offset													*/

/*	{len}:																*/

/*		00 = 2 bytes; 01 = 3 bytes; 10 =  4 bytes						*/

/*		11 00 = 5; 11 01 = 6;    11 10 =  7								*/

/*		11 11 0000 = 8; ... 11 11 1110 = 22								*/

/*		11 11 1111 0000 = 23; 11 11 1111 1110 = 37; etc.				*/

/*----------------------------------------------------------------------*/

/* Fields in (right - justified) BitStream								*/

/*----------------------------------------------------------------------*/



#define S_FIELD 0x80000000UL  /* String or (uncompressed) byte follows  */

#define T_FIELD	0x40000000L   /* Offset 7 or 11 follows					*/

#define EOCD_MARK 0x180L



unsigned int LZS_FAR Expand_Lzs_Engine(

	unsigned char LZS_FAR * LZS_FAR *ppCmprsBfr,

	unsigned char LZS_FAR	   	*pCmprsBfrEnd,

	unsigned char LZS_FAR * LZS_FAR *ppExpBfr,

	unsigned char LZS_FAR	   	*pExpBfrEnd,

	struct RestartData LZS_FAR 	*pRestartData

)

{

	dREG unsigned long BitStream;	/* Contains the	current valid		*/

									/* Token bits 						*/



	pREG unsigned char LZS_FAR 	*pCmprsBfr;

	pREG unsigned char LZS_FAR 	*pExpBfr;

	pREG unsigned char LZS_FAR 	*pExpBfrOverRun;

	pREG unsigned char LZS_FAR 	*pSi;



	dREG tMaxBfrSz len;				/* Length of string 				*/

	     tMaxBfrSz LengthReg;		/* Misc data register - 			*/

									/* Holds lengths 					*/

	dREG unsigned short bits2discard;	/* Bits used in current 		*/

									/* Compress word 					*/

	dREG unsigned short offset;		/* Offset to prior					*/

									/* String bytes 					*/



#if !defined (_FAST_)

	unsigned int    ndx;			/* Temp var - indexes into buffer 	*/

#endif



	/* Setup local pointers and vars */



	pCmprsBfr = pCmprsBfrEnd;		/* Avoid a compiler warning 		*/

	pCmprsBfr = *ppCmprsBfr;

	pExpBfr   = *ppExpBfr;

	pExpBfrOverRun = (pExpBfrEnd - pExpBfr < MAX_OVERRUN) ?

				       pExpBfr:

				       pExpBfrEnd - (MAX_OVERRUN-1);



	bits2discard = pRestartData->bits2discard;



#if defined (_FAST_)



	/* Avoid these expensive init and tests */

	/* Starting a new compressed data block */



	bits2discard = 0;

#else

	offset = pRestartData->offset;

	len = pRestartData->len;

	pCmprsBfr += (bits2discard >> 3);

	bits2discard &= 7;



	LengthReg = (tMaxBfrSz)(FP_Off(pCmprsBfrEnd) - FP_Off(pCmprsBfr));

	/* Number of compressed bytes */



	if (LengthReg < 4) {	/* This handles a case could be outlawed	*/

		BitStream = 0;

		bits2discard += 4*8;

		pCmprsBfr -= 4;



		for (ndx = 0 ; (ndx < LengthReg); ndx++) {

			bits2discard -= 8;

			pCmprsBfrEnd --;

			pCmprsBfr ++;

			BitStream |=    ((unsigned long)*pCmprsBfrEnd)

				     << (ndx<<3);

			/* Min Bits to discard = 8 */

		}



		BitStream <<= bits2discard;

		if (bits2discard > 32-9)

			goto No_Mas;



		if (pRestartData->engnState & (  ENGN_NEW_BFR

					       | ENGN_STOPPED_AT_TOKEN

					       | ENGN_OVERRUNNN) ) {



			if (pExpBfr >= pExpBfrOverRun)

				goto In_OverRun_Zone;

		

			if ( !(BitStream & S_FIELD) ) {

				/* 8-bit byte follows -min valid bits 9 */



				*pExpBfr++ = (unsigned char)

					(BitStream >> (16-RAW_TKN_SZ+16));

					/* Out char in BitStream */



				bits2discard += RAW_TKN_SZ;

				goto Fill_Bit_Stream;

				/* This will exit since already at end */

			}



			if (bits2discard < (32-(2+11+2)))

				goto Have_A_String;

				/* Can process shot string */



			if (BitStream & T_FIELD) {	/* Offset size? */

				if (bits2discard < (32-(2+7+2)) )

					goto Stng_Off7;

					/* Can process short, near string */

					/* Can process End Of Compressed  */

					/* Data Token 			  */



				if (0 == (( (unsigned short)

					(BitStream >> (23))) & 0x007F) ) {

					bits2discard += EOCD_TKN_SZ;

					goto Done;

				}

			}

			goto No_Mas;

		}

	if (pRestartData->engnState & ENGN_EXPANDING_STR)

		goto Room_4_String;



	if (len < 8)

		goto Get_Len_Single_byte_restart;



	goto Get_Len_Too_Single_byte_restart;



	}

	pCmprsBfrEnd -= 3;	/* Expander uses a 4-byte fetch */



	if (pRestartData->engnState & (  ENGN_NEW_BFR

				       | ENGN_STOPPED_AT_TOKEN

				       | ENGN_OVERRUNNN) )

		goto Fill_Bit_Stream;



	if (pRestartData->engnState & ENGN_EXPANDING_STR)

		goto Room_4_String;



	if (len < 8)

		goto Get_Len;



	goto Get_Len_Too;

#endif



Fill_Bit_Stream:

	/* Fetch 4 bytes from compress buffer and	*/

	/* Discard leading bits (until near the end).	*/



	pCmprsBfr += (bits2discard >> 3);

	bits2discard &= 7;



#if !defined (_FAST_)

	if (pCmprsBfr >= pCmprsBfrEnd)

		goto Cmprs_End;

#endif



#if (defined (LZS_ALIGNED) || (LZS_BYTE_ORDER == LZS_LITTLE_END))

	BitStream =( (((unsigned long) *(pCmprsBfr  )) << 3*8) |

		     (((unsigned long) *(pCmprsBfr+1)) << 2*8) |

		     (((unsigned long) *(pCmprsBfr+2)) << 1*8) |

		       (unsigned long) *(pCmprsBfr+3)) << bits2discard;

#else

	BitStream = *(unsigned long *)pCmprsBfr << bits2discard;

#endif



	if (pExpBfr >= pExpBfrOverRun)

		goto In_OverRun_Zone;



	if ( !(BitStream & S_FIELD) ) {

		*pExpBfr++ = (unsigned char)(BitStream >> (32-RAW_TKN_SZ));

		bits2discard += RAW_TKN_SZ;



		if ( !(BitStream & (S_FIELD >> RAW_TKN_SZ) ) ) {

			*pExpBfr++ = (unsigned char)

				(BitStream >> (32-2*RAW_TKN_SZ));



			bits2discard += RAW_TKN_SZ;



			if (bits2discard > (32-RAW_TKN_SZ))

				goto Fill_Bit_Stream;



			if (!(BitStream & (S_FIELD >> 2*RAW_TKN_SZ) ) ) {

				*pExpBfr++ = (unsigned char)

					(BitStream >> (32-3*RAW_TKN_SZ));

				bits2discard += RAW_TKN_SZ;

				goto Fill_Bit_Stream;

			}



			if (!(BitStream & (T_FIELD>>(2*RAW_TKN_SZ))))

				goto Fill_Bit_Stream;



			if (bits2discard > (32-(2+7+2)))

				goto Fill_Bit_Stream;



			BitStream = BitStream << 2*RAW_TKN_SZ;

			goto Stng_Off7;

		}

		BitStream = BitStream << RAW_TKN_SZ;

	}



Have_A_String:

	if (BitStream & T_FIELD) {



Stng_Off7:

		bits2discard += (2+7);

		offset = (((unsigned short)(BitStream >> 23)) & 0x007FU);

		if (offset == 0)

			goto Done;



		BitStream   <<= (2+7) ;

	}

	else {

		offset = ((unsigned short)(BitStream >> (19)) & 0x07FF);

		BitStream   <<= (2+11) ;

		bits2discard += (2+11) ;

	}



	if (BitStream < 0xC0000000UL) {

		pSi = pExpBfr - offset ;

		*pExpBfr = *pSi;

		*(pExpBfr+1) = *(pSi+1);

		pExpBfr += 2;



		if (BitStream & 0x40000000UL) {

			*pExpBfr++ = *(pSi+2);

		}



		else if (BitStream & 0x80000000UL) {

			*pExpBfr = *(pSi+2);

			*(pExpBfr+1) = *(pSi+3);

			pExpBfr += 2;

		}



		bits2discard += 2;

		if ( (BitStream & (S_FIELD>>2) ) &&

			 (bits2discard <= (32-(2+7+2)) ) ) {

			BitStream <<= 2;



			if (BitStream & T_FIELD)

				goto Stng_Off7;



			if (bits2discard <= (32-(2+11+2)) )

				goto Have_A_String;

		}

		goto Fill_Bit_Stream;

	}



#if !defined (_FAST_)

	Get_Len_Single_byte_restart:

#endif



	if (bits2discard > 28) {

		len = 0;

		pCmprsBfr += (bits2discard >> 3);

		bits2discard &= 7;



#if !defined (_FAST_)

		if (pCmprsBfr >= pCmprsBfrEnd)

			goto Cmprs_End_Getting_Len;

Get_Len:

#endif



#if (defined (LZS_ALIGNED) || (LZS_BYTE_ORDER == LZS_LITTLE_END))

		BitStream =(

			(((unsigned long) *(pCmprsBfr  )) << 3*8) |

			(((unsigned long) *(pCmprsBfr+1)) << 2*8) |

			(((unsigned long) *(pCmprsBfr+2)) << 1*8) |

			  (unsigned long) *(pCmprsBfr+3)) << bits2discard;

#else

		BitStream = *(unsigned long *)pCmprsBfr << bits2discard;

#endif

		}

		if (BitStream < 0xF0000000UL)  {

			len = 5 + (unsigned short)((BitStream >> (28)) & 3);

			goto Copy_String;

		}

		len = (tMaxBfrSz) - 7;

		do {

			len += 15;

			BitStream <<= 4;

			bits2discard += 4;



#if !defined (_FAST_)

Get_Len_Too_Single_byte_restart:

#endif

			if (bits2discard > 28) {

				pCmprsBfr += (bits2discard >> 3);

				bits2discard &= 7;

#if !defined (_FAST_)

				if (pCmprsBfr >= pCmprsBfrEnd)

					goto Cmprs_End_Getting_Len_Too;

Get_Len_Too:

#endif



#if (defined (LZS_ALIGNED) || (LZS_BYTE_ORDER == LZS_LITTLE_END))

				BitStream =(

				    (((unsigned long) *(pCmprsBfr  )) << 3*8) |

				    (((unsigned long) *(pCmprsBfr+1)) << 2*8) |

				    (((unsigned long) *(pCmprsBfr+2)) << 1*8) |

				      (unsigned long) *(pCmprsBfr+3)) <<

					bits2discard;

#else

				BitStream = *(unsigned long *)pCmprsBfr <<

					bits2discard;

#endif

			}

		} while (BitStream >= 0xF0000000UL);

	len += (unsigned short)(BitStream >> (4 + 8 + 16));



Copy_String:

	BitStream <<= 4;

	bits2discard += 4;



#if !defined (_FAST_)

    Room_4_String:

#endif

	if (len <= (tMaxBfrSz)(FP_Off(pExpBfrEnd) - FP_Off(pExpBfr))) {

		/* copy n bytes (word copy)(repeated byte if offset = 1) */

		pExpBfr = COPY_TO_STRING_END(pExpBfr, offset, len);

		goto Fill_Bit_Stream;   /* may be out of compress tokens */

	}



	/* Users Expand buffer too small for string, copy what fits */



	LengthReg = (tMaxBfrSz)(pExpBfrEnd - pExpBfr);

	pExpBfr = COPY_TO_STRING_END(pExpBfr, offset, LengthReg);

	len -= LengthReg;

	pRestartData->engnState = ENGN_EXPANDING_STR;

	goto UpDateVIPcount;



#if !defined (_FAST_)

No_Mas:

	pRestartData->engnState |= ENGN_PARTIAL_TOKEN;

	goto UpDateVIPcount;



Cmprs_End_Getting_Len_Too:

Cmprs_End_Getting_Len:

	pRestartData->engnState = ENGN_STOPPED_GETTING_LEN;

	goto Report_Results;



Cmprs_End:

	pRestartData->engnState = ENGN_STOPPED_AT_TOKEN;

	goto Report_Results;

#endif	/* End of not FAST expansion */





In_OverRun_Zone:

	/* If the next token is EOCD, process it.						*/

	if ((32-bits2discard >= EOCD_TKN_SZ) && 

		((BitStream & 0xff800000) == (EOCD_MARK << (32-9))))

		{

		bits2discard += EOCD_TKN_SZ;

		}

	else

		{

		pRestartData->engnState = ENGN_OVERRUNNN;

		goto Report_Results;

		}



Done:	/* The exact position of the EOCD token is not needed; 	*/

	/* but a very weak  error test is possible		*/



	pRestartData->engnState = ENGN_END_OF_CMPRS_DATA;

	bits2discard += (8 - (bits2discard & 7)) & 7;



UpDateVIPcount:

	pCmprsBfr += (bits2discard >> 3);

	bits2discard &= 7;	/* Return the end of the token */



Report_Results:



#if !defined (_FAST_)

	pRestartData->bits2discard = bits2discard;

	pRestartData->offset 	   = offset;

	pRestartData->len 	   = len;

#endif



	*ppCmprsBfr = pCmprsBfr;

	*ppExpBfr   = pExpBfr;

	return (pRestartData->engnState);

}







static unsigned int EocdToken(

	unsigned char LZS_FAR	  **ppCmprsBfr,

	unsigned char LZS_FAR	   *pCmprsBfrEnd,

	struct RestartData LZS_FAR *pRestartData

)

{

	unsigned char LZS_FAR	   *p_Peaker;



	p_Peaker = *ppCmprsBfr;



	if (pCmprsBfrEnd < (p_Peaker + 2U))

		return(ENGN_PARTIAL_TOKEN);



	if (0xC000U == (((   (p_Peaker[0] << 8) | p_Peaker[1])

			  << pRestartData->bits2discard) & 0xFF80U ) ) {



		pRestartData->bits2discard += EOCD_TKN_SZ;

		*ppCmprsBfr += pRestartData->bits2discard >> 3;

		pRestartData->bits2discard &= 7;



		if (pRestartData->bits2discard)

			(*ppCmprsBfr)++;



		return(ENGN_END_OF_CMPRS_DATA);

	}

	return(0);

}





/*----------------------------------------------------------------------*/

/*			End of Expansion engine 	   								*/

/*----------------------------------------------------------------------*/



/*----------------------------------------------------------------------*/

/* LZS_Decompress -- Expand from (compressed) buf of LZS token to data  */

/*----------------------------------------------------------------------*/

/* In:																	*/

/*      source		ptr to pointer to (compressed) buffer				*/

/*      dest		ptr to pointer to Expansion buffer					*/

/*      sourceCnt   ptr to size (in bytes) of compress buffer.			*/

/*      destCnt  	ptr to size (in bytes) of the Expansion buffer.		*/

/*		history		ptr to (call to call) memory block					*/

/*		flags		Options (bit encoded):								*/

/*      	         LZS_UNCOMPRESSED	0x02							*/

/*          	     LZS_SAVE_HISTORY   0x04							*/

/*																		*/

/* Returns:																*/

/*	LZS_INVALID        		0						       				*/

/*	LZS_SOURCE_EXHAUSTED    1											*/

/*	LZS_DEST_EXHAUSTED      2											*/

/*	LZS_END_MARKER          4   Found End of Compressed Data Token		*/

/*																		*/

/* Updates:																*/

/*	MaxCount		remaining bytes in the Expansion buffer.			*/

/*					(Original size- # expanded bytes)					*/

/*	pExpandBuffer   Next expansion byte.								*/

/*	CmprsSize       remaining bytes in the compression buffer.			*/

/*					(Original size- # compressed bytes processed)		*/

/*	pCmprsBuffer    Next compressed byte								*/

/*																		*/

/* Terminates if EOCD found or source or destination counts exhausted.	*/

/*																		*/

/* Error Handling:														*/

/*----------------------------------------------------------------------*/

/* Compressed data tokens (QIC-122 standard):							*/

/*	0<b8><b7><b6><b5><b4><b3><b2><b1>									*/

/*		- 8-bit byte (not compressed)									*/

/*	String reference: offset to prior occurrence; length of string		*/

/*	11<o7><o6><o5><o4><o3><o2><o1>{len}                             	*/

/*		- 7 bit offset													*/

/*	10<o11><o10><o9><o8><o7><o6><o5><o4><o3><o2><o1>{len}				*/

/*		- 11 bit offset													*/

/*	{len}:																*/

/* 		00 = 2 bytes; 01 = 3 bytes; 10 =  4 bytes						*/

/*		11 00 = 5; 11 01 = 6;    11 10 =  7								*/

/*		11 11 0000 = 8; ... 11 11 1110 = 22								*/

/*		11 11 1111 0000 = 23; 11 11 1111 1110 = 37; etc.				*/

/*----------------------------------------------------------------------*/



unsigned short LZS_FAR	LZS_Decompress(

	unsigned char LZS_FAR * LZS_FAR *source, /* Ptr to source buffer  	*/

	unsigned char LZS_FAR * LZS_FAR *dest,   /* Ptr to dest buffer    	*/

	unsigned long LZS_FAR	*sourceCnt,		 /* Source count 	  		*/

	unsigned long LZS_FAR	*destCnt,		 /* Dest buffer size      	*/

	void          LZS_FAR	*history,	 	 /* Decompression history 	*/

	unsigned short 		 	wOption			 /* Special flags         	*/

)

{

	unsigned char LZS_FAR 	*pExpandBuffer;		/* The destination (?) */

	

	unsigned char LZS_FAR 	*pExpBfr;			/* Current decompression dest. */

												/* may point into either the */

												/* work or dest buffer. */

	unsigned char LZS_FAR 	*pExpBfrEnd;		/* Current decompression limit */

												/* Decomp engine stops when */

												/* pExpBfr reaches this ptr */



	unsigned char LZS_FAR 	*pCmprsBfr;			/* Compressed data */

	unsigned char LZS_FAR 	*pCmprsBfrEnd;		/* End of compressed data */



#if !defined (_FAST_)

	unsigned char LZS_FAR 	*pExpBfrStart = 0;	/* Start of new data in the */

												/* work buffer to be copied to */

												/* the dest later. */



	unsigned char LZS_FAR 	*pExpStopped  = 0;	/* Destination to which new */

												/* data from the work buffer */

												/* should be copied. */

	unsigned char LZS_FAR 	*pTemp;

#endif







	tMaxBfrSz	bytes_left;	/* Remaining room in a	*/

					/* Tiny expand buffer 	*/



#if !defined (_FAST_)

	tMaxBfrSz 	bytes_saved;	/* Expanded bytes in work buffer */

	tMaxBfrSz       LengthReg;	/* Misc data registers - 	 */

					/* holds lengths 		 */

	unsigned short	wDataReg;	/* Misc data register 		 */

#endif



	unsigned short	ret_code = 0;

	unsigned int	engn_return_code = 0;



#if !defined(_SAVE_DECOMPRESS_POINTERS_)

	unsigned char LZS_FAR *	call_source		= *source;

	unsigned char LZS_FAR *	call_dest		= *dest;

	unsigned long			call_sourceCnt	= *sourceCnt;

	unsigned long			call_destCnt	= *destCnt;



	long delta_dest;

	long delta_source;

#endif



	tExpndWrkSpcPtr LZS_FAR	*p_temp;/* Temp data pointer */



	p_temp = history ;



#if !defined(_SAVE_DECOMPRESS_POINTERS_)

	if(p_temp->svd_flags & SVD_SOURCE_VALID)	/* source buffer left over	*/

	{

		*sourceCnt = p_temp->svd_sourceCnt;

		*source	   = p_temp->svd_source;

	}

	if(p_temp->svd_flags & SVD_DEST_VALID)		/* dest buffer left over	*/

	{

		*destCnt = p_temp->svd_destCnt;

		*dest    = p_temp->svd_dest;

	}

#endif



	pCmprsBfr = *source;

	pCmprsBfrEnd = pCmprsBfr + ((tMaxBfrSz) *sourceCnt);



#if defined (LZS_SMALL_BUFFERS)

/*----------------------------------------------------------------------*/

/*  Buffers are < 64k; Therefore Max counts are limited                 */

/*----------------------------------------------------------------------*/

	if ((*sourceCnt > MAX_BUFFER_SIZE) || (*destCnt > MAX_BUFFER_SIZE))

		   return (LZS_INVALID) ;

#endif



	if (wOption & (~(  LZS_SAVE_HISTORY

			 | LZS_UNCOMPRESSED  ) ))

		return (LZS_INVALID);



	bytes_left = (tMaxBfrSz) *destCnt;



	if (p_temp->restart.Sign != EXP_SIGNATURE)

		return (LZS_INVALID);



	pExpBfr = pExpandBuffer = *dest;

	/* Local copy of pointer, lets compiler know type */



	pExpBfrEnd = pExpBfr + bytes_left;



	if (p_temp->fState == 0) {

		if (*sourceCnt == 0)

			return (LZS_SOURCE_EXHAUSTED);



		p_temp->pExpBfrBase = pExpandBuffer;

		goto Resume_Parsing;

	}



#if !defined (_FAST_)

	if (   (wOption & LZS_UNCOMPRESSED)

	    && (p_temp->fState & ~(TINY_EXPAND_BFR | SAVED_CMPRS_BYTES)))

		return (LZS_INVALID);



	if (wOption & LZS_UNCOMPRESSED) {

		

		unsigned long len = 0;

		unsigned short cur_len;

		

#if !defined _ACCEPT_UNEQUAL_LZS_UNCOMPRESSED_

		if (*sourceCnt != *destCnt)

			return (LZS_INVALID);

#endif

		

	/* Copy the lesser of source and dest bytes remaining */



		if (*destCnt <= *sourceCnt) {

			len = *destCnt;

			ret_code |= LZS_DEST_EXHAUSTED;

		}

		if (*destCnt >= *sourceCnt) {

			len = *sourceCnt;

			ret_code |= LZS_SOURCE_EXHAUSTED;

		}

	

	/* Perform copy from source to dest */



		for (LengthReg = len;LengthReg != 0;

				LengthReg -= (unsigned long) cur_len) {

			cur_len = 0xffff;

			if (len < cur_len)

				cur_len = (unsigned short) len;

			

			mem_copy_r(pExpBfr,pCmprsBfr,cur_len);

			pCmprsBfr += cur_len;

			pExpBfr += cur_len;

		}

	

	/* Update history */



		if (ret_code & LZS_SOURCE_EXHAUSTED) {

			bytes_saved = (tMaxBfrSz)(p_temp->pTinyBfrBrk

				- &p_temp->SaveExpBytes[0]);



			LengthReg = len;

			if ((LengthReg + bytes_saved) > MAX_OFFSET) {

				if (LengthReg > MAX_OFFSET) {

					LengthReg = MAX_OFFSET;

					bytes_saved = 0;

				}

				else {

					wDataReg = (unsigned short)bytes_saved;

					bytes_saved = (unsigned short)(

						    MAX_OFFSET

						  - (unsigned short)LengthReg);

        	

					wDataReg -= (unsigned short)bytes_saved;

					MEM_COPY_S(&p_temp->SaveExpBytes[0],

						   &p_temp->SaveExpBytes[wDataReg],

						   (unsigned short)bytes_saved);

				}

			}

			p_temp->pTinyBfrBrk =

				MEM_COPY_S(&p_temp->SaveExpBytes[bytes_saved],

					   pExpBfr - (unsigned short)LengthReg,

					   (unsigned short)LengthReg);

        	

			p_temp->fState = TINY_EXPAND_BFR;

		}

		goto Report_Results;

	}

	

	if (p_temp->fState & TINY_EXPAND_BFR) {

		p_temp->pExpBfrBase = pExpandBuffer;

		bytes_saved = p_temp->ExpByteSaved;



		/* If expanding a string, finish copy prev string */



		if (p_temp->fState & LZS_EXPANDING_STR) {

Expand_String:

			pTemp = p_temp->pTinyBfrBrk + bytes_saved;



			/* Room = end of Save buffer - point */

			/*   where expansion stopped 	     */



			wDataReg = (unsigned short)

				(&p_temp->SaveExpBytes[DUMDUM_SZ] - pTemp);



			if (wDataReg < p_temp->restart.len) {

				/* Copy what fits */

				p_temp->restart.len -= wDataReg;

			}

			else {

				wDataReg = (unsigned short)

					p_temp->restart.len;

				p_temp->fState &= ~LZS_EXPANDING_STR;

				p_temp->restart.engnState |=

					ENGN_STOPPED_AT_TOKEN;

					/* Changed state */

			}

			if (wDataReg) {

				COPY_PREV_STRING(pTemp,

						 p_temp->restart.offset,

						 (tMaxBfrSz) wDataReg);

						 bytes_saved += wDataReg;

			}

		}	/* End of string expanding */

		if (bytes_left <= bytes_saved) {

			ret_code |= LZS_DEST_EXHAUSTED;

			p_temp->fState &= ~LZS_OVERRUNNN;

			pExpBfr = MEM_COPY_S(pExpBfr,

					     p_temp->pTinyBfrBrk,

					     (unsigned short) bytes_left);



			p_temp->pTinyBfrBrk += bytes_left;

			p_temp->ExpByteSaved = (unsigned short)

				(bytes_saved - bytes_left);



			if (0 == p_temp->ExpByteSaved) {

				if (!(LZS_EXPANDING_STR & p_temp->fState)) {

				/* If the orignal buffer and expand */

				/*   buffer are the save size       */



					pTemp = &p_temp->SaveCmpBytes[0];

					if (  (EocdToken(&pTemp,

					       &p_temp->\

						 SaveCmpBytes[p_temp->\

						 CmpByteSaved],

					       (struct RestartData LZS_FAR *)

						 &p_temp->restart ))

					    & ENGN_END_OF_CMPRS_DATA)

						goto Report_EOD;

				}

			}

			goto Report_Results;

		}

		/* Need to expand more bytes; copy   */

		/*   expanded bytes and reset window */



		pExpBfr = MEM_COPY_S(pExpBfr,

				     p_temp->pTinyBfrBrk,

				     (unsigned short) bytes_saved);

		p_temp->pExpBfrBase += bytes_saved;

		pExpStopped = pExpBfr;



		if (!(p_temp->fState & LZS_EXPANDING_STR)) {

			if (p_temp->fState & LZS_END_OF_CMPRS_DATA) {

				p_temp->pTinyBfrBrk += bytes_saved;

				p_temp->ExpByteSaved = 0;

				pExpBfr = pExpStopped;

				goto Report_EOD;

			}

		}



		wDataReg = (unsigned short) bytes_saved + (unsigned short)

			(p_temp->pTinyBfrBrk - &p_temp->SaveExpBytes[0]);



		if (wDataReg > MAX_OFFSET) {

			MEM_COPY_S(&p_temp->SaveExpBytes[0],

				&p_temp->SaveExpBytes[wDataReg-MAX_OFFSET],

				MAX_OFFSET);

			wDataReg = MAX_OFFSET;

		}

		bytes_left -= bytes_saved;

		bytes_saved = 0;

		p_temp->pTinyBfrBrk = &p_temp->SaveExpBytes[wDataReg];



		if (p_temp->fState & LZS_EXPANDING_STR)

			goto Expand_String;



		pExpBfrStart = pExpBfr = &p_temp->SaveExpBytes[wDataReg];

		pExpBfrEnd = &p_temp->SaveExpBytes[DUMDUM_SZ];

	}



	p_temp->fState &= ~SAVED_CMPRS_BYTES;



	if (p_temp->CmpByteSaved) {

		wDataReg = (*sourceCnt >= 4) ? 4 : (unsigned short)*sourceCnt;

		pCmprsBfrEnd = &p_temp->SaveCmpBytes[p_temp->CmpByteSaved];



		for ( ; (wDataReg > 0);	wDataReg--) {

			*pCmprsBfrEnd++ = *pCmprsBfr++;

			/* Span compress buffer bytes */

		}

		pCmprsBfr = &p_temp->SaveCmpBytes[0];

		p_temp->fState |= SPANNING_CMPRS_BFRS;

	}

#endif	/* End of not FAST code */



Resume_Parsing:

	engn_return_code =



#if !defined (_FAST_)

		(wOption & LZS_UNCOMPRESSED) ?

		Expand_Null_Engine (&pCmprsBfr, pCmprsBfrEnd,

				    &pExpBfr,   pExpBfrEnd,

				    (struct RestartData LZS_FAR *)

					&p_temp->restart ) :

#endif	/* End of not FAST code */



		Expand_Lzs_Engine (&pCmprsBfr, pCmprsBfrEnd,

				     &pExpBfr,   pExpBfrEnd,

				     (struct RestartData LZS_FAR *)

					&p_temp->restart );



#if defined (_FAST_)

	LZS_InitDecompressionHistory(p_temp);

	if (engn_return_code & ENGN_END_OF_CMPRS_DATA)

		 ret_code |= LZS_END_MARKER;

	else if (engn_return_code & (	ENGN_STOPPED_AT_TOKEN

				     || ENGN_PARTIAL_TOKEN

				     || ENGN_STOPPED_GETTING_LEN))

		ret_code |= LZS_SOURCE_EXHAUSTED;

	else

		ret_code |= LZS_DEST_EXHAUSTED;



	goto Report_Results;

#else

	if (engn_return_code & ENGN_END_OF_CMPRS_DATA)

		goto End_Of_Data;



	if (engn_return_code & ENGN_STOPPED_AT_TOKEN)

		goto Cmprs_Data_End;



	if (engn_return_code & ENGN_PARTIAL_TOKEN)

		goto Cmprs_Data_End_Partial;



	if (engn_return_code & ENGN_STOPPED_GETTING_LEN)

		goto Cmprs_End_Getting_Len;



	if (engn_return_code & ENGN_EXPANDING_STR)

		goto Expand_Bfr_2_small;



	if (p_temp->fState & LZS_OVERRUNNN) {



OverRunnn:

Fill_Expand_Buffer:

		p_temp->fState      &= ~LZS_OVERRUNNN;

		p_temp->pTinyBfrBrk  = pExpBfrStart + bytes_left;

		p_temp->ExpByteSaved = (unsigned short)

			(FP_Off(pExpBfr) - FP_Off(p_temp->pTinyBfrBrk));



		pExpBfr = MEM_COPY_S(pExpStopped, pExpBfrStart,

				     (unsigned short)bytes_left);



		p_temp->fState |= TINY_EXPAND_BFR;

Break_Cmprs_Bfr:

		ret_code |= LZS_DEST_EXHAUSTED;



		if (p_temp->fState & SPANNING_CMPRS_BFRS) {

			short new_source_consumed;

			

			p_temp->fState &= ~SPANNING_CMPRS_BFRS;

			if ((new_source_consumed = (short) ((FP_Off(pCmprsBfr)

					- FP_Off(&p_temp->SaveCmpBytes[0])) 

					- (short) p_temp->CmpByteSaved)) >= 0)

				pCmprsBfr = *source + new_source_consumed;

			else {

				short i,cur_cmp_byte;

				

				pCmprsBfr = *source;

				p_temp->fState |= SAVED_CMPRS_BYTES;

				for (i=0,cur_cmp_byte = new_source_consumed + 

					(short) p_temp->CmpByteSaved;

					cur_cmp_byte < ((short) p_temp->CmpByteSaved - 1);

					i++,cur_cmp_byte++) {

					p_temp->SaveCmpBytes[i] = 

						p_temp->SaveCmpBytes[cur_cmp_byte];

				}

				p_temp->CmpByteSaved -= i;

				goto Report_Results;

			}

		}



		if (pCmprsBfrEnd == pCmprsBfr || pCmprsBfrEnd == pCmprsBfr + 1)

			ret_code |= LZS_SOURCE_EXHAUSTED;

		else

			pCmprsBfrEnd = pCmprsBfr + 1;

			/* Break compress buffer at current decode point */



		goto Cmprs_Data_Save;

	}

	if (p_temp->fState & TINY_EXPAND_BFR) {

		wDataReg = (unsigned short)(pExpBfr - pExpBfrStart);

		/* If some room in Expand buffer, move what fits */



		if (wDataReg > bytes_left)

			goto Fill_Expand_Buffer;

			/* Else, move all expanded and switch to Expand */

			/* buffer now that it has Max_Offset data 	*/



		pExpBfr = MEM_COPY_S(pExpStopped, pExpBfrStart, wDataReg);

		pExpBfrEnd = pExpandBuffer + (tMaxBfrSz) *destCnt;

		p_temp->fState &= ~TINY_EXPAND_BFR;

	}

	else {	/* Finish expand by using work buffer */



		p_temp->fState |= (LZS_OVERRUNNN | TINY_EXPAND_BFR);

		bytes_left = (tMaxBfrSz)(pExpBfrEnd - pExpBfr );

		pExpStopped = pExpBfr;



		LengthReg = (tMaxBfrSz)(  FP_Off(pExpBfr)

					- FP_Off(p_temp->pExpBfrBase) );



		if (LengthReg > MAX_OFFSET)

			LengthReg = MAX_OFFSET;



		pExpBfrStart = pExpBfr = p_temp->pTinyBfrBrk =

			MEM_COPY_S (&p_temp->SaveExpBytes[0],

				    pExpBfr - (unsigned short)LengthReg,

				    (unsigned short)LengthReg);



		pExpBfrEnd = &p_temp->SaveExpBytes[DUMDUM_SZ];



		if (!(bytes_left))

			goto OverRunnn;

	}

	goto Resume_Parsing;



/* Out of room when Expanding string */



Expand_Bfr_2_small:

	p_temp->fState |= LZS_EXPANDING_STR;



	if (p_temp->fState & TINY_EXPAND_BFR) { /* In save data buffer */

		wDataReg = (unsigned short)(

			FP_Off(pExpBfrEnd) - FP_Off(pExpBfrStart));



		if (bytes_left <= wDataReg)

			goto Fill_Expand_Buffer;

			/* Move some to expand buffer */





		bytes_left -= wDataReg;

		MEM_COPY_S(pExpStopped, pExpBfrStart, wDataReg);

		pExpStopped += wDataReg;

		wDataReg = (unsigned short)(

			  FP_Off(pExpBfr) - FP_Off(&p_temp->SaveExpBytes[0]));



		if (wDataReg > MAX_OFFSET) {

			/* Make more room by moving Prior data to mid point */



			MEM_COPY_S(&p_temp->SaveExpBytes[0],

				&p_temp->SaveExpBytes[wDataReg-MAX_OFFSET],

				MAX_OFFSET);

			wDataReg = MAX_OFFSET;

		}

		pExpBfrStart = pExpBfr = &p_temp->SaveExpBytes[wDataReg];



		p_temp->fState &= ~LZS_EXPANDING_STR;

		goto Resume_Parsing;	/* And expand more string */

	}

	else {

		p_temp->fState |= TINY_EXPAND_BFR;

		LengthReg = (tMaxBfrSz)(pExpBfrEnd - p_temp->pExpBfrBase);



		if (LengthReg > MAX_OFFSET)

			LengthReg = MAX_OFFSET;



		MEM_COPY_S (&p_temp->SaveExpBytes[0],

			    pExpBfrEnd - (unsigned short)LengthReg,

			    (unsigned short)LengthReg);



		p_temp->pTinyBfrBrk =

			&p_temp->SaveExpBytes[(unsigned short)LengthReg];



		p_temp->ExpByteSaved = 0;

		if (p_temp->fState & LZS_OVERRUNNN)

			goto OverRunnn;

	}

	goto Break_Cmprs_Bfr;



Cmprs_End_Getting_Len:

Cmprs_Data_End:

Cmprs_Data_End_Partial:

	LengthReg = 0;

	if (p_temp->fState & SPANNING_CMPRS_BFRS) {

		LengthReg = (tMaxBfrSz)(

			FP_Off(pCmprsBfr) - FP_Off(&p_temp->SaveCmpBytes[0]));



		if (LengthReg < p_temp->CmpByteSaved) {

			p_temp->fState |= SAVED_CMPRS_BYTES;

		}

		else {

			p_temp->fState &= ~(SPANNING_CMPRS_BFRS | SAVED_CMPRS_BYTES);

			pCmprsBfrEnd =   (pCmprsBfr = *source)

				       + (tMaxBfrSz) *sourceCnt;



			pCmprsBfr += LengthReg - p_temp->CmpByteSaved;



			if (*sourceCnt >= 4)

				goto Resume_Parsing;

		}

	}



	if (   !(engn_return_code & ENGN_PARTIAL_TOKEN)

	    && (FP_Off(pCmprsBfr) != FP_Off(pCmprsBfrEnd)))



		/* More tokens could be caught in BitStream */

		goto Resume_Parsing;



	if (p_temp->fState & TINY_EXPAND_BFR) {

		p_temp->pTinyBfrBrk = pExpBfrStart;

		p_temp->ExpByteSaved = (unsigned short)(

			FP_Off(pExpBfr) - FP_Off(pExpBfrStart) );

		pExpBfr = pExpStopped;

	}

	ret_code |= LZS_SOURCE_EXHAUSTED;



	/* Save remaining compressed bytes and restart information */



	if (p_temp->fState & SAVED_CMPRS_BYTES) {

		if (LengthReg) {

			pCmprsBfrEnd =

				  pCmprsBfr

				+  /* Unused prior compressed bytes */

				  (  p_temp->CmpByteSaved

				   - (unsigned short)LengthReg)

				+ (unsigned short)*sourceCnt;



		 for (wDataReg = 0; FP_Off(pCmprsBfr) < FP_Off(pCmprsBfrEnd);

			wDataReg++)

			p_temp->SaveCmpBytes[wDataReg] = *pCmprsBfr++;

		}

		p_temp->CmpByteSaved += (unsigned short)*sourceCnt;

		pCmprsBfr = *source + ((unsigned short)*sourceCnt);

		goto Report_Results;

	}



Cmprs_Data_Save:

	if ((ENGN_END_OF_CMPRS_DATA & EocdToken(&pCmprsBfr, pCmprsBfrEnd,

		(struct RestartData LZS_FAR *) &p_temp->restart )) &&

		(p_temp->ExpByteSaved == 0))

		goto Report_EOD;



	p_temp->fState |= SAVED_CMPRS_BYTES;

	p_temp->CmpByteSaved = (unsigned short)(pCmprsBfrEnd - pCmprsBfr);



	for (wDataReg=0; FP_Off(pCmprsBfr)<FP_Off(pCmprsBfrEnd); wDataReg++)

		p_temp->SaveCmpBytes[wDataReg] = *pCmprsBfr++;

	goto Report_Results;



End_Of_Data:

	if (p_temp->fState & SPANNING_CMPRS_BFRS) {

		/* Move what expanded fits to Expand buffer */



		p_temp->fState &= ~SPANNING_CMPRS_BFRS;

		LengthReg = (tMaxBfrSz)(

			FP_Off(pCmprsBfr) - FP_Off(&p_temp->SaveCmpBytes[0]));



		pCmprsBfrEnd =   (pCmprsBfr = *source)

			       + (tMaxBfrSz) *sourceCnt;



		if (LengthReg > p_temp->CmpByteSaved) {

			pCmprsBfr += LengthReg - p_temp->CmpByteSaved;

		}

	}

	if (p_temp->fState & TINY_EXPAND_BFR) {

		p_temp->fState |= LZS_END_OF_CMPRS_DATA;

		wDataReg = (unsigned short)(pExpBfr - pExpBfrStart);



		if (wDataReg > bytes_left)

			goto Fill_Expand_Buffer;



		pExpBfr = MEM_COPY_S(pExpStopped, pExpBfrStart, wDataReg) ;

	}



	if ((tMaxBfrSz)(pExpBfr - pExpandBuffer) == *destCnt)

		ret_code |= LZS_DEST_EXHAUSTED;



Report_EOD:

	/* Used, prior bytes in save bfr */



	if (*sourceCnt == (tMaxBfrSz)(pCmprsBfr - *source))

		ret_code |= LZS_SOURCE_EXHAUSTED;



	bytes_saved = (tMaxBfrSz)(p_temp->pTinyBfrBrk

		- &p_temp->SaveExpBytes[0]);



	LZS_InitDecompressionHistory(p_temp);



	if (wOption & LZS_SAVE_HISTORY) {

		LengthReg = (tMaxBfrSz)(pExpBfr - p_temp->pExpBfrBase);

		/* Bytes expand into users bfr */



		if ((LengthReg + bytes_saved) > MAX_OFFSET) {

			if (LengthReg > MAX_OFFSET) {

				LengthReg = MAX_OFFSET;

				bytes_saved = 0;

			}

			else {

				wDataReg = (unsigned short)bytes_saved;

				bytes_saved = (unsigned short)(

					    MAX_OFFSET

					  - (unsigned short)LengthReg);



				wDataReg -= (unsigned short)bytes_saved;

				MEM_COPY_S(&p_temp->SaveExpBytes[0],

					   &p_temp->SaveExpBytes[wDataReg],

					   (unsigned short)bytes_saved);

			}

		}

		p_temp->pTinyBfrBrk =

			MEM_COPY_S(&p_temp->SaveExpBytes[bytes_saved],

				   pExpBfr - (unsigned short)LengthReg,

				   (unsigned short)LengthReg);



		p_temp->fState = TINY_EXPAND_BFR;

	}

	if (!(wOption & LZS_UNCOMPRESSED))

		ret_code |= LZS_END_MARKER;			/* Only set of compressed data */



#endif	/* End of not FAST code */



Report_Results:



#if !defined(_SAVE_DECOMPRESS_POINTERS_)

	delta_dest   = FP_Off(pExpBfr) - FP_Off(pExpandBuffer);

	delta_source = FP_Off(pCmprsBfr) - FP_Off(*source);



	p_temp->svd_sourceCnt = *sourceCnt - delta_source;

	p_temp->svd_source	  = pCmprsBfr;

	p_temp->svd_destCnt	  = *destCnt   - delta_dest;

	p_temp->svd_dest	  = pExpBfr;



	/* Default to source, dest valid. */

	p_temp->svd_flags    |= SVD_SOURCE_VALID | SVD_DEST_VALID;



	/* Restore calling source values */

	*source = call_source;

	*sourceCnt = call_sourceCnt;

	

	/* Override with new values if done with source buffer */

	if (wOption & LZS_UNCOMPRESSED) {



		/* On LZS_UNCOMPRESSED, pretend source delta == dest delta */

		*sourceCnt	-= delta_dest;

		*source	    = p_temp->svd_source + delta_dest - delta_source;



		/* If updating source counter, invalidate saved source data */

		p_temp->svd_flags &= ~SVD_SOURCE_VALID;

	} else if (ret_code & (LZS_END_MARKER | LZS_SOURCE_EXHAUSTED)) {

 		*sourceCnt	= p_temp->svd_sourceCnt;

		*source	    = p_temp->svd_source;	/* update source ptrs		*/



		/* If updating source counter, invalidate saved source data */

		p_temp->svd_flags &= ~SVD_SOURCE_VALID;

	}



	/* Restore calling dest values */

	*dest = call_dest;

	*destCnt = call_destCnt;



	/* Override with new values if done with dest buffer */

	if((wOption & LZS_UNCOMPRESSED)

		|| (ret_code & (LZS_END_MARKER | LZS_DEST_EXHAUSTED)))

	{

		*destCnt    = p_temp->svd_destCnt;

		*dest       = p_temp->svd_dest;		/* update dest ptrs			*/



		/* If updating source counter, invalidate saved source data */

		p_temp->svd_flags &= ~SVD_DEST_VALID;

	}



	if(ret_code & LZS_END_MARKER)

	{

		/* If EOCD, invalidate saved source and dest info */

		p_temp->svd_flags &= ~(SVD_SOURCE_VALID | SVD_DEST_VALID);

	}

#else

	*destCnt   -= FP_Off(pExpBfr) - FP_Off(pExpandBuffer);

	*dest	    = pExpBfr;

	*sourceCnt -= FP_Off(pCmprsBfr) - FP_Off(*source);

	*source   	= pCmprsBfr;

#endif



	return (ret_code);

}



#if !defined (_FAST_)



/*----------------------------------------------------------------------*/

/* Expand_Null_Engine - Expands from a (expanded) buffer to data		*/

/*----------------------------------------------------------------------*/

/* 	Not used if _FAST_ 													*/

/* In:																	*/

/*	source			(referenced) pointer to (compressed) buffer			*/

/*	pCmprsBfrEnd    Compress buffer end ***								*/

/*	ppExpBfr 		(referenced) pointer to Expansion buffer			*/

/*	pExpBfrEnd		Expansion buffer end ***							*/

/*	pRestartData	pointer to RestartData                          	*/

/*																		*/

/* Returns:																*/

/*	ENGN_OVERRUNNN				Too close to expand buffer end.			*/

/*	ENGN_END_OF_CMPRS_DATA		Found End of Compressed Data Token		*/

/*																		*/

/* Updates:																*/

/*	pExpandBuffer   Next expansion byte.								*/

/*	pCmprsBuffer    Next compressed byte								*/

/*																		*/

/*	*** = Not used if _FAST_											*/

/* Terminates when either source or destination end is encountered.		*/

/*----------------------------------------------------------------------*/



unsigned int LZS_FAR Expand_Null_Engine(

	unsigned char LZS_FAR * LZS_FAR *ppCmprsBfr,

	unsigned char LZS_FAR	        *pCmprsBfrEnd,

	unsigned char LZS_FAR * LZS_FAR *ppExpBfr,

	unsigned char LZS_FAR	        *pExpBfrEnd,

	struct RestartData LZS_FAR      *pRestartData

)

{

	dREG unsigned long in_length;		/* Length of (uncomprssed) bytes 	*/

	unsigned long 	   out_length;		/* Length of Expand buffer	 		*/

	unsigned short 	   delta;



	pRestartData->engnState = ENGN_END_OF_CMPRS_DATA;

	in_length  = (tMaxBfrSz)(pCmprsBfrEnd - *ppCmprsBfr);

	out_length = (tMaxBfrSz)(pExpBfrEnd   - *ppExpBfr);



	if (in_length > out_length) {

		in_length = out_length;

		pRestartData->engnState = ENGN_OVERRUNNN;

	}



	while (in_length > 0 ) {

		delta = (in_length > 0xFFFFUL) ? (unsigned short) in_length: 1U;

		*ppExpBfr = mem_copy_r (*ppExpBfr, *ppCmprsBfr, delta);

		*ppCmprsBfr += delta;

		in_length -= delta;

	}

	return (pRestartData->engnState);

}

#endif	/* End of not FAST expansion */



/*-------------------------- End of LZSD.C -----------------------------*/

