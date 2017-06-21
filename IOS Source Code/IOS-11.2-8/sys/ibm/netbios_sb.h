/* $Id: netbios_sb.h,v 3.1.4.1 1996/05/17 11:21:06 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ibm/netbios_sb.h,v $
 *------------------------------------------------------------------
 * netbios_sb.h -- NETBIOS subblock definition and accessor routines
 *
 * March 1996, Paul Pearce
 *
 * Copyright (c) 1996 by Cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: netbios_sb.h,v $
 * Revision 3.1.4.1  1996/05/17  11:21:06  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.5  1996/04/26  07:54:25  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.1.2.4  1996/04/03  14:34:27  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.1.2.3  1996/03/20  17:54:18  ppearce
 * IBU modularity - NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Code review comment - name changes only
 *
 * Revision 3.1.2.2  1996/03/19  02:04:39  ppearce
 * IBU modularity - NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Incorporate NetBios session access lists into NetBios subblock
 *
 * Revision 3.1.2.1  1996/03/16  12:09:27  ppearce
 * CSCdi50498:  IBU modularity - Implement NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Move NetBios fields from hwidbtype/idbtype structs to NetBios subblock
 *
 * Revision 3.1  1996/03/15  18:56:33  ppearce
 * Placeholder for IbuMod_Calif_branch
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __NETBIOS_SB_H__
#define __NETBIOS_SB_H__

#include "interface.h"
#include "string.h"


/*
 * NETBIOS subblock structure
 * --------------------------
 *
 */
struct netbios_sb_t_ {

    boolean name_caching_enabled;             /* NETB name caching enabled? */

    netbios_acclisttype *ibm_netbios_h_iacf;  /* IBM NETB   host fltr(in)   */
    netbios_acclisttype *ibm_netbios_h_oacf;  /* IBM NETB   host fltr(out)  */
    netbios_acclisttype *ibm_netbios_b_iacf;  /* IBM NETB   bytes fltr(in)  */
    netbios_acclisttype *ibm_netbios_b_oacf;  /* IBM NETB   bytes fltr(out) */
    netbios_acclisttype *ibm_netbios_bs_iacf; /* IBM NETB I-frame fltr(in)  */
    netbios_acclisttype *ibm_netbios_bs_oacf; /* IBM NETB I-frame fltr(out) */

    netbios_acclisttype *nov_netbios_h_iacf; /* Novell NETB  host fltr(in)  */
    netbios_acclisttype *nov_netbios_h_oacf; /* Novell NETB  host fltr(out) */
    netbios_acclisttype *nov_netbios_b_iacf; /* Novell NETB bytes fltr(in)  */
    netbios_acclisttype *nov_netbios_b_oacf; /* Novell NETB bytes fltr(out) */

    boolean access_server_enabled;           /* NETB Access Server enabled  */
    netbios_sess_entry *sess_table_ptr;      /* Ptr to NETB AS sess table   */

    boolean nbfcp_pak_modified;        /* NBFCP Name Projection change flag */
    paktype *netbios_nbfcp_pak;        /* NBFCP Name Projection packet      */

};
STRUCTURE_REF(netbios_sb_t);


/* NETBIOS subblock management routines
 * ------------------------------------
 *
 */
 

/* 
 * netbios_create_sb()
 *   Create NETBIOS subblock
 *
 */
static inline netbios_sb_t *netbios_create_sb (idbtype *swidb)
{
    netbios_sb_t *sb;
    swidb_sb_t sbtype = SWIDB_SB_NETBIOS;
 
    /*
     * Create subblock (if it doesn't already exist)
     *
     */
    sb = idb_get_swsb(swidb, sbtype);
    if (!sb) {
        sb = malloc_named(sizeof(netbios_sb_t), "SWIDB_SB: NETBIOS Info");
        memset(sb, 0, sizeof(netbios_sb_t));
        if (!sb) {
          return(NULL);
        }
        idb_add_swsb(swidb, &sbtype, sb);
    }

    /*
     * Initialize subblock fields
     *
     */

    sb->name_caching_enabled = FALSE;

    sb->ibm_netbios_h_iacf = NULL;
    sb->ibm_netbios_h_oacf = NULL;
    sb->ibm_netbios_b_iacf = NULL;
    sb->ibm_netbios_b_oacf = NULL;
    sb->ibm_netbios_bs_iacf = NULL;
    sb->ibm_netbios_bs_oacf = NULL;

    sb->nov_netbios_h_iacf = NULL;
    sb->nov_netbios_h_oacf = NULL;
    sb->nov_netbios_b_iacf = NULL;
    sb->nov_netbios_b_oacf = NULL;

    sb->access_server_enabled = FALSE;
    sb->sess_table_ptr = NULL;

    sb->nbfcp_pak_modified = FALSE;
    sb->netbios_nbfcp_pak = NULL;

    return(sb);

} /* End netbios_create_sb() */


/* 
 * netbios_get_sb()
 *   Return NETBIOS subblock
 *
 */
static inline netbios_sb_t *netbios_get_sb (idbtype *swidb)
{
  return(idb_get_swsb(swidb, SWIDB_SB_NETBIOS));
}


/*
 * netbios_get_or_create_sb()
 *   Return existing subblock (if it exists)
 *   if subblock does not exist,
 *     create the subblock
 *
 */
static inline
netbios_sb_t *netbios_get_or_create_sb (idbtype *swidb)
{
  netbios_sb_t *sb;
 
  sb = netbios_get_sb(swidb);
  if (!sb) {
    sb = netbios_create_sb(swidb);
  }
  return(sb);
}


/* 
 * netbios_destroy_sb()
 *   Delete NETBIOS subblock - returns FALSE if not destroyed
 *
 */
static inline boolean netbios_destroy_sb (idbtype *swidb)
{
    netbios_sb_t *sb;
    swidb_sb_t sbtype = SWIDB_SB_NETBIOS;

    sb = netbios_get_sb(swidb); 
    if (!sb) {
        return(FALSE);
    }

    if (!idb_delete_swsb(swidb, sbtype)) {
        return(FALSE);
    }

    free(sb);
    return(TRUE);
}


/* NETBIOS subblock accessor routines
 * ----------------------------------
 *
 */


/* 
 * netbios_sb_name_caching_is_enabled()
 *   Returns TRUE if NetBios name caching is enabled
 *     (i.e. (name_caching_enabled == TRUE)
 *
 */
static inline
boolean netbios_sb_name_caching_is_enabled (idbtype *swidb)
{
    netbios_sb_t  *sb;
    sb = netbios_get_sb(swidb);
    if (sb) {
        return(sb->name_caching_enabled);
    } else {
        return(FALSE);
    }
}
  


/* 
 * netbios_sb_enable_name_caching()
 *   Sets netbios_sb->name_caching_enabled to TRUE
 *   Returns FALSE if netbios_sb does not exist
 *
 */
static inline
boolean netbios_sb_enable_name_caching (idbtype *swidb)
{
    netbios_sb_t  *sb;
    sb = netbios_get_sb(swidb);
    if (sb) {
        sb->name_caching_enabled = TRUE;
        return(TRUE);
    } else {
        return(FALSE);
    }
}
  

/* 
 * netbios_sb_disable_name_caching()
 *   Sets netbios_sb->name_caching_enabled to FALSE
 *   Returns FALSE if netbios_sb does not exist 
 *
 */
static inline
boolean netbios_sb_disable_name_caching (idbtype *swidb)
{
    netbios_sb_t  *sb;
    sb = netbios_get_sb(swidb);
    if (sb) {
        sb->name_caching_enabled = FALSE;
        return(TRUE);
    } else {
        return(FALSE);
    }
}
  

/*
 * netbios_sb_ibm_iacf_is_enabled() 
 * netbios_sb_ibm_oacf_is_enabled() 
 * changed to static non-inline, 
 * will change name to remove _sb (it's not a sb function)?
 * moved to netbios_acl.h
 */

/*
 * netbios_sb_ibm_b_iacf_is_enabled() 
 *   Returns TRUE if bytes/I-frame IBM NetBios input access filter is enabled
 *
 */
static inline
boolean netbios_sb_ibm_b_iacf_is_enabled (idbtype *swidb)
{
    netbios_sb_t  *sb;

    sb = netbios_get_sb(swidb);
    if (sb) {
        return(sb->ibm_netbios_b_iacf || sb->ibm_netbios_bs_iacf);
    } else {
        return(FALSE);
    }
}


/*
 * netbios_sb_ibm_b_oacf_is_enabled() 
 *   Returns TRUE if bytes/I-frame output access filter is enabled
 *
 */
static inline
boolean netbios_sb_ibm_b_oacf_is_enabled (idbtype *swidb)
{
    netbios_sb_t  *sb;

    sb = netbios_get_sb(swidb);
    if (sb) {
        return(sb->ibm_netbios_b_oacf || sb->ibm_netbios_bs_oacf);
    } else {
        return(FALSE);
    }
}


/*
 * netbios_sb_access_server_is_enabled()
 *   Returns TRUE if NetBios Access Server is enabled
 *     (i.e. (access_server_enabled == TRUE)
 *
 */
static inline
boolean netbios_sb_access_server_is_enabled (idbtype *swidb)
{
    netbios_sb_t  *sb;
    sb = netbios_get_sb(swidb);
    if (sb) {
        return(sb->access_server_enabled);
    } else {
        return(FALSE);
    }
}


#endif __NETBIOS_SB_H__
