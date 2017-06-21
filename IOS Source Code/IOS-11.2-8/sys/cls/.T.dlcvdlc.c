
dlcvdlc.c,2761
#define INLINE210,7957
static void VdlcInit 224,8233
#define VDLC_MAJVERSION 239,8617
#define VDLC_MINVERSION 240,8643
#define VDLC_EDITVERSION 241,8669
static INLINE void VdlcStart 254,8924
VdlcEnableReq 314,10594
static void VdlcDisableReq 412,13240
static INLINE void VPortActivateSapReq 455,14452
static INLINE  void VPortModifySapReq 515,16230
static INLINE  void VPortDeactivateSapReq 532,16755
static INLINE  void VPortActivateRngReq 588,18195
static INLINE void VPortDeActivateRngReq 645,19575
static INLINE  void VSapReqOpnStnReq 761,22750
static boolean VCepRendezvous 897,26443
static INLINE  void VCepCloseStnReq 944,27863
VCepEventInput 990,29222
VCepProcessTimer 1011,29560
VCepProcessCloseStnInd 1092,31476
static INLINE void VSapHandleMsgs 1201,34461
static INLINE void VCepHandleMsgs 1364,38714
static boolean VCepSendToPartnerSap 1438,40739
static boolean VCepHandleTestResponse 1638,45803
static INLINE ClsOpcodeT DlcToClsOpcode 1659,46364
VdlcTellPartner 1688,47347
static void VCepSetStateFromDlcOpcode 1727,48348
static void VCepSetStateFromClsOpcode 1753,48961
VCepHandleClosingMsgs 1780,49556
VCepModifyStnReq 1832,50624
static INLINE  void VCepDataReq 1836,50688
static INLINE  void VCepFlowReq 1874,51631
static void VCepPongReq 1901,52323
static void VdlcWput 1924,52869
static boolean VCepClsNotify 2043,55628
static ClsDlcParamBlockT* VCepBuildParamBlock 2074,56504
static process vdlc_background 2158,58725
ClsUSapIdT VdlcGetBridgingSapId 2216,60193
#define MAXSTRING 2260,61231
static char *VCepPrState 2262,61253
static void VPrintDteString 2278,61525
static void VdlcDefaultPort 2301,62233
static VrgT *VrgCreate 2350,63363
static void VrgDestroy 2373,63793
static boolean VrgAddPort 2397,64214
static VrgT *VdlcLookupVrgByPortId 2402,64356
static VPortT *VrgLookupPortByPortId 2419,64665
static VPortT *VPortCreate 2454,65716
static void VPortDestroy 2492,66680
TCLSIMsg *VPortGetRingReq(2508,66946
static int VPortCompareMac 2531,67377
static int VPortCompareIdb(2542,67621
static VSapT *VSapCreate 2563,68349
static void VSapDestroy 2578,68676
static int VSapCompareUSapId 2595,68958
static int VSapCompareValue 2608,69193
static boolean VCepMatchUCepId 2619,69406
static VCepT *VSapLookupCepByUCepId(2639,69830
static boolean VSapSendUControlStnInd 2661,70174
static void vdlcUDataStnReq(2705,71281
static VCepT *VCepCreate 2763,73635
static void  VCepDestroy 2789,74365
VCepSendTest 2831,75123
static void VCepExploreLocal(2859,75699
static void VCepNoPartner 2897,76555
static void VCepBuildPartnerDteString 2908,76762
VCepCompare 2923,77197
static void VCepSetState 2943,77548
static void VCepTimerStart 2966,78102
static void VCepTimerStop 2972,78216
