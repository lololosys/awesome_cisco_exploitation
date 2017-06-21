
atmsig.c,3051
#define ATMSIG_TRAFFIC_INSTALL_DEFAULT 261,9542
#define ATMSIG_BYTES_PER_CELL 267,9697
#define ATMSIG_BITS_PER_BYTE 268,9741
#define ATMSIG_BITS_PER_CELL_MULTIPLIER 269,9785
#define ATMSIG_KBPS_DIVISOR 270,9848
#define ATMSIG_MAX_INPUT_SIMPLE_CPS_TO_KBPS_ALG 272,9905
#define ATMSIG_MAX_INPUT_SIMPLE_KBPS_TO_CPS_ALG 273,9962
#define ATMSIG_KBPS_OF_MAX_INPUT_SIMPLE_CPS_TO_KBPS_ALG 275,10020
#define ATMSIG_CPS_OF_MAX_INPUT_SIMPLE_KBPS_TO_CPS_ALG 276,10116
void atmSig_init 331,11447
inline boolean atmSig_isV31 451,14702
static atm_uniVersionType atmSig_getUniVersionByPak 457,14846
void atmSig_fillTosInSvc 472,15228
void sscop_enqueue 569,18730
forktype atmSig_inputProcess 606,19581
void atmSig_input 642,20319
forktype atmSig_outputProcess 748,23114
atmSvcType *atmSig_selectSvcBlock 785,24048
int atmSig_hashCallRef 943,28838
boolean atmSig_allocateCallRef 965,29359
static void atmSig_initTos 1018,30803
atmSvcType *atmSig_allocateSvcBlock 1045,32081
void atmSig_freeSvcBlock 1075,32803
void atmSig_removeVcFromDriver 1157,34851
static uint selectRateTrafficParm 1189,35721
static uint selectBurstTrafficParm 1211,36468
static void atmSig_selectShapingParms 1232,37198
boolean atmSig_reqVcFromDriver 1300,40051
atmSvcType *atmSig_findSvcBlockByCR 1420,43572
atmSvcType *atmSig_findSvcBlockByVC 1494,45191
void atmSig_enterHashEntry 1526,45805
boolean atmSig_removeHashEntry 1563,46633
boolean atmSig_validateSvc 1603,47618
void atmSig_globalReset 1638,48234
void atmSig_interfaceReset 1672,49029
static void atmSig_sscopFail 1756,51526
static void atmSig_sscopEstablish 1793,52407
static void atmSig_statusEnquiry 1838,53437
void atmSig_subinterfaceReset 1855,53912
boolean atmsig_interface_up 1896,54880
boolean atmSig_nullNsap 1926,55369
boolean atmSig_nullPrefix 1947,55659
boolean atmSig_nullEsi 1968,55949
static void atmSig_process_messages 1997,56319
forktype atmSig_timerProcess 2046,57484
void atmSig_startTimer 2083,58248
void atmSig_stopTimer 2103,58598
void atmSig_tickTock 2123,58950
void atmSig_svcTimeout 2178,60343
atmSvcPartyType *atmSig_selectPartyBlock 2381,67405
boolean atmSig_enterPartyBlock 2461,69377
boolean atmSig_removePartyBlock 2503,70390
atmSvcPartyType *atmSig_allocatePartyBlock 2557,71674
boolean atmSig_enterPartyHashEntry 2587,72353
boolean atmSig_removePartyHashEntry 2632,73526
atmSvcPartyType *atmSig_findPartyBlockByRef 2688,75125
atmSvcPartyType *atmSig_findPartyBlockByEndpointRef 2744,76769
boolean atmSig_allocateEndpointRef 2792,77894
boolean atmSig_validateParty 2871,79764
atmSig_statuscodetype atmSig_findPartyStatus 2918,80634
void atmSig_freePartyBlock 2964,81936
boolean atmSig_flushPartyQ 3032,83725
boolean atmSig_freeAllParties 3115,85838
int atmSig_parseBroadbandLowLayerInfo 3167,87077
boolean atmSig_fillBroadbandLowLayerInfo 3302,90052
long atmSig_cps_to_kbps 3411,92409
long atmSig_kbps_to_cps 3439,93341
#define ATMSIG_MAJVERSION 3467,94261
#define ATMSIG_MINVERSION 3468,94295
#define ATMSIG_EDITVERSION 3469,94328
