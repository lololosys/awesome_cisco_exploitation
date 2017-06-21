
sscop.c,2568
void sscop_init 247,8353
static sscopType *sscop_find_next_ctb 343,12027
void sscop_interfaceReset 375,12756
void sscop_initSscopCb 444,14793
void sscop_initStatistics 522,16782
static tinybool sscopGetLock 530,16901
static void sscopClearLock 538,17039
void saal_enqueue 557,17458
process sscop_inputProcess 609,18429
void sscop_input 676,20002
process sscop_outputProcess 744,21832
void sscop_output 869,24934
sscop_events sscop_findPduType 901,25575
void sscop_processSdpPdu 1032,28831
void sscop_processPollPdu 1209,35085
boolean sscop_isSdInAckQ 1287,37474
boolean sscop_isSdInRcvQ 1331,38510
long sscop_findPsInSd 1374,39550
void sscop_queueSdOnTxQ 1415,40365
void sscop_queueSdOnAckQ 1451,40994
void sscop_queueSdOnRcvQ 1505,41859
paktype *sscop_removeSdFromAckQ 1559,42784
paktype *sscop_removeSdFromRcvQ 1607,43957
void sscop_removeAllFromAckQ 1655,45123
void sscop_removeAllFromRcvQ 1696,46047
sscop_openCode sscop_open 1746,47195
void sscop_close 1784,48102
boolean sscop_transmitBeginPdu 1825,48964
boolean sscop_transmitBeginAckPdu 1875,50106
boolean sscop_transmitBeginRejPdu 1922,51218
boolean sscop_transmitEndPdu 1971,52293
boolean sscop_transmitEndAckPdu 2022,53433
boolean sscop_transmitResyncPdu 2082,54835
boolean sscop_transmitResyncAckPdu 2134,55999
boolean sscop_transmitUstatPdu 2190,57317
boolean sscop_transmitStatPdu 2244,58696
boolean sscop_transmitPollPdu 2299,60114
void sscop_sendSdPdu 2352,61311
void sscop_resendSdPdu 2493,65579
int sscop_createSolicitedList 2565,67445
void atmSig_sendToAal 2670,70147
void sscop_resetReceiver 2735,71815
void sscop_resetTransmitter 2761,72211
int sscop_processBeginPdu 2782,72486
boolean sscop_processBeginAckPdu 2862,74803
boolean sscop_processBeginRejPdu 2922,76319
boolean sscop_processEndPdu 2962,77155
boolean sscop_processEndAckPdu 3000,77966
void sscop_processUstatPdu 3037,78611
void sscop_processStatPdu 3114,80516
int sscop_processResyncPdu 3348,86746
int sscop_processResyncAckPdu 3393,87719
process sscop_timerProcess 3434,88414
void sscop_tickTock 3526,90674
void sscop_startTimer 3647,93932
void sscop_stopTimer 3660,94096
void sscop_incVms 3671,94238
boolean sscop_registerIlmi 3695,94656
void sscop_ilmiCallback 3735,95463
static tinybool sscop_quick_processPollPdu 3850,98603
static void sscop_quick_transmitStatPdu 3906,99957
sscop_quick_sendToAal 3947,101001
tinybool sscop_handle_poll_pdu 3975,101570
#define SSCOP_MAJVERSION 4048,103277
#define SSCOP_MINVERSION 4049,103311
#define SSCOP_EDITVERSION 4050,103344
