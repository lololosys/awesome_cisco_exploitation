
route1.c,3733
void ndb_free 406,15731
static void backup_free 428,16237
void net_init 446,16606
void table_init 520,19064
void table_reload 621,21509
boolean route_interface 646,22351
boolean route_ifaddress 702,23977
void ip_route_adjust 720,24499
static inline ndbtype *net_lookup_inline 841,28211
ndbtype *net_lookup 857,28596
static inline ndbtype *subnet_lookup_inline 869,28926
ndbtype *subnet_lookup 906,29977
static inline ndbtype *supernet_lookup_inline 920,30413
ndbtype *supernet_lookup 972,32039
static inline ndbtype *netsuper_lookup_inline 985,32422
ndbtype *netsuper_lookup 1005,32914
static inline boolean iprouting_allnets_broadcast 1017,33212
iprouting_first_hop 1036,33717
ipaddrtype iprouting_first_hop_on_idb 1145,37045
ulong ip_best_metric 1195,38395
ndbtype *ip_rtlookup 1256,39669
ndbtype *ip_rtlookupdummy 1293,40670
static ndbtype *iprouting_rtlookup_exact 1327,41577
idbtype *route_lookup 1354,42128
ipaddrtype route_lookup_on_idb 1478,46226
int get_nets_per_address 1542,47971
int get_nets_per_idb 1562,48384
ndbtype *ndb_malloc 1588,49028
ndbtype *network_create 1608,49464
static boolean iprouting_exterior_change 1656,50664
void network_redist 1692,51619
void invalidate_redist 1762,53681
ipaddrtype ip_ifmask 1798,54507
acl_route_debug 1832,55427
iprouting_cache_clean 1853,55787
free_all_rdbs 1865,56041
network_update 1885,56405
void rdb_delete 2611,78944
int ip_find_rdb_index 2660,80381
boolean simple_variance 2675,80664
boolean simple_holddown 2690,81091
static inline boolean ip_free_sdb 2717,82017
static inline void ip_free_ndb 2769,83441
boolean table_ager 2797,84109
void netarray_ager 2979,89187
void age_all_routes 3004,89803
void route_compress 3033,90410
inline boolean ip_valid_splithorizon 3097,91981
inline boolean validnextnetwork 3176,94133
ndbtype *getnextnetwork 3277,97363
ndbtype *find_valid_subnet 3394,100640
boolean ndb_is_up 3431,101628
void ip_route_new_default 3463,102569
ip_secondary 3645,108100
void table_walk 3705,109878
ndbtype *prune_ndb_idb 3752,111039
void table_prune_idb 3810,112417
ndbtype *prune_ndb_pdb 3829,112847
void table_prune_pdb 3883,114154
ndbtype *ip_ndb_subnet 3896,114507
boolean ip_ndb_variable 3938,115653
void ip_ndb_addmask 3962,116311
void ip_supernet_addmask 4022,117887
static void ip_ndb_unsubnet 4067,119078
void ip_ndb_removemask 4107,120090
void ip_supernet_removemask 4155,121333
backuptype *check_backup_info 4179,121878
backuptype *build_backup_info 4208,122674
ndbtype *geteachroute 4266,124126
boolean route_add_neighbor 4383,126597
void callbackup_routes 4398,127010
void ip_route_delete 4450,128489
void ip_robin_setup 4601,132456
enum DISTTYPE iprouting_distcmp_generic 4670,134036
static process iprouting_background 4683,134358
void iprouting_background_onoff 4728,135615
boolean iprouting_cache_mask 4763,136863
boolean iprouting_recursive(4835,138798
static boolean iprouting_nexthop_valid 4859,139206
boolean iprouting_destination_valid 4913,140348
static void ip_route_start_aging_core 4971,141903
static void ip_route_start_aging_work 5027,143267
static void ip_route_start_aging_nets 5053,143944
void ip_route_start_aging 5083,144710
void iprouting_shutdown 5109,145203
void iprouting_get_netsmask 5145,146088
void iprouting_delete_conn_route 5169,146634
void iprouting_delete_conn_route_int 5181,146946
boolean iprouting_delete_route 5194,147218
void iprouting_gdp_default_ndb_prio 5212,147630
static ndbtype *iprouting_lexcmp_rt 5243,148382
boolean iprouting_snmp_rtlookup_exact 5272,149005
boolean iprouting_snmp_rtlookup_lexnext 5302,149854
boolean iprouting_via_nonsubnet 5348,151122
boolean iprouting_conn_plimit_exceed 5368,151482
