
dspu_ls.c,1391
#define LS_TEMPLATE103,3874
#define LsMagicNumber 141,4786
#define LsMagicMask 142,4830
#define DownStreamBit 143,4874
#define ID_MASK 145,4914
#define LsSetUCepID(LsSetUCepID147,4952
#define XidItime 150,5060
#define CloseStnITime 151,5138
#define MAX_NULLXID_RETRIES 153,5219
typedef enum LsDownStreamWork LsDownStreamWork158,5368
        } LsDownStreamWork;LsDownStreamWork161,5457
LsCreate 197,6368
LsDelete 267,8293
LsFind 308,9479
LsReceive 335,10291
LsSendData 557,16785
LsSapClosed 578,17195
LsQueryLsOnSapAndStart 600,17667
LsLogForWork 626,18325
LsDoWork 648,18868
LsQueryBufferCount 833,24567
LsFreeBuffer 852,24919
LsFlowControlFromClsi 885,25803
LsFlowControlFromPu 914,26515
LsProcessCmd 974,27727
LsFsm 996,28157
LsMatchPu 1560,46081
static boolean LsMatchConnOutPuOnSap 1583,46587
static boolean LsMatchConnOutPuOnAdaptor 1611,47409
boolean QueryLsConnOutPuOnSap(1639,48151
boolean QueryLsConnOutPuOnAdaptor(1683,49083
LsFindActivePu 1730,50139
LsFindInactivePu 1759,50884
LsSendXidOrConnect 1794,51906
LsItimer 1843,53191
LsStartITimer 1862,53557
void DebugLs 1882,53965
void NoDebugLs 1904,54426
ShowLs 1927,54891
void LsWrite 1952,55503
void LsWriteRsrbStart 1959,55617
void LsWriteVdlcStart 1966,55753
void LsWriteOut 1973,55889
void LsDeletePoolRangeIfMatch 1981,56034
QueryFindChild 1999,56524
LsXidNegErrMsg 2019,56980
