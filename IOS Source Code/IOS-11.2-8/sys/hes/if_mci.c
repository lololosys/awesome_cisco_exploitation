/* $Id: if_mci.c,v 3.7.20.10 1996/08/28 12:45:05 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_mci.c,v $
 *------------------------------------------------------------------
 * if_mci.c -- Multi-Port Communications Interface
 *
 * 30-January-1988, Kirk Lougheed
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Gurus: dstine
 *------------------------------------------------------------------
 * $Log: if_mci.c,v $
 * Revision 3.7.20.10  1996/08/28  12:45:05  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.20.9  1996/08/21  19:17:35  fox
 * CSCdi57429:  Router cannot distinguish between FCS bit set/not set in
 * PPP header
 * Branch: California_branch
 * Add support for RFC 1638 bridging packet header fields.  Bounce packets
 * to process level if any fields are set.
 *
 * Revision 3.7.20.8  1996/08/13  02:17:45  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.7.20.7  1996/08/12  16:01:51  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.7.20.6  1996/07/23  13:22:56  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.7.20.5  1996/05/22  14:45:08  fbordona
 * CSCdi57997:  DLSw FST over WAN Frame Relay can not establish circuits
 *              Use reg_invoke_framerelay_fs() for frame-relay RXTYPE_DLSW
 *              Remove DLSw FST frame-relay code from interrupt routines.
 * Branch: California_branch
 *
 * Revision 3.7.20.4  1996/05/21  06:29:20  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.7.20.3  1996/05/17  11:18:21  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.6.2  1996/05/02  22:03:38  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.6.1  1996/04/03  14:30:08  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.7.20.1  1996/03/18  19:41:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.4  1996/03/13  01:22:08  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.2.3  1996/03/07  09:39:53  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.2  1996/02/20  00:52:15  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/01/25  11:17:56  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.6.2.1  1996/01/24  21:46:52  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.6  1996/01/19  22:14:59  wmay
 * CSCdi45344:  ACL: input access lists are ignored on SMDS subinterfaces
 * Sniff smds dest addr and switch to correct swidb.
 *
 * Revision 3.5  1995/12/13  04:19:57  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.4  1995/12/07  22:28:42  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.3  1995/11/17  09:18:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:28:16  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:40:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:00:46  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/10/25  23:09:48  vrawat
 * CSCdi41558:  STUN-basic drops HDLC frames with certain data patterns
 *
 * Revision 2.3  1995/06/22  22:49:20  wmay
 * CSCdi36246:  Appletalk doesnt fast-switch smds
 *
 * Revision 2.2  1995/06/16  17:24:30  fred
 * CSCdi34574:  ip f/s serial -> fddi is broken
 * Propagate the change in the default tx-queue-limit for priority to LAPB
 * also - this significantly simplifies the code
 *
 * Make sure that CBUS controllers (cdb) use same algorithm as MCI
 * controllers (idb).
 *
 * Make displays display actual tx-queue-limit used rather than
 * peremptorily deciding it is 1 in some cases
 *
 * Revision 2.1  1995/06/07  20:41:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "../hes/cbus_registry.h"
#include "media_registry.h"
#include "stacks.h"
#include "interface_private.h"
#include "ttysrv.h"
#include "../ui/debug.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "if_mci.h"
#include "if_hes_common.h"
#include "if_hes_serial.h"
#include "../srt/span.h"
#include "../tbridge/tbridge_debug.h"
#include "if_fci.h"
#include "logger.h"
#include "../ip/ip.h"
#include "../if/network.h"
#include "../wan/smds.h"
#include "../wan/ppp.h"
#include "../if/fddi.h"
#include "../ip/ipfast.h"
#include "../ip/ipfast_hes.h"
#include "dbus.h"
#include "../snmp/chassismib_registry.h"
#include "msg_mci.c"			/* Not a typo, see logger.h */
#include "../os/init.h"


/*
 * External Variables
 */
extern int mci_eth;
extern int mci_ser;



/*
 * Miscellaneous definitions.
 */
#define MCI_TXBUFFERS	    5		/* general TX buffers per interface */

/*
 * Amount of time to wait in ms for an MCI to reset:
 */
#define RESET_TIME	250

#define ERRORS(code, location) \
    regptr->argreg = code; \
    regptr->cmdreg = MCI_CMD_ERRORS; \
    location += regptr->argreg;


/*
 * Local Storage
 */

#define mc_debug 0
queuetype mciQ, cbusQ;
static ushort bitmask[MCI_MAXUNITS] = { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40,
					0x80, 0x100, 0x200 };
int cbus_mci_base_ether;
int cbus_mci_base_serial;


/*
 * mci_init
 * Initialize a controller.  This routine is reentrant.
 * We need to be careful of when we reinitialize a controller.
 * If we are executing critical code (in particular process level output
 * with interrupts enabled) we must not call mci_init() at interrupt level
 * since the output window pointers in the MCI will become random.
 * In that case set the restartdeferred flag and the periodic checker will
 * take care of restarting us.
 */

/*
 * This table is used in the code that sets up the tx queue limit
 * per interface.  These constants are 1/sqrt(nif)*100 which when multiplied
 * by the total number of buffers in the buffer pool divided by 100 gives us
 * the limit of the total number of tx buffers per interface.
 */
static int mci_ktbl[MCI_MAXUNITS*2] = {100, 70, 58, 50, 45, 41, 38, 35, 33, 31,
					30, 29, 28, 27, 26, 25, 24, 24, 23,
					22};
/*
 * Table of MEMA addresses of the tx buffer queue counts. Note, values
 * are octal. The address of the queue count varies depending on
 * the version of the MCI microcode. If new versions of the MCI 
 * microcode are created, these tables need to be updated along 
 * with the code below that references them.
 */
static const short mci_1516[] = {0112, 0147, 0172, 0227}; /* 1.5, 1.6 */
static const short mci_1719[] = {0212, 0247, 0272, 0327}; /* 1.7 - 1.9 */
static const short sci_1011[] = {0117, 0147, 0177, 0227}; /* 1.0, 1.1 */
static const short sci_1213[] = {0217, 0247, 0277, 0327}; /* 1.2, 1.3 */
static const short sip_10[] = {0217, 0247, 0277, 0327}; /* SIP 1.0 */

boolean mci_init (mcitype *mci, boolean criticalflag)
{
    mciregtype *regptr;
    int i, j, buffersize;
    hwidbtype *idb;
    int typecode, buffers, nif, nrx, ntx, buffer_pool;
    ushort result, overhead;
    const short *tblp;
#ifdef C7000
    dgtype* dgbus;
#endif /* C7000 */

    /*
     * Check if this is really a MCI style device.
     */
    if (!mci_controller(mci->regptr)) {
	cbus_init(mci, criticalflag);
	return(TRUE);
    }

    /*
     * First check if executing in a critical section.
     * Reset controller and read version numbers.
     */
    if (criticalflag) {
	mci->restartdeferred = TRUE;
	return(TRUE);
    }
    mci->restartdeferred = FALSE;
    mci->critical = FALSE;
    GET_TIMESTAMP(mci->lastreset);

    regptr = mci->regptr;
#ifdef C7000
    dgbus = pslot_to_dbus(mci->unit);
    if (touch_device(dgbus)) {	
	dbus_ctrlr_reset(dgbus); /* reset */
	dbus_slot_enable(dgbus); /* start without load  */
	DELAY(RESET_TIME);	
	reg_invoke_slot_enable_led_on(mci->unit); /* light board enable LED */
    } else {			/* must be old MCI card, not an SSP */
	regptr->cmdreg = MCI_CMD_RESET;
	DELAY(RESET_TIME);
    }
#else 
    regptr->cmdreg = MCI_CMD_RESET;
    DELAY(RESET_TIME);
#endif
#ifdef C7000
    if (touch_device(dgbus)) {
	mci->hw_version = reg_invoke_get_hw_version(dgbus, mci->unit);
    } else {
	regptr->argreg = MCI_HW_VERSION;
	regptr->cmdreg = MCI_CMD_VERSION;
	mci->hw_version = regptr->argreg;
    }
#else
    mci->hw_version = reg_invoke_get_hw_version(regptr, mci->unit);
#endif
    regptr->argreg = MCI_SW_VERSION;
    regptr->cmdreg = MCI_CMD_VERSION;
    mci->sw_version = regptr->argreg;
    regptr->cmdreg = MCI_CMD_CAPABILITY;
    mci->capability = regptr->argreg;
    
    /*
     * The version 1.0 MCI running 1.3 or less microcode has two obnoxious
     * microcode bugs that cause the TX1_RESERVE and BRIDGEPROTOCOL functions
     * to not work correctly.  We get around the TX1_RESERVE problem by using
     * only the TX0 window.  The BRIDGEPROTOCOL problem is circumvented by
     * using the mema addressing functions to set the correct MCI variables.
     */
#ifdef C7000
    mci->tx1bug = FALSE;
#else
    mci->tx1bug = ((mci->hw_version == 0x100) && (mci->sw_version <= 0x103));
#endif

     /*
      * Scan controller, finding, counting, and initializing any interfaces
      */
    buffersize = 0;
    for (i = 0, nif = 0; i < MCI_MAXUNITS; i++) {
	regptr->argreg = i;
	regptr->cmdreg = MCI_CMD_SELECT;
	result = regptr->cmdreg;
	if (result != MCI_RSP_OKAY)
	    continue;
	result = regptr->argreg;
	typecode = result & 0xFF;
	if ((typecode == MCI_TYPE_ETHER) || (typecode == MCI_TYPE_SERIAL)) {
	    if (mci->interface_list[i] == NULL)
		(void)cbus_mci_idb_init(mci, i, result, NULL, NULL);
	    if (!nMCI)			     /* only make count once */
		if (typecode == MCI_TYPE_ETHER) 
		    mci_eth++;
	        else 
		    mci_ser++;
	    nif++;
	} else
#ifdef C7000
	    printf("\n%%SIP%d: unknown interface type - %d",
		   mci->unit, typecode);
#else
	    printf("\n%%MCI%d: unknown interface type - %d",
		   mci->unit, typecode);
#endif
	/*
	 * Find the largest buffer needed for all interfaces on this mci
	 */
	for(idb = mci->interface_list[i] ; idb != NULL ; 
	    idb = idb->next_idb_on_physint) {
	    if (idb->buffersize > buffersize)
		buffersize = idb->buffersize;
	}
	
	/*
	 * Setup pointers to the location of the transmit queue 
	 * buffer count.
	 */
	tblp = NULL;
#ifndef C7000
	if ((mci->hw_version >> 8) == SCI_HARDWARE) {
	    if ((mci->sw_version == 0x0100) || (mci->sw_version == 0x0101))
		tblp = sci_1011;
	    else if ((mci->sw_version==0x102) || (mci->sw_version==0x103))
		tblp = sci_1213;
	} else {
	    if ((mci->sw_version == 0x105) || (mci->sw_version == 0x106))
		tblp = mci_1516;
	    else if ((mci->sw_version==0x107) || (mci->sw_version==0x108)
		     || (mci->sw_version==0x109))
		tblp = mci_1719;
	}
#else
	tblp = sip_10;
#endif
	if (tblp) {
	    for(idb = mci->interface_list[i]; idb != NULL; 
		idb = idb->next_idb_on_physint)
		idb->mci_txqloc = tblp[i];
	}
    }	
    if (nif == 0) {
#ifdef C7000
	printf("\n%%SIP-%d: no usable interfaces\n\n", mci->unit);
#else
	printf("\n%%MCI-%d: no usable interfaces\n\n", mci->unit);
#endif
	errmsg(&msgsym(NOCOOKIE,MCI),mci->unit);	
	init_errors++;
	return(FALSE);
    }
    
    /*
     * Convert to words
     */
    mci->largest_buffer = buffersize;
    buffersize = ((buffersize + 1) >> 1);

    /*
     * Allocate buffers according to controller configuration.
     * For now we assume that all buffers are the same size.
     */
    regptr->argreg = MCI_MEMSIZE_MEMA;
    regptr->cmdreg = MCI_CMD_MEMSIZE;
    mci->mema_size = regptr->argreg;	/* size of cache memory in words */
    regptr->argreg = MCI_MEMSIZE_MEMD;
    regptr->cmdreg = MCI_CMD_MEMSIZE;
    mci->memd_size = regptr->argreg;	/* size of buffer memory in words */
    regptr->argreg = MCI_OFFSETSIZE;
    regptr->cmdreg = MCI_CMD_OVERHEAD;	/* set size of buffer encaps area */
    overhead = regptr->argreg;		/* read back size of all overhead */
    buffers = (mci->memd_size - 1)/(overhead + buffersize);

    /*
     * Carve up the buffers now that we know the total.  For each interface
     * we allocate MCI_TXBUFFERS.  The rest of the buffers are evenly divided
     * among the receive queues of interfaces.
     *
     * We set the output queue limit of each interface to size of the receive
     * queue less three buffers.  Switching traffic is forbidden to consume
     * the last two transmit buffers (SLIMIT).
     *
     * This algorithm could probably be improved.
     *
     * Improvement #1: Instead of taking nrx-2 for each idb->mci_txcount
     * we now allow buffer_pool/sqrt(nif) per interface.  mci_ktbl[] is
     * scaled by a factor of 100.
     */
    ntx = nif*MCI_TXBUFFERS;
    nrx = (buffers/nif - MCI_TXBUFFERS);
    if (nrx < 3) {
       nrx = 3;
       ntx = buffers - (nrx * nif);
    }
    buffer_pool = buffers - (nrx * nif);        /* total buffer pool size */
    for (i = 0; i < MCI_MAXUNITS; i++) {
	idb = mci->interface_list[i];
	if (idb) {
	    int cnt = buffer_pool * mci_ktbl[nif-1] / 100;
	    
	    mci->bufferalloc[i] = nrx;		/* RX buffers */
	    for( ; idb != NULL ; idb = idb->next_idb_on_physint) {
		if (idb->mci_txcount_enabled)  /* output limit */
		  idb->mci_txcount_default = idb->mci_txcount;
		else
		  idb->mci_txcount_default = idb->mci_txcount = cnt;
	    }
	    regptr->argreg = i;
	    regptr->cmdreg = MCI_CMD_SELECT;
	    regptr->argreg = idb->mci_txcount;
	    regptr->cmdreg = MCI_CMD_QUEUE;
	    regptr->argreg = 2;
	    regptr->cmdreg = MCI_CMD_SLIMIT;
	}
    }

    /*
     * Set the TX and RX buffer size variable
     */
    regptr->argreg = buffersize;
    regptr->cmdreg = MCI_CMD_BUFFERSIZE;

     /*
      * Allocate the receive buffers according to our calculations.
      */
    for (i = 0; i < MCI_MAXUNITS; i++) {
	int rx_offset;
	
	idb = mci->interface_list[i];
	if (!idb)
	    continue;
	if ((idb->status & IDB_SERIAL)) {
	    /*
	     *  There is only one rx_offset for all virtual channels on an ICB.
	     *  This is calculated based on the encaps size of the first
	     *  channel on the list.  (On the platforms that use this
	     *  module, rxoffset is measured in words.)
	     */
	    rx_offset = MCI_OFFSETSIZE - (idb->encsize >> 1);
	    for( ; idb != NULL ; idb = idb->next_idb_on_physint)
		idb->rxoffset = rx_offset;
	} else
	    rx_offset = idb->rxoffset;
        mci->rxbuffers[i] = 0;
	regptr->argreg = i;
	regptr->cmdreg = MCI_CMD_SELECT;
	for (j = 0; j < mci->bufferalloc[i]; j++) {
	    regptr->argreg = rx_offset;
	    regptr->cmdreg = MCI_CMD_RX_ALLOCATE;
	    result = regptr->cmdreg;
	    if (result == MCI_RSP_OKAY)
	        mci->rxbuffers[i]++; 
	    else {
#ifdef C7000
	        printf("\n%%SIP%d: RX buffer alloc failed", mci->unit);
#else
	        printf("\n%%MCI%d: RX buffer alloc failed", mci->unit);
#endif
	        return(FALSE);
	    }
	}	
    }
    
    /*
     * Allocate rest of memory for transmit buffers.
     * Make sure we allocate at least as many buffers as we expected.
     */
    mci->txbuffers = 0;
    while (TRUE) {
	regptr->argreg = 0;
	regptr->cmdreg = MCI_CMD_TX_ALLOCATE;
	result = regptr->cmdreg;
	if (result != MCI_RSP_OKAY)
	    break;
	mci->txbuffers++;
    }
    if (mci->txbuffers < ntx) {
#ifdef C7000
	printf("\n%%MCI%d: TX buffer alloc failed: want %d, got %d",
	       mci->unit, ntx, mci->txbuffers);
#else
	printf("\n%%MCI%d: TX buffer alloc failed: want %d, got %d",
	       mci->unit, ntx, mci->txbuffers);
#endif
	return(FALSE);
    }

    /*
     * Set interrupt level for this controller.
     * This clears any pending interrupts.
     * Clear the lights.
     */
    mci->lights = 0;
    regptr->argreg = MCI_INTLEVEL;
    regptr->cmdreg = MCI_CMD_INTERRUPT;
    regptr->argreg = 0;
    regptr->cmdreg = MCI_CMD_LIGHTS;
    
    /*
     * Initialize the hardware of each interface.
     * Note that enabling an interface causes an interrupt.
     */
    for (i = 0; i < MCI_MAXUNITS; i++) {
	/*
	 *  We only do the first idb on each list, as the underlying hw is
	 *  the same
	 */
	idb = mci->interface_list[i];
	if (idb) {

	    /*
	     * Set up protocol specific bridging/routing functions
	     */
	    cbus_mci_rxtypes_update(idb);

	    TIMER_STOP(idb->xmittimer);
	    (*idb->reset)(idb);
	    if (idb->counters.resets > 0)
		idb->counters.resets--;
	}
    }

    return(TRUE);
}

/* 
 * mci_controller
 * Determine if a MCI controller lives at the passed multibus address.
 */

boolean mci_controller (mciregtype *regptr)
{
    ushort hw_version;
    
    regptr->argreg = MCI_HW_VERSION;
    regptr->cmdreg = MCI_CMD_VERSION;
    hw_version = regptr->argreg;
    hw_version >>= 8;
    return(hw_version == MCI_HARDWARE || hw_version == SCI_HARDWARE);
}

static inline void
add_mci_cards (void)
{
    mcitype* mci;
    int serialnum;

    for (mci = (mcitype*) mciQ.qhead; mci != NULL; mci = mci->next) {
	char* descr;
	card_type_t type;
	uint serial_fast, serial_slow;

	{
	    uint ix;
	    hwidbtype* hwidb;

	    serial_fast = serial_slow = 0;
	    for (ix = 0; ix < MCI_MAXUNITS; ++ix) {
		/* MCI only, so it's safe not to check the whole idb list */
		hwidb = mci->interface_list[ix];
		if (hwidb == NULL)
		    continue;
		if (hwidb->status & IDB_SERIAL) {
		    serialsb *ssb;
		    ssb = idb_get_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
		    if (ssb->serial_protocol & MCI_CAP_SPEED)
			++serial_fast;
		    else
			++serial_slow;
		}
	    }
	}
	serialnum = NO_SERIALNUM;
	switch (mci->hw_version >> 8) {
	  case MCI_HARDWARE:
	    switch (mci->ether_count) {
	      case 0:			/* 0E */
		switch (serial_slow) {
		  case 0:			/* 0S */
		    switch (serial_fast) {
		      case 1:  type = D_cardType_mci1t;		break;
		      case 2:  type = D_cardType_mci2t;		break;
		      default: type = D_cardType_unknown;	break;
#ifdef C7000
		      case 4: {
			  dgtype* dbus;

			  type = D_cardType_sip;
			  descr = "Four-port SWbus serial";
			  dbus = pslot_to_dbus(mci->unit);
			  if (dbus)
			      serialnum = dbus_serial_number(dbus);
			  goto do_version;
		      }
#endif
		    }
		    break;
		  case 1:			/* 1S */
		    switch (serial_fast) {
		      case 0:  type = D_cardType_mci1s;		break;
		      case 1:  type = D_cardType_mci1s1t;	break;
		      default: type = D_cardType_unknown;	break;
		    }
		    break;
		  case 2:  type = D_cardType_mci2s;	break;
		  default: type = D_cardType_unknown;	break;
		}
		break;
	      case 1:			/* 1E */
		switch (serial_slow) {
		  case 0:			/* 0S */
		    switch (serial_fast) {
		      case 0:  type = D_cardType_mci1e;		break;
		      case 1:  type = D_cardType_mci1e1t;	break;
		      case 2:  type = D_cardType_mci1e2t;	break;
		      default: type = D_cardType_unknown;	break;
		    }
		    break;
		  case 1:			/* 1S */
		    switch (serial_fast) {
		      case 0:  type = D_cardType_mci1e1s;	break;
		      case 1:  type = D_cardType_mci1e1s1t;	break;
		      default: type = D_cardType_unknown;	break;
		    }
		    break;
		  case 2:  type = D_cardType_mci1e2s;	break;
		  default: type = D_cardType_unknown;	break;
		}
		break;
	      case 2:			/* 2E */
		switch (serial_slow) {
		  case 0:			/* 0S */
		    switch (serial_fast) {
		      case 0:  type = D_cardType_mci2e;		break;
		      case 1:  type = D_cardType_mci2e1t;	break;
		      case 2:  type = D_cardType_mci2e2t;	break;
		      default: type = D_cardType_unknown;	break;
		    }
		    break;
		  case 1:			/* 1S */
		    switch (serial_fast) {
		      case 0:  type = D_cardType_mci2e1s;	break;
		      case 1:  type = D_cardType_mci2e1s1t;	break;
		      default: type = D_cardType_unknown;	break;
		    }
		    break;
		  case 2:  type = D_cardType_mci2e2s;	break;
		  default: type = D_cardType_unknown;	break;
		}
		break;
	      default:
		type = D_cardType_unknown;
		break;
	    }
	    descr = "MCI interface";
	    break;

	  case SCI_HARDWARE:
	    switch (serial_fast) {
	      case 0:  type = D_cardType_sci4s;		break;
	      case 2:  type = D_cardType_sci2s2t;	break;
	      case 4:  type = D_cardType_sci4t;		break;
	      default: type = D_cardType_unknown;	break;
	    }
	    descr = "SCI interface";
	    break;

	  default:
	    type = D_cardType_unknown;
	    descr = "Unknown MCI hardware";
	    break;
	}
#ifdef C7000
do_version:
#endif
	{
	    char* cp;
	    char* hwver;
	    char* swver;
	    char buf[48];
	    
	    cp = buf;
	    cp += sprintf(hwver = cp, "%u.%u",
			  mci->hw_version >> 8, mci->hw_version & 0xff);
	    cp += sprintf(swver = ++cp, "%u.%u",
			  mci->sw_version >> 8, mci->sw_version & 0xff);
	    reg_invoke_chassismib_add_card_entry(type, NO_HOTSWAP, descr,
						 serialnum, hwver, swver,
#ifdef C7000
						 mci->unit);
#else
	                                         NO_SLOTNUMBER);
#endif
	}
    }
    return;
}


/*
 * mci_set_rxoffset
 * Set the rx offset in the MCI card
 */
static void mci_set_rxoffset (hwidbtype *idb)
{
    mcitype *mci;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    mci = idb->devctl;
    mci_init(mci, FALSE);
    reset_interrupt_level(level);
}

/*
 * transmitter_command
 * Specify the transmitter delay in microseconds.  This delays successive
 * transmissions, a nice thing to do for slow receivers.
 * Since the MCI deals with delay units of roughly two microseconds, we
 * halve the user-specified microsecond argument.  We clamp the result at
 * 16 bits.
 */

void mci_set_transmit_delay (hwidbtype *idb, int value)
{
    mcitype *mci;
    leveltype level;
    mciregtype *regptr;

    mci = idb->devctl;

    idb->xmitdelay = value;
    regptr = mci->regptr;
    level = raise_interrupt_level(NETS_DISABLE);
    regptr->argreg = idb->mci_index;
    regptr->cmdreg = MCI_CMD_SELECT;	/* select interface */
    regptr->argreg = value >> 1;
    regptr->cmdreg = MCI_CMD_DELAY;
    reset_interrupt_level(level);
}


/*
 * mci_txqcount
 * Reads the MCI/SCI transmit buffer queue count which is the number
 * of free transmit buffers for this interface and returns the queue
 * size minus this number which is the the number of active buffers.
 */
int mci_txqcount (hwidbtype *idb)
{
    short val;
    mciregtype *regptr;
    leveltype level;

    /*
     * idb->mci_txqloc won't be set for MCI interfaces that we
     * have not coded a value for in the tables in mci_init().
     */
    if ((!idb->mci_txqloc) || (!(idb->status & IDB_MCI)))
	return(0);

    regptr = ((mcitype *) idb->devctl)->regptr;
    level = raise_interrupt_level(NETS_DISABLE);
    regptr->argreg = idb->mci_txqloc;
    regptr->cmdreg = MCI_CMD_MEMA_SELECT;
    regptr->cmdreg = MCI_CMD_MEMA_READ;
    val = regptr->argreg;
    reset_interrupt_level(level);
    return(idb->mci_txcount - val);
}


/*
 * MCI_ETHER_RECEIVE
 * Receive a single datagram on an Ethernet interface. 
 * This is a macro rather than a subroutine to eliminate the overhead
 * of a subroutine call.  Debug with an oscilloscope!
 */

#define MCI_ETHER_RECEIVE						\
									\
    /*									\
     * Check frame status						\
     */									\
    status = regptr->rxstatus;						\
    if ((status & MCI_EX_MASK) == 0) {					\
eretry:									\
	if ((bytes <= MCI_MAXDGRAM_DEF) && (bytes >= MINETHERSIZE)) {	\
									\
	    /*								\
	     * Determine if we should bridge this datagram.	   	\
	     * Attempt to bridge any packet with the bridge bit set.	\
	     * This attempt will succeed if the packet was forwarded,	\
	     * flooded, or filtered by the transparent bridge code.	\
	     * This attempt will fail for a variety of packets including\
	     * interesting multicasts (aliases will be flooded), and	\
	     * physical broadcasts in protocols we are routing (other	\
	     * physical broadcasts will be flooded).			\
	     */								\
									\
            if (status & (MCI_BRIDGING_MASK << 8)) {			\
		if (reg_invoke_tbridge_mci_forward(idb->fast_rcvidb))	\
		    goto rxexit;					\
		else {							\
		    erxtype = regptr->rxtype;				\
		    goto erxread;					\
		}							\
            }								\
									\
            /*								\
	     * Very high speed switching of routed protocols	        \
	     */								\
	    erxtype = regptr->rxtype;					\
            if (reg_invoke_ether_fs(erxtype, idb))			\
		goto rxexit;						\
	    else							\
		goto erxread;						\
	} else {							\
	      /*							\
	       * Datagram not within allowed size.			\
               */             						\
	      cbus_mci_input_error(idb);				\
              regptr->cmdreg = MCI_CMD_RX_FLUSH;			\
	      if (bytes > MCI_MAXDGRAM_DEF)				\
                  idb->counters.giants++;				\
              else							\
		  idb->counters.runts++;				\
              goto rxexit;						\
        }								\
   } else {                                                             \
	 /*								\
	  * If the only problem is dribble, packet is okay.		\
	  */								\
    	 if ((status & MCI_EX_MASK) == MCI_EX_FRAME) { 			\
	    idb->counters.input_dribble++;				\
	    goto eretry;						\
	 }								\
									\
         /*								\
          * MCI discovered an error -- report it.                       \
	  */								\
	 if (status & MCI_EX_CRC)	/* CRC error */			\
	     idb->counters.input_crc++;					\
	 if (status & MCI_EX_RUNT)	/* runt received */		\
	     idb->counters.runts++;					\
	 if (status & MCI_EX_OVERRUN)	/* receiver overrun */		\
	     idb->counters.input_overrun++;				\
         if (status & MCI_EX_FRAME)	/* framing error */		\
	     idb->counters.input_frame++;				\
         cbus_mci_input_error(idb);					\
         regptr->cmdreg = MCI_CMD_RX_FLUSH;				\
         goto rxexit;							\
    }									\
									\
    /*									\
     * Sniff start of packet						\
     */									\
erxread:								\
    regptr->argreg = idb->rxoffset + 2*IEEEWORDS;			\
    regptr->cmdreg = MCI_CMD_RX_SELECT;					\
									\
    /*									\
     * Read in the ethernet encapsulation junk.  Both			\
     * reg_invoke_hes_bridge_broadcasts() and the check before          \
     * mci_vring_input()  use this.				       	\
     */									\
    idb->sniff[SNIFFBASE+0].d.lword = regptr->readlong;			\
    idb->sniff[SNIFFBASE+1].d.lword = regptr->readlong;			\
    idb->sniff[SNIFFBASE+2].d.lword = regptr->readlong;			\
									\
    /*									\
     * Check for fast RSRB. This is 802.2 so the offsets are not        \
     * the same as they would be if it was an ethernet frame.           \
     */									\
    if ((idb->sniff[SNIFFBASE+0].d.sword[1] == SNAPSNAP) &&		\
	(idb->sniff[SNIFFBASE+2].d.sword[0] == TYPE_RSRB))		\
	    if (reg_invoke_mci_vring_input(idb))			\
                goto rxexit;						\
									\
    if (ROUTING_RXTYPE(idb->fast_rcvidb, erxtype)) {			\
	/*								\
	 * We piggy packed on the last readlong above to scrape off	\
	 * the IP packet type to see if this is an ip/udp datagram.	\
	 * Note: udp_fast_flood() relies on the rx pointer to be	\
	 * positioned at the ip checksum.				\
	 */								\
	if (erxtype == RXTYPE_DODIP) { 		           		\
	    switch (idb->sniff[SNIFFBASE+2].d.byte[3]) {		\
	    case UDP_PROT:						\
		if (reg_invoke_udp_fast_flood(idb->fast_rcvidb))	\
		    goto rxexit;					\
		break;							\
	    case FST_DLSW_PROT:						\
		if (reg_invoke_mci_dlsw_fst_forward(idb))		\
		    goto rxexit;					\
		break;							\
	    case FST_RSRB_PROT:						\
		if (reg_invoke_mci_rsrb_fst_forward(idb))		\
		    goto rxexit;					\
		break;							\
	    case GRE_PROT:						\
	    case IPINIP_PROT:						\
		if (reg_invoke_tunnel_ip_hes_fs(idb))		        \
		    goto rxexit;					\
		break;							\
	    }								\
	}								\
    }									\
									\
    /*									\
     * We are going to receive this datagram.				\
     * Get a packet buffer and set it up.				\
     * If we can't get a buffer, assume some sort of congestion problem \
     * and disable the interface for a very short while so the process  \
     * level code can run and recover.					\
     */									\
    pak = input_getbuffer(bytes, idb);					\
    if (!pak) {								\
	cbus_mci_idb_throttle(idb);					\
        goto rxexit;							\
    }									\
									\
    pak->datagramsize = bytes;						\
    (*idb->parse_packet)(idb, pak, &idb->sniff[SNIFFBASE+0].d.byte[0]);	\
    if (status & idb->multibits)					\
	idb->counters.input_broad++;					\
    if (idb->mci_checked) {						\
	pak->flags |= PAK_MCICHECKED;					\
	idb->mci_checked = FALSE;					\
    }									\
    if (idb->ipmfast_switched) {			       		\
	pak->flags |= PAK_MFS_SWITCHED;    			        \
	idb->ipmfast_switched = FALSE;					\
    }	                                                                \
    									\
    /*									\
     * Set pointer to start of packet and read it into packet buffer.	\
     * Hand to raw input process and flush datagram in MCI RX buffer.	\
     */									\
    regptr->argreg = idb->rxoffset;					\
    regptr->cmdreg = MCI_CMD_RX_SELECT;					\
    mci2buffer(&regptr->readlong, pak->datagramstart, bytes);		\
    regptr->cmdreg = MCI_CMD_RX_FLUSH;					\
    idb->counters.rx_cumbytes += bytes;					\
    idb->counters.inputs++;						\
    if (pak->enctype == ET_LLC2)					\
        llc_input(pak);         					\
    else								\
	(*(idb->iqueue)) (idb, pak);					\
    goto rxexit;


/*
 * MCI_SERIAL_RECEIVE
 * Receive a single datagram on a Serial interface.
 * This is a macro rather than a subroutine to eliminate the overhead
 * of a subroutine call.  Debug with an oscilloscope!
 */

#define MCI_SERIAL_RECEIVE                                              \
    charlong sniffcode;							\
									\
    /*									\
     * Check frame status						\
     */									\
    status = regptr->rxstatus;						\
    if (status & MCI_SX_MASK) {						\
	if (status & MCI_SX_CRC)	/* CRC error */			\
	    idb->counters.input_crc++;					\
	if (status & MCI_SX_ABORT)	/* abort received */		\
	    idb->counters.input_abort++;				\
	if (status & MCI_SX_OVERRUN)	/* receiver overrun */		\
	    idb->counters.input_overrun++;				\
	if (status & MCI_SX_FRAME)	/* short frame */		\
	    idb->counters.input_frame++;				\
	cbus_mci_input_error(idb);					\
	regptr->cmdreg = MCI_CMD_RX_FLUSH;				\
	goto rxexit;							\
    }									\
									\
    /*									\
     * Check frame size.						\
     */									\
    if (bytes > idb->max_pak_size) {					\
	idb->counters.giants++;						\
	cbus_mci_input_error(idb);					\
	regptr->cmdreg = MCI_CMD_RX_FLUSH;				\
	goto rxexit;							\
    } else if ((short)bytes < MINSERIALSIZE) {				\
	idb->counters.runts++;						\
	cbus_mci_input_error(idb);					\
	regptr->cmdreg = MCI_CMD_RX_FLUSH;				\
	goto rxexit;							\
    }									\
									\
    /*									\
     * phoney_rxtype says we don't do fast switching on input.		\
     */									\
    if (idb->phoney_rxtype) {						\
	goto srxread;							\
    }									\
    /*									\
     * Very high speed switching of routed protocols			\
     */									\
    summary = regptr->rxtype;						\
    switch (idb->enctype) {						\
    case ET_SMDS:							\
       /*								\
        * Switch to correct subinterface - we need to sniff the dest	\
        * address							\
        */								\
	regptr->argreg = idb->rxoffset +				\
	    btow(idb->smds_pdb->fast_input_modelen) + 2;		\
        regptr->cmdreg = MCI_CMD_RX_SELECT;				\
        PUTLONG(idb->smds_pdb->fast_smds_dest, regptr->readlong);	\
        PUTLONG(&idb->smds_pdb->fast_smds_dest[4], regptr->readlong);	\
	idb->fast_rcvidb =						\
	    reg_invoke_media_addr_to_swidb(ET_SMDS,			\
					   idb->smds_pdb->fast_smds_dest, \
					   idb);			\
	if (idb->fast_rcvidb == NULL) goto srx_process;			\
        regptr->argreg = MCI_SERIAL_SMDS_OFFSET + 			\
			  btow(sizeof(smdstype) + sizeof(snap_hdr));	\
        regptr->cmdreg = MCI_CMD_RX_SELECT;				\
	temp = regptr->readshort;					\
        switch (temp) {							\
	case TYPE_CLNS:							\
            summary = RXTYPE_UNKNOWN;					\
	    goto srxread;						\
	case TYPE_VINES:						\
	case TYPE_VINES2:						\
            summary = RXTYPE_VINES;					\
	    break;							\
	case TYPE_ETHERTALK:						\
            summary = RXTYPE_APPLE;					\
	    break;							\
        case SMDS_BRIDGE_PID_8023FCS:					\
        case SMDS_BRIDGE_PID_8023:					\
        case SMDS_BRIDGE_PID_8025FCS:					\
        case SMDS_BRIDGE_PID_8025:					\
        case SMDS_BRIDGE_PID_FDDIFCS:					\
        case SMDS_BRIDGE_PID_FDDI:					\
	     regptr->argreg = idb->rxoffset + btow(idb->encsize) - 2;	\
	     regptr->cmdreg = MCI_CMD_RX_SELECT;			\
	     temp = regptr->readshort;					\
             if (temp == SMDS_BRIDGE_OUI) {				\
		 idb->sniff[SNIFFBASE+0].d.sword[0] = regptr->readshort;\
		 goto srxbridge;					\
	     }								\
	     idb->sniff[SNIFFBASE+0].d.lword = 0;			\
             goto srx_skip;						\
	} 	 							\
        regptr->argreg = MCI_OFFSETSIZE + MCI_IP_DSTADR_OFFSET;		\
        regptr->cmdreg = MCI_CMD_RX_SELECT;				\
        if (reg_invoke_smds_fs(summary, idb))				\
            goto rxexit;						\
        regptr->argreg = MCI_OFFSETSIZE - 2;				\
        regptr->cmdreg = MCI_CMD_RX_SELECT;				\
	sniffcode.d.lword = regptr->readlong;				\
	if (sniffcode.d.sword[0] == SMDS_BRIDGE_OUI &&			\
	    sniffcode.d.sword[1] == SMDS_BRIDGE_PID_8023)		\
	    goto srxbridge;						\
	idb->sniff[SNIFFBASE+0].d.lword = 0;				\
	goto srx_skip;							\
	break;								\
    case ET_HDLC:							\
        if (reg_invoke_hdlc_fs(summary, idb))				\
            goto rxexit;						\
	break;								\
    case ET_PPP:							\
	if (summary == RXTYPE_UNKNOWN) {				\
	    regptr->argreg = MCI_OFFSETSIZE - 1;			\
	    regptr->cmdreg = MCI_CMD_RX_SELECT;				\
            summary = regptr->readshort;				\
	    summary = ppp_decode_rxtype_inline(summary);		\
	}								\
	if (    (summary != RXTYPE_UNKNOWN)				\
            && reg_invoke_ppp_fs(summary, idb))				\
	    goto rxexit;						\
        break;								\
    case ET_FRAME_RELAY:						\
	if (reg_invoke_fr_pvc_switch_or_fs(idb))			\
	   goto rxexit;							\
	goto srx_skip;							\
	break;								\
    case ET_ATM_DXI:                                                    \
        if (reg_invoke_atm_dxi_classify_pak_and_fs(idb))                \
           goto rxexit;                                                 \
	break;								\
    default:								\
	goto srxread;							\
    }									\
									\
    /*									\
     * Sniff one long word to get protocol information			\
     */									\
srxread:								\
    regptr->argreg = idb->rxoffset;					\
    regptr->cmdreg = MCI_CMD_RX_SELECT;					\
    idb->sniff[SNIFFBASE+0].d.lword = regptr->readlong;			\
    sniffcode.d.lword = idb->sniff[SNIFFBASE+0].d.lword;		\
                                                                        \
    /* CSCdi41558, frames dropped as recognized as LEX in following code\
     * Can not put in the switch statement above as idb->phoney_rxbyte  \
     * is set which has 'goto srxread'                                  \
     */                                                                 \
    if ((idb->enctype == ET_STUN)  ||                                   \
        (idb->enctype == ET_SDLCP) ||                                   \
        (idb->enctype == ET_SDLCS))                                     \
        goto srx_process;                                               \
                                                                        \
    /*									\
     * Handle possible bridging functions.				\
     */									\
    if (idb->sniff[SNIFFBASE+0].d.sword[1] == TYPE_PPP_BRIDGE) { 	\
       summary = RXTYPE_TRANSIT;					\
       /* read in ppp bridge encaps */					\
       idb->sniff[SNIFFBASE+1].d.sword[0] = regptr->readshort;		\
       if (idb->sniff[SNIFFBASE+1].d.sword[0] != PPP_FIZC_MACTYPE) {	\
	   goto srx_process;						\
       }								\
       goto srxbridge;							\
    }									\
    if (idb->sniff[SNIFFBASE+0].d.lword == HDLC_BRIDGECODE) {		\
srxbridge:								\
        if (idb->tbridge_on_hwidb)					\
	    if (reg_invoke_tbridge_mci_forward(idb->fast_rcvidb))     	\
	        goto rxexit;		                               	\
        /*                                                              \
         * Get the type/length and sap (long) of this packet            \
         * for bridged packets only.  This is for                       \
         * hdlc_pakalign() to set up where pak->datagramstart           \
         * is supposed to point to.                                     \
         * We only need the type/length portion(not da and sa)          \
         * because we check for type or length.  If it is a             \
         * length, we need to check if it is snap encapsulated.         \
         *                                                              \
         *               byte1  byte2   byte3   byte4                   \
         *  SNIFFBASE+0  HDLC_BRIDGECODE(4 bytes)                       \
         *  SNIFFBASE+1  DA     DA      DA      DA                      \
         *  SNIFFBASE+2  DA     DA      SA      SA                      \
         *  SNIFFBASE+3  SA     SA      SA      SA                      \
         *  SNIFFBASE+4  type/len       (dsap)  (ssap)                  \
         *                                                              \
         *  rxoffset + (HDLC_ENCAPBYTES/2) + (IEEEWORDS * 2)	\
	 *  will point to sniff[SNIFFBASE+4].          			\
         */                                                             \
        regptr->argreg = idb->rxoffset + (HDLC_ENCAPBYTES/2) + 	\
			(IEEEWORDS * 2);                         	\
        regptr->cmdreg = MCI_CMD_RX_SELECT;                             \
        idb->sniff[SNIFFBASE+4].d.lword = regptr->readlong;             \
     } else if (idb->sniff[SNIFFBASE+0].d.sword[1] == TYPE_PPP_LEX) {	\
        /*								\
         * Switch to the lex idb.  Check if we are routing the 		\
	 * protocol, if not try and bridge.				\
         */								\
	idbtype *lex_swidb = idb->fast_rcvidb->lex_swidb;		\
	if (lex_swidb == NULL)	{					\
            regptr->cmdreg = MCI_CMD_RX_FLUSH;				\
	    goto rxexit;						\
	}								\
	/*								\
	 * lex_swidb->hwptr->rxtype is set in the lex_fs routine.	\
	 */								\
	if (BRIDGING_RXTYPE(lex_swidb->hwptr->fast_rcvidb,		\
			    lex_swidb->hwptr->rxtype)) {		\
	    if (lex_swidb->tbridge_on_swidb) {				\
		if (reg_invoke_tbridge_mci_forward(lex_swidb))		\
		    goto rxexit;					\
	    }								\
	}								\
    } else if (idb->sniff[SNIFFBASE+0].d.sword[1] == TYPE_DLSW) {	\
	if (reg_invoke_mci_dlsw_input(idb))                             \
	    goto rxexit;                                                \
    } else if ((ROUTING_RXTYPE(idb->fast_rcvidb, RXTYPE_DODIP)) &&	\
	(idb->sniff[SNIFFBASE+0].d.sword[1] == TYPE_IP10MB)) {		\
	idb->sniff[SNIFFBASE+1].d.sword[1] = regptr->readshort;		\
	idb->sniff[SNIFFBASE+1].d.lword = regptr->readlong;		\
	idb->sniff[SNIFFBASE+2].d.lword = regptr->readlong;		\
	if (idb->sniff[SNIFFBASE+2].d.byte[3] == FST_DLSW_PROT) {	\
	    if (reg_invoke_mci_dlsw_fst_forward(idb))			\
		goto rxexit;						\
	}								\
    }									\
srx_skip:								\
    /*									\
     * Check for fast RSRB.						\
     */									\
    if (idb->sniff[SNIFFBASE+0].d.sword[1] == TYPE_RSRB)		\
	if (reg_invoke_mci_vring_input(idb))				\
            goto rxexit;						\
									\
    /*									\
     * Check to see if this is a udp broadcast.  If so, try to		\
     * take the fast path.						\
     * Note: udp_fast_flood() relies on the rx pointer to be		\
     * positioned at the ip checksum.					\
     */									\
    if (ROUTING_RXTYPE(idb->fast_rcvidb, RXTYPE_DODIP))	{		\
	if (idb->sniff[SNIFFBASE+0].d.sword[1] == TYPE_IP10MB) {	\
	    idb->sniff[SNIFFBASE+1].d.sword[1] = regptr->readshort;	\
	    idb->sniff[SNIFFBASE+1].d.lword = regptr->readlong;		\
	    idb->sniff[SNIFFBASE+2].d.lword = regptr->readlong;		\
	    switch (idb->sniff[SNIFFBASE+2].d.byte[3]) {		\
	    case UDP_PROT:						\
		if (reg_invoke_udp_fast_flood(idb->fast_rcvidb))	\
		    goto rxexit;					\
		break;							\
	    case FST_RSRB_PROT:						\
		if (reg_invoke_mci_rsrb_fst_forward(idb))		\
		    goto rxexit;					\
		break;							\
	    case GRE_PROT:						\
	    case IPINIP_PROT:						\
		if (reg_invoke_tunnel_ip_hes_fs(idb))	                \
		    goto rxexit;					\
		break;							\
	    }								\
	}								\
    }									\
									\
srx_process:                                                            \
    /*									\
     * If we can't get a buffer, we have a congestion problem.          \
     * Do not disable the interface since that will flap DTR/RTS,       \
     * causing all sorts of misery.  Just flush the packet.             \
     * Use the sniffcode at this point rather than the sniff buffer     \
     * since the sniff buffer may have been over-written by the call    \
     * to bridge_receive().                                             \
     */									\
    pak = input_getbuffer(bytes, idb);					\
    if (!pak) {								\
        if (sniffcode.d.lword != HDLC_KEEPALIVES) {			\
            regptr->cmdreg = MCI_CMD_RX_FLUSH;                          \
	    goto rxexit;						\
        }								\
	                                                                \
        /*								\
         * Attempt to get a buffer to receive this keepalive.		\
         */								\
        pak = getbuffer(bytes);						\
	if (!pak) {							\
            regptr->cmdreg = MCI_CMD_RX_FLUSH;				\
	    goto rxexit;						\
        }								\
	pak->if_input = idb->fast_rcvidb;				\
    }									\
    /*									\
     * Do encapsulation classification, set up packet buffer.		\
     */									\
    iqueue = (*idb->parse_packet)(idb, pak, (uchar *)&idb->sniff);	\
    if (iqueue == NULL) {						\
	retbuffer(pak);							\
	cbus_mci_input_error(idb);					\
	regptr->cmdreg = MCI_CMD_RX_FLUSH;				\
	goto rxexit;							\
    }									\
    if (bytes > idb->max_pak_size) {					\
	idb->counters.giants++;						\
	retbuffer(pak);							\
	cbus_mci_input_error(idb);					\
	regptr->cmdreg = MCI_CMD_RX_FLUSH;				\
	goto rxexit;							\
    }									\
    if (idb->mci_checked) {						\
	pak->flags |= PAK_MCICHECKED;					\
	idb->mci_checked = FALSE;					\
    }									\
    if (idb->ipmfast_switched) {			       		\
	pak->flags |= PAK_MFS_SWITCHED;                                 \
	idb->ipmfast_switched = FALSE;					\
    }	                                                                \
									\
    /*									\
     * Set pointer to start of packet and read it into packet buffer.	\
     * Hand to raw input process and flush datagram in MCI RX buffer.	\
     */									\
    regptr->argreg = idb->rxoffset;					\
    regptr->cmdreg = MCI_CMD_RX_SELECT;					\
    mci2buffer(&regptr->readlong, pak->datagramstart, bytes);		\
    regptr->cmdreg = MCI_CMD_RX_FLUSH;					\
    pak->datagramsize = bytes;						\
    idb->counters.rx_cumbytes += bytes;					\
    idb->counters.inputs++;						\
    (*iqueue)(idb, pak);

/*
 * We crossed an error threshold, so disable
 * the interface and let line protocol restart us.
 */
void mci_input_error (hwidbtype *idb)
{
    mciregtype *regptr;

    regptr = ((mcitype *) idb->devctl)->regptr;
    regptr->argreg = idb->mci_index;
    regptr->cmdreg = MCI_CMD_SELECT;
    regptr->argreg = MCI_IF_DISABLE;
    regptr->cmdreg = MCI_CMD_STATUS;
    idb->status |= IDB_DISABLED;
    errmsg(&msgsym(INPUTERR, MCI), idb->hw_namestring);
}

/*
 * mci_interrupt
 * Handle an MCI controller interrupt
 * This interrupt routine and its major subroutines have been heavily
 * optimized for speed.  Do not add permanent debugging code or subroutines
 * or much of anything else.  Speedups must be debugged with an oscilloscope.
 * A detailed knowledge of the MCI microcode structure is strongly advised.
 */
static void mci_interrupt (void)
{
    mcitype *mci;
    mciregtype *regptr;
    hwidbtype *idb;
    paktype *pak;
    ushort summary, bytes, status, usindex, temp;
    ulong ulindex, erxtype;
    void (*iqueue)(hwidbtype *, paktype *);

    for (mci = (mcitype *) mciQ.qhead; mci; mci = mci->next) {
	regptr = mci->regptr;
	while (TRUE) {

	    /*
	     * Process receive queue until empty.
	     * MCI_CMD_RX_SETUP clears the interrupt request.
  	     */
	    while (TRUE) {

		/*
		 * Come here if exiting early from a receive routine.
		 */
rxexit:
		regptr->cmdreg = MCI_CMD_RX_SETUP;
		usindex = regptr->argreg;
		if (regptr->cmdreg != MCI_RSP_OKAY) 
		    goto mci_setup_error;
		if ((short) usindex == -1)	/* No more RX buffers */
		    break;
		if (usindex >= MCI_MAXUNITS)
		    goto mci_setup_error;
		idb = mci->interface_list[usindex];

		/*
		 * If the scheduler has not run recently -- discard datagram.
		 */
		if (sched_run_interval)
		    goto mci_check_sched;

	    return_mci_check_sched:
		bytes = regptr->rxlength;
		idb->rxlength = bytes;
		idb->rxrifwordlen = 0;
		idb->rxtype = regptr->rxtype;

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
		if (tbridge_debug_pak_0) {
		    buginf("\nTB0: MCI %04x %s on %s %d bytes",
			   regptr->rxstatus,
			   rxtype_string(idb->rxtype),
			   idb->fast_rcvidb->namestring,
			   bytes);
		}
#endif	/* TBRIDGE_DEBUG */

#ifdef PERPACKETDEBUG
		if (cbus_debug) {
		    status = regptr->rxstatus;
		    summary = idb->rxtype;
		    buginf("\n%c%d: stat %04x, len %04x, raw len: %04x, type %04x, srb .... -  mci_interrupt()",
			   *idb->name, idb->unit, status, idb->rxlength, 
			   regptr->rxlength, summary, regptr->rxsrbinfo);
		}
#endif
		if (idb->status & IDB_ETHER) {
		    MCI_ETHER_RECEIVE
		} else if (idb->status & IDB_SERIAL) {
		    MCI_SERIAL_RECEIVE
		} else
		     errmsg(&msgsym(RXINDEX, MCI), mci->unit, usindex);
	    }

	    /*
	     * Read the attention summary register for this controller.
	     * Bit 0 will be set if interface 0 has special conditions, etc.
	     */
	    regptr->cmdreg = MCI_CMD_ASR;
	    summary = regptr->argreg;
	    if (summary == 0)
		break;

	    /*
	     * Process transmit done, interface up/down, and error counters
	     */
	    for (ulindex = 0; (summary != 0) && (ulindex < MCI_MAXUNITS);
		 ulindex++) {
		status = bitmask[ulindex];
		if (summary & status) {
		    summary &= ~status;
		    regptr->argreg = (ushort) ulindex;
		    regptr->cmdreg = MCI_CMD_CONDITIONS;
		    status = regptr->argreg;
		    if (status == 0)
			continue;

		    /*
		     * Host generated transmissions done. If there are
		     * packets waiting to be transmitted, call the
		     * output routine for this interface.  If we are
		     * running with old microcode, ensure that the TX0
		     * window is free.
		     */
			idb = mci->interface_list[ulindex];
			if (idb->output_qcount) {
			    if (!mci->tx1bug)
				cbus_mci_output(idb);
			    else if (!mci->critical) {
				mci_cbus_intflag = FALSE;
				cbus_mci_output(idb);
				mci_cbus_intflag = TRUE;
			    }
			}

		    /*
		     *  Check other special conditions	
		     */
		    if (status & (MCI_COND_ERROR|MCI_COND_DCD)) {
			idb = mci->interface_list[ulindex];
			
			/*
			 * Read error counters.  If serial interface, reset
			 * since the input resource error often comes about
			 * because of a hung USART.
		         */
			if (status & MCI_COND_ERROR) {
			    ERRORS(MCI_ERR_GIANTS, idb->counters.giants);
			    ERRORS(MCI_ERR_NOBUFFERS, idb->counters.input_resource);
			    ERRORS(MCI_ERR_ACQUIRE, idb->counters.output_error);
			    ERRORS(MCI_ERR_COLLISIONS, idb->counters.output_collision);
			}

			/*
		         * Handle interface up/down transitions
		 	 */
			if (status & MCI_COND_DCD)
			    cbus_mci_carrier(idb);
		    }
		}
	    }
	}
    }
    return;

 mci_setup_error:
    regptr = mci->regptr;
    errmsg(&msgsym(SETUPERR, MCI), mci->unit, regptr->cmdreg, 
	   regptr->argreg);
    (void) mci_init(mci, mci->critical);
    cbus_restarts_error++;
    return;

 mci_check_sched:
    if (AWAKE(sched_run_time)) {
	cbus_mci_throttle(idb);
	goto rxexit;
    }
    goto return_mci_check_sched;

}

/*
 * mci_analyze
 * Discover and initialize the MCI controllers
 */
int mci_analyze (void)
{
    int i;
    uchar *mcibase;
    mcitype *mci;
    hwidbtype *idb;
    int controllers;
    char *tptr;

    controllers = 0;
    mci_eth = mci_ser = 0;
    queue_init(&mciQ, 0L);
    queue_init(&cbusQ, 0L);
    mcibase = (uchar *) MCISTART;
    for (i = 0; i < MCI_MAXCTRLRS; i++, mcibase += MCIBYTES) {
	if (touch_device(mcibase)) {
	    if (!mci_controller((mciregtype *)mcibase))
		continue;
	    
	    /*
	     * If we have MCI controllers, determine the unit numbers for the
	     * first MCI Ethernet and Serial interfaces.  We assume that all 
	     * other such hardware has already been discovered.  We generate
	     * MCI interface numbers by pre-incrementing mci_base_ether and
	     * mci_base_serial. While we're here, create interrupt stack.
	     */
	    if (controllers == 0) {
		createlevel(MCI_INTLEVEL, mci_interrupt,
			    "Multiport Communications Interfaces");
		cbus_mci_base_ether = -1;
		cbus_mci_base_serial = -1;
		FOR_ALL_HWIDBS(idb) {
		    if (idb->status & IDB_ETHER) {
			if (idb->unit > cbus_mci_base_ether)
			    cbus_mci_base_ether = idb->unit;
		    }
		    if (idb->status & IDB_SERIAL) {
			if (idb->unit > cbus_mci_base_serial)
			    cbus_mci_base_serial = idb->unit;
		    }
		}
	    }

	    /*
	     * If controller exists, begin initialization.
	     */
	    mci = malloc_named(sizeof(mcitype), "MCI Structure");
	    if (!mci) {
	        printf(nomemory);
	        return(controllers);
	    }
	    controllers++;
	    mci->unit = i;
	    mci->regptr = (mciregtype *) mcibase;

	    /*
	     * First run controller through basic bus and memory tests.
	     * Initialize controller hardware, create interface descriptors.
	     */
	    tptr = cbus_mci_memtest(mci);
	    if (tptr) {
		errmsg(&msgsym(TESTFAIL, MCI), mci->unit, tptr);
		continue;
	    }
	    if (mci_init(mci, FALSE) == FALSE) {
		free(mci);
		continue;
	    }
	    
	    /*
	     * Controller initialized.
	     */
	    if (mci->interface_count) {
	        enqueue(&mciQ, mci);
		mci->restartdeferred = TRUE;
	    }
	}
    }

    add_mci_cards();

    /*
     * Initialize parser chains
     */
    mci_parser_init();

    return(controllers);
}


/*
 * mci_memx_select
 * Select a memx address
 */
void mci_memx_select (mciregtype *regptr, ulong address)
{
    regptr->argreg = (address >> 16);
    regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
    regptr->argreg = (address & 0xFFFF);
    regptr->cmdreg = MCI_CMD_MEMX_SELECT_LOW;
}

/*
 * mci_warning
 * Issue any warnings about obsolete firmware.
 * Software version 1.4 of MCI 1.0 hardware doesn't know about AppleTalk
 * or Novell packets.  Issue warning if someone turns on bridging when
 * routing these two protocols.
 */

void mci_warning (void)
{
    boolean foundit;
    mcitype *mci;
    idbtype *idb;
    
    /*
     * If the bridging option is not present, just return now.
     */
    if (!bridge_enable)
	return;

#ifdef C7000
    /* No problem on Cisco 7000 */
    return;
#endif

    /*
     * Search for obsolete MCI firmware.  Quit if it is all new.
     */
    foundit = FALSE;
    for (mci = (mcitype *) mciQ.qhead; mci; mci = mci->next) {
	if ((mci->hw_version == 0x100) && (mci->sw_version <= 0x104))
	    foundit = TRUE;
    }
    if (!foundit)
	return;
    
    /*
     * Check if someone has actually enabled bridging.  Quit if no bridging.
     */
    FOR_ALL_SWIDBS(idb) {
	if (RUNNING_IF(LINK_BRIDGE, idb)) {
	    /*
	     * We can bridge, we are bridging, and we have obsolete firmware.
	     * If we are routing the protocol on this interface then we've
	     * got a problem since the old microcode doesn't know about
	     * AppleTalk and Novell.  Note that we are not forbidding, just
	     * warning.
	     */
	    if (ROUTING_IF(idb, LINK_APPLETALK))
		errmsg(&msgsym(OBSOLETE, MCI), "Appletalk");
	    if (ROUTING_IF(idb, LINK_NOVELL))
		errmsg(&msgsym(OBSOLETE, MCI), "Novell");
	}
    }
}

/*
 * show_mci
 * Display current statistics for MCI controllers.
 */
void show_mci (parseinfo *csb)
{
    mcitype *mci;
    hwidbtype *idb;
    int i;
    
    automore_enable(NULL);
    for (mci = (mcitype *) mciQ.qhead; mci; mci = mci->next) {
#ifdef C7000
	printf("\nSIP %d, hardware version %d.%d, microcode version %d.%d",
	       mci->unit, (mci->hw_version >> 8), (mci->hw_version & 0xFF),
	       (mci->sw_version >> 8),	(mci->sw_version & 0xFF));
#else
	printf("\nMCI %d, controller type %d.%d, microcode version %d.%d",
	       mci->unit, (mci->hw_version >> 8), (mci->hw_version & 0xFF),
	       (mci->sw_version >> 8),	(mci->sw_version & 0xFF));
#endif
	printf("\n  %d Kbytes of main memory, %d Kbytes cache memory",
	       (mci->memd_size+1)/512,  (mci->mema_size+1)/512);
	printf("\n  %d system TX buffers, largest buffer size %d",
	       mci->txbuffers, mci->largest_buffer);
	printf("\n  Restarts: %d line down, %d hung output, %d controller error",
	       cbus_restarts_lineup, cbus_restarts_output,
	       cbus_restarts_error);
	for (i = 0; i < MCI_MAXUNITS; i++) {
	    idb = mci->interface_list[i];
	    if (idb == NULL)
		continue;
	    printf("\n  Interface %d is %s", i, idb->hw_namestring);
	    if (idb->status & IDB_ETHER)
		printf(", station address %e (bia %e)", idb->hardware,
		       idb->bia);
	    if (idb->status & IDB_SERIAL)
		printf(", electrical interface is %s", idb->applique_name);
	    printf("\n    %d total RX buffers, %d buffer TX queue limit",
		   mci->rxbuffers[i], idb->mci_txcount);
	    printf(", buffer size %d", idb->buffersize);
	    if (idb->txqlength)
		printf("\n    Tx buffer in-use count is %d", 
		       (*idb->txqlength)(idb));
	    printf("\n    Transmitter delay is %d microseconds",
				idb->xmitdelay);
	    if (idb->status & IDB_SERIAL) {
		serialsb *ssb;
		ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);
		printf("\n    %s speed synchronous serial interface",
		(ssb->serial_protocol & MCI_CAP_SPEED) ? "High" : "Normal");
	    }
	}
	
	/*
	 * If there are more controllers, print a --More-- prompt.
         */
	if (mci->next) {
	    automore_conditional(0);
	}
    }
    automore_disable();
}

/***************************************************************************
 *
 *                MC,MC+ MCI Access Routines
 *
 ***************************************************************************/
/*
 * mci_mc_read_short
 * Read one sixteen bit value from the MC card at the specified address:
 */
ushort mci_mc_read_short (hwidbtype *idb, uint MC_addr)
{
    mcitype *mci;
    ushort value;
    leveltype SR;
    
    mci = idb->devctl;
    SR = raise_interrupt_level(ETHER_DISABLE);
    
    mci_memx_select(mci->regptr, MC_addr); /* starting MC address */
    value = *((volatile ushort *)&mci->regptr->memxreg);
    reset_interrupt_level(SR);
    
    return(value);
}

/*
 * mci_mc_write_short
 * Write one sixteen bit value to the MC card at the specified address:
 */

void mci_mc_write_short (hwidbtype *idb, ushort value, uint MC_addr)
{
    mcitype *mci;
    leveltype SR;
    
    mci = idb->devctl;
    SR = raise_interrupt_level(ETHER_DISABLE);

    mci_memx_select(mci->regptr, MC_addr); /* starting MC address */
    *((volatile ushort *)&mci->regptr->memxreg) = value;
    reset_interrupt_level(SR);
}

/*
 * mci_mc_read_long
 * Read one 32 bit value from the MC card at the specified address:
 */

uint mci_mc_read_long (hwidbtype *idb, uint MC_addr)
{
    mcitype *mci;
    charlong value;
    leveltype SR;
    
    mci = idb->devctl;
    SR = raise_interrupt_level(ETHER_DISABLE);

    mci_memx_select(mci->regptr, MC_addr); /* starting MC address */
    value = mci->regptr->memxreg;
    reset_interrupt_level(SR);
    return((uint)value.d.lword);
}


/*
 * mci_mc_write_long
 * Write one 32 bit value from the MC card at the specified address:
 */

void mci_mc_write_long (hwidbtype *idb, uint data, uint MC_addr)
{
    mcitype *mci;
    charlong value;
    leveltype SR;
    
    mci = idb->devctl;
    SR = raise_interrupt_level(ETHER_DISABLE);
    
    value.d.lword = data;
    mci_memx_select(mci->regptr, MC_addr); /* starting MC address */
    mci->regptr->memxreg = value;
    reset_interrupt_level(SR);
}

/*
 * mci_mc_read_buffer
 * Copy a memory image from the MC or MC+ card into RAM.
 * Return number of bytes read.
 */

long mci_mc_read_buffer (
    hwidbtype *idb,
    uint MC_addr,		/* Address on MC card */
    uchar *buffer,		/* Buffer address */
    long size)			/* Size of image to read (bytes) */
{
    mcitype *mci;
    long count = 0L;
    leveltype SR;
    uint data;
    
    mci = idb->devctl;
    if (mc_debug)
	ttyprintf(console,
		  "\nmc_read: MC_addr=0x%06x, buffer=0x%08x, size=%d\n", 
		  MC_addr, buffer, size);
    
    SR = raise_interrupt_level(ETHER_DISABLE);
    
    mci_memx_select(mci->regptr, MC_addr); /* Starting MC address */
    if (buffer) {
	/*
	 * Read data into buffer:
	 */
	mci2buffer(&mci->regptr->memxreg, buffer, size);
    } else {
	/*
	 * No buffer. Read and discard data (for debugging):
	 */
	for (count = 0; count < (size/2); count++) {
	    data = *((volatile uint *)&mci->regptr->memxreg);
	    if (mc_debug)
		ttyprintf(console, "%06x : %08x\n", 
			  (MC_addr + (count * 4)), data);
	}
    }
    
    reset_interrupt_level(SR);
    return(count);
}

/*
 * mci_mc_write_buffer
 * Copy a memory image from RAM to the MC or MC+ card.
 * Return number of bytes written.
 */

long mci_mc_write_buffer (
    hwidbtype *idb,
    void *buffer,		/* Buffer address */
    uint MC_addr,		/* Address on MC card */
    long size)			/* Size of image (bytes) */
{
    mcitype *mci;
    long count = 0L;
    leveltype SR;
    
    mci = idb->devctl;
    if (mc_debug)
	ttyprintf(console, 
		  "\nmc_write: buffer=0x%08x, MC_addr=0x%06x, size=%d\n", 
		  buffer, MC_addr, size);
    
    
    SR = raise_interrupt_level(ETHER_DISABLE);
    
    mci_memx_select(mci->regptr, MC_addr);
    buffer2mci(buffer, &mci->regptr->memxreg, size);
    
    reset_interrupt_level(SR);
    return(count);
}


static void mci_subsys_init (subsystype *subsys)
{
    reg_add_tbridge_fddi_transit_receive(
    		(service_tbridge_fddi_transit_receive_type)return_false,
					 "return_false");
    reg_add_media_set_rxoffset(IDBTYPE_MCISERIAL, mci_set_rxoffset,
				"mci_set_rxoffset");
    nMCI = mci_analyze();		/* MCI interfaces */
}


/*
 * MCI subsystem header
 */

#define MCI_MAJVERSION   1
#define MCI_MINVERSION   0
#define MCI_EDITVERSION  1

SUBSYS_HEADER(mci,
	      MCI_MAJVERSION, MCI_MINVERSION, MCI_EDITVERSION,
	      mci_subsys_init, SUBSYS_CLASS_DRIVER,
	      "seq: dbus, cbus_mci",
	      "req: cbus_mci");

