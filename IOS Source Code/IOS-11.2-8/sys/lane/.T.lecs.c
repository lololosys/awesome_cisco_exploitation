
lecs.c,7054
#define DO_RELEASE	619,24123
#define DO_NOT_RELEASE	620,24148
#define NO_INDEX	624,24200
#define GENERIC_TIMER	628,24242
#define LES_DISCONNECTED_TIMER	629,24295
#define register_all_nsaps(register_all_nsaps725,27718
#define deregister_all_nsaps(deregister_all_nsaps728,27795
void lecs_crash 741,28281
static inline boolean maskedByteStreamCompare 766,29213
static boolean invalidMaskedAddress 794,29821
static inline boolean maskedAddressCompare 802,30132
void maskedAddress_setMacValues 817,30522
void maskedAddress_setAtmValues 833,30915
void maskedAddress_setRDValues 852,31399
void setToFixedLecsAddress 865,31738
boolean initWatchedQueue 874,31920
static LecsAddress *LecsAddress_alloc 896,32613
static void LecsAddress_destroy 927,33377
static boolean LecsAddress_stillExists 956,34199
static void LecsAddress_setAtmValues 976,34863
boolean lecsAddressIsWKA 1008,35738
void sendRelease 1037,36678
static void lecsRelease 1058,37153
static void reassignActiveLes 1077,37762
static void disconnect_LES 1136,39463
static void disconnect_LECS 1186,41033
static void reIndexLesAddresses 1242,42419
static void serverAtmAddress_destroy 1264,42979
static int compareSAAbyPrio 1305,44043
static void resolveLESconnexionConflict 1324,44770
static ServerAtmAddress *addServerAtmAddressToElan 1353,45648
boolean deleteElan_AndOr_ServerAtmAddress 1455,48606
static void cleanLesRedundancyInfo 1525,50659
static void evaluateLecsMastership 1559,51582
static void process_LES_connected 1655,54119
static void process_higher_LECS_connected 1688,55086
int myHighestRank 1716,55898
static boolean noConnectedHigher_LECS 1740,56459
static Connection *Connection_alloc 1765,57222
static void Connection_dealloc 1802,58188
static Connection *ConnList_searchByCallId 1889,60985
static Connection *ConnList_searchByCalledParty 1907,61564
static Connection *ConnList_searchByCallingParty 1937,62555
static void ConnList_destroyByPtr 1963,63333
static void ConnList_destroyByCallId 1977,63705
static void ConnList_destroyByCalledParty 1992,64191
static void ILMISetFailureCB 2018,64980
static void ILMISetSuccessCB 2045,65765
boolean ILMI_Error 2067,66328
static void lecsIlmiCB 2077,66529
static void atmsigListenCancelCBSuccess 2121,67612
static void atmsigListenCancelCBFailure 2149,68230
static void atmsigListenCancelCB 2167,68725
static boolean registerWithIlmiService 2219,70071
static boolean registerLECSwithILMI 2244,70787
static int sendSetRequestToILMI 2281,71668
static void ilmiDeRegisterAddress 2312,72550
static void ilmiRegisterAddress 2346,73604
static void signallingDeRegisterAddress 2379,74726
static void signallingRegisterAddress 2415,75737
static void registerLecsNsap 2449,76696
static void deRegisterLecsNsap 2515,78528
static void cleanUpSvcResources 2546,79364
static boolean LecsAddress_atLeastOneSearchable 2566,79955
LecsAddress *searchLecsNsapActualValues 2589,80656
static int indexOfLecs 2617,81328
static void tearDownLecsConnections 2636,81767
processCallingPartyAddress 2659,82634
static void processSetupIndication 2748,85261
static void lecsSignallingCB 2968,92023
static int compareElanNames 3069,95398
static int compareServerAtmAddresses 3080,95672
static int compareLECsByMacAddress 3094,95982
static int compareLECsByAtmAddress 3108,96291
static int compareLECsByRouteDesc 3122,96599
static boolean absolutelyIdenticalLECnsaps 3151,97334
static boolean compareLECsUsingMask 3166,97713
static boolean ConfigTable_destroyLecByPtr 3183,98227
static LecInfoBlock *ConfigTable_findLECByAtmAddr 3225,99224
void ConfigTable_destroyLecByAtmAddr 3247,99704
void ConfigTable_destroyLecByMacAddr 3263,100057
void ConfigTable_destroyLecByRouteDesc 3280,100460
static void ConfigTable_destroyLECsInElan 3309,101353
static void lecsLockInterface 3370,103268
static void lecsUnlockInterface 3380,103461
static boolean ConfigTable_destroyElanByPtr 3393,103770
ConfigTable *ConfigTable_alloc 3435,104915
static void ConfigTable_dealloc 3547,108447
boolean ConfigTable_destroyByPtr 3601,110026
ConfigTable *getConfigTableByName 3629,110671
ElanInfoBlock *getElanByName 3647,111122
static ElanInfoBlock *getElanByLesAddress 3671,111764
static boolean restricted2bool 3696,112433
void setElanRestrictedStatus 3713,112892
void elanSetLesTimeout 3747,113779
void renameElan 3773,114472
static boolean differentPriorities 3821,115911
static int LECS_LES_conflict 3842,116509
ElanInfoBlock *ConfigTable_elanCreate 3870,117196
boolean ConfigTable_setDefaultElan 4009,121274
boolean ConfigTable_destroyElanByName 4040,122061
LecInfoBlock *ConfigTable_addLec 4084,123171
void ConfigTable_bindToLecsInfo 4223,127135
static LecsInfo *LecsInfo_alloc 4268,128490
void LecsInfo_destroy 4343,130548
void lecs_swif_erase 4395,131970
void lecs_interface_updown 4421,132627
void lecs_addr_recycle 4448,133327
LecsAddress *searchLecsAddress 4487,134669
int addLecsAddress 4517,135507
int deleteLecsAddress 4565,136779
void exposeThisLecsAddress 4602,137653
static void clearExposedLecsAddress 4638,138591
void unExposeThisLecsAddress 4671,139442
LecsInfo *getLecsPointer 4704,140425
LecsInfo *getLecsPointerFrom_HW 4720,140780
LecsInfo *buildLecsPointer 4741,141470
boolean lecs_init 4786,142585
boolean lecs_enqueue 4866,144838
static void transmitPacket 4890,145518
static void sendErrorResponse 4906,145892
static void dumpReceivedPacket 4931,146588
static ElanInfoBlock *searchLEC_InMacAddrList 4975,147938
static ElanInfoBlock *searchLEC_InUnmaskedList 4998,148565
static ElanInfoBlock *searchLEC_InMaskedList 5022,149277
static ElanInfoBlock *searchLEC_InRDList 5046,149948
static boolean inconsistentElans 5067,150527
static inline boolean badLanType 5082,150906
static inline boolean badFrameSize 5091,151101
static void lecsProcessSinglePacket 5112,151741
static void checkMastership 5582,164823
static boolean sameGlobalLecsAddresses 5596,165080
static void lowerLecssSignallingCB 5617,165573
static void connectToLowerLecss 5713,168062
static void getLecsAddressesCB 5788,170010
static boolean pendindDownwardConnections 5923,174019
static void getGlobalLecsAddresses 5944,174554
#define BASE_TIME	5984,175600
#define CHECK_RECONNECT_DOWN	5992,175836
static boolean timeForConnectingToLowerLecss 5995,175924
#define CHECK_REREGISTER	6007,176295
static boolean timeToReregisterAddresses 6010,176389
#define CHECK_ILMI_LECS	6022,176731
static boolean timeForGettingLecsAddresses 6025,176827
int timeRemainingToUpdateGlobalLecsAddresses 6035,177048
static process lecsServer 6053,177388
static void release_all_circuits 6159,180160
static void register_ALL_LecsNsaps 6230,181942
boolean hintOfLecs 6262,182701
boolean lecsIsReachable 6276,182980
static boolean lecsShouldRunOnThisInterface 6296,183550
static void startLecsServerProcess 6307,183839
static void killLecsServerProcess 6321,184188
static void startLecs 6362,185034
static void stopLecs 6391,185664
static void revisitLecs 6426,186546
