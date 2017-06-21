
telnet.c,1684
#define MAXCHRS 329,11577
#define MAXBYTES 330,11596
#define CMDNOTSENTSTATE 412,13351
#define CMDSENTSTATE 413,13377
#define TERMINALTYPESENTSTATE 415,13484
#define HEWILLBINARYSTATE 417,13583
#define WEWILLBINARYSTATE 419,13677
#define TN3270STATE 425,13913
void telnet_init(433,14079
conntype *open_connection 455,14733
void close_connection 758,22650
int outguess 799,23884
boolean telnetBLOCK 841,25316
void tcp_parse_resume 905,26972
static boolean send_byte_block 948,28078
int telnet 992,29263
boolean tnof 1204,35672
void telnet_putc 1450,43431
int telnet_puts 1460,43716
int telnet_input 1482,44369
int telnet_inputfsm 1558,46708
int tnif 1619,48394
int tn_cmdstate 1645,49034
void tn_optionstate 1740,51735
void tn_suboption 1882,56609
void tn_sendsub 2105,63058
void tn_sendsubcnt 2127,63746
void telnet_suboption_pad_iac(2174,65045
void option_emit 2210,65755
void telnet_defaults 2233,66362
char *option_string 2313,69092
static boolean magic_putc 2346,70018
boolean magic_fingers 2379,70875
int pak_rembyte 2494,73989
int telnet_getline 2510,74412
boolean send_option 2592,76532
void receive_option 2625,77743
boolean telnetuser_optioncheck 2733,81258
void telnetuser_optiondone 2848,84494
boolean telnetserver_optioncheck 2913,86012
int linemode_getline 2977,87451
    if 3089,89887
#define PORTCHRS 3104,90196
#define MAXCHRS 3105,90216
void tcp_setsflow 3295,94571
void tcp_clearoutput 3308,94870
boolean telnet_alive 3329,95327
int telnet_unread 3366,96560
conntype *telnet_multiproto_open 3386,97028
void telnet_conn_acct 3534,100820
addrtype *telnet_source_address 3561,101743
boolean tty_status_ok 3588,102619
