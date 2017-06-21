
svip_cya.h,4477
#define PCIMEM_BASE_ADDR_IN_CYA 58,1944
#define PCIMEM_SBAR_SIZE	64,2106
#define PCIMEM_SBAR_ALIGNMENT	65,2177
#define CYA_FORCED_ERR_INTR	73,2362
#define CYA_BE_FROM_PMA_ON_CYA	74,2431
#define CYA_SOFT_DBUS_ERR 76,2539
#define CYA_OVF_INT_ARB_FIFO 78,2641
#define CYA_DMA_XMT_ERR 81,2743
#define CYA_DMA_RCV_ERR 82,2813
#define CYA_QE_LOW_PRI_INTR 83,2882
#define CYA_QE_PGM_MPAR_INTR 85,2983
#define CYA_CPU_REUSE_PREAD 87,3089
#define CYA_RD_UNUSED_PREAD 91,3276
#define CYA_RD_BAD_DATA 93,3371
#define CYA_GLOCK_REQ_UNAVAIL 96,3513
#define CYA_RD_DURING_STALL 98,3629
#define CYA_RD_PREAD_NOT_CMPL 101,3789
#define CYA_OVF_WR_FIFO_2MEMD 104,3931
#define CYA_ILL_ADRS_FROM_CPU 107,4051
#define CYA_RETRY_NOT_SAME_ADRS	108,4127
#define CYA_REUSE_PREAD_STALL 110,4246
#define CYA_INV_BE_FROM_CPU 115,4469
#define CYA_SINGLE_IDLE_CYCLE 117,4592
#define CYA_ODD_PBUS_GNTS 119,4698
#define CYA_PAR_ERR_PBUS 121,4794
#define CYA_PAR_ERR_INTERNAL 123,4890
#define CYA_PAR_ERR_CBUS 125,4994
#define CYA_MISSING_ACK_CBUS 127,5090
#define CYA_NACK_PRESENT_CBUS 128,5166
#define CYA_CODE_SIZE 130,5245
#define CYA_DATA_SIZE 131,5315
#define QE_XMEM_START 132,5352
#define QE_YMEM_START 134,5457
#define CYA_STALL_INDICATION 145,5729
#define CYA_STALL_IN_PROGRESS 148,5852
#define CYA_CBUS_STALL_FORCED 149,5928
#define CYA_CBUS_STALL_START 150,5997
#define CYA_CBUS_STALL_END 151,6068
#define CYA_OI_REUSE_PREAD_STALL 152,6137
#define CYA_EXCP_INTR_FORCED 157,6339
#define CYA_QE_INTR_FORCED	158,6415
#define CYA_QE_DMA_INTR 159,6481
#define CYA_DBUS_INTR_FORCED	162,6627
#define CYA_DBUS_INTR	163,6697
#define CYA_MALU_ATTN_FORCED	164,6755
#define CYA_MALU_ATTN	165,6821
#define CYA_BUS_ERROR_EVENTS 167,6877
#define CYA_MCR_CSI 173,7117
#define CYA_MCR_BCR 178,7339
#define CYA_MCR_CSDI 183,7505
#define CYA_MCR_QCAP 187,7718
#define CYA_MCR_RESET 190,7873
#define TX_STATUS_WORD 195,8033
#define IPC_STATUS_WORD 196,8071
#define IPC_STATUS_WORD_BIT 197,8109
#define QE_NUM_QUEUES 198,8147
#define QE_NUM_STATUS 199,8172
#define QUEUES_PER_STATUS_REGISTER 200,8196
#define QE_FAST_QUEUE 201,8234
#define MAX_TX_QUEUES 202,8298
#define CHAN_TXQ_FLAG 203,8373
#define PR_MASK_4BYTE 206,8485
#define PR_MASK_4BYTE_UPPER 207,8559
#define PR_MASK_2BYTE 208,8633
typedef struct cya_dma_t_ cya_dma_t_220,8879
} cya_dma_t cya_dma_t256,10375
typedef struct cya_qe_pkt_ cya_qe_pkt_262,10498
} cya_qe_pkt;cya_qe_pkt276,11005
typedef struct cya_qe_cbus_ cya_qe_cbus_283,11118
} cya_qe_cbus;cya_qe_cbus306,11968
#define QE_CBUS_WRITE_COMPLETE	311,12038
typedef struct cya_qe_diag_ cya_qe_diag_317,12128
} cya_qe_diag;cya_qe_diag337,12861
#define QE_RUN_BIT 339,12877
#define QE_DEBUG_PC_BIT 340,12900
#define QE_JS_WAIT_DISABLE_BIT 341,12929
typedef struct pstd_rd_regs_ pstd_rd_regs_343,12966
} pstd_rd_regs_t;pstd_rd_regs_t352,13621
typedef struct cya_cntrl_ cya_cntrl_354,13640
} cya_cntrl_t;cya_cntrl_t380,14977
typedef struct double_word_ double_word_382,14993
} double_word_t;double_word_t385,15063
typedef struct cya_qe_xmem_ cya_qe_xmem_387,15081
} cya_qe_xmem_t;cya_qe_xmem_t401,15741
typedef struct cya_qe_ymem_ cya_qe_ymem_403,15759
} cya_qe_ymem_t;cya_qe_ymem_t417,16390
typedef struct qe_addrs_space_ qe_addrs_space_420,16409
} qe_addrs_space;qe_addrs_space444,17499
typedef struct cya_asic_ cya_asic_454,17869
} cya_asic;cya_asic480,18664
#define CYA_ASIC 483,18678
static inline void set_qe_opt 487,18767
static inline void clear_qe_opt 493,18897
static inline void stop_qe 500,19031
static inline void start_qe 505,19100
#define CYA_REG_ADDRESS(CYA_REG_ADDRESS517,19264
#define CYA_PRB0_REG 526,19566
#define CYA_PR0_REG 527,19646
#define CYA_PRB1_REG 528,19726
#define CYA_PR1_REG 529,19806
#define CYA_PRB2_REG 530,19886
#define CYA_PR2_REG 531,19966
#define CYA_PRB3_REG 532,20046
#define CYA_PR3_REG 533,20126
#define CYA_MC_REG 534,20206
#define CYA_EIS_REG 535,20286
#define CYA_EIC_REG 536,20366
#define CYA_EIE_REG 537,20446
#define CYA_OIS_REG 538,20526
#define CYA_OIC_REG 539,20606
#define CYA_OIE_REG 540,20686
#define CYA_SBA_REG 541,20766
#define CYA_CBE_REG 542,20846
#define CYA_PBE_REG 543,20926
#define CYA_GLRQ_REG 544,21006
#define CYA_GLRI_REG 545,21086
#define CYA_GLRC_REG 546,21166
#define CYA_GLTO_REG 547,21246
#define CYA_QE_REG 548,21326
#define CYA_FIFO_RAM_DS 549,21406
#define IO_EXT_INT_LO_REG 552,21563
