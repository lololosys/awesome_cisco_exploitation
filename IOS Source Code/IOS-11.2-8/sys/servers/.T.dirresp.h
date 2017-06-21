
dirresp.h,1582
#define	__DIRRESP_H__27,944
#define	DRP_MIN_VER	31,1004
#define	DRP_MAX_VER	32,1026
#define	DRP_VERSION	33,1048
typedef struct drfailtype_ drfailtype_38,1114
} drfailtype;drfailtype46,1344
#define	DRP_FAIL_NOFAIL 49,1380
#define	DRP_FAIL_FORMAT 50,1425
#define	DRP_FAIL_VERSION 51,1483
#define	DRP_FAIL_COMMAND 52,1537
#define	DRP_FAIL_FAMILY 53,1587
#define	DRP_FAIL_AUTH	54,1648
#define	DRP_AUTH_DIGEST_LEN	59,1744
typedef struct drauthtype_ drauthtype_60,1775
} drauthtype;drauthtype68,1991
#define	DRP_AUTH_KMD5	71,2033
#define DRP_UNREACHABLE 76,2108
typedef struct drquerytype_ drquerytype_81,2192
} drquerytype;drquerytype87,2389
#define	DRP_FAM_UNKNOWN	89,2405
#define DRP_FAM_IPV4	90,2465
typedef struct dripv4qtype_ dripv4qtype_92,2514
} dripv4qtype;dripv4qtype100,2816
enum drmettype drmettype102,2832
    DRP_METTYPE_UNKNOWN 103,2849
    DRP_METTYPE_IGRP,104,2904
    DRP_METTYPE_HOPCOUNT,105,2956
    DRP_METTYPE_ASHOPCOUNT,106,3018
    DRP_METTYPE_ISIS,107,3076
    DRP_METTYPE_OSPF,108,3134
    DRP_METTYPE_RIP,109,3192
    DRP_METTYPE_IGRP2,110,3249
    DRP_METTYPE_STATIC,111,3318
    DRP_METTYPE_CONNECTED,112,3378
    DRP_METTYPE_MOBILE,113,3452
    DRP_METTYPE_BGP,114,3512
    DRP_METTYPE_MAX115,3581
typedef struct drentrytype_ drentrytype_121,3663
} drentrytype;drentrytype128,3849
#define	DRP_OP_FAILURE	133,3897
#define	DRP_OP_AUTH	134,3922
#define	DRP_OP_RTQUERY	135,3944
#define	DRP_OP_RTRESP	136,3969
typedef struct drtype_ drtype_142,4073
} drtype;drtype147,4179
#define DRP_FLAG_AUTH	162,4576
