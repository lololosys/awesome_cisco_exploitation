
xremote.h,3298
#define __XREMOTE_H__46,1648
#define BITSBIGENDIAN56,1827
  } ClientState;ClientState65,2084
#define BLOCKED 70,2175
#define NOTREADY 71,2193
#define OUT_READY 72,2212
#define DIAL_ERROR 73,2232
#define SWITCH_CLIENT 74,2253
#define DEBUG_DELTAS 76,2278
#define DEBUG_REQUESTS 77,2313
#define DEBUG_REPLIES 78,2352
#define DEBUG_FLOW 79,2388
#define DEBUG_FILES 80,2420
#define DEBUG_PACKETS 81,2454
typedef BYTE XRemoteClientId;XRemoteClientId83,2492
struct shortcommand{shortcommand85,2523
typedef struct _FileReadRequest _FileReadRequest94,2715
} FileReadRequest;FileReadRequest105,3193
#define MAXFileReadRequest 106,3212
typedef struct _FileReadDone _FileReadDone113,3383
} FileReadDone;FileReadDone119,3591
typedef struct _FileReadData _FileReadData126,3756
} FileReadData;FileReadData133,4116
#define MAXDELTASIZE 135,4133
#define NDELTAS 136,4157
#define MAXBYTESDIFF 137,4176
typedef struct _DeltaRec _DeltaRec142,4284
} DeltaRec;DeltaRec145,4364
struct delta{delta151,4472
struct deltacommand{deltacommand160,4629
#define REQ_FILE_DATA	165,4717
#define REQ_DIAL	166,4745
#define REQ_HANGUP	167,4768
#define REQ_CHANGE_CLIENT	168,4793
#define REQ_DELTA	169,4824
#define REPLY_READ_FILE_DONE	171,4849
#define REPLY_READ_FILE	172,4883
#define REPLY_CHANGE_CLIENT	173,4913
#define REPLY_CLOSE_CLIENT	174,4946
#define REPLY_DELTA	175,4978
#define REPLY_HANGUP_REPLY	176,5004
#define XREMOTE_BUFFERSIZE 178,5037
#define MAXBYTESPERCLIENT 179,5069
#define PACKET_SIZE 181,5113
#define MAX_LZW_PLAIN 182,5137
#define cb 183,5184
#define XR_TFTP_BUFFERSIZE	185,5225
#define XR_TFTP_RETRIES	186,5258
typedef struct xrfonthost_ xrfonthost_188,5286
} xrfonthost;xrfonthost195,5627
#define MAXSOCKS 197,5642
enum LineProtocol LineProtocol199,5664
enum LineProtocol {XR_XWIRE,199,5664
enum LineProtocol {XR_XWIRE, XR_FILE_ACCESS}199,5664
struct xrcontext_ xrcontext_201,5711
struct halfbuf{halfbuf270,7786
struct buffer{buffer278,8044
#define BufferSlack(BufferSlack283,8143
enum transportType transportType285,8201
enum transportType { TRANSPORT_TCP,285,8201
enum transportType { TRANSPORT_TCP, TRANSPORT_LAT,285,8201
enum transportType { TRANSPORT_TCP, TRANSPORT_LAT, TRANSPORT_LOCAL 285,8201
struct client client287,8273
typedef unsigned int Sequence;Sequence304,9043
#define SequenceWidth 305,9074
#define SequenceDEFN(SequenceDEFN306,9098
#define SequenceGet(SequenceGet307,9155
#define XMT_WINDOW 308,9213
#define RCV_WINDOW 309,9255
typedef struct _packetHeader _packetHeader311,9298
} PacketHeader;PacketHeader331,9950
#define HEADER_SIZE 332,9966
} ConnectionHeader;ConnectionHeader341,10223
} ExtendedHeader;ExtendedHeader358,10680
typedef unsigned int Counter;Counter360,10699
#define bs_link 362,10730
#define bs_buf 363,10763
#define bs_sendTime 364,10788
#define bs_wasRetransmitted 365,10817
typedef paktype Packet;Packet366,10860
typedef struct _queue _queue368,10885
} PacketQueue;PacketQueue371,10947
typedef enum {BS_CLOSED, SYNCSENT, SYNCRCVD, BS_CONNECTED} ConnectionState;ConnectionState373,10963
typedef struct _connection _connection375,11040
} SerialConnection;SerialConnection407,12149
#define X_PROTOCOL 476,14760
#define X_PROTOCOL_REVISION 477,14782
