
qllc_private.h,4109
#define __QLLC_PRIVATE_H__75,2718
    QX25_TIMER,115,4205
    QLLL_TIMER,116,4252
    QLNX_TIMER,117,4297
#define QLLC_DEFAULT_MAX_IFIELD	133,4800
#define QLLC_NO_MAJOR	134,4838
typedef enum QXInputT_ QXInputT_148,5023
} QXInputT;QXInputT173,5485
typedef enum QLInputT_ QLInputT_189,5700
} QLInputT;QLInputT229,6358
}  	QX25OpT;QX25OpT273,7485
}  	QClsOpT;QClsOpT309,8532
}	QTimerT QTimerT322,8792
} QFsmParamBlockT;QFsmParamBlockT334,8964
struct QPSapT_ QPSapT_349,9388
static inline boolean IsSVCSap 366,9678
static inline boolean QSapTestProxyXid 369,9769
static inline boolean QSapTestPartner 372,9871
static inline boolean QSapTestRemote 375,9971
static inline boolean QSapTestVmac 378,10069
static inline boolean QSapTestDlsw 381,10164
static inline boolean QSapTestTRSaps 384,10258
static inline boolean QSapTestNpsiPoll 387,10355
static inline uchar *QSapGetPartner 390,10456
static inline boolean IsSVCPSap 395,10566
static inline uchar *QPSapGetSubaddress 398,10667
static inline QSapPVCRangeT *QPSapGetPvcRange 401,10779
static inline uint QPSapGetPvcLow 404,10896
static inline uint QPSapGetPvcHigh 407,11007
static inline boolean IsPSVCPSap 410,11120
static inline boolean QPSapTestPartner 413,11209
static inline boolean QPSapTestProxyXid 416,11311
static inline boolean QPSapTestVmac 419,11415
static inline boolean QPSapTestRemote 422,11512
static inline uchar *QPSapGetPartner(425,11613
typedef enum qllc2OpenFailureCode_qllc2OpenFailureCode_440,11983
} qllcOpenFailureCode;qllcOpenFailureCode445,12099
#define QLLC_HASH_SIZE	449,12174
#define DEFAULT_X25_TIMER 453,12239
#define DEFAULT_QL_TIMER 454,12297
#define DEFAULT_QL_RETRIES	455,12355
#define QLLC_TXQ_LOW_WATER	461,12511
#define QLLC_TXQ_HIGH_WATER	462,12558
#define QLLC_CLS_FLOW_IND_DAMP	463,12606
typedef boolean QCepWalkT(QCepWalkT477,13236
static inline boolean QllcIsCls(479,13296
#define NO_VMAC	500,13932
#define VMAC_FREE	501,13990
#define VMAC_IN_USE	502,14010
struct QSapRegistryT_ QSapRegistryT_518,14397
#define QMSG_MAX	539,15083
struct qllc_data_t_ qllc_data_t_540,15145
typedef qllc_data_t QDataT;QDataT549,15349
static inline QDataT *qllc_idb_get_data 553,15441
static inline void qllc_hwidb_set_lnx_active 560,15579
static inline void qllc_idb_set_lnx_active 571,15863
static inline boolean qllc_hwidb_test_lnx_active 577,16008
static inline boolean qllc_idb_test_lnx_active 589,16228
static inline tr_ventrytype *qllc_get_vre 594,16355
static inline void qllc_set_vre 605,16555
static inline QSapRegistryT *QllcPortGetSapRegistry 622,16928
#define QVMAC_TIMEOUT 657,18214
struct  QSapVmacRangeT_ QSapVmacRangeT_659,18253
QEventCreate 690,18981
QEventDestroy 718,19535
static inline int QExtractEvent 738,19929
static inline void QLFsmEventEnqueue 775,20647
static inline void QXFsmEventEnqueue 785,20881
static inline QXInputT QXFsmEventDequeue 795,21096
static inline QLInputT QLFsmEventDequeue 805,21289
QEventCleanFsmQueues 817,21573
QEventCleanQueues 835,21938
static inline boolean QCepTestPartner 842,22070
static inline boolean QCepTestVmac 845,22173
static inline boolean QCepTestNpsiPoll 848,22270
static inline uchar *QCepGetPartner 851,22374
static inline ushort QCepGetLcn 854,22464
static inline uchar *QCepGetRemX121Addr(862,22605
static inline boolean QCepTestRemote(871,22768
static inline void QCepSetRemote(874,22871
static inline void QCepSetProxyXid(877,22958
static inline void QCepSetVmac(880,23048
QSVCSapSearch 905,23584
QSVCSapGetFirst 919,23847
QSVCSapGetNext 931,24043
QSVCSapInsert 943,24294
QSVCSapDelete 959,24584
QPVCSapSearch 983,25046
QPVCSapGetFirst 997,25301
QPVCSapGetNext 1009,25497
QPVCSapInsert 1022,25749
QPVCSapDelete 1038,26039
QVmacRSearch 1062,26517
QVmacRGetFirst 1076,26805
QVmacRGetNext 1088,27017
QVmacRInsert 1100,27288
QVmacRDelete 1116,27609
static inline qllctype *QCepSvcInsert(1149,28347
static inline qllctype *QCepSvcListDelete(1164,28641
static inline qllctype *QCepSvcGetFirst(1179,28942
static inline qllctype *QCepSvcGetNext(1191,29134
