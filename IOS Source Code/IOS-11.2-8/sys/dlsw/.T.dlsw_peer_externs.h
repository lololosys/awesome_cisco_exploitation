
dlsw_peer_externs.h,4346
#define _DLSW_PEER_EXTERNS_H_126,4771
#define CHECK_ON(CHECK_ON145,5127
#define TURN_ON(TURN_ON146,5203
#define TURN_OFF(TURN_OFF147,5274
#define PEER_UNKNOWN 152,5395
#define PEER_CISCO 153,5430
#define PEER_NOT_CISCO 154,5465
#define DLSW_UNKNOWN_PGROUP 159,5534
#define DLSW_MIN_PGROUP 160,5576
#define DLSW_MAX_PGROUP 161,5618
#define DLSW_DEF_KEEP_INT 162,5660
#define DLSW_KEEP_CNT_THOLD 163,5712
#define MAX_PORTS_IN_LIST 164,5754
#define MAX_RINGS_IN_LIST 165,5796
#define MAX_BGROUPS_IN_LIST 166,5838
#define MAX_SAPS_IN_LIST 167,5880
#define DLSW_DEF_CONNECT_INT 168,5922
#define DLSW_POD_DEF_DISC_DELAY 169,5974
#define DLSW_POD_MAX_DISC_DELAY 170,6053
#define DLSW_POD_CONN_WAIT 171,6130
#define DLSW_CONN_WINDOW 172,6182
#define DLSW_MAX_TCP_QLEN 173,6234
#define DLSW_TCP_HI_WATER_PERCENT 174,6276
#define DLSW_TCP_LO_WATER_PERCENT 175,6318
#define DLSW_PEER_CONG_INCREASE 181,6460
#define DLSW_PEER_CONG_LOW 182,6503
#define DLSW_PEER_CONG_MEDIUM 183,6541
#define DLSW_PEER_CONG_HIGH 184,6579
#define NORMAL_MODE 189,6649
#define PASSIVE_MODE 190,6686
#define DLSW_UNKNOWN_PCOST 195,6744
#define DLSW_MIN_PCOST 196,6781
#define DLSW_NORM_PCOST 197,6818
#define DLSW_MAX_PCOST 198,6855
#define DLSW_FC_INIT_WINDOW 204,6961
#define DLSW_FC_MAX_WINDOW 205,6993
#define PEER_RECV_PAK 212,7156
#define PEER_KILL_HANDLE 213,7196
#define PEER_PUT 218,7276
#define PEER_PUT_BCAST 219,7316
#define PEER_PUT_GROUP 220,7356
#define PEER_PUT_POD_RSP 221,7396
typedef struct _port_list_entry _port_list_entry227,7485
} portlistentry;portlistentry236,7770
typedef struct _port_list_entry ringlistentry;ringlistentry239,7847
typedef struct _macentry_t _macentry_t242,7946
} macentry_t;macentry_t246,8065
typedef struct _nbentry_t _nbentry_t248,8080
} nbentry_t;nbentry_t254,8289
typedef enum _core2peer_ret_t _core2peer_ret_t269,8633
} core2peer_ret_t;core2peer_ret_t278,8841
#define PEER_PASS 283,8909
#define PEER_SAP_FAIL 284,8944
#define PEER_EXCL_FAIL 285,8979
#define PEER_MAC_FAIL 286,9014
#define PEER_NBNAME_FAIL 287,9049
#define PEER_INVALID_PEER 292,9128
#define PEER_WILL_START 293,9164
#define PEER_STARTED 294,9200
#define DLSW_MAX_PEER_ID 300,9272
typedef ulong peerhandle_t;peerhandle_t306,9372
#define PEER_SSP_TYPE 323,9944
#define PEER_DLX_TYPE 324,9980
#define PEER_INFO_FRAME 325,10054
#define PEER_SNA_BCAST 326,10120
#define PEER_NETBIOS_UI 327,10188
#define PEER_CKT_START 328,10256
#define FILTER_ME 333,10363
#define PRIORITIZE_ME 334,10441
typedef enum _peer_pak_cat _peer_pak_cat339,10556
} peer_pak_cat;peer_pak_cat347,11044
typedef struct _pod_rsp_info_t _pod_rsp_info_t349,11061
} pod_rsp_info_t;pod_rsp_info_t352,11145
typedef struct _peer_put_t _peer_put_t363,11509
} peer_put_t;peer_put_t378,12008
#define put_handle 380,12023
#define put_group 381,12068
#define pod_dipaddr 382,12112
#define pod_dgroup 383,12166
typedef struct _pod_conn_t _pod_conn_t385,12220
} pod_conn_t;pod_conn_t388,12288
#define PEER_RECV_PAK 402,12625
#define PEER_KILL_HANDLE 403,12665
#define PEER_FLOW_CTRL_INDICATION 404,12705
#define PEER_DEL_CACHE 405,12745
typedef enum _peer2core_ret_t _peer2core_ret_t410,12822
} peer2core_ret_t;peer2core_ret_t413,12897
typedef struct _peer_recv_t _peer_recv_t415,12917
} peer_recv_t;peer_recv_t420,13152
#define sender_handle 422,13168
#define sender_ipaddr 423,13211
typedef struct _peer_kill_handle_t _peer_kill_handle_t425,13250
} peer_kill_handle_t;peer_kill_handle_t428,13333
typedef struct _peer_del_cache_t _peer_del_cache_t430,13356
} peer_del_cache_t;peer_del_cache_t433,13439
typedef struct _peer_new_cap_t _peer_new_cap_t435,13460
} peer_new_cap_t;peer_new_cap_t438,13539
typedef struct _peer_flow_ind_t _peer_flow_ind_t440,13558
} peer_flow_ind_t;peer_flow_ind_t444,13713
typedef union _peer_msg_u _peer_msg_u457,14046
} peer_msg_u;peer_msg_u466,14302
#define PEER_CSM_ACCEPT_CHECK 477,14556
typedef struct _csm_accept_t _csm_accept_t479,14596
} csm_accept_t;csm_accept_t482,14672
typedef union _peer_filter_msg_u _peer_filter_msg_u488,14796
} peer_filter_msg_u;peer_filter_msg_u491,14917
#define DLSW_FR_SAP 593,18407
#define FR_REEXPLORE 594,18446
#define FR_DISCONN 595,18482
#define FR_OPEN 596,18504
#define FR_CONN 597,18526
