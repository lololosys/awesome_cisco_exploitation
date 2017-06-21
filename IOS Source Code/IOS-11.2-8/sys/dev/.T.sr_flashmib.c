
sr_flashmib.c,3153
typedef struct copyQ_ copyQ_136,4995
} copyQ;copyQ142,5189
typedef struct partQ_ partQ_144,5199
} partQ;partQ150,5433
typedef struct miscopQ_ miscopQ_152,5443
} miscopQ;miscopQ158,5679
#define FLASHOP_ENTRY_AGEOUT 164,5787
#define DEF_ciscoFlashCopyCommand 171,5955
#define DEF_ciscoFlashCopyProtocol 172,6056
#define DEF_ciscoFlashCopyServerAddress	173,6142
#define DEF_ciscoFlashCopyDestinationName	174,6195
#define DEF_ciscoFlashCopyRemoteUserName	175,6241
#define DEF_ciscoFlashCopyNotifyOnCompletion 176,6286
#define DEF_ciscoFlashPartitioningCommand 179,6430
#define DEF_ciscoFlashPartitioningPartitionCount 180,6528
#define DEF_ciscoFlashPartitioningNotifyOnCompletion 181,6586
#define DEF_ciscoFlashPartitioningDestinationName 182,6691
#define DEF_ciscoFlashPartitioningPartitionSizes 183,6755
#define DEF_ciscoFlashMiscOpDestinationName	186,6868
#define DEF_ciscoFlashMiscOpNotifyOnCompletion	187,6922
init_flashmib 205,7658
ciscoFlashDevice_get 237,8806
ciscoFlashDeviceEntry_get 287,10201
ciscoFlashChipEntry_get 444,14929
ciscoFlashPartitionEntry_get 570,18713
ciscoFlashFileEntry_get 725,23444
copyQinsert 847,27040
copyQget 883,27829
static partQ *partQget 933,29085
static void partQinsert 969,30212
static miscopQ *miscopQget 998,30953
static void miscopQinsert 1034,32082
ciscoFlashCopyEntry_get 1071,33162
ciscoFlashCopyEntry_free 1215,37285
ciscoFlashCopyEntry_cleanup 1235,37931
ciscoFlashCopyEntry_test 1256,38705
ciscoFlashCopyEntry_set 1640,52395
ciscoFlashPartitioningEntry_get 1660,53072
ciscoFlashPartitioningEntry_free 1789,56915
ciscoFlashPartitioningEntry_cleanup 1806,57504
ciscoFlashPartitioningEntry_test 1827,58302
ciscoFlashPartitioningEntry_set 2171,72704
ciscoFlashMiscOpEntry_get 2189,73412
ciscoFlashMiscOpEntry_free 2308,76704
ciscoFlashMiscOpEntry_cleanup 2322,77144
ciscoFlashMiscOpEntry_test 2343,77924
ciscoFlashMiscOpEntry_set 2635,88756
k_ciscoFlashDevice_get 2647,89099
k_ciscoFlashDeviceEntry_get 2659,89423
k_ciscoFlashChipEntry_get 2732,92643
k_ciscoFlashPartitionEntry_get 2830,95834
k_ciscoFlashFileEntry_get 2934,100347
k_ciscoFlashCopyEntry_get 3061,104433
k_ciscoFlashPartitioningEntry_get 3104,105584
k_ciscoFlashMiscOpEntry_get 3140,106904
k_ciscoFlashCopyEntry_set 3179,108169
k_ciscoFlashPartitioningEntry_set 3340,114201
k_ciscoFlashMiscOpEntry_set 3474,119669
snmp_copy_ageout 3605,124549
static void snmp_flash_partition_ageout 3636,125247
static void snmp_flash_miscop_ageout 3662,125893
static forktype snmp_start_copy 3695,126812
snmp_partition_task 3865,133347
snmp_miscop_task 3919,135265
#define FLASH_COPY_TRAP_NUMBER 4084,141206
#define FLASH_COPY_TRAP_VARBIND_COUNT 4085,141246
static void snmp_flashcopy_notify_trap 4106,141921
#define PART_TRAP_VARBIND_COUNT 4160,143679
#define PART_TRAP_NUMBER 4161,143713
snmp_partition_completion_trap 4183,144444
#define  MISCOP_TRAP_VARBIND_COUNT 4237,146218
#define  MISCOP_TRAP_NUMBER 4238,146256
snmp_miscop_completion_trap 4260,146965
#define FLASHMIB_MAJVERSION 4316,148576
#define FLASHMIB_MINVERSION 4317,148606
#define FLASHMIB_EDITVERSION 4318,148636
