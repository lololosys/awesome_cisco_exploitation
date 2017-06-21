
at_domain.c,4663
    ATTIMER_DOMAIN_ZONE,121,4091
    ATTIMER_DOMAIN_LOOP,122,4116
    ATTIMER_DOMAIN_GC123,4141
#define AT_DOMAIN 126,4167
#define ZIP_QUERY_INTERVAL 129,4189
#define DOMAIN_MAX_ZIP_RETRIES 130,4230
#define Protected(Protected131,4267
#define LOOP_MIN_DATA_SIZE 133,4312
#define LOOP_NAME_SIZE 134,4342
#define LOOP_MAX_RETRIES 135,4371
#define LOOP_NBP_ID 137,4401
typedef struct TempRouteList_ TempRouteList_151,4576
} TempRouteList;TempRouteList157,4720
#define ATDOMAIN_MAJVERSION	298,10900
#define ATDOMAIN_MINVERSION	299,10930
#define ATDOMAIN_EDITVERSION	300,10960
atdomain_init 312,11284
void atdomain_parse 335,11949
atdomain_remap_parse 450,14563
atdomain_parse_no_remap 551,17040
void atdomain_parse_no_domain 582,17774
atdomain_group_parse 614,18449
UpdateDomain 689,20350
atdomain_DeleteDomain 743,21586
DeleteAllSubDomains 784,22529
DeleteSubDomain 805,22933
CleanUpDomain 830,23533
DeleteGeneralDomainList 888,24837
CleanSubDomain 911,25284
DeleteAvRemapList 947,26265
DeleteRemapTable 980,27037
atdomain_DisablePort 1013,27811
atdomain_InitialAdjust 1048,28610
PutNewIdb 1137,30801
atdomain_DeleteIDB_fromDomain 1183,31735
ActiveDomain 1224,32675
FindDomain_by_idb 1257,33400
atdomain_DDPinProcess 1283,33938
atdomain_DDPoutProcess 1348,35433
atdomain_InsertNeighbor 1411,36879
atdomain_DeleteNeighbor 1467,38093
atdomain_Initialize 1527,39409
atdomain_PathUpdate 1571,40397
CreateOUTRemaps 1611,41367
atdomain_RemapUpdate 1663,42975
atdomain_CreateRemap 1701,43890
atdomain_NetSanityCheck 1737,44757
atdomain_GetRemapOut 1777,45564
atdomain_FilterNet 1815,46581
Domain_of_Net 1847,47309
atdomain_ProcessRTMP 1869,47778
atdomain_RemapProcess 2101,54157
Domain_ShutDown 2168,55883
atdomain_FindForwardRemap 2214,56899
atdomain_FindBackwardRemap 2253,57820
atdomain_ndb_new 2291,58773
atdomain_route_done 2327,59573
atdomain_ndb_done 2370,60526
atdomain_DeleteDomainRoute 2408,61460
CreateAvRemap 2493,63597
atdomain_ZipXReply 2518,64085
atdomain_ProcessZIP 2607,66584
atdomain_ZIPReply 2637,67288
atdomain_CheckForDiscovery 2731,69797
ClearRemapZones 2775,70889
Domain_FinishZipMerge 2801,71411
RemoveUnZippedEntry 2827,71995
GetDomainNeighbor 2861,72738
Domain_AddRemapZone 2884,73207
Domain_AddRemapRoute 2918,73992
DeleteTempList 2966,75272
AddToTempRouteList 2994,75805
show_appletalk_remap 3029,76446
atdomain_showSubDomainRemap 3071,77371
atdomain_showDomainRemap 3103,78307
show_appletalk_domain 3118,78682
ShowDomain 3150,79339
ShowRemap 3192,80511
#define TO_DOMAIN 3199,80658
#define FROM_DOMAIN 3200,80678
atalk_show_pvt_remap 3258,81950
showSubDomainAv 3285,82417
at_LoopForDomain 3309,83001
LoopInspection 3341,83698
LoopSuspect 3380,84619
StartLoopDetection 3427,85737
SendLoopPackets 3476,86883
SendLoop 3503,87499
atdomain_NBPLoop 3644,90631
at_domain_nv_write 3728,92457
hop_reduction_parse 3766,93355
hop_preservation_parse 3803,94188
domain_start 3843,95035
domain_process 3885,96042
atdomain_RemapNBP 3941,97524
LegitNet 4018,99006
ConnectedNet 4033,99369
*Remap_PutNewEntry 4061,100007
Add_to_UnZippedList 4114,101566
Domain_ZIP_GC 4166,102721
Domain_GC 4199,103437
Delete_DeadRemaps 4223,104004
Domain_Loop_Process 4253,104694
Domain_SendQueries 4300,105800
domain_SendRTMPNeighborQueries 4339,106646
GetNextUnqueried 4412,108813
at_FindGeneralDomainElement 4458,109798
Domain_AddNewZone 4482,110336
Find_DomainZone 4529,111346
GetAvailRemap 4557,111958
at_ReshuffleRemapList 4593,112922
at_FindAvRemapCell 4661,114544
ProtectRemap 4684,115049
UnProtectRemap 4698,115314
boolean atdomain_DeleteRemap 4712,115619
at_FindRemap 4798,118019
static void LockAvCell(4829,118673
atdomain_UnLockAvCell 4855,119275
at_PutAvRemapCell 4884,119975
at_RemapCellSanityCheck 4930,121099
at_DeleteAvRemapCell 4950,121593
atdomain_FindDomain_by_number 4982,122503
Collect_DomainNeighbors 5004,123009
at_insque 5020,123255
at_remque 5032,123467
Remap_GotoState 5053,123856
Delete_idbs_fromDomain 5097,124654
KeepFirstRemap 5128,125360
KeepNextRemap 5146,125741
InsertDomainRoute 5177,126337
Validate_Range 5200,126831
atdomain_CreateAvList 5261,128254
PrintRemapStatus 5294,128959
atdomain_CheckForLoop 5321,129533
ReleaseRemap 5359,130326
atdomain_PurgeAllDomainRoutes(5397,131167
atdomain_command 5415,131564
atdomain_if_command 5457,132325
CheckForDomainRouter 5498,133068
atdomain_InsideInboundRemap 5521,133484
atdomain_QueueEvent 5546,134092
atdomain_DeletePathEntry 5584,135111
atdomain_MatchAnyAURPTunnel 5622,136117
atdomain_PathInAURPInbound 5653,136916
atdomain_AURPTunnel 5677,137556
