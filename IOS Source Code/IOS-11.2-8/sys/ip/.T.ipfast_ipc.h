
ipfast_ipc.h,4519
#define MAX_IP_REWRITE 117,4427
typedef struct ipc_ipfast_msg_hdr_ ipc_ipfast_msg_hdr_119,4455
} ipc_ipfast_msg_hdr;ipc_ipfast_msg_hdr122,4535
typedef struct ipc_dfs_add_msg_ ipc_dfs_add_msg_124,4558
} ipc_ipfast_add_msg;ipc_ipfast_add_msg142,5114
typedef struct ipc_ipfast_update_idb_msg_ ipc_ipfast_update_idb_msg_147,5210
} ipc_ipfast_update_idb_msg;ipc_ipfast_update_idb_msg152,5348
typedef struct ipc_ipfast_hwindex_elt_ ipc_ipfast_hwindex_elt_157,5459
}ipc_ipfast_hwindex_elt;ipc_ipfast_hwindex_elt162,5579
typedef struct ipc_ipfast_update_hwindex_msg_ ipc_ipfast_update_hwindex_msg_164,5605
} ipc_ipfast_update_hwindex_msg;ipc_ipfast_update_hwindex_msg168,5792
} ipc_ipfast_max_add_msg;ipc_ipfast_max_add_msg176,5990
typedef struct ipc_ipfast_inv_msg_ ipc_ipfast_inv_msg_178,6017
} ipc_ipfast_inv_msg;ipc_ipfast_inv_msg182,6128
typedef struct ipc_ipfast_stat_msg_ ipc_ipfast_stat_msg_184,6151
} ipc_ipfast_stat_msg;ipc_ipfast_stat_msg188,6318
typedef struct ipc_ipfast_dfs_cfgreq_msg_ ipc_ipfast_dfs_cfgreq_msg_190,6342
} ipc_ipfast_dfs_cfgreq_msg;ipc_ipfast_dfs_cfgreq_msg193,6452
typedef struct ipc_ipfast_stat_unit_ ipc_ipfast_stat_unit_199,6606
} ipc_ipfast_stat_unit;ipc_ipfast_stat_unit207,6790
typedef struct ipc_ipfast_en_dis_msg_ ipc_ipfast_en_dis_msg_209,6815
} ipc_ipfast_en_dis_msg;ipc_ipfast_en_dis_msg213,6931
typedef struct ipc_ipfast_acc_fasthash_ ipc_ipfast_acc_fasthash_215,6957
} ipc_ipfast_acc_fasthash;ipc_ipfast_acc_fasthash220,7189
typedef struct  ipc_ipfast_acc_fastitem_ ipc_ipfast_acc_fastitem_222,7218
} ipc_ipfast_acc_fastitem;ipc_ipfast_acc_fastitem228,7514
typedef struct ipc_ipfast_acc_slowtype_ ipc_ipfast_acc_slowtype_230,7543
} ipc_ipfast_acc_slowtype;ipc_ipfast_acc_slowtype255,8978
typedef struct ipc_ipfast_cfgreq_msg_ ipc_ipfast_cfgreq_msg_257,9006
} ipc_ipfast_cfgreq_msg;ipc_ipfast_cfgreq_msg261,9122
typedef struct ipc_ipfast_acl_ena_msg_ ipc_ipfast_acl_ena_msg_263,9148
} ipc_ipfast_acl_ena_msg;ipc_ipfast_acl_ena_msg267,9266
typedef struct ipc_ipfast_acl_inout_ ipc_ipfast_acl_inout_269,9293
} ipc_ipfast_acl_inout;ipc_ipfast_acl_inout275,9455
#define	IPFAST_UPDATE_PREFIX	280,9512
#define	IPFAST_INVALIDATE_PREFIX_IF	281,9545
#define	IPFAST_INVALIDATE_PREFIX_PR	282,9584
#define	IPFAST_INVALIDATE_PREFIX_ALL	283,9623
#define	IPFAST_STATS_UPLOAD	284,9663
#define IPFAST_ENABLE_RC_DIST 285,9695
#define IPFAST_DISABLE_RC_DIST 286,9745
#define IPFAST_ACCESS_ADD_FASTHASH	287,9795
#define IPFAST_ACCESS_DEL_FASTHASH	288,9833
#define IPFAST_ACCESS_ADD_FASTITEM	289,9871
#define IPFAST_ACCESS_DEL_FASTITEM	290,9910
#define IPFAST_ACCESS_ADD_SLOWITEM 291,9949
#define IPFAST_ACCESS_DEL_SLOWITEM 292,10000
#define IPFAST_CFG_DFS_REQUEST	293,10051
#define IPFAST_CFG_ACL_REQUEST	294,10087
#define IPFAST_IP_ACL_INTFACE	295,10123
#define IPFAST_CRYPTO_ENGINE_VIP_UPD_IDB 296,10158
#define IPFAST_CRYPTO_ENGINE_VIP_KEY_REQUEST 297,10209
#define IPFAST_CRYPTO_ENGINE_VIP_UPD_MAP 298,10260
#define IPFAST_UPDATE_IDB	299,10311
#define IPFAST_UPDATE_INDEX	300,10342
#define IPFAST_RSP_STATISTICS_NAME	307,10422
#define IPFAST_RSP_CONFIG_NAME	308,10486
#define IPFAST_VIP_SLOT0_CTRL_NAME	313,10582
#define IPFAST_VIP_SLOT1_CTRL_NAME	314,10642
#define IPFAST_VIP_SLOT2_CTRL_NAME	315,10702
#define IPFAST_VIP_SLOT3_CTRL_NAME	316,10762
#define IPFAST_VIP_SLOT4_CTRL_NAME	317,10822
#define IPFAST_VIP_SLOT5_CTRL_NAME	318,10882
#define IPFAST_VIP_SLOT6_CTRL_NAME	319,10942
#define IPFAST_VIP_SLOT7_CTRL_NAME	320,11002
#define IPFAST_VIP_SLOT8_CTRL_NAME	321,11062
#define IPFAST_VIP_SLOT9_CTRL_NAME	322,11122
#define IPFAST_VIP_SLOT10_CTRL_NAME 323,11182
#define IPFAST_VIP_SLOT11_CTRL_NAME 324,11248
#define IPFAST_VIP_SLOT12_CTRL_NAME 325,11314
#define IPFAST_VIP_SLOT0_CTRL_PNAME 330,11417
#define IPFAST_VIP_SLOT1_CTRL_PNAME 331,11482
#define IPFAST_VIP_SLOT2_CTRL_PNAME 332,11547
#define IPFAST_VIP_SLOT3_CTRL_PNAME 333,11612
#define IPFAST_VIP_SLOT4_CTRL_PNAME 334,11677
#define IPFAST_VIP_SLOT5_CTRL_PNAME 335,11742
#define IPFAST_VIP_SLOT6_CTRL_PNAME 336,11807
#define IPFAST_VIP_SLOT7_CTRL_PNAME 337,11872
#define IPFAST_VIP_SLOT8_CTRL_PNAME 338,11937
#define IPFAST_VIP_SLOT9_CTRL_PNAME 339,12002
#define IPFAST_VIP_SLOT10_CTRL_PNAME 340,12067
#define IPFAST_VIP_SLOT11_CTRL_PNAME 341,12133
#define IPFAST_VIP_SLOT12_CTRL_PNAME 342,12199
#define DFS_MAX_PORT_NAME	344,12266
#define	MAX_SLOTS	346,12313
static inline uchar * generate_ds_ctrl_port_name 383,13348
