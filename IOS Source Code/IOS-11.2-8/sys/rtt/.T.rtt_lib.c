
rtt_lib.c,14311
#define TMPBUFF_SIZE 99,3136
static void updateRttMonApplTimeOfLastSet 102,3186
void rttMon_get_boot_epoch 273,6539
void rttMonConvertAsciiToEbcdic 299,7332
void rttMonIPToOctetString 316,7668
void rttMonFreeOctetStringBuffer 330,8005
static void updateOctetsInUse 341,8233
static void *rttMalloc 350,8409
static OctetString *preMallocOctetString 376,8941
OctetString *rttMonPreMallocOctetString 418,10078
void populateOctetString 423,10184
void appendOctetString 451,10818
static void cloneToPreAllocatedOctetString 480,11462
static void scrubOctetString 507,12198
static int RttCmpOS 529,12692
static long getMaxRttMonAddressSize 565,13536
static void RTT_Increment 650,16432
static void freeRttMonReactTriggerAdminQItem 667,16623
static void freeRttMonReactTriggerAdminQ 687,17212
static rttMonReactTriggerAdminQItem *newRttMonReactTriggerAdminQItem 722,18381
static rttMonReactTriggerAdminQHead *newRttMonReactTriggerAdminQ 754,19444
*getUnusedRttMonReactTriggerAdminQItem 801,20860
static rttMonReactTriggerAdminQItem *findRttMonReactTriggerAdminQItem 817,21367
*findNextRttMonReactTriggerAdminQItem 873,23336
static int updateRttMonReactTriggerAdminQItem 928,25222
static void freeRttMonStatsCaptureDistQItem 969,26486
static void scrubRttMonStatsCaptureDistQItem 989,27078
static void freeRttMonStatsCaptureDistQ 1020,27936
static void scrubRttMonStatsCaptureDistQ 1053,28934
static rttMonStatsCaptureDistQItem *newRttMonStatsCaptureDistQItem 1073,29462
static rttMonStatsCaptureDistQHead *newRttMonStatsCaptureDistQ 1110,30757
static rttMonStatsCaptureDistQItem *findRttMonStatsCaptureDistQItem 1160,32042
*findNextRttMonStatsCaptureDistQItem 1217,33758
void freeRttMonStatsCollectEntry 1278,35435
static void freeShortRttMonStatsCollectEntry 1295,35809
static void scrubShortRttMonStatsCollectEntry 1316,36326
static void freeRttMonStatsCaptureHopQItem 1354,37346
static void scrubRttMonStatsCaptureHopQItem 1380,38155
static void freeRttMonStatsCaptureHopQ 1399,38610
static void scrubRttMonStatsCaptureHopQ 1432,39583
*newShortRttMonStatsCollectEntry 1453,40133
static rttMonStatsCaptureHopQItem *newRttMonStatsCaptureHopQItem 1508,41876
static rttMonStatsCaptureHopQHead *newRttMonStatsCaptureHopQ 1558,43520
*findRttMonStatsCaptureHopQItemByAddress 1612,44934
static rttMonStatsCaptureHopQItem *findRttMonStatsCaptureHopQItem 1636,45574
static rttMonStatsCaptureHopQItem *findNextRttMonStatsCaptureHopQItem 1689,47128
static void freePathList 1746,48713
static pathList *newRttMonPathListItem 1766,49162
static void freeRttMonStatsCapturePathQItem 1806,49998
static void scrubRttMonStatsCapturePathQItem 1828,50643
static void freeRttMonStatsCapturePathQ 1843,50999
static void scrubRttMonStatsCapturePathQ 1874,51984
static rttMonStatsCapturePathQItem *newRttMonStatsCapturePathQItem 1898,52660
static rttMonStatsCapturePathQHead *newRttMonStatsCapturePathQ 1939,53906
*getUnusedRttMonStatsCapturePathQItem 1995,55342
static rttMonStatsCapturePathQItem *findRttMonStatsCapturePathQItem 2011,55794
*findNextRttMonStatsCapturePathQItem 2066,57432
static void freeRttMonStatsCaptureStartTimeQItem 2123,59032
static void scrubRttMonStatsCaptureStartTimeQItem 2149,59848
static void freeRttMonStatsCaptureStartTimeQ 2168,60426
static void scrubRttMonStatsCaptureStartTimeQ 2203,61588
*newRttMonStatsCaptureStartTimeQItem 2229,62413
*newRttMonStatsCaptureStartTimeQ 2290,64417
*findRttMonStatsCaptureStartTimeQItem 2342,65783
*findNextRttMonStatsCaptureStartTimeQItem 2399,67643
static int updateRttMonStatsCaptureStartTimeQItem 2465,69698
void freeRttMonHistoryCollectionEntry 2521,71514
static void freeShortRttMonHistoryCollectionEntry 2543,72146
static void scrubShortRttMonHistoryCollectionEntry 2563,72711
static void freeRttMonHistoryCollectionSampleQItem 2607,74148
static void scrubRttMonHistoryCollectionSampleQItem 2628,74845
static void freeRttMonHistoryCollectionSampleQ 2644,75347
static void scrubRttMonHistoryCollectionSampleQ 2679,76499
*newShortRttMonHistoryCollectionEntry 2698,77061
*newRttMonHistoryCollectionSampleQItem 2756,78978
*newRttMonHistoryCollectionSampleQ 2799,80349
*findRttMonHistoryCollectionSampleQItem 2854,81841
*findNextRttMonHistoryCollectionSampleQItem 2913,83762
static void freeRttMonHistoryCollectionBucketQItem 2976,85700
static void scrubRttMonHistoryCollectionBucketQItem 2999,86441
static void freeRttMonHistoryCollectionBucketQ 3017,86944
static void scrubRttMonHistoryCollectionBucketQ 3052,88096
*newRttMonHistoryCollectionBucketQItem 3076,88838
*newRttMonHistoryCollectionBucketQ 3123,90350
*findRttMonHistoryCollectionBucketQItem 3177,91816
*findNextRttMonHistoryCollectionBucketQItem 3232,93634
static void freeRttMonHistoryCollectionLifeQItem 3292,95474
static void scrubRttMonHistoryCollectionLifeQItem 3315,96193
static void freeRttMonHistoryCollectionLifeQ 3330,96594
static void scrubRttMonHistoryCollectionLifeQ 3363,97692
*newRttMonHistoryCollectionLifeQItem 3388,98485
*newRttMonHistoryCollectionLifeQ 3430,99861
*findRttMonHistoryCollectionLifeQItem 3486,101277
*findNextRttMonHistoryCollectionLifeQItem 3541,103021
static int updateRttMonHistoryCollectionLifeQItem 3602,104853
static void freeConnectionHandleListItem 3660,106656
void rttMonFreeConnectionHandleList 3679,107155
static connectHandleList *newConnectionHandleListItem 3697,107594
static connectHandleList *newConnectionHandleListParent 3729,108625
static connectHandleList *getUnusedConnectionHandleItem 3786,110102
static connectHandleList *findConnectionHandle 3801,110471
static long collectConnectHandleList 3824,110971
static void copyEchoAdminEntry 3860,111772
static void copyCtrlAdminEntry 3887,112503
static rttMonCtrlAdminQItem *findRttMonCtrlAdminQItem 3960,115000
static rttMonCtrlAdminQItem *findNextRttMonCtrlAdminQItem 4008,116307
void initBaseRttMonCtrlAdminQ 4055,117613
void freeRttMonCtrlAdminEntry 4060,117717
static rttMonCtrlAdminEntry_t *newRttMonCtrlAdminEntryParent 4092,118631
rttMonCtrlAdminEntry_t *newRttMonCtrlAdminEntry 4168,121156
void freeRttMonEchoAdminEntry 4174,121276
static rttMonEchoAdminEntry_t *newRttMonEchoAdminEntryParent 4190,121608
rttMonEchoAdminEntry_t *newRttMonEchoAdminEntry 4225,122666
static void freeRttMonFileIOAdminEntry 4231,122786
static void freeRttMonScriptAdminEntry 4246,123100
static void freeRttMonCtrlAdminQItemMem 4266,123597
static int freeRttMonCtrlAdminQItemByIndexInternal 4354,126394
int freeRttMonCtrlAdminQItemByIndex 4381,127150
int freeRttMonCtrlAdminQItem 4396,127517
static int rttMonCtrlAdminQinsert 4414,127968
static pathList *newTempPathList 4454,128964
static void updateModificationTime 4499,130026
int newRttMonCtrlAdminQItem 4520,130518
static int createRttMonCtrlAdminQItemSecondaryMemoryInternal 4726,137717
int createRttMonCtrlAdminQItemSecondaryMemory 4885,142988
int rttMonCtrlAdminQItemSecondaryMemory 4908,143601
static void setOperState 4937,144436
static long transitionOperState 5028,147316
void freeRttMonCtrlAdminQ 5079,148774
int getRttMonCtrlAdminQSize 5168,151253
int getRttMonCtrlAdminEntry 5179,151402
int getNextRttMonCtrlAdminEntry 5231,152911
int getRttMonEchoAdminEntry 5283,154419
int getNextRttMonEchoAdminEntry 5308,155076
rttMonCtrlAdminQItem *getRttMonCtrlAdminQItem 5338,155770
int updateRttMonCtrlAdminEntry 5357,156158
int updateRttMonEchoAdminEntry 5455,159978
static void startProbe 5521,161895
int setRttMonCtrlAdminStatus 5681,166707
int setRttMonScheduleAdminRttLife 5889,173596
int setRttMonScheduleAdminRttStartTime 5917,174300
int setRttMonScheduleAdminConceptRowAgeout 5945,175022
int setRttMonReactAdminConnectionEnable 5998,176613
int setRttMonReactAdminTimeoutEnable 6026,177340
int setRttMonReactAdminThresholdType 6054,178059
int setRttMonReactAdminThresholdFalling 6092,179223
int setRttMonReactAdminThresholdCount 6130,180395
int setRttMonReactAdminThresholdCount2 6168,181564
int setRttMonReactAdminActionTypes 6206,182731
long getCurrentFrequency 6242,183880
long getCurrentOperState 6264,184393
int setRttMonCtrlOperState 6289,184934
rttMonStatsCollectEntry_t *newRttMonStatsCollectEntry 6320,185780
static void copyStatsCollectEntry 6349,186544
int getRttMonStatsCollectEntry 6401,188226
int getNextRttMonStatsCollectEntry 6480,190679
void freeRttMonReactTriggerAdminEntry 6621,195368
rttMonReactTriggerAdminEntry_t *newRttMonReactTriggerAdminEntry 6627,195515
int addUpdateRttMonReactTriggerAdminEntry 6640,195837
int setRttMonReactTriggerAdminStatus 6696,197564
int getRttMonReactTriggerAdminEntry 6740,198804
int getNextRttMonReactTriggerAdminEntry 6790,200190
void freeRttMonStatsCaptureDistEntry 6862,202383
rttMonStatsCaptureEntry_t *newRttMonStatsCaptureDistEntry 6868,202513
int getRttMonStatsCaptureDistEntry 6878,202697
int getNextRttMonStatsCaptureDistEntry 6990,206637
void freeRttMonStatsTotalsEntry 7188,213651
rttMonStatsTotalsEntry_t *newRttMonStatsTotalsEntry 7198,213812
int getRttMonStatsTotalsEntry 7204,213942
int getNextRttMonStatsTotalsEntry 7257,215637
rttMonHistoryCollectionEntry_t *newRttMonHistoryCollectionEntry 7332,218014
static void copyHistoryCollectionEntry 7368,219151
int getRttMonHistoryCollectionEntry 7426,221227
int getNextRttMonHistoryCollectionEntry 7507,223725
static void doEndLife 7647,228576
static void clearRttMonReactTriggerAdminEntries 7706,230257
*oldestStatsCaptureStartTimeQItem 7782,232739
*oldestHistoryCollectionLifeQItem 7789,232943
static long doStartLife 7795,233097
long rttMonStartLife 8037,241267
long getRttMonCtrlAdminRttType 8052,241580
long getRttMonEchoAdminProtocol 8068,241947
OctetString *rttMonNewAddressBuffer 8085,242322
OctetString *getRttMonEchoAdminTargetAddress 8094,242511
long getRttMonEchoAdminPktDataRequestSize 8115,243118
long getRttMonEchoAdminPktDataResponseSize 8133,243541
void setRttMonSchedulerPid 8151,243982
void rttMonSchedulerInit 8163,244239
void rttMonSchedulerCleanup 8204,245654
static void rttMonCleanupProcessDeath 8221,246135
static void rttMonProcessEndLife 8260,247150
static void rttMonProcessAgeOut 8369,250536
static void rttMonStartProcessing 8470,253627
static void rttMonCreateSourceToTargetVirgin 8506,254751
static void rttMonPerformEndHourProcessing 8563,256263
boolean rttMonSchedulerUpdate 8716,261252
static void rttMonUpdateHistoryBucketCurrPtr 8867,265782
static long rttMonUpdateHistorySampleCurrPtr 8959,268514
static void updateOper 9090,272538
static long updateHistory 9113,273133
static rttMonStatsCaptureDistQItem *findStatsCaptureDistQItem 9183,275172
static long updateStatsCaptureEntry 9210,275880
static void generateRttThresholdTrap 9319,278789
static void generateRttThresholdNmvt 9376,280783
static void generateRttConnectionTrap 9405,281679
static void generateRttConnectionNmvt 9462,283683
static void generateRttTimeoutTrap 9491,284593
static void generateRttTimeoutNmvt 9548,286547
static void generateRttTrigger 9577,287411
static void reactToConnectionLost 9629,289290
static void toggleConnectionLost 9676,290760
static void setConnectionLost 9700,291533
static void clearConnectionLost 9712,291883
static void reactToThreshold 9724,292229
static boolean checkConsecutiveThreshold 9771,293682
static int rttIncrementSubscriptXofY 9822,295385
static int rttIncrementSubscriptAve 9833,295586
static int numberOfEntriesXofY 9844,295785
static int numberOfEntriesAve 9863,296207
static int countXofYOccurred 9882,296633
static boolean checkXofYThreshold 9914,297278
static long averageOfEntries 10075,301684
static boolean checkAveThreshold 10110,302441
static void checkOverThreshold 10221,305577
static void reactToTimeout 10299,307882
static void toggleTimeOut 10346,309353
static void setTimeOut 10370,310064
static void clearTimeOut 10383,310382
static int setOverThreshold 10396,310705
long rttMonUpdateResponse 10414,311204
boolean rttMonCollectPath 10830,324254
static void rttMonStartUpdatePathInternal 10851,324744
void rttMonStartUpdatePath 10875,325321
static long rttMonUpdatePathInternal 10888,325606
long rttMonUpdatePath 10923,326458
static void setupDistQs 10942,326892
static void fillPathQItem 10978,327827
static rttMonStatsCapturePathQItem *findPathQItem 11028,329314
long rttMonEndUpdatePath 11102,331334
long rttMonGetFirstStoredHopAddress 11215,334971
long rttMonGetNextStoredHopAddress 11251,335917
long rttMonSetupASCIIDataPortion 11307,337416
long rttMonSetupEBCDICDataPortion 11427,340849
long rttMonVerifyASCIIDataPortion 11547,344368
long rttMonVerifyEBCDICDataPortion 11599,345644
long getRttMonCtrlAdminFrequency 11655,347097
long getRttMonCtrlAdminTimeout 11672,347479
static long transitionTransitionState 11693,347921
long rttMonTransitionTransitionState 11818,352319
long getRttMonOperState 11833,352680
void rttMonTimeStampEchoStart 11870,353598
unsigned long rttMonCalculateResponseTime 11888,354055
void rttMonProbePanicInternal 11910,354575
void rttMonProbePanic 11929,355112
void rttMonUpdateNumberOfEchosAttempted 11961,355964
boolean rttMonHopConnected 11991,356745
long rttMonSetHopConnectedState 12017,357342
long rttMonSetConnectionHandle 12045,357972
long rttMonGetConnectionHandle 12087,359065
connectHandleList *rttMonNewConnectionHandleList 12123,359922
long rttMonGetConnectionHandleList 12141,360341
long rttMonSetDiagText 12166,360920
boolean rttMonGetVerifyDataFlag 12194,361584
watched_boolean *rttMonGetPendingBoolean 12215,362053
void setRttMonCtrlAdminEntryDefaults 12231,362415
void setRttMonEchoAdminEntryDefaults 12459,371598
rttMonAppl_t *getRttMonApplPtr 12495,372770
void setRttMonAppPreConfigedReset 12500,372844
void setRttMonApplReset 12511,373006
*getRttMonApplSupportedRttTypesEntryPtr 12625,376374
*getNextRttMonApplSupportedRttTypesEntryPtr 12636,376691
static void populateSupportedProtocols 12645,376951
*getRttMonApplSupportedProtocolsEntryPtr 12667,377571
*getNextRttMonApplSupportedProtocolsEntryPtr 12683,378122
static void initRttMonApplEntry 12692,378389
static void initRttMonApplSupportedRttTypesTable 12718,379428
static void initRttMonApplSupportedProtocolsTable 12764,381805
void initGlobalRttInfo 13003,396225
void rttMonSetSelfID 13024,396800
