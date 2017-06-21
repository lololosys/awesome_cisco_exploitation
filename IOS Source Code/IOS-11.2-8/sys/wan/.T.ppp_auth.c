
ppp_auth.c,1642
enum aaa_op aaa_op486,18296
enum aaa_op {DO_CHAP_RCHALLENGE,486,18296
	     DO_CHAP_RRESPONSE,487,18329
	     DO_PAP_RREQUEST,488,18354
	     DO_PAP_SENDREQUEST}489,18377
typedef struct aaa_req_ aaa_req_490,18404
} aaa_req;aaa_req499,18759
static boolean chap_hold_response 522,19573
uchar *chap_new_peer_name 537,20157
void auth_init 561,20860
void auth_cleanup 598,21770
void auth_start 622,22378
static void chap_sfailure 667,23478
chap_hash 697,24295
ppp_hostname 716,24758
static ulong chap_xor_challenge 785,26491
static void chap_sign_challenge 797,26722
static boolean chap_check_challenge 833,27657
static void chap_rchallenge 897,29677
static void resend_chap_response 1241,39824
void flush_chap_resp_pkt 1267,40473
static void chap_rfailure 1280,40724
void chap_ssuccess 1309,41362
static void chap_rsuccess 1342,42303
static inline void ppp_bad_auth_attempt 1375,43108
static void chap_rresponse 1396,43702
ppp_auth_background 1778,54985
find_aaa_request(1869,57521
queue_aaa_request 1920,58814
void chap_input 2166,67542
static void pap_snak 2282,71045
static char *pap_display_password_error 2329,72402
static void pap_sendok 2345,72883
static void pap_rrequest 2377,73857
static void pap_rack 2693,83688
static void pap_rnak 2747,84925
void pap_input 2778,85550
void auth_protrej 2880,88162
static void chap_sendchallenge 2897,88606
static void pap_sendrequest 2981,91283
void auth_timeout 3095,94746
#define MAX_PAPNAMES 3168,96749
#define MAX_CHAPNAMES 3177,96911
void auth_print 3179,96936
boolean lcp_do_author 3207,97648
ppp_vpn 3359,101813
ppp_authen_prepare 3394,102616
