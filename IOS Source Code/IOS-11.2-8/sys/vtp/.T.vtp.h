
vtp.h,2887
#define __VTP_H__264,9596
#define FALSE 271,9667
#define TRUE 272,9684
#define NULL 273,9701
typedef unsigned char boolean;boolean274,9727
#define EXACT 279,9848
#define NEXT 283,9888
#define VTP_SOURCE_ADVERT 286,9915
#define VTP_SOURCE_EXTERN 287,9943
typedef struct list list290,10026
} dlist, *dlist_ptr;dlist_ptr293,10094
#define LIST_INIT(LIST_INIT295,10116
#define LIST_INSERT(LIST_INSERT297,10180
#define LIST_DELETE(LIST_DELETE300,10297
#define LIST_EMPTY(LIST_EMPTY307,10436
#define NAME_EQUAL 310,10542
#define NAME_LESS 311,10573
#define NAME_GREATER 312,10604
#define MAX_VTP_TX_QUEUE_SIZE 315,10637
} buff_chain, *buff_chain_ptr;buff_chain_ptr322,10779
#define VLAN_ETHER 330,11152
#define VLAN_FDDI 331,11179
#define VLAN_TR 332,11206
#define VLAN_FDDI_NET 333,11233
#define VLAN_TR_NET 334,11260
#define VLAN_OPERATIONAL 336,11359
#define VLAN_SUSPENDED 338,11450
#define VLAN_MTU_TOO_BIG_FOR_DEVICE 340,11536
#define VLAN_MTU_TOO_BIG_FOR_TRUNK 341,11593
#define OLD_VLAN 345,11842
#define NEW_VLAN 346,11869
#define MODIFIED_VLAN 347,11896
#define ENT_STP_SRT 355,12273
#define ENT_STP_SRB 356,12303
} vlan_info_ent, *vlan_info_ent_ptr;vlan_info_ent_ptr366,12736
#define VTP_VERSION 371,12834
#define VTP_MAX_STORAGE 372,12869
#define VTP_SUMMARY_ADV 373,12904
#define VTP_SUBSET_ADV 374,12939
#define VTP_ADV_RQST 375,12974
#define VTP_MAX_VLAN_INFO_SIZE 378,13050
#define PKT_OPERATIONAL 382,13212
#define PKT_SUSPENDED 383,13244
} vlan_info, *vlan_info_ptr;vlan_info_ptr392,13690
#define TAG_SR_RING 397,13821
#define TAG_SR_BRIDGE 398,13854
#define TAG_STP_TYPE 399,13887
#define TAG_PARENT_VLAN 400,13920
#define TAG_TB_VLAN 401,13953
} vlan_tlv, *vlan_tlv_ptr;vlan_tlv_ptr404,14106
} adv_header, *adv_header_ptr;adv_header_ptr414,14557
} subset_adv, *subset_adv_ptr;subset_adv_ptr430,15184
} adv_request, *adv_request_ptr;adv_request_ptr440,15692
#define VTP_MAX_CONFIG_INFO_SIZE 443,15783
} summary_adv, *summary_adv_ptr;summary_adv_ptr454,16473
#define VTP_FLOOD_INTERVAL 456,16507
#define MAX_VTP_UPDATE_PENDING_TIME 457,16569
#define VTP_FLOOD_JITTERED(VTP_FLOOD_JITTERED462,16734
typedef struct vtp_stats_ vtp_stats_464,16784
} vtp_stats;vtp_stats473,17405
#define UPDATE_RXMT_REQUESTED 511,19123
} vlan_domain, *vlan_domain_ptr;vlan_domain_ptr543,20744
#define DEFAULT_TRUNK_MTU 553,21151
#define SEND_ALL 562,21658
#define SEND_NONE 563,21683
#define RQST_ALL 565,21782
#define RQST_NONE 566,21807
#define VTP_MAX_RQST_RETRY_COUNT 568,21899
} vtp_trunk_itf, *vtp_trunk_itf_ptr;vtp_trunk_itf_ptr570,21988
#define VTP_BIG_REV 573,22069
#define VTP_REV_EQUAL(VTP_REV_EQUAL575,22183
#define ADJUST_LENGTH_LONG(ADJUST_LENGTH_LONG577,22221
#define VTP_TIMER_JITTERED_PERCENT 579,22295
#define VTP_TIMER_MAX_RANDOM_MS 580,22341
#define VTP_REQUEST_INTERVAL 581,22391
