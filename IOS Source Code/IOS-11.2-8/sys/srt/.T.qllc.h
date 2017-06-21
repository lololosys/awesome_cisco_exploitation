
qllc.h,2155
#define __QLLC_H__79,2898
struct qllcmap_t_ qllcmap_t_89,3110
#define QLLC_CMD 101,3296
#define QLLC_RSP 102,3363
#define QSM 107,3467
#define QDISC 108,3489
#define QUA 109,3511
#define QDM 110,3533
#define QFRMR 111,3555
#define QXID 112,3577
#define QTEST 113,3599
#define QRR 114,3621
#define QRD 115,3643
#define QLLC_MAX_PAKTYPE 118,3700
#define DATA_PACKET 122,3754
#define TOO_SHORT 123,3779
typedef enum qllc_state_t_ qllc_state_t_129,3859
} qllc_state_t;qllc_state_t135,3964
    QRAW_TOO_LOW 143,4037
    REC_QXID_CMD,144,4059
    REC_QTEST_CMD,145,4077
    REC_QXID_RSP,146,4096
    REC_QTEST_RSP,147,4114
    REC_QSM_CMD,149,4135
    REC_QDISC_CMD,150,4152
    REC_QRD_RSP,151,4171
    REC_QUA_RSP,152,4188
    REC_QDM_RSP,153,4205
    REC_QFRMR_RSP,155,4224
    REC_QRR_CMD,157,4245
    REC_QIFRAME,159,4264
    REC_FRMR_EVENT,161,4283
    REC_TOO_QSMALL,162,4303
    QRAW_TOO_HIGH163,4323
#define QLS_SUCCESS 169,4390
#define QLS_DISCONNECT 170,4423
#define QLS_FAILED 171,4457
#define QLS_WAITING	172,4491
#define QLS_INTDOWN 173,4521
#define QLS_TIMEOUT 174,4555
#define QLS_DETACHING	175,4589
#define QT1_DEFAULT 180,4656
#define QRETRY_COUNT 181,4693
typedef struct qllc_frmr_t_ qllc_frmr_t;qllc_frmr_t183,4724
typedef struct QllcStat_ QllcStat_185,4766
} QllcStat;QllcStat206,5659
typedef enum QLStateT_ QLStateT_211,5693
} QLStateT;QLStateT220,6128
typedef enum QX25StateT_ QX25StateT_223,6143
} QX25StateT;QX25StateT235,6827
#define MAX_QLLC_EVENTS	239,6861
#define FSM_NO_EVENT	241,6932
struct QllcEventT_ QllcEventT_243,6958
struct qllctype_ qllctype_253,7082
struct QCepListT_ QCepListT_354,10140
struct qllc_frmr_t_ qllc_frmr_t_362,10240
#define Q_RSP	370,10386
#define Q_W	372,10407
#define Q_X	373,10463
#define Q_WX	374,10524
#define Q_Y	375,10550
#define Q_Z	376,10602
#define QFRMR_SIZE	378,10639
typedef struct qllc_header_ qllc_header_393,11036
} qllc_header;qllc_header396,11097
#define QLLC_HDRSIZE 399,11180
    QLLC_CEPQ_EVENT 405,11249
    QLLC_CLEANUPQ_EVENT,406,11275
    QLLC_DATAQ_EVENT,407,11300
    QLLC_CALLQ_EVENT,408,11322
