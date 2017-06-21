
macip_util.h,2694
#define	__MACIP_UTIL_H__68,2364
#define NILVAL 70,2390
#define NIL(NIL71,2409
#define NILSTR 72,2447
#define SUCCESS 75,2504
typedef ulong		sys_addr;sys_addr77,2523
typedef sys_addr	*sysptr;sysptr78,2548
#define NILSYSADDR 80,2575
typedef ushort		*wordptr;wordptr82,2598
typedef uchar		*byteptr;byteptr83,2624
#define NILPTR 85,2650
typedef int (*PFI)PFI88,2748
#define NILPFI 89,2773
typedef void (*PFV)PFV93,2890
typedef void (*PFV)(struct server_rec_ server_rec_93,2890
#define NILPFV 94,2933
#define INRANGE(INRANGE96,2965
#define EVEN(EVEN97,3012
#define ODD(ODD98,3045
#define ODDPTR(ODDPTR99,3078
#define EVENPTR(EVENPTR100,3127
#define STRCPY(STRCPY102,3245
#define STRCAT(STRCAT103,3317
#define EMPTYSTR(EMPTYSTR104,3402
#define BSTREQ(BSTREQ105,3437
#define ATOI(ATOI106,3488
#define ISDISPLAY_ASCII(ISDISPLAY_ASCII108,3531
#define ISGRAPHIC_ASCII(ISGRAPHIC_ASCII109,3577
#define ISDIGIT(ISDIGIT110,3623
#define ISHEXDIGIT(ISHEXDIGIT111,3664
#define HEXTOBIN(HEXTOBIN115,3796
#define BINTOHEX(BINTOHEX118,3951
#define HI_NIBBLE(HI_NIBBLE121,4049
#define LO_NIBBLE(LO_NIBBLE122,4088
#define HI_16(HI_16127,4215
#define LO_16(LO_16128,4257
#define	MODID	134,4403
typedef byteptr bitarray;bitarray143,4586
#define NILBITARRAY 145,4635
typedef byteptr iddesc;iddesc183,5755
#define NILIDDESC 185,5804
#define NILID 188,5888
typedef struct str32_ str32_223,6988
} str32;str32225,7082
typedef struct str160_ str160_227,7092
} str160;str160229,7188
#define STR32PTR(STR32PTR239,7535
#define STR32LEN(STR32LEN240,7584
#define EMPTYSTR32(EMPTYSTR32241,7631
#define ISCSTR(ISCSTR244,7718
#define STR32EQ(STR32EQ246,7775
#define STR32CPY(STR32CPY250,7955
#define CSTR32CPY(CSTR32CPY255,8161
#define PSTRPTR(PSTRPTR260,8364
#define PSTRLEN(PSTRLEN261,8400
#define EMPTYPSTR(EMPTYPSTR262,8436
#define PSTREQ(PSTREQ263,8479
#define PSTRCPY(PSTRCPY264,8516
#define CPSTRCPY(CPSTRCPY265,8555
typedef uchar *symbol_table;symbol_table269,8620
typedef ushort symbol;symbol270,8649
} sy_result;sy_result275,8716
#define NILSYM 277,8730
#define SY_APPEND(SY_APPEND283,8906
#define SY_APPEND32(SY_APPEND32284,8960
#define SY_SYMBOL(SY_SYMBOL285,9017
#define SY_SYMBOL32(SY_SYMBOL32286,9071
#define SY_STRING(SY_STRING287,9128
#define SY_DELETE(SY_DELETE288,9182
typedef short database;database313,10055
#define MAXKEYS 315,10080
} key;key320,10162
#define UKSIZE(UKSIZE323,10210
} db_result;db_result328,10330
#define DB_APPEND(DB_APPEND331,10406
#define DB_REPLACE(DB_REPLACE333,10494
#define DB_RETRIEVE(DB_RETRIEVE335,10584
#define DB_DELETE(DB_DELETE337,10678
