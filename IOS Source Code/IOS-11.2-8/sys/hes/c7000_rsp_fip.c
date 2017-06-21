/* $Id: c7000_rsp_fip.c,v 3.3.60.1 1996/03/18 19:39:34 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/hes/c7000_rsp_fip.c,v $
 *------------------------------------------------------------------
 * 7000 and RSP specific FDDI functions for driving the FIP LEDs
 * [Was src-68-c7000/c7000_fddi.c]
 *
 * June 1994, Walter R. Friedrich
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: c7000_rsp_fip.c,v $
 * Revision 3.3.60.1  1996/03/18  19:39:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  21:44:47  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  09:14:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:25:39  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:37:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:37:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "../if/fddi.h"
#include "../if/fddi_private.h"
#include "../hes/dbus.h"
#include "../xdi/xdihdrsif.h"
#include "../xdi/fddihdr.h"
#include "../xdi/mibglbl.h"
#include "../if/if_fddi.h"

/* 
 * fip_leds
 * run leds for Clean FDDIT card
 *
 * LEDs look like this:
 *
 *    (*)   	     (Board enabled)
 * B  (*)  (*)  (*)  (Dual Homed / Single Attached / Dual Attached)
 * A  (*)  (*)  (*)  (Dual Homed / Single Attached / Dual Attached)
 *    
 * The LEDs are driven active LOW.  0x707 turns the lights OFF, 0x0 turns 
 * them all on.
 *
 * If we have a single-attached board, it will use PhyB.
 *
 * The following table summarizes the states:
 *
 * NNN	Isolated
 * NNN
 *
 * NNY	Thru A
 * NNY
 *
 * NNY	Wrap B (Phy A is down)
 * NNN
 *
 * NNN	Wrap A (Phy B is down)
 * NNY
 *
 * YYN	Single Attached B or Dual Homed A failed
 * NNN	(Phy A not shut down)
 * 
 * NNN	Single Attached A or Dual Homed B failed
 * YYN	(Phy B not shut down)
 *
 * NYN	Single Attached B (Phy A shut down)
 * NNN
 *
 * NNN	Single Attached A (Phy B shut down)
 * NYN
 *
 * YYN	Dual Homed B active
 * YNN
 *
 * YNN  Dual Homed A active
 * YYN
 */
#define BL 0x04
#define BM 0x02
#define BR 0x01
#define AL 0x20
#define AM 0x10
#define AR 0x08

void fip_leds (hwidbtype* idb) 
{
    ushort leds;
    phytype* phy_a;
    phytype* phy_b;
    ushort pc_state;

    if (idb->type != IDBTYPE_FDDIT) {
	buginf("\nfip_leds: expected %s to be a FIP interface, found %d",
	       idb->hw_namestring, idb->type);
	return;
    }
    
    phy_a = idb->phy[PHY_A];
    phy_b = idb->phy[PHY_B];
    pc_state = 0;
    leds = 0;
    switch (MIBDATA (idb->fddi_stn_id, mib->smtData.CF_State)) {
    case CF_wrap_a:
    case CF_c_wrap_a:
	if ((phy_a->rval & PC_TYPE_MASK) == PC_TYPE_M) {
	    if (phy_b->connect == FALSE) {
		leds = AM;	/* single attached A, phy B shut down */
	    } else {
		if (idb->fddi_options & FDDI_FAST_CMT) {
		    get_phy_pc_status(idb, PHY_B);
		    pc_state = phy_b->pc_state;
		    DELAY(5);
		    get_phy_pc_status(idb, PHY_B);
		    pc_state |= phy_b->pc_state;
		    DELAY(5);
		    get_phy_pc_status(idb, PHY_B);
		    pc_state |= phy_b->pc_state;
		} else {
		    pc_state |= phy_b->pc_state;
		}
		if (pc_state == PC_CONNECT) {
		    leds = AL|AM; /* single attached A or dual homed B failed,
				     phy B not shut down */
		} else {
		    leds = AL|AM|BL; /* dual homed A active */
		}
	    }
	} else 
	    leds = AR;		/* wrap A, phy B down */
	break;
    case CF_wrap_b:
    case CF_c_wrap_b:
	if ((phy_b->rval & PC_TYPE_MASK) == PC_TYPE_M) {
	    if (phy_a->connect == FALSE) {
		leds = BM;	/* single attached B, phy A shut down */
	    } else {
		if (idb->fddi_options & FDDI_FAST_CMT) {
		    get_phy_pc_status(idb, PHY_A);
		    pc_state = phy_a->pc_state;
		    DELAY(5);
		    get_phy_pc_status(idb, PHY_A);
		    pc_state |= phy_a->pc_state;
		    DELAY(5);
		    get_phy_pc_status(idb, PHY_A);
		    pc_state |= phy_a->pc_state;
		    DELAY(5);
		    get_phy_pc_status(idb, PHY_A);
		    pc_state |= phy_a->pc_state;
		    DELAY(5);
		    get_phy_pc_status(idb, PHY_A);
		    pc_state |= phy_a->pc_state;
		} else {
		    pc_state |= phy_a->pc_state;
		}
		if (pc_state == PC_CONNECT) {
		    leds = BL|BM; /* single attached B or dual homed A failed,
				     phy A not shut down */
		} else {
		    leds = BL|BM|AL; /* dual homed B active */
		}
	    }
	} else 
	    leds = BR;		/* wrap B, phy A down */
	break;
    case CF_thru:
	leds = AR|BR;
	break;
    default:
	leds = 0;
	break;
    }

    dbus_write_fip_leds(idb->slot, leds);
}


/*
 * hes_get_phy
 * Platform specific routine to get the appropriate phy structure
 * for this interface
 */

phytype * hes_get_phy (hwidbtype *hwidb, int index)
{
    phytype *phy;

    /* on CLEAN, idbs may have preallocated phy structures */
    phy = (phytype *) STADATA (hwidb->fddi_stn_id, portData[index]);
    if (!phy)
	phy = malloc(sizeof(phytype));
    return(phy);
}
