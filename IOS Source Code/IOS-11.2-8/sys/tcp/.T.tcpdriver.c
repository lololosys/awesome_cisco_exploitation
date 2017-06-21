
tcpdriver.c,1769
#define TCPDRIVER_MAXSEG	97,3560
#define TCPDRIVER_STACK_SIZE	98,3608
static void tcpdriver_addtail 111,4053
static void tcpdriver_del 126,4534
static boolean tcpdriverBLOCK 146,5021
static void tcpdriver_activate 175,5846
static void tcpdriver_activate_internal 202,6738
static void tcpdriver_setevent(225,7454
static void tcpdriver_clearevent(234,7686
static void tcpdriver_killme_callback 247,7982
static void tcpdriver_input 261,8322
static tcp_encaps *tcpdriver_active_open 280,8782
static tcp_encaps *tcpdriver_passive_open 359,10808
static void tcpdriver_change_timeout 421,12515
static void tcpdriver_close_internal 439,12888
static void tcpdriver_close 461,13445
static void tcpdriver_abort 479,13774
static void tcpdriver_doclose 502,14278
static void tcpdriver_update 560,15635
static boolean tcpdriver_unacked_output 581,16007
static void tcpdriver_do_flow_ctrl 588,16154
static void tcpdriver_output 621,17095
static ulong tcpdriver_oqlen 687,18915
static void tcpdriver_set_oqmax 698,19134
static inline ulong tcpdriver_get_oqmax 709,19326
static void tcpdriver_set_flow_ctrl_levels 719,19570
static int tcpdriver_send 739,20233
static int tcpdriver_send_now 791,21535
static inline tcbtype *tcpdriver_tcb 814,22020
static inline ipaddrtype tcpdriver_fhost 824,22178
static inline ipaddrtype tcpdriver_lhost 834,22367
static inline ushort tcpdriver_fport 844,22545
static inline ushort tcpdriver_lport 854,22711
static void tcpdriver_set_flag 864,22861
static void tcpdriver_clear_flag 872,22975
static void tcpdriver_set_tos 884,23142
static uchar tcpdriver_get_tos 893,23275
tcpdriver_keepalive 908,23490
static forktype tcp_driver 944,24262
static void tcpdriver_start 1051,26968
void tcpdriver_init 1064,27213
