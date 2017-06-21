
ipxcompression.c,2021
#define DEBUG_IPX_COMPRESSION_ANY 251,10949
#define DEBUG_IPX_COMPRESSION_ACTIVITY 254,11047
#define DEBUG_IPX_COMPRESSION_EVENT 256,11113
static boolean ipx_compression_shipit 268,11556
static paktype *ipx_compression_gimme_a_packet 280,11801
static paktype  *ipx_compression_send_reject 309,12617
static void ipx_compression_send_confirm 346,13934
static paktype *ipx_compression_cut_n_paste 394,15724
static inline void ipx_compression_clear_slot 484,18534
static paktype *ipx_compression_process_unconfirmed_initial 497,18890
static paktype *ipx_compression_process_confirmed_initial 553,20814
static paktype *ipx_compression_process_reject 612,22929
static paktype *ipx_compression_process_confirm 643,24055
static paktype *ipx_compression_process_regular 682,25446
static paktype *ipx_compression_process_compressed 703,26117
paktype *ipx_compression_decompress_header 787,29012
static void ipx_compression_increment_slot 846,30937
static ipx_ncp_compression_slot *ipx_compression_lookup_outbound_slot 871,31946
static paktype *ipx_compression_compress_header 997,36928
void ipx_compression_reset 1164,42605
boolean ipx_compression_exists 1224,44581
void ipx_compression_options_provide 1233,44759
void ipx_compression_options_receive 1259,45569
void ipx_compression_if_command 1280,46241
static void ipx_compression_show_idb 1373,49246
static void ipx_compression_show_idb_header 1419,51102
static void ipx_compression_print_slot_details 1425,51286
static void ipx_compression_show_idb_always_summary 1454,52729
static void ipx_copmression_show_idb_cond_summary 1460,52878
static void ipx_compression_show_idb_always_detail 1470,53130
static void ipx_copmression_show_idb_cond_detail 1509,54190
void ipx_compression_show_command 1523,54519
static void ipx_compression_show_interface 1541,55046
static void ipx_compression_init 1549,55181
#define IPX_COMPRESSION_MAJVERSION 1571,56100
#define IPX_COMPRESSION_MINVERSION 1572,56137
#define IPX_COMPRESSION_EDITVERSION 1573,56174
