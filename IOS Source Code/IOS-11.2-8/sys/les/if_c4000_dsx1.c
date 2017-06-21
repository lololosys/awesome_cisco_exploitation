/* $Id: if_c4000_dsx1.c,v 3.8.12.18 1996/09/07 22:37:07 jliu Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c4000_dsx1.c,v $
 *------------------------------------------------------------------
 * if_c4000_dsx1.c - dsx1 support routines for generic E1/T1 4XXX driver.
 *
 * Nov 27 1994, Michael Hubenthal and Manoj Leelanivas
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c4000_dsx1.c,v $
 * Revision 3.8.12.18  1996/09/07  22:37:07  jliu
 * CSCdi64205:  Unprovisioned T1 plugged into AS5200 causes box to hang
 * Branch: California_branch
 *     o The fix included display warning message to console port and used
 *       SNMP notification to send the message to network management
 *       station
 *
 * Revision 3.8.12.17  1996/08/28  12:57:51  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.12.16  1996/08/27  01:22:34  vdukki
 * CSCdi52688:  After clearing pri counters, sho controller shows loss of
 * signal
 * Branch: California_branch
 * Created dsx1_clear_mib_data function to clear the dsx1 MIB table. It
 * will be called when "clear counters" command is run, instead of
 * dsx1_init_mib_data().
 *
 * Revision 3.8.12.15  1996/08/13  02:20:00  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.8.12.14  1996/07/11  14:20:23  fbordona
 * CSCdi62521:  DLSw circuit does not connect using FST over ATM peer
 *              Remove DLSw FST hooks from platform/media specific drivers.
 * Branch: California_branch
 *
 * Revision 3.8.12.13  1996/06/28  23:23:46  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.8.12.12  1996/06/26  03:06:05  sdurham
 * CSCdi54840:  Channelized T1 needs support for ds1 physical interface in
 * snmp
 * Branch: California_branch
 * add support in ifmib for cdbtype
 *
 * Revision 3.8.12.11  1996/06/16  21:15:27  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.8.12.10  1996/06/05  20:58:24  wmay
 * CSCdi59462:  DSX1 remote interface loopback process needs help
 * Fix up remote loopback state machine - use far_end_looped field
 * to indicate actual loop state - change buffer display from serial
 * int debug to serial packet
 * Branch: California_branch
 *
 * Revision 3.8.12.9  1996/06/01  01:31:58  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.8.12.8  1996/05/21  09:56:11  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8.12.7  1996/05/21  06:35:10  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.8.12.6  1996/05/10  01:34:20  wmay
 * CPP and ppp half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.8.12.5  1996/05/06  23:24:15  sdurham
 * CSCdi54843:  RFC1406 MIB implementation has table indices starting at 0
 * Branch: California_branch
 *      initialize seed value for 4000 as is done for mip.
 *
 * Revision 3.8.12.4  1996/04/27  06:32:46  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.8.12.3  1996/04/22  18:14:32  gstovall
 * CSCdi53071:  media idb init routines will fail on second pass
 * Branch: California_branch
 * Allow media idb init routines to be called again.  Also clean up the
 * handling of the media idb queues.
 *
 * Revision 3.6.8.3  1996/04/26  01:41:24  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.6.8.2  1996/04/17  03:34:04  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.6.8.1  1996/02/23  20:51:41  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.8.12.2  1996/03/29  18:19:00  sdurham
 * CSCdi51714:  Duplicate rows in ifTable for Channelized T1
 * Branch: California_branch
 *     Deregister old channels with snmp.
 *
 * Revision 3.8.12.1  1996/03/18  20:41:35  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.3  1996/03/13  01:49:16  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.2.2  1996/02/20  01:08:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/13  02:54:02  hampton
 * Stop the carrier timer before clearing the IDB.  [CSCdi46506]
 *
 * Revision 3.7.2.1  1996/01/24  22:23:16  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.7  1996/01/22  06:37:35  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.6  1996/01/06  00:40:36  wmay
 * CSCdi46525:  BOGUSENCAP message with V.120 over PRI - allow V.120 with
 * no error message
 *
 * Revision 3.5  1995/12/13  04:22:14  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.4  1995/12/06  18:30:53  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.3  1995/11/17  17:39:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:17  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.14  1995/11/08  21:14:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.13  1995/10/18  17:21:32  wmay
 * CSCdi39845:  show interface displays serials in a non-intuitive manner
 * Remove extra interrupt disable around idb_enqueue
 *
 * Revision 2.12  1995/06/29  23:30:12  jchlin
 * Currently, c4000 and c7000 are reusing hwidbs and swidbs because
 * of dynamic changes of interface configuration. Due to the lack of
 * way to clean up the idbs when they are freed, both platforms will
 * try to release as many known buffer pointers of idb as possible
 * before they are put in the private queue for reuse.
 * When reusing idbs, idb init routines called inside idb_create()
 * are called to reinitialized idbs. In 11.0, there is a new
 * function call added in idb_create() which is not picked up by
 * both platform init_idb(). And therefore, it caused a crash
 * because a null pointer is not initialized. This fix is trying to
 * separate the malloc of idbs and initialization of idbs into
 * two different functions so that the initialization of
 * both hwidb and swidb can be done in that new rourtine as well
 * as new addition function calls in the future. Then we can avoid
 * another addition of function calls in the idb_create() to crash
 * both platform again.
 * CSCdi36076:  Configuring PRI on E1 crashes box w/ALIGN-1-FATAL
 *
 * Revision 2.11  1995/06/28  09:28:51  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.10  1995/06/21  07:14:55  jchlin
 * CSCdi35650 (transmit delay bug) and CSCdi35651 (remote loopback bug)
 * are fixed in this commit. Now, the hdlc flags will be used to
 * implement the transmit delay. The interface remote loopback will
 * send out the DDS CSU loopback patterns to activate and deactivate
 * CSU remote loopback.
 * CSCdi35650:  The interface transmit delay for T1/E1/PRI doesnt work
 *
 * Revision 2.9  1995/06/21  03:01:05  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.8  1995/06/20  20:27:58  jchlin
 * Fix the channel throttling routines so that the rx channel
 * won't get stucked after throttling.
 * Fix the rx channel descriptor ring handling so that rx
 * channel won't get stucked after the ring is full and
 * gets processed during the single interrupt.
 * CSCdi34370:  4000 crashed w/BADMAGIC during stress test
 *
 * Revision 2.7  1995/06/19  06:47:17  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.6  1995/06/11  20:34:48  mleelani
 * CSCdi35374:  Too many console messages for state changes of new
 * interface
 * Do not change the state of B channels from driver level.
 *
 * Revision 2.5  1995/06/11  20:04:41  mleelani
 * CSCdi30151:  incomplete sho interface for new interface
 * Support keepalives. Display timeslots and transmitter delay
 * in show interface for channelized serial.
 *
 * Revision 2.4  1995/06/11  19:39:05  mleelani
 * CSCdi35346:  Controller confused about interface cable type
 * Fix incorrect fallthrough.
 *
 * Revision 2.3  1995/06/09  21:43:58  bdas
 * CSCdi35456:  Spaten : Each B and D channel has two ifIndex values.
 * Avoid registering them (with SNMP) twice.
 *
 * Revision 2.2  1995/06/08  22:05:44  jliu
 * CSCdi35359:  Rename files for new hardware changes
 *
 * Revision 2.1  1995/06/07  21:34:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "../h/cs_registry.regh"
#include "fastswitch_registry.h"
#include "interface_private.h"
#include "subsys.h"
#include "packet.h"
#include "../os/pool.h"
#include "../os/buffers.h"
#include "config.h"
#include "logger.h"
#include "../if/network.h"
#include "if_les.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../wan/ppp.h"
#include "../if/ether.h"
#include "../wan/smds.h"
#include "../ip/ip.h"
#include "../tcp/tcpcompress.h"
#include "../if/if_controller.h"
#include "../if/dsx1.h"
#if defined(XX) | defined(SIERRA)
#   include "if_c4000_mcn.h"
#   include "if_c4000_dsx1_framer.h"
#endif
 
#if defined(BRASIL)
#   include "if_as5200_mcn.h"
#   include "if_as5200_dsx1_framer.h"
#endif
#include "if_c4000_m32.h"
#include "les_controller.h"
#include "if_c4000_dsx1.h"
#include "if_c4000_ce1.h"
#include "if_c4000_bt8360.h"
#include "../if/dsx1_registry.h"
#include "isdn_bri.h"
#include "../wan/atm_dxi.h"
#include "../if/serial_inline.h"
#include "../wan/dialer.h"
#include "../wan/dialer_registry.h"
#include "../wan/serial_parser.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../if/if_msg_controller.c"	/* Not a typo, see logger.h */
#include "msg_dsx1.c"			/* Not a typo, see logger.h */
#include "../snmp/chassismib_registry.h"
#include "../snmp/ifmib_registry.h"

watched_boolean *m32print_process_awaken;
watched_boolean *calldrop_process_awaken;
pid_t m32_pid = 0;

/*
 * static references and declarations.
 */
static queuetype dsx1_idbQ;      /* list of reusable hwidbs */


/*
 * =====================================================================
 * dsx1_m32_errors_poll -
 *
 * Description:
 * Use background process to poll a flag to see the error message need to
 * be print out. The reason to do this is intense errors interrupt from
 * Munich32 uses a lot of of CPU bandwidth and cause all calls on the other
 * T1 to drop. In order to prevent this happened, the troubled T1 will be put
 * in local loopback mode until user intervention to remove the errors and
 * pull the T1 out of loopback mode
 *
 * Parameters:
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
process dsx1_m32_errors_poll (void)
{
    ulong            slot, subcont;
    mcn_instance_t   *mcn;
    dsx1info         *info;
    dsx1_instance_t  *dsx1;
    m32_instance_t   *m32_ds;
    static           firsttime = TRUE;
    leveltype        SR;
    boolean          kill_this_process = TRUE;

    while (TRUE) {
        process_sleep_periodic(15*ONESEC);
        if (!process_get_arg_num(&slot)) {
            printf("\ndsx1_m32_errors_poll process: Could not get argument\n");
            process_kill(THIS_PROCESS);
        }

        SR = raise_interrupt_level(NETS_DISABLE);
        mcn = MCN_INSTANCE(slot);
        for (subcont=0; subcont < mcn->total_subcont; subcont++) {
            m32_ds = M32_INSTANCE(slot, subcont);
            dsx1 = DSX1_INSTANCE(slot, subcont);
            info = dsx1->dsx1_ds;
            if (m32_ds->idb_runts) {
               dsx1->runts_recovery = TRUE;
               m32_ds->no_auto_check = FALSE;
               m32_ds->en_runts_check = TRUE;
               reg_invoke_dsx1_noloop(m32_ds->idb_runts);

               if (!firsttime) {
                  if ((m32_ds->m32_pro_kill == TRUE) || ((m32_ds->runts_count) &&
                       (m32_ds->runts_count == m32_ds->idb_runts->counters.runts))) {
                     m32_ds->m32_pro_kill = FALSE;
                     m32_ds->no_auto_check = TRUE;
                     m32_ds->runts_count = 0x0;
                     dsx1->runts_recovery = FALSE;
                     errmsg(&msgsym(FIRMWARE, CONTROLLER), info->cdb->cdb_namestring,"AUTORECOVERY is done");
                     m32_ds->idb_runts = 0x0;
                     reg_invoke_dsx1_loopback_clk_switch(info, DSX1_AUTORECOVERED_EVENT);
                  } else {
                     m32_ds->runts_count = m32_ds->idb_runts->counters.runts;
		     kill_this_process = FALSE;
                  }
               } else {
                  firsttime = FALSE;
		  kill_this_process = FALSE;
               }
            }
        }

        reset_interrupt_level(SR); 
        if (kill_this_process) {
           m32_pid = 0;
           process_kill(THIS_PROCESS);
        } else {
           kill_this_process = TRUE;
        }
 

    }
}
/*
 * =====================================================================
 * dsx1_m32_errors_msg
 *
 * Description:
 *
 * Parameters:
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void dsx1_m32_errors_msg (ulong msg_index)
{
	switch(msg_index) {

	case M32_ERR_NO_TIMESLOT: 
		errmsg(&msgsym(M32_NO_TIMESLOT, DSX1));
		break;

	case M32_ERR_TIMESLOT_IN_USE: 
		errmsg(&msgsym(M32_TIMESLOT_IN_USE, DSX1));
		break;

	case M32_ERR_ILLEGAL_CHANNEL: 
		errmsg(&msgsym(M32_ILLEGAL_CHANNEL, DSX1));
		break;

	case M32_ERR_AR_TIMEOUT: 
		errmsg(&msgsym(M32_AR_TIMEOUT, DSX1));
		break;

	case M32_ERR_AR_FAILURE: 
		errmsg(&msgsym(M32_AR_FAILURE, DSX1));
		break;

	case M32_ERR_BAD_STATE: 
		errmsg(&msgsym(M32_BAD_STATE, DSX1));
		break;

	case M32_ERR_BAD_BITRATE: 
		errmsg(&msgsym(M32_BAD_BITRATE, DSX1));
		break;

	case M32_ERR_BAD_INTQ: 
		errmsg(&msgsym(M32_BAD_INTQ, DSX1));
		break;

	case M32_ERR_MEM_ALLOC: 
		errmsg(&msgsym(M32_MEM_ALLOC, DSX1));
		break;
	default:
		errmsg(&msgsym(M32_ERR, DSX1));
		break;

	}
}

/*
 * =====================================================================
 * dsx1_m32_errors_print -
 *
 * Description:
 * Use background process to poll a flag to see the error message need to
 * be print out. The reason to do this is intense errors interrupt from
 * Munich32 uses a lot of of CPU bandwidth and cause all calls on the other
 * T1 to drop. In order to prevent this happened, the troubled T1 will be put
 * in local loopback mode until user intervention to remove the errors and
 * pull the T1 out of loopback mode
 *
 * Parameters:
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
process dsx1_m32_errors_print (void)
{
     ulong            subcont, slot, major, minor;
     dsx1info         *info;
     mcn_instance_t   *mcn;
     dsx1_instance_t  *dsx1;
     m32_instance_t   *m32_ds;
     leveltype        SR = 0;
     hwidbtype        *idb_current, *idb;
     m32_channel_params_t *channel_params;



     process_watch_boolean(m32print_process_awaken, ENABLE, RECURRING);

     while (TRUE) {
        process_wait_for_event();
        while (process_get_wakeup(&major, &minor)) {
           switch (major) {
              case BOOLEAN_EVENT:
                 if ((minor != W_BOOLEAN_M32_TYPE) ||
                     (!process_get_boolean(m32print_process_awaken)))
                     break;

                 process_set_boolean(m32print_process_awaken, FALSE);
                 if (!process_get_arg_num(&slot)) {
                    printf("\ndsx1_m32_errors_print process: Could not get argument\n");
                    process_kill(THIS_PROCESS);
                 }

		 /*
		  * Since we might want to switch the line clock source 
		  * selection later in this process (depending on the user's 
		  * configuration), we wait until system-init is complete, 
		  * so that we are sure that the user's clock source 
		  * configuration has been parsed from NVRAM.
		  */
		 process_wait_on_system_init();

                 SR = raise_interrupt_level(NETS_DISABLE);
                 mcn = MCN_INSTANCE(slot);
                 for (subcont=0; subcont < mcn->total_subcont; subcont++) {
                     m32_ds = M32_INSTANCE(slot, subcont);
                     dsx1 = DSX1_INSTANCE(slot, subcont);
                     info = dsx1->dsx1_ds;

                     idb = m32_ds->idb_runts;
                     FOR_ALL_HWIDBS(idb_current) {
                        if ((idb_current->slot == idb->slot) &&
                                    (idb_current->subcont == idb->subcont)) {
                           channel_params = &m32_ds->channel_params[idb_current->m32_channel];
                           channel_params->runt_interval = 0x0;
                           channel_params->runt_started = 0x0;
                           channel_params->runts_errors = 0x0;
                           channel_params->drop_call = TRUE;
                           channel_params->first_start = TRUE;
                           channel_params->ar_failed = FALSE;
                        }
                     }

                     if (dsx1->m32_errmsg_print == TRUE) {
                         dsx1->m32_errmsg_print = FALSE;
                         errmsg(&msgsym(FIRMWARE, CONTROLLER),
                                   info->cdb->cdb_namestring,
                                   "in LOCAL LOOPBACK due to excessive errors");
                         reg_invoke_dsx1_loopback_clk_switch(info, DSX1_INTO_LOOP_EVENT);
                          if (!process_exists(m32_pid))
                             m32_pid = process_create(dsx1_m32_errors_poll, "M32_runts polling", NORMAL_STACK, PRIO_NORMAL);
                          if (m32_pid == NO_PROCESS) {
                              printf("\nas5200_dsx1: Unable to create dsx1_m32_errors_poll process");
                              crashdump(0);
                          }
                          process_set_arg_num(m32_pid, (ulong)slot);
                     }
                  }
                  reset_interrupt_level(SR);
                  break;
              default:
                  break;
              }
         }
     }
}

/*
 * =====================================================================
 * dsx1_call_drop -
 *
 * Description:
 * Use background process to drop a call from a particular B channel since
 * excessive errors was detected from that B channel
 *
 * Parameters:
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
process dsx1_call_drop (void)
{
     ulong                subcont, slot, major, minor, ix;
     mcn_instance_t       *mcn;
     m32_instance_t       *m32_ds;
     dsx1_instance_t      *dsx1;
     dsx1info             *info;
     m32_channel_params_t *channel_params;
     leveltype            SR = 0;


     process_watch_boolean(calldrop_process_awaken, ENABLE, RECURRING);

     while (TRUE) {
       process_set_boolean(calldrop_process_awaken, FALSE);
       process_wait_for_event();
       while (process_get_wakeup(&major, &minor)) {
         switch (major) {
            case BOOLEAN_EVENT:
               if ((minor != W_BOOLEAN_CALL_TYPE) ||
                   (!process_get_boolean(calldrop_process_awaken)))
                   break;

               if (!process_get_arg_num(&slot)) {
                  printf("\ndsx1_call_drop process: Could not get argument\n");
                  process_kill(THIS_PROCESS);
               }

               SR = raise_interrupt_level(NETS_DISABLE);
               mcn = MCN_INSTANCE(slot);
               for (subcont=0; subcont < mcn->total_subcont; subcont++) {
                   m32_ds = M32_INSTANCE(slot, subcont);
                   dsx1 = DSX1_INSTANCE(slot, subcont);
                   info = dsx1->dsx1_ds;
                   for (ix=0; ix < M32_NUM_CHANNELS; ix++) {
                     channel_params = &m32_ds->channel_params[ix];
                     if ((channel_params) && (channel_params->idb_drop)) {
                        reset_if(channel_params->idb_drop);
                        errmsg(&msgsym(FIRMWARE, CONTROLLER),info->cdb->cdb_namestring,"Call DROPPED due to excessive errors");
                        channel_params->idb_drop = 0x0;
                     }
                   }
                   m32_ds->set_calldrop = FALSE;
                }
                reset_interrupt_level(SR);
                break;
             default:
                break;
          }
       }
   }
}

/*
 * =====================================================================
 * dsx1_pro_add -
 *
 * Description:
 * Create a process to handle excessive M32 errors message print out
 *
 * Parameters:
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
void dsx1_pro_add (int slot)
{
    pid_t          pid;

    m32print_process_awaken = create_watched_boolean("M32_errors print",
                                                     W_BOOLEAN_M32_TYPE);

    pid = process_create(dsx1_m32_errors_print, "M32_runts printing",
                    NORMAL_STACK, PRIO_NORMAL);
    if (pid == NO_PROCESS) {
       printf("\nas5200_dsx1: Unable to create dsx1_m32_errors_print process");
       crashdump(0);
    }
    process_set_arg_num(pid, (ulong)slot);

    calldrop_process_awaken = create_watched_boolean("Calldrop alarm",
                                                     W_BOOLEAN_CALL_TYPE);

    pid = process_create(dsx1_call_drop, "Call drop process",
                    NORMAL_STACK, PRIO_NORMAL);
    if (pid == NO_PROCESS) {
       printf("\nas5200_dsx1: Unable to create dsx1_call_drop process");
       crashdump(0);
    }
    process_set_arg_num(pid, (ulong)slot);
}

/*
 * =====================================================================
 * dsx1_setup_buffer_pool()
 *
 * Description:
 * Setting up buffer pools and ring sizes.
 *
 * Parameters:
 * hwidb ptr, channel_init_struct ptr.
 * 
 * Returns:
 * boolean
 * =====================================================================
 */
static boolean dsx1_setup_buffer_pool (hwidbtype 	       *hwidb,
				       m32_channel_init_info_t *channel_init,
				       uint 		       num_time_slots)
{
    pooltype *dsx1_buffer_pool = NULL;
    uint rx_ring_size, tx_ring_size;
    uint multiplier;
    dsx1info *dsx1;

    dsx1 = dsx1_list[hwidb->unit];

    /*
     * We use a heuristic based on the number of timeslots assigned
     * to the interface. If one timeslot is assigned to one interface,
     * we choose the lowest watermark for the max_rx_ring. If it is
     * more than 1 and less than 2, we use double the low watermark.
     * The highest watermark is 4 * low watermark. This policy is used
     * irrespective of whether the box is a sierra of XX.
     *
     * This policy limits the max memory usage per nim to be under 1 MB
     * when using big buffers and less and around 1.5 MB, when using
     * large buffers.
     *
     * This design is envisaged because of :
     * o an E1 box can contain a max of 31 serial interfaces, and can
     *   be quite a memory hog if we use the normal buffer allocation.
     * o memory requirements are dynamic since idbs are created and
     *   deleted.
     */
    if ((hwidb->isdn_info) && (hwidb->isdn_info->chantype == ISDN_Dchan))
        multiplier = 8;   /* due to no throttling for D channel */
    else if (num_time_slots == 1)
	multiplier = 1;
    else if (num_time_slots < 4)
	multiplier = 2;
    else
	multiplier = 4;
    
    if (hwidb->max_buffer_size <= BIGDATA) {
	rx_ring_size = DSX1_BIG_BUFFER_LOW_WATERMARK * multiplier;
	tx_ring_size = 8;
    } else if (hwidb->max_buffer_size <= LARGEDATA) {
	rx_ring_size = DSX1_LARGE_BUFFER_LOW_WATERMARK * multiplier;
	tx_ring_size = 4;
    } else {
	rx_ring_size = DSX1_HUGE_BUFFER_LOW_WATERMARK * multiplier; 
	tx_ring_size = 4;
    }
    
    /*
     * Initialize private buffer pool if there isn't one.
     * Populate the pool, initialize a private buffer cache,
     * and populate the buffer cache with buffers from the newly
     * created private pool.  Also set up the fallback public buffer
     * pool. 
     */
    if (!hwidb->pool) {
	if (!hwidb->hw_namestring)
	    idb_init_names(hwidb->firstsw, FALSE);

	if (dsx1->pool_group[hwidb->vc] == POOL_GROUP_INVALID) {
	    dsx1->pool_group[hwidb->vc] = pool_create_group();
	}

	hwidb->pool_group = dsx1->pool_group[hwidb->vc];

	dsx1_buffer_pool = pak_pool_create(hwidb->hw_namestring,
					       hwidb->pool_group,
					       hwidb->max_buffer_size,
					       POOL_SANITY, NULL); 
	if (dsx1_buffer_pool) {
	    dsx1_buffer_pool->fallback =
		pak_pool_find_by_size(hwidb->max_buffer_size);
	    pool_adjust(dsx1_buffer_pool, 0, PRIV_NUM_BUF(rx_ring_size),
			PRIV_NUM_BUF(rx_ring_size), TRUE);
	    pak_pool_create_cache(dsx1_buffer_pool,
				      PRIV_NUM_CACHE(rx_ring_size));
	    pool_adjust_cache(dsx1_buffer_pool, PRIV_NUM_CACHE(rx_ring_size)); 
	    hwidb->pool = dsx1_buffer_pool;
	}
    }
    
    /*
     * Propagate ring sizes to the Munich32 channel.
     */
    channel_init->rx_ring_size = rx_ring_size;
    channel_init->tx_ring_size = tx_ring_size;

    if (hwidb->pool)
	return(TRUE);
    else
	return(FALSE);
}


/*
 * =====================================================================
 * dsx1_show_serial_state
 *
 * Desccription :
 * Show serial state specific to the CE1/T1.
 *
 * Returns:
 * void.
 * =====================================================================
 */
static void dsx1_show_serial_state (hwidbtype *idb, char *leader)
{
    char buffer[100];
    
    dsx1_show_timeslots(buffer, dsx1_find_by_unit(idb->unit), idb->vc);
    printf("\n  Timeslot(s) Used:%s, Transmitter delay is %d flags",
	   buffer, idb->xmitdelay);
}


/*
 * =====================================================================
 * dsx1_RX_interrupt()
 *
 * Description:
 *
 * The dsx1 interrupt routines are called from the Munich32 interrupt
 * handler. The Munich32 isr does ring management and interrupt 
 * handling. The dsx1 interrupt handlers need only handle packet 
 * delivery related issues.
 *
 * There is always going to be an overhead of a check for an isdn idb,
 * which is inevitable unless we have separate routine for PRI RX
 * interrupts. 
 *
 * Parameters:
 * idb - the idb for the dsx1 channel 
 * pak - pointer to the received packet
 *
 * Returns:
 * nothing for now. what we want to do is if fastswitch succeeds,
 * return a flag so that we can try receiveing the next packet in ring.
 * =====================================================================
 */
static void dsx1_RX_interrupt (hwidbtype *idb, 
			       paktype *pak)
{
    idbtype 	*swidb;
    ushort 	rxtype;
    char 	*datagramstart;

    /*
     * At this point, we have to distinguish between PRI packets
     * and serial packets. PRI D channel packets are not fast
     * switched, and hence follow a separate path. When Greg's
     * integrated serial code is merged, the non PRI path
     * can be replaced by the standard serial_RX_interrupt vector.
     */
     if ((idb->isdn_info ) && (idb->isdn_info->chantype == ISDN_Dchan)) {
	 /*
	  * D channel packet, send it straight to process level
	  */
	 serial_queue_for_process(idb, pak);
     } else {	/* This looks like a serial or B-chan packet. */
	 datagramstart = pak->datagramstart;
	 switch(idb->enctype) {
	   case ET_HDLC:
	     rxtype = hdlc_decode_encapsulation_inline(pak, (vhdlctype *)
						       datagramstart);
	     if (reg_invoke_hdlc_fs(rxtype, pak)) {
		 return; 
	     }
	     break;
	     
	   case ET_FRAME_RELAY:
	     if (reg_invoke_fr_pvc_switch_or_fs(pak)) {
		 return;
	     }
	     rxtype = pak->rxtype;
	     break;
	     
	   case ET_SMDS:
	     rxtype = smds_decode_encapsulation_inline(pak, datagramstart);
	     if (reg_invoke_smds_fs(rxtype, pak)) {
		 return; 
	     }
	     break;
	     
	   case ET_PPP:
	     rxtype = ppp_decode_encapsulation_inline(pak, (ppp_hdr *)
						      datagramstart);
	     if (reg_invoke_ppp_fs(rxtype, pak)) {
		 return; 
	     }
	     break;

	   case ET_ATM_DXI:
	     if (atm_dxi_classify_pak_and_fs_les(pak)) {
		 return; 
	     }
	     rxtype = pak->rxtype;
	     break;
 
	   default:
	     rxtype = RXTYPE_UNKNOWN;
	     break;
	 }
	 /*
	  * Handle receive frame special cases
	  */
	 switch(rxtype) {
	     /*
	      * Consider a frame for bridging
	      */
	   case RXTYPE_TRANSIT:
	     if (idb->isdn_info)
		 swidb = idb->isdn_info->d_chan_idb->firstsw;
	     else
		 swidb = idb->firstsw;
	     if (RUNNING_IF(LINK_BRIDGE, swidb)) {
		 /*
		  * Mark as bridge encapsulation
		  */
		 pak->enctype = ET_BRIDGE;
		 if (!change_if_input(pak, swidb)) {
		     clear_if_input(pak);
		     idb->counters.input_drops++;
		     MAYBE_DROPPED(idb);
		     datagram_done(pak);
		     return;
		 }
		 if (reg_invoke_tbridge_forward(pak)) {
                     return; 
		 }
	     }
	     break;
	     
	     /*
	      * Consider a frame for RSRB
	      */
	   case RXTYPE_SRB:
	     if (reg_invoke_nim_vring_input(idb, pak)) {
		 return;
	     }
	     break;

	     /*
	      * Consider a frame for DLSw
	      */
	   case RXTYPE_DLSW:
	     if (reg_invoke_nim_dlsw_input(idb, pak)) {
		 return;
	     }
	     break;
	     
	   case RXTYPE_LEX:
	     { 
		 idbtype *lex_swidb = idb->firstsw->lex_swidb;
		 
		 /*
		  * Lex not bound to serial.
		  */
		 if (lex_swidb == NULL) {
		     datagram_done(pak);
		     return;
		 }
		 
		 /*
		  * Only bridge protocols that we are not routing.
		  */
		 if (ROUTING_RXTYPE(idb->firstsw, pak->rxtype) == FALSE) {
		     /*
		      * Is there a bridge group on the lex interface.
		      */
		     if (lex_swidb->tbridge_on_swidb) {
			 /*
			  * Mark as bridge encapsulation
			  */
			 pak->enctype = ET_BRIDGE;
			 /*
			  * Change the input interface so that it points to
			  * the lex interface.
			  */
			 change_if_input(pak, lex_swidb);
			 
			 if (reg_invoke_tbridge_forward(pak)) { 
			     return;
			 }
			 change_if_input(pak, idb->firstsw);
		     }
		 }
	     }
	     break;
	     
	   default:
	     break;
	 }
	 
	 /*
	  * We did not fast switch the serial datagram.
	  * Queue packet for process level:
	  */
	 serial_queue_for_process(idb, pak);
     }
}


/*
 * =====================================================================
 * dsx1_TX_interrupt()
 *
 * Description:
 * Handle a just transmitted packet notification.
 * The bri interrupt routines are called from the Munich32 interrupt
 * handler. The Munich32 isr handles all ring and buffering issues.
 *
 * Parameters:
 * idb - the idb of the dsx1 channel 
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void dsx1_TX_interrupt (hwidbtype *hwidb, 
			       paktype *pak)
{
    GET_TIMESTAMP(hwidb->lastoutput);
    m32_tx_start(hwidb);
}


/*
 * =====================================================================
 * dsx1_encapsulation()
 *
 * Description:
 * Set the PRI/E1/T1 specific encaps.
 *
 * Parameters:
 * hwidbtype *idb
 *
 * Returns:
 * void
 * 
 * =====================================================================
 */
static void dsx1_encapsulation (parseinfo *csb)
{
    idbtype *idb = csb->interface;
    hwidbtype *hwidb = idb->hwptr;
    dialerdbtype *ddb, *member_ddb;

    if (!is_isdn(hwidb))
	serial_encapsulation(csb);
    else {
	/*
	 * This is a PRI interface.
	 */
	reg_invoke_isdn_pri_setencap(csb);
	reg_invoke_pri_member_adjust_encap(hwidb, 0);
	ddb = get_dialerdb(hwidb);
	for (member_ddb = ddb->hunt_group_first;
	     member_ddb;
	     member_ddb = member_ddb->hunt_group_next) {
	    reg_invoke_pri_member_adjust_encap(member_ddb->ownidb, 0);
	}
    }
}


/*
 * =====================================================================
 * dsx1_show_errors()
 *
 * Description:
 * Print dsx1 errors.
 *
 * Parameters:
 * hwidb - the hw idb for the dsx1 channel
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void dsx1_show_errors (hwidbtype *hwidb)
{
    m32_instance_t *m32_instance = M32_INSTANCE(hwidb->slot, hwidb->subcont);
 
    printf("  %d missed datagrams, %d overruns, %d memory errors\n",
	   hwidb->counters.input_resource, hwidb->counters.input_overrun,
	   hwidb->dma_memory_error);
    printf("  %d transmitter underruns,",hwidb->counters.output_underrun);
    printf(" %d throttles, %d enables, %d bad interrupt elements\n",
	   m32_instance->channel_params[hwidb->m32_channel].throttle_count,
	   m32_instance->channel_params[hwidb->m32_channel].enable_count,
	   m32_instance->bad_intq_element_count);
}


/*
 * =====================================================================
 * dsx1_show_controller()
 *
 * Description:
 * Print controller stats for a dsx1 interface.
 *
 * Parameters:
 * idb - idb of the dsx1 channel
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static boolean dsx1_show_controller (hwidbtype *hwidb, parseinfo *csb)
{
    m32_instance_t *m32_instance = M32_INSTANCE(hwidb->slot, hwidb->subcont);
 
    printf("\nController %d is type: ", hwidb->unit);
    switch (dsx1_list[hwidb->unit]->applique_type) {
      case APPL_T1:
      case APPL_T1_CSU:
        printf("T1 ");
        break;
      case APPL_E1_BAL:
        printf("balanced E1 ");
        break;
      case APPL_E1_UNBAL:
        printf("unbalanced E1 ");
        break;
      default:
    }
    printf("in slot %d\n", hwidb->slot);
    printf("Serial interface %d:%d", hwidb->unit, hwidb->vc);

    printf("  Munich32 version ");
    switch (m32_instance->chip_version) {
    case M32_INT_VER_21: 
        printf("2.1");
        break;
    case M32_INT_VER_22:
        printf("2.2");
        break;
    case M32_INT_VER_32:
        printf("3.2");
        break;
    case M32_INT_VER_33:
        printf("3.3");
        break;
    default:
        printf("Unknown (%x)", m32_instance->chip_version);
    }
    printf("\n");

    /*
     * Print out the number of chip config success and failures
     */
    printf("Total chip configuration successes: %d, failures: %d, timeouts: %d\n",
	   m32_instance->ar_success, m32_instance->ar_failure,
	   m32_instance->ar_timeout);

    /*
     * print the ring and error info.
     */
    m32_print_rings(m32_instance, hwidb->m32_channel);
    dsx1_show_errors(hwidb);

    /*
     * always return false becuase this is what the
     * calling function is expecting.
     */
    return(FALSE);
}


/*
 * =====================================================================
 * dsx1_txqlength()
 *
 * Description:
 * Return the number of frames on the transmit ring that are
 * waiting to be transmitted.
 *
 * Parameters:
 * hwidb
 *
 * Returns:
 * count of frames on the transmit ring
 * =====================================================================
 */
static int dsx1_txqlength (hwidbtype *hwidb)
{
    int frame_count;
    leveltype int_level;
    m32_instance_t *m32_ds;
 
    m32_ds = M32_INSTANCE(hwidb->slot, hwidb->subcont);

    int_level = raise_interrupt_level(NETS_DISABLE);
    frame_count = m32_ds->channel_params[hwidb->m32_channel].tx_ring_use_count;
    reset_interrupt_level(int_level);

    return(frame_count);
}

/*
 * =====================================================================
 * dsx1_shutdown()
 *
 * Description:
 * Shut a dsx1 interface.
 *
 * Parameters:
 * idb - the idb for the channel group.
 * newstate - the state this interface is being put into
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void dsx1_shutdown (hwidbtype *hwidb, int newstate)
{
    leveltype int_level;
    m32_instance_t *m32_ds;

    m32_ds = M32_INSTANCE(hwidb->slot, hwidb->subcont);

    /*
     * if this is a PRI interface, only allow the 
     * D channel to be shut down. 
     */
    if ((!(is_isdn(hwidb))) ||
	 (hwidb->isdn_info->chantype == ISDN_Dchan)) {

        int_level = raise_interrupt_level(NETS_DISABLE);
        /*
         * Deregister the d channel idb in the modem management subsystem
         * for call switching purpose. The modem call only use one
         * time slot.
         */
        if ((is_isdn(hwidb)) && (hwidb->isdn_info->chantype == ISDN_Dchan)) {
            reg_invoke_dsx1_delete_signaling_interface(hwidb);            
        }

	/*
	 * Down the channel.
	 */
	hwidb->state = newstate;
        GET_TIMESTAMP(hwidb->state_time);
	m32_down_channel(m32_ds, hwidb->vc);

        reset_interrupt_level(int_level);
    }   
}

/*
 * =====================================================================
 * dsx1_enable()
 *
 * Description:
 * Bring a throttled dsx1 interface back up. Simply call the
 * Munich32 function that handles this.
 *
 * Parameters:
 * idb - the idb for the channel group.
 *
 * Returns:
 * void
 * =====================================================================
 */
static void dsx1_enable (hwidbtype *hwidb)
{
    m32_instance_t *m32_ds;

    m32_ds = M32_INSTANCE(hwidb->slot, hwidb->subcont);

    m32_enable(m32_ds, hwidb->m32_channel, M32_CHANNEL_UP);
}

/*
 * =====================================================================
 * dsx1_handler()
 *
 * Description:
 * This function does the init of the frame process.
 *
 * Returns:
 * void
 * =====================================================================
 */
static void dsx1_handler (void)
{
    (void) cfork((forkproc *) framer_background_process, 0, 1000,
		"Framer background",
		0);
}

/*
 * =====================================================================
 * dsx1_controller_reset()
 *
 * Description:
 * This function does the controller reset.
 *
 * Returns:
 * void
 * =====================================================================
 */
static void dsx1_controller_reset (cdbtype *cdb)
{
    switch (cdb->cdb_state) {
      case IDBS_ADMINDOWN:
	errmsg(&msgsym(UPDOWN, CONTROLLER),
	       cdb->cdb_namestring, print_cdbstate(IDBS_ADMINDOWN));
        framer_send_ais(cdb, TRUE);
	break;

      case IDBS_DOWN:
	/*
	 * Note: All controller/idb restarts are handled from framer
	 */
        framer_send_ais(cdb, FALSE);
        break;
	
      default:
	/* do nothing */
    }
}

/*
 * =====================================================================
 * dsx1_clear_counts()
 *
 * Description:
 * This function will clear the local counts
 *
 * Returns:
 * void
 * =====================================================================
 */
static void dsx1_clear_counts(cdbtype *cdb)
{
 /* Clear the t1 and e1 counters */

   dsx1_clear_mib_data(dsx1_find_by_unit(cdb->unit));

}



/*
 * =====================================================================
 * dsx1_m32_setup_init_struct()
 *
 * Description:
 * This routine sets up the init struct for initializing a 
 * Munich32 channel.
 *
 * Parameters:
 * dsx1 - ptr to a struct describing this dsx1 interface
 * hwidb - ptr to the idb of the channel to be mapped 
 * channel_init - struct used to init a Munich32 channel
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void dsx1_m32_setup_init_struct (dsx1info   *dsx1,
					hwidbtype  *hwidb,
					m32_channel_init_info_t *channel_init)
{
    int ix, num_timeslots = 0;
    int num_bits_speed;
    m32_instance_t *m32_ds;
    m32_channel_params_t *channel_params;
 
    m32_ds = M32_INSTANCE(hwidb->slot, hwidb->subcont);
    channel_params = &(m32_ds->channel_params[hwidb->vc]);
  
    channel_init->channel_num = hwidb->m32_channel;
    channel_init->channel_idb = hwidb;

    /*
     * Figure out what encapsulation we are configured for and
     * save the datagramstart offset for dsx1_getbuf
     */
    switch (hwidb->enctype) {
       case ET_HDLC:
       case ET_FRAME_RELAY:
       case ET_PPP:
       case ET_SMDS:
       case ET_SDLCP:
       case ET_SDLCS:
       case ET_STUN:
       case ET_LAPB:
       case ET_X25:
       case ET_ATM_DXI:
       case ET_V120:
       case ET_CPP:
	channel_init->data_offset = ENCAPBYTES - hwidb->encsize;
	break;
      default:
          errmsg(&msgsym(BOGUSENCAP, LINK), hwidb->hw_namestring,
		 hwidb->enctype);
          channel_init->data_offset = ENCAPBYTES - HDLC_ENCAPBYTES;
    }

    /* 
     * bitrate of channel in Kbit/sec. the speed is stored as a
     * bitfield where every bit set is 8 Kbit/sec. this speed 
     * applies to every timeslot assigned to this channel.
     */
    num_bits_speed = num_bits(dsx1->speed[hwidb->m32_channel]);
    channel_init->bitrate = num_bits_speed * 8;
    channel_init->bitrate_which_bits = M32_TIMESLOT_USE_MSB;
    /*
     * allow channel to be throttled when required except
     * for a PRI D channel.
     */
    if ((hwidb->isdn_info) && (hwidb->isdn_info->chantype == ISDN_Dchan)) {
        channel_init->throttle_channel = FALSE;
    } else {
        channel_init->throttle_channel = TRUE;
    }

    /*
     * figure out the timeslot stuff.
     */
    channel_init->timeslot_bitfield = 0;
    FOR_ALL_DSX1_TIMESLOTS(ix, dsx1) {
        if (dsx1->timeslots[ix] == hwidb->m32_channel) {
            /*
             * T1 controllers use timeslots 0-23 though dsx1
             * timeslots are numbered starting from 1. so
             * subtract 1 when creating the m32 bitfield.
             * E1 controllers use timeslots 1-31 since timeslot
             * zero can't be used so don't subtract one for E1.
             */
            if (hwidb->type == IDBTYPE_T1) {
                channel_init->timeslot_bitfield |= (1 << (ix-1));
            } else {
                channel_init->timeslot_bitfield |= (1 << ix);
            }
            num_timeslots++;
        }
    }

    /*
     * out of the 64 long words of Munich32 internal transmit
     * buffer, assign some to the channel depending on the
     * interface type.
     */
    channel_init->internal_buf_size = DSX1_M32_XMIT_BUF * num_timeslots;
 
    /*
     * setup the vectors for disabling and enabling the framer.
     */
    channel_init->disable_adaptor =
        (m32_disable_adaptor_t)return_nothing;
    channel_init->enable_adaptor =
        (m32_enable_adaptor_t)return_nothing;
    channel_init->down_connection =
        (m32_down_connection_t)return_nothing;
 
    /*
     * setup the receive and transmit interrupt handlers.
     * different receive handlers are used if loopback is set.
     */
    if (hwidb->loopback) {
        /*
         * local loopback requires pkts received be sent
         * right back out the interface they came in on.
         */
        channel_init->loopback = M32_LOCAL_LOOPBACK;
    } else if (hwidb->far_end_looped) {
 	if (hwidb->ext_loop_type == DSX1_LOOP_NONE)
 	    channel_init->loopback = M32_REMOTE_LOOPBACK_WAIT_DOWN;
    } else if (hwidb->ext_loop_type == DSX1_LOOP_REMOTE) {
	if (hwidb->far_end_looped == FALSE)
 	    channel_init->loopback = M32_REMOTE_LOOPBACK_WAIT_UP;
    } else {
        /*
         * no loopback set. use the dsx1 receive handler.
         */
        channel_init->loopback =  M32_NO_LOOPBACK;
    }

    channel_init->recv_handler = dsx1_RX_interrupt;
    channel_init->xmit_handler = dsx1_TX_interrupt;

    /*
     * start all munich32 channels in the UP state. 
     */
    channel_init->initial_state = M32_CHANNEL_UP;
}

/*
 * =====================================================================
 * dsx1_digital_call_report ()
 *
 * Description:
 * Called by the ISDN stack when a digital call connects or disconnects
 *
 * Parameters:
 * hwidb - ptr to the idb of the channel to be mapped
 * event - Indicator whether this is a channel up or down event
 *
 * Returns:
 * nothing
 *
 * Notes:
 * =====================================================================
 */
void dsx1_digital_call_report(hwidbtype * hwidb, ushort bchan, ushort msg)
{
    m32_instance_t *m32_ds = M32_INSTANCE(hwidb->slot, hwidb->subcont);

    if(msg == DEV_CONNECTED){
        m32_up_channel(m32_ds, bchan);
    }
    else if(msg == DEV_IDLE){
        m32_down_channel(m32_ds, bchan);
    }
}


/*
 * =====================================================================
 * dsx1_configure_channel()
 *
 * Description:
 * This routine maps the dsx1 channel to a Munich32 channel.
 * Channel configuration changes are done with this routine.
 * It is the routine that does the interface initialization.
 *
 * Parameters:
 * dsx1 - ptr to a struct describing this dsx1 interface
 * hwidb - ptr to the idb of the channel to be mapped 
 * channel - channel to be mapped
 * timeslots - bitfield of timeslots assigned to this channel
 * bitrate - speed of channel in Kbits/sec
 *
 * Returns:
 * nothing
 *
 * Notes:
 * Call this routine with dsx1_init(hwidb) if you just have a 
 * hw idb to pass.
 * =====================================================================
 */
static void dsx1_configure_channel (dsx1info   *dsx1,
				    hwidbtype  *hwidb,
				    int        channel,
				    ulong      timeslots,
				    int        bitrate)
{
    leveltype               int_level;
    m32_channel_init_info_t channel_init;
    m32_instance_t          *m32_ds;
    ulong                   num_time_slots = 0, err_code = 0;
 
    m32_ds = M32_INSTANCE(hwidb->slot, hwidb->subcont);

    int_level = raise_interrupt_level(NETS_DISABLE);
    /*
     * release any resources associated with this channel
     * before re-initiializing it.
     */
    m32_release_channel(m32_ds, channel);

    if ((timeslots != 0) && (bitrate != 0)) {
        /* 
         * this is a request to create a channel group.
         */
        hwidb->vc = channel;
        hwidb->m32_channel = channel;
        hwidb->counters.resets++;
	
        /*
         * only a single timeslot channel group can be remote looped.
         */
	num_time_slots =
	    dsx1_count_timeslots(dsx1_find_by_unit(hwidb->unit), hwidb->vc);
        if ((hwidb->ext_loop_type == DSX1_LOOP_REMOTE) &&
            (num_time_slots != 1)) {
            hwidb->ext_loop_type = DSX1_LOOP_NONE;
            errmsg(&msgsym(REMLOOP, LINK), hwidb->hw_namestring,
            " - can't do it with multiple timeslots", ".");
        }

	/*
	 * Setup buffer pool and find new ring sizes if needed.
         * Init channel if we have a buffer pool.
	 */
	if (dsx1_setup_buffer_pool(hwidb, &channel_init, num_time_slots)) {
	    dsx1_m32_setup_init_struct(dsx1, hwidb, &channel_init);
	    err_code = m32_init_channel(m32_ds, &channel_init);

	    if (err_code != M32_NOERR) {
		dsx1_m32_errors_msg(err_code);
		net_cstate(hwidb, IDBS_ADMINDOWN);
	    } else {

	        /*
	         * Flush the hold queue.
	        */
	        holdq_reset(hwidb);

	        /*
	         * Show that we are being reset and change
	         * state to up, if we have a controller
	         * which is up. Make sure we dont muck
	         * with the b channels.
	         */
                if ((hwidb->state != IDBS_ADMINDOWN) &&
                    (!is_isdn(hwidb) ||
                    (hwidb->isdn_info->chantype == ISDN_Dchan))) {
                    cdbtype *cdb;
 
                    net_cstate(hwidb, IDBS_RESET);
                    cdb = controller_find(0, hwidb->unit);
                    if ((cdb->cdb_state == IDBS_UP) &&
                        (hwidb->state != IDBS_ADMINDOWN))
                        net_cstate(hwidb, IDBS_UP);
                }
            }	
	} else {
	    /*
	     * shut it down; should happen only if we have
	     * les than 4 meg mem in the box.
	     */
	    dsx1_m32_errors_msg(M32_ERR_MEM_ALLOC);
	    net_cstate(hwidb, IDBS_ADMINDOWN);
	}
    }
    else if (hwidb->pool) {
        pooltype *old_pool;

        old_pool = hwidb->pool;
        pool_destroy(old_pool);
        hwidb->pool = NULL;        

        /*
         * Deregister the d channel idb in the modem management subsystem
         * for call switching purpose.
         * Cannot use is_isdn() 'cause it's already freed when this routine
         * is called.
         */
        if (channel == dsx1->isdn_d_chan)
            reg_invoke_dsx1_delete_signaling_interface(hwidb);
    }

    /*
     * if there is no idle channel try to create one.
     */
    if (m32_ds->idle_channel == M32_IDLE_CHNL_UNASSIGNED) {
        dsx1_create_idle_channel(dsx1);
    }

    reset_interrupt_level(int_level);
}


/*
 * =====================================================================
 * dsx1_init()
 *
 * Description:
 * Initialize a dsx1 interface. This routine acts a a wrapper for
 * the dsx1_configure_channel() function which does the real init.
 * This routine allows just a hw idb to be passed in. It pulls out
 * the data needed to call dsx1_configure_channel(). 
 *
 * Parameters:
 * idb - the idb of the dsx1 channel 
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void dsx1_init (hwidbtype *hwidb, boolean maxdgram_change)
{
    dsx1info *dsx1;
    int bitfield, channel;
    ushort ix;
    leveltype status;
    pooltype *old_pool;
    
    channel = hwidb->vc;
    dsx1 = dsx1_list[hwidb->unit];

    status = raise_interrupt_level(NETS_DISABLE);

    /*
     * If the MTU has changed, Old pool has to be discarded.
     * Freeing functionality for old rx ring buffers
     * is done at M32 level. The new buffer pools are created
     * in dsx1_setup_buffer_pool.
     */
    if (hwidb->pool && maxdgram_change) {
      old_pool = hwidb->pool;
      pool_destroy(old_pool);
      hwidb->pool = NULL;
    }
     
    bitfield = 0;
    FOR_ALL_DSX1_TIMESLOTS(ix, dsx1) {
        if (dsx1->timeslots[ix] == channel) {
            bitfield |= make_bit_field(ix, ix);
        }
    }
 
    dsx1_configure_channel(dsx1, hwidb, channel, 
                           bitfield, dsx1->speed[channel]);

    /* For ISDN B channels, start out in a down state.  The
     * ISDN stack will inform us when a call comes up, and we
     * will up th channel at that time.
     */
    if ((IS_ISDN(hwidb)) && (hwidb->isdn_info->chantype != ISDN_Dchan) &&
      (IS_PRI(hwidb))){
        m32_instance_t *m32_ds;
        m32_ds = M32_INSTANCE(hwidb->slot, hwidb->subunit);
        m32_down_channel(m32_ds, channel);
     }

    reset_interrupt_level(status);
}


/*
 * =====================================================================
 * dsx1_restart()
 *
 * Description:
 * Restart a dsx1 interface
 *
 * Parameters:
 * idb - the idb for the dsx1 channel; in case of PRI do it to only
 * the dchannel. 
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void dsx1_restart (hwidbtype *hwidb)
{
    cdbtype   *cdb;
    dsx1info  *dsx1;
    hwidbtype *bchan_idb;
    int        channel;

    dsx1_init(hwidb, FALSE);

    /*
     * Make sure we do a cstate for only for the D channel, if
     * PRI is configured.
     */
    if ((is_isdn(hwidb)) && (hwidb->isdn_info->chantype != ISDN_Dchan))
	 return;

    /*
     * Find the cdb and check its state before transitioning
     * idb state.
     */
    cdb = controller_find(0, hwidb->unit);
    if ((cdb->cdb_state == IDBS_UP) && (hwidb->state != IDBS_ADMINDOWN))
	net_cstate(hwidb, IDBS_UP);

    /*
     * The following section only executes when hwidb is a D channel's idb.
     */
    if (!(is_isdn(hwidb)) || (hwidb->isdn_info->chantype != ISDN_Dchan))
         return;

    if (hwidb->state == IDBS_UP) {
        /*
         * If this is a D channel, we need to bring down all of B channels
         * associated with this D channel.
         */
        dsx1 = dsx1_find_by_unit(hwidb->unit);
        FOR_ALL_DSX1_CHANNELS(channel, dsx1) {
            bchan_idb = dsx1->hwidb[channel];
            if (is_isdn(bchan_idb) &&
                (bchan_idb->isdn_info->chantype != ISDN_Dchan) &&
                (bchan_idb->state == IDBS_UP))
                reset_if(bchan_idb);
        }
        /*
         * Register the d channel idb in the modem management subsystem
         * for call switching purpose.
         */
        reg_invoke_dsx1_add_signaling_interface(hwidb);
    }    
    else {
        /*
         * Deregister the d channel idb in the modem management subsystem
         * for call switching purpose. The modem call only use one
         * time slot.
         */
        reg_invoke_dsx1_delete_signaling_interface(hwidb);
    }
}

/*
 * =====================================================================
 * dsx1_set_transmit_delay
 *
 * Description:
 * The hdlc flags sent between frames is set through this function.
 *
 * Parameters:
 * hwidb - hardware idb pointer
 * value - the number of flags between frames
 *
 * Returns:
 * void
 * =====================================================================
 */
static void dsx1_set_transmit_delay (hwidbtype *hwidb, int value)
{
    dsx1info *dsx1;
    int channel;
    hwidbtype *idb;
 
 
    if (value < 0)
        value = 0;
    else if (value > M32_XMIT_DESC_FNUM_MAX)
        value = M32_XMIT_DESC_FNUM_MAX;
 
 
    dsx1 = dsx1_find_by_unit(hwidb->unit);
 
    if (is_isdn(hwidb)) {
        FOR_ALL_DSX1_CHANNELS(channel, dsx1) {
            idb = dsx1->hwidb[channel];
            if (is_isdn(idb)) {
                idb->xmitdelay = value;
                dsx1_restart(idb);
            }
        }
    }
    else {
        hwidb->xmitdelay = value;
        dsx1_restart(hwidb);
    }
}


/*
 * =====================================================================
 * dsx1_set_maxdgram()
 *
 * Description:
 * Called when the MTU for this interface changes. Adjust
 * the cache and buffer pool if needed and re-initialize
 * the interface.
 *
 * Parameters:
 * idb - the idb of the dsx1 channel 
 *
 * Returns:
 * boolean value used to tell if the operation was successful
 * =====================================================================
 */
static boolean dsx1_set_maxdgram (hwidbtype *hwidb, 
				  int buffersize, 
				  int maxdgram)
{
    if (hwidb->max_buffer_size != maxdgram) {
	if_maxdgram_adjust(hwidb, maxdgram);
        dsx1_init(hwidb, TRUE);
    }
    return(TRUE);
}


/*
 * =====================================================================
 * dsx1_init_idb_common()
 *
 * Description:
 * Initialize a dsx1 idb and put it on the system idb queue.
 *
 * Parameters:
 * cdb - pointer to the controller data block for this controller 
 * hwidb - ptr to the idb to be initialized
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void dsx1_init_idb_common (cdbtype *cdb, 
				  hwidbtype *idb)
{
    idbtype *swidb = idb->firstsw;
 
    idb->status = IDB_SERIAL|IDB_BCAST|IDB_HEARSELF|IDB_BRIDGE|IDB_SYNC;
    idb->type = dsx1_get_idbtype(cdb);
    idb->typestring = "DSX1";
    idb->state = IDBS_INIT;
    GET_TIMESTAMP(idb->state_time);
    idb->error_threshold = 5000;
    idb->error_count = 0;
    idb->fast_switch_flag = 0;
    idb->serial_flags |= SERIAL_FLAGS_CAN_ALT_FLAG;
    idb->serial_flags &= ~SERIAL_FLAGS_NO_CLOCKRATE;
 
    /*
     * Make sure we have a default mac addr in the idb.
     */
    ieee_copy(default_mac_addr, idb->hardware);
    ieee_copy(default_mac_addr, idb->bia);

    serial_idb_init(idb, MAXSERIALSIZE, PAN_MAXDGRAM,
                    VISIBLE_LES_SERIAL_BANDWIDTH, LES_SERIAL_DELAY);

    /*
     * Do the magic to support PRI dialer encaps
     */
    idb->setencap = dsx1_encapsulation;
    
    /*
     * Set up function calls
     */
    idb->soutput = m32_tx_start;
    idb->reset = dsx1_restart;
    idb->shutdown = dsx1_shutdown;
    idb->enable = dsx1_enable;
    idb->setup_bridging = NULL;
    idb->device_periodic = NULL;
    swidb->span_disabled = FALSE;
    idb->txqlength = dsx1_txqlength;
    idb->show_controller = dsx1_show_controller;
    idb->show_serial_state = dsx1_show_serial_state;
    idb->fastsend = m32_fastsend;
    idb->xmitdelay = 0;
 
    reg_add_media_transmit_delay(idb->type,
                                 dsx1_set_transmit_delay,
                                 "dsx1_set_transmit_delay");
    /*
     * Set up how arping is done.  This isn't used for
     * serial links but is here for consistency.
     */
    swidb->arp_arpa = swidb->arp_arpa_def = FALSE;
    swidb->arp_probe = swidb->arp_probe_def = FALSE;
    swidb->arp_snap = swidb->arp_snap_def = FALSE;
 
    /*
     * Buffer pool stuff
     */
    idb->pool = NULL;
    idb->pool_group = POOL_GROUP_INVALID;
   
    /*
     * idb->set_maxdgram must be initialized after calling serial_idb_init()
     * so that hdlc_setup does not attempt to call dsx1_set_maxdgram()
     * while doing system init
     */
    idb->set_maxdgram = dsx1_set_maxdgram;
 
    idb->reliability = 255;
    idb->load = 1;
    idb->rxload = 1;

    serial_idb_enqueue(idb);
    idb_enqueue(idb);
}


/*
 * ****************************************************************
 * The following routines are for dsx1 idb manipulation and channel
 * configuration.
 * ****************************************************************
 */
/*
 * =====================================================================
 * dsx1_create_idb()
 *
 * Description:
 * Create an idb for a dsx1 interface. Try to get an unused idb struct
 * off of the dsx1 idb queue first. If the queue is empty allocate
 * a new idb. Fill in the idb and enqueue it on the system idb queue.
 *
 * Parameters:
 * cdb - pointer to the controller data block for this controller 
 * hwidb - ptr to an existing idb, should always be null for low end
 * channel - virtual interface number
 *
 * Returns:
 * pointer to new hwidb
 *
 * Notes:
 * the hwidb parameter should always be null for access routers.
 * it is included for compatibility with high end code.
 * =====================================================================
 */
static hwidbtype *dsx1_create_idb (cdbtype *cdb, 
				   hwidbtype *hwidb, 
				   int channel)
{
    idbtype *swidb;

    /*
     * if a hwidb is passed in then just return
     */
    if (hwidb) {
        return(hwidb);
    }

    /*
     * get an idb off of the idb queue, if the queue is
     * empty, allocate a new one.
     */
    hwidb = dequeue(&dsx1_idbQ);
    if (hwidb == NULL) {
        hwidb = idb_create();
        if (hwidb == NULL) {
            return(NULL);
        }
    } else {
        /*
         * Since there's no standard way to free an IDB, we need to
         * reuse the freed IDB in our private queue. And also there's
         * no standard way to clean the IDB, we need to bzero out the
         * IDB section to ensure that there's no old information left
         * in the reused IDB.
         *
         * we got an idb struct off of the idb queue, zero it out
         */
	idb_clear_dangerous(hwidb);
        idb_init_all(hwidb, hwidb->firstsw);
    }
    
    /*
     * fill in the idb 
     */
    hwidb->slot = cdb->slot;           /* save our NIM slot number */
    hwidb->unit = cdb->unit;           /* unit number */
    hwidb->subcont = cdb->subunit;     /* subunit number */
    hwidb->slotunit = cdb->appl_no;
    hwidb->nim_type = nim_get_type(hwidb->slot);
    hwidb->nim_version = nim_get_version(hwidb->slot);
    hwidb->vc = channel;
 
    dsx1_init_idb_common(cdb, hwidb);

    /*
     * adjust the global interface counters
     */

    nserialnets++;
    nnets++;

    hwidb->state = IDBS_DOWN;

    /*
     * This idb is special...it is created after all i/fs
     * are inited. So do all special magic here.
     */
    idb_init_names(hwidb->firstsw, FALSE);
    hwidb->status |= IDB_VIRTUAL_HW;
    idb_final_hw_init(hwidb);
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb)
	idb_final_sw_init(swidb);

    /*
     * These are essential for all new idbs in a system.
     * This assumes a successful idb enqueue.
     */
    swidb = hwidb->firstsw;
    hwidb->input_qcount = 0;
    hwidb->input_defqcount = INPUTQ_DEFAULT;
    hwidb->keep_period = KEEPALIVE_DEFAULT;
    hwidb->keep_count = 0;

    /*
     * Initialize protocol support in the swidb including hardware
     * data structures (TRUE).
     */
    reg_invoke_sw_idb_init(swidb, TRUE);
    reg_invoke_hw_idb_init(hwidb);
    reg_invoke_fast_setup(hwidb);
    reg_invoke_ifmib_create_stacklink(hwidb->snmp_if_index,
				       cdb->snmp_if_index);
    return(hwidb);
}


/*
 * =====================================================================
 * dsx1_delete_idb()
 *
 * Description:
 * Delete an idb for a dsx1 interface. Dequeue it from the idb
 * system queue and enqueue it onto the dsx1 idb queue.
 *
 * Parameters:
 * cdb - pointer to the controller data block for this controller 
 * hwidb - ptr to the idb to be deleted
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void dsx1_delete_idb (cdbtype *cdb, 
			     hwidbtype *hwidb)
{
    leveltype SR;

    if (!delete_interface(hwidb->firstsw)) {
	errmsg(&msgsym(NOMEMORY, SYS), "idb structure for controller",
	       hwidb->unit);
    }

    /*
     * take the idb off of the idb system queue, then put it
     * onto the dsx1 idb queue so it can be reused the next time
     * an interface is created. Ensure exclusion.
     */
    SR = raise_interrupt_level(NETS_DISABLE);
    serial_idb_dequeue(hwidb);
    idb_unlink(hwidb);
    reset_interrupt_level(SR);
    
    /*
     * Discard private pool.
     */
    if (hwidb->pool) {
        pool_destroy(hwidb->pool);
	hwidb->pool = NULL;
    }

    /*
     * deregister idb with snmp ifTable
     */
    reg_invoke_ifmib_destroy_stacklink(hwidb->snmp_if_index,
				       cdb->snmp_if_index);
    reg_invoke_ifmib_deregister_hwidb(hwidb);

    /*
     * Reuse idb entry.
     */
    enqueue(&dsx1_idbQ, hwidb);

    /*
     * adjust the global interface counters
     */
    if (nserialnets)
        nserialnets--;
    if (nnets)
        nnets--;
}

/*
 * =====================================================================
 * dsx1_create_idle_channel -
 *
 * Description:
 * Create an idle channel for this interface. This allows all
 * unused timeslots to transmit idle flags.
 *
 * Parameters:
 * dsx1 - pointer to dsx1 i/f specific struct
 * 
 * Returns: nothing.
 * =====================================================================
 */
void dsx1_create_idle_channel (dsx1info *dsx1)
{
    cdbtype *cdb;
    m32_instance_t *m32_ds; 
    ulong err_code = 0;

    cdb = dsx1->cdb;
    if (cdb->cdb_state == IDBS_UP) {
        m32_ds = M32_INSTANCE(cdb->slot, cdb->subunit);
        err_code = m32_create_idle_channel(m32_ds);

        if (err_code != M32_NOERR)
	    dsx1_m32_errors_msg(err_code);
    }
}

/*
 * *******************************************************************
 * cardifmib_get_card_index -
 *
 * Description:
 * registry fn to get card index.
 *
 * Parameters:
 * cdb ptr
 *
 * return int
 * *******************************************************************
 */
int cardifmib_get_card_index (cdbtype *cdb)
{
   int parent_card_index;
   parent_card_index = reg_invoke_chassismib_find_slot_card_index(cdb->slot);
   if (parent_card_index < 0)
      return (-1);
   return(parent_card_index);
}

/*
 * *******************************************************************
 * cardifmib_get_slot_number-
 *
 * Description:
 * registry fn to get slot number.
 *
 * Parameters:
 * cdb ptr
 *
 * return long
 * *******************************************************************
 */
long cardifmib_get_slot_number (cdbtype *cdb)
{
   return (cdb->slot);
}

/*
 * *******************************************************************
 * cardifmib_get_port_number-
 *
 * Description:
 * registry fn to get port number.
 *
 * Parameters:
 * cdb ptr
 *
 * return long
 * *******************************************************************
 */
long cardifmib_get_port_number (cdbtype *cdb)

{
   return (cdb->appl_no);
}
/*
 * *******************************************************************
 * cardifmib_get_connector_type-
 *
 * Description:
 * registry fn to get connector_type.
 *
 * Parameters:
 * cdb ptr
 *
 * return long
 * *******************************************************************
 */
long cardifmib_get_connector_type (cdbtype *cdb)
{
   return (D_cardIfConnectorTypeEnabled_not_specified);
}

/*
 * =====================================================================
 * dsx1_init_context -
 *
 * Description:
 * Initialize the controller and dsx1 structures.
 *
 * Parameters:
 * adapter_type, media_type, applique_type, slot, subcont, total_ctrlr_unit.
 * 
 * Returns: nothing.
 * =====================================================================
 */
dsx1info *dsx1_init_context (int adapter_type,	/* Port adaptor type */
			     int media_type,    /* Media ? */
			     int applique_type, /* applique? E1 or T1 */
			     int slot,		/* slot # */
			     int subcont,       /* subcont, in case more than
					           1 controller per slot */
                             int total_ctrlr_unit)  /* total number of controller units */
{
    static firsttime = TRUE;
    cdbtype *cdb;
    dsx1info *dsx1 = NULL;
    m32_instance_t *m32_ds = M32_INSTANCE(slot, subcont);
    ushort ix;

    /*
     * Init the dsx1 registries, stubs, controller and global dsx1 instance
     * once.
     */
    if (firsttime) {
	init_dsx1_subsystem(total_ctrlr_unit);
	reg_add_dsx1_create_idb(dsx1_create_idb, "dsx1_create_idb");
	reg_add_dsx1_delete_idb(dsx1_delete_idb, "dsx1_delete_idb");
	reg_add_dsx1_map_channel(dsx1_configure_channel,
				 "dsx1_configure_channel");
        reg_add_digital_call_report(dsx1_digital_call_report,
                                    "dsx1_digital_call_report");
	/*
	 * create the queue for discarded but reusable idb structures.
	 */ 
	queue_init(&dsx1_idbQ, 0);
	
	/*
	 * One time controller structure creation.
	 * Should be moved to 4000/4500 startup routines, in case
	 * any other nim needs the controller.
	 */
	controller_init(firsttime, 0 /* we go by unit #s */,
			total_ctrlr_unit);
	firsttime = FALSE;

	/*
	 * We are here because there is at least one dsx1 card.
	 * So, init the framer background process right here.
	 */
	dsx1_handler();
    }

    /*
     * Now create the individual context for this dsx1 controller.
     */
    cdb = controller_create(adapter_type, media_type, slot);
    if (cdb) {
        cdb->subunit = subcont;
	cdb->cdb_status = CDB_CONFIGURE;
	cdb->idbtype = IDB_SERIAL;
        cdb->reset   = dsx1_controller_reset;
        cdb->clear_counts = dsx1_clear_counts;
	
	/* this can be dumped when we support an ifIndex for the T1 card */
	cdb->hw_if_index = snmp_dsx1LineIndex_seed;
	snmp_dsx1LineIndex_seed+=2; /* advance to the next valid index */
        /*
	 * Create a dsx1 context for each applique
	 */
        dsx1_create_context(cdb, applique_type, TRUE);
        dsx1 = dsx1_list[cdb->unit];
        /*
         * setup all usable timeslots so they will transmit 
         * flags when they are not configured. don't mess with 
         * timeslot zero for E1. this is the framing timeslot.
         */
        for (ix = 1; ix <= dsx1->max_timeslots; ix++) {
	    dsx1->pool_group[ix-1] = POOL_GROUP_INVALID;
            if (adapter_type == MCN_PA_T1) {
                m32_ds->idle_timeslots |= (1 << (ix-1));
            } else {
                m32_ds->idle_timeslots |= (1 << ix);
            }
        }
    } else {
	crashdump(0);
    }

    /*
     * Increment the appropriate globals
     */
    switch(adapter_type) {
      case MCN_PA_E1:
	ne1ports++;
	break;
	
      case MCN_PA_T1:
	nt1ports++;
	break;

      default:
    }
    return(dsx1);
}


static void dsx1_set_rxoffset (hwidbtype *idb)
{
    dsx1_init(idb, FALSE);
}

/*
 * =====================================================================
 * dsx1_subsys_init -
 *
 * Description: DSX1 subsystem initialisation entry point.
 *
 * Parameters:
 * subsys    : ptr to subsystem type.
 *
 * Returns:
 * nothing.
 * 
 * =====================================================================
 */
static void dsx1_subsys_init (subsystype *subsys)
{
    /*
     * Register the NIM ID for the MCN base card
     */
    reg_add_analyze_mcn_port_adaptor(MCN_PA_E1,
				     ce1_analyze_mcn_port_adaptor,
				     "ce1_analyze_mcn_port_adaptor");
    reg_add_analyze_mcn_port_adaptor(MCN_PA_T1,
				     bt8360_analyze_mcn_port_adaptor,
				     "bt8360_analyze_mcn_port_adaptor");
    reg_add_media_set_rxoffset(IDBTYPE_T1, 
			       dsx1_set_rxoffset, 
			       "dsx1_set_rxoffset");
    reg_add_media_set_rxoffset(IDBTYPE_E1, 
			       dsx1_set_rxoffset, 
			       "dsx1_set_rxoffset");
}

/*
 * DSX1 subsystem header
 */
#define DSX1_MAJVERSION   1
#define DSX1_MINVERSION   0
#define DSX1_EDITVERSION  1

SUBSYS_HEADER(dsx1, DSX1_MAJVERSION, DSX1_MINVERSION, DSX1_EDITVERSION,
              dsx1_subsys_init, SUBSYS_CLASS_DRIVER,
              "seq: mcn",
              "req: mcn");
