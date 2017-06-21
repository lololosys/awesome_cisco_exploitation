
lane_private.h,4444
#define _LANE_PRIVATE_H_301,11563
#define LANE_PROTOCOL_ID	306,11637
#define LANE_PROTOCOL_VERSION	307,11668
#define LANE_CONFIG_REQ	312,11745
#define LANE_CONFIG_RSP	313,11778
#define LANE_JOIN_REQ	314,11811
#define LANE_JOIN_RSP	315,11842
#define LANE_READY_QUERY	316,11873
#define LANE_READY_IND	317,11906
#define LANE_REG_REQ	318,11938
#define LANE_REG_RSP	319,11968
#define LANE_UNREG_REQ	320,11998
#define LANE_UNREG_RSP	321,12030
#define LANE_ARP_REQ	322,12062
#define LANE_ARP_RSP	323,12092
#define LANE_FLUSH_REQ	324,12122
#define LANE_FLUSH_RSP	325,12154
#define LANE_NARP_REQ	326,12186
#define LANE_TOPO_CHANGE	327,12217
#define LANE_OPCODE_REQ_MASK	329,12251
#define LANE_OPCODE_VALID_REQ_MASK	330,12320
#define LANE_OPCODE_REQUEST	331,12393
#define LANE_OPCODE_RESPONSE	332,12429
#define LANE_OPCODE_OP_MASK	333,12466
#define LANE_STATUS_SUCCESS	339,12566
#define LANE_STATUS_VERSION_NOT_SUP	340,12597
#define LANE_STATUS_INVALID_REQ_PARMS	341,12635
#define LANE_STATUS_DUP_LAN_DEST	343,12725
#define LANE_STATUS_DUP_ATM_ADDR	344,12760
#define LANE_STATUS_INSUFF_RESOURCE	345,12795
#define LANE_STATUS_ACCESS_DENIED	346,12833
#define LANE_STATUS_INVALID_REQ_LECID	347,12869
#define LANE_STATUS_INVALID_LAN_DEST	348,12909
#define LANE_STATUS_INVALID_ATM_ADDR	349,12948
#define LANE_STATUS_NO_CONFIG	350,12988
#define LANE_STATUS_LE_CONFIG_ERROR	351,13022
#define LANE_STATUS_INSUFF_INFO	352,13061
#define LANE_STATUS_TIMEOUT 353,13097
#define LANE_TYPE_UNSPECIFIED	360,13248
#define LANE_TYPE_802_3	361,13284
#define LANE_TYPE_802_5	362,13315
#define LANE_TAG_NOT_PRESENT	370,13487
#define LANE_TAG_MAC_ADDRESS	371,13522
#define LANE_TAG_ROUTE_DESC	372,13557
#define LANE_MAX_FRAME_UNSPECIFIED	374,13592
#define LANE_MAX_FRAME_1516	375,13632
#define LANE_MAX_FRAME_4544	376,13666
#define LANE_MAX_FRAME_9234	377,13700
#define LANE_MAX_FRAME_18190	378,13734
} lane_max_frame_size_t;lane_max_frame_size_t389,14053
#define LANE_SDU_SIZE_1516	394,14130
#define LANE_SDU_SIZE_4544	395,14163
#define LANE_SDU_SIZE_9234	396,14196
#define LANE_SDU_SIZE_18190	397,14229
#define LANE_PROT_FLAG_PROXY	406,14462
#define	LANE_PROT_FLAG_REMOTE_ADDR	415,14834
#define LANE_PROT_FLAG_TOPO_CHANGE	422,15044
#define LANE_HASHLEN	427,15118
typedef struct lane_ctl_hdr_t_ lane_ctl_hdr_t_437,15445
} lane_ctl_hdr_t;lane_ctl_hdr_t446,15646
#define LANE_CTL_OPCODE_OFFSET 449,15700
#define LANE_CTL_FLAGS_OFFSET 454,15822
#define LANE_CTL_LECID_OFFSET 458,15933
typedef struct lane_dest_t_ lane_dest_t_466,16149
} lane_dest_t;lane_dest_t475,16357
typedef struct lane_cfg_pdu_t_ lane_cfg_pdu_t_480,16405
} lane_cfg_pdu_t;lane_cfg_pdu_t492,16771
typedef struct lane_join_pdu_t_ lane_join_pdu_t_500,16913
} lane_join_pdu_t;lane_join_pdu_t511,17325
typedef struct lane_reg_pdu_t_ lane_reg_pdu_t_516,17383
} lane_reg_pdu_t;lane_reg_pdu_t522,17591
typedef struct lane_arp_pdu_t_ lane_arp_pdu_t_527,17642
} lane_arp_pdu_t;lane_arp_pdu_t535,17930
#define LANE_ARP_TARGET_OFFSET 538,17988
typedef struct lane_topo_pdu_t_ lane_topo_pdu_t_543,18100
} lane_topo_pdu_t;lane_topo_pdu_t546,18188
typedef struct lane_ready_pdu_t_ lane_ready_pdu_t_551,18235
} lane_ready_pdu_t;lane_ready_pdu_t553,18294
typedef struct lane_flush_pdu_t_ lane_flush_pdu_t_558,18346
} lane_flush_pdu_t;lane_flush_pdu_t566,18636
typedef union lane_ctl_msg_u_ lane_ctl_msg_u_568,18657
} lane_ctl_msg_u;lane_ctl_msg_u577,18893
typedef struct lane_data_hdr_t_ lane_data_hdr_t_587,19102
} lane_data_hdr_t;lane_data_hdr_t593,19219
#define LANE_DATA_ID_ETHER_OFFSET 596,19276
#define LANE_DATA_ID_TR_OFFSET 597,19318
#define LANE_TLV_LEN_OFFSET 603,19423
#define LANE_TYPE_LEN_SIZE 604,19458
#define LANE_LOCAL_SEG_TLV_SIZE 605,19493
#define LANE_LOCAL_SEG_LENGTH 606,19528
#define LANE_LOCAL_SEG_TLV_ID 607,19563
} lane_event_code;lane_event_code621,19810
} lane_addr_recycle_reason;lane_addr_recycle_reason631,19977
#define _LECS_EVENT_DBG	640,20219
#define _LECS_PACKET_DBG	641,20246
#define _LECS_FINDER_DBG	642,20273
#define _LES_EVENT_DBG	643,20300
#define _BUS_EVENT_DBG	644,20326
#define _LEC_EVENT_DBG	645,20352
#define LECS_DBG(LECS_DBG650,20473
#define LECSP_DBG(LECSP_DBG657,20644
#define LECSF_DBG(LECSF_DBG664,20822
#define LES_DBG(LES_DBG671,20992
static inline int byteStreamCompare 686,21471
#define FOR_ALL_LANE_INFOS(FOR_ALL_LANE_INFOS723,22158
