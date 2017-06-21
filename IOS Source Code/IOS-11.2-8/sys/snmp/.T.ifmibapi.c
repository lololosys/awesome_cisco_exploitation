
ifmibapi.c,1671
ifmib_find_snmpidb 158,5470
ifStackQinsert 177,5783
ifRcvAddrQinsert 210,6590
ifRcvAddrQclean 252,7726
ifStackQclean 282,8323
extern inline void snmpidb_init_common 307,8835
extern inline snmpidbtype *create_snmpidb 319,9118
extern inline snmpidbtype *get_snmpidb 343,9801
extern inline void enqueue_snmpidb 365,10174
extern inline void unqueue_snmpidb 379,10441
static void free_snmpidb 393,10728
extern inline boolean can_create_more_snmpidbs 405,10939
extern inline uint get_next_snmp_if_index 416,11142
long ifmib_get_ifNumber 426,11287
ifmib_hwidb_get(466,12317
ifmib_2indices_get 503,13337
ifmib_swidb_get(553,14741
snmpidbtype *ifmib_snmpidb_get_by_type(631,17123
snmpidbtype *ifmib_snmpidb_get(694,18564
static long ifmib_register_if 715,19096
static boolean ifmib_if_deregister 761,20408
static long ifmib_register_hwidb 813,21741
static boolean ifmib_deregister_hwidb 822,21912
static long ifmib_register_swidb 836,22155
static boolean ifmib_deregister_swidb 845,22324
static long ifmib_register_iab 858,22547
static boolean ifmib_deregister_iab 867,22706
static long ifmib_register_subiab 879,22919
static boolean ifmib_deregister_subiab 889,23137
static boolean ifmib_register_subif 898,23344
static void ifmib_deregister_subif 1018,27242
static void ifmib_delete_subif_default 1093,28966
ifRcvAddressEntry_t * ifmib_create_rcvaddr 1098,29084
void ifmib_destroy_rcvaddr 1152,30717
ifStackEntry_t * ifmib_create_stacklink 1182,31471
void ifmib_destroy_stacklink 1215,32369
init_ifmib_api 1244,32911
#define MAJVERSION_ifmib_api 1281,34614
#define MINVERSION_ifmib_api 1282,34645
#define EDITVERSION_ifmib_api 1283,34676
