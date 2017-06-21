/* $Id: if_vip_4r.h,v 3.3.54.3 1996/06/07 00:34:02 kao Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_vip_4r.h,v $
 *------------------------------------------------------------------
 * if_vip_4r.h -- header file for 4R Token Ring port adapter
 *
 * July 1995 - Stig Thormodsrud
 * 
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_vip_4r.h,v $
 * Revision 3.3.54.3  1996/06/07  00:34:02  kao
 * CSCdi59481:  4R does not work when hot swapped
 * CSCdi59000:  FDDI PA line status is always down after an eoir event
 * Branch: California_branch
 *
 * Revision 3.3.54.2  1996/03/28  01:59:50  sthormod
 * CSCdi52811:  4R SMT stats not displayed
 * Branch: California_branch
 *
 * Revision 3.3.54.1  1996/03/18  19:42:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.1  1996/01/24  21:47:30  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/29  02:47:55  sthormod
 * CSCdi42503:  4r needs modularity work
 * Clean up #ifdef C7000 kludge.
 *
 * Revision 3.2  1995/11/17  09:19:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:41:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:01:06  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/17  05:35:29  sthormod
 * Placeholders for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "../pas/if_pas_ibm2692_common.h"


#define INSTANCE_VIP_4R       hwidb->vip4r_info

#define VIP_4R_MAX_INIT_TIME  (90*ONESEC) /* time to allow init to complete */

/*
 * Open return codes
 *
 * LAN Technical Reference 8-19
 */
#define VIP_4R_OPEN_OK            0x00
#define VIP_4R_OPEN_CANCELLED     0x07

extern boolean cbus_vip_4r_reset(hwidbtype *);
extern void cbus_vip_4r_periodic(hwidbtype *);
extern void parse_vip_4r_err_ll(hwidbtype *, char *);
extern void parse_vip_4r_smt_ll(hwidbtype *, char *);
extern void parse_vip_4r_error_ll(hwidbtype *, char *);
extern void vip_4r_restart_init(hwidbtype *);
extern void vip_4r_platform_init(subsystype *);
extern ushort platform_vip_4r_anybody_out_there(hwidbtype *);
extern void cbus_vip_4r_idb_init(hwidbtype *);

/*
 * extern from if_trip.c  (hack, hack)
 */
extern void get_tokenring_cbus_errors(hwidbtype *);

