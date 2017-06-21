
lapb.c,1975
#define LAPB_U_OVERHEAD	267,10097
#define LAPB_U_PF_INX	268,10123
#define LAPB_U_PF_SHIFT	269,10147
struct lapb_mode_ lapb_mode_276,10291
#define SET_MODE(SET_MODE335,11885
static inline boolean lapb_can_tx_i 344,12224
static inline boolean lapb_rx_congestion_enter 364,12969
static inline boolean lapb_rx_drop_iframe 368,13107
static inline boolean lapb_rx_congestion_exit 380,13577
static void lapb_encaps_nvram_write 405,14227
void lapb_init 426,14689
boolean lapb_vencapsulate 485,16616
boolean lapb_output 563,18458
void lapb_u_frame 651,20477
void lapb_s_frame 747,22513
static inline void lapb_form_i_frame_inline 819,24016
paktype *lapb_pull_frame 841,24640
char *lapb_prstate 1060,30067
#define LAPB_COMMAND 1112,31083
#define LAPB_RESPONSE 1113,31108
#define LAPB_EITHER 1114,31134
#define LAPB_POLL 1115,31158
#define LAPB_FINAL 1116,31180
void lapb_prframe 1126,31443
boolean lapb_validcmdframe 1315,36612
void lapb_start 1349,37282
void lapb_input 1434,39172
void lapb_frmr 2260,63940
boolean lapb_ack 2341,66412
boolean lapbBLOCK(2397,67757
forktype lapb_timer 2492,69811
void lapb_reset 2638,73841
long lapb_getlink 2710,75471
lapb_bcast 2734,75974
void lapb_encaps 2741,76114
int lapb_default_n1 2949,81180
int lapb_admin_n1 2979,81767
int lapb_min_n1 3007,82355
void lapb_change_mtu(3029,82913
void lapb_update_n1 3041,83226
void copy_lapb_fields 3068,83938
static boolean lapb_linestate 3131,85916
static boolean lapb_extract_addr 3145,86196
static iqueue_t lapb_parse_packet 3179,86974
void lapb_deencapsulate 3244,88497
boolean lapb_setup 3303,89941
boolean lapb_x25_oqueue 3561,97045
paktype *lapb_x25_oqueue_dequeue 3640,99021
void lapb_check_output_vectors 3660,99529
void lapb_cstate 3746,101848
void lapb_interface 3807,103715
void lapb_clearcounters 3869,106107
#define LAPB_PPP_DEFAULT_N2	3891,107027
lapb_payload 3897,107169
void lapb_ppp_setup 3913,107434
void lapb_ppp_close 4084,111982
