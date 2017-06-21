/* $Id: linkdown_event.c,v 3.4.54.3 1996/05/17 11:22:49 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/if/linkdown_event.c,v $
 *------------------------------------------------------------------
 * SNA Network Management
 * Ethernet, Token Ring and HDLC Link down handler
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1988-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: linkdown_event.c,v $
 * Revision 3.4.54.3  1996/05/17  11:22:49  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.42.2  1996/05/02  22:04:35  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.42.1  1996/04/03  14:38:00  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.54.2  1996/03/29  20:46:51  mordock
 * CSCdi51575:  no linkDown trap generated
 * Branch: California_branch
 * generate link down traps when an interface is administratively taken
 * down
 *
 * Revision 3.4.54.1  1996/03/18  20:14:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.22.2  1996/03/13  01:35:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.22.1  1996/02/20  14:23:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/11/20  22:25:23  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.3  1995/11/17  09:29:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:38:15  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:50:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/21  03:28:08  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  20:53:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Before this code is implemented, there is no failure cause when a
 * link is down. Most of the failure cause had to be passed from the
 * hardware. This is saved in the hwidb. 
 * All the LINK_DOWN_TRAP for Ethernet, Tokenring and Hdlc are removed
 * from the original code, they are replaced by setting up the error
 * code.
 * The checking of link state changed is done at net_cstate, periodic,
 * this is used to call LinkDownEvent when the failCause is set.
 * Fail cause of keep alive will not be used once there is a real cause.
 * To prevent too many traps and error be sent, same error will not be
 * sent.
 */


#include "master.h"
#include "interface_private.h"
#include "network.h"
#include "../snmp/snmp_api.h"
#include "../snmp/ifmib_registry.h"
#include "../dspu/snanm.h"
#include "../dspu/snanm_registry.regh"
#include "linkdown_event.h"



void LinkDownEvent (hwidbtype *idb)
{
    switch (idb->failCause) {

      case Ether_FatalTxError:
        idb->reason = "Fatal Tx Error";
        reg_invoke_Ether_Alert(Ether_Alert12, idb);
	reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
        break;
     
      case Ether_KeepAliveFailure:
	idb->reason = "Keepalive failed";
        reg_invoke_Ether_Alert(Ether_Alert12, idb);
	reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
        break;

      case Ether_LostCarrier:
        idb->reason = "Lost Carrier";
        reg_invoke_Ether_Alert(Ether_Alert7, idb);
	reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
        break;

      case Ether_LateCollision:
        idb->reason = "Late Collision";
        reg_invoke_Ether_Alert(Ether_Alert12, idb);
	reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
        break;

      case Ether_ExcessiveCongestion:
        idb->reason = "excessive collision";
        reg_invoke_Ether_Alert(Ether_Alert10, idb);
	reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
        break;

      case Ether_OpenFailure:
        reg_invoke_Ether_Alert(Ether_Alert4, idb);
        break;


      case AdminDownCommand:
        switch (idb->fast_switch_type) {
	  case FS_TOKEN:
            reg_invoke_TR_Alert(TrAlert8, idb);
            break;
	  case FS_ETHER:
            reg_invoke_Ether_Alert(Ether_Alert5, idb);
            break;

	  case FS_HDLC:
            break;
          default:
            break;
        }
	idb->reason = print_hwidbstate(idb);
	reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
        break;

      case TR_openFailLobeTest:
	  idb->reason = "Open Failure, Lobe";
	  reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
          reg_invoke_TR_Alert(TrAlert1, idb);
          break;

      case TR_openFailRingBeacon:
	  idb->reason = "Ring Beaconing";
	  reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
          reg_invoke_TR_Alert(TrAlert2, idb);
          break;

      case TR_openFailDupAddr:
	  idb->reason = "Duplicate Address";
	  reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
          reg_invoke_TR_Alert(TrAlert3, idb);
          break;

      case TR_openFailRemove:
	  idb->reason = "Remove MAC";
	  reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
          reg_invoke_TR_Alert(TrAlert4, idb);
          break;
    
      case TR_openFailUnexpected:
	  idb->reason = "Keepalive failed";
	  reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
          reg_invoke_TR_Alert(TrAlert5, idb);
          break;

      case TR_wireFault:
	  idb->reason = "Wire-fault";
	  reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
          reg_invoke_TR_Alert(TrAlert6, idb);
          break;

      case TR_remoteRemoval:
	  idb->reason = "Auto-removal";
	  reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
          reg_invoke_TR_Alert(TrAlert7, idb);
          break; 

     case TR_KeepAliveFailure:
	  idb->reason = "Keepalive failed";
	  reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
          if (idb->lastFailCause == 0)  /* if the line didnt come up before */
             reg_invoke_TR_Alert(TrAlert14, idb);
          break;

     case TR_hardError:
	  idb->reason = "Keepalive failed";
	  reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
          reg_invoke_TR_Alert(TrAlert14, idb);
          break;

      case TR_beacon:
	  idb->reason = "Ring Beaconing";
	  reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
          reg_invoke_TR_Alert(TrAlert10, idb);
          break;

      case Hdlc_KeepAliveFailure:
         idb->reason = "Keepalive failed";
	 reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
         reg_invoke_Dlc_Alert(Hdlc_keepAliveAlert, idb);
         break;

      case Lapb_KeepAliveFailure:
         idb->reason = "LAPB down";
	 reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
         if (idb->lastFailCause == 0)     /* if not fail before */
             reg_invoke_Dlc_Alert(Lapb_keepAliveAlert, idb);
         break;

      case Serial_SignalLost:
         /*
	  * The implementation of signal lost is quite untidy. Most
	  * protocols, like SDLC, LAPB and X.25, have their own
	  * implementation of signal lost and they call snmp_link_trap.
	  * The following routine provides a single snmp_trap
          * and alert, but to avoid too many code changes, snmp_traps for
	  * LAPB, X.25 and SDLC will not be sent here.
          */
         idb->reason = "EIA signal lost";
         if (is_hdlc(idb)) {
            reg_invoke_Dlc_Alert(Hdlc_signalLostAlert, idb);
	    reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
         } else if (is_x25(idb)) {
            reg_invoke_X25_Alert(X25Alert17, 0, idb, 0);
            /* dont send trap, both lapb and plp will send trap!  */
         } else if (!is_lapb(idb) && !is_sdlc(idb)) {
            /*
	     * Don't send trap for LAPB and SDLC, they are sent at
	     * LAPB and SDLC.
	     */
	    reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
         }                         
         break;

      default:
	break;
     }
     idb->lastFailCause = idb->failCause;   /* save fail cause */
     idb->failCause = 0;                    /* remove error */
}

      
