/* $Id: netbios_as_session.c,v 3.4.20.3 1996/05/22 18:10:01 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ibm/netbios_as_session.c,v $
 *------------------------------------------------------------------
 * netbios_as_session.c - NBF Access Server NetBIOS Session support
 *
 * September 1995, Shoou Yiu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: netbios_as_session.c,v $
 * Revision 3.4.20.3  1996/05/22  18:10:01  snyder
 * CSCdi58423:  spellink errors
 * Branch: California_branch
 *
 * unnexpected -> unexpected
 *
 * Revision 3.4.20.2  1996/05/17  11:20:57  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.6.2  1996/04/03  14:34:21  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.6.1  1996/03/16  12:09:23  ppearce
 * CSCdi50498:  IBU modularity - Implement NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Move NetBios fields from hwidbtype/idbtype structs to NetBios subblock
 *
 * Revision 3.4.20.1  1996/03/18  20:11:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.2  1996/03/07  09:42:12  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.20.1  1996/02/20  00:54:39  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/01  06:03:12  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/11/30  03:11:15  syiu
 * CSCdi44692:  NBFCP Session Table display spacing
 *
 * Revision 3.2  1995/11/17  09:23:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:02:11  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  00:32:04  syiu
 * Initial placeholder for NBF.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "fastswitch_registry.h"
#include "interface_private.h"
#include "ieee.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "address.h"
#include "logger.h"
#include "netbios.h"
#include "netbios_sb.h"
#include "../llc2/llc2.h"
#include "../if/tring.h"
#include "../if/rif.h"
#include "parser_defs_netbios.h"
#include "../if/network.h"
#include "../srt/srb.h"
#include "../srt/rsrb.h"
#include "../ui/debug.h"
#include "netbios_as_debug.h"
#include "netbios_as.h"
#include "netbios_as_inline.h"

/* 
 * Function prototypes in this module 
 */
boolean nbf_insert_sess_entry(hwidbtype *, hwidbtype *, uchar *, uchar *, llctype *, llctype *, netbios_header_sess *); 
boolean nbf_delete_sess_entry(hwidbtype *, hwidbtype *, uchar *, uchar *, netbios_header_sess *); 
void nbf_reset_session_table(hwidbtype *, boolean, boolean, boolean);
void nbf_reset_lan_hash_table(hwidbtype *, boolean, boolean, boolean);
void nbf_delete_all_buckets(int, queuetype *, idbtype *idb, boolean, boolean);
boolean nbf_free_lan_sess_entry(int, uchar, uchar, hwidbtype *, hwidbtype *, uchar *, boolean);
static void nbf_free_access_sess_entry(idbtype *, int, boolean);
boolean nbf_insert_access_sess_entry(hwidbtype *, hwidbtype *, llctype *, uchar, uchar, uchar *);
netbios_lan_sess_entry *nbf_insert_lan_entry(hwidbtype *, hwidbtype *, llctype *, uchar, uchar, uchar *, uchar *);
int nbf_hash_table_entries (idbtype *);
void display_netbios_interfaces(void);
void display_netbios_sess_entry(hwidbtype *, uchar,netbios_sess_entry *);
void display_netbios_lan_sess_entry(netbios_lan_sess_entry *, uchar);

/*
 * External references
 */
extern void nbf_xmit_netbios_session_end(uchar, uchar, uchar *, idbtype *);


/*
 *  nbf_insert_sess_entry
 * Insert an entry into the relevant tables - i.e. the per interface 
 * session table and/or the global hash table.
 * For every one call to this routine, a *pair* of entries are inserted
 * into the relevant tables. While the insertion is dual, it is 
 * directionally sensitive based on the idbs.
 * Input:
 *   srcidb - hwidb which originally accepted the NetBIOS SESSION_INITIALIZE 
 *            packet.
 *   dstidb - hwidb which accepted the NetBIOS SESSION_CONFIRM packet that
 *            matched the required session context of the previous  
 *            SESSION_INITIALIZE accepted through srcidb
 *   srcmacaddr - IEEE source MAC address of sender of SESSION_INITIALIZE 
 *                (if any)
 *   dstmacaddr - IEEE source MAC address of sender of SESSION_CONFIRM
 *                (if any)
 *   
 *   netbios_hdr_sess  - pointer to NETBIOS SESSION_CONFIRM packet
 */
boolean nbf_insert_sess_entry (hwidbtype *srcidb, hwidbtype *dstidb, 
uchar *srcmacaddr, uchar *dstmacaddr, llctype *src_llc, llctype *dst_llc,
netbios_header_sess *netbios_hdr_sess)
{

   /* 
    * If SESSION_INITIALIZE came in from ASYNC or ISDN, we will first insert 
    * the entry into the session table for that interface. 
    * Then, if the SESSION_CONFIRM came in from ASYNC or ISDN, likewise 
    * is done to the output interface. 
    * Else if the output interface is a LAN interface, the entry is 
    * inserted (via hashing of the dstmacaddr).
    * At all times, if any identical entry already exists of the respective
    * tables, both entries are not entered.
    */
    if (IS_NETBIOS_ACCESS(srcidb)) {

       /*
        * Session already occupied on this interface that accepted 
        * SESSION_INITIALIZE?
        */ 
       if (nbf_sess_occupied(srcidb->firstsw, netbios_hdr_sess->destnum)) {
          if (netbios_error_debug)
             buginf("\nnbf failed insert srcnum=%d on SESSION_INITIALIZE "
                    "interface %s", netbios_hdr_sess->destnum, 
                    srcidb->hw_namestring);
          return(FALSE);
       }

       /*
        * Processing depends on which interface type that accepted 
        * SESSION_CONFIRM
        */
       if (IS_NETBIOS_ACCESS(dstidb)) {

          /*
           * Session already occupied on this interface that accepted 
           * SESSION_CONFIRM?
           */ 
          if (nbf_sess_occupied(dstidb->firstsw, netbios_hdr_sess->srcnum)) {
             if (netbios_error_debug)
                buginf("\nnbf failed insert srcnum=%d on SESSION_CONFIRM "
                       "interface %s", netbios_hdr_sess->srcnum, 
                       dstidb->hw_namestring);
             return(FALSE);
          }

          /*
           * Insert entries in tandem
           */
          nbf_insert_access_sess_entry(srcidb,
                                       dstidb, 
                                       dst_llc,
                                       netbios_hdr_sess->destnum,
                                       netbios_hdr_sess->srcnum,
                                       NULL);
          
          nbf_insert_access_sess_entry(dstidb,
                                       srcidb,
                                       src_llc,
                                       netbios_hdr_sess->srcnum,
                                       netbios_hdr_sess->destnum,
                                       NULL);

       } else if (IS_NETBIOS_LAN(dstidb)) {

          /*
           * Session already occupied on this interface that accepted 
           * SESSION_CONFIRM?
           */ 
          if (nbf_lan_sess_occupied(dstidb, srcidb, netbios_hdr_sess->srcnum,
                                    netbios_hdr_sess->destnum, dstmacaddr, 
                                    NULL)) {
             if (netbios_error_debug)
                buginf("\nnbf failed insert srcnum=%d on SESSION_CONFIRM "
                       "interface %s", netbios_hdr_sess->srcnum, 
                       dstidb->hw_namestring);
             return(FALSE);
          }

          if (!nbf_insert_lan_entry(dstidb, srcidb, src_llc, 
                                    netbios_hdr_sess->srcnum,
                                    netbios_hdr_sess->destnum, dstmacaddr,
                                    NULL)) {
             if (netbios_error_debug)
                buginf("\nnbf failed insert srcnum=%d on SESSION_CONFIRM "
                       "interface %s", netbios_hdr_sess->srcnum, 
                       dstidb->hw_namestring);
             return(FALSE);
          }

          nbf_insert_access_sess_entry(srcidb,
                                       dstidb,
                                       dst_llc, 
                                       netbios_hdr_sess->destnum,
                                       netbios_hdr_sess->srcnum,
                                       dstmacaddr);


       } else {
          buginf("\nnbf failed insert bad interface");
          return(FALSE);
       }

   /* 
    * Else if SESSION_INITIALIZE came in from a LAN interface, we will 
    * first insert the entry into the global session table (via hashing of 
    * the srcmacaddr) 
    * Then, if the SESSION_CONFIRM came in from ASYNC or ISDN, likewise 
    * is done to the output interface. Else if the output interface is
    * a LAN interface, the entry is inserted (via hashing of the dstmacaddr).
    * At all times, if any identical entry already exists of the respective
    * tables, both entries are not entered.
    */

    } else if (IS_NETBIOS_LAN(srcidb)) {

       /*
        * Session already occupied on this interface that accepted 
        * SESSION_INITIALIZE?
        */
       if (nbf_lan_sess_occupied(srcidb, dstidb, netbios_hdr_sess->destnum,
                                    netbios_hdr_sess->srcnum, srcmacaddr, 
                                    NULL)) {
          if (netbios_error_debug)
             buginf("\nnbf failed insert srcnum=%d on SESSION_INITIALIZE "
                    "interface %s", netbios_hdr_sess->destnum, 
                    srcidb->hw_namestring);
          return(FALSE);
       }

       /*
        * Processing depends on which interface type that accepted 
        * SESSION_CONFIRM
        */
       if (IS_NETBIOS_ACCESS(dstidb)) {

          /*
           * Session already occupied on this interface that accepted 
           * SESSION_CONFIRM?
           */ 
          if (nbf_sess_occupied(dstidb->firstsw, netbios_hdr_sess->srcnum)) {
             if (netbios_error_debug)
                buginf("\nnbf failed insert srcnum=%d on SESSION_CONFIRM "
                       "interface %s", netbios_hdr_sess->srcnum, 
                       dstidb->hw_namestring);
             return(FALSE);
          }

          /*
           * Insert entries in tandem
           */
          if (!nbf_insert_lan_entry(srcidb, dstidb, dst_llc,
                               netbios_hdr_sess->destnum,
                               netbios_hdr_sess->srcnum,
                               srcmacaddr, NULL)) {
             if (netbios_error_debug)  
                buginf("\nnbf failed insert srcnum=%d on SESSION_INITIALIZE "
                       "interface %s", netbios_hdr_sess->destnum, 
                       srcidb->hw_namestring);
             return(FALSE);
          }

          nbf_insert_access_sess_entry(dstidb,
                                       srcidb,
                                       src_llc,
                                       netbios_hdr_sess->srcnum,
                                       netbios_hdr_sess->destnum,
                                       srcmacaddr);

       } else if (IS_NETBIOS_LAN(dstidb)) {

          /*
           * Session already occupied on this interface that accepted 
           * SESSION_CONFIRM?
           */ 
          if (nbf_lan_sess_occupied(dstidb, srcidb, netbios_hdr_sess->srcnum,
                                    netbios_hdr_sess->destnum, dstmacaddr, 
                                    NULL)) {
             if (netbios_error_debug)
                buginf("\nnbf failed insert srcnum=%d on SESSION_CONFIRM "
                       "interface %s", netbios_hdr_sess->srcnum, 
                       dstidb->hw_namestring);
             return(FALSE);
          }

          /*
           * Insert entries in tandem.
           * Note that LAN to LAN entries do not point to each other
           * The information in each entry is enough to search the
           * the corresponding entry
           */
          if (!nbf_insert_lan_entry(dstidb, srcidb, src_llc,
                                    netbios_hdr_sess->srcnum,
                                    netbios_hdr_sess->destnum, dstmacaddr, 
                                    srcmacaddr))
             return(FALSE);
          if (!nbf_insert_lan_entry(srcidb, dstidb, dst_llc,
                                    netbios_hdr_sess->destnum,
                                    netbios_hdr_sess->srcnum,
                                    srcmacaddr, dstmacaddr)) {
             return(FALSE); 
	  }
       } else {
          buginf("\nnbf failed insert bad interface");
          return(FALSE);
       }
    } else
       return(FALSE);
    return(TRUE);
}


/*
 * nbf_delete_sess_entry 
 * Delete the dual entries when a NETBIOS_SESSION_END message is detected
 * (opposite of nbf_insert_sess_entry)
 */
boolean nbf_delete_sess_entry (hwidbtype *srcidb, hwidbtype *dstidb, 
uchar *srcmacaddr, uchar *dstmacaddr, netbios_header_sess *netbios_hdr_sess)
{
  
    /*
     * Free the enrty on the source interface and its corresponding entry
     * and the destination interface.
     */ 
    if (IS_NETBIOS_ACCESS(srcidb)) {

       /*
        * Session must be occupied on the source interface.
        */ 
       if (!nbf_sess_occupied(srcidb->firstsw, netbios_hdr_sess->srcnum)) {
          if (netbios_error_debug)
             buginf("\nnbf failed delete srcnum=%d on src interface %s", 
                     netbios_hdr_sess->srcnum, srcidb->hw_namestring);
          return(FALSE);
       }

       if (IS_NETBIOS_ACCESS(dstidb)) {

          /*
           * Session must be occupied on the destination interface 
           */ 
          if (!nbf_sess_occupied(dstidb->firstsw, netbios_hdr_sess->destnum)) {
             if (netbios_error_debug)
                buginf("\nnbf failed delete destnum=%d on dest interface %s", 
                        netbios_hdr_sess->destnum, dstidb->hw_namestring);
             return(FALSE);
          }

          /*
           * Delete the entries in tandem
           */
          nbf_free_access_sess_entry(srcidb->firstsw, 
                                     netbios_hdr_sess->srcnum, FALSE);
          nbf_free_access_sess_entry(dstidb->firstsw, 
                                     netbios_hdr_sess->destnum, FALSE);

       } else if (IS_NETBIOS_LAN(dstidb)) {

          /*
           * Session must be occupied on this destination interface 
           */ 
          if (!nbf_lan_sess_occupied(dstidb, srcidb, netbios_hdr_sess->destnum,
                                    netbios_hdr_sess->destnum, dstmacaddr, 
                                    NULL)) {
             if (netbios_error_debug)
                buginf("\nnbf failed delete destnum=%d on dest interface%s", 
                       netbios_hdr_sess->destnum, dstidb->hw_namestring);
             return(FALSE);
          }

          /*
           * Delete the entries in tandem
           */
          nbf_free_access_sess_entry(srcidb->firstsw, 
                                     netbios_hdr_sess->srcnum, FALSE);
          nbf_free_lan_sess_entry(
                           nbf_hash(dstmacaddr),
                           netbios_hdr_sess->destnum,
                           netbios_hdr_sess->srcnum,
                           dstidb,
                           srcidb,
                           dstmacaddr, 
                           FALSE);
 
       } else {
          buginf("\nnbf failed delete bad interface");
          return(FALSE);
       }

    } else if (IS_NETBIOS_LAN(srcidb)) {
       /*
        * Session must be occupied on this interface.
        */
       if (!nbf_lan_sess_occupied(srcidb, dstidb, netbios_hdr_sess->srcnum,
                                    netbios_hdr_sess->srcnum, srcmacaddr, 
                                    NULL)) {
          if (netbios_error_debug)
             buginf("\nnbf failed delete srcnum=%d on src interface %s", 
                    netbios_hdr_sess->srcnum, srcidb->hw_namestring);
          return(FALSE);
       }

       if (IS_NETBIOS_ACCESS(dstidb)) {
          /*
           * Session must be occupied on this interface.
           */ 
          if (!nbf_sess_occupied(dstidb->firstsw, netbios_hdr_sess->destnum)) {
             if (netbios_error_debug)
                buginf("\nnbf failed delete destnum=%d on dest interface %s", 
                       netbios_hdr_sess->destnum, dstidb->hw_namestring);
             return(FALSE);
          }

          /*
           * Delete the entries in tandem
           */
          nbf_free_lan_sess_entry(
                           nbf_hash(srcmacaddr),
                           netbios_hdr_sess->srcnum,
                           netbios_hdr_sess->destnum,
                           srcidb,
                           dstidb,
                           srcmacaddr, 
                           FALSE);

          nbf_free_access_sess_entry(dstidb->firstsw, 
                                     netbios_hdr_sess->destnum, FALSE);

       } else if (IS_NETBIOS_LAN(dstidb)) {
          /*
           * Session already occupied on this interface
           */ 
          if (nbf_lan_sess_occupied(dstidb, srcidb, netbios_hdr_sess->destnum,
                                    netbios_hdr_sess->destnum, dstmacaddr, 
                                    NULL)) {
             if (netbios_error_debug)
                buginf("\nnbf failed delete destnum=%d on dest interface %s", 
                       netbios_hdr_sess->destnum, dstidb->hw_namestring);
             return(FALSE);
          }

          /*
           * Delete the entries in tandem
           */
          nbf_free_lan_sess_entry(
                           nbf_hash(srcmacaddr),
                           netbios_hdr_sess->srcnum,
                           netbios_hdr_sess->destnum,
                           srcidb,
                           dstidb,
                           srcmacaddr, 
                           FALSE);

         nbf_free_lan_sess_entry(
                           nbf_hash(dstmacaddr),
                           netbios_hdr_sess->destnum,
                           netbios_hdr_sess->srcnum,
                           dstidb,
                           srcidb,
                           dstmacaddr, 
                           FALSE);

       } else {
          buginf("\nnbf failed delete bad interface");
          return(FALSE);
       }
    } else
       return(FALSE);
    return(TRUE);
}


/*
 * nbf_reset_session_table
 * Free ASYNC or ISDN session table and all corresponding
 * entries in other tables (could be entries in the LAN hash table 
 * and other ASYNC or ISDN interfaces). 
 * If delete is set, delete the nbf table for this interface. 
 * if local_notify is TRUE, notify with a NETBIOS_SESSION_END 
 * to all local sessions "behind" is this interface. 
 * If remote_notify is TRUE, notify with a NETBIOS_SESSION_END to 
 * all remote sessions that correspond with the the local sessions   
 */
void nbf_reset_session_table(hwidbtype *hwidb, boolean delete, 
boolean local_notify, boolean remote_notify)
{
   int i;
   idbtype *idb;
   netbios_sb_t *netb_sb;

   idb = hwidb->firstsw;
   if (!idb || !IS_NETBIOS_ACCESS(idb->hwptr))
      return;

   netb_sb = netbios_get_sb(idb);
   if (!netb_sb || !netb_sb->sess_table_ptr) {
       return;
   }

   /*
    * Loop thru each entry in session table
    * For each entry, delete 
    */
       
   for (i=0; i<(NETBIOS_MAX_SESSIONS+1); i++) {

     /*
      * Delete the corresponding entry depending on what 
      * the outgoing interface is. Skip empty entries
      */
     if (!(&netb_sb->sess_table_ptr[i])->dstidb)
        continue;

     if IS_NETBIOS_ACCESS((&netb_sb->sess_table_ptr[i])->dstidb)
        nbf_free_access_sess_entry(
          (&netb_sb->sess_table_ptr[i])->dstidb->firstsw,i, remote_notify);
     else if (IS_NETBIOS_LAN((&netb_sb->sess_table_ptr[i])->dstidb)) {
          if (!nbf_free_lan_sess_entry(
                  nbf_hash((&netb_sb->sess_table_ptr[i])->dstmacaddr),
                  (&netb_sb->sess_table_ptr[i])->dstnum,
                  i,
                  (&netb_sb->sess_table_ptr[i])->dstidb,
                  idb->hwptr,
                  (&netb_sb->sess_table_ptr[i])->dstmacaddr,
                  remote_notify)) {
              buginf("\nnbf unexpected bad lan session entry");
              return;
	  }
     } else {
       buginf("\nnbf unexpected bad session entry dstidb"); 
       return;
     }

     /*
      * Delete entry in its table
      */
     nbf_free_access_sess_entry(idb, i, local_notify);
   }

   if (delete) {
     free(netb_sb->sess_table_ptr);
     netb_sb->sess_table_ptr = NULL;
   }
}


/*
 * nbf_reset_lan_hash_table 
 * All buckets in the LAN hash table that matches the idb 
 * and its corresponding entries in the session tables are cleared. 
 * The LAN hash table is then deallocated.
 */
void nbf_reset_lan_hash_table (hwidbtype *hwidb, boolean delete, 
boolean local_notify, boolean remote_notify)
{
   int i;
   idbtype *idb = hwidb->firstsw;

   if (!idb || !IS_NETBIOS_LAN(idb->hwptr) || !netbios_lan_hash_table_ptr)
      return;

   for (i=0; i<NETBIOS_HASH_SESS_LEN; i++) {
      nbf_delete_all_buckets(i, &netbios_lan_hash_table_ptr[i], idb, 
                             local_notify, remote_notify);
   }

   /*
    * Only delete the hash table if the hash table is empty
    */
   if (delete && (nbf_hash_table_entries(NULL)==0)) {
     free(netbios_lan_hash_table_ptr);
     netbios_lan_hash_table_ptr = NULL; 
   } 
}


/*
 * nbf_delete_all_buckets
 * Free all buckets in this hash queue that match the specified idb 
 * and also all corresponding entries in other interfaces
 */
void nbf_delete_all_buckets (int index, queuetype *queue_ptr, idbtype *idb, 
boolean local_notify, boolean remote_notify)
{
   netbios_lan_sess_entry *curr_lan_entry, *temp_lan_entry;
 
   if (!queue_ptr || !idb)
      return;

   curr_lan_entry = queue_ptr->qhead;

   /*
    * Traverse the queue looking for the correct match 
    */
   while (curr_lan_entry != NULL) {
     if (curr_lan_entry->srcidb == idb->hwptr) { 
        
        /*
         * If found, remove the corresponding entry accordingly
         */
        if (IS_NETBIOS_ACCESS(curr_lan_entry->dstidb)) {
           nbf_free_access_sess_entry(curr_lan_entry->dstidb->firstsw, 
                        curr_lan_entry->dstnum, remote_notify);

        } else if (IS_NETBIOS_LAN(curr_lan_entry->dstidb)) {
           if (!nbf_free_lan_sess_entry(
                           nbf_hash(curr_lan_entry->dstmacaddr),
                           curr_lan_entry->dstnum,
                           curr_lan_entry->srcnum,
                           curr_lan_entry->dstidb,
                           curr_lan_entry->srcidb,
                           curr_lan_entry->dstmacaddr, 
                           remote_notify)) {
             buginf("\nnbf unexpected bad target lan session entry");
             return;
           }

        } else {
           buginf("\nnbf unexpected bad lan session entry dstidb");
           return;
        } 

        /*
         * To make the current loop search proceed to the next entry, 
         * save it in a temporary pointer.  
         */ 
        temp_lan_entry = curr_lan_entry->next;

        /*
         * Remove the current entry. Since we cannot be sure whether 
         * the corresponding entry removed above was on the same queue 
         * (e.g. what if it was the previous entry to this current entry),
         * even though we know explicitly the address of the current entry, 
         * we will remove it by the general lan session removal routine
         * and incur the loop search penalty. 
         */

        if (!nbf_free_lan_sess_entry(
                index,
                curr_lan_entry->srcnum,
                curr_lan_entry->dstnum,
                curr_lan_entry->srcidb,
                curr_lan_entry->dstidb,
                curr_lan_entry->srcmacaddr, 
                local_notify)) {

           buginf("\nnbf unexpected bad current lan session entry");
           return;
        }  
        
        /*
         * Set the current entry as the next valid entry
         */ 
        curr_lan_entry = temp_lan_entry;
        continue; 
     } 

     /*
      * No match, proceed to next entry
      */
     curr_lan_entry = curr_lan_entry->next;
   }
}


/*
 * nbf_insert_access_sess_entry 
 * Insert session entry for a IS_NETBIOS_ACCESS interface
 */
boolean nbf_insert_access_sess_entry (hwidbtype *srcidb, hwidbtype *dstidb,
                         llctype *dst_llc, uchar srcnum, uchar dstnum, 
                         uchar *dstmacaddr)
{

  netbios_sb_t *netb_sb;

  /*
   * Sanity Check
   */
  if (!srcidb) {
      return(FALSE);
  }

  netb_sb = netbios_get_sb(srcidb->firstsw);
  if (!netb_sb || !netb_sb->sess_table_ptr) {
      return(FALSE);
  }

  (&netb_sb->sess_table_ptr[srcnum])->dstnum = dstnum;
  (&netb_sb->sess_table_ptr[srcnum])->dstidb = dstidb;
  (&netb_sb->sess_table_ptr[srcnum])->dst_llc = dst_llc;

  if (dstmacaddr)
     ieee_copy(dstmacaddr, (&netb_sb->sess_table_ptr[srcnum])->dstmacaddr);

  if (netbios_packet_debug) 
      buginf("\nnbf Session table entry inserted: srcnum=%d  "
             "dstnum=%d srcidb=%s dstidb=%s dstmac=%e", 
            srcnum,
            dstnum,
            srcidb->hw_namestring,
            dstidb->hw_namestring,
            dstmacaddr);
   
  return(TRUE);
}


/*
 * nbf_insert_lan_entry
 * Insert session entry for a IS_NETBIOS_LAN interface
 */
netbios_lan_sess_entry *nbf_insert_lan_entry (hwidbtype *srcidb,
                            hwidbtype *dstidb, llctype *dst_llc, 
                            uchar srcnum, uchar dstnum,
                            uchar *srcmacaddr, uchar *dstmacaddr)
{
   queuetype *queue_ptr;
   netbios_lan_sess_entry *new_lan_entry_ptr;

   /*
    * Sanity Check 
    */
   if (!netbios_lan_hash_table_ptr || !srcmacaddr)
      return(NULL);

   /*
    * Hash to the queue
    */
   queue_ptr = (&netbios_lan_hash_table_ptr[nbf_hash(srcmacaddr)]);

   /*
    * Allocate a new entry
    */
   new_lan_entry_ptr = malloc_named(sizeof(netbios_lan_sess_entry),
                          "NetBIOS LAN session entry");

   if (!new_lan_entry_ptr)
      return(NULL);

   new_lan_entry_ptr->srcnum = srcnum;
   new_lan_entry_ptr->dstnum = dstnum;
   new_lan_entry_ptr->srcidb = srcidb;
   new_lan_entry_ptr->dstidb = dstidb;
   new_lan_entry_ptr->dst_llc = dst_llc;

   ieee_copy(srcmacaddr, new_lan_entry_ptr->srcmacaddr);

   if (dstmacaddr)
     ieee_copy(dstmacaddr, new_lan_entry_ptr->dstmacaddr);
   else
     memset(new_lan_entry_ptr->dstmacaddr, 0, IEEEBYTES);

   enqueue(queue_ptr, new_lan_entry_ptr);

   if (netbios_packet_debug) 
      buginf("\nnbf Hash table entry inserted: srcnum=%d  dstnum=%d srcidb=%s dstidb=%s srcmac=%e dstmac=%e", 
            new_lan_entry_ptr->srcnum,
            new_lan_entry_ptr->dstnum,
            new_lan_entry_ptr->srcidb->hw_namestring,
            new_lan_entry_ptr->dstidb->hw_namestring,
            new_lan_entry_ptr->srcmacaddr,
            new_lan_entry_ptr->dstmacaddr);
   
   return(new_lan_entry_ptr);
}


/*
 * nbf_free_access_sess_entry
 * Free session entry (by clearing all fields)
 */
static void nbf_free_access_sess_entry (idbtype *idb, int i, boolean notify)
{

  netbios_sb_t *netb_sb;

  if (!idb) {
      return;
  }

  netb_sb = netbios_get_sb(idb);
  if (!netb_sb || !netb_sb->sess_table_ptr) {
      return;
  }
  
  if ((i>=0) && (i<(NETBIOS_MAX_SESSIONS+1))) {

    /*
     * Send a NetBIOS SESSION_END disconnect message if required
     */
    if (notify)
       nbf_xmit_netbios_session_end( 
                        (&netb_sb->sess_table_ptr[i])->dstnum,
                        i,
                        NULL, idb);

    if (netbios_packet_debug)
       buginf("\n%s nbf session entry freed: srcnum=%d dstnum=%d dstidb=%s "
              "dstmac=%e", 
              idb->hwptr->hw_namestring,
              i, 
              (&netb_sb->sess_table_ptr[i])->dstnum,
              (&netb_sb->sess_table_ptr[i])->dstidb->hw_namestring,
              (&netb_sb->sess_table_ptr[i])->dstmacaddr);

    memset(&netb_sb->sess_table_ptr[i], 0, sizeof(netbios_sess_entry));
  }
}


/*
 * nbf_free_lan_sess_entry
 * Free NetBIOS LAN session entry (i.e. bucket in hash table) 
 */
boolean nbf_free_lan_sess_entry (int hash_table_index, uchar srcnum, 
uchar dstnum, hwidbtype *srcidb, hwidbtype *dstidb, uchar *srcmacaddr, 
boolean notify)
{
  queuetype *queue_ptr;
  netbios_lan_sess_entry *prev_lan_entry = NULL;
  netbios_lan_sess_entry *curr_lan_entry;
  boolean found = FALSE;

  queue_ptr = (&netbios_lan_hash_table_ptr[hash_table_index]);

  if (!queue_ptr)
    return(FALSE);

  curr_lan_entry = queue_ptr->qhead;

  /*
   * Traverse the queue looking for the correct match 
   */
  while ((curr_lan_entry != NULL) && !found) {
    if ((curr_lan_entry->srcnum == srcnum) &&
        (curr_lan_entry->dstnum == dstnum) &&
        (curr_lan_entry->srcidb == srcidb) &&
        (curr_lan_entry->dstidb == dstidb) &&
        ieee_equal(curr_lan_entry->srcmacaddr, srcmacaddr))
       found = TRUE;
    else {
       prev_lan_entry = curr_lan_entry;
       curr_lan_entry = curr_lan_entry->next;
    }
  }

  /*
   * If found, remove this entry from the queue and free it
   */
  if (found) {

   /*
    * Send a NetBIOS SESSION_END disconnect message if required
    */
   if (notify)
      nbf_xmit_netbios_session_end(curr_lan_entry->dstnum, 
                                   curr_lan_entry->srcnum,
                                   curr_lan_entry->srcmacaddr,
                                   curr_lan_entry->srcidb->firstsw);

   if (netbios_packet_debug) 
      buginf("\nnbf Hash table entry freed: srcnum=%d  dstnum=%d "
             "srcidb=%s dstidb=%s srcmac=%e dstmac=%e", 
             curr_lan_entry->srcnum,
             curr_lan_entry->dstnum,
             curr_lan_entry->srcidb->hw_namestring,
             curr_lan_entry->dstidb->hw_namestring,
             curr_lan_entry->srcmacaddr,
             curr_lan_entry->dstmacaddr);

    remqueue(queue_ptr, curr_lan_entry, prev_lan_entry);
    free(curr_lan_entry);
  }

  return(found);
}


/*
 * nbf_hash_table_entries
 * Returns the number of entries per IS_NETBIOS_LAN interface.
 * If idb is NULL, all entries in the hash table are returned
 */
int nbf_hash_table_entries (idbtype *idb)
{
  int i;
  int total = 0;
  netbios_lan_sess_entry *curr_lan_entry;

  if (!netbios_lan_hash_table_ptr)
     return(0);

  /*
   * A Null idb means count all entries
   * Otherwise count only entries that match the specified idb
   */
  if (!idb) {
    for (i=0; i< NETBIOS_HASH_SESS_LEN; i++) 
      total += QUEUESIZE(&netbios_lan_hash_table_ptr[i]);

  } else {

    for (i=0; i< NETBIOS_HASH_SESS_LEN; i++) {
 
       curr_lan_entry= (&netbios_lan_hash_table_ptr[i])->qhead;
 
       /*
        * Traverse the queue looking for the correct match 
        */
       while (curr_lan_entry != NULL) {
         if (curr_lan_entry->srcidb == idb->hwptr) 
            total += 1;
         curr_lan_entry = curr_lan_entry->next;
       }
     }
  }
  return(total);
}


/*
 * display_netbios_interfaces
 * Display netbios session information per interface and global
 */
void display_netbios_interfaces (void)
{
  int i;
  hwidbtype *hwidb;
  netbios_lan_sess_entry *curr_lan_entry;

  /*
   * Check NetBIOS access interface context
   */
   FOR_ALL_HWIDBS(hwidb) {   
     netbios_sb_t *netb_sb = netbios_get_sb(hwidb->firstsw);
     if (netb_sb && netb_sb->sess_table_ptr && IS_NETBIOS_ACCESS(hwidb)) {
        printf("\n\n%s NetBIOS Session Table: \n", hwidb->hw_namestring);
        printf("\nSrcnum   Destnum  Dest-Interface DestMAC\n");  
        for (i=0; i< (NETBIOS_MAX_SESSIONS+1); i++) {
           display_netbios_sess_entry(hwidb, i, (&netb_sb->sess_table_ptr[i]));
       }
     }
  }

  /*
   * Then check global table
   */
  if (!netbios_lan_hash_table_ptr)
     return;

  printf("\n\nNetBIOS Global Session Table: \n");
  printf("\nSrcnum   Destnum   Dest-Interface DestMAC  Src-Interface SrcMac(I)\n"); 

  for (i=0; i<NETBIOS_HASH_SESS_LEN; i++) {

    if (!QUEUEEMPTY(&netbios_lan_hash_table_ptr[i])) {
       curr_lan_entry= (&netbios_lan_hash_table_ptr[i])->qhead;

       /*
        * Traverse the queue looking for the correct match 
        */
       while (curr_lan_entry != NULL) {
         display_netbios_lan_sess_entry(curr_lan_entry, i);
         curr_lan_entry = curr_lan_entry->next;
       }
    }
  }
}


/*
 * display_netbios_sess_entry
 * Display NetBIOS access interface entry information 
 */
void display_netbios_sess_entry (hwidbtype *hwidb, uchar srcnum,
netbios_sess_entry *netbios_sess_entry_ptr)
{
   if (!netbios_sess_entry_ptr->dstidb)
      return;

   printf("%6d   %7d   ", srcnum, netbios_sess_entry_ptr->dstnum);
   printf("%s ", netbios_sess_entry_ptr->dstidb->hw_namestring);

   if (IS_NETBIOS_LAN(netbios_sess_entry_ptr->dstidb)) 
        printf("%e\n", netbios_sess_entry_ptr->dstmacaddr);     
}


/*
 * display_netbios_lan_sess_entry
 * Display NetBIOS lan interface entry information 
 */
void display_netbios_lan_sess_entry (netbios_lan_sess_entry *netbios_lan_sess_entry_ptr, uchar hash_index)
{ 
   if (!netbios_lan_hash_table_ptr)
     return;

   printf("%6d   %7d   ",netbios_lan_sess_entry_ptr->srcnum, 
          netbios_lan_sess_entry_ptr->dstnum);
 
   printf("%s ", netbios_lan_sess_entry_ptr->dstidb->hw_namestring);
   printf("%e  ", netbios_lan_sess_entry_ptr->dstmacaddr);

   printf("%s ", netbios_lan_sess_entry_ptr->srcidb->hw_namestring);
   printf("%e", netbios_lan_sess_entry_ptr->srcmacaddr);     
   printf("(%d)\n", hash_index);
}
