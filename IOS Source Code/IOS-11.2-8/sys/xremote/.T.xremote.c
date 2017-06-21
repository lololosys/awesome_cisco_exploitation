
xremote.c,2121
#define htoXRl(htoXRl169,6096
#define XRtohl(XRtohl170,6122
#define htoXRs(htoXRs171,6148
#define NEED_REPLIES173,6174
#define MAXCHRS 182,6351
#define MAXREPLYLENGTH 184,6371
#define XREM_MAJVERSION 223,7545
#define XREM_MINVERSION 224,7571
#define XREM_EDITVERSION 225,7597
void xremote_init 240,7915
char *client_state 256,8600
xrcontext *xremote_new_context 262,8747
void xremote_shutdown 292,9459
void logout_xremote_shutdown 337,10815
void xremote_exec_command 345,10916
void xremote_reset 508,14487
DoFileReply 522,14769
DoRequests 586,16489
CloseCachedFile 775,21318
openFile 789,21576
ReadFileData 849,23117
xremote_reply_length 890,24067
boolean DoSpoofedReply 900,24283
DoReplies 930,24992
static boolean ClientWritable 1278,36204
static boolean ClientReadable 1307,36801
boolean xremoteBLOCK 1331,37211
void DoIO 1383,38822
xremote_create_client 1406,39323
struct client *xremote_connect 1445,40251
boolean xremote_tcp_check_access 1469,40758
boolean xremote_tcp_connect 1483,41112
xremote_io_buffer 1498,41397
static struct buffer  *NewBuffer 1527,42154
struct client *NextClient 1544,42493
static void DisposeBuffer 1549,42576
DisposeClient 1582,43566
SendRequest 1600,43980
StuffDelta 1705,46846
BytesDiff 1768,48675
DialClient 1781,48850
HangupClient 1834,50457
MaybeChangeClient 1850,50871
BufferMove 1869,51410
BufferStuff 1902,52264
BufferPreview 1934,53136
BufferYank 1964,53823
int ClientGetData 1984,54330
void ClientRead 2035,55391
SerialRead 2095,56901
static void ClientWrite 2130,57770
SerialWrite 2188,59252
void CloseClient 2210,59788
HostUnswapWord 2254,60989
HostUnswapLong 2262,61145
pad 2272,61362
void xremote_command 2279,61438
static void xremote_show_server 2365,63370
static boolean xremote_show_tty 2386,64054
void show_xremote 2486,66972
void xremote_show_tcp 2506,67347
int xremote_clients 2533,67941
static sys_timestamp xremote_lastinput 2544,68134
static boolean xtrekBLOCK 2551,68288
#define XTREK_DAEMON_PORT 2565,68590
boolean xremote_xtrek_client 2567,68622
static boolean xremote_show_systat 2652,70445
