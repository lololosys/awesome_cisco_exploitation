
interface.c,2728
    IDBTYPE_HW=396,15754
    IDBTYPE_SW397,15772
typedef struct dynamic_idblist_hdr_ dynamic_idblist_hdr_414,16239
} dyn_idblist_hdr_t;dyn_idblist_hdr_t418,16390
typedef struct dynamic_idblist_elt_ dynamic_idblist_elt_437,17147
} dyn_idblist_elt_t;dyn_idblist_elt_t442,17373
idb_list_info 582,20358
idb_dyn_sb_info 610,21072
static int get_next_swidb_index 636,21829
idb_get_next_hw_index 648,22035
idb_get_current_hw_index 654,22106
idb_can_create_more 668,22365
static void swidb_init_common 678,22520
void idb_init_all 710,23405
idb_create 723,23677
idb_link_hw_sw 756,24499
idb_init_common 771,24848
idb_create_subif 833,26901
idb_unlist 956,30738
idb_free 978,31168
boolean idb_default_ordering 1008,31948
boolean idb_slotted_ordering 1059,33264
idb_enqueue 1101,34337
void idb_unlink 1137,35197
#define SLOT_PORT_DELIMITER 1146,35359
#define	PORT_VC_DELIMITER 1147,35391
int idb_name_format 1153,35517
static void idb_slotted_names 1172,35851
void idb_init_names 1298,39595
idb_first_init 1344,40781
idb_final_hw_init 1421,43588
idb_final_sw_init 1433,43949
idb_final_init 1449,44369
idb_print_summary 1486,45042
void idb_clear_dangerous 1537,47292
void media_idb_enqueue 1561,47937
idb_fill_sb 1582,48340
idb_sb_order 1598,48732
idb_add_swsb 1616,49088
idb_add_hwsb 1653,50151
idb_delete_swsb 1691,51233
idb_delete_hwsb 1723,51991
idb_get_swsb 1753,52702
idb_get_hwsb 1778,53299
idb_use_swsb 1804,53937
idb_use_hwsb 1833,54636
idb_release_swsb 1862,55329
idb_release_hwsb 1899,56145
get_list_header 1964,57959
get_list_header_link 1990,58523
idb_create_list 2020,59226
void idb_report_create_error 2066,60535
idb_destroy_list 2081,60899
idb_get_listhead 2136,62173
swidb_list_order 2153,62564
idb_add_swidb_to_list 2172,63029
hwidb_list_order 2212,64146
idb_add_hwidb_to_list 2233,64492
void idb_report_add_error 2265,65348
idb_remove_from_list 2281,65805
void idb_report_removal_error 2308,66380
idb_for_all_on_swlist 2320,66657
idb_for_all_on_hwlist 2346,67275
char *rxtype_string 2373,67810
idb_hw_state_config 2434,68704
idb_sw_state_config 2444,68919
ulong idb_inputs 2461,69352
void idb_increment_rx_proto_counts 2491,70189
ulong idb_fast_in 2505,70481
ulong idb_auto_in 2515,70658
ulong idb_rx_cumbytes 2525,70835
ulong idb_rx_fast_cumbytes 2537,71102
ulong idb_rx_auto_cumbytes 2547,71293
ulong idb_outputs 2557,71484
ulong idb_fast_out 2582,72136
ulong idb_auto_out 2592,72315
ulong idb_tx_cumbytes 2602,72494
ulong idb_tx_fast_cumbytes 2614,72761
ulong idb_tx_auto_cumbytes 2624,72952
idb_subif_allowed_here 2643,73376
idb_get_counter_info 2673,74273
char *print_snpa_string 2706,74924
boolean is_stun_sdlc_encap 2759,76166
