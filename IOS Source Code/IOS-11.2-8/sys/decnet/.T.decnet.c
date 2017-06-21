
decnet.c,2931
#define FIXLSWAP(FIXLSWAP485,19303
#define DEC_MAJVERSION 492,19373
#define DEC_MINVERSION 493,19398
#define DEC_EDITVERSION 494,19423
short decnet_fair_queue_flow_id 542,21351
static void decnet_print_network_header 576,22090
static uchar *dn_return_bcast_str 613,22997
static char *dn_connect_str 627,23399
static boolean decnet_enable_test 639,23602
static boolean decnet_routing_idb 650,23784
static boolean decnet_bridging_idb 676,24496
static void decnet_rxtypes_update 716,25819
static void decnet_showproto 756,26812
static void dncache_update 775,27324
dncache_invalidate 842,29121
void dn_init_idb 859,29569
static void dn_register_thyself 903,30931
void dn_init 944,32774
dnnet *dn_initnet 1001,34180
static void dn_cleanup 1044,35140
static process dn_input 1074,35788
static uchar *pick_dn_802addr 1159,37896
void dn_setaddr 1223,39577
void dn_flushroutes 1276,40972
void dn_maproutes 1359,42910
void dn_addmaproute 1383,43284
void dn_free_rt 1502,46866
void dn_clear_rt 1524,47364
void dn_dispose 1540,47662
void dn_packet_forward 1624,51198
void dn_forward 1644,51674
boolean dn_return 2133,66124
boolean decnet_accesscheck 2166,67131
boolean dn_source_dest_match 2546,77268
static boolean dn_access 2567,77662
boolean decnet_data_accesscheck 2607,78387
void dn_debug_cpacket 2646,79330
char *cnttostr 2722,81129
boolean decnet_fastcheck 2738,81311
void dn_control 2765,81767
void dn_hello_input 2831,83589
int dn_add_adjacency 2976,88017
void dn_change_mtu_size 3625,108322
dnmetrictype *dn_make_mvec 3660,109222
void dn_remove_via 3678,109539
void dn_rt_input 3727,110948
void dn_unsplit_all 4296,127572
void dn_unsplit 4323,128166
void dn_recomp 4390,130128
void dn_route 4618,137421
boolean dn_routeBLOCK 4879,145900
void dn_sendhellos 4898,146320
void dn_sendhello_if 4992,149527
void dn_sendroutes 5107,152613
void dn_sendroutes_if 5256,157616
static void decnet_enqueue 5565,166660
void show_decnet 5579,166914
void show_dnglobal 5685,169129
void display_dn 5768,171853
boolean show_dnroutes 5835,174438
boolean dn_show_route 5929,176791
void dn_show_neighbors 6112,181772
void dn_traffic 6156,182761
void dn_var_init 6201,184669
void decnet_route(6237,185819
void decnet_command 6251,186065
void decnetif_command 6634,194939
void dn_disable_if 6865,201491
void dn_enable_if 6892,202089
void nv_decnetif 6940,203756
void nv_decnet 7004,205944
boolean parse_decnetaddr 7150,209386
void dnaccess_command 7202,210531
void dnobjfree 7435,216043
void nv_dnaccess 7450,216327
void dn_showaccess 7460,216514
void if_printf 7550,219137
void show_dnaccess 7566,219389
show_dnaccess_shell 7578,219632
void decnetmap_command 7592,219949
void dn_purgemap 7752,223806
void show_dnmap 7816,225213
boolean decnet_on_idb 7851,225891
void decnet_clear_counters 7865,226111
boolean dn_idb_is_bilingual 7881,226402
int dn_printf 7899,226794
