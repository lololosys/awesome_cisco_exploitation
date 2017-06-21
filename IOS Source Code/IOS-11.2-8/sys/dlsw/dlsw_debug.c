/* $Id: dlsw_debug.c,v 3.6.18.8 1996/08/10 01:03:56 athippes Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_debug.c,v $
 *------------------------------------------------------------------
 * Debugging routines for DLSw
 *
 * July 1994, Frank Bordonaro 
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Processes DLSw executive commands (debug, show, clear, etc...)
 *------------------------------------------------------------------
 * $Log: dlsw_debug.c,v $
 * Revision 3.6.18.8  1996/08/10  01:03:56  athippes
 * CSCdi63699:  Adding a hidden knob - dlsw llc2 nornr
 * Branch: California_branch
 *
 * Revision 3.6.18.7  1996/08/06  18:46:47  kpatel
 * CSCdi65134:  Crash when doing show dlsw cap local in router w/no
 *              dlsw configured
 * Branch: California_branch
 *
 * Revision 3.6.18.6  1996/07/25  09:43:14  snyder
 * CSCdi63981:  eliminate unused code, fix bugs, make a common routine
 *              1. create a routine call generic_debug_all and
 *                 make most debugs use it
 *              2. eliminate if (*_debug_inited) return, most never set
 *              the var
 *                 to TRUE making the code useless.
 *              3. declare some constant arrays const
 *              4. fix bugs found along the way
 *              5. save 2768 bytes from image, 660 out of data section
 * Branch: California_branch
 *
 * Revision 3.6.18.5  1996/07/23  22:09:35  athippes
 * CSCdi63699:  Adding a hidden knob - dlsw llc2 nornr
 *              Using Registry stub.
 * Branch: California_branch
 *
 * Revision 3.6.18.4  1996/07/23  06:29:46  athippes
 * CSCdi63699:  Adding a hidden knob - dlsw llc2 nornr
 * Branch: California_branch
 *
 * Revision 3.6.18.3  1996/07/19  03:55:22  athippes
 * CSCdi60758:  dlsw : duplicate circuit identifier
 * Branch: California_branch
 *
 * Revision 3.6.18.2  1996/05/30  14:43:33  sbales
 * CSCdi59064:  DLSw+: show commands need to be more usable
 *              modify options for "sh dls reach", "sh dls cir", "sh dls
 *              loc", modify formatting for "show dls peer"
 * Branch: California_branch
 *
 * Revision 3.6.18.1  1996/05/17  10:45:35  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.4.4  1996/04/26  07:34:15  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.6.4.3  1996/04/24  18:35:10  fbordona
 * CSCdi55588:  DLSw: Use managed timers for connecting dynamic peers
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.4.2  1996/04/23  20:13:45  fbordona
 * CSCdi55437:  DLSw peer may take several minutes to connect after reload
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.4.1  1996/04/20  02:08:03  fbordona
 * CSCdi55177:  DLSw phase I to convert peer to managed timers
 *              Required for scalability of large DLSw peer nets.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6  1996/02/07  16:10:59  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.5  1996/02/01  06:01:24  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/18  22:24:40  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/11/17  09:03:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:21:48  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:23:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:55:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/30  18:52:23  widmer
 * CSCdi36635:  nv_add string should not contain newline
 * Fix dlsw NV generation
 *
 * Revision 2.1  1995/06/07  20:26:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "interface_generic.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "../dlsw/parser_defs_dlsw.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../tcp/tcpdriver.h"
#include "../if/network.h"

#include "../ibm/libibm_externs.h"
#include "../ibm/netbios.h"
#include "../srt/rsrb.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../ui/debug.h"
#include "../dlsw/dlsw_debug.h"
#include "../dlsw/dlsw_core_debug.h"
#include "../dlsw/dlsw_local_externs.h"
#include "../dlsw/dlsw_csm_externs.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "dlsw_debug_flags.h"

/* Other initialized storage */
boolean dlsw_peer_debug = FALSE;
boolean dlsw_core_debug = FALSE;
boolean dlsw_csm_debug = FALSE;
boolean dlsw_local_debug = FALSE;

/*
 * File scope function prototypes 
 */
static void dlsw_debug_all (boolean sense);
static void parse_show_csm (parseinfo *csb);
static void parse_show_peer (parseinfo *csb);
static void parse_debug_peer (parseinfo *csb);
static void parse_debug_csm (parseinfo *csb);

extern void csm_show_cache (int, char *, int);
extern void dlsw_fct_clear_cache (void);

/*
 * Global Data
 */
boolean dlsw_disable = FALSE;
boolean dlsw_nornr = FALSE;

/*
 * ---------------------------------------------------------------
 * Debug Functions
 * ---------------------------------------------------------------
 */

void dlsw_debug_init ()
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(dlsw_debug_all, "dlsw_debug_all");
    reg_add_debug_show(show_dlsw_debug, "show_dlsw_debug");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when DLSw is initialized, we want to turn on
     * all the DLSw debugging right now.
     */
    dlsw_debug_all(debug_all_p());
    dlsw_core_debug_init();
}


/*
 * dlsw_debug_all()
 */

static void dlsw_debug_all (boolean sense)
{
    csm_debug_all(FALSE, sense);
    peer_debug_cmd(FALSE, 0, NULL, sense);
    local_debug(FALSE, sense, -1);
}


void debug_dlsw (parseinfo *csb) {

    switch (GETOBJ(int,1)) {
      case DEBUG_DLSW_CORE:
        parse_debug_core(csb);
        break;

      case DLSW_DEBUG_PEER:
        parse_debug_peer(csb);
        break;

      case DLSW_DEBUG_CSM:
        parse_debug_csm(csb);
        break;

      case DLSW_DEBUG_LOCAL:
        local_debug(TRUE, csb->sense, GETOBJ(int,3));
        break;

      case DLSW_DEBUG_ALL:
        csm_debug_all(TRUE, csb->sense);
        peer_debug_cmd(TRUE, 0, csb, csb->sense);
        local_debug(TRUE, csb->sense, -1);
	parse_debug_core(csb);
        break;

      default:
        bad_parser_subcommand(csb, csb->which);
        break;
    }
}

/*
 * parse_debug_csm()
 */

static void parse_debug_csm (parseinfo *csb)
{
    csm_debug(GETOBJ(int,3), GETOBJ(int,2), TRUE, csb->sense);
    return;
}


/*
 * parse_debug_peer()
 */

static void parse_debug_peer (parseinfo *csb)
{
    peerhandle_t  ph;

    switch (GETOBJ(int,2)) {
      case DLSW_DEBUG_IP_PEER:
        ph = peer_ip_to_handle(GETOBJ(paddr,1)->ip_addr);
        if (ph)
            peer_debug_cmd(TRUE, ph, csb, csb->sense);
        else 
            printf("\nNo remote peers known by ip address %i",
                    GETOBJ(paddr,1)->ip_addr);
        break;

      case DLSW_DEBUG_INT_PEER:
        ph = peer_hwidb_to_handle(GETOBJ(idb,1)->hwptr);
        if (ph)
            peer_debug_cmd(TRUE, ph, csb, csb->sense);
        else
            printf("\nNo remote peers defined on interface %s",
                    GETOBJ(idb,1)->hwptr->hw_namestring);
        break;

      case DLSW_DEBUG_ALL_PEERS:
        dlsw_peer_debug = csb->sense;
        peer_debug_cmd(TRUE, 0, csb, csb->sense);
        break;

      default:
        printf("\nIllegal debug peers type");
        break;
    }
}


/*
 * ---------------------------------------------------------------
 * Show Functions
 * ---------------------------------------------------------------
 */

void show_dlsw_debug(void)
{
    /* First show the flag that follow the standard scheme */
    debug_show_flags(&(dlsw_debug_arr[0]), "DLSw");

    /* show each DLSw component's debug */
    show_csm_debug();
    show_peer_debug();
    show_core_debug();
    show_local_debug();
}


void show_dlsw (parseinfo *csb) {

    if (!(dlsw_peer_active(0, 0, 0)))
      return;

    switch (GETOBJ(int,1)) {
      case DLSW_SHOW_CORE:
        parse_show_core(csb);
        break;

      case DLSW_SHOW_CSM:
        parse_show_csm(csb);
        break;

      case DLSW_SHOW_PEER:
      case DLSW_SHOW_CAP:
        parse_show_peer(csb);
        break;

      case DLSW_SHOW_FAST:
        dlsw_show_fast_cache();
        break;

      case DLSW_SHOW_LOC_CKT:
        dlsw_show_local_ckt(csb);
        break;

      default:
        bad_parser_subcommand(csb, csb->which);
        break;
    }
}


/*
 * parse_show_csm()
 */

static void parse_show_csm (parseinfo *csb)
{

    extern boolean  dlsw_started;

    if (!dlsw_started) {
      printf("\nDLSw Not yet started.....");
      return;
    }
    if (GETOBJ(int,2) != DLSW_SHOW_CSM_NETB) {
        if (GETOBJ(int,3) != DLSW_SHOW_CSM_REMOTE)
            csm_show_cache(LCACHE_MAC_FLAG, GETOBJ(hwaddr,1)->addr,
                           GETOBJ(int,4));
        if (GETOBJ(int,3) != DLSW_SHOW_CSM_LOCAL)
            csm_show_cache(RCACHE_MAC_FLAG, GETOBJ(hwaddr,1)->addr,
                           GETOBJ(int,4));
    }
    if (GETOBJ(int,2) != DLSW_SHOW_CSM_MAC) {
        if (GETOBJ(int,3) != DLSW_SHOW_CSM_REMOTE)        
            csm_show_cache(LCACHE_NAME_FLAG, GETOBJ(string,1),
                          GETOBJ(int,4));
        if (GETOBJ(int,3) != DLSW_SHOW_CSM_LOCAL)        
            csm_show_cache(RCACHE_NAME_FLAG, GETOBJ(string,1),
                          GETOBJ(int,4));
    }
    return;
}



/*
 * parse_show_peer()
 */

static void parse_show_peer (parseinfo *csb)
{
    peerhandle_t ph;

    switch (GETOBJ(int,2)) {
      case DLSW_SHOW_BY_IP:
        ph = peer_ip_to_handle(GETOBJ(paddr,1)->ip_addr);
        if (ph) {
            if (GETOBJ(int,1) == DLSW_SHOW_PEER)
                show_remote_stats(ph);
            else 
                show_remote_capabilities(ph);
        } else
            printf("\nNo remote peers known by ip address %i",
                    GETOBJ(paddr,1)->ip_addr);
        break;

      case DLSW_SHOW_BY_INT:
        ph = peer_hwidb_to_handle(GETOBJ(idb,1)->hwptr);
        if (ph) {
            if (GETOBJ(int,1) == DLSW_SHOW_PEER)
                show_remote_stats(ph);
            else
                show_remote_capabilities(ph);
        } else
            printf("\nNo remote peers defined on interface %s",
                    GETOBJ(idb,1)->hwptr->hw_namestring);
        break;

      case DLSW_SHOW_LOCAL:
        print_capabilities(0);
        break;

      case DLSW_SHOW_ALL:
        if (GETOBJ(int,1) == DLSW_SHOW_PEER)
            show_remote_stats(0);
        else
            show_remote_capabilities(0);
        break;

      default:
        printf("\nIllegal show  peer or capabilities type");
        break;
    }
}


/*
 * ---------------------------------------------------------------
 * DLSw System configuration commands.
 * ---------------------------------------------------------------
 */

/*
 * dlsw_disable_command()
 *
 * This is a configuration command, but it is used for debug and
 * applies to all DLSw modules, therefore, this file is an appropriate
 * home for it.
 */

void dlsw_disable_command (parseinfo *csb)
{
    if (system_loading)
        return;

    if (!bridge_enable) {
        printf("\n%%DLSw not supported in this release");
        return;
    }

    if (csb->nvgen) {
        nv_write(dlsw_disable, "%s", csb->nv_command);
        return;
    }

    dlsw_disable = csb->sense;
    /*
     * Call each component's "disable" entry point. Each component
     * should terminate DLC or Peer sessions and remove all
     * session related data (clear caches, etc...)
     */
    dlsw_core_disable(csb);
#ifdef LATER
    dlsw_csm_disable(csb->sense);
#endif
    dlsw_peer_disable(csb->sense);
    if (dlsw_disable) {
        dlsw_local_disable();
    }
    if (!dlsw_disable) {
        /* dlsw is enabled, start explorer delay timer if enabled */
        csm_start_explr_delay_timer();
    }
}


void clear_dlsw (parseinfo *csb) {

    switch (GETOBJ(int,1)) {
      case CLEAR_DLSW_CACHE:
        parse_clear_cache(csb);
        break;

      case CLEAR_DLSW_CIRCUITS:
        parse_clear_circuits(csb);
        break;

      case CLEAR_DLSW_FAST:
        if (csb->sense)
            dlsw_fct_clear_cache();
        break;

      case CLEAR_DLSW_LOC_CKT:
        dlsw_clear_local_ckt(GETOBJ(int,3));
        break;

      default:
        bad_parser_subcommand(csb, csb->which);
        break;
    }
}

/*
 * dlsw_nornr_command()
 * This is used to disable xmit of RNR once the circuit comes up
 */

void dlsw_nornr_command (parseinfo *csb)
{
    if (csb->nvgen) {
        nv_write(dlsw_nornr, "%s", csb->nv_command);
        return;
    }
    dlsw_nornr = csb->sense;
}


