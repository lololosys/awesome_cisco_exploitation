
tcpsupport.c,2513
void tcp_closeconnection 274,9605
void tcp_dump_retrans 294,10151
#define NUM_MTUS 335,11191
static void tcp_pmtu_newmss 343,11384
static void tcp_start_pmtutimer 363,11883
static void tcp_pmtu_estimate 379,12368
void tcp_splinter 458,14851
void tcp_pmtu_response 643,20000
void tcp_pmtu_on 707,21843
void tcp_pmtu_off 715,21971
static void tcp_unreachable 731,22437
boolean tcp_seqinrange 786,23873
tcbtype *tcp_allocatetcb 800,24147
void tcp_deallocatetcb 855,25906
void tcp_flushqueue 916,27787
void tcp_changestate 990,29953
void tcp_pmtutimer_on 1045,31971
void tcp_pmtutimer_off 1070,32576
void tcp_starttimer 1098,33179
void tcp_stoptimer 1148,34471
boolean tcp_timerrunning 1177,35235
sys_timestamp tcp_nexttimeout 1196,35600
sys_timestamp tcp_timeout 1224,36170
static process tcp_timer 1366,40939
#define NTCBFLAGNAMES 1432,43655
void tcp_status 1440,43925
char *tcp_netstat_fmt 1504,46200
#define ADDRLENGTH	1511,46352
#define PORTLENGTH	1512,46374
void tcp_status_brief 1539,47006
#define LA_COL	1545,47127
#define FA_COL 1546,47187
#define ST_COL 1547,47250
char *tcp_statestring 1579,48216
void tcp_show_traffic 1602,48766
tcbtype *find_tcb 1616,49124
void tcb_enqueue 1694,51644
void tcb_unqueue 1713,52023
tcbtype *tcb_findport 1738,52532
void print_tcp 1766,53078
char *tcperror2string 1831,54806
clear_tcp_by_tcb 1878,56071
clear_tcp_by_port_addr 1901,56483
clear_tcp_by_line 1924,57044
boolean tcp_filter_debug 1954,57723
static void tcp_router_reload 1980,58317
void tcp_md5on 2001,58775
void tcp_md5digest 2013,59080
tcp_md5_connectionless_write 2078,61041
uchar *tcp_findoption 2131,62381
int tcp_optionbytes 2173,63202
void tcp_startoptions 2193,63574
void tcp_padoption 2206,63819
void tcp_writeoption 2229,64470
void tcp_endoptions 2250,64957
void tcp_changercvwnd 2261,65230
void tcp_openwindow 2296,66286
void tcp_addwindow 2315,66908
ulong tcp_selectiss 2326,67099
void tcp_retrans_sanity_check 2350,67900
void tcp_wakeup_process 2414,70052
static void tcp_pak_toobig 2441,70929
#define NUMRECS	2495,72377
struct tcp_trace_rec tcp_trace_rec2497,72398
void init_tcptrace 2521,73126
static struct tcp_trace_rec *alloc_tcp_trace_rec 2534,73358
static void dump_tcp_trace_rec 2561,73933
void show_tcptrace(2593,75010
void make_tcptrace 2636,76081
static void tcp_init 2671,76925
tcptype *tcp_get_hdr 2756,79853
#define TCP_MAJVERSION 2796,80923
#define TCP_MINVERSION 2797,80948
#define TCP_EDITVERSION 2798,80973
