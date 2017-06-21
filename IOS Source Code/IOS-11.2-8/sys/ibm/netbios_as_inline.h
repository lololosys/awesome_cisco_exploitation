/* $Id: netbios_as_inline.h,v 3.2.60.1 1996/05/17 11:20:55 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ibm/netbios_as_inline.h,v $
 *------------------------------------------------------------------
 * netbios_as_inline.h _ NBF Access Server inline routines
 *
 * September 1995, Shoou Yiu
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: netbios_as_inline.h,v $
 * Revision 3.2.60.1  1996/05/17  11:20:55  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/03  14:34:18  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.48.1  1996/03/16  12:09:21  ppearce
 * CSCdi50498:  IBU modularity - Implement NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Move NetBios fields from hwidbtype/idbtype structs to NetBios subblock
 *
 * Revision 3.2  1995/11/17  09:23:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:02:09  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  00:22:58  syiu
 * Initial placeholder for NBF (NetBIOS Frames) support.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "netbios_sb.h"

/*
 * External references
 */
extern queuetype *netbios_lan_hash_table_ptr;


/*
 * Hash a IEEEBYTE address 
 */
static inline uchar nbf_hash(uchar *macaddr)
{
  return(macaddr[5]);
}



/*
 * Check if session entry already occupied for a IS_NETBIOS_ACCESS interface
 */
static inline netbios_sess_entry *nbf_sess_occupied (idbtype *idb, 
uchar srcnum) 
{

  netbios_sb_t *netb_sb;

  /*
   * Sanity Check
   */
  if (!idb) {
      return(NULL);
  }

  netb_sb = netbios_get_sb(idb);
  if (!netb_sb || !netb_sb->sess_table_ptr) {
      return(NULL);
  }

  /*
   * An non-empty field means an occupied slot.
   * An empty field means it's an available slot
   */
  if (!(&netb_sb->sess_table_ptr[srcnum])->dstidb)
     return(NULL);

  return(&netb_sb->sess_table_ptr[srcnum]);
}



/*
 * Check if session entry already occupied for a IS_NETBIOS_LAN interface
 */
static inline netbios_lan_sess_entry *nbf_lan_sess_occupied (hwidbtype *srcidb,
                            hwidbtype *dstidb, uchar srcnum, uchar dstnum,
                            uchar *srcmacaddr, uchar *dstmacaddr) 
{
   
   netbios_lan_sess_entry *curr_lan_entry;

   /*
    * Sanity Check 
    */
   if (!netbios_lan_hash_table_ptr)
      return(NULL);

   /*
    * Hash to the queue and then to the first entry 
    */
    curr_lan_entry = (&netbios_lan_hash_table_ptr[nbf_hash(srcmacaddr)])->qhead;

   /*
    * Now search the queue for the correct match on srcmacaddr, srcnum and 
    * dstnum
    */
   while (curr_lan_entry != NULL) {
         if ((curr_lan_entry->srcnum == srcnum) && 
             ieee_equal(srcmacaddr, curr_lan_entry->srcmacaddr))
             return(curr_lan_entry);
         curr_lan_entry = curr_lan_entry->next;
   }   
   return(NULL);
}
