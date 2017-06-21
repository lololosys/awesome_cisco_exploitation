
dlsw_csm_externs.h,1974
#define __DLSW_CSM_EXTERNS_H__113,4182
#define BOX_MTU_SIZE	127,4458
typedef struct _dlsw_bgroup_t _dlsw_bgroup_t155,5222
} dlsw_bgroup_t;dlsw_bgroup_t173,6442
typedef struct _dlsw_ring_t _dlsw_ring_t180,6573
} dlsw_ring_t;dlsw_ring_t193,7180
typedef struct _dlsw_sap_t _dlsw_sap_t197,7226
} dlsw_sap_t;dlsw_sap_t204,7473
#define LCACHE_MAC_FLAG 217,7678
#define RCACHE_MAC_FLAG 218,7705
#define LCACHE_NAME_FLAG 219,7732
#define RCACHE_NAME_FLAG 220,7759
#define FUNC_ADDR_BIT 225,7805
#define LOCAL 226,7836
#define REMOTE 227,7864
#define MAX_COST 228,7892
#define FORCE 229,7921
#define NO_FORCE 230,7952
    } cache_source_t;cache_source_t235,8041
    } exclusivity_t;exclusivity_t240,8112
    } cache_action_t;cache_action_t245,8176
    } reach_sense;reach_sense250,8251
    } resource_type_t;resource_type_t255,8319
    } object_type_t;object_type_t267,8508
typedef dlsw_ring_t* lportid_t;lportid_t269,8530
    } config_obj_u;config_obj_u280,8826
typedef union u_portid_type_ u_portid_type_282,8847
    } u_portid_type_t;u_portid_type_t286,8956
typedef struct reach_entry_ reach_entry_288,8980
    } reach_entry_t;reach_entry_t295,9227
typedef union u_msg_ptr_ u_msg_ptr_297,9249
    } u_msg_ptr_t;u_msg_ptr_t300,9325
    } csm_op_t;csm_op_t314,9575
    } csm_rc_t;csm_rc_t323,9721
typedef struct csm_info_ csm_info_325,9739
} csm_info_t;csm_info_t336,10094
#define CSM_RMAC_CACHE 357,10834
#define CSM_LMAC_CACHE 358,10864
#define CSM_RNAME_CACHE 359,10894
#define CSM_LNAME_CACHE 360,10924
static inline boolean dlsw_ignore_port_lf_type 445,14722
static inline boolean dlsw_ignore_port_lf 464,15040
#define NO_LOCAL_SWITCH 471,15181
#define LOCAL_SWITCH 472,15218
#define NO_REMOTE 473,15266
#define REMOTE_SWITCH 474,15303
#define NO_NETBIOS 475,15345
#define NETBIOS_SUPPORT 476,15382
static inline boolean dlsw_can_switch_by_type 727,22739
static inline boolean dlsw_can_switch 738,23029
