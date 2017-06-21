
xns.h,4891
#define __XNS_H__67,2655
#define XNS_MIN_STD_ACL 76,2762
#define XNS_MAX_STD_ACL 77,2790
#define XNS_MIN_EXT_ACL 78,2818
#define XNS_MAX_EXT_ACL 79,2846
#define SPLIT_TRUE	82,2913
#define SPLIT_FALSE	83,2936
#define UB_FUZZY_SPLIT	84,2960
#define	XNS_TYPE_RIP	90,3016
#define	XNS_TYPE_ECHO	91,3077
#define	XNS_TYPE_ERROR	92,3124
#define XNS_TYPE_PEP	93,3173
#define XNS_TYPE_UBIPC	94,3230
#define ALLNETS_TEST 95,3284
#define	XNS_SOCK_RIP	101,3344
#define	XNS_SOCK_ECHO	102,3403
#define	XNS_SOCK_ERROR	103,3448
#define XNS_SOCK_UBIPC	104,3495
#define	XNS_INFINITY 106,3553
#define	XNS_IGRP_MIN_HOPS 107,3607
#define	XNS_IGRP_MAX_HOPS 108,3675
#define XNS_MAX_HOPS 109,3733
#define XNS_TC_HOP_MASK 110,3804
#define XNS_TC_RSV_MASK 111,3864
#define	XNSHEADERBYTES	117,3965
#define	XNS_MINSIZE	118,3994
#define	XNS_MAXSIZE	119,4020
#define	XNS_MINECHOSIZE	120,4048
#define	XNS_MULTICAST_ETHER	121,4118
#define XNS_MULTICAST_TR	122,4150
#define	XNS_FORWMAX	123,4182
#define XNS_ADDRBYTES	124,4208
#define XNS_ECHODATASTART 125,4236
#define	XNS_BROADCAST_NET	126,4283
} xns_cisco_ping;xns_cisco_ping131,4389
typedef struct xns_rip_ xns_rip_133,4408
} xns_rip_t;xns_rip_t142,4915
typedef struct xnssb_ xnssb_144,4929
} xnssb;xnssb163,6053
#define XNS_NETBYTES 169,6090
#define XNS_HOSTBYTES 170,6114
#define XNS_SOCKBYTES 171,6138
#define XNSMACRO 173,6163
typedef struct xnshdrtype_ xnshdrtype_185,6683
} xnshdrtype;xnshdrtype187,6745
#define xnsheadstart(xnsheadstart189,6760
#define	XNSOVERHEADBYTES	191,6810
#define XNSDATASTART(XNSDATASTART192,6838
#define	RIP_FLASHUPDATE	198,6939
#define RIP_DEFAULT_UPDATE 200,7006
#define RIP_AGEINTERVAL 201,7077
#define	XNSRIP_INVALIDTIME	205,7260
#define	NOVRIP_INVALIDTIME	206,7328
#define	RIP_HOLDDOWNTIME	208,7397
#define RIP_STALETIME 209,7468
#define	RIP_MAXSIZE	211,7547
#define	RIP_REQUEST	213,7598
#define	RIP_REPLY	214,7647
#define	X_RIP_REQUEST	215,7695
#define	X_RIP_REPLY	216,7755
#define RIP_MAXPATHS 217,7815
#define XNS_RIP_DELAY_TIME 218,7841
#define UB_UPDATE_INTERVAL	220,7912
#define UB_HELLO_INTERVAL	221,7987
#define UB_RIP_REPLY	223,8061
#define UB_RIP_HELLO	224,8115
#define UB_DELAY_9600 227,8205
#define UB_DELAY_19200 228,8232
#define UB_DELAY_56K 229,8259
#define UB_DELAY_128K 230,8286
#define UB_DELAY_256K 231,8313
#define UB_DELAY_512K 232,8340
#define UB_DELAY_1544K 233,8366
#define UB_DELAY_10M 234,8392
#define UB_DELAYS 236,8492
#define UB_SPEEDS 239,8687
#define UB_MAXDELAY 243,8902
typedef struct xns_ripinfotype_ xns_ripinfotype_249,9045
} xns_ripinfotype;xns_ripinfotype252,9165
typedef struct xns_riptype_ xns_riptype_254,9185
} xns_riptype;xns_riptype257,9307
typedef struct x_ripinfotype_ x_ripinfotype_259,9323
} x_ripinfotype;x_ripinfotype264,9527
typedef struct x_riptype_ x_riptype_266,9545
} x_riptype;x_riptype269,9664
typedef struct pathentrytype_ pathentrytype_273,9725
} pathentrytype;pathentrytype282,10098
typedef struct xns_ripentrytype_ xns_ripentrytype_288,10157
} xns_ripentrytype;xns_ripentrytype305,11049
typedef xns_ripentrytype *rip_routetable[rip_routetable307,11070
typedef struct xnsechotype_ xnsechotype_309,11125
} xnsechotype;xnsechotype322,11393
#define	XNS_ECHO_REQUEST	324,11409
#define	XNS_ECHO_REPLY	325,11436
typedef struct xnserrtype_ xnserrtype_332,11498
} xnserrtype;xnserrtype337,11721
#define	XNS_ERR_UNSPEC	339,11736
#define	XNS_ERR_BADSUM	340,11801
#define	XNS_ERR_NOSOCK	341,11861
#define	XNS_ERR_FULL	342,11926
#define	XNS_ERR_TUNSPEC	343,11976
#define	XNS_ERR_TBADSUM	344,12042
#define	XNS_ERR_UNREACH 345,12103
#define	XNS_ERR_2OLD	346,12164
#define	XNS_ERR_2BIG	347,12217
#define	XNS_ERR_CONGW	348,12268
#define	XNS_ERR_CONGE	349,12336
#define PEPMACRO 351,12405
typedef struct xnspephdrtype_ xnspephdrtype_355,12536
} xnspephdrtype;xnspephdrtype358,12634
#define	XNS_INPUT	364,12686
#define	XNS_LOCALDEST	365,12707
#define	XNS_BCASTIN	366,12732
#define	XNS_FORWARDED	367,12755
#define	XNS_BCASTOUT	368,12780
#define	XNS_ERRIN	369,12804
#define	XNS_ERROUT	370,12825
#define	XNS_FORMATERROR	371,12847
#define	XNS_CKSUMERR	372,12874
#define	XNS_NOTGATEWAY	373,12898
#define	XNS_HOPCOUNT	374,12924
#define	XNS_NOROUTE	375,12949
#define	XNS_ENCAPSFAILED	376,12973
#define	XNS_OUTPUT	377,13001
#define	XNS_INMULT	378,13024
#define	XNS_UNKNOWN	379,13047
#define	XNS_FWDBRD	380,13071
#define	XNS_ECHOREQIN	381,13094
#define	XNS_ECHOREQOUT	382,13120
#define	XNS_ECHOREPIN	383,13147
#define	XNS_ECHOREPOUT	384,13173
#define	XNS_MAXTRAFFIC	386,13201
static inline ushort ub_hops_to_delay 422,14300
static inline struct xnsnacctype_ *xns_get_std_acl 436,14624
static inline void xns_set_std_acl 441,14749
static inline struct xnseacctype_ *xns_get_ext_acl 446,14872
static inline void xns_set_ext_acl 451,14997
