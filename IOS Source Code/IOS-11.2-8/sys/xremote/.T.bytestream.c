
bytestream.c,2013
#define MS_FRAME_DELAY 111,3800
#define MS_ACK_DELAY 112,3876
#define RETRANS_MIN 113,3927
#define RETRANS_MAX 114,3953
#define RETRANS_INITIAL 115,3983
#define RANGESET(RANGESET116,4019
#define PROBE_INITIAL 117,4091
#define PROBE_MAX 118,4120
#define TRAILER_SIZE 119,4148
#define MAX_PPP_PACKET_SIZE 120,4221
#define ALLOWCOMPRESSION122,4303
#define BS_EC_SYNC 127,4407
#define BS_EC_SYNC_ACK 128,4428
#define BS_EC_RESET 129,4453
#define BS_EC_CONFIG_REQ 130,4475
#define BS_EC_CONFIG_REP 131,4537
#define BS_VERSION 136,4646
#define EqualConnection(EqualConnection137,4667
} ConfigurationHeader;ConfigurationHeader154,5264
#define CRQ_ADD_OPAQUE_CHAR 160,5385
#define CRP_ACK	164,5544
#define CRP_NAK	165,5579
} ExtendedCommand;ExtendedCommand174,5838
AddToQueue(206,6830
bs_getbuffer 224,7155
ClearPacket 243,7488
RemoveHeadOfQueue 254,7723
RemovePacketFromQueue 273,8152
BSStartup 306,8858
#define CH_CR 325,9196
#define CH_LF 326,9215
#define CH_PLUS 327,9234
#define MAP(MAP329,9313
BSShutdown 373,10779
BSFree 391,11177
#define CHECKQUEUES(CHECKQUEUES416,11729
#define CHECKQUEUES(CHECKQUEUES418,11770
QueueLength 420,11830
CheckQueues 432,11981
BSInit 449,12426
BSPollRead 479,13110
BSPollWrite 485,13231
DequeueInputPacket 496,13517
SendPacket 530,14516
SendBareAck 565,15536
CancelInitialRetransmit 590,16130
FillSyncPacket 615,16951
SendReset 653,18170
SendSync 702,19397
printConn 754,21040
StartOptionTransmission 769,21467
DoConfigReq 796,22364
DoConfigRep 862,24166
HandleExtendedCommand 911,25624
DeliverDelayedAcks 1059,30080
PostAck 1069,30328
PostProbe 1080,30553
QueueOutputPacket 1087,30691
BSRead 1111,31267
BSWrite 1141,31899
DoWindowProbe 1213,33603
DoRetransmit 1238,34394
CheckRetransmissionQueue 1271,35035
FigureRoundTrip 1306,36219
HandleAssembledPacket 1335,37198
GetNewPackets 1517,43128
bs_raw_enqueue 1533,43443
boolean BSBlock 1557,43981
void BSWakeup 1601,44949
void XremoteStatsDisplay 1610,45100
