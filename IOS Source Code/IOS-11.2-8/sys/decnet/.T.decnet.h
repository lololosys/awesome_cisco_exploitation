
decnet.h,6708
#define __DECNET_H__90,3140
#define DN_MIN_ACL 99,3292
#define DN_MAX_ACL 100,3313
#define DN_MAXNODES 109,3555
#define DN_MAXAREAS 110,3611
#define NSP_CONNECT 112,3635
#define NSP_RECONNECT 113,3660
#define NSP_CON_CONFIRM 114,3687
#define NSP_DISC_INITIATE 115,3716
#define NSP_DISC_CONFIRM 116,3747
#define DN_MAC_CANONICAL	118,3778
#define DN_MAC_SWAPPED	119,3814
#define DN_MAC_TR	120,3849
#define DN_MAC_ETHER	121,3883
#define DN_MAC_ADDRESS	122,3922
#define DN_MAXROUTERS 140,4784
#define T2 142,4848
#define DRDELAY 152,5233
#define TRIGGER_L1_ROUTES(TRIGGER_L1_ROUTES155,5331
#define TRIGGER_L2_ROUTES(TRIGGER_L2_ROUTES162,5597
#define TRIGGER_HELLOS(TRIGGER_HELLOS169,5837
#define DECREMENT_L1_ADJ_COUNT(DECREMENT_L1_ADJ_COUNT179,6205
#define DECREMENT_PHASEIV_PLUS_ADJ_COUNT(DECREMENT_PHASEIV_PLUS_ADJ_COUNT188,6508
#define DECREMENT_OLD_PHASEIV_ADJ_COUNT(DECREMENT_OLD_PHASEIV_ADJ_COUNT194,6764
typedef struct dnhdrtype_ dnhdrtype_208,7191
} dnhdrtype;dnhdrtype211,7338
#define dnheadstart(dnheadstart213,7352
#define DN_PAKSIZE_FIELD_LEN 218,7447
#define MAXDNDATASIZE 223,7547
#define DN_CTL	233,7847
#define DN_DATA	234,7908
#define DN_LONG	235,7971
#define DN_RETREQ	236,8040
#define DN_RET	237,8085
#define DN_LOCAL 238,8135
#define DN_PHASE3 239,8197
#define DN_PAD 240,8261
#define DN_FP	241,8312
#define DN_CTL_TYPE_MASK	246,8426
#define DN_CTL_TYPE_RT_L1	247,8486
#define DN_CTL_TYPE_RT_L2	248,8539
#define DN_CTL_TYPE_HELLO_RTR	249,8592
#define DN_CTL_TYPE_HELLO_END	250,8653
#define DN_CTL_TYPE_EXT	251,8716
#define DN_CTL_EXTTYPE_MASK	256,8792
#define DN_CTL_EXTTYPE_PFPEH	257,8860
#define DN_RT_LVL1	259,8932
#define DN_RT_LVL2	260,8977
#define DN_HELLO_RTR	261,9022
#define DN_HELLO_END	262,9074
typedef struct dnhellotype_ dnhellotype_266,9173
} dnhellotype;dnhellotype283,9961
#define DN_HELLO_VERSION 285,9977
#define DN_FUNNY_PHASEIV_HELLO_VERSION 286,10025
#define DN_PHASEIV_PLUS_VERSION 294,10307
#define DN_PHASEIV_VERSION 298,10419
#define DN_NT_LEVEL2	305,10674
#define DN_NT_LEVEL1	306,10711
#define DN_NT_END	307,10734
#define DN_NT_MASK	308,10770
#define DN_MAXPACKET	310,10826
#define DN_MINPACKET	311,10852
#define dn_area(dn_area318,11045
#define dn_node(dn_node319,11086
#define dn_id(dn_id321,11210
#define dn_idn(dn_idn323,11298
#define dn_addr(dn_addr324,11351
#define get16(get16325,11451
#define put16(put16326,11503
#define put16t(put16t330,11615
#define dn_getid(dn_getid331,11702
#define DN_ADJ_UP	336,11877
#define GET_DECNET_ADDR(GET_DECNET_ADDR340,12010
typedef struct dnmetrictype_ dnmetrictype_343,12094
} dnmetrictype;dnmetrictype346,12153
#define MAX_ENCAP_BYTES 354,12445
typedef struct dnroutetype_ dnroutetype_356,12504
} dnroutetype;dnroutetype374,13309
#define DN_RT_ROUTER 390,14050
#define DN_RT_ME 391,14105
#define DN_RT_HELLO 392,14145
#define DN_RT_WAS_STATIC 393,14190
#define DN_RT_NET 394,14264
#define DN_RT_PRIME 395,14315
#define DN_RT_STATE_MASK 396,14372
#define DN_RT_STATE_REJ 397,14448
#define DN_RT_STATE_DOWN 398,14503
#define DN_RT_STATE_INIT 399,14554
#define DN_RT_STATE_UP 400,14613
#define DN_RT_STATE_NONE 401,14662
typedef struct dnroutingmsg_ dnroutingmsg_403,14731
} dnroutingmsg;dnroutingmsg415,15117
#define DN_MAX_RTGROUP 423,15346
#define DN_DAT_DSTOFF 425,15410
#define DN_DAT_VISITOFF 426,15482
typedef struct dndatapacket_ dndatapacket_428,15539
} dndatapacket;dndatapacket443,16168
#define SIZEOF_DNDATAPACKET 445,16185
#define DN_RTFL2_CON	450,16280
#define SDID 455,16380
#define DSID 456,16430
#define RSVD 457,16469
typedef struct dnnspdispkt_ dnnspdispkt_462,16566
} dnnspdispkt;dnnspdispkt468,16797
typedef struct dnnspconpkt_ dnnspconpkt_473,16864
} dnnspconpkt;dnnspconpkt481,17169
#define SIZEOF_DNNSPCONPKT 483,17185
#define DNST_RECEIVED	487,17241
#define DN_FORMATERR	488,17266
#define DN_NOTGATEWAY	489,17290
#define DN_NOTIMP	490,17315
#define DNST_HELLOS	491,17336
#define DN_BADHELLO	492,17359
#define DN_NOTLONG	493,17382
#define DNST_DATAS	494,17404
#define DN_BIGADDR	495,17426
#define DN_NOROUTE	496,17448
#define DN_NOENCAP	497,17471
#define DNST_LVL1S	498,17494
#define DN_BADLVL1	499,17517
#define DN_TOOMANYVISITS	500,17540
#define DNST_HELLOSNT	501,17568
#define DNST_LVL1SNT	502,17594
#define DNST_FORWARD	503,17619
#define DN_NOMEM	504,17644
#define DN_OTHER_AREA_HELLO	505,17665
#define DN_OTHER_AREA_LVL1	506,17696
#define DNST_LVL2S	507,17726
#define DNST_LVL2SNT	508,17749
#define DN_NOVEC	509,17774
#define DN_OTHER_LVL2	510,17795
#define DN_NOACCESS	511,17821
#define DN_NOTNET 512,17845
#define DN_NOMAP 513,17871
#define DNST_LOCAL	514,17897
#define DN_RETURNED 515,17955
#define DN_NOTCONNECT	516,18007
#define DNST_CONVERT_IV 517,18072
#define DNST_CONGEST	518,18148
#define DNST_MAX	520,18225
#define DN_SUCCESS 522,18247
#define DN_HELLO_TIME 530,18359
#define DN_ROUTE_TIME 531,18410
#define DN_MAX_ACTIVATION 532,18470
#define DN_LONGTIME 540,18885
#define DN_ROUTE_EXPIRE 546,19129
#define DN_HELLO_EXPIRE 547,19180
#define DN_MAX_COST 548,19235
#define DN_MAX_HOPS 549,19298
#define DN_MAX_AREA_COST 550,19365
#define DN_MAX_AREA_HOPS 551,19423
#define DN_MAX_BCST_ROUTERS 552,19451
#define DN_MAX_SPLITS 553,19520
#define DN_MAX_VISITS 554,19567
#define DN_MAX_PRIO 555,19628
#define DN_MAX_AREA 556,19684
#define DN_MAX_NODE 557,19733
#define DN_DEF_NODE 559,19791
#define DN_DEF_PRIO 560,19856
#define DN_DEF_CATHRESH 561,19911
#define DN_MAXNETS 566,20015
#define dnhash(dnhash572,20167
#define DNNHASH 573,20215
#define NDECNETCMDS_H 579,20364
typedef struct _dnmap _dnmap584,20443
} dnmap;dnmap593,20828
typedef struct _dn_adv_info _dn_adv_info598,20906
} dn_adv_info;dn_adv_info601,20970
struct dnnet_ dnnet_611,21488
#define PHASEIV 657,23612
#define PHASEV 658,23632
#define OSI_INJECTED 659,23652
#define PHASEV_PHASEIV 660,23677
#define PHASEIV_PLUS 661,23719
#define PHASEIV_STATIC 662,23744
typedef struct dn_tr_multi_t_ dn_tr_multi_t_666,23776
} dn_tr_multi_t;dn_tr_multi_t669,23851
typedef struct dn_info_ dn_info_671,23873
} dn_info;dn_info675,24095
#define dnet_pabuginf 680,24130
#define dnet_cobuginf 681,24177
#define dnet_robuginf 682,24229
#define dnet_pktbuginf 683,24279
#define dnet_adjbuginf 684,24331
typedef struct decnet_staticQtype_ decnet_staticQtype_689,24429
} decnet_staticQtype;decnet_staticQtype699,24839
typedef struct decnet_advQtype_ decnet_advQtype_706,25032
} decnet_advQtype;decnet_advQtype711,25206
static inline dnacctype *dn_get_acl 717,25258
static inline void dn_set_acl 722,25358
