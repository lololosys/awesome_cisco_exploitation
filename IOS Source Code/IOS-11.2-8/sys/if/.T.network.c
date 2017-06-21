
network.c,6682
#define	MAX_RP_LTYPES	978,38540
#define BACKUP_IN_LOAD 1036,40812
#define BACKUP_OUT_LOAD 1037,40842
#define BACKUP_IN_DELAY 1038,40872
#define BACKUP_OUT_DELAY 1039,40902
#define BACKUP_LOAD_MASK 1040,40932
short linktype_is_fq_conversation 1049,41105
char *link2name 1074,41813
void holdq_init 1093,42210
void holdq_resize 1109,42486
void set_default_queueing 1127,42889
static void reset_fair_queue(1210,45428
static void reset_idb_holdq 1238,46055
void holdq_reset 1293,47475
static inline boolean holdq_enqueue_fail_inline 1319,48031
boolean holdq_enqueue_fail 1398,50018
boolean holdq_enqueue_fast_reserve 1414,50373
boolean holdq_enqueue 1436,51043
paktype *holdq_dequeue 1471,51753
#define BERT_DEGREE 1552,54800
#define BERT_DEGREES 1553,54823
#define BERT_BIT 1554,54862
static inline uint bert_random 1556,54920
boolean red_enqueue 1568,55365
paktype *red_dequeue 1676,58250
void red_set_precedence_levels 1724,59472
void holdq_display 1765,60532
void traffic_shape_out 1872,64382
void datagram_out 1949,66725
fuzzy physical_broadcast 2051,70021
boolean supports_maxdgram 2066,70315
void interface_command 2089,70798
static void network_fixup 2283,76111
void encapsulation_command 2329,77476
void transmitter_command 2477,81132
void holdqueue_command 2506,81659
void tx_queue_limit_command 2563,83094
void keepalive_command 2598,84083
void loopback_command 2686,86649
void set_max_interface_mtu(3057,96777
int if_has_multiple_mtu 3112,98240
boolean if_change_mtu 3134,98759
void max_mtu_command 3200,100190
static void set_interface_bandwidth 3308,102932
void set_default_interface_bandwidth 3341,103897
void bandwidth_command 3367,104507
void logging_interface_command 3395,105109
static void set_interface_delay 3419,105609
void set_default_interface_delay 3434,105937
void delay_command 3449,106288
void description_command 3478,106784
void transmitif_command 3514,107547
void pulsetime_command 3548,108261
void media_type_command 3577,109052
static void default_media_half_or_full_duplex_params_command 3585,109222
void media_half_or_full_duplex_params_command 3596,109515
static void backup_set_interface 3604,109742
static void backup_clear_interface 3616,110018
static boolean backup_check_interface 3633,110387
static void bring_up_subif_backup 3679,111677
static void bring_down_subif_backup 3695,112150
static inline boolean backup_is_shut 3710,112537
static void subif_backup_adjust(3716,112672
static void backup_set_default_delay 3734,113092
void backup_command 3754,113568
boolean delete_interface 4036,121723
void shutdown_command 4095,123359
void shutdown_interface 4116,123819
void shutdown_going_down 4216,126573
void shutdown_coming_up 4257,127580
void shutdown_subif 4317,129090
void subif_line_statechange 4378,130844
void reset_if 4420,131813
static void periodic_activity 4454,132835
process net_periodic 4556,136402
process net_onesecond 4573,136655
static process net_oneminute 4586,136839
static inline void net_restart_throttle 4610,137318
void idb_mark_throttled 4627,137781
void idb_mark_unthrottled 4634,137972
boolean idb_is_throttled 4639,138076
static process net_input 4661,138763
process net_background 4717,140086
void raw_enqueue 4773,141689
void netinput_enqueue 4805,142331
void protocol_discard 4817,142549
void process_enqueue_pak 4837,143079
void process_requeue_pak 4853,143549
static void loopback_enqueue 4866,143754
void net_bridge_enqueue 4880,144097
void net_bridge_copy 4891,144357
void net_bridge_enqueue_broadcast 4902,144587
void net_bridge_copy_broadcast 4913,144826
static void net_bridge_enqueue_discard 4925,145087
char *parse_interface 4948,145512
void nullidb_init 5009,147053
void show_swidb_chain 5058,148805
void show_hwidb_chain 5093,149542
char *print_idbflags 5122,150097
void clear_counters 5140,150542
void clear_idb 5182,151910
static void show_isdn_idb 5325,156591
static void show_idb_crb 5373,157915
static void show_idb_irb 5437,159548
static void show_idb_accounting 5516,161770
show_idb_switching 5573,163841
show_idb_stats 5642,166210
void show_interface 5697,167929
void show_idb 5734,168885
char *print_hwidbstate 6239,186671
char *print_idbstate 6279,187799
const char *encstring 6319,189000
uint encaptype 6330,189233
char *print_linktype 6346,189485
#define MAX_DATARATE_MULT_INDEX 6454,194079
#define SAMPLESECS 6473,194573
static void compute_idb_load 6475,194595
static process compute_loads 6643,200461
void load_interval_command 6675,201061
void net_cstate 6728,202538
static void net_sw_cstate 6784,204372
static void cstate_check 6831,205750
void carrier_delay_command 6883,207203
boolean interface_up 6930,208373
boolean hw_interface_up 6952,208791
static void backup_timers 6994,210085
static void compute_load_from_dialer_member 7111,213542
static void compute_backup_loads 7124,213908
static void backup_loads 7171,215393
void backup_adjust 7207,216448
void transition_adjust 7246,217509
void route_adjust 7267,218032
static void bridge_adjust 7283,218355
void setup_all_bridging 7299,218757
void coerce_hwaddr 7327,219642
void macaddr_command 7358,220534
void find_802addr 7414,222065
static uchar *net_get_first_ieee_hardware_address 7444,222969
static void net_default_addr_init 7488,224125
ulong create_unique_number 7539,225595
void new_physical_mtu 7559,226322
boolean can_select_subif_type 7585,226819
void set_subif_link_type 7615,227552
boolean main_if_or_vc_or_vlan 7649,228436
boolean main_if_or_vc 7672,228942
boolean main_if_or_iftype 7691,229403
boolean routed_protocols_on_idb 7706,229706
boolean invalidate_snpa 7721,229962
static void invalidate_proto_address 7733,230190
static void sprintf_unknown_address 7748,230488
static void sprintf_unknown_src_dst 7755,230601
boolean ieee_extract_hwaddr 7766,230765
boolean invalidate_hwaddr 7779,231089
int llc_mtu 7795,231492
boolean usable_interfaces 7811,231924
static ulong return_link_illegal 7827,232170
static void default_encaps_nv_write 7835,232309
default_encaps_display 7848,232559
static void default_command 7866,233021
static paktype *default_compress_header(7874,233214
void network_init 7892,233581
void create_queue_registry 7978,237374
void show_queue_command 8005,238137
snapshot_fair_queue 8037,238743
void show_fair_queue_command 8056,239124
static void show_priority_queue_command 8128,240962
snapshot_fair_queue_counters 8222,243177
void fair_clear_counters 8227,243339
void red_clear_counters 8246,243932
max_proto_mtu 8263,244337
ulong def_proto_mtu 8279,244771
if_multi_add 8299,245249
void if_maxdgram_adjust 8310,245510
