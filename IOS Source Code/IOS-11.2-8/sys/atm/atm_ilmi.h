/* $Id: atm_ilmi.h,v 3.2.56.2 1996/08/03 23:17:30 fox Exp $
 * $Source: /release/112/cvs/Xsys/atm/atm_ilmi.h,v $
 *------------------------------------------------------------------
 * A T M _ I L M I . H
 *
 * January 1995, Ricky Li Fo Sjoe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atm_ilmi.h,v $
 * Revision 3.2.56.2  1996/08/03  23:17:30  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.2.56.1  1996/03/18  18:59:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  21:17:39  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  08:45:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:59:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/21  18:04:09  kartik
 * CSCdi37022:  ilmimib causes router crash on a mibwalk.
 *
 * Revision 2.2  1995/06/28  09:22:11  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:11:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * EXTERNs
 */
extern void atm_ilmi_init(subsystype *);
extern void atm_ilmi_periodic(hwidbtype *);

extern atm_db_t *atm_find_SNMP(long *, boolean);
extern vc_info_t *atm_vc_find_SNMP(int, int, int, boolean);

