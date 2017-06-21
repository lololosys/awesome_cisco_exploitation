
master.h,3130
#define __MASTER_H__95,3328
#define	volatile100,3423
#define inline104,3460
#define	SMDS_ENABLE	136,4022
#define X25_ENABLE 139,4074
#define DDN_ENABLE 142,4125
#define BRIDGE_ENABLE 145,4179
#define SRSWITCH_ENABLE 148,4238
#define LAT_ENABLE 151,4294
#define TROUTER_ENABLE 154,4349
#define ROM_ENABLE 157,4404
#define CISCOPRO_IMAGE 160,4459
#define SYS_MAJORVERSION	167,4538
#define SYS_MINORVERSION	168,4566
#define SYSTEM_VERSION	169,4593
#define SYS_BRANCH 170,4659
#define MAXCHAR 175,4713
#define MAXUCHAR 176,4778
#define	MAXSHORT 177,4841
#define	MAXUSHORT 178,4904
#define MAXINT 179,4967
#define MAXUINT 180,5035
#define MAXLONG 181,5103
#define MAXULONG 182,5166
#define MINCHAR 183,5230
#define	MINSHORT 184,5292
#define	MININT	185,5356
#define	MINLONG	186,5424
#define ONE_K 192,5519
#define TWO_K 193,5555
#define FOUR_K 194,5591
#define EIGHT_K 195,5627
#define SIXTEEN_K 196,5663
#define THIRTYTWO_K 197,5699
#define SIXTYFOUR_K 198,5735
#define HALF_MEG 200,5772
#define ONE_MEG 201,5808
#define TWO_MEG 202,5844
#define FOUR_MEG 203,5880
#define EIGHT_MEG 204,5916
#define SIXTEEN_MEG 205,5952
#define THIRTYTWO_MEG 206,5988
#define SIXTYFOUR_MEG 207,6024
#define ONE_GIG 209,6061
#define btow(btow214,6129
#define wtob(wtob215,6183
#define ltob(ltob216,6233
#define btol(btol217,6282
#define max(max219,6332
#define min(min220,6394
#define offsetof(offsetof222,6457
#define ROUTINGDEPTH 224,6538
#define NETHASHLEN 225,6597
typedef struct dqueue_ dqueue_234,6892
} dqueue_t;dqueue_t239,7016
static inline int queuesize_inline 246,7091
static inline boolean queuefull_inline 250,7167
static inline boolean queuefull_reserve_inline 254,7284
static inline boolean queueempty_inline 258,7434
static inline int queuemax_inline(262,7523
#define QUEUESIZE(QUEUESIZE267,7598
#define	QUEUEFULL(QUEUEFULL268,7639
#define	QUEUEFULL_RESERVE(QUEUEFULL_RESERVE269,7680
#define QUEUEEMPTY(QUEUEEMPTY271,7760
#define QUEUEMAXIMUM(QUEUEMAXIMUM272,7803
#define ABORTED 278,7877
#define OK 279,7927
#define NO_DATA 280,7962
#define END_OF_FILE 281,8011
#define URGENT_DATA 282,8071
#define NO_MEMORY 283,8134
#define RETRY 284,8188
#define FOREIGN_RESET 285,8222
#define LOCAL_RESET 286,8283
#define REJECTED 287,8351
#define TIMEDOUT 288,8401
#define CARRIER_DROP 289,8449
#define IPADDRESS 290,8509
#define NAMEISTOOLONG 291,8563
#define BADCHARACTERS 292,8622
#define UNKNOWN_HOSTNAME 293,8692
#define UNREACHABLE 294,8744
#define IDLE_TIMEDOUT 295,8801
#define OPTSTOOBIG 296,8852
#define INUSE 297,8909
#define CMD_PRV	304,9057
#define CMD_INV	305,9116
#define CMD_QUES 306,9172
#define CMD_HPRV 307,9243
#define CMD_ASM	308,9312
#define CMD_AGS	309,9373
#define	CMD_X25	310,9433
#define CMD_SERIAL 311,9482
#define CMD_TOKENRING 312,9555
#define CMD_ISDN 313,9624
#define CM_TTY 320,9755
#define CM_RNG 321,9812
#define CM_COM 322,9866
#define CM_INT 323,9912
#define CM_RTR 324,9959
#define	CM_BTH 325,10003
#define CM_MULT 326,10057
#define SYSUPTIME_UNITS 331,10184
#define SYSUPTIME_SCALE 332,10230
