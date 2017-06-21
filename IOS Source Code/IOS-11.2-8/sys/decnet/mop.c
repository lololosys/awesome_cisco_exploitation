/* $Id: mop.c,v 3.8.8.9 1996/09/04 23:55:12 snyder Exp $
 * $Source: /release/112/cvs/Xsys/decnet/mop.c,v $
 *------------------------------------------------------------------
 * mop.c - Maintenance Operation Protocol
 *
 * 30-May-1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mop.c,v $
 * Revision 3.8.8.9  1996/09/04  23:55:12  snyder
 * CSCdi68120:  save a little memory here and there
 *              declare things that are const, const
 *              savings 300 out of data section
 *                       64 out of image
 * Branch: California_branch
 *
 * Revision 3.8.8.8  1996/08/28  12:40:12  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.8.7  1996/07/23  13:21:05  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.8.8.6  1996/07/02  23:10:40  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.8.8.5  1996/06/17  23:30:11  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.8.8.4  1996/05/21  09:44:58  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8.8.3  1996/05/04  22:24:37  asastry
 * CSCdi55017:  Allow MOP to be enabled for routers without ethernet
 * interfaces.
 * Branch: California_branch
 *
 * Revision 3.8.8.2  1996/04/15  21:13:18  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.8.8.1  1996/03/18  19:11:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.4  1996/03/16  06:33:22  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.2.3  1996/03/13  01:19:38  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.2.2  1996/03/07  08:42:42  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.1  1996/02/20  00:43:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/29  02:07:35  asastry
 * CSCdi49406:  Port fix (for MOP over Frame-Relay) to 10.0 and later
 * releases.
 *
 * Revision 3.7  1996/02/07  16:10:48  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.6  1996/02/01  06:00:54  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/30  08:15:27  asastry
 * CSCdi47558:  Fix COPY MOP FLASH feature.
 *
 * Revision 3.4  1996/01/18  20:52:21  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/11/17  08:59:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:15:05  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:16:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/07  22:46:42  hampton
 * Convert DECNET to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi36830]
 *
 * Revision 2.2  1995/06/09 12:59:44  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:23:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "../dev/monitor1.h"
#include "packet.h"
#include "connect.h"
#include "../ui/debug.h"
#include "../if/ether.h"
#include "../if/network.h"
#include "mop.h"
#include "mop_debug.h"
#include "config.h"
#include "file.h"
#include "parser.h"
#include "parser_defs_dec.h"
#include "../os/boot.h"
#include "sched.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../dev/flash_registry.h"

/*
 * Local storage
 */

watched_queue *mopQ;			/* Queue of MOP packets */
boolean mop_running;			/* TRUE when initialization done */
pid_t mopfork;				/* Fork of MOP process */
sys_timestamp mop_sysid_timer;		/* Periodic sysid timer */
uchar mop_boot_multicast[] = { 0xAB, 0x00, 0x00, 0x01, 0x00, 0x00 };
uchar mop_console_multicast[] = { 0xAB, 0x00, 0x00, 0x02, 0x00, 0x00 };
					/* Console multicast address */

boolean mop_load_solicit;		/* TRUE when soliciting a loader */
boolean mop_loading;			/* TRUE when loading MOP data */
sys_timestamp mop_load_timeout;		/* Current MOP load timeout */
paktype *mop_loadpak;			/* Current receive packet */
uchar mop_loadseq;			/* Next expected sequence number */
hwidbtype *mop_load_idb;			/* Selected IDB for loading */

uchar mop_device_code = MOP_DEVICE_CISCO; /* Device code */
uchar mop_retries = MOP_DEFAULT_RETRIES; /* Retries */
ulong mop_retransmit_time = MOP_DEFAULT_RETRANSMIT * ONESEC; /* Retransmit */
#ifdef MOP_DROP_REPLY
int mop_drop;
#endif /* MOP_DROP_REPLY */

/*
 * MOP subsystem header
 */

#define MOP_MAJVERSION 1
#define MOP_MINVERSION 0
#define MOP_EDITVERSION  1

SUBSYS_HEADER(mop, MOP_MAJVERSION, MOP_MINVERSION, MOP_EDITVERSION,
	      mop_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      NULL);

/*
 * mop_start_process - Start the MOP process
 */

static void mop_start_process (void)
{
    if (!mopfork)
      mopfork = process_create(mop_protocols, "MOP Protocols", NORMAL_STACK,
			       PRIO_NORMAL);
}

/*
 * mop_init_idb
 * Initialize an idb for use by MOP
 */

void mop_init_idb (hwidbtype* idb) 
{
    if (!(idb->status & IDB_ETHER)) /* only for Ethernets */
	return;
    idb->mop_enabled = TRUE;
    idb->mop_sysid_enabled = TRUE;
}

/*
 * mop_enqueue
 * Called from interrupt level to put a packet on the MOP input queue
 */

void mop_enqueue (paktype *pak)
{
    if (pak->if_input->hwptr->mop_enabled) {
	net_bridge_copy_broadcast(pak);
	process_enqueue_pak(mopQ, pak);
    } else
	protocol_discard(pak, FALSE);
}

/*
 * mop_process_input
 *
 * Process any packets waiting on the MOP input queue.
 */
static void mop_process_input (void)
{
    int count;
    int len;
    paktype *pak;
    mophdrtype *mop;
    ieee_addrs *addrs;
    hwidbtype *idb;

    count = PAK_SWITCHCOUNT;
    while (--count > 0) {	/* Process a limited number of packets */
	pak = process_dequeue(mopQ); /* Get a packet */
	if (!pak)
	    break;

	mop = (mophdrtype *)mopheadstart(pak);
	addrs = (ieee_addrs *)pak->mac_start;
	idb = pak->if_input->hwptr;	/* Get input interface */

	idb_increment_rx_proto_counts(pak, ACCT_PROTO_MOP);

	len = lit2h(mop->datalen);	/* Get the length */
	if (len < 1 || len > pak->datagramsize - (ETHER_ARPA_ENCAPBYTES + 2)) {
	    if (mop_debug) {
		buginf("\nMOP(%s): Bad length %d received from host %e",
		       pak->if_input->namestring, len, addrs->saddr);
	    }
	} else {
	    switch (mop->code) {	/* Dispatch based on packet type */
	      case MOP_LOAD_XFER:
	      case MOP_LOAD:
	      case MOP_PARAMETER_LOAD_XFER:
		pak = mop_load(idb, pak, len);
		break;
	      case MOP_ASSISTANCE_VOLUNTEER:
		pak = mop_assistance_volunteer(idb, pak, len);
		break;
	      case MOP_REQUEST_ID:
		pak = mop_request_id(idb, pak, len);
		break;
	      case MOP_RESERVE_CONSOLE:
		pak = mop_reserve_console(idb, pak, len);
		break;
	      case MOP_RELEASE_CONSOLE:
		pak = mop_release_console(idb, pak, len);
		break;
	      case MOP_CONSOLE_COMMAND:
		pak = mop_console_command(idb, pak, len);
		break;
	    }
	}
	if (pak)
	    datagram_done(pak);	/* Free the packet */
    }
}

/*
 * mop_protocols
 * This is the process that runs all of the MOP code. It receives packets
 * from the network, and generates appropriate replies. Additionally, it
 * sends out periodic sysid messages.
 */

process mop_protocols (void)
{
    ulong major, minor;
    paktype *pak;

    mop_running = TRUE;
    if (!system_loading) { 
	/*
	 * Only process the time if not bootstrapping.
	 * And, also wait for system initialization to complete.
	 */
	process_wait_on_system_init();
	process_watch_timer(&mop_sysid_timer, ENABLE);
    }

    mopQ = create_watched_queue("MOP packets", 0, 0);
    process_watch_queue(mopQ, ENABLE, RECURRING);
    reg_add_raw_enqueue(LINK_MOP_BOOT, mop_enqueue, "mop_enqueue");
    reg_add_raw_enqueue(LINK_MOP_CONSOLE, mop_enqueue, "mop_enqueue");

    while (mop_running) {
	process_safe_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case QUEUE_EVENT:
		mop_process_input();
		break;

	      case TIMER_EVENT:
		send_periodic_sysid();	/* Send them out */
		break;

	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }

    /*
     * Tear down local data structures.
     */
    reg_delete_raw_enqueue(LINK_MOP_BOOT);
    reg_delete_raw_enqueue(LINK_MOP_CONSOLE);
    process_watch_queue(mopQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(mopQ)) != NULL)
	datagram_done(pak);
    delete_watched_queue(&mopQ);
    process_watch_timer(&mop_sysid_timer, DISABLE);

    mopfork = 0;			/* We are to kill ourselves... */
    process_kill(THIS_PROCESS);		/* Go do it... */
}

/*
 * This section contains SYSID support
 */

/*
 * request_id - Process a request ID message
 *
 * This routine is used to process a request ID message. If the message
 * appears valid (contains a receipt number), we reply with a sysid
 * message sent directly to the requestor.
 */

paktype *mop_request_id (hwidbtype *idb, paktype *pak, int len)
{
    mophdrtype *mop;
    ieee_addrs *addrs;

    mop = (mophdrtype *)mopheadstart(pak);
    addrs = (ieee_addrs *)pak->mac_start;

    if (mop_debug)
	buginf("\nMOP(%s): Got request_id message from %e", 
	       pak->if_input->namestring, addrs->saddr);
    if (len >= 4)
#ifdef MOP_DROP_REPLY
      if (mop_drop++)
#endif /* MOP_DROP_REPLY */
	send_sysid(idb, addrs->saddr, mop->parameters[1] + 
		   (mop->parameters[2] << 8));
    return(pak);
}

/*
 * mop_system_info - Send information about the current system
 *
 * This routine is used to fill in the parameters for messages which require
 * a system to identify itself.
 */

uchar *mop_system_info (hwidbtype *idb, uchar *ptr)
{
    int temp;
    uchar *hw_ptr;

    /* Fill in the version number */

    *ptr++ = MOP_SYSID_VERSION;		/* Set LSB of info type */
    *ptr++ = MOP_SYSID_VERSION >> 8;	/* And MSB */
    *ptr++ = 3;				/* Length is 3 bytes */
    *ptr++ = MOP_VERSION;
    *ptr++ = MOP_ECO;
    *ptr++ = MOP_USER_ECO;

    /* Fill in the supported functions */

    *ptr++ = MOP_SYSID_FUNCTIONS;	/* Set LSB of info type */
    *ptr++ = MOP_SYSID_FUNCTIONS >> 8;	/* And MSB */
    *ptr++ = 2;				/* Length is 2 bytes */
    temp = MOP_FUNCTION_LOOP|MOP_FUNCTION_CONSOLE; /* Get our functions */
    if (idb->mop_console) {		/* If we have a remote console */
	temp |= MOP_FUNCTION_CONSOLE_RESERVED; /* Report that */
    }

    *ptr++ = temp;			/* Set LSB of the functions */
    *ptr++ = temp >> 8;			/* Set MSB of the functions */

    /* Fill in the console user */

    *ptr++ = MOP_SYSID_CONSOLE_USER; 	/* Fill in the function code LSB */
    *ptr++ = MOP_SYSID_CONSOLE_USER >> 8; /* And MSB */
    *ptr++ = IEEEBYTES;		/* Length of the address */
    if (idb->mop_console)	      	/* If there is a console user */
      ieee_copy(((mopcon *)idb->mop_console)->user,ptr);
					/* And the address itself */
    else
      ieee_zero(ptr);			/* Fill in all zeroes if free */
    ptr += IEEEBYTES;			/* Point past the address */

    /* Fill in the console timeout */

    *ptr++ = MOP_SYSID_RESERVATION_TIMER; /* Set function code LSB */
    *ptr++ = MOP_SYSID_RESERVATION_TIMER >> 8; /* And MSB */
    *ptr++ = 2;				/* Length is 2 bytes */
    *ptr++ = MOP_CONSOLE_TIMEOUT;	/* Set LSB */
    *ptr++ = MOP_CONSOLE_TIMEOUT >> 8;	/* And MSB */
    
    /* Fill in the console command size */

    *ptr++ = MOP_SYSID_COMMAND_SIZE;	/* Set function code LSB */
    *ptr++ = MOP_SYSID_COMMAND_SIZE >> 8; /* And MSB */
    *ptr++ = 2;				/* Set length */
    *ptr++ = MOP_CONSOLE_INBUF & 0xff; 	/* And value LSB */
    *ptr++ = MOP_CONSOLE_INBUF >> 8;	/* And MSB */

    /* Fill in the console response size */

    *ptr++ = MOP_SYSID_RESPONSE_SIZE;	/* Set function code LSB */
    *ptr++ = MOP_SYSID_RESPONSE_SIZE >> 8; /* And MSB */
    *ptr++ = 2;				/* Set the length */
    *ptr++ = MOP_CONSOLE_OUTBUF & 0xff;	/* And value LSB */
    *ptr++ = (MOP_CONSOLE_OUTBUF >> 8) & 0xff;	/* And value MSB */

    /* Fill in the hardware address */

    *ptr++ = MOP_SYSID_MAC_ADDRESS & 0xff;	/* Set function code LSB */
    *ptr++ = (MOP_SYSID_MAC_ADDRESS >> 8) & 0xff; /* And function code MSB */
    *ptr++ = IEEEBYTES;		/* Fill in the length */

    if (idb->status & IDB_IEEEMASK) {           /* Fill in the address */
        ieee_copy(idb->hardware, ptr);		
    }
    else {
	/*
	 * Get the hardware address of first ethernet, if present, or hardware
	 * address of first IEEE interface. If we didn't get an address,
	 * just use the default MAC address that was assigned to the serial
	 * interface.
	 */
	hw_ptr = reg_invoke_media_get_first_ieee_hardware_address();
	if (hw_ptr)
	    ieee_copy(hw_ptr, ptr);
	else
	    ieee_copy(idb->hardware, ptr);
    }

    ptr+= IEEEBYTES;			/* And point past it */

    /* Fill in the cisco device code */

    *ptr++ = MOP_SYSID_DEVICE & 0xff;		/* Set function code LSB */
    *ptr++ = (MOP_SYSID_DEVICE >> 8) & 0xff;	/* And function code MSB */
    *ptr++ = 1;				/* Length is one byte */
    *ptr++ = mop_device_code;		/* Set the device code */

    /* Fill in the datalink code */

    *ptr++ = MOP_SYSID_DATALINK & 0xff;	/* Set function code LSB */
    *ptr++ = (MOP_SYSID_DATALINK >> 8) & 0xff;	/* Set function code MSB */
    *ptr++ = 1;				/* Length is one byte */
    *ptr++ = MOP_DATALINK_ETHERNET;	/* Set code as ethernet */

    /* Now fill in the datalink size. Due to compatibility with old
     * versions of MOP, we send 18 larger than the actual value
     */

    *ptr++ = MOP_SYSID_MAXMTU & 0xff;		/* Set function code LSB */
    *ptr++ = (MOP_SYSID_MAXMTU >> 8) & 0xff;	/* Set function code MSB */
    *ptr++ = 2;				/* Length is two bytes */
    temp = idb->maxmtu + 18;		/* Get the max MTU + kludge */
    *ptr++ = temp;			/* Set the LSB */
    *ptr++ = temp >> 8;			/* Set the MSB */

    return(ptr);
}

/*
 * send_sysid - Send a system ID message
 *
 * This routine is used to send a sysid message to a specific interface.
 * It is called periodically to multicast our existance, and is also used
 * to reply to a request ID message.
 */

void send_sysid (hwidbtype *idb, uchar *dest, ushort receipt)
{
    paktype *pak;
    uchar *ptr;
    uchar *dataptr;

    pak = getbuffer(MEDDATA);           /* Get a data buffer */
    if (!pak)				/* Nothing to do if failure */
	return;

    dataptr = mopheadstart(pak);        /* Point to start of data */
    ptr = dataptr + 2;			/* Now point past length word */

    /* Fill in the MOP SYSID header */
    *ptr++ = MOP_SYSTEM_ID;		/* This is a system ID message */
    *ptr++ = 0;				/* Skip reserved byte */
    *ptr++ = receipt;			/* Set LSB of receipt number */
    *ptr++ = receipt >> 8;		/* Set MSB of receipt number */

    ptr = mop_system_info(idb, ptr);	/* Fill in the common fields */

    *(ushort *)dataptr = h2lit(ptr-(dataptr+2)); /* Fill in the length */

    pak->if_output = idb->firstsw;	/* Set up the output interface */
    pak->linktype = LINK_MOP_CONSOLE;	/* Set the link type */
    pak->acct_proto = ACCT_PROTO_MOP;	/* Protocol for accounting */
    pak->enctype = ET_ARPA;		/* Encapsulation is ARPA */

    if (mop_debug) {
	buginf("\nMOP(%s): Sending sysid message to %e", 
	       pak->if_output->namestring, dest);
    }

    if (pak_encap(idb, pak, ptr-dataptr, (long) dest))
	datagram_out(pak);
    else {
	if (mop_debug) {
	    buginf("\nMOP(%s): sysid encapsulation to %e failed", 
		   pak->if_output->namestring, dest);
	}
	datagram_done(pak);
    }
}

/*
 * send_periodic_sysid ()
 *
 * This routine is used to send out the periodic sysid messages.
 *
 * It also notices if no interfaces are running MOP, and kills the
 * process.
 */

void send_periodic_sysid (void)
{
    hwidbtype *idb;
    boolean mop_used = FALSE;

    FOR_ALL_HWIDBS(idb) {
	if (idb->mop_enabled) {
	    mop_used = TRUE;
	    if (idb->mop_sysid_enabled)
	      send_sysid(idb, mop_console_multicast, 0);
	}
    }
    /* Set an 8 - 12 minute timeout */
    TIMER_START_JITTERED(mop_sysid_timer, 12*ONEMIN, 33);
    if (!mop_used)
      mop_running = FALSE;
}

/*
 * This section contains loading support
 */

/*
 * mop_load - Handle received load messages
 *
 * This routine is called by the MOP process when a load, load with transfer
 * address, or parameter load with transfer address message is received.
 * If we have a MOP transfer in process on this interface, we will check to
 * see if this is the proper sequence number (zero is always acceptable).
 * If the sequence number is correct, we pass this packet off to the
 * process doing the loading. If the sequence number is incorrect, we
 * send back a request for the proper sequence number.
 */

paktype *mop_load (hwidbtype *idb, paktype *pak, int len)
{
    uchar address[IEEEBYTES];
    mophdrtype *mop;

    mop = (mophdrtype *)mopheadstart(pak);

    if (mop_loading && !mop_loadpak && (idb == mop_load_idb || 
	!mop_load_idb)) {		/* Looking for a packet? */
	mop_load_idb = idb;		/* Save IDB (multi-circuit,
					 * non-multicast case) */
	if (!mop->parameters[0] || mop->parameters[0] == mop_loadseq) {
	    mop_loadpak = pak;		/* Remeber packet pointer */
	} else {			/* Dropped a request */
	    mop->datalen = h2lit(3);	/* Set the length */
	    mop->code = MOP_REQUEST_MEMORY_LOAD; /* Set function */
	    mop->parameters[0] = mop_loadseq; /* Set sequence number */
	    mop->parameters[1] = 0;	/* Set no error */

	    pak->if_output = idb->firstsw;
	    pak->enctype = ET_ARPA;
	    pak->linktype = LINK_MOP_BOOT;
	    pak->acct_proto = ACCT_PROTO_MOP;
	    ieee_copy(pak->datagramstart+IEEEBYTES, address);

	    if (pak_encap(idb, pak, 5, (long)address))
		datagram_out(pak);
	    else {
		if (mop_debug)
		  buginf("\nMOP(%s): encapsulation of retransmit of"
			 " memory load failed", idb->hw_namestring);
		return(pak);
	    }
	}
	return(NULL);			/* We've used the packet */
    }
    return(pak);			/* Just give the packet back */
}

/*
 * mop_assistance_volunteer - Handle received assistance volunteer messages
 *
 * If we are soliciting for someone to load us, save this packet for whatever
 * is doing the load for us.
 */

paktype *mop_assistance_volunteer (hwidbtype *idb, paktype *pak, int len)
{
    if (mop_load_solicit && !mop_loadpak) {
	mop_loadpak = pak;
	mop_load_idb = idb;
	return(NULL);
    }
    return(pak);
}

/*
 * mop_loadwaitBLOCK - Wait for the MOP loader to be free
 */

boolean mop_loadwaitBLOCK (int dummy)
{
    if (mop_loading || mop_load_solicit)
      return(FALSE);
    return(TRUE);
}

/*
 * mop_loadBLOCK - Wait for a MOP load reply or timeout
 */

boolean mop_loadBLOCK (int dummy)
{
    if (mop_loadpak || AWAKE(mop_load_timeout))
      return(FALSE);
    return(TRUE);
}

/*
 * mop_send_request_program - Send a request program message
 */

void mop_send_request_program (
     hwidbtype *idb,
     char *filename,
     int filelen,
     uchar *mac)
{
    mophdrtype *mop;
    paktype *reqpak = NULL;
    uchar *ptr;
    int len;

    while (!reqpak) {
	reqpak = getbuffer(MEDDATA); /* Get a buffer */
	if (reqpak)
	  break;
	process_sleep_for(ONESEC/2);
    }

    mop = (mophdrtype *)mopheadstart(reqpak);

    ptr = &mop->code;       		/* Point to code field */

    *ptr++ = MOP_REQUEST_PROGRAM;	/* Function is request program */
    *ptr++ = MOP_DEVICE_CISCO;		/* Set our device code */
    *ptr++ = MOP_PROGRAM_VERSION;	/* Set protocol version */
    *ptr++ = MOP_PROGRAM_TYPE_SYSTEM; 	/* Set program type */
    *ptr++ = filelen;			/* Set the filename length */
    bcopy(filename, ptr, filelen);	/* Set up the filename */
    ptr += filelen;			/* Point past it */
    *ptr++ = MOP_PROGRAM_PROC_SYSTEM; 	/* System processor */
    ptr = mop_system_info(idb, ptr); 	/* Fill in the parameters */

    len = ptr - &mop->code;		/* Get the length of the data */
    mop->datalen = h2lit(len);	        /* Set it in the packet */

    reqpak->if_output = idb->firstsw;	/* Set the output interface */
    reqpak->linktype = LINK_MOP_BOOT;	/* And the link code */
    reqpak->acct_proto = ACCT_PROTO_MOP;
    reqpak->enctype = ET_ARPA;		/* Set the encapsulation type */
    if (pak_encap(idb, reqpak, len + 2, (long) mac))
	datagram_out(reqpak);
    else {
	if (mop_debug)
	  buginf("\nMOP(%s): Encapsulation of load solicit failed",
		 idb->hw_namestring);
	datagram_done(reqpak);
    }
}

static const char mop_timedout[] = " timed out\n";
static uint successfulblocks;

/*
 * mop_validations() -
 * Ensure that
 * a) file name is within 16 chars
 * b) MOP process is running
 * c) MOP is enabled on specified interface
 * Also ensure we are not waiting for a response from a
 * MOP server. If we are, block until we either receive
 * the response or time out. In other words, ensure we
 * are in a clean state.
 */
int mop_validations (int filelen, hwidbtype *idb, boolean verbose)
{
    if (filelen > MOP_PROGRAM_MAX_FILENAME) {
        if (verbose)
            printf("\nFilename exceeds %d characters",MOP_PROGRAM_MAX_FILENAME);
        return (ERROR_NOFILE); /* Illegal filename */
    }

    if (!mopfork || (idb && !idb->mop_enabled)) {
        if (verbose)
            printf("\nMOP not enabled");
        return (ERROR_NOFILE);
    }

    /* Wait until the load server becomes free */
    while (mop_loading || mop_load_solicit)
        edisms((blockproc *)mop_loadwaitBLOCK, 0);

    if (mop_loadpak) {  /* Ensure no residual packet */
        datagram_done(mop_loadpak);
        mop_loadpak = NULL;
    }

    /* clear info about any idb that we may have been loading from */
    mop_load_idb = NULL;
    return (~ERROR_NOFILE);
}

/*
 * mop_program_req() - Request a file
 * Send out a request for a file from a MOP server. If an idb
 * is specified, the request is sent out that interface; else
 * the request is sent out all interfaces on which MOP is enabled.
 */
boolean mop_program_req (char *fname, uchar *mac, hwidbtype *idb, boolean verbose)
{
    int retries = mop_retries + 1;
    int filelen;
    hwidbtype *outidb;

    /*
     * If the caller didn't provide an interface, we will send the
     * program request message out all interfaces; otherwise, we will
     * use the specified interface.
     */

    filelen = strlen(fname);
    while (!mop_loadpak && retries--) {
        if (!idb) {
            FOR_ALL_HWIDBS(outidb) {
                if (outidb->mop_enabled)
                    mop_send_request_program(outidb, fname,filelen, mac);
            }
        } else
            mop_send_request_program(idb, fname, filelen, mac);

        TIMER_START(mop_load_timeout, mop_retransmit_time);
        edisms((blockproc *)mop_loadBLOCK, 0); /* wait for response or timeout */
        if (verbose && !mop_loadpak) {
            printf(".");
            flush();
        }
    }

    if (!mop_loadpak) { /* Uh Oh. No response :-( */
        mop_load_solicit = FALSE; /* clean up */
        mop_loading = FALSE;
        if (verbose)
            printf("\nMOP: Timed out on load assistant solicit\n");
        return (FALSE);
    }
    return (TRUE);
}

/*
 * mop_process_pkt() - Process a received packet
 */
int mop_process_pkt (paktype *pak, uchar *bufstart, ulong bufsize,
                     boolean verbose)
{
    mophdrtype *mop;

    int len, datalen, count;
    boolean done = FALSE;

    datalen = 0;
    if (verbose) {
        if ((++successfulblocks & 0x1f) == 0) {
            printf("!");
            flush();
        }
    }

    mop = (mophdrtype *)mopheadstart(pak);
    len = lit2h(mop->datalen);
    if (mop->code == MOP_PARAMETER_LOAD_XFER) {
        done = TRUE;
        len =- 2; /* Account for function and sequence number */
        count = 0; /* Initialize parameter pointer */
        while (len > 0 && mop->parameters[count]) {
            if (len < mop->parameters[count + 1] + 3)
                break; /* Ensure all of parameter is valid */

                /* At this point, you would parse the parameters */

                len -= mop->parameters[count + 1] + 2;
                count += mop->parameters[count + 1] + 2;
        }
    } else {
        if (mop->code == MOP_LOAD)
            datalen = len - 6;
        else {
            datalen = len - 10;
            done = TRUE;
        }
        /*
         * Ensure that the length of the data portion is positive and
         * that the message will fit within our buffer. If so, copy
         * the data into our buffer and return the data length. The
         * caller will use the same packet to reply back.
         * If length is not ok, dump packet, clean up, and return
         * zero. The caller will terminate on receiving zero.
         *
         */

        if (datalen >= 0 && datalen < bufsize) {
            bcopy(&mop->parameters[5], bufstart, datalen);
        } else {
            if (mop_debug)
                buginf("\nMOP: Buffer overflow");
            if (verbose)
                printf(" [Buffer Overflow: %d/%d]\n", datalen, bufsize);
            mop_loadpak = NULL; /* Dump packet pointer */
            mop_loading = FALSE;        /* Not loading anymore */
            datagram_done(pak);		/* Free the packet */
            return (0);
        }
    }
    return (datalen);
}

/*
 * mop_load_reply() - Reply to a received packet, requesting
 * for a block of file data. mop_loadseq gives the block no.
 */
void mop_load_reply (paktype *pak, hwidbtype *outidb, uchar *mac)
{
    mophdrtype *mop;

    mop = (mophdrtype *)mopheadstart(pak);

    mop->datalen = h2lit(3);    /* Set message length */
    mop->code = MOP_REQUEST_MEMORY_LOAD; /* Set function code */
    mop->parameters[0] = mop_loadseq; /* Set sequence number */
    mop->parameters[1] = 0;             /* Set no error */

    pak->if_output = outidb->firstsw;
    pak->enctype = ET_ARPA;
    pak->linktype = LINK_MOP_BOOT;
    pak->acct_proto = ACCT_PROTO_MOP;

    if (pak_encap(outidb, pak, 5, (long)mac))
        datagram_out(pak);
    else {
        if (mop_debug)
            buginf("\nMOP(%s): encapsulation of request memory"
                    " load to %e failed", outidb->hw_namestring, mac);
        datagram_done(pak);
    }
}


/*
 * mop_load_reply_retry() -
 * Wait for the next block of data. If timeout, resend request
 * for next block. Repeat until retry limit exceeded or packet
 * received. If packet received, rejoice.
 */
int mop_load_reply_retry (hwidbtype *idb, uchar *mac, boolean verbose)
{
    int retries = mop_retries + 1;
    paktype *pak;
    mophdrtype *mop;

    /*
     * Loop while packet not received and retry limit not exceeded
     */
    while (!mop_loadpak && retries--) {
        TIMER_START(mop_load_timeout, mop_retransmit_time);
        edisms((blockproc *)mop_loadBLOCK, 0); /* timeout until event */
        if (mop_loadpak)  /* Aahh. A packet received. */
            break;
        pak = getbuffer(SMALLDATA);
        if (!pak) {
            retries++;          /* Don't count as a retry */
            continue;
        }

        mop = (mophdrtype *)mopheadstart(pak);

        if (verbose) {
            printf(".");
            flush();
        }
        mop_load_reply(pak, idb, mac); /* Resend request for data */
    }
    if (!mop_loadpak) {  /* Failure */
        if (mop_debug)
            buginf("\nMOP(%s): Loading from %e timed out",
                    idb->hw_namestring, mac);
        if (verbose)
            printf(mop_timedout);
        mop_loading = FALSE;
        return (ERROR_TIMEOUT);
    }
    return (~ERROR_TIMEOUT);
}

/*
 * mop_read() -
 * Read in a MOP file from a server. This function is called while
 * netbooting from a MOP server.
 * It first does validations, then requests for the file.
 * If the caller has specified the server MAC address, the request
 * is sent to that MAC addr.
 * If the caller has not specified the server MAC addr, a server is
 * first solicited. The solicit is sent with the MOP boot multicast
 * addr (ab00.0001.0000). The server MAC addr is then pulled out from
 * the responding server's reply packet.
 * The file is requested block by block and copied into the caller's
 * buffer.
 */
static int mop_read (char *filename, uchar *bufstart, ulong *bufsize,
              hwidbtype **idb, uchar *mac, uchar *loader_mac, boolean verbose,
              boolean (*func)(uchar **buf, long *bytesleft, long len,
                              void *opaque_args),
              void *func_arg)
{
    uchar assistant_mac[IEEEBYTES];
    hwidbtype *outidb;
    paktype *pak;
    mophdrtype *mop;
    boolean done;
    uchar *buff = bufstart;
    ulong bsize = *bufsize, rcvlen;
    int blen;
    int status;
    ieee_addrs *addrs;

    ieee_zero(assistant_mac);  /* initialize */

    status = mop_validations(strlen(filename), *idb, verbose);
    if (status == ERROR_NOFILE)
        return (ERROR_NOFILE);

    /*
     * If we don't have a destination address to use, set solicit phase
     * and set up to use the MOP boot multicast address. If we do have
     * a destination address, we'll go directly to loading phase, and
     * load from the specified address.
     */

    if (!mac || (mac && ieee_equal(mac, assistant_mac))) { 
        /* Caller hasn't specified server address */
        mop_load_solicit = TRUE;                /* Set solicit phase */
        ieee_copy(mop_boot_multicast, assistant_mac);
    } else {  /* Caller knows server address */
        mop_loading = TRUE;
        ieee_copy(mac, assistant_mac);
    }

    successfulblocks = 0;
    if (verbose) {
        printf("\nLoading %s ", filename);
        flush();
    }
    if (!mop_program_req(filename, assistant_mac, *idb, verbose))
        return (ERROR_TIMEOUT);

    /*
     * Got a load assistant; make a copy of the source MAC address
     * and return the volunteer message.
     *
     * Since we need to remember the source MAC address, this code
     * is currently ethernet-specific.
     */

    outidb = mop_load_idb;
    pak = mop_loadpak;
    mop = (mophdrtype *)mopheadstart(pak);
    addrs = (ieee_addrs *)pak->mac_start;

    if (mop_load_solicit) /* We had solicited a server. Get its addr */
        ieee_copy(addrs->saddr, assistant_mac);
    if (verbose) {
        printf("from interface %s address %e: ", outidb->hw_namestring,
               assistant_mac);
        flush();
    }

    if (mop_load_solicit) {
        mop_load_solicit = FALSE;
        mop_loading = TRUE;
        mop_loadseq = 0;
        mop_loadpak = NULL;
        datagram_done(pak);

        /*
         * Send a program request to the assistant node andawait a reply
         */

        if (!mop_program_req(filename, assistant_mac, outidb, verbose))
            return (ERROR_TIMEOUT);
    }

    /*
     * We have the first packet of the transfer. We will now start our
     * main processing loop, where we actually do the entire file transfer.
     */

    done = FALSE;
    rcvlen = 0;

    while (!done) {
        /*
         * Process first block received
         */
        pak = mop_loadpak;
	mop = (mophdrtype *)mopheadstart(pak);
        blen = mop_process_pkt(pak, buff, bsize, verbose);
        if (!blen) {
            done = TRUE;
            if (mop_loading == FALSE)
                return (ERROR_TOOBIG); /* buffer overflow */
        }
        else {
            bsize -= blen;
            rcvlen += blen;

            /*
             * If user callback function present, call it and let it
             * handle the data in the buffer. Else, just adjust our  
             * buffer pointer and the buffer space left.
             */
            if (func) {
                if ((*func)(&buff, &bsize, blen, func_arg) == FALSE) {
                    mop_loadpak = NULL; /* Dump packet pointer */
                    mop_loading = FALSE;        /* Not loading anymore */
                    datagram_done(pak);		/* Free the packet */
                    if (*idb == NULL)
                        *idb = outidb;   /* pass back interface for download */
                    return (ERROR_ABORT);
                }
            }
            else
                buff += blen;
        }

        mop_loadpak = NULL;     /* Not the input packet anymore */
        mop_loadseq = mop->parameters[0] + 1; /* Set sequence number */

        /*
         * Reply, asking for next block
         */
        mop_load_reply(pak, outidb, assistant_mac);

        /*
         * Check if we received next block. Retry if not, until
         * we receive next block or retry limit is exceeded.
         *
         */
        if (!done) {
            status = mop_load_reply_retry(outidb, assistant_mac, verbose);
            if (status == ERROR_TIMEOUT)
                return (ERROR_TIMEOUT);
        }
    }

    /*
     * We have successfully completed the load. All we needto do now is
     * to mark the server free and return the calculated size.
     */

    mop_loading = FALSE;
    if (verbose)
        printf("\n[OK - %d / %d bytes]\n", rcvlen, *bufsize );

    *bufsize = rcvlen;

    if (*idb == NULL)
        *idb = outidb;

    if (loader_mac)
        ieee_copy(assistant_mac, loader_mac);

    return (ERROR_OK);
}

/*
 * mop_file_read
 * A jacket routine for mop_read() that conforms to the SERVICE_FILE_READ
 * calling conventions.  Index is FILE_ACCESS_MOP
 */

static int mop_file_read (filetype *confg, uchar *buffer, ulong *buffersize,
                   boolean verbose,
                   boolean (*func)(uchar **buf, long *bytesleft, long len,
                                   void *opaque_args),
                   void *func_arg)
{
    int code;

    code = mop_read(confg->filename, buffer, buffersize,
		    &confg->ps.mop.interface,
		    confg->ps.mop.has_addr ? confg->ps.mop.bootaddr : NULL,
		    confg->ps.mop.respondaddr, verbose, func, func_arg);
    return(code);
}

/*
 * mop_config - Handle MOP configuration commands
 */

void mop_config (parseinfo *csb)
{
    if (csb->nvgen) {
	switch (csb->which) {
	  case MOP_CFG_DEVICE_CODE:
	    nv_write((mop_device_code != MOP_DEVICE_CISCO) &&
		     (mop_device_code == GETOBJ(int,1)), csb->nv_command);
	    break;
	  case MOP_CFG_RETRANSMIT:
	    nv_write((mop_retransmit_time !=
		      (MOP_DEFAULT_RETRANSMIT * ONESEC)),
		     "%s %d", csb->nv_command,
		     mop_retransmit_time / ONESEC);
	    break;
	  case MOP_CFG_RETRIES:
	    nv_write((mop_retries != MOP_DEFAULT_RETRIES),
		     "%s %d", csb->nv_command, mop_retries);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case MOP_CFG_DEVICE_CODE:
	if (csb->sense) {
	    mop_device_code = GETOBJ(int,1);
	} else {
	    mop_device_code = MOP_DEVICE_CISCO;
	}
	break;
	
      case MOP_CFG_RETRANSMIT:
	if (csb->sense) {
	    mop_retransmit_time = GETOBJ(int,1) * ONESEC;
	} else {
	    mop_retransmit_time = MOP_DEFAULT_RETRANSMIT * ONESEC;
	}
	break;
      case MOP_CFG_RETRIES:
	if (csb->sense) {
	    mop_retries = GETOBJ(int,1);
	} else {
	    mop_retries = MOP_DEFAULT_RETRIES;
	}
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
    TIMER_START(mop_sysid_timer, 10*ONESEC);	/* Restart sysid timer */
}

void mopif_config (parseinfo *csb)
{
    hwidbtype *idb;

    idb = hwidb_or_null(csb->interface);
    if (! idb) {
	return;
    }

    if (csb->nvgen) {
	switch (csb->which) {
	  case MOP_CFG_ENABLED:
	    if (idb->status & IDB_ETHER) {
		nv_write(!idb->mop_enabled, "no %s", csb->nv_command);
	    } else {
		nv_write(idb->mop_enabled, csb->nv_command);
	    }
	    break;
	  case MOP_CFG_SYSID:
	    if (idb->status & IDB_ETHER) {
		nv_write(!idb->mop_sysid_enabled, "no %s",
			 csb->nv_command);
	    } else {
		nv_write(idb->mop_sysid_enabled, csb->nv_command);
		}
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case MOP_CFG_ENABLED:
	if (csb->set_to_default)
	    csb->sense = (idb->status & IDB_ETHER);
	if (csb->sense) {
	    idb->mop_enabled = TRUE;
	    mop_start_process();
	} else {
	    idb->mop_enabled = FALSE;
	}
	break;
      case MOP_CFG_SYSID:
	if (csb->set_to_default)
	    idb->mop_sysid_enabled = (idb->status & IDB_ETHER);
	else
	    idb->mop_sysid_enabled = csb->sense;
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
    TIMER_START(mop_sysid_timer, 10*ONESEC);	/* Restart sysid timer */
}

/*
 * mop_file_match
 * Does the protocol dependent information in two file descriptors match?
 * Called by SERVICE_FILE_MATCH with index FILE_ACCESS_MOP
 */

static boolean mop_file_match (filetype *cfg1, filetype *cfg2)
{
    if (cfg1->ps.mop.has_addr) {
	if (!cfg2->ps.mop.has_addr)
	    return(FALSE);
	if (!ieee_equal(cfg1->ps.mop.bootaddr, cfg2->ps.mop.bootaddr))
	    return(FALSE);
    }
    return(TRUE);
}

/*
 * mop_bootinfo_get 
 * Record protocol specific boot information.
 * Called by SERVICE_BOOTINFO_GET with index of FILE_ACCESS_MOP
 */

static void mop_bootinfo_get (bootinfo_t *bootinfo, filetype *sysconfg)
{
    idbtype *mopidb;

    ieee_copy(bootinfo->mopaddr, sysconfg->ps.mop.respondaddr);
    parse_interface(bootinfo->mopif, &mopidb);
    sysconfg->ps.mop.interface = hwidb_or_null(mopidb);
}

/*
 * mop_bootinfo_set
 * After loading in the image, tell bootstrap where it came from.
 * Used by SERVICE_BOOTINFO_SET with index FILE_ACCESS_MOP
 */

static void mop_bootinfo_set (bootinfo_t *bootinfo, filetype *sysconfg)
{
    ieee_copy(sysconfg->ps.mop.respondaddr, bootinfo->mopaddr);
    strcpy(bootinfo->mopif, sysconfg->ps.mop.interface->hw_namestring);
}

/*
 * mop_file_source
 * Return string indicating the source of a file
 * Called by SERVICE_FILE_SOURCE, index is FILE_ACCESS_MOP
 */

static void mop_file_source (filetype *file, uchar *buffer)
{
    sprintf(buffer, "mop from %e", file->ps.mop.respondaddr);
}

/*
 * mop_bootfile_parse
 * Given a protocol name and parts of a source address, figure out if
 * MOP should be used to access the file.  Set up fields in file
 * descriptor as appropriate. Returns TRUE if there was a match.
 * Called by SERVICE_BOOTFILE_PARSE, index is FILE_ACCESS_MOP
 */

static boolean mop_bootfile_parse (filetype *file, char *protocol, char *src1,
			    char *src2)
{
    idbtype *idb;
    boolean has_addr;
    uchar bootaddr[IEEEBYTES];

    if (!strmatch(protocol, "mop", FALSE))
	return(FALSE);

    /*
     * Try parsing an IEEE MAC address.
     * If it fails, maybe treat it as an interface.
     */
    has_addr = FALSE;
    if (!null(src1)) {
	has_addr = parse_ether_address(src1, bootaddr);
	if (!has_addr) {
	    src2 = src1;
	    src1 = NULL;
	}
    }

    /*
     * Now try parsing an interface name.
     */
    idb = NULL;
    if (!null(src2)) {
	parse_interface(src2, &idb);
	if (!idb) {
	    printf("\n%% Bad interface or address");
	    return(FALSE);
	}
    }
    file->proto = FILE_ACCESS_MOP;
    file->ps.mop.interface = hwidb_or_null(idb);
    ieee_copy(bootaddr, file->ps.mop.bootaddr);
    file->ps.mop.has_addr = has_addr;
    return(TRUE);
}

/*
 * mop_file_parse
 * Fill in protocol specific information when parsing a file specification
 * Called by SERVICE_FILE_PARSE, index is FILE_ACCESS_MOP
 */

static void mop_file_parse (filetype *file, parseinfo *csb)
{
    char *bufptr;
    hwidbtype *idb;

    if (csb->nvgen) {
	nv_add(TRUE, "mop %s", file->filename);
	nv_add(file->ps.mop.has_addr, " %e", file->ps.mop.bootaddr);
	if (file->ps.mop.interface)
	    nv_add(TRUE, " %s", file->ps.mop.interface->hw_namestring);
        /*
         * Now add boot command to ROM monitor BOOT variable.
         */
        bufptr = reg_invoke_monvar_buff(ROMMON_BOOT_VAR);
        if (bufptr) {
            bufptr += strlen(bufptr);
            idb = file->ps.mop.interface;
            sprintf(bufptr, "mop:%s %e %s,%d;", file->filename,
                    file->ps.mop.has_addr ? (char *)&file->ps.mop.bootaddr[0]:"",
                    idb ? idb->hw_namestring : "", 1);
        }
	return;
    }
    file->ps.mop.has_addr = FALSE;
    file->ps.mop.interface = NULL;
    if (GETOBJ(hwaddr,1)->type != 0) {		/* mop address */
	ieee_copy(GETOBJ(hwaddr,1)->addr, file->ps.mop.bootaddr);
	file->ps.mop.has_addr = TRUE;
    }
    if (GETOBJ(idb,1))				/* mop interface */
	file->ps.mop.interface = GETOBJ(idb,1)->hwptr;
}

/*
 * mop_forus
 *
 * Determine if a packet is for us, i.e. packet is addressed to one of our
 * interface addresses.
 */
boolean mop_forus (paktype *pak)
{
    ieee_addrs *addrs;
    uchar *dstaddr;
    hwidbtype *idb;

    addrs = (ieee_addrs *)pak->mac_start;
    dstaddr = addrs->daddr;
    
    FOR_ALL_HWIDBS(idb) {
	if (ieee_equal(idb->hardware, dstaddr))
	    return (TRUE);
    }

    return (FALSE);
}

/*
 * Parse chains for "copy" command
 * This could be modularized better, but at least this is a start.
 */
#include "../parser/parser_defs_flash.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_config.h"
#include "../dev/flash_defines.h"

#define ALTERNATE	NONE
/* extend "copy mop ..." from here */
LINK_TRANS(exec_copy_mop_file_extend_here, no_alt);
LINK_TRANS(exec_copy_mop_extend_here, exec_copy_mop_file_extend_here);

/* copy mop flash */
EOLS    (exec_copy_mop_flash_eol, flash_copy_command, COPY_MOP_FLASH);
KEYWORD (exec_copy_mop_flash, exec_copy_mop_flash_eol,
         exec_copy_mop_extend_here,
         SF, "Copy to system flash", PRIV_OPR);

/* copy mop bootflash */
EOLS    (exec_copy_mop_bflash_eol, flash_copy_command, COPY_MOP_BOOTFLASH);
KEYWORD (exec_copy_mop_bflash, exec_copy_mop_bflash_eol, exec_copy_mop_flash,
         BF, "Copy to boot flash", PRIV_OPR);
IFELSE  (ifelse_mop_flash, exec_copy_mop_bflash, exec_copy_mop_flash,
         reg_invoke_flash_alias_match(BF));
KEYWORD (exec_copy_mop, ifelse_mop_flash, ALTERNATE,
         "mop", "Copy from a MOP server", PRIV_OPR);
LINK_POINT(mop_copy_commands, exec_copy_mop);
#undef ALTERNATE

/*
 * Parse chain registration array for mop
 */
static parser_extension_request mop_chain_table[] = {
    { PARSE_ADD_COPY_CMD, &pname(mop_copy_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * mop_init
 * Initialize the MOP server
 *
 */

void mop_init (subsystype *subsys)
{
    hwidbtype *idb;
    boolean start;
    list_element *l_elt;

    /*
     * Default all ethernet interfaces to having MOP enabled
     */
    start = FALSE;
      FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_ETHER, l_elt, idb) {
	start = TRUE;
	mop_init_idb(idb);
    }

    /*
     * Allow the MOP subsystem to get initialized, in the case of
     * boxes that have serial, but no ethernet interfaces. If
     * the box had ethernet interfaces, then MOP would already have
     * been started, in which case we don't bother doing this (again).
     */
    if (!start) {
        FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_SERIAL, l_elt, idb) {
	    start = TRUE;
        }
    }

    /*
     * Only start MOP if we have some reason to use it, like we
     * have Ethernet interfaces.
     */	 
    if (!start)
	return;
    mopQ = NULL;
    mop_vty_init();			/* Initialize the VTY handler */
    GET_TIMESTAMP(mop_sysid_timer);	/* Send the first sysid really soon */
    mop_start_process();
    mop_debug_init();
    mop_parser_init();
    mop_media_registry();

    if (!system_loading) {
	/* copy rcp ... */
	parser_add_link_point(PARSE_ADD_COPY_MOP_CMD,
			      "copy mop ...",
			      &pname(exec_copy_mop_extend_here));
	parser_add_link_point(PARSE_ADD_COPY_MOP_FILE_CMD,
			      "copy mop ...",
			      &pname(exec_copy_mop_file_extend_here));
    }
    parser_add_command_list(mop_chain_table, "mop");

    reg_add_hw_idb_init(mop_init_idb, "mop_init_idb");
    reg_add_setup_int2(mop_setup_int, "mop_setup_int");
    reg_add_file_read(FILE_ACCESS_MOP, mop_file_read, "mop_file_read");
    reg_add_file_match(FILE_ACCESS_MOP, mop_file_match, "mop_file_match");
    reg_add_bootinfo_get(FILE_ACCESS_MOP,mop_bootinfo_get, "mop_bootinfo_get");
    reg_add_bootinfo_set(FILE_ACCESS_MOP,mop_bootinfo_set, "mop_bootinfo_set");
    reg_add_file_source(FILE_ACCESS_MOP, mop_file_source, "mop_file_source");
    reg_add_bootfile_parse(mop_bootfile_parse, "mop_bootfile_parse");
    reg_add_file_parse(FILE_ACCESS_MOP, mop_file_parse, "mop_file_parse");
    reg_add_mop_forus(mop_forus, "mop_forus");
}
