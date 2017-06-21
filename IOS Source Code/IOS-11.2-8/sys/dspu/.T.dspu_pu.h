
dspu_pu.h,4933
#define __PU_H55,1756
#define WORK_LU_UP 65,1921
#define WORK_LU_DOWN 66,1951
#define WORK_DACTPU 67,1981
#define WORK_ACTPU 68,2011
#define WORK_NMVTRSP 69,2041
#define RqdActive 72,2119
#define RqdSendNmvtPsid 73,2143
#define NmvtPsidSent 74,2171
#define RqdPendDelete 75,2234
#define ConstructedBySna 76,2313
#define PuStartRqsted 77,2391
#define DddluSupported 78,2458
 struct ConnParmsT ConnParmsT80,2527
 struct RetryInfoT RetryInfoT85,2725
typedef byte PuWorkType;PuWorkType90,2940
        } PuActFailRsnE;PuActFailRsnE99,3191
typedef struct LuMap LuMap102,3276
        } LuMap;LuMap109,3629
struct Pu Pu115,3810
typedef enum PuCommand{PuCommand165,6564
typedef enum PuCommand{PuResetCmd, PuStopCmd, PuStartCmd}PuCommand;PuCommand165,6564
typedef enum T5State T5State167,6633
                      T5PendingInact, T5Max} T5State;T5State168,6700
typedef enum T5Input T5Input169,6754
} T5Input;T5Input183,7491
typedef enum T2State T2State193,7692
typedef enum T2State {T2Reset, T2Active, T2Busy, T2Max} T2State;T2State193,7692
typedef enum T2Input T2Input196,7759
} T2Input;T2Input213,8685
#define GetLuChildren(GetLuChildren305,10804
#define PuGetPoolRangeHeader(PuGetPoolRangeHeader308,10921
#define FreeDataIndBuffer(FreeDataIndBuffer317,11207
#define PuQueryActive(PuQueryActive320,11268
#define PuQueryUpPuActive(PuQueryUpPuActive323,11369
#define PuQueryUpPuInActive(PuQueryUpPuInActive326,11478
#define PuQueryUpPuBusy(PuQueryUpPuBusy329,11590
#define PuQueryDirection(PuQueryDirection332,11692
#define GetPuName(GetPuName335,11792
#define GetPuType(GetPuType339,11934
#define GetPuCfgXid(GetPuCfgXid342,12004
#define GetPuConfiguredIdb(GetPuConfiguredIdb345,12089
#define GetPuCfgWindowSize(GetPuCfgWindowSize348,12183
#define GetPuCfgMaxIFrame(GetPuCfgMaxIFrame351,12287
#define GetPuRcvdXidFmt(GetPuRcvdXidFmt354,12389
#define SetPuRcvdXidFmt(SetPuRcvdXidFmt357,12490
#define GetPuRcvdXid(GetPuRcvdXid360,12588
#define SetPuRcvdXid(SetPuRcvdXid363,12678
static inline void PuLuMapSetLuActive(365,12740
static inline void PuLuMapSetLuInactive(369,12859
static inline boolean PuLuMapIsLuActive(373,12984
#define PuIsDddluSupported(PuIsDddluSupported378,13153
#define GetPuNegWindowSize(GetPuNegWindowSize381,13264
#define GetPuNegMaxIFrame(GetPuNegMaxIFrame384,13368
#define GetPuLocalSapAddr(GetPuLocalSapAddr387,13469
#define GetPuRemoteSapAddr(GetPuRemoteSapAddr390,13553
#define GetPuAddrP(GetPuAddrP393,13642
#define GetPuAdaptorIdP(GetPuAdaptorIdP396,13727
#define GetPuDlcType(GetPuDlcType399,13814
#define GetPuDlcUnit(GetPuDlcUnit402,13914
#define GetPuDlcPort(GetPuDlcPort405,14014
#define GetPuMaxRetryCount(GetPuMaxRetryCount408,14120
#define GetPuMaxRetryTimeout(GetPuMaxRetryTimeout411,14222
#define GetPuByteSents(GetPuByteSents414,14321
#define GetPuByteReceives(GetPuByteReceives417,14410
#define GetPuFrameSents(GetPuFrameSents420,14500
#define GetPuFrameReceives(GetPuFrameReceives423,14591
#define GetPuNegativeRspSents(GetPuNegativeRspSents426,14688
#define IncrPuNegativeRspSents(IncrPuNegativeRspSents429,14792
#define GetPuNegativeRspReceives(GetPuNegativeRspReceives432,14900
#define IncrPuNegativeRspReceives(IncrPuNegativeRspReceives435,15013
#define GetPuSscpState(GetPuSscpState438,15118
#define GetPuLsP(GetPuLsP441,15203
#define GetPuLsState(GetPuLsState444,15274
#define GetPuChildLocP(GetPuChildLocP447,15363
#define GetPuIndex(GetPuIndex450,15443
#define GetPuPoolRangeHdr(GetPuPoolRangeHdr453,15533
#define IsPuDefault(IsPuDefault456,15625
#define IsPuSna(IsPuSna459,15710
#define IsPuStarted(IsPuStarted462,15813
#define IsPuRetryCountInfinite(IsPuRetryCountInfinite465,15928
#define GetPuStartTime(GetPuStartTime469,16070
#define SetPuStartTime(SetPuStartTime472,16152
#define GetPuStateChgTime(GetPuStateChgTime475,16261
#define SetPuStateChgTime(SetPuStateChgTime478,16352
#define GetPuActFailCnt(GetPuActFailCnt481,16456
#define IncrPuActFailCnt(IncrPuActFailCnt484,16542
#define GetPuLastActFailRsn(GetPuLastActFailRsn487,16643
#define SetPuLastActFailRsn(SetPuLastActFailRsn490,16763
#define GetPuDspuMibNextPuP(GetPuDspuMibNextPuP493,16877
#define SetPuDspuMibNextPuP(SetPuDspuMibNextPuP496,16997
#define GetPuDspuMibPuIdx(GetPuDspuMibPuIdx499,17112
#define SetPuDspuMibPuIdx(SetPuDspuMibPuIdx502,17225
#define PuGetParentLs(PuGetParentLs505,17290
#define SscpPuSend(SscpPuSend509,17412
#define SscpPuRequestToSend(SscpPuRequestToSend512,17525
#define PuActivateLLC(PuActivateLLC521,17800
#define PuActivateQLLC(PuActivateQLLC524,17886
#define FindActiveUpPu(FindActiveUpPu589,19887
#define FindActiveDownPu(FindActiveDownPu590,19954
#define FindInactivePu(FindInactivePu599,20275
#define FindInactiveUpPu(FindInactiveUpPu602,20385
#define FindInactiveDownPu(FindInactiveDownPu603,20456
