
ospf_spf.c,3338
inline boolean acl_ospf_spf_intra_debug 302,10466
inline boolean acl_ospf_spf_inter_debug 307,10631
inline boolean acl_ospf_spf_ext_debug 312,10796
inline void check_ospf_suspend 320,10982
inline void check_ospf_suspend_limit(352,11754
static boolean update_rtr_route 371,12248
rtr_route *lookup_rtr_route 497,16042
static void delete_rtr_route 560,17636
static void remove_invalid_routes 678,21036
static void free_dpath 736,22491
static void delete_one_route_list 765,23180
static void delete_route_list 828,25296
void process_add_req 860,25995
void ospf_rcv_add_req 1009,30148
static inline void process_dbf_ase_use 1032,30749
static void insert_route_list 1047,31099
static void ospf_insert_faddr 1084,32175
void ospf_destroy_faddrq 1112,32872
void ospf_destroy_maxage 1136,33314
void ospf_service_maxage 1161,33775
void ospf_insert_maxage 1207,34925
void ospf_delete_maxage 1242,35822
void ospf_service_exdelayq 1268,36419
void ospf_insert_exdelayq 1314,37489
void ospf_destroy_exdelayq 1334,37828
ndbtype *ospf_lookup_net_route 1362,38566
static idbtype *ospf_get_idb 1412,39843
void ospf_ex_faddr 1440,40388
static void create_sum 1483,41553
void generate_sum 1551,43125
static ushort delete_old_routes 1578,43651
static void ex_delete_old_routes 1795,50468
static void sum_delete_old_routes 1867,52777
static lsdbtype * find_db 1935,55040
static void addroute 1961,55508
static void add_ex_route 2104,59903
static void add_sum_route 2205,62710
void free_plist 2309,65578
static idbtype * ospf_ipaddr_to_idb 2332,66093
static idbtype *map_to_unnumbered_idb 2367,67058
static pathtype * add_to_plist 2392,67708
static void enq_clist(2435,68937
static void deq_clist(2482,70169
static void add_path 2517,71062
static void add_better_path 2689,76690
static lsdbtype * getnext_clist 2724,77635
static ipaddrtype if_addr_to_stub 2744,78076
void delete_route_old_mask 2776,79039
static inline void db_spf_init_common 2913,83480
static void spf_init 2932,83853
static boolean rtr_backlink 2977,85092
static areatype * ospf_lookup_transit_area 3062,87591
static boolean virtual_backlink 3101,88733
boolean net_backlink 3169,90524
static inline void ospf_add_stub_routes_inline 3188,90981
static inline void ospf_free_stub_routes_inline 3203,91318
static void ospf_add_all_stub_routes 3215,91539
static void spf_intra 3276,93080
static void summary_path_delete 3673,105617
static void process_sum_sub 3706,106626
void process_sum 3981,114621
static void external_path_delete 4025,115787
static void add_better_ex_path_fwd 4063,116826
static void add_better_ex_path 4103,117901
static void process_ase_sub 4129,118584
void process_ase 4508,129697
static void virtual_link_check 4591,132066
static void spf_ex_init 4642,133360
static void spf_ex 4663,133935
static void spf_sum_init 4694,134803
static void spf_sum 4726,135645
void ospf_force_spf 4774,137009
void ospf_set_all_for_immediate_spf 4791,137346
void spf 4817,137979
static boolean ospf_area_set_spf_timer 4859,138987
void ospf_set_spf_timer 4882,139604
static ulong ospf_highest_rtr_id 4896,139910
void run_spf 4927,140588
void ospf_schedule_partial_spf 5118,146640
void ospf_clean_partial_spfQ 5168,147941
static int ospf_process_partial_spfQ 5189,148374
void ospf_service_partial_spf 5228,149313
