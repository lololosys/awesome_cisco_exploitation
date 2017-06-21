/* $Id: serial_autodetect.c,v 3.1.2.2 1996/08/28 13:22:38 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/serial_autodetect.c,v $
 *------------------------------------------------------------------
 * serial_autodetect.c  Autodetect serial encapsulations on wire
 *
 * July 1996, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: serial_autodetect.c,v $
 * Revision 3.1.2.2  1996/08/28  13:22:38  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.2.1  1996/07/28  07:33:18  irfan
 * CSCdi60568:  v120 autodetect not working
 * Branch: California_branch
 * allow serial interfaces to autodetect encaps on the wire for incoming
 * connections.
 *
 * Revision 3.1  1996/07/28  07:22:11  irfan
 * files needed for serial autodetect.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "interface_private.h"
#include "subsys.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "mgd_timers.h"
#include "address.h"
#include "logger.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "../ui/debug.h"
#include "../if/network.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../wan/serial_debug.h"
#include "../wan/serial_autodetect.h"
#include "../wan/dialer.h"
#include "../les/isdn_bri.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

/*
 * Parser Chains and definitions
 */
#define ALTERNATE	NONE
#include "../wan/cfg_int_autodetect.h"
LINK_POINT(autodetect_commands, ALTERNATE);
#undef ALTERNATE


/*
 * local vars
 */
static pid_t serial_autodetect_fork = 0;         /* Our process id */
static watched_queue *serial_autodetect_inQ;     /* Our inQ */
static mgd_timer serial_autodetect_timer;	 /* Our timer */

/*
 * table of autodetect info per encapsulation.
 *
 * To add a new encaps to the autodetect
 *
 *     add a struct to the following table with
 *                        ET_xx (encaps type)
 *                        AUTODETECT_PARSE_xx (in serial_autodetect.h)
 *                        name (a user freindly name for parse chains)
 *                        NULL, NULL (for test and switch vectors)
 *
 *  a call to reg_invoke_serial_autodetect_register_encaps with
 *  the correct vectors from the encaps code will complete the job,
 *  once the appropriate parse chains have been added.
 *  DO REMEMBER to change MAX_AUTODETECT_ENC_TYPES
 *
 */
static autodetect_type autodetect_enc_table[MAX_AUTODETECT_ENC_TYPES] = { 
    {ET_PPP, AUTODETECT_PARSE_PPP, "ppp", NULL, NULL},
    {ET_V120, AUTODETECT_PARSE_V120, "v120", NULL, NULL},
    {ET_FRAME_RELAY, AUTODETECT_PARSE_FR, "frame-relay", NULL, NULL},
    {ET_HDLC, AUTODETECT_PARSE_HDLC, "hdlc", NULL, NULL},
    {ET_X25, AUTODETECT_PARSE_X25, "x25", NULL, NULL},
    {ET_SMDS, AUTODETECT_PARSE_SMDS, "smds", NULL, NULL}
};



/*
 * create_ser_autodetect_info
 *
 * return the serial autodetect info block for the
 * given hwidb. create the block, if missing.
 */
static ser_autodetect_info
   *create_ser_autodetect_info (hwidbtype *hwidb)

{
    serialsb *ssb;

    ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
    if (ssb) {
	if (!ssb->ser_autodetect) {
	    ssb->ser_autodetect = malloc_named(sizeof(ser_autodetect_info),
					       "SA'Detect");
	}
	return (ssb->ser_autodetect);
    }
    return (NULL);
}

/*
 * serial_autodetect_needed
 *
 * see if we have already switched encaps through autodetection.
 * or other mechanisms like all-incoming-calls-v120 etc. etc.
 *
 * Currently we only test of already switched encaps or already
 * autodetected encaps.
 */
static boolean serial_autodetect_needed (hwidbtype *hwidb)
{

    ser_autodetect_info *autodetect_info;

    autodetect_info = get_ser_autodetect_info(hwidb);

    /*
     * This might be a channel in a permanently nailed
     * mode, or might have been already autodetected,
     * whereby we don't want autodetection to be run
     * on it.
     */
    if (hwidb->enctype != hwidb->enctype_cfg) {
	/*
	 * We already switched modes
	 */
	return (FALSE);
    }
    return (TRUE);
}

/*
 * serial_autodetect_iqueue
 *
 * send the packet to our process level
 */
static void serial_autodetect_iqueue (hwidbtype *hwidb, paktype *pak)
{

    /*
     * we will queue the packet for the
     * autodetect process to pick up.
     */
    process_enqueue(serial_autodetect_inQ, pak);
}

/*
 * serial_autodetect_parse_packet
 *
 * do nothing here, wait for the packet to come in at
 * our iqueue vector.
 */
static iqueue_t serial_autodetect_parse_packet (hwidbtype *hwidb,
						paktype *pak, uchar *data)
{

    /*
     * we don't know which one of the individual
     * encaps will recognize this packet. So we
     * can not correctly parse the packet.
     * We will just return the serial_autodetect_iqueue
     */
    if (data) {
	pak->datagramstart = data;
    }
    return (serial_autodetect_iqueue);
}

/*
 * serial_autodetect_remove
 *
 * OK put the old encaps information back to the interface
 */
static void serial_autodetect_remove (hwidbtype *hwidb)
{

    leveltype status;
    serialsb *ssb;
    ser_autodetect_info *autodetect_info;

    ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);

    status = raise_interrupt_level(NETS_DISABLE);
    if ((!ssb) || (NULL == (autodetect_info = ssb->ser_autodetect))) {
	reset_interrupt_level(status);
	return;
    }
    ssb->ser_autodetect = NULL;          /* remove references to struct   */
    hwidb->iqueue = autodetect_info->save_iqueue;  /* reset hwidb vectors */
    hwidb->parse_packet = autodetect_info->save_parse_packet;
    hwidb->enctype = autodetect_info->saved_enctype;
    hwidb->linestate = autodetect_info->saved_linestate;
    hwidb->reset = autodetect_info->saved_reset;
    reset_interrupt_level(status);

    mgd_timer_stop(&autodetect_info->fail_timer);  /* stop the timer   */
    free(autodetect_info);              /* free the autodetect struct */
}

/*
 * serial_autodetect_reset
 *
 * called when an interface should be brought down.
 * autodetect uses it to relinquish control.
 */
static inline void serial_autodetect_reset (hwidbtype *hwidb)
{

    if (serial_debug) {
	buginf("\nSer-Autodetect %s: interface reset while"
	       " autodetecting", hwidb->hw_short_namestring);
    }
    /*
     * restoring the encaps will tell the
     * input process to skip this hwidb
     */
    serial_autodetect_remove(hwidb);
    if (hwidb->reset) {
	(*hwidb->reset)(hwidb);
    }
}

/*
 * serial_autodetect_setup_vectors
 *
 * takeover this interface by rewriting the various
 * critical vectors in the hwidb
 */
static boolean serial_autodetect_setup_vectors (hwidbtype *hwidb)
{
    leveltype status;
    ser_autodetect_info *autodetect_info = NULL;
    hwidbtype *cfg_hwidb;
    ulong netcfg_config;

    /*
     * test for configuration in the netcfg_idb
     * complain if we don't have any autodetect
     * configuration.
     *
     * for ISDN interfaces, we will first check the D channel
     * config, failing which, we will use the rotary group
     * config.
     */
    if (is_isdn(hwidb)) {
	cfg_hwidb = hwidb->isdn_info->d_chan_idb;
	autodetect_info = get_ser_autodetect_info(cfg_hwidb);
    }

    if (NULL == autodetect_info) {
	cfg_hwidb = get_netidb(hwidb);
	autodetect_info = get_ser_autodetect_info(cfg_hwidb);
    }

    if ((!autodetect_info) || (!autodetect_info->allowed_encaps)) {
	if (serial_debug) {
	    buginf("\nSer-Autodetect %s: no autodetect configuration",
		       hwidb->hw_short_namestring);
	}
	return (FALSE);
    }

    netcfg_config = autodetect_info->allowed_encaps;

    autodetect_info = create_ser_autodetect_info(hwidb);
    if (!autodetect_info) {
	    return (FALSE);
    }

    status = raise_interrupt_level(NETS_DISABLE);
    autodetect_info->save_iqueue = hwidb->iqueue;
    autodetect_info->save_parse_packet = hwidb->parse_packet;
    autodetect_info->saved_enctype = hwidb->enctype;
    autodetect_info->saved_linestate = hwidb->linestate;
    autodetect_info->saved_reset = hwidb->reset;
    autodetect_info->allowed_encaps = netcfg_config; /* pass config down */

    hwidb->iqueue = serial_autodetect_iqueue;
    hwidb->parse_packet = serial_autodetect_parse_packet;
    hwidb->enctype = ET_SERIAL_AUTODETECT;
    hwidb->linestate = (linestate_t) return_true;
    hwidb->reset = serial_autodetect_reset;
    reset_interrupt_level(status);
    autodetect_info->pak_count = 0;
    mgd_timer_init_leaf(&autodetect_info->fail_timer,
			&serial_autodetect_timer, 0, hwidb, FALSE);
    mgd_timer_start(&autodetect_info->fail_timer, SERIAL_AUTODETECT_MAX_TIME);
    if (serial_debug) {
	buginf("\nSer-Autodetect %s: starting",
	       hwidb->hw_short_namestring);
    }
    return (TRUE);
}

/*
 * serial_autodetect_start
 *
 * we have a serial interface coming up. Let Autodetect
 * takeover this interface.
 */
static boolean serial_autodetect_start (hwidbtype *hwidb)
{
    if (hwidb && serial_autodetect_needed(hwidb)) {
	return (serial_autodetect_setup_vectors(hwidb));
    }
    return (FALSE);
}

/*
 * autodetect_check_and_set_encaps
 *
 * this where we test individual encaps, provided the config
 * allows it. If the encaps is detected, it will attempt to
 * switch the line/channel to the detected encaps.
 */
static inline boolean autodetect_check_and_set_encaps (hwidbtype *hwidb,
						       paktype *pak,
						       boolean *hangup)
{

    int ix;
    ser_autodetect_info *autodetect_info;
    autodetect_type *encaps_autodetect_entry;

    autodetect_info = get_ser_autodetect_info(hwidb);

    if (!autodetect_info) {
	serial_autodetect_remove(hwidb);
	*hangup = TRUE;
	return (FALSE);
    }

    *hangup = FALSE;

    for (ix = 0; ix < MAX_AUTODETECT_ENC_TYPES; ix++) {

	encaps_autodetect_entry = &autodetect_enc_table[ix];

	/*
	 * skip this entry if it seems invalid
	 */
	if ((!encaps_autodetect_entry->test_this_encaps_pak) ||
	    (!encaps_autodetect_entry->switch_to_this_encaps)) {
	    continue;
	}

	/*
	 * see if this encaps test routine recognizes the packet ?
	 */
	if ((encaps_autodetect_entry->test_this_encaps_pak)(hwidb,
							     pak)) {

	    /*
	     * if it is not an allowed encaps, complain.
	     */
	    if (!(autodetect_info->allowed_encaps &
		  encaps_autodetect_entry->parse_bit)) {

		if (serial_debug) {
		    buginf("\nSer-Autodetect %s: Autodetect not configured"
			   " for %s encaps",
			   hwidb->hw_short_namestring,
			   encaps_autodetect_entry->name);
		}
		/*
		 * treat this just like failed detection.
		 * this will let them try the next encaps/packet.
		 */
		continue;
	    }

	    /*
	     * we have autodetected a new-different encaps
	     */
	    if (serial_debug) {
		buginf("\nSer-Autodetect %s: Autodetected %s encaps",
		       hwidb->hw_short_namestring,
		       encaps_autodetect_entry->name);
	    }

	    /*
	     * Restore the correct vectors, for possible saving
	     * by switch encaps function.
	     */
	    serial_autodetect_remove(hwidb);

	    /*
	     * don't bother switching if it's the default
	     */
	    if (autodetect_info->saved_enctype ==
		encaps_autodetect_entry->enctype) {

		return (TRUE);
	    }

	    /*
	     *
	     * ask the encapsulation to activate itself
	     */
	    if (!((encaps_autodetect_entry->switch_to_this_encaps)
		  (hwidb))) {
		if (serial_debug) {
		    buginf("\nSer-Autodetect %s: could not"
			   " switch encaps", hwidb->hw_short_namestring);
		}
		*hangup = TRUE;
		return (FALSE);
	    }
	    return (TRUE);

	}   /* if detected encaps */
    }       /* for all encaps */

    /*
     * it was an autodetection failure.
     */
    if (ix >= MAX_AUTODETECT_ENC_TYPES) {
	autodetect_info->pak_count++;
	if (serial_debug) {
	    buginf("\nSer-Autodetect %s: Failed attempt %d",
		   hwidb->hw_short_namestring, autodetect_info->pak_count);
	}
	if (autodetect_info->pak_count >= SERIAL_AUTODETECT_MAX_ATTEMPTS) {
	    serial_autodetect_remove(hwidb);
	    if (serial_debug) {
		buginf(" -  Restoring default encaps: %s",
		       encstring(hwidb->enctype));
	    }
	    return (TRUE);
	}
    }
    return (FALSE);
}

/*
 * serial_autodetect_input
 *
 * go through the queue of packets on hwidb's that need
 * to be autodetected.
 */
static void serial_autodetect_input (void)
{

    int count;
    paktype *pak;
    hwidbtype *hwidb;
    boolean hangup;


    count = PAK_SWITCHCOUNT;
    while (count-- > 0) {
	pak = process_dequeue(serial_autodetect_inQ);
	if (!pak) {
	    break;
	}
	hwidb = (pak->if_input) ? pak->if_input->hwptr : NULL;
	if ((!hwidb) || (!get_ser_autodetect_info(hwidb)) ||
	    (ET_SERIAL_AUTODETECT != hwidb->enctype)) {

	    datagram_done(pak);
	    continue;
	}

	/*
	 * if any autodetection sampler finds the packet
	 * interesting, it will activate itself.
	 * Note the autodetected modes should be able to
	 * flexibly activate and de-activate while saving
	 * and restoring the various hwidb->vectors.
	 */

	if (autodetect_check_and_set_encaps(hwidb, pak, &hangup)) {

	    /*
	     * tell the hw/media that we have a new
	     * encaps on the wire.
	     */
	    if (hwidb->lineaction) {
		(*(hwidb->lineaction))(hwidb);
	    }
	    /*
	     * The packet is now fed again through the input
	     * path for new/default encapsulation.
	     */
	    serial_queue_for_process(hwidb, pak);

	} else {

	    datagram_done(pak);
	    if (hangup) {
		if (hwidb->reset) {
		    (*hwidb->reset)(hwidb);
		}
	    }
	}       /* if (autodetect_check_and_set_encaps()) */
    }           /* while (pak.count) */
}

/*
 * serial_autodetect_timers
 *
 * Run this when a timer expiration happens.  Time out
 * interfaces that have not completed autodetection in the 
 */

static void serial_autodetect_timers (void)
{
    mgd_timer *expired_timer;
    hwidbtype *hwidb;
    ser_autodetect_info *autodetect_info;


    while (mgd_timer_expired(&serial_autodetect_timer)) {

	expired_timer = mgd_timer_first_expired(&serial_autodetect_timer);
	hwidb = mgd_timer_context(expired_timer);
	autodetect_info = get_ser_autodetect_info(hwidb);
	mgd_timer_stop(expired_timer);
	if ((!autodetect_info) || (ET_SERIAL_AUTODETECT != hwidb->enctype)) {
	    if (serial_debug) {
		buginf("\nSer-Autodetect %s: bad timer expiration",
		       hwidb->hw_short_namestring);
	    }
	    continue;
	}
	if (serial_debug) {
	    buginf("\nSer-Autodetect %s: timed-out",
		   hwidb->hw_short_namestring);
	}
	/*
	 * reset the interface and pass it back to
	 * whoever it belongs to.
	 */
	serial_autodetect_reset(hwidb);

    }       /* while */
}

/*
 * serial_autodetect
 *
 * main process for serial_autodetect subsystem
 * initialize incoming packet queue and run the autodetect
 * process loop
 */
static process serial_autodetect (void)
{
    ulong major, minor;

    process_watch_queue(serial_autodetect_inQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&serial_autodetect_timer, ENABLE);

    while (TRUE) {
	/*
	 * Wait for incoming packet
	 */
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {

	      case QUEUE_EVENT:
		serial_autodetect_input();
		break;

	      case TIMER_EVENT:
		serial_autodetect_timers();
		break;

	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}


/*
 * Our UI stuff
 */
static parser_extension_request autodetect_chain_init_table[] = {
    { PARSE_ADD_CFG_INT_CMD, &pname(autodetect_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * autodetect_command
 *
 * Process the  autodetect interface subcommand
 */
void autodetect_command (parseinfo *csb)
{
    hwidbtype *hwidb;
    ser_autodetect_info *autodetect_info;
    uint ix, enc_parse_bit;

    hwidb = hwidb_or_null(csb->interface);
    if (!hwidb) {
	return;
    }

    if (csb->nvgen) {

	autodetect_info = get_ser_autodetect_info(hwidb);
	if (autodetect_info && autodetect_info->allowed_encaps) {

	    nv_write(TRUE, "%s", csb->nv_command);

	    for (ix = 0; ix < MAX_AUTODETECT_ENC_TYPES; ix++) {
		    nv_add((autodetect_info->allowed_encaps &
			    autodetect_enc_table[ix].parse_bit),
			   " %s", autodetect_enc_table[ix].name);
		}
	}
	return;
    }

    /*
     * !csb->nvgen
     */

    autodetect_info = create_ser_autodetect_info(hwidb);
    if (!autodetect_info) {
	return;
    }

    if (!(csb->sense || GETOBJ(int,1))) {
	autodetect_info->allowed_encaps = 0;
	return;
    }

    for (ix = 0; ix < MAX_AUTODETECT_ENC_TYPES; ix++) {

	enc_parse_bit = autodetect_enc_table[ix].parse_bit;

	if (GETOBJ(int,1) & enc_parse_bit) {
	    if (csb->sense) {
		autodetect_info->allowed_encaps |= enc_parse_bit;
	    } else {
		autodetect_info->allowed_encaps &= ~enc_parse_bit;
	    }
	}
    }
}

/*
 * serial_autodetect_reg_encaps
 *
 * register the test and switch vectors for
 * encapsulations. Note, we must have this
 * encaps in our hard-coded table, already.
 */
static void serial_autodetect_reg_encaps (int encap,
					  void *test_encap,
					  void *switch_encap)
{
    int ix;

    for (ix = 0; ix < MAX_AUTODETECT_ENC_TYPES; ix++) {
	if (autodetect_enc_table[ix].enctype == encap) {
	    autodetect_enc_table[ix].test_this_encaps_pak = test_encap;
	    autodetect_enc_table[ix].switch_to_this_encaps = switch_encap;
	}
    }
}

/*
 * serial_autodetect_init
 *
 * initializations for serial_autodetect subsystem
 */
static void serial_autodetect_init (subsystype *subsys)
{

    /*
     * Link in the parser chains
     */
    parser_add_command_list(autodetect_chain_init_table, "autodetect");

    serial_autodetect_inQ = create_watched_queue("SERIAL_Autodetect inpaks",
						 0, 0);
    mgd_timer_init_parent(&serial_autodetect_timer, NULL);

    /*
     * Hook up ourselves to the system
     */
    reg_add_serial_autodetect_start(serial_autodetect_start,
				    "serial_autodetect_start");
    reg_add_serial_autodetect_register_encaps(serial_autodetect_reg_encaps,
					      "serial_autodetect_reg_encaps");
    /*
     * fire up the serial autodetect process
     */
    serial_autodetect_fork = process_create(serial_autodetect,
					    "SERIAL A'detect",
					    NORMAL_STACK,
					    PRIO_NORMAL);
    if (serial_autodetect_fork) {
	process_set_crashblock(serial_autodetect_fork, TRUE);
    }
}


/*
 * Serial Autodetect subsystem header
 */
#define SERIALAUTODETECT_MAJVERSION   1
#define SERIALAUTODETECT_MINVERSION   0
#define SERIALAUTODETECT_EDITVERSION  1

SUBSYS_HEADER(ser_autodetect, SERIALAUTODETECT_MAJVERSION,
	      SERIALAUTODETECT_MINVERSION, SERIALAUTODETECT_EDITVERSION,
	      serial_autodetect_init, SUBSYS_CLASS_KERNEL,
	      "seq: serial",
	      "req: serial");
