
standby.h,1924
#define STANDBY_AUTH_LEN 65,2259
struct standbyaddrtype_ standbyaddrtype_70,2352
struct standbyinfotype_ standbyinfotype_77,2640
#define STANDBY_ARP_RESENT 104,4039
#define standby_primary_addr(standby_primary_addr106,4074
#define standby_have_addr(standby_have_addr107,4124
#define standby_ms2sec(standby_ms2sec109,4204
#define standby_sec2ms(standby_sec2ms110,4251
#define SFLAG_CFG_IP	115,4325
#define SFLAG_CFG_IP_ADDR	116,4378
#define SFLAG_CFG_AUTH	117,4446
#define SFLAG_CFG_PREEMPT	118,4513
#define SFLAG_CFG_PRIORITY	119,4575
#define SFLAG_CFG_TIMERS	120,4640
#define SFLAG_HWADDR_COERCE	121,4701
#define SFLAG_CFG_GROUP	122,4769
#define SFLAG_CFG_TRACK	123,4829
#define SFLAG_CFG_USEBIA 128,4940
#define SDEFAULT_IP_ADDR	129,5008
#define SDEFAULT_AUTHSTRING	130,5070
#define SDEFAULT_PRIORITY	131,5141
#define SDEFAULT_HELLOTIME	132,5194
#define SDEFAULT_HOLDTIME	133,5254
#define SDEFAULT_GROUP	134,5313
#define SSTATE_VIRGIN	136,5340
#define SSTATE_LEARN	137,5367
#define SSTATE_LISTEN	138,5393
#define SSTATE_SPEAK	139,5420
#define SSTATE_STANDBY	140,5446
#define SSTATE_ACTIVE	141,5474
#define SSTATE_SPEAKER 146,5554
typedef struct standbytype_ standbytype_148,5626
} standbytype;standbytype159,6083
#define LOPCODE_HELLO	161,6099
#define LOPCODE_COUP	162,6124
#define LOPCODE_RESIGN	163,6148
#define STANDBY_VERSION	165,6175
#define STANDBY_MAXHELLOTIME	166,6202
#define STANDBY_MINCFG	167,6235
#define STANDBY_MAXPACKETSIZE	168,6272
#define STANDBY_802ADDR	169,6324
#define STANDBY_PRI_DEC	170,6386
#define STANDBY_GROUPS	171,6414
struct standbyidbtype_ standbyidbtype_173,6443
#define RESTART_ACTIVETIMER	178,6611
#define RESTART_STANDBYTIMER	183,6800
#define FOR_ALL_SBGROUPS(FOR_ALL_SBGROUPS188,6991
#define FOR_ALL_SBADDRS(FOR_ALL_SBADDRS191,7115
static inline boolean standby_mincfg 198,7292
static inline boolean standby_if_mincfg 209,7511
