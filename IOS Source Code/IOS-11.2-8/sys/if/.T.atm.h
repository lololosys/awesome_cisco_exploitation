
atm.h,14767
#define __ATM_H__270,10629
#define ATM_DM_PTI 277,10765
#define ATM_DM_XD_INTT 278,10835
#define ATM_DM_EOM_EN 279,10874
#define ATM_DM_APP_CRC32 280,10913
#define ATM_DM_PKT_AAL34 281,10952
#define ATM_DM_PKT_AAL5 282,11028
#define ATM_DM_PKT_OAM_VC 283,11102
#define ATM_DM_PKT_OAM_DM 284,11180
#define ATM_DM_PKT_MSK 285,11253
typedef	struct	atm_multicast atm_multicast289,11341
} atm_multicast_t;atm_multicast_t294,11587
} atm_setup_status_t;atm_setup_status_t300,11696
#define	ATM_MID_MIN	302,11719
#define	ATM_MID_MAX	303,11743
#define ATM_KBPS_TO_CELLRATE(ATM_KBPS_TO_CELLRATE305,11770
#define ATM_MBPS2KBPS	312,12008
#define ATM_MAX_NSAP_STRING	317,12097
#define	ATM_RXBUFF_DEFAULT	322,12170
#define	ATM_TXBUFF_DEFAULT	323,12201
#define	ATM_EXCQ_DEFAULT	324,12232
#define	ATM_RAWQ_DEFAULT	325,12260
#define	ATM_VC_PER_VP_DEFAULT	326,12288
#define ATM_MAX_VPI_BITS 327,12323
#define ATM_PAYLOAD_BYTES_IN_A_CELL 328,12358
#define	ATM_VP_FILTER_DEFAULT	329,12428
#define	ATM_MAX_VC_DEFAULT	330,12463
#define	ATM_MID_DEFAULT	331,12496
#define	ATM_IDLETIME_DEFAULT	332,12533
#define ATM_CELLSIZE 333,12575
#define ATM_DATASIZE 334,12634
#define ATM_AAL34_OVERHEAD 335,12699
#define ATM_ADDREG_DEFAULT 336,12769
#define ATM_ILMIPOLL_DEFAULT 337,12813
#define ATM_ILMIPOLL_MIN 338,12846
#define	ATM_MTP_INTERVAL_DEFAULT 339,12889
#define ATM_MTU_REJECT_DEFAULT 340,12936
#define ATM_BITS2NUM(ATM_BITS2NUM346,13107
#define ATM_MAX_VCNUM	348,13159
#define ATM_VP_FILTER_MIN 353,13261
#define ATM_VP_FILTER_MAX 354,13289
#define	ATM_ENCTYPE_SNAP	366,13758
#define	ATM_ENCTYPE_SMDS	367,13811
#define	ATM_ENCTYPE_NLPID	368,13862
#define	ATM_ENCTYPE_FRNLPID	369,13912
#define	ATM_ENCTYPE_SAAL	370,13967
#define	ATM_ENCTYPE_RAW	371,14017
#define	ATM_ENCTYPE_ILMI	372,14073
#define ATM_ENCTYPE_LANE 373,14119
#define	ATM_ENCTYPE_VCSTAT	374,14194
#define ATM_ENCTYPE_LANE_BUS	388,14854
#define TBRIDGE_OUI 393,14943
#define TBRIDGE_OUI_1ST_BYTE 394,14980
#define TBRIDGE_LLC_OUI_1ST_LONG 395,15016
#define TBRIDGE_OUI_LAST_SHORT 396,15062
#define TBRIDGE_PID_8023FCS 397,15100
#define TBRIDGE_PID_8024FCS 398,15162
#define TBRIDGE_PID_8025FCS 399,15224
#define TBRIDGE_PID_FDDIFCS 400,15286
#define TBRIDGE_PID_8026FCS 401,15347
#define TBRIDGE_PID_8029FCS 402,15409
#define TBRIDGE_PID_8023 403,15471
#define TBRIDGE_PID_8024 404,15528
#define TBRIDGE_PID_8025 405,15585
#define TBRIDGE_PID_FDDI 406,15642
#define TBRIDGE_PID_8026 407,15698
#define TBRIDGE_PID_8029 408,15755
#define TBRIDGE_PID_BPDU 409,15812
#define TBRIDGE_ATM_PAD 411,15861
#define	ATM_BRIDGE_PAD_SIZE	412,15895
enum	ATM_COUNTERS	ATM_COUNTERS417,15994
	ATM_INPUT,418,16014
	ATM_OUTPUT,419,16063
	ATM_BROADCAST,420,16114
	ATM_INPUT_NOVC,421,16163
	ATM_OUT_NOVC,422,16219
	ATM_BRIDGE,423,16274
	ATM_CRC_ERRS,424,16302
	ATM_CELL_LOSS,425,16332
	ATM_OAM_INPUT_CELLS,426,16363
	ATM_OAM_OUTPUT_CELLS,427,16412
#define	ATM_MAX_COUNTERS	430,16467
typedef enum atm_universiontype_ atm_universiontype_437,16634
} atm_uniVersionType;atm_uniVersionType444,16841
#define ATMUNI_VERSION_DEFAULT 445,16863
typedef ushort	vcd_t;vcd_t447,16918
typedef int     user_vc_type_t;user_vc_type_t448,16940
typedef	struct	atmaddr	atmaddr450,16973
} atmaddr_t;atmaddr_t452,17035
enum	ATM_VC_ENCAP ATM_VC_ENCAP459,17227
	ATM_VC_AAL5SNAP,460,17247
	ATM_VC_AAL5NLPID,461,17294
	ATM_VC_AAL5FRNLPID,462,17336
	ATM_VC_AAL5MUX,463,17387
	ATM_VC_AAL34SMDS,464,17427
	ATM_VC_AALQSAAL,465,17472
	ATM_VC_AAL5ILMI,466,17512
	ATM_VC_AAL5LANE,467,17545
enum	ATM_OAM_Status	ATM_OAM_Status473,17629
	ATM_OAM_DISABLED,474,17651
	ATM_OAM_SENT,475,17697
	ATM_OAM_RECEIVED,476,17748
	ATM_OAM_FAILED,477,17799
#define	ATM_INARP_FREQ	480,17852
#define	ATM_VC_INARP	481,17894
#define	ATM_VC_OAM	482,17931
#define	VC_FLAG_AAL_MASK	484,17995
#define	VC_FLAG_ACTIVE	485,18052
#define	VC_FLAG_PVC	486,18101
#define	VC_FLAG_SVC	487,18146
#define	VC_FLAG_NOIDLETIME	488,18191
#define	VC_FLAG_TEMP	489,18255
#define	VC_FLAG_MULTIPOINT	490,18316
#define VC_FLAG_DEFAULT_RATE 491,18375
#define VC_FLAG_DEFAULT_BURST 492,18455
#define VC_FLAG_NOBUMP_CACHE 493,18528
#define VC_FLAG_DELETED 497,18672
#define VC_FLAG_PVC_DOWN 498,18746
#define	VC_FLAG_HOLD	501,18877
#define	MAX_BURST_SIZE	503,18933
#define	MAX_OAM_RETRIES	504,18963
#define UNSUPPORTED_COUNT 505,19022
struct	vc_info_t_ vc_info_t_507,19058
#define  ATM_TRAFFIC_SHAPING_VERIFIED 583,22270
#define  ATM_PEAKRATE_ERROR 584,22334
#define  ATM_AVGRATE_ERROR 585,22390
#define  ATM_BURST_ERROR 586,22445
#define  ATM_OUT_OF_RATE_QUEUES 587,22505
typedef enum atm_qos_t_ atm_qos_t_589,22582
    } atm_qos_t;atm_qos_t595,22720
typedef struct atm_userCellRate_t_ atm_userCellRate_t_597,22738
} atm_userCellRate_t;atm_userCellRate_t606,23312
typedef struct atm_trParamQos_t_ atm_trParamQos_t_608,23335
} atm_trParamQos_t;atm_trParamQos_t611,23428
#define	IS_VC_HOLD(IS_VC_HOLD614,23454
#define	IS_VC_DELETED(IS_VC_DELETED615,23514
#define IS_PVC_DOWN(IS_PVC_DOWN616,23586
#define IS_ATMVC_SVC(IS_ATMVC_SVC617,23659
#define IS_ATMVC_PVC(IS_ATMVC_PVC618,23712
#define IS_ATMIF_AIP(IS_ATMIF_AIP619,23768
#define	ISVCBROADCAST(ISVCBROADCAST621,23839
#define	ISVCACTIVE(ISVCACTIVE622,23906
#define	ISVCAAL34(ISVCAAL34623,23968
#define	ISVCAAL5SNAP(ISVCAAL5SNAP624,24033
#define	ISVCAAL5NLPID(ISVCAAL5NLPID625,24100
#define	ISVCILMI(ISVCILMI626,24168
#define ISVCAAL5LANE(ISVCAAL5LANE627,24231
#define	HASHVC(HASHVC630,24328
#define	ATM_CFG_MODE_AAL5	632,24380
#define	ATM_CFG_MODE_AAL34	633,24428
#define	ATM_CFG_DELAY_TIME	634,24479
#define	ATM_VC_MINRATE	635,24539
#define	ATM_VC_MAXRATE	636,24592
#define ATM_PLIM_INVALID	638,24655
#define ATM_PLIM_4B5B	639,24684
#define ATM_PLIM_SONET	640,24711
#define ATM_PLIM_E3	641,24739
#define ATM_PLIM_DS3	642,24764
#define ATM_PLIM_E1	643,24790
#define ATM_PLIM_DS1	644,24815
#define ATM_PLIM_8B10B	645,24841
#define ATM_RATEQUEUE_MAX 647,24870
#define ATM_VC_RATEQ_SHIFT 648,24934
#define ATM_VC_RATEQ0 649,25010
#define ATM_VC_RATEQ1 650,25082
#define ATM_VC_RATEQ2 651,25154
#define ATM_VC_RATEQ3 652,25226
#define ATM_VC_RATEQ4 653,25298
#define ATM_VC_RATEQ5 654,25369
#define ATM_VC_RATEQ6 655,25440
#define ATM_VC_RATEQ7 656,25511
#define ATM_RATEQ_MAX 657,25582
#define ATM_RATEQ_MIN 658,25638
#define	PLIM_4B5B_RATE	660,25691
#define	PLIM_SONET_RATE	661,25744
#define	PLIM_E3_RATE	662,25799
#define	PLIM_DS3_RATE	663,25846
#define	PLIM_E1_RATE	664,25895
#define	PLIM_DS1_RATE	665,25940
enum atm_media atm_media667,25994
  ATM_MEDIA_UNKNOWN,668,26011
  ATM_MEDIA_COAX,669,26032
  ATM_MEDIA_SINGLE_FIBER,670,26082
  ATM_MEDIA_MULTI_FIBER,671,26136
  ATM_MEDIA_STP,672,26189
  ATM_MEDIA_UTP,673,26247
enum atm_loop atm_loop676,26311
  ATM_LOOP_NONE,677,26327
  ATM_LOOP_LINE,678,26365
  ATM_LOOP_PAYL,679,26413
  ATM_LOOP_CELL,680,26465
  ATM_LOOP_DIAG,681,26514
  ATM_LOOP_TEST	682,26563
typedef struct	atm_config_ atm_config_685,26615
} atm_config;atm_config700,27321
typedef struct atm_def_config_ atm_def_config_702,27336
} atm_def_config;atm_def_config708,27668
#define	ATM_HASH_DEFAULT	710,27687
#define	ATM_BAD_VC(ATM_BAD_VC711,27744
#define ATM_OUT_OF_VCDS 713,27843
#define	ATM_DEFAULT_MTU	715,27923
#define	ATM_BUFFERSIZE_WORDS	716,27953
#define	MAX_ATM_INTFC	718,28031
#define MAX_DEBUG_FILTER_TYPES 719,28088
#define ATM_PACKET_FILTERING 720,28157
#define ATM_OAM_FILTERING 721,28230
typedef	void	(*rst_t)rst_t724,28301
typedef	void	(*rst_t)(hwidbtype hwidbtype724,28301
typedef	void	(*love_note_t)love_note_t725,28337
typedef	void	(*love_note_t)(cdbtype *, ushort)ushort725,28337
typedef	void	(*rawinput_t)rawinput_t726,28385
typedef	void	(*rawinput_t)(hwidbtype *, paktype paktype726,28385
typedef	unsigned (*encapsulate_t)encapsulate_t727,28437
typedef	unsigned (*encapsulate_t)(atm_db_t *, paktype *, vc_info_t *, ushort)ushort727,28437
typedef	boolean	(*setupvc_t)setupvc_t728,28516
typedef	boolean	(*setupvc_t)(hwidbtype *, vc_info_t vc_info_t728,28516
typedef	boolean	(*teardownvc_t)teardownvc_t729,28572
typedef	boolean	(*teardownvc_t)(hwidbtype*, vc_info_t vc_info_t729,28572
typedef	boolean	(*config_t)config_t730,28630
typedef	boolean	(*config_t)(hwidbtype hwidbtype730,28630
typedef atm_setup_status_t (*setup_rateq_t)setup_rateq_t731,28672
typedef atm_setup_status_t (*setup_rateq_t)(hwidbtype *, ushort)ushort731,28672
typedef boolean	(*getvc_stats_t)getvc_stats_t732,28738
typedef boolean	(*getvc_stats_t)(hwidbtype *, vc_info_t vc_info_t732,28738
typedef ushort *(*sniff_t)sniff_t733,28798
                           ushort ushort734,28861
typedef uint (*qos_param_verify_t)qos_param_verify_t735,28909
				      unsigned avgrate,unsigned burstcells)burstcells736,28975
typedef	struct	intfc_functions_t_ intfc_functions_t_739,29026
} intfc_func_t;intfc_func_t753,29752
typedef struct	atm_MIB_stats_t_	atm_MIB_stats_t_755,29769
} atm_MIB_stat_t;atm_MIB_stat_t762,30015
typedef struct	atm_real_stats_t_	atm_real_stats_t_771,30323
} atm_real_stats_t;atm_real_stats_t777,30491
enum	Config_Status Config_Status779,30512
	ATM_CONFIG_OK,780,30533
	ATM_CONFIG_CHANGED,781,30549
	ATM_CONFIG_ENABLE,782,30570
	ATM_CONFIG_IN_PROGRESS,783,30590
	ATM_VCS_NOT_UP,784,30615
	ATM_VCS_UP,785,30632
enum	RATE_QUEUE_STATE	RATE_QUEUE_STATE794,30845
	RATE_QUEUE_DISABLED,795,30869
	RATE_QUEUE_PERMANENT,796,30891
	RATE_QUEUE_DYNAMIC797,30914
typedef	struct rateq_info rateq_info800,30938
} rateq_info_t;rateq_info_t804,31089
typedef struct rateq_param_t_ rateq_param_t_806,31106
} rateq_param_t;rateq_param_t810,31215
#define ILMI_PREFIX_SIZE 814,31278
#define ILMI_ESI_OFFSET 815,31306
#define ILMI_ESI_SIZE 816,31348
#define ILMI_ADDR_SET_LEN 817,31372
#define OID_HDR_SIZE 818,31401
typedef struct atmSig_nsap_info_ atmSig_nsap_info_821,31488
} atmSig_nsap_info;atmSig_nsap_info830,32011
#define	VCD_UPDATE_INTERVAL	832,32032
struct	atm_db_t_	atm_db_t_835,32100
#define	ATM_FLAG_OAMF5	844,32537
#define	ATM_FLAG_OAMF4	845,32594
#define	ATM_FLAG_PVC	846,32652
#define	ATM_FLAG_UP	847,32694
#define	ATM_FLAG_DS3SCRAM	848,32742
#define ATM_FLAG_FLUSH_CACHE	849,32800
#define ATM_FLAG_ARP_RESET	850,32866
#define	ATM_FLAG_DB_INUSE	851,32929
typedef struct	atm_map_t_	atm_map_t_942,37318
} atm_map_t;atm_map_t951,37681
#define ATM_FWPKRATE0 956,37731
#define ATM_BKPKRATE0 957,37758
#define ATM_FWPKRATE1 958,37785
#define ATM_BKPKRATE1 959,37812
#define ATM_FWSTRATE0 960,37839
#define ATM_BKSTRATE0 961,37866
#define ATM_FWSTRATE1 962,37893
#define ATM_BKSTRATE1 963,37920
#define ATM_FWBSIZE0 964,37947
#define ATM_BKBSIZE0 965,37974
#define ATM_FWBSIZE1 966,38001
#define ATM_BKBSIZE1 967,38028
#define ATM_TRAFMGMT 968,38055
#define ATM_BLLIE 969,38082
typedef struct atm_params_type_ atm_params_type_971,38109
} atm_params_type;atm_params_type990,38525
typedef	struct	atm_encap_ atm_encap_992,38545
} atm_encap_t;atm_encap_t996,38685
typedef	struct	atm_aal5mux_ atm_aal5mux_998,38701
} atm_aal5mux_t;atm_aal5mux_t1000,38781
typedef	struct	atm_aal5snap_ atm_aal5snap_1002,38799
} atm_aal5snap_t;atm_aal5snap_t1006,38940
typedef struct  atm_nlpid_snap atm_nlpid_snap1008,38959
} atm_nlpid_snap_t;atm_nlpid_snap_t1013,39172
typedef struct  atm_aal5nlpid_ atm_aal5nlpid_1015,39193
} atm_aal5nlpid_t;atm_aal5nlpid_t1019,39325
typedef struct  atm_aal5frnlpid_ atm_aal5frnlpid_1021,39345
} atm_aal5frnlpid_t;atm_aal5frnlpid_t1026,39517
typedef struct  atm_bridge_ether_hdr_ atm_bridge_ether_hdr_1028,39539
} atm_bridge_ether_hdr;atm_bridge_ether_hdr1035,39857
typedef struct  atm_bridge_other_hdr_ atm_bridge_other_hdr_1037,39882
} atm_bridge_other_hdr;atm_bridge_other_hdr1048,40372
typedef struct atm_aal5lane_ atm_aal5lane_1050,40397
} atm_aal5lane_t;atm_aal5lane_t1054,40494
typedef struct atm_aal5_cspdu_trail_ atm_aal5_cspdu_trail_1056,40513
} atm_aal5_cspdu_trail_t;atm_aal5_cspdu_trail_t1061,40733
#define	ATM_ENCAP_HDRBYTES	1063,40760
#define	ATM_MUX_HDRBYTES	1064,40809
#define	ATM_NLPID_HDRBYTES	1065,40836
#define	ATM_FRNLPID_HDRBYTES	1066,40910
#define	ATM_SAP_HDRBYTES	1067,40986
#define	ATM_SNAP_SAP_ADJBYTES	1068,41013
#define	ATM_SNAP_HDRBYTES	1069,41080
#define	ATM_SMDS_L3_HDRBYTES	1070,41120
#define	ATM_SMDS_HDRBYTES	1071,41168
#define	ATM_NLPID_SNAP_HDRBYTES	1072,41233
#define	ATM_AAL5_CSPDU_TRLBYTES	1073,41292
#define ATM_BRIDGE_ETHER_ENCAPBYTES 1075,41358
#define ATM_BRIDGE_OTHER_ENCAPBYTES 1076,41425
#define ATM_BRIDGE_ETHER_ENCAPWORDS 1077,41492
#define ATM_BRIDGE_OTHER_ENCAPWORDS 1078,41564
#define ATM_BRIDGE_PID_OFFSET_BYTE 1079,41636
#define	ATM_SNAP_ENCAPBYTES	1081,41709
#define	ATM_SAP_ENCAPBYTES	1082,41778
#define	ATM_MUX_ENCAPBYTES	1083,41845
#define	ATM_SMDS_ENCAPBYTES	1084,41912
#define ATM_NLPID_ENCAPBYTES 1085,41981
#define	ATM_FRNLPID_ENCAPBYTES	1086,42055
#define	ATM_MAX_ENCAPBYTES	1087,42107
#define ATM_LANE_LECID_ENCAPBYTES	1093,42307
#define ATM_LANE_CONTROL_ENCAPBYTES	1094,42356
#define ATM_LANE_DATA_ENCAPBYTES	1095,42411
#define ATM_AAL5_LANE_ENCAPBYTES 1099,42534
#define ATM_AAL5_LANE_ENCAPWORDS 1102,42668
#define ATM_NLPID_SNAP_ENCAPBYTES 1105,42741
#define atm_snapvc	1107,42803
#define	atm_nlpidvc	1108,42858
#define	atm_frnlpidvc	1109,42915
#define	atm_smdsvc	1110,42976
#define	atm_muxvc	1111,43031
#define	ATM_NLPID_CTL	1116,43139
#define	ATM_NLPID_PID_IP	1117,43191
#define	ATM_NLPID_PID_SNAP1	1118,43221
#define	ATM_NLPID_PID_SNAP0	1119,43254
#define	ATM_NLPID_PID_CLNP	1120,43287
#define	ATM_NLPID_PID_ESIS	1121,43319
#define	ATM_NLPID_PID_ISIS	1122,43351
#define	ATM_NLPID_PID_ISO	1123,43383
#define ATM_NLPID_PID_Q2931 1124,43414
#define ATM_NLPID_IETF_IP 1126,43452
#define ATM_NLPID_SNAP1PAD 1127,43526
#define ATM_NLPID_SNAP0PAD 1128,43603
#define ATM_NLPID_CLNP 1129,43680
#define ATM_NLPID_ESIS 1130,43756
#define ATM_NLPID_ISIS 1131,43832
#define ATM_NLPID_ISO 1132,43908
#define ATM_NLPID_Q2931 1133,43983
#define ATM_NLPID_DEF(ATM_NLPID_DEF1135,44061
#define	ATM_OUTFAST(ATM_OUTFAST1140,44174
#define	ATM_INFAST(ATM_INFAST1145,44316
#define	ATM_VCINFAST(ATM_VCINFAST1150,44453
#define	ATM_VCOUTFAST(ATM_VCOUTFAST1160,44796
#define	ATM_INAS(ATM_INAS1165,44967
#define ATM_OUTAS(ATM_OUTAS1178,45435
atm_in_inc(1203,46375
atm_out_inc(1211,46508
wavl_to_vcinfo 1223,46779
wavl_to_vcinfo_void 1234,47044
atm_is_glommable_vc(1245,47323
atm_getvc_inline 1263,47682
static inline boolean  is_atm_ds3 1282,47998
static inline boolean  is_atm_e3 1287,48119
static inline boolean  is_atm_sonet 1292,48238
