
isdnintrfc.c,4804
#	define	assert(assert400,15090
#	define	assert(assert402,15206
#define FROM_ROUTERSW	415,15528
#define FROM_DRIVER	418,15673
#define FROM_MODEM	419,15708
#define FOR_MODEM_MSGS	420,15741
#define FOR_ROUTER_MSGS 421,15775
#define FROM_PHYSICAL_LAYER 423,15813
#define FIRST_DSL 424,15850
#define SUBADDR_SEPARATOR 425,15899
#define ZERO_OFFSET 426,15929
#define AFI_BYTE 427,15991
#define SUBADDR_SEP_LEN 429,16128
#define NON_SPC_CALL 430,16154
#define FAILED_CALL 431,16182
#define ACCEPTED_CALL 432,16204
#define REFUSED_CALL 433,16228
#define DISC_OTHER_CAUSE 434,16252
#define NORMAL_DISC_SENT 435,16290
#define NORMAL_DISC_RECEIVED 436,16328
#define DISC_NETWORK_OUT_OF_ORDER 437,16367
#define DISC_CALL_REJECTED 438,16406
#define DISC_USER_BUSY 439,16445
#define NO_CHANNEL_AVAILABLE 440,16484
#define INTERWORKING_ERROR 441,16522
enum frametypes frametypes468,17501
	Q921_INFO 475,17686
	Q921_RR,476,17702
	Q921_RNR,477,17712
	Q921_REJ,478,17723
	Q921_SABME,479,17734
	Q921_DM,480,17747
	Q921_UI,481,17757
	Q921_DISC,482,17767
	Q921_UA,483,17779
	Q921_FRMR,484,17789
	Q921_XID,485,17801
	Q931_ALERTING,488,17838
	Q931_CALL_PROC,489,17887
	Q931_CONNECT,490,17904
	Q931_CONNECT_ACK,491,17919
	Q931_PROGRESS,492,17938
	Q931_SETUP,493,17954
	Q931_SETUP_ACK,494,17967
	Q931_RESUME,495,17984
	Q931_RESUME_ACK,496,17998
	Q931_RESUME_REJ,497,18016
	Q931_SUSPEND,498,18034
	Q931_SUSPEND_ACK,499,18049
	Q931_SUSPEND_REJ,500,18068
	Q931_USER_INFO,501,18087
	Q931_DISCONNECT,502,18104
	Q931_RELEASE,503,18122
	Q931_RELEASE_COMP,504,18137
	Q931_RESTART,505,18157
	Q931_RESTART_ACK,506,18172
	Q931_SEGMENT,507,18191
	Q931_CONG_CONT,508,18206
	Q931_INFORMATION,509,18223
	Q931_FACILITY,510,18242
	Q931_NOTIFY,511,18258
	Q931_STATUS,512,18272
	Q931_STATUS_ENQ,513,18286
	Q931_USETUP,514,18304
	NUM_STAT_ENTRIES	516,18319
enum direction direction518,18360
enum direction { TX 518,18360
enum direction { TX = 0, RX,518,18360
enum direction { TX = 0, RX, NUM_DIR_ENTRIES 518,18360
struct br_msg br_msg522,18512
inline char *DSL_TO_NAME 602,21762
static inline hwidbtype *BCHAN_TO_IDB 610,21989
static inline hwidbtype *BCHAN_TO_IDB_WITH_CHECK 615,22120
static inline ulong BCHAN_TO_BITFIELD 624,22319
inline void isdn_increment_bchan 634,22515
inline void isdn_decrement_bchan 661,23246
isdn_buginf_flush(681,23856
void isdn_init_dsl_list(702,24268
int isdn_verify_dsl 717,24539
void isdn_assign_dsl 737,24929
void isdn_deassign_dsl 752,25288
void isdn_init_core 759,25409
void show_isdn_L2timers 860,28295
void show_isdn_pri 880,28980
void show_isdn_status 889,29160
isdn_switchtype 1003,32294
isdn_getswitchtype 1023,32732
isdn_p 1038,33063
isdn_p2 1051,33506
isdn_p4 1070,34087
isdn_lsd 1080,34205
void isdn_iqueue 1089,34335
static void isdn_remove_v120 1110,34875
void UserIdle 1131,35388
static void process_modem_command 1554,47890
Pri_Activate_L2 1771,55523
Host_Start 1789,55977
L2D_Srq_Task 1800,56149
isdn_fromrouter 1962,61147
isdn_frommodem 2005,62160
process_bri_call 2043,63110
process_pri_call 2354,73726
process_rejectcall 2652,83702
process_disconnect 2704,84863
process_ignore 2804,87764
process_simple 2837,88599
process_pri_simple 2913,90748
process_modem_callproc 3004,93266
process_modem_conn 3095,95919
process_disc_ack 3135,96919
isdn_phys_ind 3176,97959
process_rxdata 3310,102643
static call_id_string_t *isdn_find_cid_table(3428,106005
static char *display_remote_number 3442,106334
process_rxstate 3457,106652
#define	VN2_FACILITY_NATIONALE_IE	3835,119357
isdn_report 4737,150566
parse_hlc 4927,156754
static void parse_llc 4983,158125
static uchar YORN 5096,160918
static void display_llc 5102,161000
parse_bearercap 5128,161835
parse_network_spec_facil 5206,163563
parse_progress 5221,163862
parse_service 5240,164191
extract_bchan 5295,165484
extract_calling_num 5388,167885
my_Get_IE 5435,169154
my_Get_IE_pkt 5466,169924
PUBLIC void itoa 5495,170645
static void inttoasc 5506,170874
isdn_exit 5537,171357
do_statistics 5551,171602
show_statistics 5624,174347
isdn_display 5652,175309
#define is_usetup(is_usetup5675,175891
decode_info 6117,187901
decode_ie 6295,192662
badframe 6658,203293
badpacket 6669,203611
show_data 6681,203943
#define isprintc(isprintc6708,204690
show_cause 6750,205869
show_progress 7005,213233
show_signal 7043,214189
static char * extract_called_number 7101,215551
static char * extract_spc_number 7123,216234
static char * extract_sa 7141,216708
static boolean verify_answer_number 7164,217329
compare_num 7199,218368
match_num 7258,220484
void isdn_mib_update 7292,221238
void isdn_mib_extract_cause 7308,221789
void isdn_mib_cause_init 7343,222724
hwidbtype *isdn_mib_find_idb 7392,224226
