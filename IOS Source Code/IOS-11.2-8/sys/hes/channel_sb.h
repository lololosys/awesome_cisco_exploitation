/* $Id: channel_sb.h,v 3.1.4.2 1996/05/29 18:25:00 motto Exp $
 * $Source: /release/112/cvs/Xsys/hes/channel_sb.h,v $
 *------------------------------------------------------------------
 * channel_sb.h -- CHANNEL subblock definition and accessor routines
 *
 * March 1996, Paul Pearce
 *
 * Copyright (c) 1996 by Cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: channel_sb.h,v $
 * Revision 3.1.4.2  1996/05/29  18:25:00  motto
 * CSCdi46832:  vencap failed debug messages displayed for non cip
 * explorers
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  11:16:59  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.2  1996/04/03  14:27:26  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.1.2.1  1996/03/21  06:07:13  ppearce
 * IBU modularity: Channel subblock
 * Branch: IbuMod_Calif_branch
 *   Move Channel fields from hwidbtype/idbtype structs to Channel subblock
 *
 * Revision 3.1  1996/03/20  16:50:42  ppearce
 * Placeholder for IbuMod_Calif_branch
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CHANNEL_SB_H__
#define __CHANNEL_SB_H__

#include "string.h"

/* 
 * Invalid channel VC 
 *
 */
#define NO_CHANNEL_VC 0xFFFF


/*
 * Defined media types for Channel LAN emulation
 *
 */
enum CIP_VLAN_MEDIA {
     CIP_VLAN_UNKNOWN = -1,
     CIP_VLAN_ETHER   =  0,
     CIP_VLAN_TOKEN   =  1,
     CIP_VLAN_FDDI    =  2
};


/* 
 * Typedef's for CSNA vector routines
 *
 */
typedef ushort (*channel_ilan_vc_from_macaddr_t)(hwidbtype *, uchar *, uchar *);
typedef hwidbtype *(*channel_ilan_hwidb_from_vc_t)(hwidbtype *, ushort);


/* 
 * Default routine for channel_sb->get_vc_from_macaddr() vector
 *    return_NO_CHANNEL_VC() defined in ..hes//if_cip.c
 *
 */
extern ushort return_NO_CHANNEL_VC(void *dummy, ...);
 

/*
 * CHANNEL subblock structure
 * --------------------------
 *
 */
struct channel_sb_t_ {

    struct cip_idb_info *cip_info;       /* Pointer to CIP info        */

    enum CIP_VLAN_MEDIA emulated_media;  /* CSNA internal LAN media    */

    void *channel_ilan_access_list;

    channel_ilan_hwidb_from_vc_t   get_hwidb_from_vc;
    channel_ilan_vc_from_macaddr_t get_vc_from_macaddr;

};
STRUCTURE_REF(channel_sb_t);


/* CHANNEL subblock management routines
 * ------------------------------------
 *
 */
 

/* 
 * channel_create_sb()
 *   Create CHANNEL subblock
 *
 */
static inline channel_sb_t *channel_create_sb (hwidbtype *hwidb)
{
    channel_sb_t *sb;
    hwidb_sb_t sbtype = HWIDB_SB_CHANNEL;
 
    /*
     * Create subblock (if it doesn't already exist)
     *
     */
    sb = idb_get_hwsb(hwidb, sbtype);
    if (!sb) {
        sb= malloc_named(sizeof(channel_sb_t), "HWIDB_SB: CHANNEL Info");
        memset(sb, 0, sizeof(channel_sb_t));
        if (!sb) {
          return(NULL);
        }
        idb_add_hwsb(hwidb, &sbtype, sb);
    }

    /*
     * Initialize subblock fields
     *
     */

 
    sb->cip_info = NULL;

    sb->emulated_media = CIP_VLAN_UNKNOWN;

    sb->channel_ilan_access_list = NULL;

    sb->get_hwidb_from_vc = (channel_ilan_hwidb_from_vc_t)return_null;

    sb->get_vc_from_macaddr 
                 = (channel_ilan_vc_from_macaddr_t)return_NO_CHANNEL_VC;

    return(sb);

} /* End channel_create_sb() */


/* 
 * channel_get_sb()
 *   Return CHANNEL subblock 
 *
 */
static inline channel_sb_t *channel_get_sb (hwidbtype *hwidb)
{
    return(idb_get_hwsb(hwidb, HWIDB_SB_CHANNEL));
}


/*
 * channel_get_or_create_sb()
 *   Return existing subblock (if it exists)
 *   if subblock does not exist,
 *     create the subblock
 *
 */
static inline
channel_sb_t *channel_get_or_create_sb (hwidbtype *hwidb)
{
    channel_sb_t *sb;
 
    sb = channel_get_sb(hwidb);
    if (!sb) {
      sb = channel_create_sb(hwidb);
    }
    return(sb);
}


/* 
 * channel_destroy_sb()
 *   Delete CHANNEL subblock - returns FALSE if not destroyed
 *
 */
static inline boolean channel_destroy_sb (hwidbtype *hwidb)
{
    channel_sb_t *sb;
    hwidb_sb_t sbtype = HWIDB_SB_CHANNEL;

    sb = channel_get_sb(hwidb); 
    if (!sb) {
        return(FALSE);
    }

    if (!idb_delete_hwsb(hwidb, sbtype)) {
        return(FALSE);
    }

    free(sb);
    return(TRUE);
}


/* CHANNEL subblock accessor routines
 * ----------------------------------
 *
 */


/*
 * channel_sb_get_cip_info()
 *   Returns "cip_info" pointer
 *
 */
static inline
struct cip_idb_info *channel_sb_get_cip_info (hwidbtype *hwidb)
{
    channel_sb_t *sb = channel_get_sb(hwidb);
    if (sb)
      return(sb->cip_info);
    else
      return(NULL);
}


/*
 * channel_sb_cip_info_is_null()
 *   Returns TRUE if "cip_info" pointer is null
 *
 */
static inline
boolean channel_sb_cip_info_is_null (hwidbtype *hwidb)
{
    if (channel_sb_get_cip_info(hwidb) == NULL)
      return(TRUE);
    else
      return(FALSE);
}


/*
 * channel_sb_get_emulated_media()
 *   Returns value of "emulated_media"
 *
 */
static inline
enum CIP_VLAN_MEDIA channel_sb_get_emulated_media (hwidbtype *hwidb)
{
    channel_sb_t *sb = channel_get_sb(hwidb);
    if (sb)
      return(sb->emulated_media);
    else
      return(CIP_VLAN_UNKNOWN);
}

#endif __CHANNEL_SB_H__
