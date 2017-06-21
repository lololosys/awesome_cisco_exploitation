
fr_svc.h,15314
#define __FR_WATCHED_BOOL__86,3145
# define	IN 95,3297
# define 	OUT	96,3312
typedef void (*EVENTFUNC)EVENTFUNC102,3430
				uint, struct fr_svc_params fr_svc_params103,3507
typedef struct StateTable_ StateTable_110,3705
}   StateTable;StateTable113,3830
typedef void (*NLEVENTFUNC)NLEVENTFUNC118,3906
				uint, uint)uint119,3983
typedef struct NLStateTable_ NLStateTable_121,4001
}   NLStateTable;NLStateTable124,4130
# define FR_SVC_IN_CIR_DEF	132,4319
# define FR_SVC_OUT_CIR_DEF	133,4353
# define FR_SVC_MIN_IN_CIR_DEF	134,4388
# define FR_SVC_MIN_OUT_CIR_DEF	135,4426
# define FR_SVC_IN_CBS_DEF	136,4466
# define FR_SVC_OUT_CBS_DEF	137,4512
# define FR_SVC_IN_EBS_DEF	138,4559
# define FR_SVC_OUT_EBS_DEF	139,4605
# define FR_SVC_MAG_DEF	140,4652
# define FR_SVC_MULT_DEF	141,4699
# define FR_SVC_IDLE_TIMER	142,4741
# define FR_SVC_IN_FMIF	143,4783
# define FR_SVC_OUT_FMIF	144,4814
# define FR_SVC_DEF_PRIORITY	146,4846
# define FR_SVC_HOLDQ_SIZE	149,4925
# define T303_DEFAULT	155,5016
# define T305_DEFAULT	156,5050
# define T308_DEFAULT	157,5085
# define T309_DEFAULT	158,5119
# define T310_DEFAULT	159,5154
# define T322_DEFAULT	160,5189
# define T303_RETRY_COUNT	162,5224
# define T308_RETRY_COUNT	163,5253
# define T322_RETRY_COUNT	164,5282
# define NLI_DEF_Q_SIZE	169,5386
# define MAX_CALL_REF_PER_IDB 175,5531
# define MAX_CALL_ID_PER_IDB 176,5574
# define MAX_MAPS_PER_NLI	182,5775
# define MAX_ILLEGAL_IE_PER_MSG	184,5806
# define MAX_ADDRESS_LENGTH	191,5933
# define EXT_BIT_1	197,6065
# define EXT_BIT_0	198,6091
# define EXT_BIT_0_MASK	199,6117
# define  EXT_BIT_1_MASK	200,6148
typedef struct message_header_ message_header_205,6235
} message_header;message_header210,6440
# define PROT_DISC_IE_ID	217,6570
# define PROT_DISC_IE_LENGTH	218,6601
# define MAX_CALL_REF_PER_BOX	226,6856
# define CALL_REF_IE_ID	227,6894
# define CALL_REF_IE_LENGTH	228,6925
# define CALL_REF_TOTAL_LENGTH	229,6960
# define CALL_REF_FLAG_MASK	230,6997
# define FR_SVC_CALL_REF 231,7031
# define FR_SVC_PID 232,7076
# define T_ACTIVITY_DEF 233,7121
typedef struct call_reference_ call_reference_235,7174
} call_reference;call_reference239,7317
# define BEARER_CAPABILITY_IE_ID	251,7740
# define BEARER_CAP_IE_LENGTH 252,7778
# define BEARER_CAP_HDR_LENGTH 253,7812
# define BEARER_CAP_IE_TOTAL_LENGTH 254,7847
# define BEARER_CAP_BYTE3	255,7886
# define BEARER_CAP_BYTE4	256,7918
# define BEARER_CAP_BYTE5_A	257,7950
# define BEARER_CAP_BYTE5_B	258,7984
typedef struct bearer_capability_ bearer_capability_259,8018
} bearer_cap_ie;bearer_cap_ie265,8282
# define DLCI_IE_ID	277,8629
# define DLCI_IE_TOTAL_LENGTH 278,8656
# define DLCI_IE_HDR_LENGTH 279,8690
# define DLCI_IE_LENGTH 280,8722
# define DLCI_SPARE_BITS	281,8750
# define DLCI_LOW_BITS_MASK	282,8778
# define DLCI_HI_BITS_MASK	283,8814
# define DLCI_EXCLUSIVE	284,8849
# define DLCI_PREFERRED	285,8880
typedef struct dlci_ie_ dlci_ie_286,8911
} dlci_ie;dlci_ie291,9128
# define LINK_LAYER_CORE_PARAM_IE_ID	298,9302
# define LINK_CORE_IE_HDR_LENGTH	299,9344
# define LINK_CORE_IE_MAX_LENGTH	300,9379
# define CONNECT_LINK_CORE_IE_MAX_LENGTH	301,9415
# define LINK_CORE_IE_TOTAL_LENGTH	302,9459
# define MAX_FMIF_ID	303,9497
# define THROUGHPUT_ID	304,9525
# define MIN_ACCEP_THROUGHPUT_ID	305,9555
# define COMMITED_BURST_SIZE_ID	306,9593
# define EXCESS_BURST_SIZE_ID	307,9631
# define CBS_MAG_ID	308,9667
# define EBS_MAG_ID	309,9694
# define UPPER_MUL_BITS	310,9721
# define BIT_FIELD_SIZE	311,9754
# define MAG_LEFT_SHIFT	312,9782
# define LOWER_BITS_MASK	313,9810
# define UPPER_BITS_MASK	314,9843
typedef struct link_core_ie_ link_core_ie_316,9877
} link_core_ie;link_core_ie360,11626
typedef struct connect_link_core_ie_ connect_link_core_ie_366,11764
} connect_link_core_ie;connect_link_core_ie403,13224
# define CALLING_PARTY_NUMBER_ID	409,13378
# define CALLING_PARTY_IE_HDR_LENGTH	410,13416
# define TYPE_UNKNOWN_NUMBER	411,13455
# define TYPE_INTERNATIONAL_NUMBER	412,13489
# define NUMBER_IS_E164	413,13528
# define NUMBER_IS_X121	414,13559
# define NUMBER_TYPE_MASK	416,13591
# define NUMBER_PLAN_MASK	417,13623
# define PRESNT_IND_MASK	418,13655
# define SCREEN_IND_MASK	419,13686
# define MAX_ADDRESS_LENGTH	420,13717
typedef struct calling_party_number_ie_ calling_party_number_ie_422,13750
} calling_pty_number_ie;calling_pty_number_ie428,14048
# define CALLING_PARTY_SUBADDR_ID	434,14149
# define CALLING_PARTY_SUBADDR_HDR_LNGTH	435,14189
# define CALLING_PARTY_SUBADDR_LENGTH	436,14232
# define USER_SPECIFIED_SUBADDR	437,14273
typedef struct calling_party_subaddr_ie_ calling_party_subaddr_ie_439,14313
} calling_pty_subaddr_ie;calling_pty_subaddr_ie444,14537
# define CALLED_PARTY_NUMBER_ID	452,14733
# define CALLED_PARTY_HDR_LENGTH	453,14771
typedef struct called_party_ie_ called_party_ie_454,14806
} called_pty_ie;called_pty_ie459,15043
# define CALLED_PARTY_SUBADDR_ID	465,15135
# define CALLED_PARTY_SUBADDR_HDR_LNGTH	466,15173
typedef struct called_party_subaddr_ie_ called_party_subaddr_ie_467,15215
} called_pty_subaddr_ie;called_pty_subaddr_ie472,15440
# define CONN_PARTY_NUMBER_ID	482,15798
# define CONN_PARTY_IE_HDR_LENGTH	483,15835
typedef struct connected_party_ie_ connected_party_ie_484,15872
} connected_pty_ie connected_pty_ie490,16183
# define CONN_PARTY_NUMBER_SUBADDR_ID	498,16355
# define CONN_PARTY_SUBADDR_IE_HDR_LENGTH	499,16399
typedef struct connected_num_subaddr_ie_ connected_num_subaddr_ie_500,16443
} connected_num_subaddr_ie;connected_num_subaddr_ie505,16669
# define TRANSIT_NETWORK_IE_ID	514,16963
# define TRANSIT_NETWORK_HDR_LENGTH	515,17000
# define TRANSIT_NETWORK_IE_LENGTH	516,17038
# define USER_SPECIFIED_NETW	517,17075
# define NATIONAL_NETW	518,17110
# define INTERNATIONAL_NETW	519,17140
# define UNKNOWN_NET_ID_PLAN	521,17175
# define CARRIER_ID_PLAN	522,17210
# define DATA_NETW_ID_PLAN	523,17241
typedef struct transit_network_selection_ie_ transit_network_selection_ie_524,17274
} transit_network_selection_ie;transit_network_selection_ie529,17486
# define LOW_LAYER_COMPATIBILITY_IE_ID	540,17820
# define LOW_LAYER_COMP_IE_LENGTH 541,17864
# define LOW_LAYER_COMP_IE_HDR_LENGTH 542,17901
# define LOW_LAYER_COMP_IE_TOTAL_LENGTH 543,17942
# define LLC_IE_CODING_STNDRD	544,17984
# define LLC_BYTE3	545,18020
# define INFO_XFER_CAPABILITY	546,18046
# define FRAME_XFER_MODE	547,18082
# define LLC_IE_CCITT_Q22	548,18113
typedef struct low_layer_compatibility_ie_ low_layer_compatibility_ie_549,18145
} low_layer_compatibility_ie;low_layer_compatibility_ie555,18416
# define USER_USER_IE_ID	561,18549
# define USER_USER_IE_HDR_LENGTH	562,18580
# define USER_USER_IE_LENGTH	563,18615
typedef struct user_user_ie_ user_user_ie_564,18647
} user_user_ie;user_user_ie569,18839
# define FMIF_IE_ID	579,19192
# define FMIF_HDR_LENGTH	580,19218
# define FMIF_IE_LENGTH	581,19245
# define FMIF_UPPER_BITS	582,19272
# define FMIF_LOWER_BITS	583,19302
# define FMIF_UPPER_BITS_SHIFT	584,19332
typedef struct fmif_ie_ fmif_ie_586,19441
} fmif_ie;fmif_ie592,19587
# define THROUGHPUT_IE_ID	600,19798
# define THROUGHPUT_IE_HDR_LENGTH	601,19829
# define THROUGHPUT_IE_LENGTH	602,19865
# define THRGHPT_MULT_UPPER_BITS	603,19898
# define THRGHPT_MULT_LOWER_BITS	604,19936
# define THROUHPUT_MSB_BITS	605,19974
# define THRGHPT_MAG_MASK	606,20005
typedef struct throughput_ throughput_607,20037
} throughput;throughput613,20180
# define MIN_ACCEPTABLE_THROUGHPUT	622,20421
# define MIN_THROUGHPUT_IE_HDR_LENGTH	623,20461
# define MIN_THROUGHPUT_IE_LENGTH	624,20501
# define MAGNITUDE_MASK	625,20537
# define MULTIPLIER_MSB_MASK	626,20568
# define MULTIPLIER_LSB_MASK	627,20603
typedef struct min_accept_throughput_ie_ min_accept_throughput_ie_629,20639
} min_accept_throughput_ie;min_accept_throughput_ie635,20805
# define COMMITTED_BURST_SZ_IE_ID	643,21086
# define CBSZ_IE_HDR_LENGTH	644,21125
# define CBSZ_IE_LENGTH	645,21156
# define BURST_SIZE_MASK	646,21184
# define BURST_SIZE_SHIFT	647,21215
typedef struct committed_burst_size_ie_ committed_burst_size_ie_649,21245
} committed_burst_size_ie;committed_burst_size_ie655,21394
# define EXCESS_BURST_SZ_IE_ID	665,21696
# define EBSZ_IE_HDR_LENGTH	666,21733
# define EBSZ_IE_LENGTH	667,21764
typedef struct excess_burst_size_ie_ excess_burst_size_ie_669,21793
} excess_burst_size_ie;excess_burst_size_ie675,21941
typedef struct cbs_mag_ie_ cbs_mag_ie_677,21966
} cbs_mag_ie;cbs_mag_ie680,22035
typedef struct ebs_mag_ie_ ebs_mag_ie_682,22050
} ebs_mag_ie;ebs_mag_ie685,22119
typedef struct illegal_ie_ illegal_ie_691,22157
} illegal_ie;illegal_ie694,22223
typedef struct svc_setup_msg_ svc_setup_msg_700,22351
} svc_setup_msg;svc_setup_msg715,22913
typedef struct svc_call_proc_msg_ svc_call_proc_msg_721,23044
} svc_call_proc_msg;svc_call_proc_msg724,23152
typedef struct svc_connect_msg_ svc_connect_msg_730,23287
} svc_connect_msg;svc_connect_msg737,23577
# define CAUSE_IE_ID	747,23891
# define CAUSE_IE_HDR_LENGTH	748,23918
# define CAUSE_IE_LENGTH	749,23949
# define CODING_STD_LOCATION	750,23976
typedef struct cause_ie_ cause_ie_751,24010
} cause_ie;cause_ie757,24236
typedef struct svc_disconnect_msg_ svc_disconnect_msg_763,24362
} svc_disconnect_msg;svc_disconnect_msg766,24470
typedef struct svc_release_msg_ svc_release_msg_772,24606
} svc_release_msg;svc_release_msg775,24711
typedef struct svc_release_msg svc_rel_complete_msg;svc_rel_complete_msg777,24731
# define CALL_STATE_IE_ID	785,24927
# define CALL_STATE_IE_HDR_LENGTH	786,24959
typedef struct 	call_state_ie_ call_state_ie_787,24995
} call_state_ie;call_state_ie791,25077
typedef struct svc_status_msg_ svc_status_msg_797,25208
} svc_status_msg;svc_status_msg801,25349
typedef struct svc_status_enq_ svc_status_enq_807,25481
} svc_status_enq;svc_status_enq809,25553
# define L3_DBG	816,25693
# define L3CC_DBG	817,25743
# define L3IE_DBG	818,25794
# define NLI_DBG	819,25847
const typedef struct generic_def_ generic_def_825,26011
} generic_def;generic_def828,26077
enum nli_msgs nli_msgs833,26137
	NL_CALL_REQ 834,26153
	NL_CALL_IND,835,26172
	NL_CALL_CNF,836,26187
	NL_CALL_REJ,837,26201
	NL_REL_REQ,838,26216
	NL_REL_IND,839,26230
	NL_REL_CNF,840,26244
	NL_REL_REJ,841,26258
	L3_CALL_REL_REQ,842,26272
	L3_CALL_REQ,843,26291
	NL_CALL_ACK,844,26306
	NL_CALL_NACK,845,26321
	NL_MSG_UNKNOWN846,26337
enum nli_state nli_state852,26399
	STATE_NL_NULL,853,26416
	STATE_NL_CALL_REQ,854,26433
	STATE_NL_CALL_IND,855,26454
	STATE_NL_CALL_CNF,856,26475
	STATE_NL_CALL_REJ,857,26495
	STATE_NL_REL_REQ,858,26516
	STATE_NL_REL_IND,859,26536
	STATE_NL_REL_CNF,860,26556
	STATE_NL_REL_REJ,861,26576
	STATE_L3_CALL_REL_REQ,862,26595
	STATE_L3_CALL_REQ,863,26619
	STATE_NL_CALL_ACK,864,26639
	STATE_NL_CALL_NACK865,26659
enum svc_type svc_type871,26777
	PREFERRED 872,26793
	EXCLUSIVE873,26809
#define 	UNKNOWN_MSG	877,26825
# define MSG_CALL_PROCEEDING 912,28304
# define MSG_SETUP	913,28339
# define MSG_CONNECT 914,28364
# define MSG_DISCONNECT	915,28392
# define MSG_RELEASE	916,28422
# define MSG_RELEASE_COMPLETE	917,28449
# define MSG_STATUS_ENQUIRY	918,28484
# define MSG_STATUS	919,28517
# define MSG_UNKNOWN	920,28543
# define T303_TIMEOUT	930,28821
# define T305_TIMEOUT	931,28849
# define T308_TIMEOUT	932,28877
# define T310_TIMEOUT	933,28905
# define T322_TIMEOUT	934,28933
# define TIDL_TIMEOUT	935,28961
# define RELEASE_REQ	936,28989
# define DISCONNECT_REQ	937,29016
# define SPL_RELEASE_REQ	938,29046
# define SETUP_REQUEST	939,29076
# define REJECT_REQUEST	940,29105
# define SETUP_RESPONSE	941,29135
enum call_states call_states944,29167
	STATE_NULL 945,29186
	STATE_CALL_INITIATED 946,29226
	STATE_CALL_PROCEEDING 948,29318
	STATE_CALL_PRESENT 950,29419
	STATE_INCOMING_CALL_PROC 952,29516
	STATE_ACTIVE 956,29724
	STATE_DISCONNECT_REQUEST 957,29782
	STATE_DISCONNECT_INDICATION 959,29897
	STATE_RELEASE_REQUEST 962,30003
# define STATE_INC_CALL_PROC 966,30126
# define STATE_DISC_REQUEST 967,30180
# define STATE_DISC_IND 968,30233
# define NO_ERRORS 971,30287
# define IS_OKAY	972,30311
# define UNALLOC_NUMBER	973,30340
# define NO_ROUTE_TO_TRANSIT_NET	974,30368
# define NO_ROUTE_TO_DESTINATION	975,30403
# define CHANNEL_UNACCEPTABLE	976,30438
# define CALL_AWARDED_OVER_CHANNEL	977,30471
# define NORMAL_CALL_CLEARING	978,30508
# define USER_BUSY	979,30542
# define NO_USER_RESPONDING	980,30566
# define NO_ANSWER_FROM_USER	981,30598
# define CALL_REJECTED	982,30631
# define NUMBER_CHANGED	983,30659
# define NON_SELECTED_USER_CLEARING	984,30688
# define DEST_OUT_OF_ORDER	985,30727
# define INVALID_NUMBER_FORMAT	986,30758
# define FACILITY_REJECTED	987,30793
# define RESP_TO_STATUS_ENQ	988,30824
# define NORMAL_UNSPECIFIED	989,30856
# define NO_CIRCUIT_AVAILABLE	990,30888
# define NETWORK_OUT_OF_ORDER	991,30922
# define TEMPORARY_FAILURE	992,30956
# define SWITCH_CONGESTION	993,30987
# define ACCESS_INFO_DISCARDED	994,31018
# define REQ_CIRCUIT_NOT_AVAILABLE	995,31053
# define RESOURCE_UNAVAILABLE	996,31091
# define QOS_UNAVAILABLE	997,31125
# define REQ_FACILITY_NOT_SUBSCRIBED	998,31154
# define BEARER_CAP_NOT_AUTHORIZED	999,31194
# define BEARER_CAP_NOT_AVAILABLE	1000,31232
# define SERVICE_NOT_AVAILABLE	1001,31269
# define BEARER_CAP_NOT_IMPLEMENTED	1002,31304
# define CHANNEL_TYPE_NOT_IMPLEMENTED	1003,31343
# define REQ_FACILITY_NOT_IMPLEMENTED	1004,31384
# define ONLY_DIG_INFO_IS_AVAILABLE	1005,31425
# define SERVICE_OPTION_NOT_IMPLEMENTED	1006,31464
# define INVALID_CALL_REF_VALUE	1007,31507
# define CHANNEL_DOES_NOT_EXIST	1008,31543
# define CALL_ID_IN_USE	1009,31579
# define NO_CALL_SUSPENDED	1010,31608
# define CALL_HAS_BEEN_CLEARED	1011,31639
# define INCOMPATIBLE_DESTINATION	1012,31674
# define INVALID_TRANSIT_NET_SELECTION	1013,31711
# define INVALID_MESSAGE	1014,31753
# define MANDATORY_IE_MISSING	1015,31782
# define MESSAGE_TYPE_NOT_IMPLEMENTED	1016,31816
# define MESSAGE_INCOMPATIBLE_WITH_CALL	1017,31857
# define IE_NOT_IMPLEMENTED	1018,31900
# define INVALID_IE_CONTENTS	1019,31932
# define MSG_NOT_COMPATIBLE_CALL_STATE	1020,31966
# define RECOVERY_ON_TIMER_EXPIRY	1021,32009
# define PROT_ERROR_UNSPECIFIED	1022,32047
# define INTERWORKING_UNSPECIFIED	1023,32084
# define GLOBAL_CALL_REF	1032,32278
# define INVALID_CALL_REF_LENGTH	1033,32308
# define INVALID_PROT_DISC	1034,32345
# define INVALID_IE_TYPE	1035,32377
# define INVALID_IE_LENGTH	1036,32407
# define UNKNOWN_BEARER_CAP_BYTE3	1037,32439
# define UNKNOWN_BEARER_CAP_BYTE4	1038,32477
# define UNKNOWN_BEARER_CAP_BYTE5	1039,32515
# define NO_MEMORY_TO_ASSIGN	1040,32553
# define NO_FR_MEMORY	1041,32587
# define END_OF_IE	1042,32615
# define ILLEGAL_EXTENSION_BIT	1043,32640
# define INVALID_CONTENTS_IN_FIELD	1044,32676
# define IE_TOO_LONG	1045,32715
# define IE_TOO_SHORT	1046,32742
# define PROBLEM_WITH_CALL_PARAMETERS	1047,32770
# define PROBLEM_WITH_REGISTRY_LOOP	1049,32813
typedef struct error_message_def_ error_message_def_1051,32854
} error_message_def;error_message_def1056,32946
