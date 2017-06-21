
lec_private.h,3496
#define _LEC_PRIVATE_H_312,12165
#define LEC_PACKET_DEBUG	333,12611
#define LEC_SIGNAL_DEBUG	334,12675
#define LEC_STATE_DEBUG	335,12739
#define LEC_TOPO_DEBUG	336,12792
#define ISMULTICAST(ISMULTICAST341,12878
#define	C7_MINIMUM	348,13001
#define C7_DEFAULT	349,13033
#define C7_MAXIMUM	350,13066
#define C10_MINIMUM	353,13134
#define C10_DEFAULT	354,13157
#define C10_MAXIMUM	355,13180
#define C11_MINIMUM	358,13238
#define C11_DEFAULT	359,13270
#define C11_MAXIMUM	360,13302
#define C12_MINIMUM	363,13362
#define C12_DEFAULT	364,13394
#define C12_MAXIMUM	365,13429
#define C13_MINIMUM	368,13488
#define C13_DEFAULT	369,13512
#define C13_MAXIMUM	370,13535
#define C14_DEFAULT	373,13571
#define C17_MINIMUM	376,13612
#define C17_DEFAULT	377,13648
#define C17_MAXIMUM	378,13684
#define C18_MINIMUM	381,13746
#define C18_DEFAULT	382,13780
#define C18_MAXIMUM	383,13815
#define C20_MINIMUM	386,13878
#define C20_DEFAULT	387,13912
#define C20_MAXIMUM	388,13946
#define C21_MINIMUM	391,13998
#define C21_DEFAULT	392,14032
#define C21_MAXIMUM	393,14066
#define C22_MINIMUM 396,14147
#define C22_DEFAULT 397,14192
#define C22_MAXIMUM 398,14237
#define C25_DEFAULT	401,14327
#define C26_DEFAULT	402,14352
#define C28_MINIMUM	405,14408
#define C28_DEFAULT	406,14443
#define C28_MAXIMUM	407,14477
#define LEC_IDLE_TIMEOUT	413,14599
#define LEC_IDLE_TIMEOUT_MAX	414,14637
#define LEC_JITTER_PCT	416,14682
#define MAX_OWNER_STRING_LEN 422,14887
#define MAX_IF_NAME_LEN 424,14947
#define MAX_IF_DESCR_LEN 425,14982
typedef enum lec_timer_t_ lec_timer_t_430,15044
} lec_timer_t;lec_timer_t440,15318
typedef enum lec_call_state_t_ lec_call_state_t_442,15334
} lec_call_state_t;lec_call_state_t447,15462
typedef enum lec_ready_state_t_ lec_ready_state_t_449,15483
} lec_ready_state_t;lec_ready_state_t452,15553
typedef struct lec_vc_t_ lec_vc_t_454,15575
} lec_vc_t;lec_vc_t465,15861
typedef enum lec_mac_state_t_ lec_mac_state_t_471,15972
} lec_mac_state_t;lec_mac_state_t477,16109
typedef enum lec_arp_entry_type_t lec_arp_entry_type_t480,16130
} lec_arp_entry_type_t;lec_arp_entry_type_t484,16236
typedef struct lec_le_arp_t_ lec_le_arp_t_486,16261
} lec_le_arp_t;lec_le_arp_t502,16789
typedef enum lec_config_source_t_ lec_config_source_t_507,16878
} lec_config_source_t;lec_config_source_t511,17040
typedef struct lec_nsap_info_t_ lec_nsap_info_t_518,17243
} lec_nsap_info_t;lec_nsap_info_t525,17448
typedef struct lec_lecs_addr_t_ lec_lecs_addr_t_531,17541
} lec_lecs_addr_t;lec_lecs_addr_t534,17638
typedef struct lec_snmp_info_t_ lec_snmp_info_t_542,17815
} lec_snmp_info_t;lec_snmp_info_t604,20512
#define LEC_HSRP_MACS	610,20631
typedef enum lec_fail_reason_t_ lec_fail_reason_t_616,20762
} lec_fail_reason_t;lec_fail_reason_t646,21586
struct lec_info_t_ lec_info_t_654,21727
#define FOR_ALL_LEC(FOR_ALL_LEC806,25827
void lec_stop_timer 817,26158
void lec_start_timer 828,26354
void lec_start_arp_age_timer 844,26831
void lec_stop_arp_age_timer 861,27372
void lec_start_arp_control_timer 873,27627
void lec_stop_arp_control_timer 882,27869
void lec_start_flush_timer 893,28130
void lec_stop_flush_timer 903,28371
void lec_make_hwaddr 909,28493
boolean lec_dest_equal 921,28759
lec_info_t *lec_info_from_csb 943,29159
lec_info_t *lec_info_from_swidb 975,29700
lec_info_t *lec_info_from_vlanid 1000,30101
int lane_get_mcast_vcd 1018,30519
static inline boolean lec_mac_valid 1033,30804
