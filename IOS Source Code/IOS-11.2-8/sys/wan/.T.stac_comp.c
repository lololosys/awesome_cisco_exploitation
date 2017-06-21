
stac_comp.c,2440
#define		LZS_DEBUG	130,4534
#define	HISTORY_SIZE	152,5071
#define	HISTORY_MASK	153,5099
#define	MAX_STRING_LENGTH	155,5142
#define	MIN_RLE_LENGTH	156,5181
#define	BITS_PER_RLE_UNIT	157,5252
#define	RLE_UNIT_LENGTH	158,5322
#define	HASH_TABLE_SIZE	166,5514
#define	BOUNDARY_BETWEEN_HALVES	167,5545
#define	INVALID_FOR_1ST_HALF	168,5621
#define	INVALID_FOR_2ND_HALF	169,5691
#define	SEARCH_COUNT_LIMIT	203,6912
#define	REMATCH_SIZE_LIMIT	204,6983
typedef	unsigned	char	uchar;uchar221,7372
typedef	unsigned	int		uint;uint222,7416
typedef	unsigned	short	ushort;ushort223,7459
typedef	unsigned	long	ulong;ulong224,7505
typedef	signed		char	schar;schar226,7550
typedef	signed		int		sint;sint227,7593
typedef	signed		short	sshort;sshort228,7635
typedef	signed		long	slong;slong229,7680
#define		boolean	236,7812
#define		TRUE	237,7837
#define		FALSE	238,7854
#define		OK	239,7872
#define		NOT_OK	240,7891
	} CscratchRamType;CscratchRamType323,10620
		DscratchRamType;DscratchRamType352,11747
		scratchRamType scratchRamType374,12329
typedef	struct lengthTableEntryStructlengthTableEntryStruct388,12620
		lengthTableEntryType;lengthTableEntryType393,12695
#define min(min410,13000
#define GetHashFor(GetHashFor455,14361
#define PutBitsMac(PutBitsMac491,15299
#define	PutARawByte(PutARawByte522,16030
#define	StartNewString(StartNewString552,16678
#define HashTableNeedsRefresh(HashTableNeedsRefresh584,17531
#define CurrentStringExtends(CurrentStringExtends615,18229
		PUT_RAW_BYTE,685,20389
		STARTED_STRING,686,20405
		EXTENDED_STRING,687,20423
		FOUND_LONGER_MATCH,688,20442
		ENDED_STRING689,20464
	RAW_OR_COMPRESSED,699,20542
	PUTTING_RAW_BYTE,700,20562
	GETTING_OFFSET_TYPE,701,20581
	GETTING_OFFSET,702,20603
	LENGTH_DIBIT_1,703,20620
	LENGTH_DIBIT_2,704,20637
	LENGTH_NIBBLE_2,705,20654
	PUT_RLE_DATA,706,20672
	RLE_BITS707,20687
static void InitCompressVars(777,22077
void LZS_FAR	LZS_InitHistory(853,23329
static void InitHashTable923,24858
static void RefreshOldHashEntries(983,26323
static void	PutBits(1065,28639
static void PutEndMark(1140,30292
static void	PutCompressedString(1210,31955
static void PerformFlush(1300,33863
ushort LZS_FAR	LZS_Compress(1491,38380
static void InitDecompressVars(1998,53970
static boolean GetABit(2058,55423
static boolean GetBits(2147,58117
static void PutOutput(2252,61071
ushort LZS_FAR	LZS_Decompress(2320,62737
