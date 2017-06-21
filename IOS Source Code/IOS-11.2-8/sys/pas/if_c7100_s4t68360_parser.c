/* $Id: if_c7100_s4t68360_parser.c,v 3.1.44.4 1996/09/09 01:34:20 xtang Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_c7100_s4t68360_parser.c,v $
 *------------------------------------------------------------------
 * if_c7100_s4t68360_parser.c - C7100 PCI Adaptor s4t media driver parser
 *
 * January 1996, Greg Stovall
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c7100_s4t68360_parser.c,v $
 * Revision 3.1.44.4  1996/09/09  01:34:20  xtang
 * CSCdi56632:  unknown clock rates on 4T PA, FSIP
 * Branch: California_branch
 *         add ALT_HELPS to use csb->which in different
 *         driver procedures, and print the
 *         driver specific info. In this case, the clockrate
 *         option table, and csb->which = CFG_INT_CLOCKRATE
 *
 * Revision 3.1.44.3  1996/08/23  04:47:22  xtang
 * CSCdi59553:  clockrate 250000 is not configurable on FSIP in 75xx
 * Branch: California_branch
 *              added 250000 Hz clockrate to s4t68360 4T
 *              port adaptor. The corresponding microcode change are in
 *              cvs -d /micro s4t68360
 *
 * Revision 3.1.44.2  1996/04/30  16:49:53  gstovall
 * CSCdi55513:  Fix OIR problems
 * Branch: California_branch
 * Fix oir memory handling on 4t.
 *
 * Revision 3.1.44.1  1996/03/21  23:22:57  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.3  1996/02/08  08:47:44  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.1.2.2  1996/01/26  02:50:08  gstovall
 * Branch: ELC_branch
 * Oodles of fixes and enhancements:
 *         - Added support for the crc command to the predator 4T
 *         - Added support for the ignore-dcd command to the predator 4T
 *         - Do not keep stats on a VIP 4T
 *         - Pulled in a needed bugfix (CSCdi47390)
 *         - Revived the show controller command, it should work correctly
 *           with FDDI and token ring now.
 *
 * Revision 3.1.2.1  1996/01/18  22:11:11  gstovall
 * Branch: ELC_branch
 * Preadator now knows the joy and splendor of serial interfaces.
 *
 * Revision 3.1  1996/01/18  21:56:02  gstovall
 * Placeholders for ELC_branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
#include "../if/network.h"
#include "ieee.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../wan/ppp.h"
#include "../wan/smds.h"
#include "../wan/atm_dxi.h"
#include "../ip/ip.h"
#include "../if/serial_inline.h"
#include "if_pas.h" 
#include "if_pas_s4t68360.h"


/*
 * s4t_clockrate()
 * Set the DCE clock rate. 
 */
void s4t_clockrate_command (hwidbtype *hwidb, parseinfo *csb)
{
    int  rate = 0;
    int i;
    serialsb *ssb;

    if (csb->sense) {
        rate = GETOBJ(int,1);
    }

    if ((csb->which == CFG_INT_CLOCKRATE) && (rate == 0)) {
          printf("\n  \t%s\n", "Speed (bits per second)");

	  for (i = 1; i < S4T_CLOCKS; i++) {
	      printf("  %d\n", s4t_clock[i].rate);
	  }
	  return;
    }

    ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);

    if (csb->nvgen) {
        if (!hwidb->dte_interface) {
            nv_write(ssb->serial_clockindex, "%s %d", csb->nv_command,
                     s4t_clock[ssb->serial_clockindex].rate);
        }
	idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
        return;
    }

    /*
     * Scan up through the array looking for the
     * divider index
     */
    for (i = 0; i < S4T_CLOCKS; i++) {
	if (rate == s4t_clock[i].rate) {
	    break;
	}
    }
    
    if (i == S4T_CLOCKS) {
	printf("\nunknown clock rate");
	idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
	return;
    }
    
    /*
     * Always store the clock index irregardless of
     * whether the cable is DTE or DCE
     */
    ssb->serial_clockindex = i; 

    idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
    
    if (!hwidb->dte_interface || !csb->sense) {
	/*
	 * Reset the interface for the clockrate to take
	 * effect
	 */
	s4t_init(hwidb);
    }
}
