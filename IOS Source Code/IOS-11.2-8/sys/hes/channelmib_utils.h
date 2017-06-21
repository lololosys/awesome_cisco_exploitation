/* $Id: channelmib_utils.h,v 3.2.60.1 1996/03/18 19:39:55 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/hes/channelmib_utils.h,v $
 *------------------------------------------------------------------
 * channelmib_utils.h:  Utilities for cip related mibs.
 *
 * April 1995, Mike Otto
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: channelmib_utils.h,v $
 * Revision 3.2.60.1  1996/03/18  19:39:55  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  21:45:00  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:15:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:37:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/27  20:46:14  motto
 * Code Review updates:
 *   - Make use of snmp_if_index instead of hw_if_index
 *     using snmp functions in sr_core.c.
 *   - Source line length adjustments.
 *   - Change VLAN to LAN for CIP internal LANs.
 *     This was basically renaming the MIBS,
 *     mib source files, and makefiles.
 *     This also involved changing the
 *     CIPCSNA mib references to virtual
 *     LANs on the CIP.
 *   - Fix query bug in sr_ciplanmib.c for
 *     CIP internal LAN adapters.
 * CSCdi33408:  CIP SNA SNMP MIB support
 *
 * Revision 2.1  1995/06/07  20:38:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
#ifndef __CHANNELMIB_UTILS_H__
#define __CHANNELMIB_UTILS_H__

void InitOctetString(OctetString *os, uchar *srcStr, int srcLen);
void index_to_path_device(long index, long *path, long *device);
long path_device_to_index(long path, long device);
hwidbtype *cip_get_hwidb( int searchType, long ifIndex);
void do_channel_trap (const OID*       vbList,
                      const OID*       entOID,
                      char*            v2TrapOID,
                      OID*             indexOID,
                      int              vbLength,
                      int              genericTrapNum,
                      int              specificTrapNum,
                      void             *dataP[] );
extern boolean chan_link_fail_traps_enabled;  
#endif  /*  __CHANNELMIB_UTILS_H__  */
