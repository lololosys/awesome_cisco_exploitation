/* $Id: pactl.h,v 1.1.68.1 1996/04/19 14:57:49 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/include/pa/pactl.h,v $
 *------------------------------------------------------------------
 * Cylink Source code: pactl.h
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pactl.h,v $
 * Revision 1.1.68.1  1996/04/19  14:57:49  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/11/28  01:49:53  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:14:05  bew
 * Placeholders for SDU files.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*****************************************************************************
*  FileName: pactl.h			Product Name: SecureLAN
*  File STATUS:
*     $Header: /SecureLAN/SDU/Include/PA/PACTL.H 11    5/03/95 11:11a Venkat 
*
*  Description:
*      Data Types for Control shared among Rx, Pa, Tx, and Other world if any
*  Notes:
*    Any change in data structures should be carefully controlled as 
*	  there is a physical size constraint on the control block.
*
*  Copyright (c) Cylink Corporation 1994. All rights reserved.
*
******************************************************************************
*
* $History: PACTL.H 
* 
* *****************  Version 11  *****************
* User: Venkat       Date: 5/03/95    Time: 11:11a
* Updated in $/SecureLAN/SDU/Include/PA
* PA (IPX config) variable.
* 
* *****************  Version 10  *****************
* User: Venkat       Date: 5/01/95    Time: 12:45p
* Updated in $/SecureLAN/SDU/Include/PA
* Added a Counter(PA Blocked).
* 
* *****************  Version 9  *****************
* User: Sduhost      Date: 3/03/95    Time: 10:12a
* Updated in $/SecureLAN/SDU/Include/PA
* Bug fix in Type declaration of a field (Source MAC Address) in IPX
* header.
* 
* *****************  Version 8  *****************
* User: Sduhost      Date: 2/21/95    Time: 7:54a
* Updated in $/SecureLAN/SDU/Include/PA
* Change to support CS 3 function; Sending CS originated frames to  CS
* task.
* 
* *****************  Version 7  *****************
* User: Croyle       Date: 1/09/95    Time: 4:10p
* Updated in $/SecureLAN/SDU/Include/pa
* changed "#include ep.h" to "#include ep_frm.h"
* 
* *****************  Version 6  *****************
* User: Croyle       Date: 1/09/95    Time: 1:08p
* Updated in $/SecureLAN/SDU/Include/pa
* Added cryptoData field to Pa2EpFrameControl data structure for storing
* cryptographic parameters prior to encryption.
* 
* *****************  Version 5  *****************
* User: Venkat       Date: 1/07/95    Time: 1:21p
* Updated in $/SecureLAN/SDU/Include/pa
* Missed UDP checksum protoype during last checkin...
* 
* *****************  Version 4  *****************
* User: Venkat       Date: 1/07/95    Time: 12:51p
* Updated in $/SecureLAN/SDU/Include/pa
* System Module - Protocol IDs are moved from mibmgr.h; Also UDP checksum
* function is madel available to others.
* 
* *****************  Version 3  *****************
* User: Venkat       Date: 10/20/94   Time: 11:27a
* Updated in $/SecureLAN/SDU/Include/pa
* Redefined Data Structures - Overlay of Control Information; esp.
* Physical (Driver) and Logical (Application) Pointers.
* 
* *****************  Version 2  *****************
* User: Venkat       Date: 10/06/94   Time: 10:01a
* Updated in $/SecureLAN/SDU/Include/pa
* Incorporated changed for CS, and moved epFrameInfo to ep.h file, as
* Jim maintains it.
* 
* *****************  Version 1  *****************
* User: Venkat       Date: 10/05/94   Time: 1:04p
* Created in $/SecureLAN/SDU/Include/pa
* PA Control Information Defintions.
* 
*****************************************************************************/

# ifndef PACTL_H
# define PACTL_H

/* Control information is created by RX module and PA module. All info
	created by PA is appended to that created by RX module. But info created
	by a module is never modified by another module, unless it is no more 
	required. All the	control info is carried in a control block which is 
	acquired by an Rx module, and destroyed by any module when it decides 
	that the control info is no more required. The Control information is 
	NOW piggybacked onto the frame buffer. Both go together. No physical 
	separation is possible!
 */
	
/* useful to CS task, but set by the driver (macType param) */
# define ETHERNET 		1
# define IEEE8023 		0

/* useful for several tasks/modules, set by the driver (receivedPath param) */
# define PE_SIDE			1
# define UPE_SIDE			0

/* CRC computation control - reqd. for a frame xmission (actually this belongs */
/* to the Driver! */
# define CRC_DISABLE	0		/* default */
# define CRC_ENABLE		1		/* all locally generated frames need this value */

/* Values assigned to frameMode parameter */
/* WARNING: Never change the following values w/o looking into connstup.c */
/*          jump table.(CS task) */
enum { FrameBypass=0, FrameProcess=1, NncFrame=2, ConnectInit=3, FrameReject=4,
	   Frame2CStask=5 } ;

/* A common control header that is created/used by Rx, Pa, Tx, and any module 
	that interfaces with them */
typedef struct frameControlHeader { /* size = 26 bytes (or 28?) */
				void 	*descLogP ;	 /* Logical pointer used by the s/w */
				void 	*descPhysP ; /* physical pointer used by the Xmitter */
				void 	*frameP ;	 /* Logical pointer to a Frame this control belongs */
				void 	*txPortP ;	 /* set by the Rx Driver to indicate which  */
										 /* Xmitter should be handed over, if necessary */
				ushort frameSize ;	 /* Total MAC Frame Size (incl. CRC) */
				ushort frameControl ; /* To Xmitter whether CRC be computed or not */
				ushort macFrameType ; /* Ethernet / 802.2 Frame indicator */
				ushort receivedPort ; /* which side this frame arrived */
				ushort frameClass ;	 /* Holds modes specified above in 'enum' */
#ifdef CISCO
				void *otherP ; /* Pointer to platform-specific info */
#endif
		 } FrameControlHeader ;
		 
/* Rx --> PA control information */
typedef FrameControlHeader Rx2PaFrameControl ;
		
/* PA --> Mg, Cy --> Tx control exchange */
typedef FrameControlHeader	Pa2MgFrameControl, Cy2TxFrameControl ;

/* Never change the following values w/o looking into paisr.c fun table. */
/* assigned to epEntry.pid */
# define  INTERNET_IP	  		1
# define  XEROX_IDP 			2
# define  NOVELL_IPX			3
# define  APPLE_DDP 			4  

/* Use some unique values for the echo code point so it is less likely */
/* that someone out there is using the same code point */
# define IP_NNC_CODE	      74  /* NNC codepoint */
# define IPX_NNC_CODE		  96
# define IDP_NNC_CODE		 148
# define DDP_NNC_CODE		 226

#include "../crypto/sdu/include/ep_frm.h"	   /* PA --> EP info exchange */

typedef struct {
			FrameControlHeader fch ;
			# define descriptorLink		fch.descLogP
			# define descPhysLink 		fch.descPhysP 
			# define txPortId			fch.txPortP
			# define frameLink			fch.frameP
			# define frameLength		fch.frameSize
			# define frameMode  		fch.frameClass
			# define macType 			fch.macFrameType
			# define receivedPath		fch.receivedPort
			epFrameInfo epEntry ;
			uchar  *networkLink ;
			uchar  *transportLink ;
			uchar	cryptoData[20];
		} Pa2EpFrameControl ;

/*----------I suggest not to touch the data structs below this line---------- */

/* Note: The byte order is not important for L2 protocols, thus this structure
			would work; BUT NO HOLES SHOULD BE PRESENT. Directly operates on the
			Frame */
typedef struct {
		  uchar 	destMac[6] ;  	/* don't ever change destMac, sourceMac types */
		  uchar 	sourceMac[6] ;
		  union {
				struct {
					ushort type ;
					uchar  packet ;	/* packet in ether frame */
				} ether ;
				struct {
					ushort frameLen ;
					union {
						ushort sapType ;
						uchar novellPacket ; /* some novell frames have no SAP! */
					} control ;
				} sap ;
		  } frame ;
		  # define etherType		frame.ether.type
		  # define etherPacket		frame.ether.packet 
		  # define sapType			frame.sap.control.sapType
		  # define novellPacket     frame.sap.control.novellPacket 
		  union {
				struct {
						uchar control ;	
						uchar packet ; 	/* sap packet */
					} sap ;
				struct {
					ushort ctlOrg1 ; 	/* ctl, org1 */
					ushort org2Org3 ;	/* org2, org3 */
					ushort code ;		/* which snap protocol ? */
					uchar  packet ;	/* packet start  -- end of snap */
				} snap ;
		  } sapUnion ;
		  # define 	sapPacket 			sapUnion.sap.packet 
		  # define 	snapCode 			sapUnion.snap.code
		  # define 	snapPacket 			sapUnion.snap.packet 
	  } L2EtherFrameHeader ;

/* IP Packet:
	This structure is NOT as per RFC byte ordering. This is specific to Intel;
	Still all multi-byte values require to reorder them to get values for any
	arithmetic or logical operations. MAKE SURE THERE ARE NO HOLES CREATED IN 
	THIS STRUCTURE; DIRECTLY WORKS ON THE PACKET;
 */
# define IP_NOT_1stFRAG		~0x00E0 /* is this not a 1st fragment? */
# define IP_NOT4_VERSION 	 0xB0	  /* check for version 4 */
# define IP_HLEN   			 0x0F  /* header length nibble */

typedef struct {
		  uchar  versIhl ; 	/* versIhl(1) */
		  uchar  tos ; 		/* tos(1) */
		  ushort length ;		/* byte flip  */
		  ushort ident ;		/* identification when fragmented */
		  ushort fragment  ; /* byte flip (fragment control + offset) */
		  uchar  ttl ;
		  uchar  protocol ;
		  ushort checksum ;
		  ulong  sourceAdr ;
		  ulong  destAdr ;
	  } IpPacketHeader ;

/* IPX/IDP packet header:
		PA only peeks at packeType, destNetwork, destSocket, 
		sourceNetwork, sourceSocket 
 */
typedef struct {
		  ushort checksum ;
		  ushort length ;
		  uchar  tcontrol ;
		  uchar  packetType ;
		  ulong  destNetwork ;
		  ulong  destHost1 ;
		  ushort destHost2 ;
		  ushort destSocket ;
		  ulong  sourceNetwork ;
		  ulong  sourceHost1 ;
		  ushort sourceHost2 ;
		  ushort sourceSocket ;
	  } IpxPacketHeader, IdpPacketHeader  ;
	  
/* AppleTalk Packet Header:
			PA peeks only at packetType, network and socket #s.
 */
typedef struct {
		  ushort hopAndLength ;
		  ushort checksum ;
		  ushort destNetwork ;
		  ushort sourceNetwork ;
		  uchar  destNodeId ;
		  uchar  sourceNodeId ;
		  uchar  destSocket ;
		  uchar  sourceSocket ;
		  uchar  packetType ;
	} DdpPacketHeader ;

/* shared with system modules - defined here to preserve their standard
	RFC defined values, except arp */	
# define SYS_ARP			  0
# define SYS_ICMP	 		  1
# define SYS_BOOTP 		 	 68
# define SYS_TFTP			 69
# define SYS_SNMP 			161

/* function prototypes for intermodule communication */
extern ushort udpChecksum( IpPacketHeader *netPtr, ushort *udpControl);
extern void paReceiveFrame( Rx2PaFrameControl *rxp) ;
# define epFrameHandler		 ep_frame  /* declared in ep.h  */
extern unsigned long paBlockedFramesCount ;
extern unsigned int paIpxHashIndex ;

/* Defines for Protocol ID's (PID_X_X) defined as per the bit position */
/* specified in working paper #45 Table, protocol vector bit definitions. */

#define PID_IP_TCP		31
#define PID_IP_UDP		30
#define PID_IP				29

#define PID_IPX_SPX		28
#define PID_IPX_PEP		27
#define PID_IPX_NCP		26
#define PID_IPX			25

#define PID_IDP_SPP		24
#define PID_IDP_PEP		23
#define PID_IDP			22

#define PID_DDP_ATP		21
#define PID_DDP_ADSP		20
#define PID_DDP			19

#define PID_DRP_NSP		18
#define PID_DRP			17

#define PID_VIP_VIPC		16
#define PID_VIP_VSPP		15
#define PID_VIP			14

#define PID_NTBS			13

# endif
					
				
