
pactl.h,2646
# define PACTL_H113,4139
# define ETHERNET 126,4755
# define IEEE8023 127,4777
# define PE_SIDE	130,4879
# define UPE_SIDE	131,4900
# define CRC_DISABLE	135,5026
# define CRC_ENABLE	136,5064
enum { FrameBypass=141,5298
enum { FrameBypass=0, FrameProcess=141,5298
enum { FrameBypass=0, FrameProcess=1, NncFrame=141,5298
enum { FrameBypass=0, FrameProcess=1, NncFrame=2, ConnectInit=141,5298
enum { FrameBypass=0, FrameProcess=1, NncFrame=2, ConnectInit=3, FrameReject=141,5298
	   Frame2CStask=142,5378
typedef struct frameControlHeader frameControlHeader146,5511
		 } FrameControlHeader FrameControlHeader160,6321
typedef FrameControlHeader Rx2PaFrameControl Rx2PaFrameControl163,6387
typedef FrameControlHeader	Pa2MgFrameControl, Cy2TxFrameControl Cy2TxFrameControl166,6481
# define  INTERNET_IP	170,6654
# define  XEROX_IDP 171,6682
# define  NOVELL_IPX	172,6707
# define  APPLE_DDP 173,6732
# define IP_NNC_CODE	177,6892
# define IPX_NNC_CODE	178,6943
# define IDP_NNC_CODE	179,6971
# define DDP_NNC_CODE	180,6999
			# define descriptorLink	186,7149
			# define descPhysLink 187,7190
			# define txPortId	188,7232
			# define frameLink	189,7267
			# define frameLength	190,7302
			# define frameMode 191,7341
			# define macType 192,7381
			# define receivedPath	193,7421
		} Pa2EpFrameControl Pa2EpFrameControl198,7566
		  # define etherType	221,8241
		  # define etherPacket	222,8282
		  # define sapType	223,8328
		  # define novellPacket 224,8377
		  # define 	sapPacket 237,8765
		  # define 	snapCode 238,8813
		  # define 	snapPacket 239,8858
	  } L2EtherFrameHeader L2EtherFrameHeader240,8908
# define IP_NOT_1stFRAG	248,9227
# define IP_NOT4_VERSION 249,9294
# define IP_HLEN 250,9354
	  } IpPacketHeader IpPacketHeader263,9766
	  } IpxPacketHeader, IdpPacketHeader IdpPacketHeader282,10209
	} DdpPacketHeader DdpPacketHeader297,10582
# define SYS_ARP	301,10712
# define SYS_ICMP	302,10735
# define SYS_BOOTP 303,10760
# define SYS_TFTP	304,10787
# define SYS_SNMP 305,10811
# define epFrameHandler	310,11019
#define PID_IP_TCP	317,11313
#define PID_IP_UDP	318,11336
#define PID_IP	319,11359
#define PID_IPX_SPX	321,11381
#define PID_IPX_PEP	322,11405
#define PID_IPX_NCP	323,11429
#define PID_IPX	324,11453
#define PID_IDP_SPP	326,11475
#define PID_IDP_PEP	327,11499
#define PID_IDP	328,11523
#define PID_DDP_ATP	330,11545
#define PID_DDP_ADSP	331,11569
#define PID_DDP	332,11594
#define PID_DRP_NSP	334,11616
#define PID_DRP	335,11640
#define PID_VIP_VIPC	337,11662
#define PID_VIP_VSPP	338,11687
#define PID_VIP	339,11712
#define PID_NTBS	341,11734
