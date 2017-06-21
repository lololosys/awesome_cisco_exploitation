/* $Id: stac_comp.c,v 3.2.62.1 1996/03/18 22:48:59 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/wan/stac_comp.c,v $
 *------------------------------------------------------------------
 * COMP_STAC.C
 *
 * Feburary 11, 1994 Bill Thomas
 *
 * Copyright (c) 1994,1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: stac_comp.c,v $
 * Revision 3.2.62.1  1996/03/18  22:48:59  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  11:08:34  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:53:55  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:05:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:20:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
* 
*  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*  !!!																	   !!!
*  !!!	NOTE:															   !!!
*  !!!																	   !!!
*  !!!	The contents of this document constitute CONFIDENTIAL INFORMATION  !!!
*  !!!	which is the property of Stac Electronics.						   !!!
*  !!!																	   !!!
*  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
* 
*/
/*  TITLE   LZSC.C  -  C Version of Stac's compression algorithm
*
******************************************************************************
*
* Copyright 1988-92 Stac Electronics, Carlsbad, California.
* All rights reserved.  This code is Stac confidential property, and the
* algorithms and methods used herein may not be disclosed to any party without
* express written consent from Stac Electronics.
*
******************************************************************************

******************************************************************************
*
*
* Filename:          LZSC.C
*
* Project:           DCS221-C
*
* Functional Block:  DCS221-C
*
* Comments:          C language version of Stac Electronic's data compression
*					 and decompression algorithms.
*
*					 Four space tabs have been used while formatting this
*					 document (col 5 9 13 etc.)
*
* Contents:
*
*	Included Files
*	Symbolic Constants
*	Structures and Unions
*	GetHashFor				MACRO: Hashes a byte pair
*	PutBitsMac				MACRO: Used to call the PutBits function
*	PutARawByte				MACRO: Output a raw byte
*	StartNewString			MACRO: Start new string with currentBytePair
*	HashTableNeedsRefresh	MACRO: Return TRUE if hash table needs refresh
*	CurrentStringExtends	MACRO: Returns TRUE if current string extends
*	Global Variables
*	Function Prototypes
*	InitCompressVars		- Initialize compression variables
*	LZS_InitHistory			- Initialize compression/decompression engines
*	InitHashTable			- Initializes the hash table
*	RefreshOldHashEntries	- Makes sure old hash table entries look old
*	PutBits					- Outputs bits of compressed data
*	PutEndMark				- Outputs an end of compressed data mark
*	PutCompressedString		- Output a compressed string of data
*	PerformFlush			- Flush all compressed data & write EOCD mark
*	LZS_Compress			- Compress a buffer full of data
*	InitDecompressVars		- Initialize decompression variables
*	GetBits					- Get n bits from input file
*	PutOutput				- Output a byte to outFile and update history
*	LZS_Decompress			- Decompress a buffer of data
*
******************************************************************************
*
* Version Control Information:
*
*    Rev 3.01  01 Dec 1992 17:38:00   beb
*     Added GetABit function to speed up LZS_Decompress by 20%.
*
*    Rev 3.00   11 Nov 1992 16:48:40   lap
*     Added performance tuning parameters and removed indirection for
*     greater speed performance.
*
*    Rev 1.00   01 Apr 1992 14:07:04   beb
*     Initial revision.
*
*
*
******************************************************************************

******************************************************************************
*
*						 Included Files
*
*****************************************************************************/

/* Debug switch */

#define		LZS_DEBUG	0		/* 0 = Debug off, 1 = Debug on */

#include	"stac_comp.h"

/*

****************************************************************************
*
* Block Name:        Symbolic Constants
*
* Functional Block:  DCS221-C
*
* Special Notes:     
*
*	09/23/92  tim	Take note of the "not_used" conditional symbol.
*					It is for temporarily removing a function which was
*					replaced with in-line code.
*
*****************************************************************************/

/* Various Compression Constants */

#define	HISTORY_SIZE			2048
#define	HISTORY_MASK			(HISTORY_SIZE - 1)

#define	MAX_STRING_LENGTH		0xFFFFFFFFL
#define	MIN_RLE_LENGTH			23   /* Min string length for RLE encoding	*/
#define	BITS_PER_RLE_UNIT		4	 /* # bits in each unit of RLE length	*/
#define	RLE_UNIT_LENGTH			((1 << BITS_PER_RLE_UNIT) - 1)
									 /* # bytes input for each unit of RLE	*/
									 /* encoded output */
									 /* to start */


/* Hash Table related */

#define	HASH_TABLE_SIZE			4096
#define	BOUNDARY_BETWEEN_HALVES	0x7FFFFFFFL	/* history pointer test mask	*/
#define	INVALID_FOR_1ST_HALF	0xC0000000L	/* history pointer value		*/
#define	INVALID_FOR_2ND_HALF	0x40000000L	/* history pointer value		*/


/*

****************************************************************************
*
* Block Name:        Performance tuning parameters
*
* Functional Block:  DCS221-C
*
* Special Notes:     SEARCH_COUNT_LIMIT specifies the maximun number of 
*					 attempts to find a longer string that starts with the
*					 current byte pair being processed.  The range of valid
*					 values for SEARCH_COUNT_LIMIT is 0 to 2044.
*
*					 REMATCH_SIZE_LIMIT specifies the compressed string length
*					 which is "good enough".  Once a string of this length
*					 has been found no attempt will be made to find a longer
*					 string.  The range of valid values for REMATCH_SIZE_LIMIT
*					 is 2 to 23.
*
*					 For both of these parameters lowering the parameter
*					 increases speed and decreases compression.  Increasing
*                    either or both parameters decreases the speed and raises
*					 the compression.
*
*					 Note that SEARCH_COUNT_LIMIT was replaced by the
*					 performance parameter to LZS_Compress starting with
*					 version 3.0 of this code.
*
*****************************************************************************/


#define	SEARCH_COUNT_LIMIT		8	/* Max attempted hash bucket searches	*/
#define	REMATCH_SIZE_LIMIT		8	/* String size limit on hash searches	*/


/*

****************************************************************************
*
* Block Name:        Shorthand declarations
*
* Functional Block:  DCS221-C
*
* Special Notes:
*
*****************************************************************************/

/* shorthand declarations */			/* size assumptions */

typedef	unsigned	char	uchar;		/*  8 bits */
typedef	unsigned	int		uint;		/* 16 bits */
typedef	unsigned	short	ushort;		/* 16 bits */
typedef	unsigned	long	ulong;		/* 32 bits */

typedef	signed		char	schar;		/*  8 bits */
typedef	signed		int		sint;		/* 16 bits */
typedef	signed		short	sshort;		/* 16 bits */
typedef	signed		long	slong;		/* 32 bits */

ulong x25_pbp_stac_history_size = LZS_HISTORY_SIZE;


/* Other constant definitions */

#define		boolean		ushort
#define		TRUE		1
#define		FALSE		0
#define		OK			TRUE
#define		NOT_OK		FALSE


/*

******************************************************************************
*
* Block Name:        Scratch RAM structure(s)
*
* Functional Block:
*
* Special Notes:
*
*****************************************************************************/

typedef struct
	{
	uchar LZS_FAR *	inputDataHandle ; /* global pointer into source buffer	*/
	ulong			inputDataCount  ; /* global bytes left in source buffer */
	uchar LZS_FAR *	outputDataHandle; /* global pointer into dest buffer	*/
	ulong			outputDataCount ; /* global bytes left in dest buffer	*/

						/* Compress function variables */

	ulong		historyPointer;
	ulong		historyPointerMinus1;
	ushort		historyIndexMinus1;

	uchar		currentByte;				/* last byte read in		*/
	uchar		dummy1;			/* This preserves the size of the struct	*/
								/* when a word aligned machine is used (68K)*/
	ushort		currentBytePair;			/* last two bytes read in	*/

	ulong		stringLength;				/* current string length,	*/
											/* not including RLE_Length.*/

						/* !!! Note that stringLength == 0 means !!! */
						/* !!! that there is no	current string	 !!! */

	ulong		RLE_Length;		/* Amount of string that is RLE encoded		*/
								/* Note total string length = stringLength	*/
								/* + RLE_Length for recently hashed byte	*/
								/* pairs.									*/

	ushort		matchIndex;			/* index into history of current		*/
									/* match string							*/

	ulong LZS_FAR	* hashAddress;	/* pointer into hash table				*/
	ulong		bytePairOffset;		/* offset to last byte pair w/ same		*/
									/* hash as currentBytePair				*/

	ushort		matchStringOffset;	/* offset to current match string		*/
	ushort		currentStringIndex;	/* history index of current match string*/

	ushort		firstMatchOffset;  /* offset to 1st match of currentBytePair*/
	ushort		rematchOffset;	/* possible currentBytePair rematch offset	*/
	ushort		searchCount;	/* depth of stringList (hash bucket) search	*/
	boolean		compressedSomeData;	/* first pass thru compress flag		*/


/* NOTE: PutBits VARS MUST BE GLOBAL FOR CURRENT RECURSIVE CODING OF PutBits*/

						/* PutBits Input Vars */

	ushort		bitPattern;		/* right justified bit pattern to output	*/
	sshort		numberOfBits;	/* number of bits in pattern to output		*/

						/* PutBits state vars */

	uchar		outputByte;		/* current partial byte not yet output		*/
	uchar		dummy2;			/* This preserves the size of the struct	*/
								/* when a word aligned machine is used (68K)*/
	sshort		nextFreeBit;	/* next available bit position in outputByte*/
								/* 1 relative from the right.				*/

						/* PutBits return value */

	boolean		destBufferFull;

						/* PutCompressedString variables */

	boolean		stringOutputStarted;

	} CscratchRamType;

/*--------------------------------------------------------------------------*/

typedef struct
	{
	uchar LZS_FAR *	inputDataHandle ; /* global pointer into source buffer	*/
	ulong			inputDataCount  ; /* global bytes left in source buffer */
	uchar LZS_FAR *	outputDataHandle; /* global pointer into dest buffer	*/
	ulong			outputDataCount ; /* global bytes left in dest buffer	*/

	ushort		historyIdx;					/* current index into history	*/
	ushort		lastByteOfCompressedInput;	/* unused bits from last read	*/
											/* byte of compressed input	*/

	ushort		decompressState;
	boolean		decompressInitialized;		/* global state flag	*/

	sshort		getBitsCount;	/* Number of bits to get from source buffer	*/
	ushort		sourceBits;		/* Holds the return value					*/
	ushort		getBitsResult;	/* Holds the return value					*/
	sshort		bitAlign;		/* number of bits not yet used from	last	*/
								/* read byte of compressed input			*/

	sshort		offset;			/* Offset of current string into history	*/
	ulong		length;			/* Length of current string					*/

	boolean		decompressingAString;		/* global state flag	*/
	}
		DscratchRamType;

/*--------------------------------------------------------------------------*/

typedef struct
	{
	uchar	c_history[HISTORY_SIZE];	/* compression history				*/

	ushort	stringList[HISTORY_SIZE];	/* Hash buckets containing linked	*/
										/* list of relative offsets to		*/
										/* similarly hashed byte pairs		*/

	ulong	hashTable[HASH_TABLE_SIZE]; /* contains historyPointer values	*/

	CscratchRamType	sc ;

	uchar	d_history[HISTORY_SIZE];	/* decompression history			*/

	DscratchRamType	sd ;

	uchar	reserved[64];				/* room for future growth			*/
	}
		scratchRamType ;

#if LZS_DEBUG
#include	<stdio.h>
#include	<stdlib.h>
#endif

/*
*****************************************************************************
*
*						 Other Structure and Union Definitions
*
****************************************************************************/

typedef	struct lengthTableEntryStruct
	{
	ushort	pattern;
	sshort	size;
	}
		lengthTableEntryType;


/*

*****************************************************************************
*
*						 Macro Definitions
*
****************************************************************************/

/* min() is not available with some compilers */

/* #ifndef min(a,b) */

#ifndef min

#define min(a,b)    (((a) < (b)) ? (a) : (b))

#endif

/*

+*****************************************************************************
*
* Function Name:	GetHashFor
*
* Function:         MACRO: Hashes a byte pair
*
* Arguments:        diByte	- byte pair to be hashed.
*
* Globals Used:     None.
*
* Return:           12 bit hashed value for the 16 bit byte pair.
*
* Globals Altered:  None.
*
* Error Handling:	None.
*
* Algorithms:		If x and y represent the each byte of the sixteen bit byte
*					pair then this function is defined as follows:
*
*						Hash(x,y) = (x >> 4) ^ y;
*
* Notes:			Note that the hash is non-unique in that sixteen bit
*				    values are hashed into 12 bit values so duplications are
*					certain to occur.  Testing this particular function
*					several others on a large mixed data set showed that it
*					did an excellent job of distributing the encountered
*					values evenly amoung the has buckets.
*
*					This has function also has the following property:
*
*						if Hash(x,y) = Hash(a,b), and if x = a then y = b.
*
*					This means that only one byte in a hashed pair needs to
*					be tested to guarentee uniqueness.  The compress function
*					makes use of this fact to speed its search thru the
*					stringList (hash buckets).
*
-****************************************************************************/

#define GetHashFor(diByte) ((((diByte) & 0xFF00) >> 4) ^ ((diByte) & 0x00FF))


/*

+*****************************************************************************
*
* Function Name:	PutBitsMac
*
* Function:         Macro for calling the PutBits function
*
* Arguments:        bits	- Right aligned compressed data bit pattern to
*							   output
*					count	- Number of bits of compressed data to output
*
* Globals Used:     See the function PutBits.
*
* Return:           None.
*
* Globals Altered:  bitPattern		- set to bits
*					numberOfBits	- set to count;
*					See the function PutBits.
*
* Error Handling:	The global destBufferFull is set to TRUE if the
*					destination buffer count is exhausted.  It is set to OK
*					otherwise.
*
* Algorithms:
*
* Notes:			This macro exists to save time passing arguments to the
*					PutBits function.
*
*
*
-****************************************************************************/

#define PutBitsMac(bits,count) 	sr.sc.bitPattern=(bits); \
								sr.sc.numberOfBits=(count); \
								PutBits()


/*

+*****************************************************************************
*
* Function Name:	PutARawByte
*
* Function:         MACRO: Output a raw byte
*
* Arguments:        rawByte	- Right aligned raw byte value to output.
*
* Globals Used:     See the function PutBits.
*
* Return:           None.
*
* Globals Altered:  See the function PutBits.
*
* Error Handling:	The global destBufferFull is set to TRUE if the
*					destination buffer count is exhausted.  It is set to OK
*					otherwise.
*
* Algorithms:
*
* Notes:
*
-****************************************************************************/

#define	PutARawByte(rawByte) PutBitsMac((rawByte),9); event = PUT_RAW_BYTE


/*

+*****************************************************************************
*
* Function Name:	StartNewString
*
* Function:         Start a new string with currentBytePair
*
* Arguments:        None.
*
* Globals Used:     firstMatchOffset
*					historyIndexMinus1
*
* Return:           None.
*
* Globals Altered:  currentStringIndex
*					matchStringOffset
*					stringLength
*
* Error Handling:	None.
*
* Algorithms:
*
* Notes:			This macro tidys up use of inline code for speed.
*
-****************************************************************************/

#define	StartNewString() sr.sc.stringLength = 2; \
						 sr.sc.currentStringIndex=sr.sc.historyIndexMinus1; \
						 sr.sc.matchStringOffset = sr.sc.firstMatchOffset; \
						 event = STARTED_STRING


/*

+*****************************************************************************
*
* Function Name:	HashTableNeedsRefresh
*
* Function:         MACRO: Returns TRUE if hash table needs refreshing
*
* Arguments:        None.
*
* Globals Used:     historyPointer
*
* Return:           TRUE if the hashTable need to refreshed, FALSE otherwise.
*
* Globals Altered:  None.
*
* Error Handling:	None.
*
* Algorithms:		See the function RefreshOldHashEntries.  Essentially the
*					has table is refreshed whenever the historyPointer equals
*					0 or 0x80000000.
*
* Notes:
*
-****************************************************************************/

#define HashTableNeedsRefresh() ((sr.sc.historyPointer & BOUNDARY_BETWEEN_HALVES) == 0)


/*

+*****************************************************************************
*
* Function Name:	CurrentStringExtends
*
* Function:         MACRO: Returns TRUE if current string extends
*
* Arguments:        None.
*
* Globals Used:     currentByte
*					c_history
*					matchIndex
*					stringLength
*
* Return:           TRUE if the next byte of the current match string equals
*					currentByte (the last byte read in), FALSE otherwise.
*
* Globals Altered:  None.
*
* Error Handling:	None.
*
* Algorithms:
*
* Notes:
*
-****************************************************************************/

#define CurrentStringExtends() 	(sp->c_history [ (ushort) \
								(( (ulong) sr.sc.matchIndex \
								+ sr.sc.stringLength + sr.sc.RLE_Length) \
								& (ulong) HISTORY_MASK)] == sr.sc.currentByte)


/*

*****************************************************************************
*
*						 Variable Declarations
*
****************************************************************************/

/* Copyright */

/* GNU compiler complains:
 * ../unix/comp_stac.c:594: warning: `copyright' defined but not used
 * make: *** [comp_stac.o] Error 1
 *static char	copyright[] =	"LZS221-C version 2.01\nCopyright 1988-92 Stac Electronics, Carlsbad, California.\nAll rights reserved.";
 */
static scratchRamType	LZS_FAR *	sp ;	/* global pointer to the scratch RAM */
static scratchRamType			 	sr ;	/* copy of the scratch RAM */

static lengthTableEntryType	lengthTable[24] =	/* look up table used to encode	*/
{										/* string lengths */
	{ 0x0,	0 },	/* not used		*/
	{ 0x0,	0 },	/* not used		*/
	{ 0x0,	2 },	/* length = 2	*/
	{ 0x1,	2 },	/* length = 3	*/
	{ 0x2,	2 },	/* length = 4	*/
	{ 0xC,	4 },	/* length = 5	*/
	{ 0xD,	4 },	/* length = 6	*/
	{ 0xE,	4 },	/* length = 7	*/
	{ 0xF0,	8 },	/* length = 8	*/
	{ 0xF1,	8 },	/* length = 9	*/
	{ 0xF2,	8 },	/* length = 10	*/
	{ 0xF3,	8 },	/* length = 11	*/
	{ 0xF4,	8 },	/* length = 12	*/
	{ 0xF5,	8 },	/* length = 13	*/
	{ 0xF6,	8 },	/* length = 14	*/
	{ 0xF7,	8 },	/* length = 15	*/
	{ 0xF8,	8 },	/* length = 16	*/
	{ 0xF9,	8 },	/* length = 17	*/
	{ 0xFA,	8 },	/* length = 18	*/
	{ 0xFB,	8 },	/* length = 19	*/
	{ 0xFC,	8 },	/* length = 20	*/
	{ 0xFD,	8 },	/* length = 21	*/
	{ 0xFE,	8 }		/* length = 22	*/
	};


static uint	performanceMode;

/* Performance mode hash decision table */

/*	Performance mode:	PM=0	PM=1	PM=2	   Event */

static uint	okToHash[5][3]=	{
						{TRUE,	TRUE,	TRUE},	/* Put a raw byte			*/
						{TRUE,	TRUE,	TRUE},	/* Started a new string		*/
						{FALSE,	FALSE,	TRUE},	/* Extended a string		*/
						{FALSE,	FALSE,	TRUE},	/* Found a longer match		*/
						{FALSE,	TRUE,	TRUE}	/* Ended a string			*/
						};


/* Enumerated event values used with the hash decision table */

enum	{
		PUT_RAW_BYTE,
		STARTED_STRING,
		EXTENDED_STRING,
		FOUND_LONGER_MATCH,
		ENDED_STRING
		};

static uint	event;


/* decompression states */

enum
	{
	RAW_OR_COMPRESSED,
	PUTTING_RAW_BYTE,
	GETTING_OFFSET_TYPE,
	GETTING_OFFSET,
	LENGTH_DIBIT_1,
	LENGTH_DIBIT_2,
	LENGTH_NIBBLE_2,
	PUT_RLE_DATA,
	RLE_BITS
	};

/*

*****************************************************************************
*
*						 Function Prototypes
*
****************************************************************************/

static void InitCompressVars(void);

static void RefreshOldHashEntries(void);

static void PutBits(void);

static void PutEndMark(void);

static void InitHashTable
			(
			ulong LZS_FAR *		hashTablePtr,
			ulong				initalValue
			);

static void PutCompressedString(
							   ushort	rawStringOffset,
							   ulong	rawStringLength
							   );

static void InitDecompressVars(void);

static boolean GetABit(void);

static boolean GetBits(void);

static void PutOutput(uchar c);


/*

+*****************************************************************************
*
* Function Name:	InitCompressVars
*
* Function:         Initialize compression variables
*
* Arguments:        None.
*
* Globals Used:     None.
*
* Return:           None.
*
* Globals Altered:  sr.sc.compressedSomeData
*					sr.sc.currentBytePair
*					sr.sc.destBufferFull
*					sr.sc.matchStringOffset
*					sr.sc.nextFreeBit
*					sr.sc.outputByte
*					sr.sc.stringLength
*
* Error Handling:	None.
*
* Algorithms:
*
* Notes:			The history pointers, history indicies, and hash table
*					are NOT initialized by this function.
*
-****************************************************************************/

static void InitCompressVars(void)
	{

	/*
	Local Data Declarations:
	*/

	/*
	Body of function:
	*/

	sr.sc.nextFreeBit			= 8;

	sr.sc.outputByte			= 0;

	sr.sc.matchStringOffset		=
	sr.sc.currentBytePair		= 0;

	sr.sc.stringLength			=
	sr.sc.RLE_Length			= 0;

	sr.sc.compressedSomeData	=
	sr.sc.stringOutputStarted	= FALSE; /* used by PutCompressedString */

	return;

	}	/* InitCompressVars */

/*

+*****************************************************************************
*
* Function Name:	LZS_InitHistory
*
* Function:         Initialize compression scratch area
*
* Arguments:        None.
*
* Globals Used:     None.
*
* Return:           None.
*
* Globals Altered:
*
*	compress variables:
*
*					compressedSomeData
*					currentBytePair
*					historyPointer
*					historyPointerMinus1
*					matchStringOffset
*					nextFreeBit
*					outputByte
*					destBufferFull
*					stringLength
*
*	decompress variables:
*
*					bitAlign
*					decompressingAString
*					decompressInitialized
*					hashTable
*					histEndOfCompression
*					historyIdx
*					lastByteOfCompressedInput
*					length
*					offset
*
* Error Handling:	None.
*
* Algorithms:
*
* Notes:
*
-****************************************************************************/

void LZS_FAR	LZS_InitHistory(void LZS_FAR	*scratch)
	{

	/*
	Local Data Declarations:
	*/

	/*
	Body of function:
	*/

#if LZS_DEBUG
	if (LZS_HISTORY_SIZE != sizeof(sr))
		{
		printf(
			  "The value of LZS_HISTORY_SIZE is incorrect it should be %d!\n",
			  sizeof(sr)
			  );
		exit (-1);
		}
#endif

	sp = (scratchRamType LZS_FAR *) scratch;

	/* compression vars */

	sr.sc = sp->sc ;					/* Make local copy of vars		*/
	sr.sc.historyPointer		= 0xFFFFFFFFL;					/* -1	*/
	sr.sc.historyPointerMinus1  = sr.sc.historyPointer - 1;
	sr.sc.destBufferFull		= FALSE;
	InitCompressVars();
	InitHashTable(sp->hashTable, INVALID_FOR_1ST_HALF);
	sp->sc = sr.sc;						/* Copy locals back over globals */

	/* decompression stuff */

	sr.sd = sp->sd ;					/* Make local copy of vars */
	sr.sd.historyIdx = 0; /* start over at beginning of decompress history */
	InitDecompressVars();
	sp->sd = sr.sd ;					/* Copy locals back over globals */

	return;

	}	/* LZS_InitHistory */

/*

+*****************************************************************************
*
* Function Name:	InitHashTable
*
* Function:         Initializes the hash table
*
* Arguments:        hashTablePtr	- Address of hash table to be initialized
*					initialValue	- Initial value for hash table entries
*
* Globals Used:     None.
*
* Return:           None.
*
* Globals Altered:  None.
*
* Error Handling:	None.
*
* Algorithms:
*
* Notes:			Fills hash table with initial value.
*
-****************************************************************************/

static void InitHashTable
			(
			ulong LZS_FAR *		hashTablePtr,
			ulong				initalValue
			)
	{

	/*
	Local Data Declarations:
	*/

	ushort	i;

	/*
	Body of function:
	*/

	for (i = 0; i < HASH_TABLE_SIZE; i++)

		*hashTablePtr++ = initalValue;

	return;

	}	/* InitHashTable */

/*

+*****************************************************************************
*
* Function Name:	RefreshOldHashEntries
*
* Function:         Makes sure old hash table entries look old
*
* Arguments:        None.
*
* Globals Used:     hashTable
*					historyPointer
*
* Return:           None.
*
* Globals Altered:  hashTable
*
* Error Handling:	None
*
* Algorithms:		The values of history pointer range from 0 to 0xFFFFFFFF.
*					This range is divided into two sub-ranges, the first half,
*					0 thru 0x7FFFFFFF inclusive, and the second half including
*					0x80000000 thru 0xFFFFFFFF.  As the transition from the
*					first half to the second half is made old hash entries are
*					filled with a value from the first half to guarentee that
*					they will be invalid throughout the second half.  The
*					complementary process happens on the transition from the
*					second half back to the first half.
*
* Notes:			Since the hashTable contains 32 bit history pointer values
*					this refresh only takes place once for every 2 gigabytes
*					of raw data input to the compress function.
*
-****************************************************************************/

static void RefreshOldHashEntries()
	{

	/*
	Local Data Declarations:
	*/

	register	ushort			i;
				ulong LZS_FAR *	invalidationPtr;
				ulong			invalidationValue;


	/*
	Body of function:
	*/

	invalidationValue = INVALID_FOR_1ST_HALF;		/* assume first half	*/
	if (sr.sc.historyPointer != 0)					/* which half ?			*/
		invalidationValue = INVALID_FOR_2ND_HALF;	/* second half			*/

	invalidationPtr = sp->hashTable;				/* for every table entry*/
	for (i = 0; i < HASH_TABLE_SIZE; i++, invalidationPtr++)

		/* if the entry is old (minus 2 accounts for currentBytePair */
		if (sr.sc.historyPointer - *invalidationPtr > HISTORY_SIZE - 2)

			*invalidationPtr = invalidationValue;	/* invalidate old entry */

	return;

	}	/* RefreshOldHashEntries */

/*

+*****************************************************************************
*
* Function Name:	PutBits
*
* Function:         Outputs bits of compressed data
*
* Arguments:        None.
*
* Globals Used:     bitPattern
*					nextFreeBit
*					numberOfBits
*					outputByte
*					outputDataCount
*					outputDataHandle
*
* Return:           None.
*
* Globals Altered:  destBufferFull
*					outputByte
*					nextFreeBit
*					outputDataHandle
*					outputDataCount
*					outputByte
*					bitPattern
*					numberOfBits
*
* Error Handling:	The global destBufferFull is set to TRUE if the
*					destination buffer count is exhausted.  It is set to OK
*					otherwise.
*
* Algorithms:		Basically input comes from the variables bitPattern and
*					numberOfBits.  bitPattern contains the right aligned
*					compressed bit pattern to output.  Number of bits contains
*					the nubmer of bits to output from bitPattern.  The
*					new bit pattern is or'd left justified into outputByte
*					starting at nextFreeBit.  The values of nextFreeBit range
*					from 8 (msb) to 1 (lsb).  When the value of nextFreeBit
*					reaches 0 outputByte is written to the destination buffer
*					and nextFreeBit is reset to 8.
*
* Notes:			OutputByte and nextFree bit are globals whose state is
*					preserved across calls.  After all of the source data
*					has been compressed a call to PutEndMark will assure that
*					any remaining data held held in output byte is flushed to
*					buffer.
*
-****************************************************************************/

static void	PutBits(void)
	{

	/*
	Local Data Declarations:
	*/

	register	sshort		shiftCount;

	/*
	Body of function:
	*/

	if (sr.sc.outputDataCount == 0)	/* watch for full output buffer			*/
		sr.sc.destBufferFull = TRUE;/* initialized to OK by LZS_InitHistory */

	if ((shiftCount = sr.sc.numberOfBits - sr.sc.nextFreeBit) > 0)
		{
		sr.sc.outputByte |= (sr.sc.bitPattern >> shiftCount);
		sr.sc.nextFreeBit = 8;
		*sr.sc.outputDataHandle++ = sr.sc.outputByte;
		--sr.sc.outputDataCount;
		sr.sc.outputByte = 0;

		sr.sc.bitPattern &= ((1 << shiftCount) - 1);
		sr.sc.numberOfBits = shiftCount;
		PutBits();
		}

	else if (shiftCount < 0)
		{
		sr.sc.outputByte |= (sr.sc.bitPattern << -shiftCount);
		sr.sc.nextFreeBit -= sr.sc.numberOfBits;
		}

	else /* (shiftCount == 0) */
		{
		sr.sc.outputByte |= sr.sc.bitPattern;
		sr.sc.nextFreeBit = 8;
		*sr.sc.outputDataHandle++ = sr.sc.outputByte;
		--sr.sc.outputDataCount;
		sr.sc.outputByte = 0;
		}

	return;

	}	/* PutBits */

/*

+*****************************************************************************
*
* Function Name:	PutEndMark
*
* Function:         Outputs an end of compressed data mark
*
* Arguments:        None.
*
* Globals Used:     See PutBits.
*
* Return:           None.
*
* Globals Altered:  See PutBits.
*
* Error Handling:	The global destBufferFull is set to TRUE if the
*					destination buffer count is exhausted.  It is set to OK
*					otherwise.
*
* Algorithms:
*
* Notes:			Calling this functions forces all data that has been
*					compressed to be flushed to the destination buffer.
*
-****************************************************************************/

static void PutEndMark(void)
	{

	/*
	Local Data Declarations:
	*/

	/*
	Body of function:
	*/

	/*----------------------------------------*/
	/* output the end of compressed data mark */
	/*----------------------------------------*/

	PutBitsMac(0x180, 9);

	/*------------------------------------*/
	/* flush any remaining bits to buffer */
	/*------------------------------------*/

	if (sr.sc.nextFreeBit != 8)
		{
		*sr.sc.outputDataHandle++ = sr.sc.outputByte;
		--sr.sc.outputDataCount;

		if (sr.sc.outputDataCount == 0)		/* watch for full output buffer */
			{
			sr.sc.destBufferFull = TRUE;	/* initialized to OK by			*/
			}								/* LZS_InitHistory				*/
		}

	sr.sc.nextFreeBit	= 8;
	sr.sc.outputByte	= 0;

	return;

	}	/* PutEndMark */

/*

+*****************************************************************************
*
* Function Name:	PutCompressedString
*
* Function:			Output a compressed string of data
*
* Arguments:        rawStringOffset	- Offset of compressed string
*					rawStringLength	- Length of compressed string
* Globals Used:     See PutBits.
*
* Return:           None.
*
* Globals Altered:  stringLength	- The current string length is set to 0
*									  at the end of this routine.  This is
*									  used as the indication that there is
*									  no current string.
*
*					See PutBits for complete list of altered globals.
*
* Error Handling:	The global destBufferFull is set to TRUE if the
*					destination buffer count is exhausted.  It is set to OK
*					otherwise.
*
* Algorithms:
*
* Notes:			Read note above about stringLength!
*
-****************************************************************************/

static void	PutCompressedString(
							   ushort	rawStringOffset,
							   ulong	rawStringLength
							   )

	{

	/*
	Local Data Declarations:
	*/


	/*
	Body of function:
	*/

	if (sr.sc.stringOutputStarted == FALSE)
		{
		/*----------------*/
		/* 7 bit offset ? */
		/*----------------*/
		if (rawStringOffset < 128)
			{
			PutBitsMac(0x180 | rawStringOffset, 9);
			}
				/*-----------------*/
		else	/* 11 bit offset ? */
				/*-----------------*/
			{
			PutBitsMac(0x1000 | rawStringOffset, 13);
			}
		}

	/*--------------------------------------------------------------*/
	/* take care of any run length encoded portion of string length */
	/*--------------------------------------------------------------*/
	if (rawStringLength >= MIN_RLE_LENGTH)
		{
		PutBitsMac(RLE_UNIT_LENGTH, BITS_PER_RLE_UNIT);
		rawStringLength  -= RLE_UNIT_LENGTH;
		sr.sc.RLE_Length += RLE_UNIT_LENGTH;
		sr.sc.stringOutputStarted = TRUE;
		}

	else
		{
		/*--------------------------------------------*/
		/* look up the remainder of the string length */
		/*--------------------------------------------*/
		PutBitsMac(
				  lengthTable[(ushort) rawStringLength].pattern,
				  lengthTable[(ushort) rawStringLength].size
				  );

		/*----------------------------*/
		/* no current string any more */
		/*----------------------------*/
		sr.sc.stringOutputStarted = FALSE;
		sr.sc.stringLength = 0;
		sr.sc.RLE_Length   = 0;
		event = ENDED_STRING;
		}
	return;

	}	/* PutCompressedString */

/*

+*****************************************************************************
*
* Function Name:	PerformFlush
*
* Function:         Flush all compressed data & write EOCD mark
*
* Arguments:
* Globals Used:
* Hardware Inputs:
*
* Return:
* Globals Altered:
* Hardware Outputs:
*
* Error Handling:
*
* Algorithms/Structures:
*
* Notes:
*
-****************************************************************************/

static void PerformFlush(ushort	flags)
	{

	/*
	Local Data Declarations:
	*/

	/*
	Body of function:
	*/

	/*--------------------------*/
	/* Flush any remaining data */
	/*--------------------------*/
	if (sr.sc.compressedSomeData == TRUE)
		{
		if (sr.sc.stringLength == 0)
			{
			PutARawByte(sr.sc.currentByte);
			}
		else
			PutCompressedString(sr.sc.matchStringOffset, sr.sc.stringLength);
		}

	PutEndMark();

	if ((flags & LZS_NO_CLEAR) == FALSE)
		{
		/*-----------------------------*/
		/* Invalidate old hash entries */
		/*-----------------------------*/
		if (0 - sr.sc.historyPointer >= HISTORY_SIZE)
			InitHashTable(sp->hashTable, INVALID_FOR_1ST_HALF);

		else if (0x80000000L - sr.sc.historyPointer >= HISTORY_SIZE)
			InitHashTable(sp->hashTable, INVALID_FOR_2ND_HALF);

		sr.sc.historyPointer		+= HISTORY_SIZE;
		sr.sc.historyPointerMinus1	+= HISTORY_SIZE;

		InitCompressVars();
		}
	/*---------------------------------------*/
	/* Forget old data. Reprime w/ next byte */
	/*---------------------------------------*/
	sr.sc.compressedSomeData = FALSE;

	return;

	}	/* PerformFlush */

/*

+*****************************************************************************
*
* Function Name:	LZS_Compress
*
* Function:         compress a buffer full of data
*
* Arguments:        sourceHandle	- pointer to pointer to source buffer
*					destHandle		- pointer to pointer to destination buffer
*					sourceCount		- pointer to size in bytes of data in
*									   source buffer.
*					destCount		- pointer to size in bytes of destination
*									   buffer.
*
* Globals Used:     compressedSomeData
*					currentByte
*					currentBytePair
*					currentStringIndex
*					hashTable
*					c_history
*					historyPointer
*					historyPointerMinus1
*					inputDataCount
*					inputDataHandle
*					matchIndex
*					matchStringOffset
*					outputDataCount
*					outputDataHandle
*					searchCount
*					stringLength
*					stringList
*
*
* Return:           None.
*
* Globals Altered:  bytePairOffset
*					compressedSomeData
*					currentByte
*					currentBytePair
*					currentStringIndex
*					firstMatchOffset
*					hashAddress
*					hashTable
*					c_history
*					historyPointer
*					historyPointerMinus1
*					inputDataCount
*					inputDataHandle
*					matchIndex
*					matchStringOffset
*					outputDataCount
*					outputDataHandle
*					rematchOffset
*					searchCount
*					searchCount
*					stringLength
*					stringList
*
* Error Handling:	Terminates if either source or destination counts are
*					exhausted.
*
* Algorithms:		Hash incoming byte pairs.  Store the current history
*					pointer in the hashTable.  Store offsets to previous hash
*					entries in stringList (hash buckets).  Search stringList
*					for matches to current byte pair.
*
* Pseudo-code:
*
* BEGIN
* {
* if there is data to compress and room to put it
* 	{
* 	initialize data;
* 	get a byte to prime the pump;
* 	while still data to compress and room to put it;
* 		{
* 		advance history and related vars;
* 		see if hash table needs to be refreshed;
* 		get another byte, update history, and string list;
* 		if offset to last hashed string greater than history size;
* 			stringList for current byte pair = 0;
* 		else
* 			{
* 			update string list with current byte pair offset;
* 			if not currently extending a string;
* 				search string list for current byte pair match;
* 			}
* 		update hash table with current byte pair information;
* 		if not currently extending a string
* 			if current byte pair match found
* 				start a new string;
* 			else
* 				output a raw byte;
* 		else
* 			{
* 			if current byte pair definitely not contained in history
* 				output current string;
* 			else
* 				{
* 				if string has reached maximum length
* 					output current string;
* 
* 				else if current match continues
* 					extend the current string;
* 
* 				else if the current string is long enough
* 					output current string;
*
* 				else
* 					{
* 					search for a longer match;
* 					if a longer match found
* 						extend the current string;
* 					else
* 						output current string;
* 					}
*
* 				if current string was output
* 					{
* 					search string list for current byte pair match;
* 					if match for current byte pair found
* 						start new string;
* 					}
* 				}
* 			}
* 		}
* 	}
* 	if the output buffer is full
* 		{
* 		flush any remaining data;
* 		put an end mark
* 		}
* }
* END
* 
* Notes:
*
-****************************************************************************/

ushort LZS_FAR	LZS_Compress(
					uchar	LZS_HANDLE	 sourceHandle,
					uchar	LZS_HANDLE	 destHandle,
					ulong	LZS_FAR		*sourceCount,
					ulong	LZS_FAR		*destCount,
					void	LZS_FAR		*scratch,
					ushort				 flags,
					ushort				 performance
					)
	{

	/*--------------------------*/
	/* Local Data Declarations: */
	/*--------------------------*/

	register	ushort	ii;
	register	boolean	foundLongerMatch;
				ushort	testIndex;
				ushort	returnValue = 0;

	/*--------------------------*/
	/* Body of function:        */
	/*--------------------------*/

#if LZS_TARGET == LZS_INTEL_86

	/*--------------------------------------------------------------------*/
	/* Validate source and destination counts on Intel X86 platforms.     */
	/* This is necessary because far pointers are used not huge pointers. */
	/*--------------------------------------------------------------------*/

	if ((*sourceCount > 0xFFFF) || (*destCount > 0xFFFF))
		{
		return (LZS_INVALID);
		}
#endif

	/*---------------------------------------*/
	/* initialize global pointers and counts */
	/*---------------------------------------*/
#if LZS_DEBUG
			event = 0xFFFF;
#endif

	/*----------------------------------------------------------------*/
	/* Make local copy of most vars to avoid speed hit of indirection */
	/*----------------------------------------------------------------*/

	sp = (scratchRamType LZS_FAR *) scratch;
	sr.sc = sp->sc ;

	sr.sc.inputDataHandle	= *sourceHandle;
	sr.sc.inputDataCount	= *sourceCount;
	sr.sc.outputDataHandle	= *destHandle;
	sr.sc.outputDataCount	= *destCount;
	sr.sc.destBufferFull	= FALSE;
											 /*-----------------------------*/
	performanceMode = LZS_PERF_MASK & flags; /* Extract performance mode.	*/
	if (performanceMode > LZS_PERF_MODE_2)	 /* Validate it.				*/
		{									 /*								*/
		performanceMode = LZS_PERF_MODE_2;	 /* If invalid use mode 2.		*/
		}									 /*								*/
	performanceMode >>= 3;					 /* Convert to array index for	*/
											 /* use with okToHash array.	*/
											 /*-----------------------------*/
	if (*destCount <= LZS_DEST_MIN)
										/*---------------------------------*/
		returnValue = LZS_INVALID;	/* Don't do anything just return ! */
										/*---------------------------------*/
										  /*-------------------------------*/
	else if (sr.sc.inputDataCount != 0)	  /* and *destCount > LZS_DEST_MIN */
										  /*-------------------------------*/
		{
		/*------------------------------------------------*/
		/* Provide for a cushion at the	end of the buffer */
		/*------------------------------------------------*/

		sr.sc.outputDataCount -= LZS_DEST_MIN;

		/*-------------------------*/
		/* if very first time thru */
		/*-------------------------*/
		if (sr.sc.compressedSomeData == FALSE)
			{
			/*-----------*/
			/*  GetAByte */
			/*-----------*/
			sr.sc.currentByte = *sr.sc.inputDataHandle++;
			--sr.sc.inputDataCount;
			/*----------------------------------------*/
			/* increment history pointer and indicies */
			/*----------------------------------------*/
			++sr.sc.historyPointer;
			++sr.sc.historyPointerMinus1;
			sr.sc.historyIndexMinus1 = (ushort) sr.sc.historyPointerMinus1
									  & HISTORY_MASK;
			/*------------------------------------*/
			/* update history and currentBytePair */
			/*------------------------------------*/
			sp->c_history [ (ushort) 
				sr.sc.historyPointer & HISTORY_MASK ] = sr.sc.currentByte;
			sr.sc.currentBytePair = (sr.sc.currentBytePair << 8)
								  +  sr.sc.currentByte;
			sr.sc.compressedSomeData = TRUE;
			}
		/*-----------------------------*/
		/* while buffers not exhausted */
		/*-----------------------------*/
		while ((sr.sc.inputDataCount != 0)
		&&    (sr.sc.destBufferFull == FALSE))
			{
			/*----------------------------------------*/
			/* increment history pointer and indicies */
			/*----------------------------------------*/
			++sr.sc.historyPointer;
			++sr.sc.historyPointerMinus1;
			sr.sc.historyIndexMinus1 = (ushort) sr.sc.historyPointerMinus1
									  & HISTORY_MASK;
			/*---------------------------------------*/
			/* Invalidate old pointers in hash table */
			/*---------------------------------------*/
			if (HashTableNeedsRefresh())
				RefreshOldHashEntries();
			/*-------------------------------------------------*/
			/*  GetAByte, update history and current byte pair */
			/*-------------------------------------------------*/
			sp->c_history [ (ushort) sr.sc.historyPointer & HISTORY_MASK ]
				= sr.sc.currentByte
				= *sr.sc.inputDataHandle++;

			sr.sc.currentBytePair = (sr.sc.currentBytePair << 8)
								  + sr.sc.currentByte;
			--sr.sc.inputDataCount;
			/*-----------------------------------*/
			/* update hash table and string list */
			/*-----------------------------------*/
			sr.sc.hashAddress = sp->hashTable
							  + GetHashFor(sr.sc.currentBytePair);

			/*--------------------------------------------------------*/
			/* has a previous byte pair been found with the same hash */
			/*--------------------------------------------------------*/
			if ((sr.sc.bytePairOffset = sr.sc.historyPointerMinus1
									  - *sr.sc.hashAddress)
									  > HISTORY_SIZE - 2)
				{
				/*----------------------------------*/
				/* no, mark the string list as empty */
				/*----------------------------------*/
				sp->stringList[sr.sc.historyIndexMinus1] = 0;
				if (sr.sc.stringLength != 0)
					{
					/*---------------------------------*/
					/* if currently extending a string */
					/*---------------------------------*/
					PutCompressedString(
									   sr.sc.matchStringOffset,
									   sr.sc.stringLength
									   );
					}
				else
					{
					PutARawByte(sr.sc.currentBytePair >> 8);
					}
				}
					/*----------------------------------*/
			else	/* yes, a previous byte pair exists */
					/*----------------------------------*/
				{
				sp->stringList[sr.sc.historyIndexMinus1] =
					(ushort) sr.sc.bytePairOffset;

				/*------------------------------*/
				/* currently extending a string */
				/*------------------------------*/
				if (sr.sc.stringLength != 0)
					{
					if (CurrentStringExtends()
					&& ((sr.sc.stringLength + sr.sc.RLE_Length)
						 < (ulong) MAX_STRING_LENGTH))
						{
						++sr.sc.stringLength;
						event = EXTENDED_STRING;
						if (sr.sc.stringOutputStarted)
							{
							/*---------------------------------------------*/
							/* if string long enough to output next nibble */
							/*---------------------------------------------*/
							if (sr.sc.stringLength >= MIN_RLE_LENGTH)
								{
								/*--------------------*/
								/* output next nibble */
								/*--------------------*/
								PutBitsMac(
										  RLE_UNIT_LENGTH,
										  BITS_PER_RLE_UNIT
										  );
								sr.sc.stringLength -= RLE_UNIT_LENGTH;
								sr.sc.RLE_Length   += RLE_UNIT_LENGTH;
								}
							}
						/*-----------------------------------------------*/
						/* else if string is long enough to start output */
						/*-----------------------------------------------*/
						else if (sr.sc.stringLength >= MIN_RLE_LENGTH)
							{
							/*---------------------*/
							/* start string output */
							/*---------------------*/
							if (sr.sc.matchStringOffset < 128)
								{
								/*--------------*/
								/* 7 bit offset */
								/*--------------*/
								PutBitsMac(
										  0x180 | sr.sc.matchStringOffset,
										  9
										  );
								}
							else
								{
								/*---------------*/
								/* 11 bit offset */
								/*---------------*/
								PutBitsMac(
										  0x1000 | sr.sc.matchStringOffset,
										  13
										  );
								}
							/*---------------------------*/
							/* start Run Length Encoding */
							/*---------------------------*/
							PutBitsMac(RLE_UNIT_LENGTH, BITS_PER_RLE_UNIT);
							sr.sc.stringLength		 -= RLE_UNIT_LENGTH;
							sr.sc.RLE_Length	     += RLE_UNIT_LENGTH;
							sr.sc.stringOutputStarted = TRUE;
							}
						}
					else if (sr.sc.stringOutputStarted)
						{
						/*--------------------------------------------*/
						/* finish string output                       */
						/* look up the remainder of the string length */
						/*--------------------------------------------*/
						PutBitsMac(
						  lengthTable[(ushort) sr.sc.stringLength].pattern,
						  lengthTable[(ushort) sr.sc.stringLength].size
								  );
						sr.sc.stringLength = 0;
						sr.sc.RLE_Length	= 0;
						sr.sc.stringOutputStarted = FALSE;
						event = ENDED_STRING;
						}
					else if (sr.sc.stringLength >= REMATCH_SIZE_LIMIT)
						{
						/*---------------------------------------*/
						/* else if current string is long enough */
						/*---------------------------------------*/
						PutCompressedString( sr.sc.matchStringOffset,
											 sr.sc.stringLength);
						}
					else
						{
						/*---------------------------*/
						/* search for a longer match */
						/*---------------------------*/
						foundLongerMatch = FALSE;
						sr.sc.rematchOffset = sr.sc.matchStringOffset;

						while ((sp->stringList[sr.sc.matchIndex] != 0)
						&& (foundLongerMatch == FALSE)
						&& (sr.sc.searchCount < performance)
						&& (sr.sc.rematchOffset < (ushort) (HISTORY_SIZE -
											 sr.sc.stringLength)))
						/*--------------------------------------------------*/
						/* RLE_Length does not need to be include in the	*/
						/* final condition above since rematching is not	*/
						/* allowed once Run Length Encoded string output	*/
						/* has begun. This also applies to next "if" below.	*/
						/*--------------------------------------------------*/
							{
							sr.sc.rematchOffset +=
								sp->stringList[sr.sc.matchIndex];
							++sr.sc.searchCount;
							if (sr.sc.rematchOffset < (ushort) (HISTORY_SIZE
												    - sr.sc.stringLength))
								{
								sr.sc.matchIndex = (sr.sc.matchIndex
										   - sp->stringList[sr.sc.matchIndex])
										   & HISTORY_MASK;

								/*-----------------*/
								/* compare strings */
								/*-----------------*/
								if (sp->c_history[ sr.sc.matchIndex ]
								== sp->c_history[ sr.sc.currentStringIndex ])
									{
									foundLongerMatch = TRUE;
								/*------------------------------------------*/
								/* If the first bytes match then the hash	*/
								/* guarantees that the second bytes match	*/
								/* as well so we can skip ahead to the 3rd	*/
								/* byte for the rest of the comparison.		*/
								/* If the hash were unique the comparison	*/
								/* can start immediately with the 3rd bytes.*/
								/*------------------------------------------*/
									for (
										ii = 2,
										testIndex = (sr.sc.matchIndex+2)
												  & HISTORY_MASK;

										ii <= (ushort) sr.sc.stringLength;

										ii++,
										testIndex = ++testIndex & HISTORY_MASK
										)
										{
										if (sp->c_history[testIndex] !=
											sp->c_history[(sr.sc.currentStringIndex+ii)
																& HISTORY_MASK ])
											{
											foundLongerMatch = FALSE;
											break;
											}
										}
									}
								}
							}
						if (foundLongerMatch)
							{
							sr.sc.matchStringOffset = sr.sc.rematchOffset;
							++sr.sc.stringLength;
							event = FOUND_LONGER_MATCH;
							}
						else
							{
							/*---------------*/
							/* end of string */
							/*---------------*/
							PutCompressedString( sr.sc.matchStringOffset,
												 sr.sc.stringLength );
							}
						}
					}
				/*------------------------------------------------------*/
				/* If the hash is non-unique then the string list must 	*/
				/* be searched here rather than assuming that the first	*/
				/* item in the string list is a match.					*/
				/*------------------------------------------------------*/
				else
					{
					/*------------------------------------*/
					/* not currently extending a string */
					/*------------------------------------*/
					sr.sc.firstMatchOffset = (ushort) sr.sc.bytePairOffset;
					sr.sc.searchCount = 0;

					do
						{
						sr.sc.matchIndex = (ushort)((sr.sc.historyPointerMinus1
										 - sr.sc.firstMatchOffset)
										 & HISTORY_MASK);

						if (sp->c_history[sr.sc.matchIndex]
							 == (uchar) (sr.sc.currentBytePair >> 8))
							{
							/*----------------------*/
							/* set stringLength = 2 */
							/*----------------------*/
							StartNewString();
							break;
							}
						else
							{
							sr.sc.firstMatchOffset +=
								sp->stringList[sr.sc.matchIndex];
							++sr.sc.searchCount;
							}
						}
					while ((sp->stringList[sr.sc.matchIndex] != 0)
						&& (sr.sc.searchCount < performance)
						&& (sr.sc.firstMatchOffset < HISTORY_SIZE - 2));

					if (sr.sc.stringLength == 0)
						{
						PutARawByte(sr.sc.currentBytePair >> 8);
						}
					}
				}
			
#if LZS_DEBUG
			if (event == 0xFFFF)
				{
				printf("\n\nInvalid event value!!!\n");
				exit(3);
				}
#endif

			if (okToHash[event][performanceMode])
				{
				*sr.sc.hashAddress = sr.sc.historyPointerMinus1;
				}
#if LZS_DEBUG
			event = 0xFFFF;
#endif
			}
		/*---------------------------*/
		/* Process Termination Flags */
		/*---------------------------*/
		if (sr.sc.inputDataCount == 0)
			{
			returnValue = LZS_SOURCE_EXHAUSTED;
			if (flags & LZS_SOURCE_FLUSH)
				{
				PerformFlush(flags);
				returnValue |= LZS_FLUSHED;
				}
			}
		if ((sr.sc.outputDataCount == 0) || (sr.sc.destBufferFull))
			{
			if ( !(returnValue & LZS_SOURCE_EXHAUSTED) )
				{
				returnValue = LZS_DEST_EXHAUSTED;
				if (flags & LZS_DEST_FLUSH)
					{
					PerformFlush(flags);
					returnValue |= LZS_FLUSHED;
					}
				}
			else
				{
				returnValue |= LZS_SOURCE_DEST_EXHAUSTED;
				if ((!(flags & LZS_SOURCE_FLUSH)) && (flags & LZS_DEST_FLUSH))
					{
					PerformFlush(flags);
					returnValue |= LZS_FLUSHED;
					}
				}
			}
		/*-----------------------------------------------*/
		/* Remove cushion from end of destination buffer */
		/*-----------------------------------------------*/

		sr.sc.outputDataCount += LZS_DEST_MIN;
		}
			/*----------------------------------------------------------*/
	else	/* *destCount > LZS_DEST_MIN and sr.sc.inputDataCount == 0	*/
			/*----------------------------------------------------------*/
		{
		returnValue = LZS_SOURCE_EXHAUSTED;
		if (flags & LZS_SOURCE_FLUSH)
			{
			PerformFlush(flags);
			returnValue |= LZS_FLUSHED;
			}
		}

	/*----------------------------------------*/
	/* Update external vars with local values */
	/*----------------------------------------*/

	sp->sc = sr.sc;
	*sourceHandle	= sr.sc.inputDataHandle ;
	*sourceCount	= sr.sc.inputDataCount ;
	*destHandle		= sr.sc.outputDataHandle ;
	*destCount		= sr.sc.outputDataCount ;

	return (returnValue);
	}

	/*---------------------*/
	/* End of LZS_Compress */
	/*---------------------*/

/*

+*****************************************************************************
*
* Function Name:	InitDecompressVars
*
* Function:         Initialize decompression variables
*
* Arguments:        None.
* Globals Used:     None.
*
* Return:           None.
*
* Globals Altered:  sr.sd.historyIdx
*					sr.sd.bitAlign
*					sr.sd.getBitsCount
*					sr.sd.offset
*					sr.sd.getBitsResult
*					sr.sd.lastByteOfCompressedInput
*					sr.sd.sourceBits
*					sr.sd.length
*					sr.sd.decompressingAString
*					sr.sd.decompressState
*					sr.sd.decompressInitialized
*
* Error Handling:	None.
*
* Algorithms:
*
* Notes:
*
-****************************************************************************/

static void InitDecompressVars(void)
	{

	/*
	Local Data Declarations:
	*/

	/*
	Body of function:
	*/

	sr.sd.historyIdx &= HISTORY_MASK; /* make sure history index is valid */
	sr.sd.bitAlign					= 
	sr.sd.getBitsCount				= 
	sr.sd.offset					= 0;	/* Offset into history */
	sr.sd.getBitsResult				= 
	sr.sd.lastByteOfCompressedInput	=
	sr.sd.sourceBits				= 0;
	sr.sd.length					= 0;	/* Length of pattern in history */
	sr.sd.decompressingAString		= FALSE;
	sr.sd.decompressState			= RAW_OR_COMPRESSED;
	sr.sd.decompressInitialized		= TRUE;

	return;
	}	/* InitDecompressVars */

/*

+*****************************************************************************
*
* Function Name:	GetABit
*
* Description:		Get one bit from input file
*
* Arguments:		None.
*
* Globals Used:		bitAlign
*					inputDataCount
*					inputDataHandle
*					lastByteOfCompressedInput
*
* Return:			Returns an unsigned short containing the right adjusted
*					number of bits requested.
*
* Globals Altered:	bitAlign
*					inputDataCount
*					inputDataHandle
*					lastByteOfCompressedInput
*
* Error Handling:	Returns 0 if the input buffer has been exhausted.
*					(i.e. inputDataCount == 0)
*
* Algorithms:
*
* Notes:			bitAlign keeps track of last bit used from input stream.
*					lastByteOfCompressedInput contains any unused bits from
*					the last byte read from the input buffer.
*
-****************************************************************************/

static boolean GetABit(void)
	{
	if ((sr.sd.inputDataCount == 0) && (sr.sd.bitAlign == 0))
		sr.sd.getBitsResult = NOT_OK;
	else
		{
		sr.sd.getBitsResult = OK;
		/*--------------------------------------------------*/
		/* Do we need to read another byte from input file? */
		/*--------------------------------------------------*/
		if (sr.sd.bitAlign == 0)
			{
			/* Read the next byte from input file */
			sr.sd.lastByteOfCompressedInput = *sr.sd.inputDataHandle++;
			--sr.sd.inputDataCount;
			sr.sd.bitAlign = 7;

			/*--------------------------------------*/
			/* Store the result in the return value */
			/*--------------------------------------*/
			sr.sd.sourceBits
				= ((char) sr.sd.lastByteOfCompressedInput < 0) ? 1 : 0;


			/*--------------------------------------------------------*/
			/* Clear all bits that have already been used from 'byte' */
			/*--------------------------------------------------------*/
			sr.sd.lastByteOfCompressedInput &= ((uchar) 0x7F);
			}
		else
			{
			/*--------------------------------------*/
			/* Store the result in the return value */
			/*--------------------------------------*/
			sr.sd.sourceBits =
				(sr.sd.lastByteOfCompressedInput >> (sr.sd.bitAlign - 1));

			/*---------------------------------------------------------*/
			/* Update # of bits not yet used from the current byte     */
			/*---------------------------------------------------------*/
			--sr.sd.bitAlign;

			/*--------------------------------------------------------*/
			/* Clear all bits that have already been used from 'byte' */
			/*--------------------------------------------------------*/
			sr.sd.lastByteOfCompressedInput &=
				((uchar)0xFF >> (8 - sr.sd.bitAlign));
			}
		}

	return (sr.sd.getBitsResult);

	}	/* GetABit */

/*

+*****************************************************************************
*
* Function Name:	GetBits
*
* Description:		Get n bits from input file
*
* Arguments:		Passed via globals listed below.
*
* Globals Used:		bitAlign
*					getBitsCount
*					inputDataCount
*					inputDataHandle
*					lastByteOfCompressedInput
*
* Return:			Returns an unsigned short containing the right adjusted
*					number of bits requested.
*
* Globals Altered:	bitAlign
*					inputDataCount
*					inputDataHandle
*					lastByteOfCompressedInput
*
* Error Handling:	Returns 0 if the input buffer has been exhausted.
*					(i.e. inputDataCount == 0)
*
* Algorithms:
*
* Notes:			bitAlign keeps track of last bit used from input stream.
*					lastByteOfCompressedInput contains any unused bits from
*					the last byte read from the input buffer.
*
-****************************************************************************/

static boolean GetBits(void)
	{
	/*--------------------*/
	/* Local Declarations */
	/*--------------------*/

	ushort	temp;
	sshort	x;

	/*-------------------*/
	/* Body of Function */
	/*-------------------*/

	/*--------------------------------------------------------------------*/
	/* Clear the return value to zero if previous operation not suspended */
	/*--------------------------------------------------------------------*/

	if (sr.sd.getBitsResult == OK)
		sr.sd.sourceBits = 0;
	else
		sr.sd.getBitsResult = OK;
		
	/*-------------------------*/
	/* Get 'getBitsCount' bits */
	/*-------------------------*/
	while ((sr.sd.getBitsCount > 0) && (sr.sd.getBitsResult == OK))
		{
		if ((sr.sd.inputDataCount == 0) && (sr.sd.bitAlign == 0))
			sr.sd.getBitsResult = NOT_OK;
		else
			{
			/*--------------------------------------------------*/
			/* Do we need to read another byte from input file? */
			/*--------------------------------------------------*/
			if (sr.sd.bitAlign == 0)
				{
				/* Read the next byte from input file */
				sr.sd.lastByteOfCompressedInput = *sr.sd.inputDataHandle++;
				--sr.sd.inputDataCount;
				sr.sd.bitAlign = 8;
				}

			/*-----------------------------------*/
			/* Arrange the bits properly in temp */
			/*-----------------------------------*/
			temp = sr.sd.lastByteOfCompressedInput;
			if ((x = sr.sd.getBitsCount - sr.sd.bitAlign) > 0)
				temp <<= x;
			else
				temp >>= -x;

			/*--------------------------------------*/
			/* Store the result in the return value */
			/*--------------------------------------*/
			sr.sd.sourceBits |= temp;

			/*---------------------------------------------------------*/
			/* Update count of # of bits yet to get, and also update the
				 value of # of bits not yet used from the current byte */
			/*---------------------------------------------------------*/
			x = min(min(8, sr.sd.getBitsCount), sr.sd.bitAlign);
			sr.sd.getBitsCount -= x;
			sr.sd.bitAlign -= x;

			/*--------------------------------------------------------*/
			/* Clear all bits that have already been used from 'byte' */
			/*--------------------------------------------------------*/
			sr.sd.lastByteOfCompressedInput &=
				((uchar)0xFF >> (8 - sr.sd.bitAlign));
			}
		}

	return (sr.sd.getBitsResult);

	}	/* GetBits */
/*

+*****************************************************************************
*
* Function Name:	PutOutput
*
* Description:		Output byte to decompressed data buffer & update history
*
* Arguments:		c	- character to write to decompressed data buffer
*
* Globals Used:		historyIdx
*					outputDataCount
*					outputDataHandle
*
* Return:			None.
*
* Globals Altered:	d_history
*					historyIdx
*					outputDataCount
*					outputDataHandle
*
* Error Handling:	Writes no data if the decompressed buffer data count
*					has been exhausted
*
* Algorithms:
*
* Notes:
*
-****************************************************************************/

static void PutOutput(uchar c)
	{
	/*--------------------*/
	/* Local Declarations */
	/*--------------------*/

	/*------------------*/
	/* Body of Function */
	/*------------------*/

	/*-------------------------------------*/
	/* put the byte into the output buffer */
	/*-------------------------------------*/
	if (sr.sd.outputDataCount != 0)
		{
		*sr.sd.outputDataHandle++ = c;
		--sr.sd.outputDataCount;

		/*----------------*/
		/* Update history */
		/*----------------*/
		sp->d_history[sr.sd.historyIdx++] = (uchar)c;
		sr.sd.historyIdx &= HISTORY_MASK;
		}
	}	/* PutOutput */
/*

+*****************************************************************************
*
* Function Name:	LZS_Decompress
*
* Description:		Decompress a buffer full of data
*
* Arguments:        sourceHandle	- pointer to pointer to source buffer
*					destHandle		- pointer to pointer to destination buffer
*					sourceCount		- pointer to size in bytes of data in
*									   source buffer.
*					destCount		- pointer to size in bytes of destination
*									   buffer.
*
* Globals Used:		decompressingAString
*					decompressInitialized
*					d_history
*					historyIdx
*					lastByteOfComrpessedInput
*					length
*					offset
*
* Return:			None.
*
* Globals Altered:	decompressingAString
*					d_history
*					inputDataCount
*					inputDataHandle
*					length
*					offset
*					outputDataCount
*					outputDataHandle
*
* Error Handling:	Terminates if either source or destination counts are
*					exhausted.
*
* Algorithms:
*
* Notes:			Parses compressed data as specified by QIC-122 standard
*
-****************************************************************************/

ushort LZS_FAR	LZS_Decompress(
							  uchar		LZS_HANDLE 	 sourceHandle,
							  uchar		LZS_HANDLE 	 destHandle,
							  ulong		LZS_FAR		*sourceCount,
							  ulong		LZS_FAR		*destCount,
							  void		LZS_FAR		*scratch,
							  ushort			   	 flags
							  )
{
	/*--------------------*/
	/* Local Declarations */
	/*--------------------*/

	boolean		hitEndOfCompression	= FALSE;
	ushort		returnValue = 0;

	/*------------------*/
	/* Body of Function */
	/*------------------*/

#if LZS_TARGET == LZS_INTEL_86

	/*--------------------------------------------------------------------*/
	/* Validate source and destination counts on Intel X86 platforms.     */
	/* This is necessary because far pointers are used not huge pointers. */
	/*--------------------------------------------------------------------*/

	if ((*sourceCount > 0xFFFF) || (*destCount > 0xFFFF))
		{
		return (LZS_INVALID);
		}
#endif

	/*---------------------------------------*/
	/* initialize global pointers and counts */
	/*---------------------------------------*/

	sp = (scratchRamType LZS_FAR *) scratch;
	sr.sd = sp->sd ;
	sr.sd.inputDataHandle	= *sourceHandle;
	sr.sd.inputDataCount  	= *sourceCount;
	sr.sd.outputDataHandle	= *destHandle;
	sr.sd.outputDataCount	= *destCount;

	if (flags & LZS_RESET)
		{
		InitDecompressVars();
		}

	if ((sr.sd.inputDataCount != 0) && (sr.sd.outputDataCount != 0))
		{
		/*---------------------------*/
		/* Start processing the data */
		/*---------------------------*/
		while ((sr.sd.outputDataCount != 0)
			&& ((sr.sd.inputDataCount != 0) || (sr.sd.bitAlign != 0))
			&& (hitEndOfCompression == FALSE))
			{
			if (sr.sd.decompressingAString)
				{
				while ((sr.sd.outputDataCount != 0) && (sr.sd.length > 0))
						{
						sr.sd.offset &= HISTORY_MASK;
						PutOutput(sp->d_history[sr.sd.offset++]);
						--sr.sd.length;
						}
				if (sr.sd.length == 0)
					sr.sd.decompressingAString = FALSE;
					sr.sd.decompressState = RAW_OR_COMPRESSED;
				}

			/*--------------------------------------------*/
			/* Check if next pattern is raw or compressed */
			/*--------------------------------------------*/
			else
				{
				/*----------------------*/
				/* if (GetBits(1) == 0) */
				/*----------------------*/
switch (sr.sd.decompressState)
	{
case RAW_OR_COMPRESSED:

				GetABit();
				if (sr.sd.sourceBits == 0)
					{
					/*---------------------*/
					/* Output the raw byte */
					/*---------------------*/
					/*------------------------*/
					/* PutOutput(GetBits(8)); */
					/*------------------------*/
					sr.sd.getBitsCount = 8;
					sr.sd.decompressState = PUTTING_RAW_BYTE;

case PUTTING_RAW_BYTE:
					GetBits();
					if (sr.sd.getBitsResult == OK)
						{
						PutOutput((uchar) sr.sd.sourceBits);
						sr.sd.decompressState = RAW_OR_COMPRESSED;
						}
					}
				else
					{
					/*------------------------*/
					/* Pattern is compressed  */
					/* Get 7 or 11-bit offset */
					/*------------------------*/

					/* sr.sd.offset = GetBits(GetBits(1) ? 7 : 11); */

					sr.sd.decompressState = GETTING_OFFSET_TYPE;

case GETTING_OFFSET_TYPE:
					GetABit();
					if (sr.sd.getBitsResult == OK)
						{
						sr.sd.getBitsCount = sr.sd.sourceBits ? 7 : 11;
						sr.sd.decompressState = GETTING_OFFSET;
case GETTING_OFFSET:
						GetBits();
						if (sr.sd.getBitsResult == OK)
							{
							sr.sd.offset = sr.sd.sourceBits;

							/*-------------------------------------*/
							/* Check for End-of-Compression Marker */
							/*-------------------------------------*/
							if (sr.sd.offset == 0)
								hitEndOfCompression = TRUE;
							else
								{
								/*-----------------------------------*/
								/* its a string						 */
								/* convert offset to a history index */
								/*-----------------------------------*/
								sr.sd.offset = sr.sd.historyIdx
											 - sr.sd.offset;
								/*----------------------------------*/
								/* Get length of compressed pattern */
								/*----------------------------------*/
								/* sr.sd.length = 2 + GetBits(2); */
								sr.sd.getBitsCount = 2;
								sr.sd.decompressState = LENGTH_DIBIT_1;
case LENGTH_DIBIT_1:
								GetBits();
								if (sr.sd.getBitsResult == OK)
									{
									sr.sd.length = 2 + sr.sd.sourceBits;
									if (sr.sd.length == 5)
										{
										/* sr.sd.length += GetBits(2); */
										sr.sd.getBitsCount = 2;
										sr.sd.decompressState =LENGTH_DIBIT_2;
case LENGTH_DIBIT_2:
										GetBits();
										if (sr.sd.getBitsResult == OK)
											{
											sr.sd.length += sr.sd.sourceBits;
											if (sr.sd.length == 8)
												{
												/*--------------------------*/
												/* get next 4 bits of length*/
												/*--------------------------*/
												sr.sd.getBitsCount = 4;
												sr.sd.decompressState = LENGTH_NIBBLE_2;
case LENGTH_NIBBLE_2:
												GetBits();
												if (sr.sd.getBitsResult ==OK)
													{
													sr.sd.length += sr.sd.sourceBits;
													if (sr.sd.length == MIN_RLE_LENGTH)
														{
														do
															{
case PUT_RLE_DATA:
															while ((sr.sd.outputDataCount != 0)
															&&	  (sr.sd.length > 0))
																{
																sr.sd.offset &= HISTORY_MASK;
																PutOutput(sp->d_history[sr.sd.offset++]);
																--sr.sd.length;
																}
															if (sr.sd.outputDataCount == 0)
																{
																sr.sd.decompressState = PUT_RLE_DATA;
																break;
																}
															else
																{
																/*-----------------------------*/
 																/* GetBits(BITS_PER_RLE_UNIT); */
																/*-----------------------------*/
																sr.sd.getBitsCount = BITS_PER_RLE_UNIT;
																sr.sd.decompressState = RLE_BITS;
case RLE_BITS:
																GetBits();
																if (sr.sd.getBitsResult ==OK)
																	sr.sd.length += sr.sd.sourceBits;
																else
																	break;
																}
															} while (sr.sd.sourceBits == RLE_UNIT_LENGTH);
														}
													}
												}
											}
										}
									}
								if ((sr.sd.getBitsResult == OK)
								&& (sr.sd.decompressState != PUT_RLE_DATA))
									{
									/*-------------------------*/
									/* Output from the history */
									/*-------------------------*/
									sr.sd.decompressingAString = TRUE;
									}
								}
							}
						}
					}
	} /*----- end of switch statement ---------*/
				}
			}
		/*------------------------------------------------*/
		/* if ran out of source while outputting a string */
		/*------------------------------------------------*/
		if (sr.sd.decompressingAString)
			{
			while ((sr.sd.outputDataCount != 0) && (sr.sd.length > 0))
					{
					sr.sd.offset &= HISTORY_MASK;
					PutOutput(sp->d_history[sr.sd.offset++]);
					--sr.sd.length;
					}
			if (sr.sd.length == 0)
				sr.sd.decompressingAString = FALSE;
				sr.sd.decompressState = RAW_OR_COMPRESSED;
			}
		}

	/*------------------------*/
	/* Construct return value */
	/*------------------------*/

	if (hitEndOfCompression)
		{
		InitDecompressVars();
		returnValue |= LZS_END_MARKER;
		}

	if (sr.sd.inputDataCount == 0) /* Note bitAlign != 0 not distinguished! */
		{
		returnValue |= LZS_SOURCE_EXHAUSTED;
		}

	if (sr.sd.outputDataCount == 0)
		{
		returnValue |= LZS_DEST_EXHAUSTED;
		}

	sp->sd = sr.sd ;
	*sourceHandle	= sr.sd.inputDataHandle ;
	*sourceCount	= sr.sd.inputDataCount ;
	*destHandle		= sr.sd.outputDataHandle ;
	*destCount		= sr.sd.outputDataCount ;

	return (returnValue);

	}	/* LZS_Decompress */

/****************************************************************************
*							End - of - File									*
****************************************************************************/

