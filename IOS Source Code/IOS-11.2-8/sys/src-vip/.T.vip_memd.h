
vip_memd.h,4058
#define __VIP_MEMD_H__135,4737
typedef struct vip_memd_ifcntl_t_ vip_memd_ifcntl_t;vip_memd_ifcntl_t142,4806
typedef volatile struct vip_bhdr_t_ vip_bhdr_t;vip_bhdr_t143,4859
#define VIP_BUFFERIN_OFFSET	156,5084
#define VIP_SP_BUFFERIN_SUPPORT	157,5152
#define BUFFER_HEADER_SPACE 159,5224
#define MALU_ATTN_CMD_DEPTH	160,5293
#define CCB_LENGTH 161,5353
#define BH_PARK_LIMIT	163,5407
#define VIP_DISABLE 168,5485
#define VIP_ENABLE 169,5511
#define VIP_QUIESCE 170,5537
#define VIP_UNQUIESCE 171,5563
#define VIP_QUIESCE_FLAG	176,5658
#define ADRSPC_CBUS_SIZE 192,6353
#define MEMD_CBUS_MASK 193,6425
static inline boolean is_memd_addr 195,6506
#define MEMD_CNTL_CCB	207,6756
#define MEMD_CNTL_SELECT	208,6804
#define MEMD_CNTL_MEMD_ADDR	209,6866
#define MEMD_CNTL_LAST_SYNC	210,6925
#define MEMD_CNTL_SWVER	211,6993
#define MEMD_CNTL_VPLDVER 212,7049
#define MEMD_CNTL_ROMVER 213,7125
#define MEMD_CNTL_NOQERR	214,7200
#define MEMD_CNTL_SYNC_COUNT	215,7258
#define MEMD_CNTL_MAX_LV_LTR	216,7317
#define MEMD_CNTL_PA_INT_MASK 217,7381
#define MEMD_CNTL_PASCB_TABLE	219,7446
#define MEMD_CNTL_IFTYPE	221,7508
#define MEMD_CNTL_RXCOUNT	224,7608
#define MEMD_CNTL_IFCARRIER	227,7704
#define MEMD_CMD_PASCB	230,7803
#define MEMD_CMD_CMDARG0	231,7851
#define MEMD_CMD_ARG1ARG2	232,7903
#define MEMD_CMD_ARG3	233,7960
#define MEMD_CMD_DRIVER_CMD	234,8007
#define MEMD_CMDQ_READ	236,8066
#define MEMD_CMDQ_WRITE	237,8113
#define MEMD_CMDQ_CMDQ	238,8162
typedef struct memd_cntl_t_ memd_cntl_t_246,8337
} memd_cntl_t;memd_cntl_t290,9873
typedef struct malu_cmd_t_ malu_cmd_t_295,9953
} malu_cmd_t;malu_cmd_t303,10118
typedef struct malu_q_t_ malu_q_t_305,10133
} malu_q_t;malu_q_t309,10250
typedef void (*cmd_dispatch_t)cmd_dispatch_t314,10304
typedef void (*cmd_dispatch_t)(malu_cmd_t *cmd)cmd314,10304
struct vip_bhdr_t_ vip_bhdr_t_319,10389
#define bf_rpsp_link	384,11582
#define bf_rsp_rxstatus	385,11619
#define bf_rsp_evnt_code	386,11659
#define bf_rpsp_txicb	388,11700
#define bf_rpsp_rxbytes	389,11738
#define bf_rsp_rxbytes	390,11778
#define bf_rsp_evnt_id	391,11817
#define bf_word0	393,11857
#define bf_rsp_txptr	394,11881
#define bf_rsp_evnt_code_and_id	395,11909
#define bf_rpsp_txstart 400,11981
#define bf_rpsp_rxsts 401,12026
#define bf_rsp_rxstatus2	402,12071
#define bf_rsp_txbytes	403,12111
#define bf_rsp_evnt_len	404,12150
#define bf_rpsp_txbytes 406,12191
#define bf_rsp_freeq	407,12236
#define bf_rsp_qptr	408,12273
#define bf_rsp_evnt_fq	409,12309
#define bf_word1 411,12349
#define bf_rsp_evnt_len_and_fq	412,12380
#define bf_rsp_txlen_and_fq	413,12417
#define bf_rpsp_rxicb	418,12485
#define bf_rpsp_rifsts	420,12524
#define bf_rsp_bcastid	421,12563
#define bf_word2	423,12603
#define bf_rpsp_pointer	428,12661
#define bf_rpsp_page	429,12701
#define bf_rpsp_ptr_page	431,12739
#define bf_rsp_bufptr	432,12770
#define bf_rsp_evnt_buf	433,12799
#define bf_word3	434,12830
} vip_rpsp_icb_t;vip_rpsp_icb_t466,13376
#define icb_rpsp_type 468,13396
#define icb_rpsp_freeq 469,13440
#define icb_rpsp_txque 470,13485
#define icb_rpsp_txque_word 471,13530
#define icb_rpsp_reserved0 472,13567
#define icb_rpsp_ipcque 473,13616
typedef struct vsys_vector_t_ vsys_vector_t_478,13696
} vsys_vector_t;vsys_vector_t557,16000
struct vip_memd_ifcntl_t_ vip_memd_ifcntl_t_562,16055
#define MAX_BUFFER_HEADERS	621,17510
#define MEMD_BHDR_ADDR_MASK 622,17543
#define MEMD_ADDR_MASK 623,17590
#define VIP_MEMD_STATIC_INLINES717,20957
static inline boolean is_cbus_quiesced 724,21068
static inline void set_cbus_quiesced 729,21169
static inline void clr_cbus_quiesced 734,21261
static inline void vip_pak_vector_init 739,21354
static inline vip_bhdr_t *vip_dequeue_bh_stack 752,21682
static inline void vip_enqueue_bh_stack 768,22026
static inline boolean vip_empty_bh_stack 778,22254
static inline void vip_rxbh_setup 787,22421
static inline vip_bhdr_t *vip_dfs_bh_allocate 831,23515
static inline boolean vip_rsp_memd_fastsend 847,24153
