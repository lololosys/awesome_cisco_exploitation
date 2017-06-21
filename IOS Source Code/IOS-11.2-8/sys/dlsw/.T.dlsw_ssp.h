
dlsw_ssp.h,4581
#define __SSP_H__50,1663
typedef struct _ssp_ckt_id _ssp_ckt_id54,1684
} ssp_ckt_id_t;ssp_ckt_id_t57,1757
typedef struct _ssp_ckt_transport_id _ssp_ckt_transport_id59,1774
} ssp_transport_id_t;ssp_transport_id_t63,1882
typedef struct _ssp_i_frame _ssp_i_frame65,1905
} ssp_i_frame_t;ssp_i_frame_t73,2109
typedef struct _ssp_dlc_id _ssp_dlc_id75,2127
} ssp_dlc_id_t;ssp_dlc_id_t80,2300
}ssp_llc_flags_t;ssp_llc_flags_t85,2380
typedef struct _ssp_control_frame _ssp_control_frame87,2399
} ssp_control_frame_t;ssp_control_frame_t107,2920
struct ssp_hdr_t_ ssp_hdr_t_111,2971
typedef struct _ssp_data_msg_t _ssp_data_msg_t120,3083
} ssp_info_t;ssp_info_t123,3158
typedef struct _ssp_ctl_msg_t _ssp_ctl_msg_t125,3173
} ssp_ctl_t;ssp_ctl_t128,3253
typedef struct _ssp_hdr_start_t _ssp_hdr_start_t134,3334
} ssp_hdr_start_t;ssp_hdr_start_t138,3474
#define ssp_vers	141,3495
#define ssp_hdr_len	142,3536
#define ssp_msg_len	143,3583
#define rem_dlc_corr	145,3632
#define rem_dlc_port_id	146,3689
#define ssp_msg_type	148,3753
#define ssp_fc	150,3801
#define ssp_prot_id	152,3847
#define ssp_hdr_no	154,3895
#define ssp_flags	156,3944
#define ssp_dlc_id	158,3989
#define target_mac	160,4031
#define origin_mac	161,4091
#define target_sap	162,4151
#define origin_sap	163,4208
#define ssp_dirn	167,4268
#define ssp_dlc_hdr_len	169,4317
#define ssp_lf	171,4372
#define ssp_priority	172,4422
#define origin_dlc_port_id	174,4472
#define origin_dlc_corr	175,4535
#define origin_transport_id	176,4593
#define target_dlc_port_id	178,4659
#define target_dlc_corr	179,4722
#define target_transport_id	180,4780
#define ssp_llc_cr	183,4847
#define ssp_llc_pf	184,4905
#define ssp_llc_flags	186,4967
#define SSP_CNTL_HDR_LEN 188,5021
#define SSP_INFO_HDR_LEN 189,5052
#define SSP_VERS 190,5083
#define SSP_OLD_VERS 191,5125
#define SSP_HDR_NUM	192,5166
#define SSP_DIRN_FORWARD 194,5193
#define SSP_DIRN_BACKWARD 195,5224
#define SSP_PROTOCOL_ID 197,5257
#define SSP_NB_DLC_HDR_LEN	198,5287
#define SSP_SNA_DLC_HDR_LEN	199,5319
#define SSP_PT_DLC_HDR_LEN	200,5352
#define SSP_FLAGS_EX	202,5415
#define SSP_LF_IGNORE_MASK 215,5696
#define SSP_LF_BASE_MASK 216,5733
#define SSP_LF_EXT_MASK 217,5770
#define SSP_LF_IGNORE_BIT 218,5807
#define SSP_LF516 220,5845
#define SSP_LF1500 221,5882
#define SSP_LF2052 222,5919
#define SSP_LF4472 223,5956
#define SSP_LF8144 224,5993
#define SSP_LF11407 225,6030
#define SSP_LF17800 226,6067
#define SSP_LFMAX 227,6104
#define SSP_FLOW_CONTROL_FCI 255,6917
#define SSP_FLOW_CONTROL_FCA 256,6984
#define SSP_FLOW_CONTROL_MASK 258,7047
#define SSP_FLOW_CONTROL_RWO 260,7085
#define SSP_FLOW_CONTROL_IWO 261,7152
#define SSP_FLOW_CONTROL_DWO 262,7222
#define SSP_FLOW_CONTROL_ZWO 263,7292
#define SSP_FLOW_CONTROL_HWO 264,7363
} ssp_priority_t;ssp_priority_t279,7695
#define DLSW_CKT_WINDOW_LOW_THRESHOLD(DLSW_CKT_WINDOW_LOW_THRESHOLD281,7714
#define is_ssp_fc_ind(is_ssp_fc_ind283,7787
#define is_ssp_fc_ack(is_ssp_fc_ack284,7849
#define is_ssp_fc_none(is_ssp_fc_none285,7911
#define get_ssp_fc_oper(get_ssp_fc_oper287,7985
#define SSP_PRIORITY_MASK 300,8252
#define SSP_PRIORITY_UNSPECIFIED 302,8292
#define SSP_PRIORITY_LOW 303,8331
#define SSP_PRIORITY_MEDIUM 304,8370
#define SSP_PRIORITY_HIGH 305,8409
#define SSP_PRIORITY_HIGHEST 306,8448
} ssp_op_t;ssp_op_t340,10429
#define SSP_FC_IND_MASK	342,10442
#define SSP_FC_ACK_MASK	343,10472
#define SSP_FC_OPER_MASK	344,10502
#define SSP_FC_OPER_SHIFT	346,10533
#define SSP_FC_OPER(SSP_FC_OPER348,10562
#define SSP_FC_REP_WINDOW	350,10638
#define SSP_FC_INC_WINDOW	351,10669
#define SSP_FC_DEC_WINDOW	352,10700
#define SSP_FC_RESET_WINDOW	353,10731
#define SSP_CAP_EX_ID 355,10765
#define SSP_CAP_POS_RSP_ID 356,10801
#define SSP_CAP_NEG_RSP_ID 357,10837
#define is_ssp_ctl_hdr(is_ssp_ctl_hdr363,10925
#define is_ssp_info_hdr(is_ssp_info_hdr380,11542
#define is_ssp_fc_ctr_op(is_ssp_fc_ctr_op384,11661
#define is_ssp_ifcm(is_ssp_ifcm388,11785
#define is_ssp_ctl_mac_hdr(is_ssp_ctl_mac_hdr391,11862
#define is_ssp_lf_valid(is_ssp_lf_valid398,12120
#define is_ssp_need_data(is_ssp_need_data404,12339
#define is_ssp_filter_priority(is_ssp_filter_priority413,12644
#define is_ssp_csm_op(is_ssp_csm_op419,12809
#define is_ssp_valid_op(is_ssp_valid_op428,13128
#define is_ssp_fc_op(is_ssp_fc_op433,13300
#define is_ssp_priority_op(is_ssp_priority_op446,13734
#define is_ssp_rem_id_op(is_ssp_rem_id_op451,13883
static inline boolean ssp_sanity_check_inline 458,14087
