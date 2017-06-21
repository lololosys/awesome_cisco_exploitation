
lecs_finder.c,2001
#define THRESHOLD_TIME	293,10980
typedef struct _userRequestBlock_userRequestBlock302,11271
} UserRequestBlock;UserRequestBlock327,11861
typedef struct switchLecsAddressswitchLecsAddress343,12416
} SwitchLecsAddress;SwitchLecsAddress363,12873
typedef struct _lecsFinderData_lecsFinderData374,13209
} LecsFinderData;LecsFinderData416,14366
#define ILMI_HUNG_TIMEOUT	429,14667
#define NO_PROBLEM	439,14966
static boolean finderGetIlmiClientId 450,15346
void lecs_finder_init 468,15791
void globalLecsAddresses_regenerate 486,16194
static void forceGetNext_nextTime 509,16852
#define MAX_TRANSID	521,17284
uint lecsFinderRequest 523,17313
static void tellUserAllLecsAddresses 608,19590
static void tellUserMasterLecsAddress 678,21330
static void answerAllUrbs 712,22261
static MaskedAddress *manuallySpecifiedLecsAddress 764,23491
static void setOneMasterLecsAddress 795,24495
static void setAllMasterLecsAddresses 840,25809
static void connectToLecs 855,26144
static void tryNextLecs 908,27648
static void startConnecting 928,28276
static void finderRelease 960,29284
static SwitchLecsAddress *findSLecsAddress_byAddress 980,30015
static SwitchLecsAddress *findSLecsAddress_byCallId 1002,30601
static void switchLecsAddress_destroy 1025,31273
void switchLecsAddress_delete 1050,31974
static void switchLecsAddress_deleteAll 1088,32941
void switchLecsAddress_add 1106,33518
static boolean vblist2hwaddrtype 1231,36870
static boolean endOfGetNext 1253,37445
static boolean invalidLecsAddrOid 1264,37716
static void lecsFinderIlmiCB 1292,38514
static void registerWithIlmiServices 1386,41240
static void getAllSwitchLecsAddresses 1424,42546
static void lecsFinderSignallingCB 1547,46329
boolean prepareLecsFinderModule 1643,49223
void clearLecsFinderModule 1730,51680
static void processUserRequest 1766,52669
static process lecsFinder 1848,54947
#define COUNT	1855,55078
static void allLecssCallback 1915,56588
static process finderStressTest 1923,56832
