/* $Id: ibipc.h,v 1.1.4.1 1996/05/09 15:01:47 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/sysif/ibipc.h,v $
 *------------------------------------------------------------------
 * ibipc.h
 *
 * March 1995, jjchen
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ibipc.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:47  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:43  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:43:12  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:53  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:36  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:51  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.11  1995/10/27  01:23:43  vinod
 * Fix for CSCdi41033 & CSCdi42307, cleaned up IPC download to the ATM
 * card that was clobbering spantree and other timers.
 *
 * Revision 1.10  1995/10/21  04:09:33  vinod
 * Recycled unused #define IBIPC_DL_WRITING_FLASH to #define IBIPC_DL_CODE_FAILED_CHKSUM
 *
 * Revision 1.9  1995/10/04  01:07:43  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.8  1995/09/15  23:49:10  vinod
 * Fixed IPC for multiple card support, moved some debug prints to
 * trace prints.
 *
 * Revision 1.7  1995/08/30  18:00:01  vinod
 * First phase of Multiple cards support for IPC, mainly from the
 * NMPs perspective, more changes soon. Rx upcalls (on the NMP side)
 * pass the ModuleNumber.
 *
 * Revision 1.6  1995/07/06  23:13:28  vinod
 * Corrected MIN & MAX SLOT IDs.
 *
 * Revision 1.5  1995/06/27  02:49:56  rahuja
 * Added void * to formal argument declarations.
 *
 * Revision 1.4  1995/06/13  23:27:13  rlowe
 * SYNALC builds use "buginf" instead of "printf".
 *
 * Revision 1.3  1995/06/09  23:45:27  vinod
 * Added ALC and NMP LANE IDs for IPC.
 *
 * Revision 1.2  1995/05/22  19:32:43  jjchen
 * Modified download retry count and timeout.
 *
 * Revision 1.1  1995/05/05  18:48:42  jjchen
 * Moved from syn_core/nmp/nmpflash/h.
 *
 * Revision 1.2  1995/04/07  02:52:57  jjchen
 * Added macro for in-band download control block.
 *
 * Revision 1.1  1995/03/29  06:12:38  jjchen
 * Created.
 *
*
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 * In-band IPC function return code
 */
enum ibipc_rc { IBIPC_RC_OK = 0,
	        IBIPC_RC_DUPLICATED_ID = 1,
	        IBIPC_RC_ILLEGAL_SID,
	        IBIPC_RC_ILLEGAL_DID,
	        IBIPC_RC_ILLEGAL_SLOT,
	        IBIPC_RC_ILLEGAL_HANDLE,
	        IBIPC_RC_REGISTERED,
	        IBIPC_RC_NOTXDPENDING,
	        IBIPC_RC_TOOMANYTX,
	        IBIPC_RC_NOMEMORY,
		IBIPC_RC_NODRVRBUF,
		IBIPC_RC_TOS_ERROR,
	        IBIPC_RC_DUMMY_FUNCTION
	      };
/*
 * In-band IPC ID uses 6 bits.
 */
#define MAX_IB_IPC_ID 0x3f
#define MIN_IB_IPC_ID 1
/*
 * Slot ID for Synergy box
 */
#define MIN_SLOT_ID   2
#define MAX_SLOT_ID   5

typedef struct ibipc_stats {
	uInt32 rxpkt;
	uInt32 txpkt;
	uInt32 rxbadpkt;
	uInt32 txnobuffer;
} IBIPC_STATS_T;

typedef struct ibipc_txd {
	struct ibipc_txd * next;
	uChar		 * pkt;
	uInt16		 pkt_len;
	uChar 		 tos;
	uInt16		 did;
	uChar		 slot;
	uInt16 		 timer; /* 1/1000 second */
	uChar		 retry;
	int		 timer_index;	
} IBIPC_TXD_T;

typedef struct ibipc_table {
	struct ibipc_table * next;
	uInt32 handle;
	uInt16 sid;
	uInt16 did;
	uChar  slot;
	uChar  SourceSlot;
	uChar  qflag;
	void (* App_IpcTxAck)(void *);
	void (* App_IpcTxNak)(void *);
#ifdef CATALYST_OS
	void (* App_IpcRxInd)(uInt32, void *);
#else
	void (* App_IpcRxInd)(void *);
#endif
	void (* App_IpcConfirm)(void *);
	IBIPC_TXD_T * txd;
	IBIPC_STATS_T stats;
} IBIPC_Table_T;

/* Type of service */
#define IPC_NON_BLOCK_MASK 0x80 /* MSB  = 1, non-blocking; 0, blocking */
#define IPC_DATAGRAM 0
#define IPC_TYPE3    1

/* qflag in Ipc_AppRegLink() */
#define IPC_IN_BAND   0
#define IPC_OUT_BAND  1

/* max application data size */
#define IPC_IB_APP_DATA_SIZE 1484 /* 1514 - (MAC[14]+LLCSNAP[8]+IPC[8]) */
#define IPC_OB_APP_DATA_SIZE 249  /* MAX_SCP_PKT_SIZE (in scp_msgs.h) */

#ifdef OLD
typedef struct ibipc_hdr {
	uInt16 len;
	uChar  did;
	uChar  sid;
	uChar  ctl;
	uChar  rsv0;
	uChar  rsv1;
	uChar  rsv2;
} IBIPC_HDR_T;
#endif

typedef struct ibipc_hdr {
        uChar  Revision;
	uChar  ctl;
	uChar  did;
	uChar  sid;
	uChar  DestSlot;
	uChar  SrcSlot;
	uInt16 len;
} IBIPC_HDR_T;


#define IPC_CTL_DIR_MASK 0x08 /* bit3 = 1, request/indication; 0, response/confirm */
#define IPC_CTL_FT_MASK	 0x04 /* bit2 = 1, datagram; 0, connectionless-acknowledged */

#define IPC_CTL_ACK		0x00
#define IPC_CTL_DATAGRAM	0x0c
#define IPC_CTL_TYPE3		0x08

#define IPC_ACK_TIMER		1000
#define IPC_TX_RETRY		3

/*
 */

typedef struct ibipc_pkt_hdr {
#ifndef NO_VLAN
	uChar  vlan[12]; /* synergy VLAN header */
#endif
	struct etherhdr   a8023;
	struct llcsnap    llcsnap; 
	struct ibipc_hdr  ipc_hdr;
} IBIPC_PKT_HDR_T;

typedef struct ibipc_pkt {
#ifndef NO_VLAN
	uChar  vlan[12]; /* synergy VLAN header */
#endif
	struct etherhdr   a8023;
	struct llcsnap    llcsnap; 
	struct ibipc_hdr  ipc_hdr;
	char   data[1];
} IBIPC_PKT_T;

	/*
	**	SAP id's
	*/
#define IBIPC_SAP     		0x38
#define IBIPC_DLD_ID  		0x0a
#define IBIPC_DL_ID   		0x0b
#define IBIPC_ALC_LANE_ID   0x0c
#define IBIPC_NMP_LANE_ID   0x0d



#define IB_IPC_APP_HDR_SIZE sizeof(IBIPC_HDR_T)
#define IBIPC_ID_WILD_CARD 0xffff
#define IBIPC_HANDLE_WILD_CARD 0xffffffff

#define LLCHDR_SIZE 3
#define A8023HDR_SIZE  17
#define IPC_PKT_HDR_SIZE (IB_IPC_APP_HDR_SIZE + sizeof(struct etherhdr) + sizeof(LLCSNAP))

/* Time out in 1/1000 seconds */
#define IBIPC_DL_STATE_IDLE_TO             0
#define IBIPC_DL_STATE_GET_IMAGE_TO        2000
#define IBIPC_DL_STATE_WRITE_FLASH_TO      10000
#define IBIPC_DL_STATE_WRITE_FLASH_DONE_TO 10000

#define IBIPC_DL_TX_TO                     6000

/* In-band download packet data format
 */

typedef struct ibipc_dl_hdr {
	uChar   cmd;
	uChar   err;
	uInt16 seq;
	/* rsv has expected sequence number in ACK packet for download image or 
	 * the current state if err indicates a wrong state for the command.
	 */
	uInt16 rsv; 
} IBIPC_DL_HDR_T;

/* In-band download control block
 */
typedef struct ibipc_dl_cb {
	Int32 tftp_pid;
	Int32 ipc_pid;
	uInt32 handle;
	uInt32 mtu;
	uChar in_progress;
	uInt16 expect_seq;
	uChar  last_cmd;
	uChar  tftp_wait;
	uChar * txbuf;
	uInt16  txlen;
	uInt16  retry;
	uInt16  sid;
	uInt16  did;
	uInt16  slot;
	uInt16  qflag;
	uChar * image_buf;
	uInt16  image_len;
	uInt32	timer_index;
	IBIPC_DL_HDR_T dlhdr;
} IBIPC_DL_CB_T;

#define IBIPC_DL_STATE_IDLE             0x00
#define IBIPC_DL_STATE_GET_IMAGE        0x01
#define IBIPC_DL_STATE_WRITE_FLASH      0x02
#define IBIPC_DL_STATE_WRITE_FLASH_DONE 0x03

#define IBIPC_DL_MAX_STATE              (IBIPC_DL_STATE_WRITE_FLASH_DONE+1)

#define IBIPC_DL_STATE_NONE             (IBIPC_DL_MAX_STATE + 1)
#define IBIPC_DL_STATE_RETRY		(IBIPC_DL_STATE_NONE + 1)
#define IBIPC_DL_STATE_ABORT		0xff

#define IBIPC_DL_TX_RETRY_CNT           0x09

/* The (MSB == 1) indicates the last image packet tftp client receoved. */
#define TFTP_END_MASK 			0x80

/* 'cmd' field; it is a command if the MSB == 0 or an acknowledge if
 *              the MSB == 1.
 */
#define IBIPC_DL_NOP   0x00
#define IBIPC_DL_REQ   0x01
#define IBIPC_DL_END   0x02
#define IBIPC_DL_IMAGE 0x03
#define IBIPC_DL_COMPLETE_ENQ 0x04

#define IBIPC_DL_RESET 0x05

#define IBIPC_DL_ACK_MASK 0x80

/* 'err' field; it provides the reason of a rejected command.
 */
#define IBIPC_DL_ACK_OK            0x00
#define IBIPC_DL_DUPLICATE_REQ     0x01
#define IBIPC_DL_NO_DOWNLOAD       0x02
#define IBIPC_DL_UNEXPECTED_SEQ    0x03
#define IBIPC_DL_UNKNOWN_CMD       0x04
/*#define IBIPC_DL_WRITING_FLASH     0x05*/
#define IBIPC_DL_CODE_FAILED_CHKSUM 0x05
#define IBIPC_DL_WRONG_STATE       0x06

#define IBDLCB		(IBIPC_DL_CB_T *)_proc->ibdlcb
#define IBDLCB_TFTP_PID	((IBIPC_DL_CB_T *)(_proc->ibdlcb))->tftp_pid
#define IBDLCB_IPC_PID	((IBIPC_DL_CB_T *)(_proc->ibdlcb))->ipc_pid
#define IBDLCB_HANDLE	((IBIPC_DL_CB_T *)(_proc->ibdlcb))->handle
#define IBDLCB_MTU	((IBIPC_DL_CB_T *)(_proc->ibdlcb))->mtu
#define IBDLCB_IN_PROGRESS	((IBIPC_DL_CB_T *)(_proc->ibdlcb))->in_progress
#define IBDLCB_EXPECT_SEQ	((IBIPC_DL_CB_T *)(_proc->ibdlcb))->expect_seq
#define IBDLCB_LAST_CMD	((IBIPC_DL_CB_T *)(_proc->ibdlcb))->last_cmd
#define IBDLCB_TFTP_WAIT	((IBIPC_DL_CB_T *)(_proc->ibdlcb))->tftp_wait
#define IBDLCB_TXBUF	((IBIPC_DL_CB_T *)(_proc->ibdlcb))->txbuf
#define IBDLCB_TXLEN	((IBIPC_DL_CB_T *)(_proc->ibdlcb))->txlen
#define IBDLCB_RETRY	((IBIPC_DL_CB_T *)(_proc->ibdlcb))->retry
#define IBDLCB_SID	((IBIPC_DL_CB_T *)(_proc->ibdlcb))->sid
#define IBDLCB_DID	((IBIPC_DL_CB_T *)(_proc->ibdlcb))->did
#define IBDLCB_SLOT	((IBIPC_DL_CB_T *)(_proc->ibdlcb))->slot
#define IBDLCB_QFLAG	((IBIPC_DL_CB_T *)(_proc->ibdlcb))->qflag
#define IBDLCB_IMAGE_BUF	((IBIPC_DL_CB_T *)(_proc->ibdlcb))->image_buf
#define IBDLCB_IMAGE_LEN	((IBIPC_DL_CB_T *)(_proc->ibdlcb))->image_len
#define IBDLCB_TIMER_INDEX	((IBIPC_DL_CB_T *)(_proc->ibdlcb))->timer_index
#define IBDLCB_DLHDR	((IBIPC_DL_CB_T *)(_proc->ibdlcb))->dlhdr

#if	(defined SYNALC) || (defined IOS_BOOT)
#define	printf	buginf
#endif
