
nhrp.c,3642
nhrpfuns *nhrp_funmap 148,4779
uchar nhrp_proto2idx 158,4938
ulong nhrp_extractmask 177,5287
static uchar nhrp_htoa 190,5473
static inline void nhrp_printnsap_subr 203,5688
void nhrp_printnsap 219,5972
uchar *nhrp_sprint_nbma 254,6611
static nhrpaddr *nhrp_find_nhs 288,7374
static boolean nhrp_valid_nbma_len 332,8337
static inline boolean nhrp_copy_nbma 370,9204
void nhrp_bugnet 390,9661
void nhrp_bugaddr 409,10036
void nhrp_printaddr 428,10392
void nhrp_printpre 447,10765
static inline void nhrp_cache_populate 466,11152
static boolean nhrp_cache_request 493,11938
static boolean nhrp_cache_reply 530,12800
static boolean nhrp_cache_reg 564,13671
uchar *nhrp_ptype_string 588,14288
static uchar *nhrp_err_string 611,14655
static void nhrp_stat 640,15284
static idbtype *nhrp_nextidb 668,15912
static uchar *nhrp_opt_string 679,16135
static void nhrp_show_record 706,16633
static void nhrp_show_options 732,17330
static void nhrp_bugpak_mand 765,18104
static void nhrp_write 807,19232
static void nhrp_send 891,21441
static inline ulong nhrp_hold 940,22803
static boolean nhrp_use_record 963,23352
static ulong nhrp_add_options 979,23711
static paktype *nhrp_getbuffer 1031,24884
static void nhrp_err 1044,25095
static boolean nhrp_our_nbma 1092,26454
ushort nhrp_nbma_type 1141,27538
static ushort nhrp_add_record 1172,28060
static idbtype *nhrp_ouraddress 1206,29084
static boolean nhrp_record_sanity 1231,29594
idbtype *nhrp_respif 1258,30214
static ushort nhrp_parse_options 1275,30614
static boolean nhrp_cache_inc 1488,35942
ushort nhrp_use 1505,36406
static boolean nhrp_revise_inc 1521,36759
static void nhrp_tap_req 1605,38865
static boolean nhrp_cache_ourself 1628,39473
static nhrpcachetype *nhrp_tap_ourself 1654,40223
static void nhrp_send_rep 1674,40699
static void nhrp_send_rep_cache 1763,43504
static void nhrp_rep_ouraddr 1778,43984
static inline nhrp_eventtype *nhrp_get_ev 1803,44643
void nhrp_req_enq 1817,44868
static void nhrp_send_req 1845,45464
static void nhrp_forward 1919,47769
static void nhrp_recv_request 1987,49672
static void nhrp_recv_reply 2089,52283
void nhrp_queue_retrans 2164,54408
static void nhrp_send_purge_subr 2180,54797
void nhrp_send_purge 2224,56038
void nhrp_send_purge_all 2236,56319
static void nhrp_recv_purge_ack 2261,56822
static void nhrp_recv_purge 2299,57830
static void nhrp_recv_reg 2324,58418
static boolean nhrp_cache_map 2348,58925
void nhrp_map_mcast 2374,59632
void nhrp_map_command 2422,60713
static boolean nhrp_pak_sanity 2457,61415
static inline void nhrp_bugpak 2500,62500
static void nhrp_recv_err 2522,63055
void nhrp_enqueue 2531,63179
static void nhrp_recv 2545,63408
static void nhrp_send_reg_subr 2606,64773
static void nhrp_send_reg 2647,66135
static void nhrp_if_up 2693,67232
static void nhrp_if_down 2716,67658
static void nhrp_announce 2738,68063
static void nhrp_init 2768,68640
boolean nhrp_resolve_nhop 2797,69608
boolean nhrp_our_purge_pak 2838,70614
void nhrp_update_nhs 2853,70881
ushort nhrp_link2idx 2928,72476
boolean nhrp_tunnel_bcast 2948,72901
static void nhrp_events 2992,73826
static void nhrp_periodic 3027,74524
static void nhrp_timer_check 3094,76166
static void nhrp_teardown 3137,77224
static process nhrp_process 3166,77945
void nhrp_process_start 3208,78872
void nhrp_process_kill 3227,79279
void nhrp_show_purge 3254,79782
static void nhrp_show_traffic_idb 3282,80434
void nhrp_show_traffic 3300,80865
void nhrp_if_defaults 3335,81662
#define NHRP_MAJVERSION 3356,82241
#define NHRP_MINVERSION 3357,82267
#define NHRP_EDITVERSION 3358,82293
