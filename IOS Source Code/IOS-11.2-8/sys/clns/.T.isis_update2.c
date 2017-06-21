
isis_update2.c,1999
typedef enum age_relation_ age_relation_237,9294
				NEW_NEWER, OLD_OLDER = NEW_NEWER} age_relation;age_relation238,9366
typedef enum spf_change_ spf_change_239,9418
typedef enum spf_change_ {SPF_NO_CHANGE, SPF_PARTIAL, SPF_FULL} spf_change;spf_change239,9418
static inline boolean isis_safe_option_code 248,9655
static spf_change isis_check_spf 277,10650
static void isis_schedule_psnp 444,15596
static age_relation isis_compare_age 470,16170
static isis_lspdbtype *isis_try_fastpsnp 523,17767
static void isis_populate_fastpsnp_entry 596,19557
static boolean isis_regenerate_lsp 630,20633
static boolean isis_lsp_no_change 707,22482
static void isis_lsp_avl_insert 738,23256
void isis_link_lsp 1111,33939
void isis_delink_lsp 1130,34362
static boolean isis_validate_nlsp_lsp 1157,34902
void isis_rcv_lsp 1176,35197
static inline boolean isis_snp_data_left 1398,41966
static void isis_snp_advance_item 1408,42172
static void isis_snp_next_item 1421,42515
static isis_snp_entry *isis_snp_next_entry 1449,43073
static isis_lspdbtype *isis_build_null_lsp 1488,43983
static void isis_install_dummy_lsps(1517,44788
static void isis_add_dummy_lsp 1551,45726
static void isis_no_lsp_for_snp 1577,46337
static void isis_compare_lsp_snp 1600,47055
static boolean isis_rw_process_csnp 1663,48921
void isis_rcv_csnp 1787,52433
void isis_rcv_psnp 1910,55927
boolean isis_valid_adjacency 2032,59259
void isis_schedule_lsp_build 2088,60898
void isis_mark_L1_changed_delayed 2110,61375
void isis_mark_L2_changed_delayed 2123,61727
void isis_mark_L1_changed 2136,62112
void isis_mark_L2_changed 2152,62524
void isis_mark_L1_dr_changed 2167,62884
void isis_mark_L2_dr_changed 2180,63231
void isis_trigger_partial_route_calculation 2193,63560
void isis_trigger_spf 2235,64862
static void isis_display_spf_log 2292,66271
void show_isis_spf_log 2375,68197
static char *isis_display_timer 2413,68912
void show_isis_mesh_groups 2548,71363
void show_isis_timers 2572,71734
