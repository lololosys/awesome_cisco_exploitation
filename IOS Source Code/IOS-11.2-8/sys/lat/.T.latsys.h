
latsys.h,3018
#define LATSYS_H	35,1313
#define LE_HOST	41,1614
#define LE_HOST_AS	42,1662
#define	LE_SERVER	44,1717
#define LE_SERVER_LS	45,1768
#define LE_SERVER_AGENT 46,1826
#define LE_SUBJECT_HIC	48,1889
#define LE_OBJECT_HIC	49,1950
#define LE_OBJECTS	51,2011
#define LE_HS_STATS	52,2058
#define LE_ND_STATS	53,2111
#define LE_UPPER	54,2164
#define LE_ND_RETAIN	64,2614
#define	LE_LS_DUPS	65,2666
#define CB_VIEW(CB_VIEW70,2917
#define SB_VIEW(SB_VIEW71,2961
#define	AS_VIEW(AS_VIEW72,3005
#define LS_VIEW(LS_VIEW73,3056
#define ND_VIEW(ND_VIEW74,3103
#define OB_VIEW(OB_VIEW75,3148
#define HI_VIEW(HI_VIEW76,3192
#define QR_VIEW(QR_VIEW77,3243
#define IF_VIEW(IF_VIEW78,3288
#define OF_VIEW(OF_VIEW79,3335
#define	swap(swap85,3667
#define	DLL_MAX	89,3903
#define INP_MAX	90,3961
    typedef byte IF_VIEW(IF_VIEW92,4017
    typedef byte OF_VIEW(OF_VIEW93,4083
    typedef struct ofof99,4242
    } of_node, OF_VIEW(OF_VIEW108,4636
#define OF_LENGTH	110,4669
#define OF_REXMIT	111,4725
#define OF_OUTPUT	112,4775
#define	OF_DELETE	113,4829
    typedef void *	aid_type;aid_type118,5075
    typedef void *	oid_type;oid_type119,5135
    typedef void *	sid_type;sid_type120,5189
#define AID_NULL	122,5244
#define SID_NULL	123,5309
#define OID_NULL	124,5370
#define MSEC_TIC	129,5620
#define SECS_TOC	130,5675
#define CB_MAXIMUM	138,6146
#define SB_MAXIMUM	139,6197
#define AS_MAXIMUM	140,6248
#define LS_MAXIMUM	141,6304
#define ND_MAXIMUM	142,6357
#define OB_MAXIMUM	143,6407
#define HI_MAXIMUM	144,6452
#define QR_MAXIMUM	145,6502
#define	CB_VC_MSEC	150,6744
#define CB_HD_MSEC	151,6800
#define CB_HD_MAX_MSEC 152,6856
#define CB_KA_SECS	153,6931
#define CB_PROGRESS	154,6987
#define CB_R1_UNIT	155,7044
#define CB_DEF_TRY	156,7100
#define CB_MAX_SES	157,7152
#define CB_S_RCV_EXTRA	158,7207
#define CB_H_RCV_EXTRA	159,7265
#define	SB_MAX_DATA	164,7513
#define SB_MAX_ATTN	165,7566
#define SB_REJECTS	167,7622
#define HI_STS_SEC	172,7877
#define HI_CMD_SEC	173,7930
#define HI_DEF_TRY	174,7982
#define AS_MC_SEC	179,8233
#define LS_UPDATES	184,8486
#define LS_MC_DEF	185,8542
#define ND_AGE_MAX	191,8869
#define ND_INTERVAL	192,8935
#define CB_TABLES	206,9534
#define AS_OBJECTS	208,9590
#define ND_HASHES	210,9645
#define nd_hash(nd_hash211,9699
#define SB_HASHES	213,9735
#define sb_hash(sb_hash214,9785
#define SC_STR	220,10069
#define	ND_STR	221,10111
#define SV_STR	222,10149
#define OB_STR	223,10190
#define	ID_STR	224,10231
#define GROUPS	230,10550
#define	DB_XON	243,11261
#define DB_XOFF	244,11296
#define DB_E_PARITY	249,11516
#define DB_E_RECV	250,11570
#define DB_E_XMIT	251,11622
#define DB_E_BELL	252,11674
#define DB_E_MODE	253,11726
#define DB_E_STATUS	254,11778
#define DB_E_PORT	256,11833
    } data_b;data_b267,12280
#define LAT_DATABUF 269,12295
#define LAT_OUTBOUND_CREDITS 270,12360
#define LAT_INBOUND_CREDITS 271,12432
#define SVC_CLASSES 273,12518
#define LATSTRING(LATSTRING279,12695
