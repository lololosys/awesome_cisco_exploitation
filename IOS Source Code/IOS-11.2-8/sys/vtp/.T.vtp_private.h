
vtp_private.h,4216
#define __VTP_PRIVATE_H__20,601
#define VTP_SOURCE_ADVERT 26,659
#define VTP_SOURCE_EXTERN 27,687
#define NAME_EQUAL 30,755
#define NAME_LESS 31,786
#define NAME_GREATER 32,817
#define MAX_VTP_TX_QUEUE_SIZE 35,850
#define MAX_VTP_LOCK_RETRY 36,891
#define VTP_MAC_ADDRESS_SIZE 37,932
#define VTP_VLAN_BASE	40,1006
#define MAX_NUM_VLANS	41,1031
#define VLAN_BYTE_STR_SIZ	42,1059
#define VLAN_BIT_STR_SIZ	43,1131
#define FIRST_VLAN_MOD	44,1201
#define LAST_VLAN_MOD	45,1269
} buff_chain, *buff_chain_ptr;buff_chain_ptr51,1445
typedef struct vlan_info_ent_ vlan_info_ent_54,1535
#define VLAN_ETHER VLAN_ETHER61,1870
#define VLAN_FDDI VLAN_FDDI62,1897
#define VLAN_TR VLAN_TR63,1924
#define VLAN_FDDI_NET VLAN_FDDI_NET64,1951
#define VLAN_TR_NET VLAN_TR_NET65,1978
#define VLAN_OPERATIONAL VLAN_OPERATIONAL67,2077
#define VLAN_SUSPENDED VLAN_SUSPENDED69,2168
#define VLAN_MTU_TOO_BIG_FOR_DEVICE VLAN_MTU_TOO_BIG_FOR_DEVICE71,2255
#define VLAN_MTU_TOO_BIG_FOR_TRUNK VLAN_MTU_TOO_BIG_FOR_TRUNK72,2312
#define OLD_VLAN OLD_VLAN76,2561
#define NEW_VLAN NEW_VLAN77,2588
#define MODIFIED_VLAN MODIFIED_VLAN78,2615
#define ENT_STP_SRT ENT_STP_SRT86,2999
#define ENT_STP_SRB ENT_STP_SRB87,3029
} vlan_info_ent, *vlan_info_ent_ptr;vlan_info_ent_ptr92,3246
#define VTP_VERSION 97,3344
#define VTP_STORAGE_UNKNOWN 98,3379
#define VTP_SUMMARY_ADV 99,3417
#define VTP_SUBSET_ADV 100,3452
#define VTP_ADV_RQST 101,3487
#define VTP_MAX_VLAN_INFO_SIZE 104,3563
#define VTP_MIN_VLAN_INFO_SIZE 105,3605
#define PKT_OPERATIONAL 109,3767
#define PKT_SUSPENDED 110,3799
} vlan_info, *vlan_info_ptr;vlan_info_ptr119,4245
#define TAG_SR_RING 124,4376
#define TAG_SR_BRIDGE 125,4409
#define TAG_STP_TYPE 126,4442
#define TAG_PARENT_VLAN 127,4475
#define TAG_TB_VLAN 128,4508
} vlan_tlv, *vlan_tlv_ptr;vlan_tlv_ptr131,4661
}   summary_pruning_tlv, *summary_pruning_tlv_ptr;summary_pruning_tlv_ptr138,4900
#define SUMMARY_PRUNING_TLV_SIZ 140,4952
#define TAG_PRUNING_SUPPORT 141,5020
#define TAG_PRUNING_SUPPORT_LEN 144,5125
} adv_header, *adv_header_ptr;adv_header_ptr157,5709
} subset_adv, *subset_adv_ptr;subset_adv_ptr173,6357
} adv_request, *adv_request_ptr;adv_request_ptr185,6903
#define VTP_MAX_CONFIG_INFO_SIZE 188,6994
} summary_adv, *summary_adv_ptr;summary_adv_ptr204,7777
#define VTP_ONE_SECOND 206,7811
#define VTP_FLOOD_INTERVAL 207,7853
#define MAX_VTP_UPDATE_PENDING_TIME 208,7915
#define VTP_FLOOD_JITTERED(VTP_FLOOD_JITTERED213,8081
typedef struct vtp_stats_ vtp_stats_215,8131
} vtp_stats;vtp_stats224,8752
#define DEF_VLAN_STATUS_SIZ 228,8789
typedef struct vtp_trunk_itf_ vtp_trunk_itf_231,8881
#define DEFAULT_TRUNK_MTU DEFAULT_TRUNK_MTU237,9237
#define SEND_ALL SEND_ALL246,9760
#define SEND_NONE SEND_NONE247,9785
#define RQST_ALL RQST_ALL249,9884
#define RQST_NONE RQST_NONE250,9909
#define VTP_MAX_RQST_RETRY_COUNT VTP_MAX_RQST_RETRY_COUNT252,10001
} vtp_trunk_itf, *vtp_trunk_itf_ptr;vtp_trunk_itf_ptr309,12824
typedef struct vlan_domain_ vlan_domain_313,12911
#define UPDATE_RXMT_REQUESTED UPDATE_RXMT_REQUESTED347,14594
} vlan_domain, *vlan_domain_ptr;vlan_domain_ptr405,16956
#define DOMAIN_STS_ADD_DOMAIN 409,17080
#define DOMAIN_STS_ADD_VLAN 410,17125
#define DOMAIN_STS_ADD_TRUNK 411,17170
#define vtp_pruning_not_complete(vtp_pruning_not_complete414,17290
#define set_vtp_tlv_pruning_mode(set_vtp_tlv_pruning_mode418,17481
#define clr_vtp_tlv_pruning_mode(clr_vtp_tlv_pruning_mode420,17582
#define get_vtp_tlv_pruning_mode(get_vtp_tlv_pruning_mode422,17681
#define set_vtp_tlv_pruning_ok(set_vtp_tlv_pruning_ok424,17756
#define clr_vtp_tlv_pruning_ok(clr_vtp_tlv_pruning_ok425,17828
#define is_vtp_tlv_pruning_ok(is_vtp_tlv_pruning_ok426,17897
#define TRUNK_STS_JOIN_BIT_CHG_1 431,18057
#define TRUNK_STS_JOIN_STATE_CHG 432,18138
#define TRUNK_STS_JOIN_TRIG_PERIOD 433,18209
#define DEF_VLAN_STATUS_SIZ 436,18340
#define VTP_BIG_REV 439,18424
#define VTP_REV_EQUAL(VTP_REV_EQUAL441,18542
#define ADJUST_LENGTH_LONG(ADJUST_LENGTH_LONG443,18580
#define VTP_TIMER_JITTERED_PERCENT 445,18648
#define VTP_TIMER_MAX_RANDOM_MS 446,18694
#define VTP_REQUEST_INTERVAL 447,18744
