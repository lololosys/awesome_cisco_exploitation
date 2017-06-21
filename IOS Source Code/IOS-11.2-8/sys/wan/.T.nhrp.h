
nhrp.h,7686
#define AF_RESERVED	64,2059
#define AF_IP	65,2081
#define AF_IP6	66,2122
#define AF_NSAP	67,2165
#define AF_HDLC	68,2195
#define AF_1822	69,2243
#define AF_802	70,2277
#define AF_E163	72,2361
#define AF_E164	73,2392
#define AF_F69	74,2448
#define AF_X121	75,2485
#define AF_IPX	76,2537
#define AF_ATALK	77,2566
#define AF_DECNET	78,2602
#define AF_VINES	79,2639
#define AF_RESERVED1	80,2678
#define NHRP_REQUEST	85,2729
#define NHRP_REPLY	86,2774
#define NHRP_REG	87,2815
#define NHRP_PURGE	88,2857
#define NHRP_ERR	89,2898
#define NHRP_TYPES	90,2937
#define NHRP_OP_MASK	95,3000
#define NHRP_OP_NETID	96,3047
#define NHRP_OP_RESP	97,3089
#define NHRP_OP_RECORD	98,3137
#define NHRP_OP_RRECORD	99,3184
#define NHRP_OP_QOS	100,3232
#define NHRP_OP_AUTH	101,3265
#define NHRP_PFLAG 104,3312
#define NHRP_AFLAG 105,3382
#define NHRP_SFLAG 106,3443
#define NHRP_QFLAG 107,3514
#define NHRP_IFLAG 111,3612
#define NHRP_RFLAG 112,3679
#define NHRP_GFLAG 113,3748
#define NHRP_UFLAG 114,3819
#define NHRP_CACHED_FLAGS 117,3921
#define NHRPFIXED 122,4035
typedef struct nhrpfixedtype_ nhrpfixedtype_130,4318
} nhrpfixedtype;nhrpfixedtype132,4364
#define NHRP_FIXEDLEN 134,4382
typedef struct nhrpreqreptype_ nhrpreqreptype_140,4487
} nhrpreqreptype;nhrpreqreptype147,4751
#define NHRP_REQLEN 149,4770
typedef struct nhrpnhoptype_ nhrpnhoptype_155,4920
} nhrpnhoptype;nhrpnhoptype161,5195
#define NHRP_NHOPLEN 163,5212
#define NHRP_PURGE_AFLAG	169,5330
typedef struct nhrpregtype_ nhrpregtype_175,5434
} nhrpregtype;nhrpregtype180,5609
#define NHRP_REGLEN 182,5625
typedef struct nhrperrtype_ nhrperrtype_188,5727
} nhrperrtype;nhrperrtype193,5944
#define NHRP_ERRLEN 195,5960
#define NHRP_ERR_OPT	197,6030
#define NHRP_ERR_NETID	198,6074
#define NHRP_ERR_LOOP	199,6125
#define NHRP_ERR_CANT	200,6169
#define NHRP_ERR_FULL	201,6224
#define NHRP_ERR_NHS_REACH 202,6268
#define NHRP_ERR_PROTO	203,6319
#define NHRP_ERR_FRAG	204,6367
typedef union nhrpaddrtype_ nhrpaddrtype_209,6488
} nhrpaddrtype;nhrpaddrtype216,6844
typedef union nhrpaddr_ nhrpaddr_221,6908
} nhrpaddr;nhrpaddr229,7065
typedef union nhrpnet_ nhrpnet_231,7078
} nhrpnet;nhrpnet241,7298
typedef union nhrpaddrnet_ nhrpaddrnet_243,7310
} nhrpaddrnet;nhrpaddrnet272,7917
#define NHRP_RADIX_KEYLEN 274,7933
typedef struct nhrphwaddrtype_ nhrphwaddrtype_281,8089
} nhrphwaddrtype;nhrphwaddrtype286,8308
typedef struct nhrpnbmatype_ nhrpnbmatype_288,8327
} nhrpnbmatype;nhrpnbmatype293,8563
} nhrp_cachemodetype;nhrp_cachemodetype302,8785
typedef struct nhrpcacherequestor_ nhrpcacherequestor_308,8846
} nhrpcacherequestor;nhrpcacherequestor319,9230
typedef struct nhrpcachetype_ nhrpcachetype_321,9253
} nhrpcachetype;nhrpcachetype338,10090
typedef struct nhrpmcasttype_ nhrpmcasttype_340,10108
} nhrpmcasttype;nhrpmcasttype345,10326
typedef void * nhrp_mask;nhrp_mask347,10344
typedef boolean (*nhrp_onnettype)nhrp_onnettype348,10370
typedef boolean (*nhrp_onnettype)(nhrpnet *, nhrpaddr nhrpaddr348,10370
typedef void (*nhrp_copynettype)nhrp_copynettype349,10428
typedef void (*nhrp_copynettype)(nhrpnet *, nhrpnet nhrpnet349,10428
typedef boolean (*nhrp_specifictype)nhrp_specifictype350,10484
typedef boolean (*nhrp_specifictype)(nhrpnet *, nhrpnet nhrpnet350,10484
typedef boolean (*nhrp_specific2type)nhrp_specific2type351,10544
typedef boolean (*nhrp_specific2type)(uchar *, uchar uchar351,10544
typedef boolean (*nhrp_cmpaddrtype)nhrp_cmpaddrtype352,10601
typedef boolean (*nhrp_cmpaddrtype)(nhrpaddr *, nhrpaddr nhrpaddr352,10601
typedef boolean (*nhrp_cmpnettype)nhrp_cmpnettype353,10662
typedef boolean (*nhrp_cmpnettype)(nhrpnet *, nhrpnet nhrpnet353,10662
typedef idbtype * (*nhrp_nhoptype)nhrp_nhoptype354,10720
typedef idbtype * (*nhrp_nhoptype)(paktype *, nhrpaddr *, boolean)boolean354,10720
typedef nhrpaddr *(*nhrp_ifaddrtype)nhrp_ifaddrtype355,10788
typedef nhrpaddr *(*nhrp_ifaddrtype)(idbtype idbtype355,10788
typedef void (*nhrp_copymasktype)nhrp_copymasktype356,10837
typedef void (*nhrp_copymasktype)(nhrp_mask, nhrpcachetype nhrpcachetype356,10837
typedef boolean (*nhrp_isbroadtype)nhrp_isbroadtype357,10900
typedef boolean (*nhrp_isbroadtype)(nhrpaddr *, idbtype idbtype357,10900
typedef idbtype *(*nhrp_rtlookuptype)nhrp_rtlookuptype358,10960
				      boolean boolean359,11032
typedef boolean (*nhrp_ouraddtype)nhrp_ouraddtype360,11054
typedef boolean (*nhrp_ouraddtype)(nhrpaddr nhrpaddr360,11054
typedef boolean (*nhrp_ourifaddtype)nhrp_ourifaddtype361,11102
typedef boolean (*nhrp_ourifaddtype)(idbtype *, nhrpaddr nhrpaddr361,11102
typedef boolean (*nhrp_lookuptype)nhrp_lookuptype362,11163
typedef boolean (*nhrp_lookuptype)(paktype *, void *, uchar uchar362,11163
typedef void (*nhrp_checkcachetype)nhrp_checkcachetype363,11234
typedef void (*nhrp_checkcachetype)(nhrpaddr nhrpaddr363,11234
typedef nhrp_mask (*nhrp_netsmasktype)nhrp_netsmasktype364,11283
typedef nhrp_mask (*nhrp_netsmasktype)(nhrpaddr *, boolean boolean364,11283
typedef struct nhrpfuns_ nhrpfuns_365,11346
} nhrpfuns;nhrpfuns387,12040
typedef boolean (*nhrp_cacheupdatefunc)nhrp_cacheupdatefunc389,12053
					void *, nhrpfuns nhrpfuns391,12149
typedef struct nhrpnhstype_ nhrpnhstype_397,12236
} nhrpnhstype;nhrpnhstype401,12401
#define nhrpopstart(nhrpopstart406,12471
#define NHRP_HASHLEN 408,12539
#define NHRP_DEF_HOLDTIME 409,12599
#define NHRP_VERSION 410,12649
#define NHRP_MAXHOPS 411,12699
#define NHRP_BYTESIZE 412,12759
#define NHRP_NBMA_MAXLEN 416,12854
#define NHRP_NBMA_BYTES(NHRP_NBMA_BYTES417,12912
typedef struct nhrpstattype_ nhrpstattype_420,12973
} nhrpstattype;nhrpstattype422,13073
typedef struct nhrpopgen_ nhrpopgen_424,13090
} nhrpopgentype;nhrpopgentype427,13161
typedef struct nhrpopmask_ nhrpopmask_429,13179
} nhrpopmasktype;nhrpopmasktype432,13246
typedef struct nhrpopnetid_ nhrpopnetid_434,13265
} nhrpopnetidtype;nhrpopnetidtype437,13334
typedef struct nhrpopres_ nhrpopres_439,13354
} nhrpoprestype;nhrpoprestype442,13420
typedef struct nhrpoprec_ nhrpoprec_444,13438
} nhrpoprectype;nhrpoprectype447,13509
typedef struct nhrpopauth_ nhrpopauth_449,13527
} nhrpopauthtype;nhrpopauthtype452,13646
#define NHRP_AUTH_CLEAR	454,13665
#define NHRP_AUTH_MD5	455,13693
#define NHRP_OPLEN_MASK	457,13720
#define NHRP_OPLEN_NETID	458,13747
#define NHRP_OPLEN_RESP	459,13774
#define NHRP_OPLEN_REC	460,13801
#define NHRP_OPDEF_MASK 462,13857
#define NHRP_OPLEN_MAXTOTAL	463,13892
#define NHRP_TL_LEN	464,13924
#define NHRP_OPLEN_DEF	470,14094
#define NHRP_ALIGN(NHRP_ALIGN472,14143
#define NHRP_BYTE_LEN(NHRP_BYTE_LEN473,14214
#define NHRP_TLV_OPTBIT 474,14253
#define NHRP_AUTHLEN_MAX 476,14285
#define NHRP_NBMA_MAXSTR 477,14345
#define NHRP_NETID_UNDEF	478,14407
#define NHRP_CFG_HOLD	480,14435
#define NHRP_CFG_NOREC	481,14461
#define NHRP_IF_INIT	482,14488
#define NHRP_PERIODIC 487,14592
#define NHRP_IF_RATE_IVL 488,14648
#define NHRP_MAX_PKT 489,14717
#define NHRP_USE_DEF 490,14774
#define NHRP_RETRANS_RATE 491,14839
#define NHRP_RETRANS_EXP_TIME 492,14905
#define NHRP_MAPNAME 493,14980
#define NHRP_MAPNAME_LEN 494,15009
#define NHRP_MAX_RET_DELAY 495,15037
#define NHRP_NETPROTOCNT 497,15109
 NHRP_NETIP 499,15184
 NHRP_NETAT,500,15201
 NHRP_NETIPX501,15214
} nhrp_eventcode;nhrp_eventcode506,15262
typedef struct nhrp_event_ nhrp_event_508,15281
} nhrp_eventtype;nhrp_eventtype519,15488
} nhrp_timertype;nhrp_timertype524,15564
} nhrpproinfotype;nhrpproinfotype553,16665
struct nhrpinfotype_ nhrpinfotype_555,16685
