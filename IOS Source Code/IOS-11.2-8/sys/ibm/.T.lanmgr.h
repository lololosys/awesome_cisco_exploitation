
lanmgr.h,9303
#define MAC_MAXSIZE 82,3139
#define IEEEBYTES 83,3162
#define NLNM_HASH_SIZE 84,3182
#define NLNM_HASH_MASK 85,3208
typedef struct _major_vector _major_vector87,3253
} major_vector;major_vector93,3390
#define major_vector_headstart(major_vector_headstart95,3407
#define MAC_MVID_LEN 97,3467
#define MAX_TR_FRAMESIZE 99,3491
typedef struct _subvector_t _subvector_t101,3523
} subvector_t;subvector_t105,3617
#define SVID_HDR_LEN 107,3633
typedef struct _long_subvector _long_subvector109,3657
} long_subvector;long_subvector114,3797
#define CLASS_RINGSTATION 119,3866
#define CLASS_DLCLNMGR	120,3894
#define CLASS_CRS	121,3921
#define CLASS_RPS	122,3943
#define CLASS_REM	123,3965
#define MVID_RESPONSE 128,4032
#define MVID_BEACON	129,4061
#define MVID_CLAIM	130,4087
#define MVID_RINGPURGE	131,4112
#define MVID_AMP	132,4141
#define MVID_SMP	133,4164
#define MVID_DUPTEST	134,4187
#define MVID_LOBETEST	135,4214
#define MVID_TRANSMITFWD	136,4242
#define MVID_REMOVE	137,4272
#define MVID_CHANGEPARAM	138,4298
#define MVID_INITIALIZE	139,4328
#define MVID_REQADDRESS	140,4358
#define MVID_REQSTATE	141,4388
#define MVID_REQATTACH	142,4416
#define MVID_REQINIT	143,4445
#define MVID_RPTADDRESS	144,4472
#define MVID_RPTSTATE	145,4502
#define MVID_RPTATTACH	146,4530
#define MVID_RPTNEWMON	147,4559
#define MVID_RPTNAUN	148,4588
#define MVID_NNINCOMP	149,4615
#define MVID_RPTMONERR	150,4643
#define MVID_SOFTERR	151,4672
#define MVID_RPTTXMTFWD	152,4699
#define MVID_LAST	153,4729
#define SVID_BEACONTYPE	158,4802
#define   BEACON_RECOVERYMODE 159,4845
#define   BEACON_SIGNALLOSS 160,4877
#define   BEACON_STRM_N_CLAIM 161,4909
#define   BEACON_STRM_CLAIM 162,4941
#define SVID_NAUN	163,4973
#define SVID_LOCALRING	164,5012
#define SVID_ASSGN_PHYS_LOC	165,5052
#define SVID_SOFTERRTIMER	166,5096
#define SVID_ENABLEDCLASS	167,5138
#define SVID_ALLOWEDPRIORITY	168,5189
#define SVID_CORRELATOR	169,5243
#define SVID_LASTADDRESS	170,5293
#define SVID_PHYS_LOC	171,5338
#define SVID_RESPONSE	172,5381
#define   RESP_ACK 173,5424
#define   RESP_ERROR	174,5459
#define   RESP_NO_MVID	175,5491
#define   RESP_MV_LEN	176,5525
#define   RESP_UNKN_MVID 177,5558
#define   RESP_BAD_SCLASS 178,5593
#define   RESP_SV_LEN	179,5628
#define   RESP_BAD_FWD	180,5661
#define   RESP_NO_REQ_SVID 181,5695
#define   RESP_UNKN_SVID 182,5730
#define   RESP_TOO_LARGE 183,5765
#define   RESP_FN_DISABLED 184,5800
#define SVID_ADDRESS_MOD	185,5835
#define SVID_PRODUCTID	186,5880
#define   IDLEN 187,5924
#define SVID_UCODELEVEL	188,5943
#define   UCODELEN 189,5988
#define SVID_WRAPDATA	190,6010
#define SVID_FRAMEFORWARD	191,6051
#define SVID_STATIONID	192,6093
#define SVID_STATUSSUBVEC	193,6137
#define   STATUSLEN 194,6183
#define SVID_XMITSTATUS	195,6205
#define SVID_GROUPADDRESS	196,6250
#define SVID_FUNCTIONALADDRESS	197,6296
#define SVID_ISOLATINGCOUNTS	198,6347
#define SVID_NONISOLATINGCOUNTS	199,6396
#define SVID_ERROR	200,6448
#define SVID_LAST	201,6488
#define PATHTRACE_NONE 206,6568
#define PATHTRACE_BRIDGE 207,6605
#define PATHTRACE_CONFIG 208,6642
#define PATHTRACE_STATION 209,6679
typedef struct _response _response214,6758
} response_t;response_t219,6867
typedef struct _ucodelevel _ucodelevel221,6882
} ucodelevel_t;ucodelevel_t224,6956
typedef struct _isolating_counts _isolating_counts226,6973
} isolating_counts_t;isolating_counts_t233,7112
typedef struct _nonisolating_counts _nonisolating_counts235,7135
} nonisolating_counts_t;nonisolating_counts_t242,7299
#define MAX_MAC_PAK 244,7325
typedef struct mac_data_ptrs_ mac_data_ptrs_250,7414
} mac_data_ptrs;mac_data_ptrs298,8774
#define SV_NONE	303,8812
#define SV_BEACONTYPE	304,8841
#define SV_NAUN	305,8875
#define SV_LOCALRING	306,8904
#define SV_ASSGN_PHYS_LOC	307,8937
#define SV_SOFTERRTIMER	308,8974
#define SV_ENABLEDCLASS	309,9010
#define SV_ALLOWEDPRIORITY	310,9046
#define SV_CORRELATOR	311,9084
#define SV_LASTADDRESS	312,9118
#define SV_PHYS_LOC	313,9153
#define SV_RESPONSE	314,9185
#define SV_ADDRESS_MOD	315,9217
#define SV_PRODUCTID	316,9252
#define SV_UCODELEVEL	317,9285
#define SV_WRAPDATA	318,9319
#define SV_FRAMEFORWARD	319,9351
#define SV_STATIONID	320,9387
#define SV_STATUSSUBVEC	321,9420
#define SV_XMITSTATUS	322,9456
#define SV_GROUPADDRESS	323,9490
#define SV_FUNCTIONALADDRESS	324,9526
#define SV_ISOLATINGCOUNTS	325,9566
#define SV_NONISOLATINGCOUNTS	326,9604
#define SV_ERROR	327,9645
#define MVR_RESPONSE 329,9675
#define MVO_RESPONSE 330,9710
#define MVR_BEACON	331,9761
#define MVO_BEACON	332,9819
#define MVR_CLAIM	333,9853
#define MVO_CLAIM	334,9880
#define MVR_RINGPURGE	335,9911
#define MVO_RINGPURGE	336,9942
#define MVR_AMP	337,9977
#define MVO_AMP	338,10003
#define MVR_SMP	339,10033
#define MVO_SMP	340,10059
#define MVR_DUPTEST	341,10089
#define MVO_DUPTEST	342,10118
#define MVR_LOBETEST	343,10147
#define MVO_LOBETEST	344,10181
#define MVR_TRANSMITFWD	345,10215
#define MVO_TRANSMITFWD	346,10256
#define MVR_REMOVE	347,10297
#define MVO_REMOVE	348,10325
#define MVR_CHANGEPARAM	349,10353
#define MVO_CHANGEPARAM	350,10386
#define MVR_INITIALIZE	353,10529
#define MVO_INITIALIZE	354,10561
#define MVR_REQADDRESS	356,10658
#define MVO_REQADDRESS	357,10690
#define MVR_REQSTATE	358,10728
#define MVO_REQSTATE	359,10758
#define MVR_REQATTACH	360,10794
#define MVO_REQATTACH	361,10826
#define MVR_REQINIT	362,10863
#define MVO_REQINIT	363,10923
#define MVR_RPTADDRESS	364,10959
#define MVO_RPTADDRESS	366,11068
#define MVR_RPTSTATE	367,11107
#define MVO_RPTSTATE	369,11183
#define MVR_RPTATTACH	370,11218
#define MVO_RPTATTACH	373,11344
#define MVR_RPTNEWMON	374,11375
#define MVO_RPTNEWMON	375,11435
#define MVR_RPTNAUN	376,11466
#define MVO_RPTNAUN	377,11509
#define MVR_NNINCOMP	378,11538
#define MVO_NNINCOMP	379,11575
#define MVR_RPTMONERR	380,11605
#define MVO_RPTMONERR	381,11661
#define MVR_SOFTERR	382,11692
#define MVO_SOFTERR	384,11786
#define MVR_RPTTXMTFWD	385,11815
#define MVO_RPTTXMTFWD	386,11853
#define MAXLINKS 394,12084
#define LMKEYLEN 395,12103
typedef struct isolating_counts_l_ isolating_counts_l_400,12162
} isolating_counts_l;isolating_counts_l406,12278
typedef struct nonisolating_counts_l_ nonisolating_counts_l_408,12301
} nonisolating_counts_l;nonisolating_counts_l414,12447
typedef struct station_info_ station_info_416,12473
} station_info;station_info468,14086
#define INFO_REMOVING 473,14126
#define INFO_BEACONING 474,14156
#define ERR_ME_WEIGHT 480,14321
#define ERR_ME_PREWEIGHT 481,14351
#define ERR_ME_CONGEST 482,14381
#define ERR_UPSTREAM 483,14411
#define ERR_DOWNSTREAM 484,14441
#define ERR_EXIST	485,14471
#define ERR_LANMGR 487,14496
#define ERR_ME 488,14567
#define ERR_FAULT 489,14624
struct ring_info_ ring_info_495,14727
#define BCN_NONE	545,16174
#define BCN_BEACONING	546,16195
#define BCN_PERMANENT	547,16221
#define BCN_POSTMORTEM	548,16247
#define BCN_POSTMORTEM1	549,16275
#define BCN_POSTMORTEM2	550,16304
#define BCN_POSTMORTEM3	551,16333
#define RPS_STATE_OFF	553,16363
#define RPS_STATE_DETERMINE	554,16388
#define RPS_STATE_PASSIVE	555,16418
#define RPS_STATE_ACTIVE	556,16446
#define RPS_MAX_TRIES 558,16474
#define CRS_MAX_TRIES 559,16502
#define CRS_REMOVE_TRIES 561,16531
#define RPS_SOFTERR_DEFAULT 564,16560
#define REM_WORK_INTERVAL 566,16642
#define REM_DECREMENT_INTERVAL 567,16686
#define REM_DECREMENT 568,16731
#define REM_THRESH_PREWEIGHT 570,16766
#define REM_THRESH_WEIGHT 571,16802
#define REM_WEIGHT_LIMIT 572,16838
#define REM_WEIGHT_DECREASING 573,16874
#define REM_NONISO_LIMIT 579,17039
#define REM_AUTOIN_DEF	580,17075
#define REM_RINGIN_DEF	581,17112
#define REM_NOTIF_DEF	582,17149
#define REM_CONGEST_INIT 588,17190
#define REM_CONGEST_TIME 589,17223
#define REM_SOFTERR_INIT 591,17268
#define REM_SOFTERR_TIME 592,17330
#define IBMNM_TEMP_RETAIN 594,17375
#define REPORT_LRM 599,17470
#define REPORT_LBS 600,17502
#define REPORT_CRS 601,17534
#define REPORT_REM 602,17566
#define REPORT_RPS 603,17598
#define REPORT_AVAIL 604,17630
#define REPORT_ALL 605,17662
#define LRM_CONTROLLING 607,17695
#define LRM_OTHER 608,17721
#define LRM_ALL 609,17747
#define LM_DEF_ALT_LMS 611,17774
#define LM_MAX_ALT_LMS 612,17800
#define LM_DEF_ALT_LMS 617,17950
#define LM_DEF_LOSS_THRESH 618,17980
#define LM_DEF_NOTIF_INT 619,18010
#define LM_DEF_CALC_INT 620,18040
typedef struct bridge_link_info_ bridge_link_info_622,18071
} bridge_link_info;bridge_link_info632,18419
typedef struct bridge_info_ bridge_info_634,18440
} bridge_info;bridge_info661,18955
} lnmst_t;lnmst_t671,19462
} clsi2lnm_state_t;clsi2lnm_state_t684,19708
typedef struct _lnm_clsi_t _lnm_clsi_t686,19729
} lnm_clsi_t;lnm_clsi_t706,20732
typedef struct _lnm_sap_t _lnm_sap_t710,20776
} lnm_sap_t;lnm_sap_t717,21024
typedef struct _lnm_dlc_id _lnm_dlc_id719,21038
} lnm_dlc_id_t;lnm_dlc_id_t724,21211
typedef struct _lnm_hash _lnm_hash726,21232
} lnm_t;lnm_t762,22470
static inline void start_lanmgr_timer 788,23243
static inline void set_lanmgr_timer 794,23367
static inline void bump_lanmgr_timer 799,23483
static inline void stop_lanmgr_timer 804,23572
