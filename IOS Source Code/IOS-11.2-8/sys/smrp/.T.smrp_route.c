
smrp_route.c,2229
static struct smrproute_globals_ smrproute_globals_152,5627
#define smrp_routeMasterTimer 159,5924
#define smrp_routeHoldDownTimer 160,5992
#define smrp_routePurgeTimer 161,6062
#define smrp_ageCacheTimer 162,6129
    SMRP_AGECACHE_TIMER,172,6388
    SMRP_HOLDDOWN_TIMER,173,6413
    SMRP_PORTPARENT_TIMER,174,6438
    SMRP_PURGEROUTE_TIMER175,6465
smrp_NetRangeCompare 202,7251
smrp_NetRangeIdentical 221,7860
smrp_RBTreeNetRangeMax 237,8292
smrp_RBTreeNetRangeCompare 254,8771
smrp_RouteCount 261,8921
smrp_GetFirstRoute 267,9004
smrp_FindRoute 281,9421
smrp_FindRouteByNet 301,9924
smrp_RouteChanged 320,10362
smrp_FindPortParentLink 328,10543
smrp_FixupPortParentLink 349,10866
smrp_FixupPortParentLinks 390,11791
smrp_RouteGetPortParentAddress 408,12278
smrp_RouteGetPortParentDistance 427,12686
smrp_RouteAreWePortParentNode 451,13323
smrp_RouteSetPortParentInfo 476,13979
smrproute_IsFirstPreferred 493,14342
static void smrp_checkStalePortParents 504,14572
smrp_LinkPortToRoute 574,16459
smrp_AddPortToRoute 611,17459
smrp_AddAllPortsToRoute 630,18039
smrp_AddPortByRoute 644,18451
smrp_AddPortToAllRoutes 662,18945
smrp_DeletePortFromRoute 676,19357
smrp_DeletePortFromAllRoutes 716,20336
smrp_DeleteRoutePortParents 730,20757
smrp_ClearRouteOpData 752,21335
smrp_InitRouteOp 766,21794
smrp_CleanupRouteOp 794,22478
smrp_ResetRouteOpData 810,22929
smrp_RouteOpQualify 824,23320
smrp_AddVector 847,24054
smrp_SendRoutingTable 888,25322
smrp_DoNodeRoutingUpdate 965,27667
smrp_SendRoutingUpdateToOtherNodes 998,28504
smrp_IsPortRoute 1015,29001
smrp_CreateRoute 1042,29726
smrp_DeleteRoute 1105,31433
smrp_PortRouteChange 1144,32604
smrp_HandleHolddownTimeout 1232,35092
smrp_StartHoldDownTimer 1243,35305
smrp_AgeRoute 1264,36003
smrp_route_do_timers 1286,36433
smrp_ProcessDistanceVector 1329,37550
smrp_DoDistanceVectorRequest 1572,46613
smrp_PoisonRoute 1678,49622
smrp_PoisonRoutesFromNode 1709,50446
smrp_PoisonRoutesForPort 1732,51048
smrp_ForAllRoutesDo 1753,51543
smrp_CheckNetRange 1766,51798
smrp_route_teardown 1791,52363
smrp_route 1812,52994
smrp_route_Initialize 1891,55038
smrp_FixupRoute 1930,56208
void smrp_FixupRouteDistances 1939,56389
