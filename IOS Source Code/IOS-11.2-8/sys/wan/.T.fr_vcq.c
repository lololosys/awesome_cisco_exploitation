
fr_vcq.c,1666
#define FR_VCQ_MAJVERSION 134,4458
#define FR_VCQ_MINVERSION 135,4486
#define FR_VCQ_EDITVERSION 136,4514
#define QUEUING_NOT_ALLOWED 145,4726
static char *fr_vcq_queue2name 152,4870
static boolean fr_vcq_intq_chk 170,5278
static boolean fr_vcq_prilist_chk 200,6063
static inline struct VCQ *vcq_create 230,6564
static void fr_vcq_write 261,7379
static int fr_vcq_priority_qsize 297,8162
static ulong fr_vcq_custom_qbytecnt 320,8634
static void fr_vcq_reset 340,9265
void fr_vcq_mapparams 385,10202
boolean fr_vcq_map_assign 425,11289
void fr_vcq_mapclass_nvgen 439,11516
static void fr_vcq_unmap_vc 471,12124
static void fr_vcq_map_vc 492,12582
static void fr_vcq_mapclass_remove 510,12970
void fr_vcq_mapclass 526,13293
void fr_vcq_intf_queueing 609,15374
void fr_vcq_queue_add 632,15770
static void fr_vcq_onoff 688,16915
void fr_vcq_frclass 727,17740
void fr_traffic_command 753,18176
static void fr_vcq_idb_init 849,20510
static void fr_vcq_idb_uninit(862,20814
static int fr_vcq_priority_classify 878,21088
static boolean fr_vcq_priority_enqueue 1062,26393
static paktype *fr_vcq_priority_dequeue 1119,27643
static paktype *fr_vcq_custom_dequeue(1175,29032
boolean fr_vcq_fcfs_enqueue(1304,32615
paktype *fr_vcq_fcfs_dequeue 1342,33307
void fr_enqueue 1386,34512
paktype *fr_vc_dequeue 1424,35437
void fr_vcq_default_queueing(1463,36304
void fr_vcq_queue_setup 1485,36705
static void fr_vcq_queue_reset 1538,38014
static void fr_vcq_clear_counters 1560,38502
static void fr_vcq_holdq_display 1583,39016
static void fr_vcq_show_default 1628,40337
static void fr_vcq_show_queueing 1635,40520
void fr_vcq_init 1669,41144
