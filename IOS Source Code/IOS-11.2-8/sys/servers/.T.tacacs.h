
tacacs.h,2731
#define __TACACS_H__71,2300
#define	TA_VERSION	73,2322
#define XTA_VERSION	74,2344
#define XTA_MAJOR_VERSION_MASK	75,2370
#define XTA_MINOR_VERSION_MASK	76,2406
#define XTA_MINOR_VER_0	78,2443
#define XTA_MINOR_VER_1	79,2473
#define XTA_MINOR_VER_CUR	80,2503
#define TACACS_EXTEND	85,2588
#define TACACS_ASKCONNECT	86,2649
#define TACACS_NOTECONNECT	87,2707
#define TACACS_ASKSLIP	88,2771
#define TACACS_NOTESLIP	89,2829
#define TACACS_ASKENA	90,2891
#define TACACS_NOTEENA	91,2944
#define TACACS_NOTELOGOUT	92,2999
#define TACACS_GETSLIPACL	93,3067
#define	TA_QUERY	99,3162
#define	TA_ANSWER	100,3182
#define	TA_CHANGE	101,3203
#define	TA_FOLLOW	102,3224
#define	TA_A_ACCEPTED	108,3274
#define	TA_A_REJECTED	109,3298
#define TA_A_NONE	115,3361
#define	TA_A_EXPIRING	116,3381
#define	TA_A_PASSWORD	117,3405
#define	TA_A_DENIED	118,3429
#define	TA_A_NOROUTE	119,3451
#define	TA_A_LOGINREQ	120,3509
typedef struct tacacstype_ tacacstype_127,3628
} tacacstype;tacacstype133,3746
#define	TACACS_SIZE	135,3761
typedef struct xtacacstype_ xtacacstype_141,3854
} xtacacstype;xtacacstype163,4634
#define XTACACS_SIZE	168,4699
#define	XTA_LOGIN	174,4744
#define	XTA_ANSWER	175,4764
#define	XTA_CHANGE	176,4785
#define	XTA_FOLLOW	177,4806
#define XTA_CONNECT	178,4827
#define XTA_ENABLE	179,4849
#define XTA_LOGOUT	180,4870
#define XTA_RELOAD	181,4891
#define XTA_SLIPON	182,4912
#define XTA_SLIPOFF 183,4933
#define XTA_SLIPADDR 184,4959
#define XTA_ARAP_AUTH 185,4985
#define XTA_CHAP_AUTH 186,5011
#define	XTA_A_ACCEPTED	192,5066
#define	XTA_A_REJECTED	193,5091
#define XTA_A_NONE	199,5161
#define	XTA_A_EXPIRING	200,5182
#define	XTA_A_PASSWORD	201,5207
#define	XTA_A_DENIED	202,5232
#define	XTA_A_NOROUTE	203,5255
#define	XTA_A_LOGINREQ	204,5314
#define XTA_R_QUIT	209,5468
#define XTA_R_IDLE	210,5514
#define XTA_R_DROP	211,5554
#define XTA_R_BAD	212,5597
#define XTA_F_NOENABLE	221,5786
#define XTA_F_NOENAPASS	222,5840
#define XTA_F_NOSLIP	223,5908
#define XTA_F_SLIPPASS	224,5962
#define XTA_F_CONNPASS	225,6022
#define XTA_F_ACCESS	226,6090
#define XTA_F_ROUTING	230,6238
#define	TACACS_RETRANS	232,6304
#define	TACACS_TIMEOUT	233,6329
#define TAC_SINGLELINE_CHAR	239,6476
#define TAC_SINGLELINE_ARAP_PASS	240,6509
typedef struct ta_blocktype_ ta_blocktype_244,6553
} ta_blocktype;ta_blocktype252,6913
typedef struct tacacsblock_ tacacsblock_258,6973
} tacacsblock;tacacsblock266,7312
typedef struct servtype_ servtype_268,7328
} servtype;servtype286,8311
typedef struct xta_slip_acl_ xta_slip_acl_291,8390
} xta_slip_acl;xta_slip_acl294,8458
static inline boolean xtacacs_ask_addr(369,11417
static inline paktype *xtacacs_setup(374,11583
