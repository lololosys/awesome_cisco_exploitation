
qllc.c,4725
#define WHICHSTREAM_STARTUP(WHICHSTREAM_STARTUP342,12484
qllctype *qllc_findfree 355,12786
void qllc_freeup 419,14792
static boolean qllc_x25_macaddr 469,15836
static void qllc_sprintf_address_link 480,16159
static boolean qllc_x25_address_map_enter 486,16289
static void qllc_x25_address_map_delete 505,16745
static void qllc_x25_lci_busy 521,17068
static boolean qllc_x25_pvc_setup 531,17256
void QPvcAddUnclaimed 628,19511
void QPvcRemoveUnclaimed 648,20028
static qllctype *QPvcLookupUnclaimed 662,20320
static enum avl_compare QLcnCompare 679,20657
static void qllc_send_quench 713,21352
static void qllc_enqueue 735,21739
paktype * qllc_get_stored_xid3 752,22156
static process qllc_input_process 768,22512
void qllc_input 859,25486
uchar qllc_extract_ctl 1001,29284
static void UpdateQllcRxStat 1018,29616
static int find_qllcpaktype 1062,30618
void qllc_init 1211,33596
boolean qllc_cleanupBLOCK 1264,35237
static forktype qllc_cleanup_proc 1277,35431
void qllc_kill_lci 1340,37327
void qllc_periodic 1381,38438
static boolean QllcIsLegacy 1505,42263
static boolean qllc_x25_incoming_call 1541,43067
#define MAXSTRING	1586,43979
static char *pr_x25_event 1588,44001
qllc_x25_lci_notify_event 1608,44402
void qllc_clean_qllc_inputQ 1781,48838
qllctype *qllc_setup 1816,49616
void qllc_quench 1871,50799
qllctype *qllc_by_dte 1915,51895
boolean qllc_validate 1931,52166
static boolean qllc_openBLOCK 1951,52461
qllctype *qllc_open 1978,53253
void qllc_contact_accept 2034,54664
void qllc_contact_reject 2053,55139
boolean qllc_close 2072,55651
void qllc_detach 2093,56198
boolean qllc_fwd_xid 2128,56867
void qllc_send 2150,57434
boolean qllc_psend 2212,59094
    if 2319,61863
void qllc_start 2333,62090
void qllc_ind_connect 2407,63957
void qllc_ind_connect_snx 2430,64460
void qllc_ind_connect_lnx 2483,65630
qllc_open_upstream_snx 2523,66615
void qllc_fix_sdlc 2623,69117
qllc_open_upstream_lnx 2659,69924
char *qllc_addr_short_help 2739,71902
boolean match_qllc_addr 2744,71994
static void QllcPrPacket 2769,72661
QSapRegistryT *QSapRegistryCreate 2871,74948
void QSapRegistryDestroy 2890,75327
enum avl_compare QSVCSapCompare 2902,75552
enum avl_compare QPVCSapCompare 2925,76008
static boolean QSRUSapIdMatch 2955,76631
QPSapT *QSRSearchByUSap 2976,77048
static avl_walker_type QSRPSapIdMatch 3006,77667
QSapT *QSRSearchByPSapId 3027,78098
static QPSapT *QSRSearchSubaddress 3047,78501
static QPSapT *QSRSearchByPvcRange 3062,78848
static QPSapT *QSRSearchByPvc 3081,79332
QPSapT *QSRSearchBySap 3100,79741
boolean QSRAddQSap 3114,80045
boolean QSRRemoveQSap 3138,80525
qllctype *QCepSearchByUCepId 3156,80954
QPSapT *QSapCreate 3176,81455
void QSapDestroy 3200,81929
enum avl_compare QVmacRCompare 3230,82851
QVmacRangeCreate 3261,83537
void QVmacRangeDestroy 3280,83930
static int QSapFindFreeVmac 3292,84120
boolean QSapVmacMapInUse 3314,84564
void QSapVmacMapAllocate 3320,84712
void QSapVmacMapFree 3326,84853
boolean QSapClaimPvcs 3353,85601
QCepListT *QCepListCreate 3420,87607
static boolean QCepDetachSap 3438,88053
void QCepListDestroy 3473,88867
qllctype *QCepListInsert 3503,89514
qllctype *QCepListGetFirst 3517,89753
qllctype *QCepGetNext 3531,89964
static boolean QCepListWalk 3540,90148
QCepSvcCompare 3563,90608
static qllctype **QCepPvcFindSapEntry 3632,92276
static boolean QCepInPvcList 3657,92809
qllctype *QCepPvcInsert 3673,93094
qllctype *QCepPvcListDelete 3696,93489
qllctype *QCepPvcGetFirst 3719,93912
qllctype *QCepPvcGetNext 3743,94353
boolean QCepPvcWalk 3768,94929
boolean QCepSvcProc 3803,95754
static qllctype *QCepCreate 3855,97110
void QCepDestroy 4000,101042
static void PakDrainQ 4040,101759
void QCepReset 4053,101935
static void badsubaddress 4100,102975
static boolean QCepIncomingCall 4129,103821
static boolean QCepMatchReq 4234,106686
qllctype *QCepFindOpened 4292,108549
QFindX121Address 4316,109252
QX121AddressLookup 4346,110159
qllctype *QCepReqOpenStn 4387,111113
void QCepBindLci 4434,112330
void QCepUnbindLci 4451,112580
QX25EventInput 4492,113528
static void QX25PacketInput 4588,115976
static QLInputT QMapToCsliEvent 4623,116801
static inline int qllc_hash_vmac 4665,117882
qllcmap_t *qllcmap_add 4679,118156
boolean qllcmap_remove 4719,118998
qllcmap_t *qllcmap_lookup 4756,119718
QDataT *qllc_hwidb_get_data 4789,120475
void QllcConfigDlsw 4833,121684
static void QllcUnconfigDlsw 4943,124334
static boolean QllcNvWriteDlswSap 5003,125773
void QllcNvWriteDlsw 5046,126922
static TCLSIMsg *QllcGetActRingMsg 5072,127481
static void QllcShowOneCep 5089,127758
static void QllcShowSapsCeps 5116,128411
void QllcShowCeps 5127,128575
