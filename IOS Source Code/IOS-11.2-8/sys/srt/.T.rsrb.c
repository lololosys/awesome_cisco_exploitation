
rsrb.c,5165
tr_ventrytype *malloc_vre 552,20733
static inline void rsrb_tcpdriver_cleanup 573,21278
static void rsrb_tcpdriver_abort 593,21723
static void rsrb_tcpdriver_close 610,21999
void vring_init 624,22234
static void vr_purge_remote_rings(647,23002
void rsrb_newstate 688,24150
paktype *vrpak 773,26841
tr_vgrouptype *vrfind_ringgroup 799,27442
tr_ventrytype *vrfind_rn2entry 818,28023
static boolean vrvalid_peer(919,30540
tr_ventrytype *vrfind_idb2entry 934,30859
tr_vpeertype *vrfind_peer 960,31382
char *vr_op2str 1030,32888
char *vrpeer_id 1083,35780
char *vrpeer_state 1114,36496
int rif_match_offset(1139,37339
void vrforce_xchg_delayed 1215,39770
paktype *ring_xchg_pak 1227,40014
void vrqueue_null_xchg 1304,42331
boolean rsrb_checkqueue 1322,42729
void vrprocess_ring_xchg 1363,43803
void update_peers 1465,46805
void vrsend_version 1514,48066
boolean vrsend_caps 1547,48957
boolean vrsend_xchg 1583,50007
boolean vr_version_reasonable 1614,50921
boolean rsrb_tcp_correct_port 1653,52515
boolean vrcheck_conn_state 1669,52971
boolean vrconnectorBLOCK 1737,54554
void vrconnect1 1757,55075
forktype vrconnector 1891,58840
void rsrb_vidb_iqueue 1929,59966
boolean rsrb_vidb_oqueue 2020,62872
hwidbtype *vr_new_vidb 2064,64286
tr_ventrytype * vr_virtualp 2151,66915
boolean virtualp_default(2182,67690
uchar *vr_first_virtual 2195,67935
uchar *vr_next_virtual 2217,68338
void vr_vre_input 2261,69333
void vr_nukepak 2312,70911
void vrif_err 2328,71263
void vrif_send_punt 2347,71784
static ushort find_dlci 2370,72455
void vrif_input 2383,72712
boolean vrif_sender 2805,84951
boolean vrif_direct 2896,87543
void vrif_abort 2960,89354
void vrif_close 2971,89571
vpeer_connrtn vrif_connect 2981,89758
void vrfst_err 3056,92658
void vrfst_send_punt 3073,93131
void vrfst_input 3097,93848
paktype *rsrb_fst_full_duplicate 3597,108235
boolean vrfst_sender 3652,110073
boolean vrfst_direct 3746,112773
void vrfst_abort 3884,116941
void vrfst_close 3895,117160
vpeer_connrtn vrfst_connect 3905,117353
int rsrb_packet_classify 3989,120431
tr_vpeertype *rsrb_packet_classify_peer 4062,122850
boolean vrtcpd_sender 4108,124182
boolean vrtcpd_direct 4341,132124
void vrtcpd_abort 4384,133123
void vrtcpd_close 4397,133463
vpeer_connrtn vrtcpd_connect 4422,134161
ipaddrtype vrtcp_peername 4580,139518
void vrtcpd_send_close 4600,139970
void vrtcpd_err 4641,141159
boolean vrtcpd_gethdr 4670,142057
void vrtcpd_drain 4736,144464
boolean vrtcpd_op_expected 4772,145421
void vrtcpd_phdr 4800,146382
void vrtcpd_ahdr 4955,151234
void vrtcpd_rdhdr 5062,154331
void vrtcpd_rddata 5216,158900
void vrtcpd_fini 5629,171613
void vrtcpd_listen 5662,172716
boolean add_vring_interface 5706,173990
void set_vring_interfaces 5781,176039
void remove_vring_interface 5876,178938
boolean vre_fastout_err(5935,180316
void rsrb_shutdownallfst(5949,180594
void vrnail_peer 5959,180717
void add_all_spanidb(6061,183625
void rsrb_add_virtual_span(6082,184145
tr_vgrouptype *create_vring 6129,185384
void destroy_vring 6176,186726
static tr_vpeertype *rsrb_newvrp 6262,188656
void create_vrpeer 6513,195250
void destroy_vrpeer 6629,199355
void vrforward_send_all_peers 6676,200505
void vrforward_explorer 6762,202982
boolean fs_explorer_check 7179,217208
void vrforward_pak 7278,219918
void vring_background 7823,239067
void rsrb_periodic_per_peer 8113,249452
void rsrb_periodic 8120,249590
void set_rsrb_tcpqlen 8137,249997
void set_rsrb_lack_connect_timeout 8163,250663
void rsrb_clear_counters 8171,250811
void rsrb_print_peer_info 8247,253312
void show_rsrb 8307,254907
boolean rsrb_active 8446,258828
void rsrb_newbia 8456,259013
void set_rsrb_largest 8490,259913
boolean is_80d5_enabled(8627,264682
void xform_80d5_to_802(8667,265531
void xform_802_to_80d5(8689,266152
boolean fs_srtlb_explorers 8718,267073
boolean fs_srtlb_packets 8784,269295
boolean rsrb_vbridge_oqueue 8976,274922
boolean rsrb_create_virtual_bridge 9172,279763
idbtype *rsrb_find_virtual_bridge 9305,283567
void rsrb_delete_virtual_bridge 9317,283854
tr_vgrouptype *rsrb_exists_translational 9356,284645
void rsrb_netbios_delete_list 9375,284980
boolean rsrb_opcode_valid 9407,285857
boolean rsrb_out_check 9458,287239
boolean rsrb_netbios_acfcheck 9523,289300
idbtype * create_vidb 9611,291879
void rsrb_add_spanidb(9677,293652
void rsrb_delete_virtual_span 9718,294852
void vrforward_bpdu(9742,295288
boolean rsrb_permit_vidb_span 9842,298208
void rsrb_enable_virtual_span(9876,299094
void rsrb_disable_virtual_span 9898,299552
boolean rsrb_vidb_span_forward 9923,300178
short rsrb_fair_queue_flow_id 9944,300713
tr_vpeertype *rsrb_find_vrp(9983,301718
boolean rsrb_is_pak_from_lak_peer(10035,302881
#define NUMRECS	10048,303071
#define M_FRMR	10050,303092
#define LPDUBYTES 10052,303111
struct llc_trace_rec llc_trace_rec10054,303132
void init_llctrace 10073,303590
static struct llc_trace_rec *alloc_llc_trace_rec 10086,303822
static void dump_llc_trace_rec 10113,304397
void show_llctrace(10163,305880
void make_llctrace 10206,306951
void frame_make_llctrace 10245,307933
