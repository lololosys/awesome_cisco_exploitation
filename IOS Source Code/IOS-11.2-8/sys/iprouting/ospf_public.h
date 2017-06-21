/* $Id: ospf_public.h,v 3.2.60.1 1996/05/23 07:37:53 myeung Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ospf_public.h,v $
 *------------------------------------------------------------------
 * Prototypes for public routines
 *
 * January 1993, Dave Katz
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: ospf_public.h,v $
 * Revision 3.2.60.1  1996/05/23  07:37:53  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.2  1995/11/17  17:36:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:09:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:09:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern char *ospf_area_id_str(pdbtype *pdb, ulong area_id);
extern void ospf_idb_address_change(idbtype *,ipaddrtype, ipaddrtype,
				    boolean, boolean);
extern void ospf_route_adjust(idbtype *, boolean);
extern void ospf_timers_command(parseinfo *);
extern void ospf_perform_asbr_duty(pdbtype *pdb);

/* External LSA metric type */
#define	METRIC_TYPE1	1
#define METRIC_TYPE2	2
