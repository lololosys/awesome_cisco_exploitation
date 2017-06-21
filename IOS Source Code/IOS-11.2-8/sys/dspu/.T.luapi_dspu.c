
luapi_dspu.c,3400
typedef enum ApiState ApiState66,1955
} ApiState;ApiState70,2195
typedef enum SscpLuInput SscpLuInput82,2325
} SscpLuInput;SscpLuInput106,3933
typedef enum LuApiSscpLuState LuApiSscpLuState112,4014
} LuApiSscpLuState;LuApiSscpLuState122,4665
typedef enum LuLuInput LuLuInput180,6848
} LuLuInput;LuLuInput196,7897
typedef enum LuLuState LuLuState202,7967
} LuLuState;LuLuState207,8177
typedef enum ReqRspInd ReqRspInd248,8830
} ReqRspInd;ReqRspInd251,8895
typedef struct LuApiCommon LuApiCommon267,9263
} LuApiCommon;LuApiCommon281,9906
typedef struct SscpLuApi SscpLuApi287,9969
} SscpLuApi;SscpLuApi299,10462
typedef struct LuLuApiOpenParm LuLuApiOpenParm305,10546
} LuLuApiOpenParm;LuLuApiOpenParm316,10902
typedef struct LuLuApi LuLuApi322,10966
} LuLuApi;LuLuApi352,12212
enum LuApiWorkBits LuApiWorkBits359,12349
    LUAPIWORK_ACTLURSP 360,12370
    LUAPIWORK_ACTLURSP      =  0x01,360,12370
    LUAPIWORK_INITSELF 361,12407
    LUAPIWORK_INITSELF      =  0x02,361,12407
    LUAPIWORK_NOTIFYAVAIL 362,12444
    LUAPIWORK_NOTIFYAVAIL   =  0x04,362,12444
    LUAPIWORK_NOTIFYUNAVAIL 363,12481
    LUAPIWORK_NOTIFYUNAVAIL =  0x08363,12481
struct LuApi LuApi370,12582
static LuApiHandle* LuApiHandle_nw 405,13763
static Lu* LuApi_nw 423,14147
static void LuApiCommon_init 460,15071
static SscpLuApi* SscpLuApi_nw 490,15738
static LuLuApiOpenParm* LuLuApiOpenParm_nw 516,16425
static LuLuApi* LuLuApi_nw 531,16734
static void LuLuApi_dl 565,17684
static void SscpLuApi_dl 585,18162
static void LuApi_dl 606,18683
static void SendPosRsp 632,19355
static void SendPosRspWithPacing 651,19993
static void SendNegRsp 669,20549
static void SendNegRspWithPacing 693,21367
static boolean SendActluPosRsp 717,22146
static boolean SendInitself 737,22671
static void LogForLuApiWork 765,23567
static void LogActluPosRsp 776,23784
static void IfOpenLogForInitself 796,24325
boolean LuApiDoWork 818,25058
static void AckWorkComplete 888,27380
static void CompleteLuReceive 906,27886
static void CompleteSscpLuReceive 956,29431
static void CloseSscpLu 1012,30977
static void SaveOpenResult 1046,31917
static void SaveOpenResultWithSenseCode 1060,32262
static void CloseLu 1081,32827
static void CloseLuApis 1114,33690
static void CompleteOpen 1133,34187
static boolean CompleteBind 1153,34787
static void PacingQSend 1222,36798
static void PacingQRelease 1252,37655
static boolean SscpLuFsm 1304,40426
LuLuFsm 1705,54228
void LuApiReceiveSscpLu 1876,59341
void LuApiReceiveLuLu 1922,60595
void LuApiReceiveLuNmvtPsidRsp 1990,62463
static void LuApiStartLu 2013,62900
void LuApiResetLu 2026,63175
void LuApiLuDelete 2040,63580
static LuApiReturnCode FindPuForLuApiUse 2051,63789
static byte FindLocaddr 2079,64599
LuApiReturnCode luT0orT2QueryPu 2112,65436
static LuApiReturnCode luT0orT2Send 2126,65744
static LuApiReturnCode luT0orT2Receive 2172,67160
static LuApiReturnCode luT0orT2ReceiveContinue 2198,67861
static LuApiReturnCode luT0orT2Close 2220,68394
LuApiReturnCode luT0orT2Open 2304,70874
LuApiReturnCode sscpLuOpen 2463,76735
LuApiReturnCode sscpLuSend 2591,81167
LuApiReturnCode sscpLuReceive 2625,82122
LuApiReturnCode sscpLuReceiveContinue 2651,82825
LuApiReturnCode sscpLuClose 2673,83403
void LuApiShowLu 2743,85557
boolean LuApiQueryLuActive 2767,86246
boolean LuApiQueryLuBound 2778,86470
