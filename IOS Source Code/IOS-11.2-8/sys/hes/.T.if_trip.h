
if_trip.h,7019
#define __IF_TRIP_H__74,2804
#define FCI_CTR_VERSION	88,3146
#define FCI_CTR_RESET	89,3174
#define FCI_CTR_SELECT_INT	90,3200
#define FCI_CTR_TX_START	91,3230
#define FCI_CTR_MEMD_WRITE	92,3258
#define FCI_CTR_MEMD_READ	93,3300
#define FCI_CTR_MEMB_SELECT	94,3341
#define FCI_CTR_MEMB_C_WRITE	95,3391
#define FCI_CTR_MEMB_C_READ	96,3447
#define FCI_CTR_POKE_CUETRC_ARG0	97,3501
#define FCI_CTR_MEMB_NC_WRITE	98,3544
#define FCI_CTR_MEMB_NC_READ	99,3605
#define FCI_CTR_MEMA_SELECT	100,3664
#define FCI_CTR_MEMA_WRITE	101,3716
#define FCI_CTR_MEMA_READ	102,3759
#define FCI_CTR_INIT	103,3800
#define FCI_CTR_SPYG_MEM_SELECT	104,3857
#define FCI_CTR_SPYG_MEM_WRITE	105,3920
#define FCI_CTR_SPYG_MEM_READ	106,3981
#define FCI_CTR_INIT_COMPLETE	107,4040
# define FCI_INIT_RAM_ERR	108,4105
#define FCI_CTR_POKE_CUETRC_CMD	109,4169
#define FCI_CTR_GET_CHECK_PTR	110,4226
#define FCI_CTR_READ_RING_STAT	111,4296
#define FCI_CTR_STATUS	119,4575
# define FCI_CTR_STATUS_THROTTLE	120,4642
# define FCI_CTR_STATUS_UNTHROTTLE	121,4677
#define FCI_CTR_STATION	122,4714
#define FCI_CTR_AF_OR	123,4743
#define FCI_CTR_MAC_HIGH	125,4771
#define FCI_CTR_MAC_MIDDLE	126,4834
#define FCI_CTR_MAC_LOW	127,4899
#define FCI_CTR_SPYG_MEM_READ_RSLT	128,4962
#define FCI_CTR_SET_PH_BIT	130,5034
#define FCI_CTR_RESET_PH_BIT	131,5088
#define FCI_CTR_AF_OUTPUT	132,5145
#define FCI_CTR_ACCEPT_MAC	133,5210
#define   MAC_PITCH 134,5276
#define   MAC_ACCEPT 135,5299
#define FCI_CTR_AF_SIZE	136,5322
#define FCI_CTR_OPEN_RESULT	137,5384
#define CTR_AF_PAGES 142,5487
#define CTR_AF_WORDS 143,5511
#define CTR_COND_DCD	148,5619
#define CTR_COND_INIT_DONE 149,5689
#define CTR_COND_SRB_COMPLETE 150,5762
#define CTR_COND_RING_STAT 151,5837
#define CTR_COND_ADAPTER_CHECK 152,5912
#define CTR_COND_ENABLE_COMP 153,5990
#define CTR_COND_UCODE_CHECK 154,6041
#define CTR_COND_MASK	155,6119
#define CTR_RXSTAT_ROUTED 166,6487
#define CTR_RXSTAT_FS 167,6520
#define CTR_RXSTAT_MAC 168,6553
#define CTR_RXSTAT_EXT 169,6586
#define CTR_RXSTAT_INT 170,6663
#define CTR_RXSTAT_BOTH 172,6741
#define CTR_RXSTAT_SRB 173,6774
#define CTR_RXSTAT_MINE 174,6838
#define CTR_RXSTAT_TB 175,6918
typedef struct trc_cue_t_ trc_cue_t_187,7228
} trc_cue_t;trc_cue_t196,7408
#define TRC_TO_CUE_ADDRESS 198,7422
#define CUE_TO_TRC_ADDRESS 199,7468
#define SPY_CMDBLK_START 200,7514
#define SPY_CMDBLK_END 201,7560
#define CTR_MEMB_SIZE 202,7606
typedef struct trc_cue_init_t_ trc_cue_init_t_207,7721
} trc_cue_init_t;trc_cue_init_t216,7948
#define SPYG_INIT_SUCCESS 221,7999
#define SPYG_INIT_FIRST	222,8036
#define SPYG_INIT_PARITY 223,8073
#define SPYG_INIT_ROS	224,8110
#define SPYG_INIT_RAM	225,8145
#define SPYG_INIT_INSTRUCTION 226,8180
#define SPYG_INIT_INTERRUPT 227,8217
#define SPYG_INIT_PH	228,8254
#define SPYG_INIT_LAST	229,8288
typedef struct trc_cue_ramtest_t_ trc_cue_ramtest_t_234,8387
} trc_cue_ramtest_t;trc_cue_ramtest_t243,8594
#define SPYG_RAMTEST_SUCCESS 248,8648
#define SPYG_RAMTEST_INV_ADDR 249,8688
#define SPYG_RAMTEST_ADDR_ERR	250,8728
#define SPYG_RAMTEST_PRTY_ERR	251,8766
#define SPYG_RAMTEST_LAST	252,8804
typedef struct spyg_srb_t_ spyg_srb_t_259,8939
} spyg_srb_t;spyg_srb_t263,9027
#define SPYG_SRB_INIT_COMP 265,9042
#define SPYG_SRB_SET_SIGNAL	266,9079
#define SPYG_SRB_READ_ERROR_LOG 267,9112
#define SPYG_SRB_OPEN_ADAPTER	268,9149
#define SPYG_SRB_CLOSE_ADAPTER 269,9184
#define SPYG_SRB_SET_FUNCTIONAL 270,9221
#define SPYG_SRB_SET_GROUP 271,9258
#define SPYG_SRB_SET_BRIDGE 272,9295
#define SPYG_SRB_START_TBRIDGE 273,9332
#define SPYG_ARB_RING_STATUS 274,9369
#define SPYG_SRB_RETURN 276,9419
typedef struct spyg_srb_init_t_ spyg_srb_init_t_278,9457
} spyg_srb_init_t;spyg_srb_init_t284,9618
typedef struct spyg_srb_signal_t_ spyg_srb_signal_t_286,9638
} spyg_srb_signal_t;spyg_srb_signal_t290,9737
typedef struct spyg_srb_errlog_t_ spyg_srb_errlog_t_292,9759
} spyg_srb_errlog_t;spyg_srb_errlog_t298,9907
typedef struct spyg_srb_open_t_ spyg_srb_open_t_300,9929
} spyg_srb_open_t;spyg_srb_open_t308,10117
#define SPYG_OPEN_WRAP 310,10137
#define SPYG_OPEN_NOHARD	311,10177
#define SPYG_OPEN_NOSOFT	312,10210
#define SPYG_OPEN_PASS_ADAPTER	313,10243
#define SPYG_OPEN_PASS_ATTENTION 314,10282
#define SPYG_OPEN_CONTENDER 315,10322
#define SPYG_OPEN_PASS_BEACON 316,10362
#define SPYG_OPEN_NO_ETR 317,10402
#define SPYG_OPEN_OPTIONS 319,10443
typedef struct spyg_srb_close_t_ spyg_srb_close_t_321,10484
} spyg_srb_close_t;spyg_srb_close_t325,10582
typedef struct spyg_srb_functional_t_ spyg_srb_functional_t_327,10603
} spyg_srb_functional_t;spyg_srb_functional_t333,10755
typedef struct spyg_srb_group_t_ spyg_srb_group_t_335,10781
} spyg_srb_group_t;spyg_srb_group_t341,10923
typedef struct spyg_srb_bridge_t_ spyg_srb_bridge_t_343,10944
} spyg_srb_bridge_t;spyg_srb_bridge_t351,11202
typedef struct spyg_srb_transparent_t_ spyg_srb_transparent_t_353,11224
} spyg_srb_transparent_t;spyg_srb_transparent_t357,11328
#define SPYG_ERR_SUCCESS	359,11355
#define SPYG_ERR_INV_CMD 360,11385
#define SPYG_ERR_NOT_RUNNING 361,11422
#define SPYG_ERR_CLOSED 362,11459
#define SPYG_ERR_OPEN 363,11496
#define SPYG_ERR_OPEN_FAILED 364,11533
#define SPYG_ERR_INV_ADDR 365,11570
#define SPYG_ERR_INV_GROUP 366,11607
#define SPYG_ERR_INV_BRIDGE 367,11644
#define SPYG_ERR_TARGET_RING 368,11681
#define SPYG_ERR_SOURCE_RING 369,11718
#define SPYG_ERR_LAST 370,11755
#define SPYG_RCR_NRESET	378,11917
#define SPYG_RCR_NFRAQ	379,11952
#define SPYG_RCR_INSERT	380,11986
#define SPYG_RCR_NWRAP	381,12021
#define SPYG_RCR_ACCLB	382,12055
#define SPYG_RCR_ACCRI	383,12108
#define SPYG_RCR_RESERVED	384,12161
#define SPYG_RCR_RXEN	385,12197
#define SPYG_RCR_NOCHAIN	386,12230
#define SPYG_RCR_IGNCTL	387,12265
#define SPYG_RCR_ROPT0	388,12300
#define SPYG_RCR_RPOT1	389,12354
#define SPYG_RCR_IGN0	390,12413
#define SPYG_RCR_IGN1	391,12446
#define SPYG_RCR_IGN2	392,12479
#define SPYG_RCR_IGN3	393,12512
#define SPYG_RCR_IGN_MASK	394,12545
typedef struct spyg_addrs_t_ spyg_addrs_t_402,12722
} spyg_addrs_t;spyg_addrs_t406,12813
typedef struct spyg_parms_t_ spyg_parms_t_408,12830
} spyg_parms_t;spyg_parms_t431,14120
typedef struct spyg_error_log_t_ spyg_error_log_t_433,14137
} spyg_error_log_t;spyg_error_log_t448,14442
#define SPYG_VERS_LEN 452,14537
#define SPYG_PRODUCTID_LEN 453,14562
enum INIT_PHASE INIT_PHASE455,14593
    INIT_NONE,456,14611
    INIT_DISABLE,457,14626
    INIT_INIT,458,14644
    INIT_ENABLE,459,14659
    INIT_OPEN,460,14676
    INIT_OPEN_COMPLETE461,14691
struct spyglass_data_t_ spyglass_data_t_464,14722
#define TR_POLLING_OK	547,17813
#define TR_POLLING_TMOUT 548,17838
#define TR_POLLING_PERIOD	549,17866
#define SPY_MAX_CMD_TIME	551,17916
#define SPY_SRB_MAX_OPEN_TIME	552,17990
#define SPY_MAX_INIT_TIME 553,18068
#define SPYGLASS_DEADMAN_TIMER_ERROR_ENABLE 558,18189
#define SPYGLASS_PH_INTCTL 559,18240
