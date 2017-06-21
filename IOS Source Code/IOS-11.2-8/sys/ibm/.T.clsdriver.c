
clsdriver.c,1603
#define IS_XID 85,2959
#define IS_DEBUG 86,2977
int GnDluReadPut(97,3185
GnDLUEntity* GnDLUCreate(106,3374
GnDLUPort* GnDLUPortCreate(122,3676
void GnDLUAddPort(135,3936
GnDLUPort*  GnDLURemovePort(142,4079
GnDLUPort* GnDLUFindPort(154,4481
GnSapStruct* GnDLUFindSap(169,4869
void GnDLUPortDestroy(183,5199
GnSapStruct* GnDLUSapCreate(195,5403
GnDLUCep* GnDLUCepCreate(212,5764
GnDLUCep*  GnDLURemoveCep(228,6098
void GnDLUCepDestroy(241,6392
void GnShutdown(248,6492
void GnDLUShutdown 258,6646
GnDLUEntity* GnFindDlu(269,6838
GnDLUPort* GnEnablePort(282,7126
void GnDLUSendEnable(340,8508
void GnDisablePort(374,9582
void GnDLUSendDisable(384,9750
void GnDLUSendActListningSaps(410,10370
void GnDLUSendActSap(423,10730
void GnDLUSendReqOpnStn(469,12136
void GnDLUSendConnectReq(529,13960
void GnDLUSendModifyStnReq(559,14743
void GnDLUSendDataReq(593,15771
void GnDLUSendSignalStn(620,16474
void GnDLUSendDiscReq(646,17141
void GnDLUSendDiscRsp(672,17812
void GnDLUSendCloseStnReq(698,18485
void GnDLUSendDeactSap(725,19238
void clsdriver_start(755,20060
void clsdriver_open(799,21159
int clsdriver_send(807,21326
void clsdriver_close(816,21544
void clsdriver_abort(825,21718
boolean GnDluBlock(840,21938
forktype GnDluMain 849,22098
void hwaddr_to_dlci 1282,36024
void dlci_to_hwaddr(1290,36190
void GnDLUSendIDReq(1303,36484
void GnDLUSendIDRsp(1331,37163
void GnDLUSendConnectRsp(1360,37842
static void clsdriver_init 1392,38660
#define CLSDRIVER_MAJVERSION 1410,39055
#define CLSDRIVER_MINVERSION 1411,39086
#define CLSDRIVER_EDITVERSION 1412,39117
