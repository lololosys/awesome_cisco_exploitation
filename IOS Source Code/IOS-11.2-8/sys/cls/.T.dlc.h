
dlc.h,5455
#define __DLC_H_140,5079
#define NO_CLS 152,5266
} ClsOpcodeT;ClsOpcodeT202,6280
} DlcOpcodeT;DlcOpcodeT252,7129
typedef ClsUSapT	*ClsUSapIdT;ClsUSapIdT260,7198
typedef DlcPSapT	*DlcPSapIdT;DlcPSapIdT261,7228
typedef ClsUCepT	*ClsUCepIdT;ClsUCepIdT262,7258
typedef ulong		DlcPCepIdT;DlcPCepIdT267,7312
typedef idbtype		*DlcPPortIdT;DlcPPortIdT268,7358
typedef struct LlcInitT_LlcInitT_276,7450
} LlcInitT;LlcInitT279,7501
typedef union DlcInitT_DlcInitT_283,7543
} DlcInitT;DlcInitT286,7593
typedef struct TREnableT_	TREnableT_292,7650
} TREnableT;TREnableT298,7943
#define DLC_RSRB_ENABLE_LACK_ON 303,7995
typedef struct RSRBEnableT_	RSRBEnableT_305,8042
} RSRBEnableT;RSRBEnableT314,8580
typedef struct VDLCEnableT_ VDLCEnableT_316,8596
} VDLCEnableT;VDLCEnableT323,8852
typedef struct NCIAEnableT_NCIAEnableT_325,8868
} NCIAEnableT;NCIAEnableT333,9336
typedef struct TBRIDGEEnableT_TBRIDGEEnableT_361,10591
} TBRIDGEEnableT;TBRIDGEEnableT369,10896
typedef struct ENEnableT_	ENEnableT_371,10915
} ENEnableT;ENEnableT376,11144
typedef struct FDDIEnableT_	FDDIEnableT_378,11158
} FDDIEnableT;FDDIEnableT383,11385
typedef struct FREnableT_	FREnableT_385,11401
} FREnableT;FREnableT390,11597
typedef struct SDLCEnableT_	SDLCEnableT_392,11611
} SDLCEnableT;SDLCEnableT395,11690
typedef struct QLLCEnableT_	QLLCEnableT_397,11706
} QLLCEnableT;QLLCEnableT400,11785
} DlcEnableT;DlcEnableT433,12644
} DlcDisableT;DlcDisableT442,12903
typedef struct LlcActSapT_LlcActSapT_447,12941
} LlcActSapT;LlcActSapT461,13435
typedef struct SdlcActSapT_SdlcActSapT_463,13450
} SdlcActSapT;SdlcActSapT467,13524
typedef struct QLLCActSapT_QLLCActSapT_469,13540
} QLLCActSapT;QLLCActSapT477,13816
typedef struct VDLCActSapT_VDLCActSapT_479,13832
} VdlcActSapT;VdlcActSapT484,14004
typedef LlcActSapT NciaActSapT;NciaActSapT486,14020
typedef struct DlcActSapT_DlcActSapT_488,14053
} DlcActSapT;DlcActSapT502,14519
typedef enum XidT_XidT_516,14884
} XidT;XidT520,14945
typedef enum TestT_TestT_523,15016
} TestT;TestT527,15080
typedef enum UDataT_UDataT_531,15162
} UDataT;UDataT535,15228
#define TR_MAX_RIF_LENGTH 537,15239
typedef struct DlcXidIdStnT_DlcXidIdStnT_549,15562
} DlcXidIdStnT;DlcXidIdStnT564,16123
typedef DlcXidIdStnT LlcXidIdStnT;LlcXidIdStnT566,16140
typedef DlcXidIdStnT SdlcXidIdStnT;SdlcXidIdStnT567,16175
typedef struct DlcIdStnT_DlcIdStnT_569,16212
} DlcIdStnT;DlcIdStnT605,17503
typedef struct DlcIdT_DlcIdT_607,17517
} DlcIdT;DlcIdT627,18257
typedef struct LlcTestStnT_LlcTestStnT_629,18268
} LlcTestStnT;LlcTestStnT645,18847
typedef LlcTestStnT SdlcTestStnT;SdlcTestStnT647,18863
typedef struct DlcTestStnT_DlcTestStnT_659,19190
} DlcTestStnT;DlcTestStnT694,20291
typedef struct DlcTestT_DlcTestT_697,20308
} DlcTestT;DlcTestT707,20614
typedef struct LlcUdataStnT_LlcUdataStnT_719,20918
} LlcUdataStnT;LlcUdataStnT735,21498
typedef struct DlcUdataStnT_DlcUdataStnT_737,21515
} DlcUdataStnT;DlcUdataStnT772,22559
typedef struct LlcConnectStnT_LlcConnectStnT_784,22866
} LlcConnectStnT;LlcConnectStnT800,23448
typedef struct DlcConnectStnT_DlcConnectStnT_802,23467
} DlcConnectStnT;DlcConnectStnT830,24365
typedef struct LlcActRngT_LlcActRngT_832,24384
} LlcActRngT;LlcActRngT837,24628
typedef LlcActRngT SdlcActRngT;SdlcActRngT839,24643
typedef LlcActRngT QLLCActRngT;QLLCActRngT840,24675
typedef LlcActRngT VdlcActRngT;VdlcActRngT841,24707
typedef LlcActRngT NciaActRngT;NciaActRngT842,24739
typedef struct DlcActRngT_DlcActRngT_844,24772
} DlcActRngT;DlcActRngT861,25376
typedef struct LlcDeActRngT_LlcDeActRngT_863,25391
} LlcDeActRngT;LlcDeActRngT868,25637
typedef struct VdlcDeActRngT_VdlcDeActRngT_870,25654
} VdlcDeActRngT;VdlcDeActRngT873,25727
typedef LlcDeActRngT NciaDeActRngT;NciaDeActRngT875,25745
typedef struct DlcDeActRngT_DlcDeActRngT_877,25782
} DlcDeActRngT;DlcDeActRngT893,26354
typedef struct LlcReqOpenStnT_LlcReqOpenStnT_899,26402
} LlcReqOpenStnT;LlcReqOpenStnT931,27714
typedef struct VdlcReqOpenStnT_VdlcReqOpenStnT_933,27733
} VdlcReqOpenStnT;VdlcReqOpenStnT941,28082
typedef struct NciaReqOpenStnT_NciaReqOpenStnT_943,28102
} NciaReqOpenStnT;NciaReqOpenStnT953,28463
typedef struct SdlcReqOpenStnT_SdlcReqOpenStnT_955,28483
} SdlcReqOpenStnT;SdlcReqOpenStnT970,29077
typedef struct QLLCReqOpenStnT_QLLCReqOpenStnT_977,29163
} QLLCReqOpenStnT;QLLCReqOpenStnT980,29227
typedef struct DlcReqOpenStnT_DlcReqOpenStnT_982,29247
} DlcReqOpenStnT;DlcReqOpenStnT1004,30055
typedef struct DlcConnectT_DlcConnectT_1006,30074
} DlcConnectT;DlcConnectT1025,30768
typedef struct DlcDisconnectT_DlcDisconnectT_1027,30784
} DlcDisconnectT;DlcDisconnectT1035,31096
typedef struct DlcCloseStnT_DlcCloseStnT_1037,31115
} DlcCloseStnT;DlcCloseStnT1051,31535
}  ClsDlcParamBlockT;ClsDlcParamBlockT1103,33193
static inline idbtype *DlcToIdb 1120,33491
typedef void clsLlcMapFnT;clsLlcMapFnT1131,33687
typedef void clsSdlcMapFnT;clsSdlcMapFnT1132,33714
typedef void clsQllcMapFnT;clsQllcMapFnT1133,33742
typedef void clsNciaMapFnT;clsNciaMapFnT1134,33770
typedef struct ClsSdlcMapT_ClsSdlcMapT_1136,33799
} ClsSdlcMapT;ClsSdlcMapT1139,33913
typedef struct ClsQLLCMapT_ClsQLLCMapT_1141,33929
} ClsQLLCMapT;ClsQLLCMapT1144,34037
typedef struct ClsDlcMapT_ClsDlcMapT_1146,34053
} ClsDlcMapT;ClsDlcMapT1149,34157
