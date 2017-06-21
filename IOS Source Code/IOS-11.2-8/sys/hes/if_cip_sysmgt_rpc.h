/* $Id: if_cip_sysmgt_rpc.h,v 3.2.60.3 1996/08/07 08:58:59 snyder Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_cip_sysmgt_rpc.h,v $
 *------------------------------------------------------------------
 * if_cip_sysmgt_rpc.c - CIP system management RPC interface.
 *
 * April 1995, Mike Otto
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_cip_sysmgt_rpc.h,v $
 * Revision 3.2.60.3  1996/08/07  08:58:59  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/06/02  15:31:52  motto
 * CSCdi50208:  Issuing csna, llc or tcp CIP show cmd immed after CIP
 * reset fails
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  19:41:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  21:46:24  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:17:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:40:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:41:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SYSMGT_RPC_H__
#define __SYSMGT_RPC_H__

#include "sysmgt_types.h"

extern char *const netmgt_operations[NETMGT_MAX_OPERATIONS];

extern void *sysmgt_rpc(hwidbtype *hwidb, netmgt_header_t *nm_hdr,
			void *sm_data);
extern void show_sysmgt_rpc(parseinfo *csb);
extern void cip_sysmgt_info_reset(hwidbtype *hwidb);

#endif
