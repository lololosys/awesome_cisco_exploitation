
qllstate.c,2964
#define inline187,7041
void qllc_setstate(249,9455
void qllc_stateswitch 265,9807
void qadmstate 323,10813
void qsetupstate 407,12608
void qind_concnf 479,14395
void qresetstate 494,14670
void qd_connstate 528,15238
void qnormalstate 578,16047
void qindinfo 640,17340
void qllc_type1event(681,18192
void qllc_type1event_snx(695,18485
void qllc_type1event_lnx 791,20531
void qllc_sendcontrol 828,21368
void qllc_sendQFRMR 863,21999
void qllc_goto_adm 903,22802
void qllc_sendRR 923,23238
char *qllc_prstate(942,23589
char *qllc_inevent 975,24069
char *qllc_prstatus 1033,25061
#define QLLC_FSM_BUFF_SIZE	1070,25707
#define QAction	1078,25902
#define QT1N2Action	1079,25950
#define QT1TimerAct 1080,26005
#define QLFsmDecode	1081,26064
#define QXFsmDecode	1082,26119
#define QFsmPB	1083,26174
static inline int QllcFsmDecode 1092,26349
static int QllcLFsmDecode 1105,26554
static int QllcXFsmDecode 1116,26735
QFsmEventEnqueue 1127,26945
static ushort QFsmT1TimerAct 1146,27306
static ushort QFsmT1N2Action 1172,27997
static ushort QFsmAction 1184,28348
static inline boolean QLFsmTestAndSetRunning 2110,71409
static inline boolean QXFsmTestAndSetRunning 2120,71612
static inline void QLFsmClearRunning 2130,71789
static inline void QXFsmClearRunning 2142,71969
void QLFsmExecute 2158,72315
if 2201,73459
void QXFsmExecute 2226,74031
if 2276,75386
static inline void QCall 2314,76448
static inline void QAccept 2350,77325
static inline void QClearRst 2357,77439
static inline void QClearRstConf 2391,78073
static inline void QTxXid 2400,78276
static inline void QXTxNact 2430,78971
static inline void QTxTest 2454,79355
static inline void QTxSetMode 2486,79977
static inline void QTxUA 2498,80155
static inline void QTxDisc 2510,80333
static inline void QTxRD 2522,80509
static inline void QTxDM 2534,80682
static inline void QDlcQuench 2546,80875
static inline void QDlcDataOut 2557,81078
static inline void QDlcDataDrop 2595,81983
static inline void QReqOpenStnConf 2609,82309
static inline void CloseStnConf 2628,82670
static inline void QIdInd 2661,83695
static inline void QLNact 2708,84743
static inline void QTestInd 2733,85101
static inline void QConnInd 2756,85463
static inline void QConnConf 2771,85686
static inline void QConnectedInd 2790,86105
static inline void QDiscInd 2806,86399
static inline void QDiscConf 2822,86620
static inline void QClsQuench 2838,86897
static inline void QDataInd 2853,87127
static inline void QDataConf 2877,87508
static inline void QCleanup 2897,87920
static inline void QZombie 2931,89097
static inline void QReportRemoteError 2941,89242
static inline boolean QX25Up(2947,89375
#define MAXSTRING	2961,89602
static char *prClockAction 2963,89624
static char *prDlcAction 3015,90516
static char *prClsAction 3053,91145
char *prQLState 3075,91567
char *prQXState 3099,91984
static char *prQLEvent 3138,92684
static char *prQXEvent 3166,93263
