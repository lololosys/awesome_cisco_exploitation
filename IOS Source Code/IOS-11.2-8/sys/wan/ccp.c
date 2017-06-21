/* $Id: ccp.c,v 3.6.32.17 1996/09/12 13:31:55 fox Exp $
 * $Source: /release/112/cvs/Xsys/wan/ccp.c,v $
 *------------------------------------------------------------------
 * ccp.c - PPP Compression Control Protocol.
 *
 * October 1994, George M. Taylor
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ccp.c,v $
 * Revision 3.6.32.17  1996/09/12  13:31:55  fox
 * CSCdi68759:  AAA per-user routes are not removed when connection drops
 * Branch: California_branch
 * Separate parallel connection support for per-user routes from the
 * support for neighbor routes and dynamic dialer maps.  Transfer
 * ownership of the static route to the last idb to add it (applies only
 * to per-user routes).
 *
 * Revision 3.6.32.16  1996/08/28  13:20:50  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.32.15  1996/08/18  09:07:35  fox
 * CSCdi58100:  PPP option types displayed inconsistently
 * Branch: California_branch
 * Consistently display PPP codes and types in decimal.  Convert several
 * occurrences of '0x%x' to '%#x'.
 *
 * Revision 3.6.32.14  1996/08/07  09:04:17  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.6.32.13  1996/08/03  23:41:05  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.6.32.12  1996/07/23  13:30:58  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.6.32.11  1996/07/01  20:47:16  fox
 * CSCdi61571:  Local Timeout at _ppp_getlink(0x303d730+0x1fff82)+0x160
 * Branch: California_branch
 * Virtual PPP interfaces may not be complete.  Sanity check existence
 * of NCP FSM pointer table.
 *
 * Revision 3.6.32.10  1996/06/24  09:26:52  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.6.32.9  1996/05/30  20:46:56  gtaylor
 * CSCdi58710:  Crash in doprnt, buginf, ccp_up. buginf being passed a NULL
 * string pointer.
 * Branch: California_branch
 *
 * Revision 3.6.32.8  1996/05/10  01:44:15  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.6.32.7  1996/05/06  16:46:00  gtaylor
 * CSCdi56464:  Maximum compressable packet size is maxmtu for interface
 * plus the PPP protocol header (2 bytes). Code wrongly assumed maxmtu
 * only.
 * Branch: California_branch
 *
 * Revision 3.6.32.6  1996/05/01  14:33:15  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Make PPP NCPs into real subsystems.  Add them to all images where
 * PPP and the related network protocol are located.  NCPs are dependant
 * on PPP and the related network protocol subsystem but not vice versa.
 * Remove dependancies between PPP and the compression code.
 *
 * Revision 3.6.32.5  1996/04/29  09:45:11  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.6.32.4  1996/04/27  06:35:35  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.6.32.3  1996/04/23  20:26:49  gtaylor
 * CSCdi41791:  Memory corruption crash. Workspace for PPP compression is
 * now based on current link max MTU size.
 * Branch: California_branch
 *
 * Revision 3.6.2.2  1996/04/25  23:57:38  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.6.2.1  1996/02/23  21:31:06  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.6.32.2  1996/04/11  17:32:38  turadek
 * CSCdi51869:  Router hangs when hdlc stacker serial is dis/ena repeatedly
 * Branch: California_branch
 * avoid decompression of zero-length packets
 *
 * Revision 3.6.32.1  1996/03/18  22:46:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.6.1  1996/03/07  11:06:44  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6  1996/01/11  19:21:26  gtaylor
 * CSCdi46753:  PPP Stacker compession should give priority to LCB check
 *              method.
 *
 * Revision 3.5  1995/11/23  00:54:22  gtaylor
 * CSCdi43498:  PPP Stacker compression requires support for LCB check
 * method. This fix should guarantee that we will be able to work with
 * Combinets PPP Stac compression and other vendors supporting the LCB
 * check method.
 *
 * Revision 3.4  1995/11/22  17:36:26  gtaylor
 * CSCdi42981:  PPP stac compression has buffer failures and high packet
 * loss. Caused by not freeing new buffer when decompression resulted in
 * error due to packet/history corruption.
 *
 * Revision 3.3  1995/11/17  18:00:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:49:03  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:47:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  21:35:01  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/10/13  22:27:45  gtaylor
 * CSCdi38706:  lapb wont come up. Caused by compression violation
 * maxmtu size. Compression checks now that if packet expands it
 * remains smaller than the MTU.
 *
 * Revision 2.7  1995/08/02  21:42:16  sgannu
 * CSCdi37788:  bridging does not work for flex box with ppp compression
 * When compression is negotiated set the span_bridge_process_force to
 * SPAN_BR_PROC_COMPRESSION in the case of lex interface.
 *
 * Revision 2.6  1995/07/14  07:18:51  misko
 * CSCdi36902:  PPP Fastswitching incorectly disabled via compression code
 * CSCdi35161 failed to take into account that if comp_db is defined,
 * that it caused fast switching to be disabled.  Since CSCdi35161 always
 * allocated comp_db when PPP was enabled, but it was never freed, this
 * causes all fast switching to be disabled for WAN encaps after PPP had
 * been configured once.  It also meant fast switching was always disabled
 * for PPP.  Make sure comp_db is only allocated when compression is
 * enabled, and free and clear it when compression is disabled or
 * encapsulation is changed.
 *
 * Revision 2.5  1995/06/29  17:22:05  gtaylor
 * CSCdi35161:  PPP compression should support the show compression stats
 * command.
 * Support for PPP compression stats added. ISDN B channel compression
 * stats
 * display also included.
 *
 * Revision 2.4  1995/06/20  20:37:58  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/18  06:19:27  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.2  1995/06/15 21:05:45  sgannu
 * CSCdi35730:  Compression does not work with lex fastswitching on.
 *
 * Revision 2.1  1995/06/07  23:16:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Point-to-Point Protocol.
 *
 * Copyright (c) 1989 Carnegie Mellon University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by Carnegie Mellon University.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "subsys.h"
#include "packet.h"
#include "config.h"
#include "logger.h"
#include "../if/network.h"
#include "ppp.h"
#include "ppp_registry.h"
#include "ncp.h"
#include "ccp.h"
#include "ppp_public.h"
#include "stac_comp.h"          /* Stacker include file */
#include "../x25/compress.h"    /* Predictor (Rand) include file */
#include "comp_sys.h"
#include "../parser/parser_defs_compress.h"
#include "../if/ether.h"

static process ccp_input(void);    /* Input queue manager */
static void ccp_enqueue(paktype *pak);    /* Interrupt level packet enqueue */
static void ccp_resetci(fsm *);           /* Reset our Configuration Information */
static int  ccp_cilen(fsm *);             /* Return length of our CI */
static void ccp_addci(fsm *, uchar *);         /* Add our CIs */
static int  ccp_ackci(fsm *, uchar *, int);    /* Ack some CIs */
static void ccp_alternatives(fsm *f, uchar rejtype); /* Offer altern comp type */
static void ccp_nakci(fsm *, uchar *, int);    /* Nak some CIs */
static void ccp_rejci(fsm *, uchar *, int);    /* Reject some CIs */
static uchar ccp_reqci(fsm *, uchar *, short *);      /* Check the requested CIs */
static void ccp_up(fsm *);        /* Link came up */
static void ccp_down(fsm *);      /* Link going down */
static void ccp_closed(fsm *);    /* Link has closed */
static boolean ccp_resetreq(fsm *, uchar *, int);    /* Reset Request received */
static void ccp_resetack(fsm *, uchar, uchar *, int);    /* Reset Acknowledge received */
static void ccp_coderej(fsm *f); /* Code Rejected received */
static void ccp_compression_request_reset(hwidbtype *, uchar, uchar *, int); 
static boolean ccp_decompress_stacker(paktype **pak);   /* ppp IF to stacker compression */
static boolean ccp_compress_stacker(paktype *pak);      /* ppp IF to stacker decompression */
static void ccp_decompress_stacker_failure(hwidbtype *idb); /* Deal with decomp err */
static boolean ccp_decompress_lzs_dcp(paktype **pak);   /* ppp IF to stacker DCP header compression */
static boolean ccp_compress_lzs_dcp(paktype *pak);      /* ppp IF to stacker DCP header decompression */
static void ccp_send_request_reset_lzs_dcp(hwidbtype *hwidb); /* Reset Request for DCP*/
static boolean ccp_compress_predictor1(paktype *pak);   /* ppp IF to Predictor1 comp */
static void ccp_decompress_predictor1_failure(hwidbtype *idb); /* Deal with decomp err */
static boolean ccp_decompress_predictor1(paktype **pak);/* ppp IF to Predictor1 decomp */
static void ccp_compression_legacy_restart (hwidbtype *idb); /* Legacy sync method */


static fsm_callbacks ccp_callbacks = { /* CCP callback routines */
    ccp_resetci,     /* Reset our Configuration Information */
    ccp_cilen,       /* Length of our Configuration Information */
    ccp_addci,       /* Add our Configuration Information */
    ccp_ackci,       /* ACK our Configuration Information */
    ccp_nakci,       /* NAK our Configuration Information */
    ccp_rejci,       /* Reject our Configuration Information */
    ccp_reqci,       /* Request peer's Configuration Information */
    ccp_up,          /* Called when fsm reaches OPEN state */
    ccp_down,        /* Called when fsm leaves OPEN state */
    ccp_closed,      /* Called when fsm reaches CLOSED state */
    NULL,            /* Called when Protocol-Reject received */
    NULL,            /* Retransmission is necessary */
    ccp_resetreq,    /* Reset Requested by peer */
    ccp_resetack,    /* Reset Acknowledge by peer */
    ccp_coderej         /* Code Rejected by peer */
};

static pid_t ccp_pid;               /* Process ID of CCP process */
static watched_queue *CCPpacketQ;

static char *const stac_checkmode [] = {"NONE","LCB","CRC","SEQUENCED",
                                        "EXTENDED", "UNKNOWN"};
static char *const dcp_checkmode [] = {"NONE","LCB","SEQ","SEQ & LCB",
                                       "UNKNOWN"};
static char *const dcp_processmode [] = {"NONE","PROCESS","UNKOWN"};

static char *const scompressor [] = { "Proprietary",
                                      "Predictor 1",
                                      "Predictor 2",
                                      "Puddle Jumper",
                                      "Unassigned(4)",
                                      "Unassigned(5)",
                                      "Unassigned(6)",
                                      "Unassigned(7)",
                                      "Unassigned(8)",
                                      "Unassigned(9)",
                                      "Unassigned(10)",
                                      "Unassigned(11)",
                                      "Unassigned(12)",
                                      "Unassigned(13)",
                                      "Unassigned(14)",
                                      "Unassigned(15)",
                                      "Unassigned(16)",
                                      "Stacker",
                                      "MicroSoft PPC",
                                      "Gandalf FZA",
                                      "V.42bis",
                                      "BSD LZW",
                                      "Undefined(22)",
                                      "LZS-DCP",
                                      "Unknown"};

/*
 * check_compression_type
 * Return pointer to static compression type string
 */
static char *check_compression_type (uchar type)
{
    if (type > CI_MAX_COMPRESSION_TYPES)
        type = CI_MAX_COMPRESSION_TYPES;
    return (scompressor[type]);
}


/*
 * stac_check_mode_name
 * Return pointer to static check mode string
 */
static char *stac_check_mode_name (uchar check_mode)
{
    if (check_mode > LZS_CHECK_MODE_EXTND)
        check_mode = LZS_CHECK_MODE_EXTND + 1;
    return (stac_checkmode[check_mode]);
}


/*
 * dcp_check_mode_name
 * Return pointer to static check mode string
 */
static char *dcp_check_mode_name (uchar check_mode)
{
    if (check_mode > LZSDCP_CHECK_MODE_SEQ_LCB)
        check_mode = LZSDCP_CHECK_MODE_SEQ_LCB + 1;
    return (dcp_checkmode[check_mode]);
}


/*
 * dcp_check_process_name
 * Return pointer to static check mode string
 */
static char *dcp_check_process_name (uchar process_mode)
{
    if (process_mode > LZSDCP_PROCESS_MODE_PROC)
        process_mode = LZSDCP_PROCESS_MODE_PROC + 1;
    return (dcp_processmode[process_mode]);
}


/*
 * ccp_init - Initialize CCP.
 */
static void ccp_init (fsm *ccp_fsm)
{

    if (!ccp_fsm->wantoptions)
        ccp_fsm->wantoptions = malloc(sizeof(ccp_options));
    else
        memset(ccp_fsm->wantoptions, 0, sizeof(ccp_options));
    if (!ccp_fsm->wantoptions)
        return;
  
    if (!ccp_fsm->allowoptions)
        ccp_fsm->allowoptions = malloc(sizeof(ccp_options));
    else
        memset(ccp_fsm->allowoptions, 0, sizeof(ccp_options));
    if (!ccp_fsm->allowoptions)
        return;
  
    if (!ccp_fsm->heroptions)
        ccp_fsm->heroptions = malloc(sizeof(ccp_options));
    else
        memset(ccp_fsm->heroptions, 0, sizeof(ccp_options));
    if (!ccp_fsm->heroptions)
        return;
  
    if (!ccp_fsm->gotoptions)
        ccp_fsm->gotoptions = malloc(sizeof(ccp_options));
    else
        memset(ccp_fsm->gotoptions, 0, sizeof(ccp_options));
    if (!ccp_fsm->gotoptions)
        return;
  
    if (!ccp_fsm->ouroptions)
        ccp_fsm->ouroptions = malloc(sizeof(ccp_options));
    else
        memset(ccp_fsm->ouroptions, 0, sizeof(ccp_options));
    if (!ccp_fsm->ouroptions)
        return;
  
    ccp_fsm->retransmits = 0;
    TIMER_STOP(ccp_fsm->timeouttime);
    ccp_fsm->failures = 0;
    ccp_fsm->callbacks = &ccp_callbacks;
    ccp_fsm->flags |= AOPENDING;    /* Active Open */
  
    /*
     * Compression needs it's own incoming queue handler
     */
    if (ccp_pid == NO_PROCESS){
        ccp_pid = process_create(ccp_input, "CCP manager", NORMAL_STACK,
                                 PRIO_HIGH);
    }
}

/*
 * ccp_input
 * This pulls packets of it's queue and decompresses them. The uncompressed
 * packets are then re-enqueued on the correct protocol input queue.
 */
static process ccp_input (void)
{
    void (*siqueue)(hwidbtype *, paktype *);
    hwidbtype *hwidb;
    int protocol, pak_count;
    paktype *pak;
    leveltype status;
    int pak_size;
  
    process_wait_on_system_init();

    /*
     * Set up this process' world.
     */
    CCPpacketQ = create_watched_queue("CCP input", 0, 0);
    process_watch_queue(CCPpacketQ, ENABLE, RECURRING);
    reg_add_raw_enqueue(LINK_PPP_COMPRESSION, ccp_enqueue, "ccp_enqueue");

    while (TRUE) {
        process_wait_for_event();

        pak_count = PAK_SWITCHCOUNT;

        while (TRUE) {
            /*
             * Process only a limited number of packets per pass
             */
            if (pak_count-- == 0) {
                break;
            }

            pak = process_dequeue(CCPpacketQ);
            if (pak == NULL) {
                break;
            }

            hwidb = pak->if_input->hwptr;
            protocol = ppp_get_protocol_type(pak);

            /*
             * Are we about to decompress that which should be decompressed?
             */
            if (protocol != TYPE_PPP_COMPRESS){
                datagram_done(pak);
                if (ppp_error_debug) {
                    buginf("\n%s:CCP: dropped, Protocol = %#x", 
                            hwidb->hw_namestring, protocol);
                }
                continue;
            }

            /*
             * Check for a non-NULL decompression vector. NULL vector indicates
             * decompression has not been negotiated or has reached the CLOSED
             * state.
             */
            if (hwidb->ppp_decompression){
                pak->datagramstart += pak->encsize;
                pak->datagramsize -= pak->encsize;
                pak->encsize = 0;
                if (!(*hwidb->ppp_decompression)(&pak)){
                    pak->flags |= PAK_ABORT;
                    datagram_done(pak);
                    continue;
                }
            }else{
                if (ppp_error_debug)
                    buginf("\n%s:CCP: got compressed pkt; CCP not OPEN",
                            hwidb->hw_namestring);
                pak->flags |= PAK_ABORT;
                datagram_done(pak);
                continue;
            }

            siqueue = ppp_parse_packet(hwidb, pak, NULL);
            /*
             * If fast switching is enabled on this interface, try to fast
             * switch when the link type is IP only
             */
            pak->linktype = (*hwidb->getlink)(pak);
            pak_size = pak->datagramsize;
            switch (pak->linktype){
            case LINK_IP:
                status = raise_interrupt_level(NETS_DISABLE);
                if (reg_invoke_ppp_from_process_fs(RXTYPE_DODIP, pak)){
                    hwidb->counters.inputs_fast[ACCT_PROTO_IP]--;
                    hwidb->counters.rx_cumbytes_fast[ACCT_PROTO_IP] -= pak_size;
                    reset_interrupt_level(status);
                    continue;
                }
                reset_interrupt_level(status);
            break;

            case LINK_NOVELL:
                status = raise_interrupt_level(NETS_DISABLE);    
                if (reg_invoke_ppp_from_process_fs(RXTYPE_NOVELL1, pak)){
                    hwidb->counters.inputs_fast[ACCT_PROTO_NOVELL]--;
                    hwidb->counters.rx_cumbytes_fast[ACCT_PROTO_NOVELL] -= 
                        pak_size;
                    reset_interrupt_level(status);
                    continue;
                }
                reset_interrupt_level(status);
            break;
            }
    
            if (siqueue)
                (*siqueue)(hwidb, pak);
            else{
                if (ppp_error_debug)
                    buginf("\n%s:CCP: Failed to forward packet to any"
                            " protocol queue", hwidb->hw_namestring);
                datagram_done(pak);
            }
        }
    }
}

/*
 * ccp_enqueue
 * Interrupt level packet enqueue
 */
static void ccp_enqueue (paktype *pak)
{
    fsm *f;

    f = ppp_serial2fsm_inline(TYPE_PPP_CCP, pak->if_input->hwptr);
 
    if (!f || f->state != OPEN) {
        protocol_discard(pak, TRUE);
    } else {
        process_enqueue_pak(CCPpacketQ, pak);
    }
}

/*
 * ccp_resetci - Reset our CI.
 */
static void ccp_resetci (fsm *f)
{
    ccp_options *wo;
    ccp_options *ao;
    ccp_options *go;
    ccp_options *oo;
    hwidbtype *idb;
  
    wo = f->wantoptions;
    ao = f->allowoptions;
    go = f->gotoptions;
    oo = f->ouroptions;
  
    idb = f->idb;
  
    idb->ppp_compression = NULL;
    idb->ppp_decompression = NULL;

    if (idb->comp_db)
        idb->comp_db->comp_configed = COMP_NOT_ENABLED;

    /*
     * wantoptions or "wo" is what the user has configured as the prefered
     * compression type for outgoing packets.
     *
     * allowoptions or "ao" is what we will allow remote hosts to ask for.
     *
     * heroptions or "ho" is what was negotiated by a remote host and we
     * have agreed to.
     *
     * gotoptions or "go" are the options we have negotiated for and the
     * remote host has agreed on.
     *
     * ouroptions or "oo" are the alternative choices we have not tried to
     * negotiate for yet. In the event that our current choice is
     * rejected we consult this data structure for our next choice of
     * options available.
     */

    wo->lzs.negotiate = CCP_NON_NEGOTIABLE_COMPRESSION;
    wo->lzsdcp.negotiate = CCP_NON_NEGOTIABLE_COMPRESSION;
    wo->pred1.negotiate = CCP_NON_NEGOTIABLE_COMPRESSION;

    /*
     * Setup the negotiable parameters for the compression type choosen as
     * preference.
     */
    if (wo->negotiate_compression){
        switch(wo->user_preference){
            case CI_STACKER: /* Stacker Options */
                wo->lzs.negotiate = CCP_NEGOTIABLE_COMPRESSION;
                wo->lzs.type = CI_STACKER;
                wo->lzs.length = LEN_CI_LZS;
                wo->lzs.history_count = LZS_MAX_HISTORIES;
                /*
                 * If we don't have a reliable link then we need to use a
                 * check method on packets to trap out of sequence packets
                 * corrupting the compression history
                 */
                if (idb->ppp_lapb)
                    wo->lzs.check_mode = LZS_CHECK_MODE_NONE;
                else
                    wo->lzs.check_mode = LZS_CHECK_MODE_LCB;
                if (idb->comp_db)
                    idb->comp_db->comp_configed = COMP_STAC;
            break;

            case CI_LZSDCP: /* LZS DCP Options */
                wo->lzsdcp.negotiate = CCP_NEGOTIABLE_COMPRESSION;
                wo->lzsdcp.type = CI_LZSDCP;
                wo->lzsdcp.length = LEN_CI_LZSDCP;
                wo->lzsdcp.history_count = LZSDCP_MAX_HISTORIES;
                wo->lzsdcp.process_mode = LZSDCP_PROCESS_MODE_PROC;
                /*
                 * If we don't have a reliable link then we need to use a
                 * check method on packets to trap out of sequence packets
                 * corrupting the compression history
                 */
                if (idb->ppp_lapb)
                    wo->lzsdcp.check_mode = LZSDCP_CHECK_MODE_NONE;
                else
                    wo->lzsdcp.check_mode = LZSDCP_CHECK_MODE_SEQ;

                if (idb->comp_db)
                   idb->comp_db->comp_configed = COMP_STAC;
            break;
  
    
            case CI_PREDICTOR1: /* Predictor 1 Options */
                wo->pred1.negotiate = CCP_NEGOTIABLE_COMPRESSION;
                wo->pred1.type = CI_PREDICTOR1;
                wo->pred1.length = LEN_CI_PRED1;
                if (idb->comp_db)
                    idb->comp_db->comp_configed = COMP_RAND;
            break;
        }
    }
  
    /* Clear out the negotiated compression variable */
    wo->negotiated_type = CI_COMPRESSION_NONE;
  
    *go = *wo;    /* Structure Copy */

    /* We are allowing the other side to ask for Stacker */
    ao->lzs.negotiate = CCP_NEGOTIABLE_COMPRESSION;
    ao->lzs.type = CI_STACKER;
    ao->lzs.length = LEN_CI_LZS;
    ao->lzs.history_count = LZS_MAX_HISTORIES;
    /*
     * If no reliable link in place then we will insist on a check
     * method.
     */
    if (idb->ppp_lapb)
        ao->lzs.check_mode = LZS_CHECK_MODE_NONE;
    else
        ao->lzs.check_mode = LZS_CHECK_MODE_LCB;

    /* We are allowing the other side to ask for LZS DCP */
    ao->lzsdcp.negotiate = CCP_NEGOTIABLE_COMPRESSION;
    ao->lzsdcp.type = CI_LZSDCP;
    ao->lzsdcp.length = LEN_CI_LZSDCP;
    ao->lzsdcp.history_count = LZSDCP_MAX_HISTORIES;
    /*
     * If no reliable link is in place then we will insist on a check
     * method.
     */
    if (idb->ppp_lapb)
        ao->lzsdcp.check_mode = LZSDCP_CHECK_MODE_NONE;
    else
        ao->lzsdcp.check_mode = LZSDCP_CHECK_MODE_SEQ;

    /* We are allowing the other side to ask for Predictor */
    ao->pred1.negotiate = CCP_NEGOTIABLE_COMPRESSION;
    ao->pred1.type = CI_PREDICTOR1;
    ao->pred1.length = LEN_CI_PRED1;

    /* Clear out the negotiated compression variable */
    ao->negotiated_type = CI_COMPRESSION_NONE;

    /*
     * Our own alternative options for negotiation are the same as the list of
     * negotiatable options we offer the remote host.
     */
    *oo = *ao;    /* Structure Copy */
}

/*
 * ccp_cilen - Return length of our CI.
 */
static int ccp_cilen (fsm *f)
{
    ccp_options *go;
  
    go = f->gotoptions;
  
    return(LENCISTACKER(go->lzs.negotiate)
           + LENCILZSDCP(go->lzsdcp.negotiate)
           + LENCIPRED1(go->pred1.negotiate));
}

/*
 * ccp_addci - Add our desired CIs to a packet.
 */
static void ccp_addci (fsm *f, uchar *ucp)
{
    ccp_options *go;

    go = f->gotoptions;

    ADDCISTACKER(go->lzs);
    ADDCILZSDCP(go->lzsdcp);
    ADDCIPRED1(go->pred1);
}

/*
 * ccp_ackci - Ack our CIs.
 *
 * Returns:
 *    0 - Ack was bad.
 *    1 - Ack was good.
 *
 * This routine ensures that a received ACK message is valid. An ACK
 * message must contain all of the options that we specified, in the order
 * that we specified.
 */

static int ccp_ackci (fsm *f, uchar *p, int len)
{
    ushort  cishort;
    uchar   cichar, cilen, citype;
    ccp_options *go;
  
    go = f->gotoptions;

    /*
     * CIs must be in exactly the same order that we sent...
     * Check packet length and CI length at each step.
     * If we find any deviations, then this packet is bad.
     */
    ACKCISTACKER(go->lzs,go->negotiated_type);
    ACKCILZSDCP(go->lzsdcp,go->negotiated_type);
    ACKCIPRED1(go->pred1,go->negotiated_type);
  
    /*
     * If there are any remaining CIs, then this packet is bad.
     */
    if (len == 0)
        return(1);
    bad:
    if (ppp_error_debug)
        buginf("\n%s:CCP: received bad Ack!", f->idb->hw_namestring);
    return(0);
}


/*
 * ccp_alternatives - Re-configure for negotiating alternative compression.
 * This routine checks what options we've got left and reconfigures the
 * "gotoptions" structure with an alternative mode of compression for
 * negotiation.
 */
static void ccp_alternatives (fsm *f, uchar rejtype)
{
    ccp_options *go, *oo;
  
    go = f->gotoptions;
    oo = f->ouroptions;
  
    if (rejtype == CI_COMPRESSION_NONE)
        return;
  
    /*
     *  1st we remove the rejected compression type from the list of 
     * "our allowed" options, so we don't offer it again.
     */
    switch (rejtype){
        case CI_STACKER:
            oo->lzs.negotiate = CCP_NON_NEGOTIABLE_COMPRESSION;
        break;
        case CI_PREDICTOR1:
            oo->pred1.negotiate = CCP_NON_NEGOTIABLE_COMPRESSION;
        break;
        case CI_LZSDCP:
            oo->lzsdcp.negotiate = CCP_NON_NEGOTIABLE_COMPRESSION;
        break;
        default:
            if (ppp_error_debug)
                buginf("\n%s:CCP: received bad rejected type!",
                       f->idb->hw_namestring);
    }
    go->negotiate_compression = FALSE; /*
                                        * At this point we are not going to
                                        * negotiate compression. The next
                                        * stage MAY change that.
                                        */                                  
    /*
     * OK, so our suggested compression type was rejected - we should see what
     * else we can offer them in our next conf_req, if anything.
     */

    /* LZS DCP Option 23 */
    if (oo->lzsdcp.negotiate == CCP_NEGOTIABLE_COMPRESSION){
        go->lzs = oo->lzs; /* Structure Copy */
        go->negotiate_compression = TRUE;
        return;
    }

    /* Stacker Option 17 */
    if (oo->lzs.negotiate == CCP_NEGOTIABLE_COMPRESSION){
        go->lzs = oo->lzs; /* Structure Copy */
        go->negotiate_compression = TRUE;
        return;
    }

    /* Predictor Option 1 */
    if (oo->pred1.negotiate == CCP_NEGOTIABLE_COMPRESSION){
        go->pred1 = oo->pred1; /* Structure Copy */
        go->negotiate_compression = TRUE;
        return;
    }
}

/*
 * ccp_nakci - NAK some of our CIs.
 *
 * This routine handles a received NAK message. The NAK message will
 * list all of the options that we specified, the peer supports, but
 * the peer would prefer a different value.
 */
static void ccp_nakci (fsm *f, uchar *p, int len)
{
    uchar process_mode;
    uchar check_mode;
    ushort history_count;
    ccp_options *go;
    uchar rejtype;
  
    go = f->gotoptions;
    rejtype = CI_COMPRESSION_NONE;
  
    /*
     * Any Nak'd CIs must be in exactly the same order that we sent.
     * Check packet length and CI length at each step.
     * If we find any deviations, then this packet is bad.
     */
  
    NAKCISTACKER(go->lzs,
        if (go->lzs.history_count != history_count){
            /*
             * We're only going to support single histories per B
             * channel, or physical connection. So don't renegotiate.
             */
            go->lzs.negotiate = CCP_NON_NEGOTIABLE_COMPRESSION;
            rejtype = go->lzs.type;
            go->negotiate_compression = FALSE;

	    if (ppp_negotiation_debug)
	      buginf("\n%s:CCP: NAK of history_count, peer wants %d histories",
			f->idb->hw_namestring, history_count);
        }
           
        if (go->lzs.check_mode != check_mode)
            if ((check_mode == LZS_CHECK_MODE_NONE) ||
               (check_mode == LZS_CHECK_MODE_SEQ) ||
               (check_mode == LZS_CHECK_MODE_LCB)){
                go->lzs.check_mode = check_mode;
            }else{
                /*
                 * We don't support this check mode. 
                 */
	      if (ppp_negotiation_debug)
		buginf("\n%s:CCP: NAK of check mode peer wants %s",
		       f->idb->hw_namestring,
		       stac_check_mode_name(check_mode));
	      if (go->lzs.check_mode != LZS_CHECK_MODE_SEQ){
		/*
		 *  We should fall back on the default check method,
		 *  Sequence Number.
		 */	
		go->lzs.check_mode = LZS_CHECK_MODE_SEQ;
	      }else{	  
		go->lzs.negotiate = CCP_NON_NEGOTIABLE_COMPRESSION;
		rejtype = go->lzs.type;
		go->negotiate_compression = FALSE;
	      }
            }
    );
  
    NAKCILZSDCP(go->lzsdcp,
        if (go->lzsdcp.history_count != history_count){
            /*
             * We're only going to support single histories per B
             * channel, or physical connection. So don't renegotiate.
             */
            go->lzsdcp.negotiate = CCP_NON_NEGOTIABLE_COMPRESSION;
            rejtype = go->lzsdcp.type;
            go->negotiate_compression = FALSE;

            if (ppp_negotiation_debug)
                buginf("\n%s:CCP: NAK of history_count, peer wants %d histories",
                       f->idb->hw_namestring, history_count);
        }
           
        if (go->lzsdcp.check_mode != check_mode)
            if ((check_mode == LZSDCP_CHECK_MODE_NONE) ||
               (check_mode == LZSDCP_CHECK_MODE_LCB) ||
               (check_mode == LZSDCP_CHECK_MODE_SEQ) ||
               (check_mode == LZSDCP_CHECK_MODE_SEQ_LCB)){
                go->lzsdcp.check_mode = check_mode;
            }else{
                /*
                 * We don't support this check mode. Don't renegotiate.
                 */
                go->lzsdcp.negotiate = CCP_NON_NEGOTIABLE_COMPRESSION;
                rejtype = go->lzsdcp.type;
                go->negotiate_compression = FALSE;

               if (ppp_negotiation_debug)
                   buginf("\n%s:CCP: NAK of check mode peer wants %s",
                          f->idb->hw_namestring,
                          dcp_check_mode_name(check_mode));
           }
        if (go->lzsdcp.process_mode != process_mode){
            /*
             * We will insist on the process mode that passes uncompress
             * packets received to the decompress for history fix up.
             * This feature alone is why we have implemented the DCP header.
             */
            go->lzsdcp.negotiate = CCP_NON_NEGOTIABLE_COMPRESSION;
            rejtype = go->lzsdcp.type;
            go->negotiate_compression = FALSE;

            if (ppp_negotiation_debug)
                buginf("\n%s:CCP: NAK-ing their request for process mode %s",
                        f->idb->hw_namestring,
                        dcp_check_process_name(process_mode));
        }
    );
  
    NAKCIPRED1(go->pred1,
        if (ppp_error_debug)
            buginf("\n%s:CCP: NAK of Predictor1 - unexpected reply from host",
                   f->idb->hw_namestring);
            go->pred1.negotiate = CCP_NON_NEGOTIABLE_COMPRESSION;
            rejtype = go->pred1.type;
            go->negotiate_compression = FALSE;
    );
  
    /*
     * If there are any remaining CIs, then this packet is bad.
     * At this point, we could check for any appended options.
     */
    if (len != 0){
        if (ppp_error_debug)
            buginf("\n%s:CCP: ignoring unrequested options!",f->idb->hw_namestring);
        return;
    }
  
    /*
     * It's possible that we've decided not to re-negotiate our present choice
     * of compression, since we don't like the alternative configuration
     * requested by the other side. We should however check here if we can
     * offer an alternative compression type for negotiation.
     */
    if (rejtype != CI_COMPRESSION_NONE)
        ccp_alternatives(f, rejtype);
}


/*
 * ccp_rejci - Reject some of our CIs.
 */
static void ccp_rejci (fsm *f, uchar *p, int len)
{
    ushort   cishort;
    uchar    cichar;
    uchar    rejtype;
    ccp_options *go;
  
    rejtype = CI_COMPRESSION_NONE;
    go = f->gotoptions;
  
    /*
     * Any Rejected CIs must be in exactly the same order that we sent.
     * Check packet length and CI length at each step.
     * If we find any deviations, then this packet is bad.
     */
    REJCISTACKER(go->lzs,rejtype);
    REJCILZSDCP(go->lzsdcp,rejtype);
    REJCIPRED1(go->pred1,rejtype);
  
    /*
     * If there are any remaining CIs, then this packet is bad.
     */
    if (len != 0)
        goto bad;

    /*
     * If the stacker compression was rejected, then let's renegotiate.
     * This is needed for working with the Lex box. This code will be
     * removed after the lex compression software is upgraded
     */
    if((rejtype == CI_STACKER) && (go->lzs.check_mode == LZS_CHECK_MODE_LCB)
       && (go->lzs.history_count == LZS_MAX_HISTORIES)){
         go->lzs.negotiate = CCP_NEGOTIABLE_COMPRESSION;
         go->lzs.check_mode = LZS_CHECK_MODE_SEQ;
         go->negotiate_compression = TRUE;
         return;
    }

    /*
     * Check for alternative compression options.
     */
    ccp_alternatives(f, rejtype);
    return;
  
    bad:
    if (ppp_error_debug)
        buginf("\n%s:CCP: received bad Reject!", f->idb->hw_namestring);
}


/*
 * ccp_reqci - Check the peer's requested CIs and send appropriate response.
 *
 * Returns: CONFACK, CONFNAK or CONFREJ and input packet modified
 * appropriately.
 */
static uchar ccp_reqci (fsm *f, uchar *inp, short *len)
{
    ccp_options *ho, *ao;
    uchar *cip;              /* Pointer to Current CI */
    ushort cilen, citype;    /* Parsed len, type */
    ushort cishort;          /* Parsed short value */
    uchar cichar1,cichar2;   /* Parsed char value */
    int rc;                  /* Final packet return code */
    int orc;                 /* Individual option return code */
    uchar *p;                /* Pointer to next char to parse */
    uchar *ucp;              /* Pointer to current output char */
    int remaining = *len;    /* Length left */
  
  
    rc = CONFACK;
    p = inp;
    ucp = inp;
    ao = f->allowoptions;
    ho = f->heroptions;
  
    /*
     * Reset all her options.
     */
    ho->negotiate_compression = FALSE;
    ho->negotiated_type = CI_COMPRESSION_NONE;
    ho->lzs.negotiate = CCP_NON_NEGOTIABLE_COMPRESSION;
    ho->pred1.negotiate = CCP_NON_NEGOTIABLE_COMPRESSION;
  
    /*
     * Process all her options.
     */
    while (remaining){
        orc = CONFACK;      /* Assume success */
        cip = p;            /* Remember begining of CI */
    
        if (remaining < 2 ||        /* Not enough data for CI header or */
           p[1] < 2 ||             /* CI length too small or */
           p[1] > remaining){      /* CI length too big? */
            if (ppp_error_debug)
                buginf("\n%s:CCP: confreq has bad CI length!",
                        f->idb->hw_namestring);
           orc = CONFREJ;        /* Reject bad CI */
           cilen = remaining;    /* Reject till end of packet */
           remaining = 0;        /* Don't loop again */
           goto endswitch;
        }
    
        GETCHAR(citype, p);        /* Parse CI type */
        GETCHAR(cilen, p);         /* Parse CI length */
        remaining -= cilen;        /* Adjust remaining length */
        cilen -= 2;                /* Adjust cilen to just data */

        if (ppp_negotiation_debug)
            buginf("\n%s:CCP: Host requested compression type %s\n",
                   f->idb->hw_namestring, check_compression_type(citype));

        switch (citype){           /* Check CI type */
            case CI_STACKER:
                if (!ao->lzs.negotiate || cilen != (LEN_CI_LZS - 2)){
                    /* Check CI length */
                    INCPTR(cilen, p);     /* Skip rest of CI */
                    orc = CONFREJ;        /* Reject CI */
                    if (ppp_negotiation_debug)
                        buginf(" (rejected)");
                    break;
                }
      
                /*
                 * Handle the History Count.
                 */
                PPP_GETSHORT(cishort, p);
                if (ppp_negotiation_debug)
                    buginf(" -> Host has requested History Count %d", cishort);
      
                /*
                 * Single history support for now.
                 */
                if (cishort != LZS_MAX_HISTORIES){
                    orc = CONFNAK;
                    cishort = LZS_MAX_HISTORIES;
                }
      
                /*
                 * Handle the Check mode
                 */
                GETCHAR(cichar1, p);        /* Parse Check Mode */
                if (ppp_negotiation_debug)
                buginf(", check mode %d (%s)", cichar1,
                       stac_check_mode_name(cichar1));

                /*
                 * Are they requesting the check mode we expect.
                 * Reliable link running - No check mode
                 * No Reliable link - LCB or Sequence number check mode
                 */
                if (cichar1 != LZS_CHECK_MODE_SEQ &&
                    cichar1 != LZS_CHECK_MODE_LCB){
                    orc = CONFNAK;
                    /*
                     * Since everyone must support sequence numbers they
                     * really cann't refuse this suggestion.
                     */
                    cichar1 = LZS_CHECK_MODE_SEQ; /* Tell them we want SEQ */
                }

                /*
                 * Sanity Check that we're not agreeing to using more than
                 * one type of compression at the same time.
                 */
                if (ho->negotiate_compression){
                    /*
                     * More than one type of compression has been requested
                     * for. Reject this one.
                     */
                    if (ppp_negotiation_debug)
                        buginf("\n%s:CCP: Already accepted another CCP "
                               "option, rejecting this",
                               f->idb->hw_namestring);
                    orc = CONFREJ;
                }else{
                    /*
                     * Remember the options from this message
                     */
                    ho->lzs.negotiate = CCP_NEGOTIABLE_COMPRESSION;
                    ho->lzs.history_count = cishort;
                    ho->lzs.check_mode = cichar1;
                    ho->negotiated_type = CI_STACKER;
                    ho->negotiate_compression = TRUE;
                }

                /*
                 * If we are sending a NAK, update the parameters
                 */
                if (orc == CONFNAK) {
                    DECPTR(sizeof(uchar) + sizeof(ushort), p);
                    PPP_PUTSHORT(cishort, p); /* Send History Count */
                    PUTCHAR(cichar1, p); /* and Check mode */
                    if (ppp_negotiation_debug)
                        buginf("\n%s:CCP: NAKing with history count = %d, "
                               "check mode %s",
                               f->idb->hw_namestring, cishort,
                               stac_check_mode_name(cichar1));
                }
            break;
      
            case CI_LZSDCP:
                if (!ao->lzsdcp.negotiate || cilen != (LEN_CI_LZSDCP - 2)){
                    /* Check CI length */
                    INCPTR(cilen, p);     /* Skip rest of CI */
                    orc = CONFREJ;        /* Reject CI */
                    if (ppp_negotiation_debug)
                    buginf(" (rejected)");
                    break;
                }
      
                /*
                 * Handle the History Count.
                 */
      
                PPP_GETSHORT(cishort, p);
                if (ppp_negotiation_debug)
                    buginf(" -> Host has requested History Count %d", cishort);
      
                /*
                 * Single history support for now.
                 */
      
                if (cishort != LZS_MAX_HISTORIES){
                    orc = CONFNAK;
                    cishort = LZS_MAX_HISTORIES;
                }
      
                /*
                 * Handle the Check mode
                 */
                GETCHAR(cichar1, p);        /* Parse Check Mode */
                if (ppp_negotiation_debug)
                    buginf(", check mode %d (%s),", cichar1,
                           dcp_check_mode_name(cichar1));

                /*
                 * Are they requesting the check mode we expect.
                 * Reliable link running - No check mode
                 * No Reliable link - LCB or Sequence number check mode
                 */
                if (cichar1 != LZSDCP_CHECK_MODE_LCB &&
                    cichar1 != LZSDCP_CHECK_MODE_SEQ &&
                    cichar1 != LZSDCP_CHECK_MODE_SEQ_LCB){
                    orc = CONFNAK;
                    cichar1 = LZSDCP_CHECK_MODE_SEQ;
                }

                /*
                 * Handle the Process mode
                 */
                GETCHAR(cichar2, p);        /* Parse Process Mode */
                if (ppp_negotiation_debug)
                    buginf("\n    process mode %d (%s)", cichar2,
                           dcp_check_process_name(cichar2));

                /*
                 * Are they requesting the process mode we expect?
                 * If not then insist on process mode.
                 */
                if (cichar2 != LZSDCP_PROCESS_MODE_PROC){
                    orc = CONFNAK;
                    cichar2 = LZSDCP_PROCESS_MODE_PROC;
                }

                /*
                 * Sanity Check that we're not agreeing to using more than
                 * one type of compression at the same time.
                 */
                if (ho->negotiate_compression){
                    /*
                     * More than one type of compression has been requested
                     * for. Reject this one.
                     */
                    if (ppp_negotiation_debug)
                        buginf("\n%s:CCP: Already accepted another CCP "
                               "option, rejecting this",
                               f->idb->hw_namestring);
                    orc = CONFREJ;
                }else{
                    /*
                     * Remember the options from this message
                     */
                    ho->lzsdcp.negotiate = CCP_NEGOTIABLE_COMPRESSION;
                    ho->lzsdcp.history_count = cishort;
                    ho->lzsdcp.check_mode = cichar1;
                    ho->lzsdcp.process_mode = cichar2;
                    ho->negotiated_type = CI_LZSDCP;
                    ho->negotiate_compression = TRUE;
                }

                /*
                 * If we are sending a NAK, update the parameters
                 */
                if (orc == CONFNAK) {
                    DECPTR((2*sizeof(uchar)) + sizeof(ushort), p);
                    PPP_PUTSHORT(cishort, p); /* Send History Count */
                    PUTCHAR(cichar1, p); /* the check mode */
                    PUTCHAR(cichar2, p); /* and the process mode */
                    if (ppp_negotiation_debug)
                        buginf("\n%s:CCP: NAKing with history count = %d, "
                               "check mode %s and process mode %s",
                               f->idb->hw_namestring, cishort,
                               dcp_check_mode_name(cichar1),
                               dcp_check_process_name(cichar2));
                }
            break;
      
            case CI_PREDICTOR1:
                if (!ao->pred1.negotiate || cilen != (LEN_CI_PRED1-2)){
                    INCPTR(cilen, p);
                    orc = CONFREJ;
                    if (ppp_negotiation_debug)
                        buginf(" (rejected)");
                     break;
                }

                /*
                 * Sanity Check that we're not agreeing to using more than
                 * one type of compression at the same time.
                 */
                if (ho->negotiate_compression){
                    /*
                     * More than one type of compression has been requested
                     * for. Reject this one.
                     */
                    orc = CONFREJ;
                }else{
                    /*
                     * Remember the options from this message
                     */
                    ho->pred1.negotiate = CCP_NEGOTIABLE_COMPRESSION;
                    ho->negotiated_type = CI_PREDICTOR1;
                    ho->negotiate_compression = TRUE;
                }
            break;
      
            default:
                if (ppp_error_debug)
                    buginf("\n%s:CCP: Rejecting Compression type %s",
                           f->idb->hw_namestring,
                           check_compression_type(citype));
                INCPTR(cilen, p);
                orc = CONFREJ;
            break;
        }
        cilen += 2;            /* Adjust cilen whole CI */
    
        endswitch:
        if (ppp_negotiation_debug)
            buginf(" (%s)",
                   orc == CONFACK ? "ACK" : (orc == CONFNAK ? "NAK" : "REJ"));
    
        if (orc == CONFACK &&    /* Good CI */
           rc != CONFACK)       /* but prior CI wasnt? */
            continue;            /* Don't send their one */
    
        if (orc == CONFNAK){     /* Nak this CI? */
            if (rc == CONFREJ)   /* Rejecting prior CI? */
                continue;        /* Don't send this one */
            if (rc == CONFACK){  /* Ack'd all prior CIs? */
                rc = CONFNAK;    /* Not anymore... */
                ucp = inp;       /* Backup */
            }
        }
    
        if (orc == CONFREJ &&    /* Reject this CI */
            rc != CONFREJ){      /* but no prior ones? */
            rc = CONFREJ;
            ucp = inp;            /* Backup */
        }
    
        if (ucp != cip)            /* Need to move CI? */
            bcopy(cip, ucp, cilen);    /* Move it */
    
        INCPTR(cilen, ucp);        /* Update output pointer */
    }
  
    /*
     * XXX If we wanted to send additional NAKs (for unsent CIs), the
     * code would go here. This must be done with care since it might
     * require a longer packet than we received.
     */
  
    *len = ucp - inp;            /* Compute output length */
 
    if (ppp_negotiation_debug)
        buginf("\n%s:CCP: ccp_reqci: returning %s.",
               f->idb->hw_namestring,
               rc == CONFACK ? "CONFACK" :
               rc == CONFNAK ? "CONFNAK" : "CONFREJ");
    return(rc);            /* Return final code */
}

/*
 * ccp_up - FSM has reached up state
 */

static void ccp_up (fsm *f)
{
    hwidbtype *idb;
    ccp_options *go;
    ccp_options *ho;
    int max_pkt_size;
  
    go = f->gotoptions;
    ho = f->heroptions;
  
    idb = f->idb;        /* Get the IDB */

    /*
     * Maximum size of a packet that PPP compression will see.
     */
    max_pkt_size = idb->maxmtu + PPP_PROTO_HDR_ENCAPBYTES;
  
    /*
     * Intialise compression stacks etc here
     */
    switch(ho->negotiated_type){
        case CI_STACKER:
        {
            stacHistory    *hdr_xmt;
            int            wrk_space_size;
      
            if (!idb->ppp_comp_history){
                idb->ppp_comp_history = malloc(LZS_SizeOfCompressionHistory()
                                               + sizeof(stacHistory) );
                if (!idb->ppp_comp_history){
                    ncp_close(NULL, f);
                    return;
                }
                hdr_xmt = (stacHistory *)idb->ppp_comp_history;
                /*
                 * Only initialise this boolean if the link has just come up.
                 */
                hdr_xmt->legacy_support = FALSE;            
            }
            else
                hdr_xmt = (stacHistory *)idb->ppp_comp_history;

            hdr_xmt->check_mode = ho->lzs.check_mode;
            hdr_xmt->seq_num = LZS_SEQ_NUM_INIT;
            hdr_xmt->reset_id = LZS_ID_NUM_INIT;
            hdr_xmt->reset_requested = FALSE;
            hdr_xmt->reset_acked = FALSE;
            hdr_xmt->history = idb->ppp_comp_history + sizeof(stacHistory);
            LZS_InitCompressionHistory(hdr_xmt->history);
            if (!idb->ppp_comp_wrkspc){
                /*
                 * Our workspace needs to be as large as the largest packet we
                 * can expect to receive plus at least 12.5% extra space to
                 * take into account the maximum expansion LZS can create.
                 */
                wrk_space_size = max_pkt_size + (max_pkt_size/8) +
                                  CCP_LZS_COMPRESS_FUDGE;
                idb->ppp_comp_wrkspc = malloc(wrk_space_size);
                hdr_xmt->wrk_spc_size = wrk_space_size;
            }
            if (!idb->ppp_comp_wrkspc){
                ncp_close(NULL, f);
                return;
            }
            idb->ppp_compression = ccp_compress_stacker;
        }
        break;
    
        case CI_LZSDCP:
        {
            lzsdcpHistory   *hdr_xmt;
            int                wrk_space_size;
      
            if (!idb->ppp_comp_history)
                idb->ppp_comp_history = malloc(LZS_SizeOfCompressionHistory()
                                               + sizeof(lzsdcpHistory) );
            if (!idb->ppp_comp_history){
                ncp_close(NULL, f);
                return;
            }

            hdr_xmt = (lzsdcpHistory *)idb->ppp_comp_history;
            hdr_xmt->check_mode = ho->lzsdcp.check_mode;
            hdr_xmt->seq_num = LZSDCP_SEQ_NUM_INIT;
            hdr_xmt->flags = 0;
            hdr_xmt->history = idb->ppp_comp_history + sizeof(lzsdcpHistory);
            LZS_InitCompressionHistory(hdr_xmt->history);
            if (!idb->ppp_comp_wrkspc){
                /*
                 * Workspace needs to be as large as the largest packet we
                 * can expect to receive plus at least 12.5% extra space to
                 * take into account the maximum expansion LZS can cause.
                 */
                wrk_space_size = max_pkt_size + (max_pkt_size/8) +
                                  CCP_LZS_COMPRESS_FUDGE;
                idb->ppp_comp_wrkspc = malloc(wrk_space_size);
                hdr_xmt->wrk_spc_size = wrk_space_size;
            }
            if (!idb->ppp_comp_wrkspc){
                ncp_close(NULL, f);
                return;
            }
            idb->ppp_compression = ccp_compress_lzs_dcp;
        }
        break;
    
        case CI_PREDICTOR1:
        {
            pred1History    *hdr_xmt;
            int             wrk_space_size;
      
            if (!idb->ppp_comp_history){
                idb->ppp_comp_history = malloc(GUESS_TABLE_SIZE +
                                               sizeof(pred1History));
                if (!idb->ppp_comp_history){
                    ncp_close(NULL, f);
                    return;
                }
                hdr_xmt = (pred1History *)idb->ppp_comp_history;
                /*
                 * Only initialise this boolean if the link has just come up.
                 */
                hdr_xmt->legacy_support = FALSE;
            }
            else
                hdr_xmt = (pred1History *)idb->ppp_comp_history;
            hdr_xmt->hash = PRED1_HASH_MASK_INIT;
            hdr_xmt->reset_id = PRED1_ID_NUM_INIT;
            hdr_xmt->reset_requested = FALSE;
            hdr_xmt->guess_table = idb->ppp_comp_history +
                                        sizeof (pred1History);
            memset(hdr_xmt->guess_table, 0, GUESS_TABLE_SIZE);
            if (!idb->ppp_comp_wrkspc){
                /*
                 * Workspace needs to be as large as the largest packet we
                 * can expect to receive plus room for expansion.
                 */
                wrk_space_size = max_pkt_size + (max_pkt_size/8) + 1;
                idb->ppp_comp_wrkspc = malloc(wrk_space_size);
                hdr_xmt->wrk_spc_size = wrk_space_size;
                if (!idb->ppp_comp_wrkspc){
                    ncp_close(NULL, f);
                    return;
                }
            }
            idb->ppp_compression = ccp_compress_predictor1;
        }
        break;
    
        default:
            if (ho->negotiated_type != CI_COMPRESSION_NONE){
                if (ppp_error_debug)
                    buginf("\n%s:CCP: We have unknown compress type %d(%s).",
                           f->idb->hw_namestring, ho->negotiated_type,
                           check_compression_type(ho->negotiated_type));
                ncp_close(NULL, f);
                return;
            }
           idb->ppp_compression = NULL;
    }
  
    /*
     * Intialise decompression stacks etc here
     */
    switch(go->negotiated_type){
        case CI_STACKER:
        {
            stacHistory    *hdr_rcv;
      
            if (!idb->ppp_decomp_history){
                idb->ppp_decomp_history = malloc(LZS_SizeOfDecompressionHistory()
                                                 + sizeof(stacHistory));
                if (!idb->ppp_decomp_history){
                    ncp_close(NULL, f);
                    return;
                }
                hdr_rcv = (stacHistory *)idb->ppp_decomp_history;
                /*
                 * Only initialise this boolean if the link has just come up.
                 */
                hdr_rcv->legacy_support = FALSE;            
            }
            else
                hdr_rcv = (stacHistory *)idb->ppp_decomp_history;
            hdr_rcv->check_mode = go->lzs.check_mode;
            hdr_rcv->seq_num = LZS_SEQ_NUM_INIT;
            hdr_rcv->reset_id = LZS_ID_NUM_INIT;
            hdr_rcv->reset_requested = FALSE;
            hdr_rcv->reset_acked = FALSE;
            hdr_rcv->history = idb->ppp_decomp_history + sizeof(stacHistory);
            LZS_InitDecompressionHistory(hdr_rcv->history);
            idb->ppp_decompression = ccp_decompress_stacker;
        }
        break;
    
        case CI_LZSDCP:
        {
            lzsdcpHistory    *hdr_rcv;
      
            if (!idb->ppp_decomp_history)
                idb->ppp_decomp_history = malloc(LZS_SizeOfDecompressionHistory()
                                                 + sizeof(lzsdcpHistory) );
            if (!idb->ppp_decomp_history){
                ncp_close(NULL, f);
                return;
            }
            hdr_rcv = (lzsdcpHistory *)idb->ppp_decomp_history;
            hdr_rcv->history = idb->ppp_decomp_history + sizeof(lzsdcpHistory);
            LZS_InitDecompressionHistory(hdr_rcv->history);
            hdr_rcv->check_mode = go->lzsdcp.check_mode;
            hdr_rcv->seq_num = LZSDCP_SEQ_NUM_INIT;
            hdr_rcv->flags = 0;
            idb->ppp_decompression = ccp_decompress_lzs_dcp;
        }
        break;
    
        case CI_PREDICTOR1:
        {
            pred1History    *hdr_rcv;

            if (!idb->ppp_decomp_history){
                idb->ppp_decomp_history = malloc(GUESS_TABLE_SIZE +
                                                sizeof (pred1History));
                if (!idb->ppp_decomp_history){
                    ncp_close(NULL, f);
                    return;
                }
                hdr_rcv = (pred1History *)idb->ppp_decomp_history;
                /*
                 * Only initialise this boolean if the link has just come up.
                 */
                hdr_rcv->legacy_support = FALSE;
            }
            else
                hdr_rcv = (pred1History *)idb->ppp_decomp_history;
            hdr_rcv->hash = PRED1_HASH_MASK_INIT;
            hdr_rcv->reset_id = PRED1_ID_NUM_INIT;
            hdr_rcv->reset_requested = FALSE;
            hdr_rcv->guess_table = idb->ppp_decomp_history
                                    + sizeof (pred1History);
            memset(hdr_rcv->guess_table, 0, GUESS_TABLE_SIZE);
            idb->ppp_decompression = ccp_decompress_predictor1;
        }
        break;
    
        default:
            if (ho->negotiated_type != CI_COMPRESSION_NONE){
                if (ppp_error_debug)
                    buginf("\n%s:CCP: They have unknown compress type %d(%s).",
                            f->idb->hw_namestring, go->negotiated_type,
                            check_compression_type(go->negotiated_type));

                ncp_close(NULL, f);
                return;
            }
            if ((ho->negotiated_type == CI_COMPRESSION_NONE)
                       && ppp_negotiation_debug)
                buginf("\n%s:CCP: Decompression not negotiated",
                        idb->hw_namestring);
                idb->ppp_decompression = NULL;
    }
    /*
     * Must inform the fast switching code of the change of state.  Failure to
     * call this, will result in no packets being passed through PPP
     * encapsulation. Hence no compresssion.
     */
    reg_invoke_fast_setup(idb);
    /* 
     * This has to be done here as lex is bound to a serial interface.
     */
    if (idb->firstsw->lex_swidb) {
        hwidbtype *lex_hwidb;
        lex_hwidb = idb->firstsw->lex_swidb->hwptr;
        lex_hwidb->ppp_compression = idb->ppp_compression;
        lex_hwidb->span_bridge_process_force = SPAN_BR_PROC_COMPRESSION;
        reg_invoke_fast_setup(lex_hwidb);
    }
}


/*
 * ccp_down - FSM has left the OPEN state
 */

static void ccp_down (fsm *f)
{
    hwidbtype *idb;

    idb = f->idb;        /* Get the IDB */

    /*
     * Clearing of the function pointer to compression prevents any more
     * outgoing packets being compressed until such times as the FSM returns to
     * the OPEN State.
     * The decompression function pointer is cleared to ensure that
     * all compressed packets received are discarded
     */

    idb->ppp_compression = NULL;
    idb->ppp_decompression = NULL;
}


/*
 * ccp_closed - FSM has gone to CLOSED state
 */

static void ccp_closed (fsm *f)
{
    hwidbtype *idb;
  
    idb = f->idb;        /* Get the IDB */
  
    /*
     * Clearing function pointers for compression and decompression ensures
     * that compression is halted.
     */

    idb->ppp_compression = NULL;
    idb->ppp_decompression = NULL;
  
    /*
     * Release any memory allocated for compression history since the next
     * compression type negotiated may need a different size of workspace.
     */
    if (idb->ppp_decomp_history != idb->ppp_comp_history){
        /*
         * Some compression algorithims like Stacker use the same history for
         * decompression as compression, so be carefull not to free the same
         * memory block twice.
         */
        free(idb->ppp_decomp_history);
    }
    idb->ppp_decomp_history = NULL;
    free(idb->ppp_comp_history);
    idb->ppp_comp_history = NULL;
    free(idb->ppp_comp_wrkspc);
    idb->ppp_comp_wrkspc = NULL;
  
    /*
     * Must inform the fast switching code of the change of state.
     * Fastswitching may be used now that compression is not.
     */
    reg_invoke_fast_setup(idb);
    /* 
     * This has to be done here as lex is bound to a serial interface.
     */
    if (idb->firstsw->lex_swidb) {
        hwidbtype *lex_hwidb;
        lex_hwidb = idb->firstsw->lex_swidb->hwptr;
        lex_hwidb->ppp_compression = idb->ppp_compression;
        lex_hwidb->span_bridge_process_force = 0;
        reg_invoke_fast_setup(lex_hwidb);
    }
}

/*
 * ccp_resetreq - Reset Request by our peer of one our compression histories
 */
static boolean ccp_resetreq (fsm *f, uchar *data, int len)
{
    hwidbtype *idb;
    ccp_options *ho;
    ushort history;
    uchar  *p;

    idb = f->idb;        /* Get the IDB */
    ho = f->heroptions;

    switch(ho->negotiated_type){
        case CI_STACKER:
        {
            stacHistory    *hdr;            

            if (ppp_negotiation_debug)
                buginf("\n%s:CCP: Stacker received RESET-REQ",
                        idb->hw_namestring);

            if (idb->ppp_comp_history)
                hdr = (stacHistory *)idb->ppp_comp_history;
            else
                return(FALSE);
            /*
             * If it has been included, check that the history number makes
             * sense.
             */
            p = data;
            PPP_GETSHORT(history, p);
            
            if (history != LZS_MAX_HISTORIES){
                if (ppp_error_debug)
                    buginf("\n%s:CCP: Stac - bad history # in resetreq (%d)",
                            idb->hw_namestring, history);
                return(FALSE);
            }

            hdr->reset_requested = TRUE;
            return(TRUE);
        }
        break;

        case CI_PREDICTOR1:
        {
            pred1History *hdr;

            if (ppp_negotiation_debug)
                buginf("\n%s:CCP: Predictor received RESET-REQ",
                        idb->hw_namestring);

            if (idb->ppp_comp_history)
                hdr = (pred1History *)idb->ppp_comp_history;
            else
                return(FALSE);
            hdr->reset_requested = TRUE;
            return(TRUE);
        }
        break;

        default:
            return(FALSE);
    }
}

/*
 * ccp_resetack - Reset Ack received from peer.
 */
static void ccp_resetack (fsm *f, uchar id, uchar *data, int len)
{
    hwidbtype *idb;
    ccp_options *go;
    ushort    history;
    uchar    *p;

    idb = f->idb;        /* Get the IDB */
    go = f->gotoptions;
    
    switch(go->negotiated_type){
        case CI_STACKER:
        {
            stacHistory    *hdr;

            if (ppp_negotiation_debug)
                buginf("\n%s:CCP: Stacker received RESET-ACK",
                        idb->hw_namestring);

            if (idb->ppp_decomp_history)
                hdr = (stacHistory *)idb->ppp_decomp_history;
            else
                return;
            /*
             * Check that the id of this ACK is what we expect.
             */
            if (hdr->reset_id != id)
                return;
            /*
             * If it has been included, check that the history number makes
             * sense.
             */
            p = data;
            PPP_GETSHORT(history, p);
            
            if (history != LZS_MAX_HISTORIES){
                if (ppp_error_debug)
                    buginf("\n%s:CCP: Stac - bad history # in resetack (%d)",
                            idb->hw_namestring, history);
                return;
            }
            LZS_InitDecompressionHistory(hdr->history);
            hdr->reset_requested = FALSE;
            hdr->reset_acked = TRUE; /* Required for seq number re-sync */
            /*
             * Update stats
             */
            if (idb->comp_db)
                idb->comp_db->comp_restarts ++;
        }
        break;

        case CI_PREDICTOR1:
        {
            pred1History *hdr;

            if (ppp_negotiation_debug)
                buginf("\n%s:CCP: Predictor received RESET-ACK",
                        idb->hw_namestring);

            if (idb->ppp_decomp_history)
                hdr = (pred1History *)idb->ppp_decomp_history;
            else
                return;
            /*
             * Check that the id of this ACK is what we expect.
             */
            if (hdr->reset_id != id)
                return;

            memset(hdr->guess_table, 0, GUESS_TABLE_SIZE);
            hdr->hash = PRED1_HASH_MASK_INIT;
            hdr->reset_requested = FALSE;
            /*
             * Update stats
             */
            if (idb->comp_db)
                idb->comp_db->comp_restarts ++;
        }
        break;
    }
}

/*
 * ccp_coderej - Code Reject received from peer
 *
 * We've got to assume that if we received a code reject for CCP that it
 * was as a result of an implementation which doesn't support the history
 * syncing mechanism reset-req/ack. In other words our legacy code.
 * The reason we can only assume is because our dumb legacy code doesn't
 * return the packet we sent. It's sends garbage instead. Sigh....
 */
static void ccp_coderej (fsm *f)
{
    hwidbtype *idb;
    ccp_options *go;

    idb = f->idb;
    go = f->gotoptions;

    if (ppp_error_debug)
        buginf("\nCCP: Rcvd a Code-Reject for our Reset-Request");

    switch(go->negotiated_type){
        case CI_STACKER:
        {
            stacHistory    *hdr;

            if (idb->ppp_decomp_history)
                hdr = (stacHistory *)idb->ppp_decomp_history;
            else
                return;
            hdr->legacy_support = TRUE;
            /*
             * Try reseting again. This time the "old" method will be used.
             */
            ccp_decompress_stacker_failure(idb);
        }
        break;

        case CI_PREDICTOR1:
        {
            pred1History *hdr;

            if (idb->ppp_decomp_history)
                hdr = (pred1History *)idb->ppp_decomp_history;
            else
                return;
            hdr->legacy_support = TRUE;
            /*
             * Try reseting again. This time the "old" method will be used.
             */
            ccp_decompress_predictor1_failure(idb);
        }
        break;
    }    
}

/*
 * Used to check if CCP is configured for negotiation
 */

static boolean ccp_compression_running (idbtype *idb)
{
    hwidbtype *hwidb;
    fsm *ccpfsm;
    ccp_options *wo;

    if (!idb)
        return(FALSE);

    hwidb = idb->hwptr;

    ccpfsm = ppp_serial2fsm(TYPE_PPP_CCP, hwidb);
    if (!ccpfsm)
        return(FALSE);

    wo = ccpfsm->wantoptions;
    if (!wo)
        return(FALSE);

    return(wo->negotiate_compression);
}

/*
 * ccp_compression_request_reset - Called by decompression routines when an
 * error is detected. Sends RESET-REQ to the compressor on the remote side.
 */
static void ccp_compression_request_reset (hwidbtype *idb, uchar id,
                                    uchar *data, int len)
{
    fsm *f;
    paktype *pak;
    char *outp;
  
    f = ppp_serial2fsm(TYPE_PPP_CCP, idb);
  
    if (f->state != OPEN)
        return;

    if (data){
        pak = (paktype *)getbuffer((sizeof(ppp_hdr)) + len + DLLHEADERLEN);
        if (!pak){
            buginf("%s:CCP: Failed to send Reset Req. Closing CCP\n",
                    idb->hw_namestring);
            ncp_close(NULL, f);
            return;
        }
        pak->datagramstart = pak->network_start - DLLHEADERLEN;
        outp = (uchar *) PACKET_DATA(pak->datagramstart);
        bcopy(data, outp, len);
        ppp_send_packet(f->protocol, RESETREQ, id, pak, len, idb);
    }else
        ppp_send_packet(f->protocol, RESETREQ, id, NULL, 0, idb);
}

/*
 * Attempt to compress a protocol datagram using the compression alogrithm
 * negotiated by CCP. If compression is successfull it adds the CCP protocol
 * header to the start of the packet.
 * If compression fails the packet is returned unchanged since it can still
 * be transmitted in it's original form with it's original Proto Header.
 */
void ccp_compress_outgoing_pkt (hwidbtype *idb, paktype *pak)
{
    /*
     * In future developments it may be necessary to switch of interrupts at
     * this point since some compression algorithms (e.g. stacker) are not
     * re-enterant. PPP does not operate, currently, in any way that would
     * cause a problem in this respect. This may change.
     */
    (*idb->ppp_compression)(pak);
}

/*
 * 
 * This is our old legacy method of re-syncing histories. It uses CCP
 * renegotiation.
 */
static void ccp_compression_legacy_restart (hwidbtype *idb)
{
    fsm *f;
  
    if (ppp_error_debug) {
        buginf("\n%s:CCP: Re-Syncing history using legacy method",
            idb->hw_namestring);
    }
  
    f = ppp_serial2fsm(TYPE_PPP_CCP, idb);
  
    if (!f || (f->state != OPEN)) {
        return;
    }
  
    if (f->callbacks->resetci) {
        (*f->callbacks->resetci)(f);
    }
    fsm_sconfreq(f);
    f->state = REQSENT;
    if (idb->comp_db) {
        idb->comp_db->comp_restarts ++;
    }
}

/*
 * Create an LCB for uncompressed data as defined by the draft document,
 * "PPP Stacker LZS Compression Protocol"
 */
static uchar ccp_compute_stacker_lcb (char *data, ulong len)
{
    uchar lcb;
    
    lcb = LZS_LCB_NUM_INIT;

    while(len--)
        lcb ^= *data++;
    return (lcb);
}

/*
 * PPP interface to Stacker Compression
 */
static boolean ccp_compress_stacker (paktype *pak)
{
    hwidbtype *idb;
    stacHistory *hdr;
    uchar *src_p, *dest_p;
    uchar *src_p_wk, *dest_p_wk;
    ulong src_len, src_len_wk;
    ulong dest_len;
    ulong max_dest_len;
    ulong max_dest_len_wk;
    ulong pak_max_size;
    ushort result;
    uchar *buffer;
    int proto_hdr_size;
    uchar lcb;
  
    idb = pak->if_output->hwptr;
    pak_max_size = pak_get_pool_size(pak);

    if (!idb->ppp_comp_history){
        if (ppp_error_debug)
            buginf("\n%s:CCP: Stacker Comp - No compression history!",
                   idb->hw_namestring);
        return(FALSE);
    }
  
    if (!idb->ppp_comp_wrkspc){
        if (ppp_error_debug)
            buginf("\n%s:CCP: Stacker Comp - No compression workspace!",
                   idb->hw_namestring);
        return(FALSE);
    }
  
    buffer = idb->ppp_comp_wrkspc;
    hdr = (stacHistory *)idb->ppp_comp_history;
    lcb = 0; /* Keeps the compiler happy */

    /*
     * Has a compression history reset been requested by the other side?
     */
    if (hdr->reset_requested){
        LZS_InitCompressionHistory(hdr->history);
        hdr->reset_requested = FALSE;
        /*
         * Update compression stats
         */
        if (idb->comp_db)
            idb->comp_db->comp_restarts ++;
    }

    /*
     * pak_max_size cannot be larger than the MTU
     */
    if (pak_max_size > (idb->maxmtu + PPP_PROTO_HDR_ENCAPBYTES))
        pak_max_size = idb->maxmtu + PPP_PROTO_HDR_ENCAPBYTES;

    proto_hdr_size = PPP_ENCAPBYTES;

    src_len = pak->datagramsize;
    src_p = pak->datagramstart;

    /*
     * Update stats
     */
    if (idb->comp_db)
        idb->comp_db->comp_xmt_in_bytes += src_len;

    /*
     * Do we need to leave space for a check value?
     */
    switch(hdr->check_mode){
        case LZS_CHECK_MODE_SEQ:
            proto_hdr_size += sizeof(uchar);
        break;

        case LZS_CHECK_MODE_LCB:
            proto_hdr_size += sizeof(uchar);
            /*
             * Get the LCB of the uncompressed data.
             */
            lcb = ccp_compute_stacker_lcb(src_p, src_len);
        break;

        default:
        break;
    }

    /*
     * According to the Stacker Spec. expansion should be at most 12.5% of the
     * orginal size, so we divide the current length by 8 and add it on.
     */
    max_dest_len = src_len + (src_len/8);

    /*
     * We need to check if the pak we have will hold this much data. We cannot
     * allocate a new packet since we have no way of passing it back.
     * This code is called as an encapsulation routine.
     */
    if ((proto_hdr_size + max_dest_len) > pak_max_size)
        max_dest_len = pak_max_size - proto_hdr_size;

    /*
     * Adding on the FUDGE factor guarantees that Stacker will give up only
     * after having used all the available workspace.
     */
    max_dest_len += CCP_LZS_COMPRESS_FUDGE;

    dest_p = buffer;
  
    /*
     * We don't add the PPP control & address fields.
     */
    proto_hdr_size -= PPP_PROTO_HDR_ENCAPBYTES;

    /* LZS_Compress() modifies the following 4 copied variables */
    max_dest_len_wk = max_dest_len;
    src_p_wk = src_p;
    dest_p_wk = dest_p + proto_hdr_size;
    src_len_wk = src_len;
  
    result = LZS_Compress(&src_p_wk, &dest_p_wk, &src_len_wk,
                          &max_dest_len_wk, hdr->history,
                         (LZS_SAVE_HISTORY | LZS_SOURCE_FLUSH |
                          LZS_DEST_FLUSH | LZS_PERFORMANCE_MODE_1),
                         3);
    dest_len = (max_dest_len - max_dest_len_wk) + proto_hdr_size;
  
    /*
     * Check for anti-compression.
     */
    if ((result == LZS_INVALID) || (result & LZS_DEST_EXHAUSTED)){
        LZS_InitCompressionHistory(hdr->history);
        if (ppp_error_debug)
            buginf("\n%s:CCP: LZS - packet expansion too great. "
                   "Reseting Comp history", idb->hw_namestring);
       /*
        * Update stats
        */
       if (idb->comp_db){
           idb->comp_db->comp_restarts ++;
           idb->comp_db->uncompressed_sent += src_len;
       }
  
       /*
        * Return TRUE since we don't need to reset the other side.
        */
       return(TRUE);
    }
  
    PPP_PUTSHORT(TYPE_PPP_COMPRESS, dest_p);
    /*
     * Add check value to the header.
     */
    switch(hdr->check_mode){
        case LZS_CHECK_MODE_SEQ:
            PUTCHAR((hdr->seq_num ++), dest_p);
        break;

        case LZS_CHECK_MODE_LCB:
            PUTCHAR(lcb, dest_p);
        break;

        default:
        break;
    }
    pak->datagramsize = dest_len;
    pak->linktype = LINK_PPP_COMPRESSION;
    bcopy(buffer, pak->datagramstart, pak->datagramsize);
    /*
     * Update stats
     */
    if (idb->comp_db)
        idb->comp_db->compressed_sent += dest_len;

    return(TRUE);
}

/*
 * Deals with a decompression error/failure. Need to indicate to the other side
 * that the history is corrupt and needs reseting.
 */
static void ccp_decompress_stacker_failure (hwidbtype *idb)
{
    stacHistory *hdr;
    char history[2];
    char *p;

    
    hdr = (stacHistory *)idb->ppp_decomp_history;

    if (hdr->legacy_support){
        ccp_compression_legacy_restart(idb);
        return;
    }

    hdr->reset_id ++;
    hdr->reset_requested = TRUE;

    /*
     * This assumes that we support only 1 history. Later when multiple
     * histories are added a data field containing the history number will need
     * to be added.
     */
    p = history;
    PPP_PUTSHORT(LZS_MAX_HISTORIES, p);

    if (ppp_error_debug)
        buginf("\n%s:CCP: Stacker - Sending Reset Request to peer",
                    idb->hw_namestring);

    ccp_compression_request_reset(idb, hdr->reset_id, history, 2);
}

/*
 * PPP interface to Stacker Decompression
 */
static boolean ccp_decompress_stacker (paktype **pak)
{
    hwidbtype *idb = (*pak)->if_input->hwptr;
    stacHistory *hdr;
    uchar *src_p, *dest_p, *dest_p_wk, *tail_p;
    ulong src_len, dest_len, max_dest_len, max_dest_len_wk;
    ushort result;
    paktype *newpak;
    uchar seq_num, lcb;

    if (!idb->ppp_decomp_history){
        if (ppp_error_debug)
            buginf("\n%s:CCP: Stacker Decomp - No decompression history!",
                   idb->hw_namestring);
        return(FALSE);
    }
  
    hdr = (stacHistory *)idb->ppp_decomp_history;

    /*
     * Are we awaiting a Reset-Ack from a previous decompression failure?
     */
    if (hdr->reset_requested){
        return(FALSE);
    }
    
    lcb = 0; /* Keeps the compiler happy */  
    src_p = (*pak)->datagramstart;
    src_len = (*pak)->datagramsize;

    /*
     * If we negotiated the use of a check value then extract it and, if
     * possible at this point, check it.
     */
    switch(hdr->check_mode){
        case LZS_CHECK_MODE_SEQ:
        {
            GETCHAR(seq_num, src_p);
            src_len -= sizeof(uchar);
            /*
             * If this packet is the first one we receive after a RESET-ACK
             * then we re-sync our sequence number count on the sequence number
             * in this packet.
             */
            if (hdr->reset_acked)
                hdr->seq_num = seq_num;
            else
                if (seq_num != hdr->seq_num){
                /*
                 * If the packet is out of sequence then return with an error.
                 */
                    if (ppp_error_debug)
                        buginf("\n%s:CCP: Stac Decomp Error."
                                " Expected pkt# %d got %d",
                               idb->hw_namestring, hdr->seq_num, seq_num);
                    ccp_decompress_stacker_failure(idb);
                    return(FALSE);
                 }
             hdr->seq_num ++;
        }
        break;

        case LZS_CHECK_MODE_LCB:
            GETCHAR(lcb, src_p)
            src_len -= sizeof(uchar);
        break;

        default:
        break;
    }

    /*
     * Clear reset acked flag
     */
    hdr->reset_acked = FALSE;

    if ((long)src_len <= 0){    /* for zero-length incoming paks */        
        if (ppp_error_debug)
            buginf("\n%s:CCP: Stac Decomp Error. zero length packet",
                    idb->hw_namestring);
        ccp_decompress_stacker_failure(idb);
        return(FALSE);
    }
  
    /*
     * Zero Deletion/Insertion: Need to add a trailing zero to guarantee that
     * we have an end marker
     */
    tail_p = src_p + src_len;
    PUTCHAR(0, tail_p);
    src_len ++;

    if ((newpak = getbuffer(idb->maxmtu + PPP_ENCAPBYTES + ETHER_BAGGAGE)) == NULL){
        if (idb->comp_db)
            idb->comp_db->comp_rcv_no_bufs++;
        ccp_decompress_stacker_failure(idb);
        return(FALSE);
    }

    pak_copy(*pak, newpak, 0);
    dest_p = pak_center(newpak);
    newpak->encsize = PPP_ENCAPBYTES;
    newpak->network_start = dest_p;
    dest_p -= newpak->encsize;    /* protocol data starts on word boundary */ 
    newpak->datagramstart = dest_p;
    PPP_PUTSHORT(PPP_STATION, dest_p); /* ppp_parse_packet needs this header */
    max_dest_len = idb->maxmtu + PPP_PROTO_HDR_ENCAPBYTES + ETHER_BAGGAGE;
    max_dest_len_wk = max_dest_len;
    dest_p_wk = dest_p;

    result = LZS_Decompress(&src_p, &dest_p_wk, &src_len, &max_dest_len_wk,
                            hdr->history, LZS_SAVE_HISTORY);

    if ((result & LZS_DEST_EXHAUSTED) && !(result & LZS_END_MARKER)){
        if (ppp_error_debug)
            buginf("\n%s:CCP: Stacker Decomp - Destination buffer exhausted.",
                   idb->hw_namestring);
        datagram_done(newpak);
        ccp_decompress_stacker_failure(idb);
        return(FALSE);
    }

    if (!(result & LZS_END_MARKER)){
        if (ppp_error_debug)
            buginf("\n%s:CCP: Stacker Decomp - LZS end marker not reached.",
                   idb->hw_namestring);
        datagram_done(newpak);
        ccp_decompress_stacker_failure(idb);
        return(FALSE);
    }

    dest_len = max_dest_len - max_dest_len_wk;

    /*
     * If we are using LCB as the check mode then get the LCB for the data we
     * just uncompress and compare it to what we should be expecting.
     */
    if (hdr->check_mode == LZS_CHECK_MODE_LCB)
        if (lcb != ccp_compute_stacker_lcb(dest_p, dest_len)){
            if (ppp_error_debug)
                buginf("\n%s:CCP: Stacker Decomp - LCB match failed.",
                       idb->hw_namestring);
            datagram_done(newpak);
            ccp_decompress_stacker_failure(idb);
            return(FALSE);
        }

    newpak->datagramsize = (ushort) dest_len + PPP_PROTO_HDR_ENCAPBYTES;

    /*
     * Update stats
     */
    if (idb->comp_db){
        idb->comp_db->comp_rcv_out_bytes += (max_dest_len - max_dest_len_wk);
        idb->comp_db->compressed_rcvd += (*pak)->datagramsize;
    }

    datagram_done(*pak);
    *pak = newpak;
  
    return(TRUE);
}

/*
 * LZS DCP's own method of history resetting.
 */
static void ccp_send_request_reset_lzs_dcp (hwidbtype *hwidb)
{
    paktype *pak;
    lzsdcpHistory *hdr;
    ppp_dll_hdr *dll;
    int payload_size;

    hdr = (lzsdcpHistory *)hwidb->ppp_decomp_history;
    hdr->flags |= LZS_DCP_RESET_REQ;

    /*
     * Update compression stats
     */
     if (hwidb->comp_db)
         hwidb->comp_db->comp_restarts ++;

    /*
     * Start a timer to make sure we get the reset acknowledge.
     */
    TIMER_START(hdr->ResetAckTimer, (3 * ONESEC));

    /*
     * For now just the DCP octect. Later this will need to take into account
     * the history number field when we support mutliple histories.
     */
    payload_size = sizeof(uchar);

    /*
     * We need to build up an empty CCP data packet to carry the DCP header
     * with the reset request flag.
     */
    if ((pak = (paktype *)getbuffer(DLLHEADERLEN + payload_size)) == NULL){
        /*
         * Failed to get a packet to send the request.
         * Leave it and let the timer deal with it.
         */
        return;
    }
    pak->if_output = hwidb->firstsw;
    pak->linktype = LINK_PPP;
    pak->datagramsize = DLLHEADERLEN + sizeof(uchar);
    pak->datagramstart = pak->network_start - DLLHEADERLEN;

    dll = (ppp_dll_hdr *)pak->datagramstart;
    dll->address = PPP_ADDRESS;
    dll->control = PPP_CONTROL;
    dll->type    = TYPE_PPP_COMPRESS;
    dll->data[0] = LZS_DCP_HEADER_FIXED | LZS_DCP_RESET_REQ;
    pak->enctype = ET_PPP;
    pak->encsize = DLLHEADERLEN;
    pak->flags |= PAK_PRIORITY; /* Make it get there ASAP */
    
    /*
     * If PPP is using an extra layer of encapsulation, ie. LAPB, then the
     * packet should be output using the idb oqueue vector.
     */
    if (hwidb->ppp_lapb)
        (*hwidb->oqueue)(hwidb, pak, TAIL);
    else
    {
        if (hwidb->board_encap)
        if (!(*hwidb->board_encap)(pak,hwidb)) {
            retbuffer(pak);
            return;
        }
        datagram_out(pak);
    }
}

/*
 * PPP interface to Stacker LZS Compression with Motorola's DCP header.
 */
static boolean ccp_compress_lzs_dcp (paktype *pak)
{
    hwidbtype *idb;
    lzsdcpHistory *hdr;
    uchar *src_p, *dest_p, *tail_p;
    uchar *src_p_wk, *dest_p_wk;
    ulong src_len, src_len_wk;
    ulong dest_len;
    ulong max_dest_len;
    ulong max_dest_len_wk;
    ushort result;
    uchar *buffer;
    int proto_hdr_size,  proto_tail_size;
    uchar lcb, dcp_hdr;
  
    idb = pak->if_output->hwptr;

    if (!idb->ppp_comp_history){
        if (ppp_error_debug)
            buginf("\n%s:CCP: LZS DCP Comp - No compression history!",
                   idb->hw_namestring);
        return(FALSE);
    }
  
    if (!idb->ppp_comp_wrkspc){
        if (ppp_error_debug)
            buginf("\n%s:CCP: LZS DCP Comp - No compression workspace!",
                   idb->hw_namestring);
        return(FALSE);
    }
  
    buffer = idb->ppp_comp_wrkspc;
    hdr = (lzsdcpHistory *)idb->ppp_comp_history;
    lcb = 0; /* Keeps the compiler happy */
    dcp_hdr = LZS_DCP_HEADER_FIXED;

    /*
     * Have we been told to reset our compression history?
     */
    if (hdr->flags & LZS_DCP_RESET_REQ){
        LZS_InitCompressionHistory(hdr->history);
        hdr->flags &= ~LZS_DCP_RESET_REQ;
        dcp_hdr |= LZS_DCP_RESET_ACK;
        /*
         * Update compression stats
         */
        if (idb->comp_db)
            idb->comp_db->comp_restarts ++;
        if (ppp_error_debug)
            buginf("\n%s:CCP: LZS DCP. Reset Requested.",
                    idb->hw_namestring);
    }

    proto_hdr_size = PPP_PROTO_HDR_ENCAPBYTES + sizeof(uchar);
    proto_tail_size = 0;

    src_len = pak->datagramsize;
    src_p = pak->datagramstart;

    /*
     * Do we need to leave space for a check value?
     */
    switch(hdr->check_mode){
        case LZSDCP_CHECK_MODE_SEQ:
            proto_hdr_size += sizeof(uchar);
        break;

        case LZSDCP_CHECK_MODE_SEQ_LCB:
            proto_hdr_size += sizeof(uchar);
        case LZSDCP_CHECK_MODE_LCB:
            /*
             * Get the LCB of the uncompressed data.
             */
            proto_tail_size = 1;
            lcb = ccp_compute_stacker_lcb(src_p, src_len);
        break;

        default:
        break;
    }

    max_dest_len = hdr->wrk_spc_size;

    dest_p = buffer;
  
    /* LZS_Compress() modifies the following 4 copied variables */
    max_dest_len_wk = max_dest_len;
    src_p_wk = src_p;
    dest_p_wk = dest_p + proto_hdr_size;
    src_len_wk = src_len;
  
    result = LZS_Compress(&src_p_wk, &dest_p_wk, &src_len_wk,
                          &max_dest_len_wk, hdr->history,
                         (LZS_SAVE_HISTORY | LZS_SOURCE_FLUSH |
                          LZS_DEST_FLUSH | LZS_PERFORMANCE_MODE_1),
                         3);
  
    /*
     * Check compression routine's return value is what should be expected.
     */
    if ((result == LZS_INVALID) ||
       ((result & LZS_DEST_EXHAUSTED) && !(result & LZS_SOURCE_EXHAUSTED))){
        LZS_InitCompressionHistory(hdr->history);
        if (ppp_error_debug)
            buginf("\n%s:CCP: LZS DCP Comp - LZS_Compress return value bad. 0x%x ",
                       idb->hw_namestring, result);
       /*
        * Update stats
        */
       if (idb->comp_db)
           idb->comp_db->comp_restarts ++;
  
       /*
        * Return TRUE since we don't need to reset the other side.
        */
       return(TRUE);
    }

    dest_len = max_dest_len - max_dest_len_wk;

    /*
     * Check for anti-compression.
     */
    if (dest_len > src_len){
        /*
         * Update stats
         */
        if (idb->comp_db)
            idb->comp_db->uncompressed_sent += src_len;
        pak->datagramsize += proto_hdr_size;
        pak->datagramstart -= proto_hdr_size;
        dest_p = pak->datagramstart;
        PPP_PUTSHORT(TYPE_PPP_COMPRESS, dest_p);
        PUTCHAR(dcp_hdr, dest_p);
        /*
         * Add check value(s).
         */
        switch(hdr->check_mode){
            case LZSDCP_CHECK_MODE_SEQ:
            case LZSDCP_CHECK_MODE_SEQ_LCB:
                PUTCHAR((hdr->seq_num ++), dest_p);
            break;

            case LZSDCP_CHECK_MODE_LCB:
                /* No LCB added to uncompressed packets */
            break;

            default:
            break;
        }
    }else{
        /*
         * Update stats
         */
        if (idb->comp_db){
            idb->comp_db->comp_xmt_in_bytes += src_len;
            idb->comp_db->compressed_sent += dest_len;
        }
        dcp_hdr |= LZS_DCP_COMPRESSED;
        pak->datagramsize = dest_len + proto_hdr_size + proto_tail_size;
        PPP_PUTSHORT(TYPE_PPP_COMPRESS, dest_p);
        PUTCHAR(dcp_hdr, dest_p);
        /*
         * Add check value(s) to the head and tail.
         */
        switch(hdr->check_mode){
            case LZSDCP_CHECK_MODE_SEQ:
                PUTCHAR((hdr->seq_num ++), dest_p);
            break;

            case LZSDCP_CHECK_MODE_SEQ_LCB:
                PUTCHAR((hdr->seq_num ++), dest_p);
            case LZSDCP_CHECK_MODE_LCB:
                tail_p = buffer + (pak->datagramsize - proto_tail_size);
                PUTCHAR(lcb, tail_p);
            break;

            default:
            break;
        }
        bcopy(buffer, pak->datagramstart, pak->datagramsize);
    }
    pak->linktype = LINK_PPP_COMPRESSION;
    pak->encsize = proto_hdr_size;
    return(TRUE);
}


/*
 * PPP interface to Stacker LZS Decompression with Motorola's DCP header.
 */
static boolean ccp_decompress_lzs_dcp (paktype **pak)
{
    hwidbtype *idb = (*pak)->if_input->hwptr;
    lzsdcpHistory *hdr;
    lzsdcpHistory *compressor;
    uchar *src_p, *src_p_wk, *dest_p, *dest_p_wk, *tail_p;
    ulong src_len, src_len_wk, dest_len, max_dest_len, max_dest_len_wk;
    ushort result, flags;
    paktype *newpak;
    uchar seq_num, lcb, dcp_hdr;
    boolean compressed, reset_requested, reset_acknowledged, reset_history;

    if (!idb->ppp_decomp_history){
        if (ppp_error_debug)
            buginf("\n%s:CCP: LZS DCP Decomp - No decompression history!",
                   idb->hw_namestring);
        return(FALSE);
    }

    hdr = (lzsdcpHistory *)idb->ppp_decomp_history;  
    lcb = 0; /* Keeps the compiler happy */
    src_p = (*pak)->datagramstart;
    src_len = (*pak)->datagramsize;

    /*
     * Isolate the flags in the DCP header.
     */
    GETCHAR(dcp_hdr, src_p);
    src_len -= sizeof(uchar);
    reset_requested = dcp_hdr & LZS_DCP_RESET_REQ;
    reset_acknowledged = dcp_hdr & LZS_DCP_RESET_ACK;
    compressed = dcp_hdr & LZS_DCP_COMPRESSED;
    reset_history = FALSE;

    /*
     * If Reset Requested we must notify our compressor that it must initialise
     * and respond.
     */
    if (reset_requested && idb->ppp_comp_history){
        compressor = (lzsdcpHistory *)idb->ppp_comp_history;
        compressor->flags |= LZS_DCP_RESET_REQ;
    }

    /*
     * Are we awaiting a Reset Acknowledge?
     */
    if (hdr->flags & LZS_DCP_RESET_REQ){
        if (reset_acknowledged){
            LZS_InitDecompressionHistory(hdr->history);
            hdr->flags &= ~LZS_DCP_RESET_REQ;
            reset_history = TRUE;
            TIMER_STOP(hdr->ResetAckTimer);
        }else{
            /*
             * Other side hasn't indicated that it has reset its compression
             * history yet so discard this packet. Check timer on Reset Ack.
             */
             if (TIMER_RUNNING_AND_AWAKE(hdr->ResetAckTimer)){
                 /*
                 * Our timer has expired and we given up waiting for our Reset
                 * Ack. Stop the timer and send another request.
                 */
                 TIMER_STOP(hdr->ResetAckTimer);
                 ccp_send_request_reset_lzs_dcp(idb);
                if (ppp_error_debug)
                    buginf("\n%s:CCP: LZS DCP. Reset Request Timeout.",
                            idb->hw_namestring);
             }
             return(FALSE);
        }
    }

    /*
     * It is possible that we received a packet containing only the DCP header
     * and no data & check field.
     */
    if (src_len == 0)
        return(FALSE);

    /*
     * If we negotiated the use of a check value then extract it and, if
     * possible at this point, check it.
     */
    switch(hdr->check_mode){
        case LZSDCP_CHECK_MODE_LCB:
            if (compressed){
                src_len -= sizeof(uchar);
                tail_p = src_p + src_len;
                GETCHAR(lcb, tail_p);
            }
        break;

        case LZSDCP_CHECK_MODE_SEQ_LCB:
            if (compressed){
                src_len -= sizeof(uchar);
                tail_p = src_p + src_len;
                GETCHAR(lcb, tail_p);
            }
        case LZSDCP_CHECK_MODE_SEQ:
            GETCHAR(seq_num, src_p);
            src_len -= sizeof(uchar);
            /*
             * If this packet contain a reset acknowledge and our history was
             * reset then we don't need to check the sequence number. Instead
             * we set our count of the sequence number to what was contained in
             * the header. 
             */
            if (reset_history)
                hdr->seq_num = seq_num;
            else
                if (seq_num != hdr->seq_num){
                /*
                 * If the packet is out of sequence then return with an error.
                 */
                    if (ppp_error_debug)
                        buginf("\n%s:CCP: LZS Decomp Error."
                                " Expected pkt# %d got %d",
                               idb->hw_namestring, hdr->seq_num, seq_num);
                    ccp_send_request_reset_lzs_dcp(idb);
                    return(FALSE);
                 }    
             hdr->seq_num ++;
        break;

        default:
        break;
    }

    if ((long)src_len <= 0){    /* for zero-length incoming paks */
        ccp_send_request_reset_lzs_dcp(idb);
        return(FALSE);
    }
  
    /*
     * Zero Deletion/Insertion: Need to add a trailing zero to guarantee that
     * we have an end marker on all compressed packets.
     */
    if (compressed){
        tail_p = src_p + src_len;
        PUTCHAR(0, tail_p);
        src_len ++;
    }

    if ((newpak = getbuffer(idb->maxmtu + PPP_ENCAPBYTES)) == NULL){
        if (idb->comp_db)
            idb->comp_db->comp_rcv_no_bufs++;
        ccp_send_request_reset_lzs_dcp(idb);
        return(FALSE);
    }

    pak_copy(*pak, newpak, 0);
    dest_p = pak_center(newpak);
    newpak->encsize = PPP_ENCAPBYTES;
    newpak->network_start = dest_p;
    dest_p -= newpak->encsize;    /* protocol data starts on word boundary */ 
    newpak->datagramstart = dest_p;
    PPP_PUTSHORT(PPP_STATION, dest_p);
    dest_p_wk = dest_p;
    src_len_wk = src_len;
    src_p_wk = src_p;

    if (compressed){
        flags = LZS_SAVE_HISTORY;
        max_dest_len = idb->maxmtu + PPP_PROTO_HDR_ENCAPBYTES;
    }else{
        flags = LZS_SAVE_HISTORY | LZS_UNCOMPRESSED;
        max_dest_len = src_len;
    }

    max_dest_len_wk = max_dest_len;

    result = LZS_Decompress(&src_p_wk, &dest_p_wk, &src_len_wk, &max_dest_len_wk,
                            hdr->history, flags);

    if (compressed){
        if ((result & LZS_DEST_EXHAUSTED) && !(result & LZS_END_MARKER)){
            if (ppp_error_debug)
                buginf("\n%s:CCP: LZS DCP Decomp - Destination buffer exhausted.",
                       idb->hw_namestring);
            datagram_done(newpak);
            ccp_send_request_reset_lzs_dcp(idb);
            return(FALSE);
        }
        if (!(result & LZS_END_MARKER)){
            if (ppp_error_debug)
                buginf("\n%s:CCP: LZS DCP Decomp - LZS end marker not reached.",
                       idb->hw_namestring);
                datagram_done(newpak);
                ccp_send_request_reset_lzs_dcp(idb);
                return(FALSE);
        }
        dest_len = max_dest_len - max_dest_len_wk;
    }else
        dest_len = src_len;

    /*
     * If we are using LCB as the check mode then get the LCB for the data
     * we just uncompress and compare it to what we should be expecting.
     */
    if (hdr->check_mode == LZS_CHECK_MODE_LCB)
        if (lcb != ccp_compute_stacker_lcb(dest_p, dest_len)){
            if (ppp_error_debug)
                buginf("\n%s:CCP: LZS Decomp - LCB match failed.",
                       idb->hw_namestring);
            datagram_done(newpak);
            ccp_send_request_reset_lzs_dcp(idb);
            return(FALSE);
        }

    newpak->datagramsize = (ushort) dest_len + PPP_PROTO_HDR_ENCAPBYTES;

    /*
     * Update stats
     */
    if (idb->comp_db){
        if (compressed){
            idb->comp_db->comp_rcv_out_bytes += (max_dest_len
                                                 - max_dest_len_wk);
            idb->comp_db->compressed_rcvd += (*pak)->datagramsize;
        }else
            idb->comp_db->uncompressed_rcvd += (*pak)->datagramsize;            
    }

    datagram_done(*pak);
    *pak = newpak;  

    return(TRUE);
}


/*
 * PPP interface to Predictor1 Compression
 */
static boolean ccp_compress_predictor1 (paktype *pak)
{
    hwidbtype *idb;
    pred1History *hdr;
    uchar *src_p, *dest_p;
    ulong src_len, dest_len;
    uchar compressed_bit;
    uchar *buffer;
    ushort comp_n_length;
    ushort crc16;
    uchar twobytes[2];
    
    idb = pak->if_output->hwptr;
    
    if (!idb->ppp_comp_history){
        if (ppp_error_debug)
            buginf("\n%s:CCP: Predictor Comp - No compression history!",
                 idb->hw_namestring);
        return(FALSE);
    }
    
    if (!idb->ppp_comp_wrkspc){
        if (ppp_error_debug)
            buginf("\n%s:CCP: Predictor Comp - No compression workspace!",
                 idb->hw_namestring);
        return(FALSE);
    }
    
    buffer = idb->ppp_comp_wrkspc;
    hdr = (pred1History *)idb->ppp_comp_history;
    
    /*
     * Has a compression history reset been requested by the other side?
     */
    if (hdr->reset_requested){
        hdr->hash = PRED1_HASH_MASK_INIT;
        memset(hdr->guess_table, 0, GUESS_TABLE_SIZE);
        hdr->reset_requested = FALSE;
        /*
         * Update compression stats
         */
        if (idb->comp_db)
            idb->comp_db->comp_restarts ++;
    }

    src_len = pak->datagramsize;
    src_p = pak->datagramstart;
    
    /*
     * Update stats
     */
    if (idb->comp_db)
        idb->comp_db->comp_xmt_in_bytes += src_len;

    /*
     * Generate a CRC16 of the two byte length field in network order followed
     * by the original uncompressed data.
     */
    twobytes[0] = ((ushort)(src_len)) >> 8;
    twobytes[1] = ((ushort)(src_len)) & 0xff;
    crc16 = pppfcs(PPPINITFCS, twobytes, 2);
    crc16 = PPPINITFCS ^ pppfcs(crc16, src_p, (int)src_len);
    
    dest_p = buffer + CCP_PRED1_HEADERBYTES + PPP_PROTO_HDR_ENCAPBYTES;
    compressed_bit = compress_rand(hdr->guess_table, &hdr->hash,
                                   src_p, src_len, dest_p, &dest_len);
    
    if (compressed_bit & COMP_ANTI_EXPANSION)
    {
        /*
         * Anti-Compression has occured so send the original packet with
         * CCP encapsulation. Predictor needs to parse it on the other side to
         * maintain history sync.
         * NOTE: COMP_ANTI_EXPANSION bit set means expansion. See comp_rand.c
         * code. It doesn't make sense in English, but hey we're engineers...
         */
        pak->datagramsize += PPP_PROTO_HDR_ENCAPBYTES + CCP_PRED1_HEADERBYTES
                             + CCP_PRED1_TAILBYTES;
        pak->linktype = LINK_PPP_COMPRESSION;
        pak->datagramstart -= PPP_PROTO_HDR_ENCAPBYTES + CCP_PRED1_HEADERBYTES;
        dest_p = pak->datagramstart;
        PPP_PUTSHORT(TYPE_PPP_COMPRESS, dest_p);
        /*
         * First 2 bytes are Predictor header bytes.    These bytes contain a 15
         * bit length field. The LSBit (bit 0) indicates if the payload packet
         * is compressed or not
         */
        comp_n_length = src_len & PRED1_LEN_MASK;
        comp_n_length &= ~(PRED1_COMP_BIT_MASK); /* Clear anti-exp bit */
        PPP_PUTSHORT(comp_n_length, dest_p);
        dest_p += src_len;        /* Point to the end of the data */
        PRED1_PUTCRC16(crc16, dest_p);
        /*
         * Update stats
         */
        if (idb->comp_db)
            idb->comp_db->uncompressed_sent += dest_len;
    }else{
        pak->datagramsize = dest_len + PPP_PROTO_HDR_ENCAPBYTES +
                            CCP_PRED1_HEADERBYTES + CCP_PRED1_TAILBYTES;
        pak->linktype = LINK_PPP_COMPRESSION;    
        dest_p = buffer;
        PPP_PUTSHORT(TYPE_PPP_COMPRESS, dest_p);
            
        /*
         * First 2 bytes are Predictor header bytes.    These bytes contain a 15
         * bit length field. The LSBit (bit 0) indicates if the payload packet
         * is compressed or not
         */
        comp_n_length = src_len & PRED1_LEN_MASK;
        comp_n_length |= PRED1_COMP_BIT_MASK; /* Set anti-exp bit */
        PPP_PUTSHORT(comp_n_length, dest_p);
        dest_p += dest_len;        /* Point to the end of the data */
        PRED1_PUTCRC16(crc16, dest_p);
        bcopy(buffer, pak->datagramstart, pak->datagramsize);
        /*
         * Update stats
         */
        if (idb->comp_db)
            idb->comp_db->compressed_sent += dest_len;
    }

    return(TRUE);
}


/*
 * Deals with a decompression error/failure. Need to indicate to the other side
 * that the history is corrupt and needs reseting.
 */
static void ccp_decompress_predictor1_failure (hwidbtype *idb)
{
    pred1History *hdr;
    
    hdr = (pred1History *)idb->ppp_decomp_history;

    if (hdr->legacy_support){
        ccp_compression_legacy_restart(idb);
        return;
    }

    hdr->reset_id ++;
    hdr->reset_requested = TRUE;

    if (ppp_error_debug)
        buginf("\n%s:CCP: Predictor - Sending Reset Request to peer",
                    idb->hw_namestring);
    ccp_compression_request_reset(idb, hdr->reset_id, NULL, 0);
}

/*
 * PPP interface to Predictor1 Decompression
 */
static boolean ccp_decompress_predictor1 (paktype **pak)
{
    hwidbtype *idb = (*pak)->if_input->hwptr;
    pred1History *hdr;
    uchar *tail_p;
    uchar *src_p;
    uchar *dest_p;
    ulong src_len;
    ulong dest_len;
    ulong expected_len;
    paktype *newpak;
    ushort comp_n_length;
    ushort compressed_bit;
    uchar twobytes[2];
    ushort rcv_crc16;
    ushort gen_crc16;

    if (!idb->ppp_decomp_history){
        if (ppp_error_debug)
            buginf("\n%s:CCP: Predictor Decomp - No decompression history!",
                   idb->hw_namestring);
        return(FALSE);
    }
    
    hdr = (pred1History *)idb->ppp_decomp_history;
    
    /*
     * First 2 bytes are Predictor header bytes.    These bytes contain a 15 bit
     * length field. The LSBit (bit 0) indicates if the payload packet is
     * compressed or not
     */
    src_p = (*pak)->datagramstart;
    PPP_GETSHORT(comp_n_length, src_p);
    compressed_bit = comp_n_length & PRED1_COMP_BIT_MASK;
    expected_len = comp_n_length & PRED1_LEN_MASK;
    
    /* Point to last two bytes, this should be the CRC16 */
    tail_p = (*pak)->datagramstart + (*pak)->datagramsize - CCP_PRED1_TAILBYTES;
    PRED1_GETCRC16(rcv_crc16, tail_p);
    
    src_len = (*pak)->datagramsize - (CCP_PRED1_HEADERBYTES
                                      + CCP_PRED1_TAILBYTES);

    if ((newpak = getbuffer(expected_len + PPP_ENCAPBYTES)) == NULL){
        if (idb->comp_db)
            idb->comp_db->comp_rcv_no_bufs++;
        ccp_decompress_predictor1_failure(idb);
        return(FALSE);
    }
    pak_copy(*pak, newpak, 0);
    dest_p = pak_center(newpak);
    newpak->encsize = PPP_ENCAPBYTES;
    newpak->network_start = dest_p;
    dest_p -= newpak->encsize;        /* protocol data will start on word boundary */ 
    newpak->datagramstart = dest_p;
    PPP_PUTSHORT(PPP_STATION, dest_p); /* ppp_parse_packet needs this header */

    if (compressed_bit){
        /*
         * This packet payload is compressed.
         */
        decompress_rand(hdr->guess_table, &hdr->hash, src_p, src_len,
                        dest_p, &dest_len);
        if (idb->comp_db)
            idb->comp_db->compressed_rcvd += (*pak)->datagramsize;
    }else{
        /*
         * This packet payload is not compressed but the dictionary needs
         * updating.
         */
        decompress_rand_fixup(hdr->guess_table, &hdr->hash, src_p,
                              src_len, dest_p);
        dest_len = src_len;
        if (idb->comp_db)
            idb->comp_db->uncompressed_rcvd += dest_len;
    }

    /*
     * Update stats
     */
    if (idb->comp_db)
        idb->comp_db->comp_rcv_out_bytes += dest_len;

    newpak->datagramsize = dest_len + PPP_PROTO_HDR_ENCAPBYTES;
    datagram_done(*pak);
    *pak = newpak;

    /* Check that our lengths match up */
    if (dest_len != expected_len){
        if (ppp_error_debug){
            buginf("\n%s:CCP: ccp_decompress_predictor1: Decompressed to wrong length!");
            buginf("\n%s:CCP: expected length = %d, decompressed length = %d",
                   idb->hw_namestring, comp_n_length, dest_len);
        }
        ccp_decompress_predictor1_failure(idb);
        return(FALSE);
    }

    /*
     * Finally check that the data decompressed passes the CRC check.
     * The CRC16 covers the two byte length field in network order followed
     * by the original uncompressed data.
     */
    twobytes[0] = ((ushort)dest_len) >> 8;
    twobytes[1] = ((ushort)dest_len) & 0xff;
    gen_crc16 = pppfcs(PPPINITFCS, twobytes, 2);
    gen_crc16 = pppfcs(gen_crc16, dest_p, (int)dest_len);
    /*
     * Do not use network byte order for processing the received crc16. It
     * should be processed LSB first.
     */
    twobytes[0] = rcv_crc16 & 0xff;
    twobytes[1] = rcv_crc16 >> 8;
    if ((gen_crc16 = pppfcs(gen_crc16, twobytes, 2)) != PPPGOODFCS){
        if (ppp_error_debug)
            buginf("\n%s:CCP: ccp_decompress_predictor1: bad CRC16 0x%04x",
                   idb->hw_namestring, gen_crc16);
        ccp_decompress_predictor1_failure(idb);
        return(FALSE);
    }

    return(TRUE);
}

/*
 * ccp_init_subsys
 * Initialization of CCP subsystem
 */

static void ccp_init_subsys (subsystype *subsys)
{
    ccp_pid = NO_PROCESS;

    reg_invoke_ncp_register(LINK_PPP_COMPRESSION, TYPE_PPP_CCP,
                ccp_init, NULL, "CCP");
    reg_add_proto_on_swidb(LINK_PPP_COMPRESSION, ccp_compression_running,
               "ccp_compression_running");
    reg_add_ccp_compress_outgoing_pkt(ccp_compress_outgoing_pkt,
                      "ccp_compress_outgoing_pkt");
}

/*
 * The CCP subsystem header
 */

#define CCP_MAJVERSION 1
#define CCP_MINVERSION 0
#define CCP_EDITVERSION  1

SUBSYS_HEADER(ccp, CCP_MAJVERSION, CCP_MINVERSION, CCP_EDITVERSION,
          ccp_init_subsys, SUBSYS_CLASS_PROTOCOL,
          "seq: ppp",
          "req: compress, ppp");
