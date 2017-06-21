
access.h,3835
#define __ACCESS_H__101,3548
#define MINFASTACC 103,3570
#define MAXFASTACC 104,3616
#define MINSLOWACC 105,3638
#define MAXSLOWACC 106,3686
#define MINTYPEACC 107,3709
#define MAXTYPEACC 108,3759
#define MINDECACC 109,3782
#define MAXDECACC 110,3825
#define	MINXNSACC 111,3848
#define	MAXXNSACC 112,3888
#define	MINEXNSACC 113,3911
#define	MAXEXNSACC 114,3960
#define	MINATALKACC 115,3983
#define	MAXATALKACC 116,4030
#define MINMACADDRACC 117,4054
#define MAXMACADDRACC 118,4110
#define	MINNOVACC 119,4136
#define	MAXNOVACC 120,4179
#define	MINENOVACC 121,4202
#define	MAXENOVACC 122,4254
#define	MINNOVSAPACC 123,4277
#define	MAXNOVSAPACC 124,4343
#define	MINXMACACC 125,4369
#define	MAXXMACACC 126,4425
#define MINNOVSUM 127,4451
#define MAXNOVSUM 128,4510
#define MAXACCESSGROUP 130,4537
#define	MINIDLEACC	132,4571
#define	MAXIDLEACC	133,4619
#define ACCESSSIZE 135,4671
#define MINREGEXPACC 143,4921
#define MAXREGEXPACC 144,4944
#define LIST_INCREMENT 146,4970
#define LIST_IP1 148,5000
#define LIST_IP2 149,5023
#define LIST_TYPE 150,5046
#define LIST_DECNET 151,5069
#define LIST_XNS1 152,5092
#define LIST_XNS2 153,5115
#define LIST_APPLE 154,5138
#define LIST_MAC 155,5161
#define LIST_NOVELL1 156,5184
#define LIST_NOVELL2 157,5207
#define LIST_NOVELL3 158,5230
#define LIST_NOVELL4 159,5254
#define	LIST_XMAC 160,5278
#define MAXTYPEITEMS	166,5342
typedef struct typeitem_ typeitem_168,5367
} typeitem;typeitem172,5439
typedef struct typetype_ typetype_175,5453
} typetype;typetype178,5581
typedef struct dnacctype_ dnacctype_184,5626
} dnacctype;dnacctype187,5786
typedef struct dnaccitemtype_ dnaccitemtype_189,5800
} dnaccitemtype;dnaccitemtype217,6917
#define DNO_EQ 222,6982
#define DNO_SON 223,7027
#define DNO_SOE 224,7088
#define DNO_DON 225,7153
#define DNO_DOE 226,7212
#define DNO_ID 227,7275
#define DNO_PW 228,7318
#define DNO_AC 229,7367
#define DNO_SUIC 230,7415
#define DNO_DUIC 231,7466
#define DNO_ANY 232,7515
typedef struct mac_address_type_ mac_address_type_235,7562
} mac_address_type;mac_address_type241,7669
#define MAC_ADDRESS_TYPE_TO_UCHAR(MAC_ADDRESS_TYPE_TO_UCHAR246,7758
#define MAC_ADDRESS_TYPE_PTR_TO_UCHAR(MAC_ADDRESS_TYPE_PTR_TO_UCHAR247,7824
#define XNS_SRCNET 252,7934
#define XNS_SRCHOST 253,7965
#define XNS_SRCHOSTMASK 254,7997
#define XNS_DSTNET 255,8028
#define XNS_DSTHOST 256,8059
#define XNS_DSTHOSTMASK 257,8090
#define XNS_PROTOCOL 261,8161
#define XNS_SRCSOCKET 262,8192
#define XNS_DSTSOCKET 263,8223
#define XNS_SERVER_TYPE 264,8254
#define XNS_PEPCLIENT 265,8285
#define XNS_SERVER_NAME 266,8316
#define XNS_SRCNETMASK 267,8347
#define XNS_DSTNETMASK 268,8378
#define XNS_ACCESS_COMMON 274,8550
typedef struct xnsnacctype_ xnsnacctype_290,8855
} xnsnacctype;xnsnacctype293,8943
typedef struct nov_sumlisttype_ nov_sumlisttype_298,9002
} nov_sumlisttype;nov_sumlisttype307,9412
typedef struct xnseacctype_ xnseacctype_312,9477
} xnseacctype;xnseacctype322,9746
typedef struct macaddracctype_ macaddracctype_330,9881
} macaddracctype;macaddracctype335,10016
struct regexpacctype_ regexpacctype_346,10319
typedef struct accessptr_ accessptr_358,10560
} accessptr;accessptr367,10749
static inline typetype *type_get_acl 371,10797
static inline void type_set_acl 376,10897
static inline macaddracctype *mac_get_std_acl 381,11006
static inline void mac_set_std_acl 386,11121
typedef void (*access_callbackfunction)access_callbackfunction402,11712
typedef void (*access_callbackfunction)(ulong)ulong402,11712
typedef struct _access_callback _access_callback403,11760
} access_callback;access_callback408,11907
} acltype;acltype416,12052
#define MAXNACLS 418,12064
#define MAXACLNAME 419,12086
struct acl_headertype_ acl_headertype_428,12356
