
rsp_qasic.h,3564
#define __RSP_QASIC_H__45,1655
#define	QA_DIAGSIZE	47,1680
#define	QA_QSIZE	48,1721
#define	QA_ACCSIZE	49,1768
#define	QA_NQ_ELEMENTS	50,1819
#define	DIAG_ERROR_LOG_BASE	52,1849
#define	DIAG_MEMD_BASE_REG	53,1917
#define	QA_LSA_BIT	54,1984
#define ACC_LOWBITS	56,2013
#define ACC_LOWMASK	57,2040
#define	qaptr2boffset(qaptr2boffset59,2068
#define	QNUM(QNUM60,2141
#define	ANUM(ANUM61,2194
#define QA_BASE_OFFSET	69,2310
#define	QA_Q_OFFSET	70,2370
#define	QA_QEND_OFFSET	71,2436
#define	QA_ACC_OFFSET	72,2495
#define	QA_ACCEND_OFFSET	73,2562
#define	QA_END_OFFSET	74,2621
#define	QA_NQUEUES	76,2677
#define	QA_NACCS	77,2737
#define	QA_FIRST_QNUM	79,2802
#define	QA_QNUM_LIMIT	80,2848
#define QA_LAST_QNUM	81,2898
#define	QA_FIRST_ACCNUM	83,2939
#define	QA_ACCNUM_LIMIT	84,2994
#define	QA_LAST_ACCNUM	85,3028
#define POOL2_NUM_ACC 87,3093
#define POOL2_FIRST_ACCNUM 89,3221
#define POOL2_FIRST_TXQNUM 90,3287
#define POOL2_LAST_ACCNUM 91,3338
#define POOL2_LAST_TXQNUM 92,3411
#define POOL1_NUM_ACC 93,3462
#define POOL1_FIRST_ACCNUM 94,3536
#define POOL1_LAST_ACCNUM 95,3592
#define POOL1_LAST_TXQNUM 96,3649
#define QA_STATUS_WORDS_RSP	99,3756
#define QA_STATUS_WORDS_RSP	113,4095
#define QA_ACC_OFFSET	115,4160
#define QA_NQUEUES 118,4187
#define QA_NACCS 119,4251
#define	QA_FIRST_QNUM	121,4320
#define QA_LAST_QNUM	123,4401
#define QA_QNUM_LIMIT 124,4428
#define	QA_FIRST_ACCNUM	126,4478
#define	QA_LAST_ACCNUM	128,4561
#define QA_ACCNUM_LIMIT 129,4590
#define POOL2_NUM_ACC 131,4662
#define POOL2_FIRST_ACCNUM 133,4790
#define POOL2_FIRST_TXQNUM 134,4856
#define POOL2_LAST_ACCNUM 135,4907
#define POOL2_LAST_TXQNUM 136,4980
#define POOL1_NUM_ACC 137,5031
#define POOL1_FIRST_ACCNUM 138,5105
#define POOL1_LAST_ACCNUM 139,5161
#define POOL1_LAST_TXQNUM 140,5218
typedef volatile ushort	qa_hwq_reg_t;qa_hwq_reg_t153,5355
typedef volatile short	qa_acc_reg_t;qa_acc_reg_t154,5393
typedef volatile ushort qa_qstatus_reg_t;qa_qstatus_reg_t155,5430
struct qa_hwq_t_ qa_hwq_t_157,5510
struct qa_acc_t_ qa_acc_t_163,5716
#define QA_DIAG_INPUT_PARITY_ERROR	170,5951
#define QA_DIAG_REUSE0_LINK_ERROR	171,5991
#define QA_DIAG_MEM_PARITY_ERROR	172,6030
#define QA_DIAG_ALT_QWRITE_ERROR	173,6068
#define QA_DIAG_BAD_ADD_ERROR 174,6106
#define QA_DIAG_SELECT_MASK 177,6146
#define QA_DIAG_ADDRESS_MASK 178,6192
#define QA_DIAG_DATA_PARITY_MASK 179,6239
#define QA_DIAG_NQUEUES	181,6285
#define QA_DIAG_NACCS	182,6348
#define QA_DIAG_BUF_MASK	185,6485
#define QA_DIAG_BUF_SHIFT	186,6518
typedef struct qa_diag_data_t_ qa_diag_data_t_196,6727
} qa_diag_data_t;qa_diag_data_t199,6800
typedef struct qa_diag_t_ qa_diag_t_202,6820
} qa_diag_t;qa_diag_t223,7323
#define	QA_READ_HEAD(QA_READ_HEAD238,7723
#define	QA_READ_TAIL(QA_READ_TAIL243,7888
#define	QA_READ_BODY(QA_READ_BODY251,8135
#define	QA_READ_LSA(QA_READ_LSA256,8313
#define	QA_READ_STATUS(QA_READ_STATUS260,8478
#define	QA_READ_ATTN(QA_READ_ATTN264,8635
#define	QA_READ_DIAG_ACC(QA_READ_DIAG_ACC268,8770
static inline ushort qa_read_head(275,8960
static inline ushort qa_read_tail(280,9072
static inline ushort qa_read_body(285,9184
static inline ushort qa_read_lsa(290,9308
static inline ushort qa_read_status(295,9430
static inline ushort qa_read_attn(300,9546
static inline ushort qa_read_diag_acc(305,9658
#define	QA_CLEAR_ERROR(QA_CLEAR_ERROR312,9778
#define QA_ALLOC_ERROR	314,9847
} queue_t;queue_t326,10241
#define SLOT2CMDQ(SLOT2CMDQ334,10425
} rsp_cbus_state_t;rsp_cbus_state_t340,10544
