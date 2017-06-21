
ppp.c,4197
struct protent protent1013,41262
#define MAX_LCPOPTIONS 1076,43566
#define PPP_MAJVERSION 1082,43633
#define PPP_MINVERSION 1083,43658
#define PPP_EDITVERSION 1084,43683
static char * ppp_get_endpoint_string 1096,43974
uchar * ppp_get_remote_name 1117,44368
static int prottbl_lookup 1128,44625
static boolean ppp_linestate 1149,45059
static boolean ppp_extract_addr 1175,45664
static void ppp_bridge_enqueue 1195,46212
iqueue_t ppp_parse_packet 1380,51644
static boolean ppp_autodetect_start_interface 1499,54865
static boolean ppp_autodetect_test 1532,55542
boolean ppp_setup 1563,56257
fsm *ppp_link2fsm 1745,62555
fsm *ppp_serial2fsm 1755,62751
int ppp_serial2link 1765,62935
static long ppp_getlink 1774,63075
static void ppp_if_state_change 1862,65791
static void ppp_if_prot_change 1875,66206
void ppp_enqueue 1889,66483
static process ppp_input 1928,67646
void ppp_send_packet 2072,71696
boolean ppp_vencapsulate 2176,74356
static void ppp_deencap 2452,82199
static void ppp_cstate 2487,83021
#define ECHOLEN 2548,84462
static void ppp_timer 2550,84521
static void ppp_echo 2639,86528
ppp_get_authname(2730,88940
static boolean ppp_put_authname 2779,89747
static void ppp_init 2839,91341
static void demuxprotrej 2880,92703
void fsm_init 2894,92928
static void fsm_open_delayed 2910,93506
void fsm_activeopen 2948,94628
void fsm_passiveopen 2994,95842
void fsm_close 3022,96684
static void fsm_timeout 3066,98155
void fsm_lowerup 3151,100584
void fsm_lowerdown 3165,100920
static void is_negotiation_done 3180,101366
void fsm_input 3247,103111
static void fsm_rconfack 3519,110748
static void fsm_rconfnak 3592,112594
static void fsm_rconfrej 3641,113883
static void fsm_rtermreq 3680,114834
static void fsm_rtermack 3730,116265
static void fsm_rcoderej 3753,116754
static void fsm_rprotrej 3773,117243
void fsm_sconfreq 3796,117778
static void fsm_rechorep 3825,118483
void fsm_sprotrej 3897,120969
static void ppp_cleanup_options 3914,121569
static void lcp_init 3941,121984
static void lcp_activeopen 4119,127163
static void lcp_passiveopen 4134,127374
void lcp_close 4158,127952
void lcp_lowerup 4168,128049
static void lcp_lowerdown 4219,129216
static void lcp_input 4234,129415
static void lcp_protrej 4249,129783
static void lcp_resetci 4263,130059
static int lcp_cilen 4440,134924
static void lcp_addci 4465,135551
static int lcp_ackci 4502,136650
static boolean lcp_ackci_forced 4571,139076
static void lcp_nakci 5095,152434
static void lcp_rejci 5307,159019
static uchar lcp_reqci 5413,162334
static void lcp_up 6154,184561
void lcp_authcomplete 6345,190832
static void lcp_down 6482,194827
static void lcp_closed 6512,195623
static void copy_ppp_fields 6613,199091
void ppp_command 6808,204838
void ppp_ncp_command 7511,224601
static void copy_ncp_fields 7584,226338
static void ppp_interface 7605,226784
static void ppp_periodic 7642,227527
#define SLQRLEN	7710,229390
#define MAX_LOST_LQRS	7711,229458
#define MIN_LOST_LQRS	7712,229518
static void lcp_slqr 7717,229629
static void lcp_poorquality 7823,233139
static void lcp_goodquality 7869,234524
static void lcp_loopedback 7892,235094
static void lcp_notloopedback 7911,235522
static void lqr_protrej 7928,235909
static void lqr_init 7950,236402
static void lcp_rlqr 7960,236554
static void lcp_print 8289,246809
#define DEBUGLEN 8340,248244
void ppp_dataprint 8343,248297
static boolean ppp_callback_accept 8385,249234
u16 pppfcs 8483,252946
static void fsm_clear_author 8495,253158
typedef struct author_queuetype_ author_queuetype_8501,253272
} author_queuetype;author_queuetype8504,253357
static boolean fsm_author_in_progress 8506,253378
static boolean fsm_do_author 8519,253676
static boolean fsm_check_author 8687,257788
static void ppp_continue_vpn 8742,259011
static boolean ppp_lcpstate 8750,259177
static boolean ppp_set_lcpstate 8761,259404
static uchar protocol_to_authtype 8777,259760
static char *authtype_name 8802,260168
static void get_another_auth_method 8821,260631
boolean auth_method_supported 8870,261924
static void ppp_lapb_event 8889,262351
static void ppp_send_lapb_dm 8917,263116
