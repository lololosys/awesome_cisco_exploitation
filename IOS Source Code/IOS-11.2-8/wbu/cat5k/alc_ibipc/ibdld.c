/* $Id: ibdld.c,v 1.1.4.1 1996/05/09 14:55:19 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_ibipc/ibdld.c,v $
 *------------------------------------------------------------------
 * ibdld.c
 *
 * March 1995, jjchen
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ibdld.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:19  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.2  1996/05/02  06:47:26  rlowe
 * Clean up compilation warnings for bootrom-specific code.
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.40.1  1996/04/27  06:14:47  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.2  1996/04/26  05:51:28  rlowe
 * Treat compiler warnings as errors.  Eliminate such warnings from
 * current ACP source.  Revise LCP handshake pulse width to be closer
 * to 1.0 usec.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:39  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:56  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  10:52:37  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:47  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:15  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.12.6.1.2.1  1996/02/29  07:00:01  rlowe
 * Changes needed for IOS-side sync to Arkansas V111_0_16
 * (Synalc3_Cal_V111_0_16_branch).
 *
 * Revision 1.12.6.1  1996/01/23  08:49:20  rlowe
 * Suppress excessive trace and propeller output from code image
 * download and flash burn routines.
 *
 * CSCdi46640:  Excessive console output from ACP flash driver.
 * Branch: ATM21_branch
 *
 * Revision 1.12  1995/10/21  04:22:17  vinod
 * Cleaned (with help from Rick), look for download failures to reset
 * IPC download state machine, at end of download verify integrity of
 * code and return status to NMP before programming the FLASH, cleaned
 * up timer usage.
 *
 * Revision 1.11  1995/10/04  01:07:40  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.10  1995/08/31  12:02:37  rlowe
 * Changes to work with IOS 11.0 and higher.
 *
 * Revision 1.9  1995/08/01  03:01:07  rlowe
 * Remove dependency on IOS header file "sys_registry.h" and its
 * sub-dependencies.
 *
 * Revision 1.8  1995/07/29  03:30:20  rlowe
 * Fix to work in IOS kernel case (as well as ACP boot case).
 *
 * Revision 1.7  1995/07/27  21:55:36  rlowe
 * Initial try at getting download to work properly within IOS, using
 * real exploder and download buffers.
 *
 * Revision 1.6  1995/06/23  20:10:57  rlowe
 * Clean-up acquiring of MAC/slot addresses in IOS case.
 *
 * Revision 1.5  1995/05/22  19:19:03  jjchen
 * Removed ipc from ipc_dl SUBSYS_HEADER.
 *
 * Revision 1.4  1995/05/10  23:01:48  jjchen
 * Learned NMP MAC address.
 *
 * Revision 1.3  1995/05/05  18:48:41  jjchen
 *
 * Revision 1.2  1995/05/05  16:27:58  jjchen
 * Modified to support ALC boot download.
 *
 * Revision 1.1  1995/05/01  23:46:43  jjchen
 * Moved from syn_core/nmp/nmpflash/ibipc.
 *
 * Revision 1.2  1995/04/11  20:25:14  rfa
 * Changed name of ValidateAndProgramFlash to dlprogram
 *
 * Revision 1.1  1995/03/29  05:43:16  jjchen
 * Created.
 *
*
 *------------------------------------------------------------------
 * $Endlog$
 */

#if ((! defined IOS_BOOT) || (defined MAKE_DEPEND))
#include "../../../sys/h/master.h"
#include "../../../sys/h/subsys.h"
#include "../../../sys/h/interface_private.h"
#include "../../../sys/h/ieee.h"
#include "../../../sys/h/packet.h"
#include "../../../sys/ui/debug.h"
#include "../../../sys/if/network.h"
#include "../../../sys/h/config.h"
#include "../../../sys/os/old_timer_callbacks.h"
#include "../../../sys/dev/monitor1.h"

#include "bdg_if.h"
#endif

#include "lo_wdog.h"
#include "utypes.h"

#if ((! defined NO_VLAN) || (defined MAKE_DEPEND))
#include "synpacket.h"
#endif

#if ((defined IOS_BOOT) || (defined MAKE_DEPEND))
#include "ipcboot.h"
#endif

#include "acp_hw_defs.h"
#include "acp_image_map.h"
#include "ibframe.h"

#include "ibipc.h"
#include "ib_externs.h"

#ifdef CATALYST_OS
extern struct scdata sc_data[];
#endif
extern uChar ibipc_llcsnap[];
#ifdef CATALYST
#define DRAM_START 0x1000000
uInt32 LOAD_AREA_START = DRAM_START;
#else
#ifdef CATALYST_OS
extern uInt32 LOAD_AREA_START;
#else
uInt32 LOAD_AREA_START = ARAM_DOWNLOAD_BUFR ;	/* For IOS/ACP use. */
#endif
#endif

#ifdef CATALYST_OS
extern void kernel_exit();
#endif

void ipc_dl_init();
void ipc_dl_deamon();
void ipc_dl_addq(uChar *);
void ipc_dl_timeout();
void ipc_dl_rx();
void image_wr_init();
void image_wr_process();

#ifndef CATALYST_OS
boolean ipcdlBLOCK(queuetype *q);
void kill_ipc_dl();
#endif

void DL_TracePkts (uInt16);
/* local variables */

uChar my_slot = 5; /* one based slot number */
uChar nmp_slot = 1; /* one based slot number */
uChar my_mac_address[6] = {0x00, 0x02, 0x03, 0x04, 0x05, 0x06};
uChar nmp_mac_address[6] = {0x01, 0x00, 0x00, 'i', 'p', 'c'};

IBIPC_DL_CB_T ibdlcb;

#ifdef CATALYST_OS
struct ifqueue ipc_dl_q; /* Queue of IPC download packets */
#else
queuetype ipc_dl_q;
#endif		 

int ipc_dl_state_to[IBIPC_DL_MAX_STATE] = {IBIPC_DL_STATE_IDLE_TO,
					   IBIPC_DL_STATE_GET_IMAGE_TO,
					   IBIPC_DL_STATE_WRITE_FLASH_TO,
					   0}; /* IBIPC_DL_STATE_WRITE_FLASH_DONE_TO */
uInt32 image_len;
uChar * image_buf;

#ifndef CATALYST_OS
#ifndef IOS_BOOT
/*
 * IPC subsystem header
 */

#define IPCDL_MAJVERSION 1
#define IPCDL_MINVERSION 0
#define IPCDL_EDITVERSION  1

SUBSYS_HEADER(ipc_dl, IPCDL_MAJVERSION, IPCDL_MINVERSION, IPCDL_EDITVERSION,
	      ipc_dl_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,	/* "seq: ipc", */
	      NULL);	/* "req: ipc"); */

/*
 * ipcdlBLOCK
 *
 * Block waiting for something for the cdp process to do.
 */

boolean ipcdlBLOCK (queuetype *q)
{
    if (q->qhead)
	return(FALSE);			/* input available */

    return(TRUE);
}
#endif
#endif

/* routines */

uChar * get_slot_addr(uChar slot)
{
	if (slot == my_slot)
	    return(my_mac_address);
	else
	    return(nmp_mac_address);
}

uChar * get_my_addr(uChar slot)
{
#ifdef CATALYST_OS
	return(sc_data[0].sc_addr);
#else
	return(my_mac_address);
#endif
}

uInt16 get_nmp_slot()
{
	return(nmp_slot);
}

uInt16 get_my_slot()
{
	return(my_slot);
}

uInt16 ipc_cfg_flag = 0;

#define IPC_CFG_MY_SLOT  0x0001
#define IPC_CFG_NMP_SLOT 0x0002
#define IPC_CFG_MY_MAC   0x0100
#define IPC_CFG_NMP_MAC  0x0200
#define IPC_CFG_ALL (IPC_CFG_MY_SLOT | IPC_CFG_NMP_SLOT | IPC_CFG_MY_MAC | IPC_CFG_NMP_MAC)

void ipc_cfg_address()
{
#ifndef CATALYST_OS				/* For IOS boot/kernel. */
	if (my_slot = LCP_PhysSlotNbr())
	    ipc_cfg_flag |= IPC_CFG_MY_SLOT;

	if (LCP_ReadMacAddr(my_mac_address))
	    ipc_cfg_flag |= IPC_CFG_MY_MAC;

	if (nmp_slot = LCP_NMP_PhysSlotNbr())
	    ipc_cfg_flag |= IPC_CFG_NMP_SLOT;

	if (LCP_NMP_MacAddr(nmp_mac_address))
	    ipc_cfg_flag |= IPC_CFG_NMP_MAC;
#endif
}

/*
 * Initialize IPC download daemon.
 */
#ifdef CATALYST_OS
void test_ipc_init()
{
	ipc_dl_init();
}
#endif

#ifndef IOS_BOOT
void ipc_dl_init()
#else
void ipc_dl_init(uChar * dlptr)
#endif
{
	short pid;

#ifdef CATALYST_OS
	memset((uChar *)&ipc_dl_q, 0, sizeof(struct ifqueue));
	ipc_dl_q.ifq_maxlen = 8;
#else /* Cisco IOS */
	queue_init(&ipc_dl_q, 0);
#endif

	memset((uChar *)&ibdlcb, 0, sizeof(IBIPC_DL_CB_T));

	ibdlcb.in_progress = IBIPC_DL_STATE_NONE;
	ibdlcb.sid = IBIPC_DLD_ID;
	ibdlcb.did = IBIPC_DL_ID;
	ibdlcb.slot = get_nmp_slot();

#ifdef CATALYST_OS
	ibdlcb.timer_index = -1;
#else /* Cisco IOS */
	ibdlcb.timer_index = (int)create_timer (ipc_dl_timeout, (void *)0, 0, 0, "IPC Download", TMT_ONCE);
#endif

#ifdef CATALYST_OS
	if ((pid = create_process("IPC DL Deamon",ipc_dl_deamon,0,(char **)NULL,0)) < 0)
	    printf("Failed to create ipc download deamon\n");
#else /* Cisco IOS */
#ifndef IOS_BOOT
	ibdlcb.ipc_pid = cfork(ipc_dl_deamon, 0L, 0, "IPC DL Deamon", 0);
#else
	if (dlptr)
	    LOAD_AREA_START = (uInt32) dlptr;

	ipc_cfg_address();

	Ipc_AppRegLink(ibdlcb.sid, ibdlcb.did, ibdlcb.slot, 0, 0, 
			(void (*)(void *)) ipc_dl_addq, 0, 1,
			&ibdlcb.handle, &ibdlcb.mtu);
#endif
#endif
}

/*
 * Exit process for the IPC download daemon.
 */

void ipc_dl_exit(uChar * arg, int status)
{
	uInt32 rc;

#ifdef CATALYST_OS
	/* remove timers */
	untimeout (ibdlcb.timer_index);
#else /* Cisco IOS */
	destroy_timer((timer_t *)ibdlcb.timer_index);
#endif
	ibdlcb.timer_index = 0;

	ibdlcb.in_progress = IBIPC_DL_STATE_NONE;

	rc = Ipc_AppDeRegister(ibdlcb.handle);

	if (rc) 
	    printf("IPC de-registration failure : %d\n", rc);
}

/*
 * IPC donwload daemon.
 */

void ipc_dl_deamon(argc, argv)
{
	uInt32 rc;

	uChar * cp;


#ifdef CATALYST_OS
	atexit(ipc_dl_exit, 0);
#else /* Cisco IOS */
#ifndef IOS_BOOT
	edisms((blockproc *)systeminitBLOCK, (ulong) (NULL));
#endif
#endif

	rc = Ipc_AppRegLink(ibdlcb.sid, ibdlcb.did, ibdlcb.slot, 0, 0,
			    (void (*)(void *)) ipc_dl_addq, 0, 1,
			    &ibdlcb.handle, &ibdlcb.mtu);

	if (rc) {
	    printf("IPC registration failure : %d\n", rc); /* panic !!! */
#ifdef CATALYST_OS
	    return;
#else /* Cisco IOS */
	    kill_ipc_dl();
#endif
	}

#ifndef IOS_BOOT
	while (TRUE)
	{
		ipc_dl_rx ();

		if (ibdlcb.in_progress == IBIPC_DL_STATE_ABORT)
			ibdlcb.in_progress = IBIPC_DL_STATE_NONE ;
	}

	kill_ipc_dl();
#endif	/* IOS_BOOT */
}

ipc_dl_tx()
{
	uChar * bp;

	ibdlcb.last_cmd = ((IBIPC_DL_HDR_T *)ibdlcb.txbuf)->cmd;

	if (bp = Ipc_AppMsgMalloc(ibdlcb.txlen)) {
	    bcopy((uChar *) ibdlcb.txbuf, bp, ibdlcb.txlen);
	    Ipc_AppSend(ibdlcb.handle, ibdlcb.did, ibdlcb.slot, IPC_DATAGRAM, ibdlcb.txlen, bp);
	    Ipc_AppMsgFree(bp);
	}

	/* Even if no memory buffer, we still set time out to retransmit.
         */
	if (ibdlcb.in_progress < IBIPC_DL_MAX_STATE) {
	if (ipc_dl_state_to[ibdlcb.in_progress])
#ifdef CATALYST_OS
	    ibdlcb.timer_index = timeout (ipc_dl_timeout, 0, ipc_dl_state_to[ibdlcb.in_progress]);
#else /* Cisco IOS */
	    set_timer((timer_t *)ibdlcb.timer_index, ipc_dl_state_to[ibdlcb.in_progress]);
#endif
	}
}

void ipc_dl_rx()
{
#ifdef CATALYST_OS
	struct mbuf * m;
#else /* Cisco IOS */
#endif
	uChar * mptr;
	uInt16 i, len;
	IBIPC_HDR_T * ipc_hdr;
	IBIPC_DL_HDR_T * ipc_dl_hdr;

	uInt32 rc;
	uChar error_code = IBIPC_DL_ACK_OK;


#ifdef CATALYST_OS
	IF_DEQUEUE(&ipc_dl_q, m);

	if (m == 0)
	    return;
#else /* Cisco IOS */
	paktype * pak;

#ifndef IOS_BOOT
	edisms((blockproc *)ipcdlBLOCK, (ulong)&ipc_dl_q);
#endif

	pak = (paktype *) pak_dequeue(&ipc_dl_q);
	if (!pak) return;
#endif

#ifdef CATALYST_OS
	/* remove timers */
	untimeout (ibdlcb.timer_index);
#else /* Cisco IOS */
	stop_timer((timer_t *)ibdlcb.timer_index);	
#endif

#ifdef CATALYST_OS
	mptr = mtod(m, uChar *);

	ipc_hdr = &((IBIPC_PKT_T *)mptr)->ipc_hdr;
#else /* Cisco IOS */
	mptr = (uChar *)pak;
	ipc_hdr = &((IBIPC_PKT_T *)pak->datagramstart)->ipc_hdr; /* where the VLAN (if any) starts ??? */
#endif
	ipc_dl_hdr = (IBIPC_DL_HDR_T *)(ipc_hdr + 1);

	ipc_dl_hdr->rsv = ipc_dl_hdr->seq;


	switch(ipc_dl_hdr->cmd) {
		case IBIPC_DL_RESET:
			printf("R\n");
			/* learn NMP MAC address */
#ifdef	CATALYST_OS
			bcopy((uChar *)&((IBIPC_PKT_T *)mptr)->a8023.sa, nmp_mac_address, 6);
#else	/* IOS */
			bcopy((uChar *)
				&((IBIPC_PKT_T *)pak->datagramstart)->a8023.sa,
				nmp_mac_address, 6);
#endif
			ibdlcb.in_progress = IBIPC_DL_STATE_IDLE;
			
			break;

		case IBIPC_DL_REQ:
			printf("Q\n");
			if (ibdlcb.in_progress == IBIPC_DL_STATE_IDLE) {
			    ibdlcb.in_progress = IBIPC_DL_STATE_GET_IMAGE;
			    ibdlcb.expect_seq = 1;

			    image_buf = (uChar *)LOAD_AREA_START;
			    image_len = 0;
			    break;
			}

			error_code = IBIPC_DL_WRONG_STATE;

			break;

		case IBIPC_DL_END:
			printf("E\n");
			if (ibdlcb.in_progress == IBIPC_DL_STATE_GET_IMAGE) {
			    ibdlcb.in_progress = IBIPC_DL_STATE_WRITE_FLASH;

			    /* 
			     * Checksum the code and return an acknowledgement
			     */
			    if (BDG_HasExploderChecksumOnly () == TRUE) {
			      error_code = IBIPC_DL_CODE_FAILED_CHKSUM;
			      ibdlcb.in_progress = IBIPC_DL_STATE_ABORT;
			      buginf("Checksum failed\n");
			    } else {
			      buginf("Checksum passed\n");
			    }

			    break;
			}

			error_code = IBIPC_DL_WRONG_STATE;

			break;

		case IBIPC_DL_IMAGE:
			if (ibdlcb.in_progress == IBIPC_DL_STATE_GET_IMAGE) {
			    if (ibdlcb.expect_seq == ipc_dl_hdr->seq) {
				DL_TracePkts (ibdlcb.expect_seq) ;
				ibdlcb.expect_seq++;
				/* Write to image buffer. */
#ifndef	CATALYST_OS		/* IOS or ACP boot case. */
				bcopy((uChar *)(ipc_dl_hdr+1), image_buf, ipc_hdr->len - sizeof(IBIPC_DL_HDR_T));
				image_buf += ipc_hdr->len - sizeof(IBIPC_DL_HDR_T);
				image_len += ipc_hdr->len - sizeof(IBIPC_DL_HDR_T);
#endif
			    }
			    else {
				error_code = IBIPC_DL_UNEXPECTED_SEQ;
				ipc_dl_hdr->rsv = ibdlcb.expect_seq;
			    }
			}
			else {
			    error_code = IBIPC_DL_WRONG_STATE;
			}
			break;

		case IBIPC_DL_COMPLETE_ENQ:
			if (ibdlcb.in_progress == IBIPC_DL_STATE_WRITE_FLASH_DONE) {
			    ibdlcb.in_progress = IBIPC_DL_STATE_IDLE;
			    break;
			}
			
			error_code = IBIPC_DL_WRONG_STATE;
			break;		

		default:
			error_code = IBIPC_DL_UNKNOWN_CMD;
			break;
	}

	/* set up ack packet
         */

	ipc_dl_hdr->cmd |= IBIPC_DL_ACK_MASK;
	ipc_dl_hdr->err = error_code;
	if (error_code == IBIPC_DL_WRONG_STATE)
	    ipc_dl_hdr->rsv = ibdlcb.in_progress;

	bcopy((uChar *) ipc_dl_hdr, &ibdlcb.dlhdr, sizeof(IBIPC_DL_HDR_T));

	ibdlcb.retry = IBIPC_DL_TX_RETRY_CNT;
	ibdlcb.txlen = sizeof(IBIPC_DL_HDR_T);
	ibdlcb.txbuf = (uChar *)&ibdlcb.dlhdr;

	/* release the rx packet
	 */

	Ipc_AppRxDone(ibdlcb.handle, mptr);

	/* send acknowledgement.
	 */

	ipc_dl_tx();

	/*
	 * At the end of the download (good or bad) do NOT bother with
	 * any further application-level message timeouts.
	 */
	if (ibdlcb.in_progress == IBIPC_DL_STATE_ABORT)
	{
		stop_timer((timer_t *)ibdlcb.timer_index);
	}
	else  if (ibdlcb.in_progress == IBIPC_DL_STATE_WRITE_FLASH)
	{
		stop_timer((timer_t *)ibdlcb.timer_index);	
		image_wr_init();
	}
}

void ipc_dl_timeout(uInt32 pp)
{
/*	switch(ibdlcb.in_progress) {
		case IBIPC_DL_STATE_GET_IMAGE:
			break;
		case IBIPC_DL_STATE_WRITE_FLASH:
			break;
		case IBIPC_DL_STATE_WRITE_FLASH_DONE:
			break;
		case IBIPC_DL_STATE_IDLE: *//* since no timer set for it *//*
		default:
			break;
	}*/

	if (--ibdlcb.retry)
	    ipc_dl_tx();
	else {
	    /* Fatal failure */
	    ibdlcb.in_progress = IBIPC_DL_STATE_ABORT;
	    printf("IPC download fatal failure\n");
	}
}

/*
 * Add a received IPC download packet into ipc_dl_q and let IPC download daemon process
 * to process them.
 */
void ipc_dl_addq(uChar * mptr)
{
#ifdef CATALYST_OS
	struct ifqueue *inq;
	struct mbuf * m = dtom(mptr);

	inq = &ipc_dl_q;

	if (IF_QFULL(inq)) {
	    if (0)
	    printf( "ipc_dl_q is full; dropping frame\n");

	    IF_DROP(inq);
	    m_freem(m);
	}
	else {

	    IF_ENQUEUE(inq, m);
	}
#else /* Cisco IOS */
	pak_enqueue(&ipc_dl_q, (void *)mptr);
#endif
}

/*
 * Verify an IPC packet.
 *
 * return: 1 - TRUE, 0 - FALSE.
 */

#ifdef CATALYST_OS
int is_ipc_pkt(struct mbuf * m)
{
    struct a8023hdr *a8023_hdr;
    uChar * cp;
    int i;

    a8023_hdr = mtod(m, struct a8023hdr *);
    cp = (uChar *)&a8023_hdr->llchdr;	
    
    for(i = 0; i < sizeof(LLCSNAP); i++) {
	if (cp[i] != ibipc_llcsnap[i])
	    break;
    }

    if (i == sizeof(LLCSNAP))
	return(1);
    else
	return(0);
}
#else /* Cisco IOS */
int is_ipc_pkt(paktype * pak)
{
    struct a8023hdr *a8023_hdr;
    uChar * cp;
    int i;

    a8023_hdr = (struct a8023hdr *)pak->datagramstart;
    cp = (uChar *)&a8023_hdr->llchdr;	
    
    for(i = 0; i < sizeof(LLCSNAP); i++) {
	if (cp[i] != ibipc_llcsnap[i])
	    break;
    }

    if (i == sizeof(LLCSNAP))
	return(1);
    else
	return(0);
}
#endif


void image_wr_init()
{
	short pid;

#ifdef CATALYST_OS
	if ((pid = create_process("Image Write",image_wr_process,0,(char **)NULL,0)) < 0)
	    printf("Failed to create image write process\n");
#else /* Cisco IOS */
#ifndef IOS_BOOT
#ifdef	JJC
	cfork(image_wr_process, 0L, 0, "Image Write", 0);
#else
	(void) BDG_SetupExploder (TRUE, A_SvcWatchdog) ;
#endif
#endif
#endif
}

void image_wr_process(argc, argv)
{
#ifdef CATALYST_OS
	dlprogram(LOAD_AREA_START, image_len, 0x00000001);
#else /* Cisco IOS */
	/* What is it ??? */
#endif
	
	ibdlcb.in_progress = IBIPC_DL_STATE_WRITE_FLASH_DONE;
}

void DL_TracePkts (uInt16 val)
{
#ifdef	DEBUG
		/* Change to emit a debug trace character every 128 */
		/* packets instead of every packet.  This should be */
		/* about every 64 K instead of every 0.5 K bytes.   */
	if (! (val & 0x007f))
	{
		printf("~");
	}
#endif
}

#ifndef CATALYST_OS

/* Kill the IPC download deamon process.
 */

void kill_ipc_dl()
{
	ipc_dl_exit(0, 0);
#ifndef IOS_BOOT
	process_kill (ibdlcb.ipc_pid);
#endif
}

#endif
