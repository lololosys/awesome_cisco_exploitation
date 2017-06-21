/* $Id: exec_clear_atmsig.h,v 3.3.20.2 1996/08/12 16:00:31 widmer Exp $
 * $Source: /release/112/cvs/Xsys/atm/exec_clear_atmsig.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ A T M S I G . H
 *
 * February, 1995       Percy P. Khabardar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *
 *  Clear signalling vc's
 *
 *------------------------------------------------------------------
 * $Log: exec_clear_atmsig.h,v $
 * Revision 3.3.20.2  1996/08/12  16:00:31  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.20.1  1996/05/14  06:28:54  achopra
 * CSCdi47186:  Implement clear atm arp command.
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/29  07:08:16  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  08:46:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/17  19:20:55  ronnie
 * CSCdi35693:  clear atm-vc command is useless
 *
 * Revision 2.2  1995/07/04  22:34:43  percyk
 * CSCdi36587:  cannot manually clear a connection off a subinterface.
 *
 * Revision 2.1  1995/07/04  09:35:12  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.2  1995/06/21  18:17:31  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:15:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear atm-vc <interface> <vcd>
 *
 * OBJ(int,1) = <vcd>
 * OBJ(idb,1) = <interface>
 */
EOLNS    (clear_atmvc_eol, atmSig_clear_vc);
NUMBER (clear_atmvc_vcd, clear_atmvc_eol, no_alt,
        OBJ(int,1), 1, 4095, "Virtual Circuit Number (VCD)");
INTERFACE (clear_atmvc_if, clear_atmvc_vcd, no_alt, OBJ(idb,1),
	   IFTYPE_ATM | IFTYPE_SUBIF);

KEYWORD (clear_atmvc, clear_atmvc_if, ALTERNATE,
            "vc", "Clear ATM virtual circuits on an interface", PRIV_OPR);

#undef  ALTERNATE
#define ALTERNATE       clear_atmvc
