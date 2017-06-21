
dlsw_cap_exchg.h,2890
#define _DLSW_CAP_EXCHG_H_47,1618
#define CAP_VENDOR_ID 52,1689
#define CAP_VERS_NUM 53,1723
#define CAP_IPW 54,1757
#define CAP_VERS_STRING 55,1791
#define CAP_MAC_EXCL 56,1825
#define CAP_SAP_LIST 57,1859
#define CAP_NUM_TCP 58,1893
#define CAP_NETBIOS_EXCL 59,1927
#define CAP_MAC_ADDR 60,1961
#define CAP_NB_NAME 61,1995
#define CAP_VENDOR_CNTXT 62,2029
#define CAP_LACK 68,2147
#define CAP_PRIORITY 69,2181
#define CAP_PEER_GRP 70,2215
#define CAP_BORDER 71,2249
#define CAP_COST 72,2283
#define CAP_CISCO_VERS 73,2317
#define CAP_SPOOF_AND_FRAG 74,2351
#define CAP_XID_OP_SUPPORT 75,2385
#define CAP_PEER_TYPE 76,2419
typedef enum _rsp_reason_t _rsp_reason_t81,2490
} rsp_reason_t;rsp_reason_t95,2947
#define AIW_CP_VERS 100,3012
#define AIW_CP_REL 101,3049
#define CISCO_VERSION_1 102,3087
#define CISCO_NUM_TCP_PIPES 104,3126
#define DLSW_DEF_NUM_TCP_PIPES 105,3163
#define NOT_EXCL_RESOURCES 106,3200
#define EXCL_RESOURCES 107,3237
typedef struct _cap_gds_t _cap_gds_t113,3320
} cap_gds_t;cap_gds_t116,3387
#define cap_pos_rsp_t 118,3401
typedef struct _cap_neg_rsp_t _cap_neg_rsp_t120,3434
} cap_neg_rsp_t;cap_neg_rsp_t124,3528
typedef struct _cv_hdr_t _cv_hdr_t130,3580
} cv_hdr_t;cv_hdr_t133,3643
typedef struct _cv_vendor_id_t _cv_vendor_id_t139,3697
} cv_vendor_id_t;cv_vendor_id_t142,3787
typedef struct _cv_vers_num_t _cv_vers_num_t144,3806
} cv_vers_num_t;cv_vers_num_t148,3900
typedef struct _cv_ipw_t _cv_ipw_t150,3918
} cv_ipw_t;cv_ipw_t153,3985
typedef struct _cv_sap_t _cv_sap_t155,3998
} cv_sap_t;cv_sap_t158,4082
typedef struct _cv_vstr_t _cv_vstr_t160,4095
} cv_vstr_t;cv_vstr_t163,4169
typedef struct _cv_tcp_t _cv_tcp_t165,4183
} cv_tcp_t;cv_tcp_t168,4254
typedef struct _cv_excl_t _cv_excl_t170,4267
} cv_excl_t;cv_excl_t173,4332
typedef struct _cv_mac_t _cv_mac_t175,4346
} cv_mac_t;cv_mac_t179,4438
typedef struct _cv_nbname_t _cv_nbname_t181,4451
} cv_nbname_t;cv_nbname_t184,4523
typedef struct _cv_vendor_ctxt_t _cv_vendor_ctxt_t186,4539
} cv_vctxt_t;cv_vctxt_t189,4631
typedef struct _cv_cisco_vers_t _cv_cisco_vers_t195,4688
} cv_cisco_vers_t;cv_cisco_vers_t198,4769
typedef struct _cv_lack_t _cv_lack_t200,4789
} cv_lack_t;cv_lack_t203,4854
typedef struct _cv_priority_t _cv_priority_t205,4868
} cv_pri_t;cv_pri_t208,4941
typedef struct _cv_peer_group_t _cv_peer_group_t210,4954
} cv_pgrp_t;cv_pgrp_t213,5027
typedef struct _cv_border_t _cv_border_t215,5041
} cv_border_t;cv_border_t218,5110
typedef struct _cv_cost_t _cv_cost_t220,5126
} cv_cost_t;cv_cost_t223,5191
typedef struct _cv_biu_seg_t _cv_biu_seg_t225,5205
} cv_biu_seg_t;cv_biu_seg_t228,5282
typedef struct _cv_xid_op_t _cv_xid_op_t230,5299
} cv_xid_op_t;cv_xid_op_t233,5376
typedef struct _cv_peer_type_t _cv_peer_type_t235,5392
} cv_peer_type_t;cv_peer_type_t238,5467
