
dual.c,4253
static inline boolean dual_serno_earlier 349,15189
static inline boolean dual_serno_later 361,15435
static inline dual_rdbtype *dual_thread_to_drdb 377,15821
static inline dual_ndbtype *dual_thread_to_dndb 400,16381
static inline boolean dual_xmit_threaded 417,16758
static inline boolean dual_iidb_on_quiescent_list 427,16965
void dual_remove_iidb_from_quiescent_list 437,17194
static void dual_add_iidb_to_quiescent_list 454,17665
static void dual_xmit_unthread_unanchored 474,18270
static ulong dual_next_serno 533,20142
static igrp2_xmit_anchor *dual_anchor_entry 555,20678
void dual_unanchor_entry 582,21328
static void dual_xmit_unthread 621,22288
inline static void dual_freedata 672,23621
static void dual_rdbdelete 685,23918
static void dual_rtdelete 711,24734
static void dual_zapdrdb 727,25159
static void dual_dndbdelete 765,26113
static void dual_cleanup_dndb 829,27740
static void dual_move_xmit 847,28243
static void dual_cleanup_drdb 876,29176
static void dual_process_acked_reply 917,30130
static void dual_drop_dndb_refcount 994,32189
static void dual_process_acked_multicast 1051,33911
static void dual_process_acked_startup 1115,35700
static igrp2_xmit_thread *dual_packetize_thread 1222,38860
static void dual_packetize_interface 1417,43776
static boolean dual_process_acked_packet 1543,47643
void dual_xmit_continue 1599,49628
static void dual_xmit_drdb 1651,51192
static void dual_xmit_dndb 1704,52887
void dual_finish_update_send_guts 1789,55926
void dual_new_peer 1821,56883
static boolean dual_splithorizon 1952,60792
static advertise_request_type dual_should_advertise 1988,61628
typedef enum thread_kind_ thread_kind_2007,62084
    thread_kind;thread_kind2008,62155
igrp2type *dual_buildpacket 2009,62172
void dual_last_peer_deleted 2364,71603
void dual_first_peer_added 2424,73220
static inline dual_rdbtype *dual_successor 2435,73565
static void dual_state 2448,73860
static void dual_clearhandle 2460,74237
static void dual_promote 2491,75155
static void dual_rtupdate 2548,76828
static void dual_rdbclear 2633,79362
static boolean dual_active 2658,80092
dual_ndbtype *dual_ndblookup 2876,87301
static dual_rdbtype *dual_rdblookup 2895,87704
static void dual_sendreply 2919,88166
static inline void dual_setFD 2943,89019
static inline void dual_maxRD 2952,89181
static inline ulong dual_setRD 2965,89583
static boolean dual_compare_drdb 2984,90106
#define DUAL_MAXCOPYCHAR 3034,92071
static ulong dual_updatetopology 3029,91860
static void dual_scanupdate 3245,98303
static void dual_routeupdate 3398,103262
static void dual_setdistance 3586,109227
static boolean dual_testFC 3645,111115
static void dual_FCsatisfied 3731,113358
static boolean dual_FC 3783,114573
static void dual_transition 3879,117039
static void dual_cleanup_reply_status 3948,118622
static void dual_validate_ndb_route 3979,119525
void dual_sendupdate 4008,120397
void dual_validate_route 4073,122826
void dual_reload_proto_table 4089,123211
static void dual_rcvupdate 4115,123758
static void dual_rcvquery 4205,126172
static void dual_rcvreply 4269,127971
void dual_rcvpkt 4330,129877
void dual_peerdown 4352,130340
void dual_ifdelete 4502,134430
void dual_conn_rtchange 4562,136009
void dual_rtchange 4621,138173
void dual_lostroute 4679,139586
dual_ndbtype *dual_table_walk 4725,141011
void dual_cleanup 4760,141718
static void dual_log_copy 4801,142720
void dual_sia_copy_log 4823,143239
static void dual_unstick_dndb 4846,143856
void dual_scan_active 4942,147318
dual_rdbtype *dual_routelookup 5007,149040
static void dual_format_event_record 5142,157424
void dual_log_event 5178,158478
void dual_allocate_free_lists 5235,159838
boolean dual_init_events 5249,160204
void dual_clear_event_logging 5289,161048
void dual_clear_log 5299,161225
void dual_adjust_scratchpad 5313,161649
static void dual_init_timers 5336,162133
boolean dual_init_ddb 5365,163051
char *dual_query_origin_str 5407,164642
static char *dual_routetypestring 5420,164866
static char *dual_route_event_string 5441,165317
dual_show_event_param 5462,165760
static void dual_free_show_blocks 5523,167339
void dual_show_events 5545,167975
void dual_configure_logging 5680,171382
