
dlcsw.c,1522
static void dlcsw_init 239,8191
static boolean pcDluInit 247,8394
int pcDluReadPut(283,9259
boolean pcdluBlock(313,9858
forktype pcdlu_main 321,9975
static TDLUPort* DLUPortInit(490,14460
static TDLUSap* DLUSapInit(527,15378
static TDLUCvt* DLUCvtInit(557,16074
static TDLUCep* DLUCepInit(578,16506
static TDLUCep* DLUCheckCep(598,17008
static TDLUCep* DLUIdbToCep(614,17407
static idbtype* DLUCepToIdb(627,17693
uchar get_free_sap(632,17803
boolean DLUBackupEnable(656,18373
static void DLURemoveBackupCep(735,21065
void DLUReplaceBackupCep(757,21665
static void DLURemoveBanCep(781,22583
static boolean BanCmdValid(811,23517
ushort DLUGetBanDlci(845,24600
void DLUBanCepEnable(873,25430
void DLUBanEnable(925,26882
void pcdlu_command 999,29164
void DLUPortActSap(1454,44139
TDLUCep* DLUGetCep(1493,45328
TDLUCep* DLUFindNewCep(1515,45709
void DLUFsData(1601,48012
static void displayCep 1611,48297
static void displayDlciBni 1658,50066
static void displayDlci 1681,50989
static void displayBanCep 1705,51952
static void show_dlcsw_ban_command 1747,53775
void show_dlcsw_command 1776,54545
boolean DLURouteDestroy(1803,55188
void DLUBanRouteDestroy(1851,56515
TDLUBackup *IsAnyBackup(1864,56891
uchar GetFrasMaxRetry(1869,56942
void DLUUpdateAvailDlci(1874,56997
void DLUUpdateDlciStatus(1900,57784
void frDLCIStatusChange_fras(1928,58516
boolean isDlciDown(1987,61245
#define DLCSW_MAJVERSION 2022,62083
#define DLCSW_MINVERSION 2023,62110
#define DLCSW_EDITVERSION 2024,62137
