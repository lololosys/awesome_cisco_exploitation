/* $Id: llc2_sb.h,v 3.1.14.3 1996/07/11 04:51:25 monical Exp $
 * $Source: /release/112/cvs/Xsys/llc2/llc2_sb.h,v $
 *------------------------------------------------------------------
 * llc_sb.h -- LLC subblock definition and accessor routines
 *
 * February 1996, Paul Pearce
 *
 * Copyright (c) 1996-1997 by Cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: llc2_sb.h,v $
 * Revision 3.1.14.3  1996/07/11  04:51:25  monical
 * CSCdi60994:  llc2 dynwind command not parsed correctly
 * Branch: California_branch
 *
 *
 * This ddts also includes a fix for the problem where the parser
 * accepts invalid values, even though an error message was displayed.
 *
 * Revision 3.1.14.2  1996/05/18  13:24:18  ppearce
 * CSCdi58098:  Remove use of llc2_default[] table - use defaults directly
 * Branch: California_branch
 *
 * Revision 3.1.14.1  1996/05/17  11:27:14  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.4  1996/03/21  05:12:59  ppearce
 * IBU modularity: - LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Move LLC2 defaults definition into the LLC subblock file
 *
 * Revision 3.1.2.3  1996/03/17  18:07:04  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.1.2.2  1996/03/15  05:55:51  ppearce
 * CSCdi50498:  IBU modularity - Implement LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Code review comments
 *
 * Revision 3.1.2.1  1996/03/01  16:31:16  ppearce
 * CSCdi50498:  IBU modularity - Implement LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Move LLC fields from idbtype struct to LLC subblock
 *
 * Revision 3.1  1996/02/29  22:35:17  ppearce
 * Add placeholder to IbuMod_Calif_branch
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LLC2_SB_H__
#define __LLC2_SB_H__

#include "interface.h"


/* 
 * LLC2 default values
 *
 */
#define AKMAX_DEFAULT    (3)
#define AKDELAY_DEFAULT  (100L)
#define IDLE_DEFAULT     (10*ONESEC)
#define LOC_WIND_DEFAULT (7)
#define REM_WIND_DEFAULT (127)
#define N2_DEFAULT       (8) 
#define T1_DEFAULT       (10*100L)
#define LLC1_T_DEFAULT   (1 * ONESEC)
#define TBUSY_DEFAULT    (12*800L)
#define TREJ_DEFAULT     (4*800L)
#define TPF_DEFAULT      (10*100L)
#define XID_NEG_TIME     (0)
#define XID_RTRY_TIME    (75*800L)

#define EXPLORER_TIME    (1000)     /* millisecs to wait for TEST response */
#define EXPLORER_RETRY   (3)        /* Number of time to retry TEST        */
#define XID_TIME         (1000)     /* millisecs to wait for XID response  */
#define XID_RETRY        (3)        /*  Number of time to retry XID        */

#define ADM_TIMER_DEFAULT (60*ONESEC)

#define L3_PID           (0x83)     /* L3 PID for APPN traffic             */
#define TXQ_MAX_DEFAULT  (200)      /* Default LLC transmit queue depth    */


/*
 * Frame Relay congestion control defaults
 *
 */
#define FRAME_RELAY_DWC_DEFAULT (1)
#define FRAME_RELAY_DWC_0       (0)  /*value when no window size is specified*/
#define FRAME_RELAY_DWC_1       (2)
#define FRAME_RELAY_DWC_2       (4)
#define FRAME_RELAY_DWC_3       (8)
#define FRAME_RELAY_DWC_4       (16)
#define DYNWIND_NW_DEFAULT      (4)


/*
 * LLC subblock structure
 * ----------------------
 *
 */
struct llc_sb_t_ {
 
    ulong llc2_wind;                /* Max number unack'd I-frames xmit'd    */
    ulong llc2_akmax;               /* Max number unack'd I-frames rcv'd     */
    ulong llc2_akdelay_time;        /* Max time I-frame remains unack'd      */
    ulong llc2_n2;                  /* N2 retransmission count               */
    ulong llc2_T1_time;             /* T1 time - wait for ack of I-frame     */
    ulong llc2_trej_time;           /* TREJ time  - wait for reject I-frame  */
    ulong llc2_tpf_time;            /* TPF time   - wait for final response  */
    ulong llc2_tbusy_time;          /* TBUSY time - wait before re-polling   */
    ulong llc2_idle_time;           /* IDLE time  - freq. of idle polling    */
    ulong llc2_txQ_max;             /* Max depth of LLC2 transmit queue      */
    ulong llc2_xid_time;            /* Time between XID retries              */
    ulong llc2_xid_retry;           /* Number of times to retry XID          */
    ulong llc2_xid_rtry_time;       /* XID retry time - wait for XID rsp     */
    ulong llc2_xid_neg_val_time;    /* Frequency of XID                      */
    ulong llc2_explorer_time;       /* Time between explorer retries         */
    ulong llc2_explorer_retry;      /* Number of times to retry explorer     */
    ulong llc2_l3_pid;              /* Layer 3 protocol ID                   */
    ulong llc2_adm_timer_value;     /* Timer for ADM timeout for APPN        */
    boolean llc2_dynwind;           /* Dynamic window configured?            */
    ulong   llc2_nw;                /* Dynamic window for unack'd I-frames   */
    ulong   llc2_fr_dwc;            /* Dynamic window for Frame Relay        */
    sap_registry_t *llc2_sap_registry;   /* SAP registry for CLSI            */

};
STRUCTURE_REF(llc_sb_t);


/* LLC subblock management routines 
 * --------------------------------
 *
 */


/* 
 * llc_create_sb()
 *   Create LLC subblock 
 *
 */
static inline 
llc_sb_t *llc_create_sb (idbtype *swidb)
{
    llc_sb_t *sb;
    swidb_sb_t sbtype = SWIDB_SB_LLC2;

    /* 
     * Create subblock (if it doesn't already exist)
     *
     */
    sb = idb_get_swsb(swidb, sbtype);
    if (!sb) {
        sb = malloc_named(sizeof(llc_sb_t), "SWIDB_SB: LLC2 Info");
        memset(sb, 0, sizeof(llc_sb_t));
        if (!sb) {
          return(NULL);
        }
        idb_add_swsb(swidb, &sbtype, sb);
    }

    /* 
     * Initialize subblock fields
     *
     */
    sb->llc2_wind          = LOC_WIND_DEFAULT;
    sb->llc2_akmax         = AKMAX_DEFAULT;
    sb->llc2_akdelay_time  = AKDELAY_DEFAULT;
    sb->llc2_n2            = N2_DEFAULT;
    sb->llc2_T1_time       = T1_DEFAULT;
    sb->llc2_trej_time     = TREJ_DEFAULT;
    sb->llc2_tpf_time      = TPF_DEFAULT;
    sb->llc2_tbusy_time    = TBUSY_DEFAULT;
    sb->llc2_idle_time     = IDLE_DEFAULT;
    sb->llc2_txQ_max       = TXQ_MAX_DEFAULT;

    sb->llc2_xid_time      = XID_TIME;
    sb->llc2_xid_retry     = XID_RETRY;
    sb->llc2_xid_rtry_time = XID_RTRY_TIME;

    sb->llc2_xid_neg_val_time = XID_NEG_TIME;

    sb->llc2_explorer_time  = EXPLORER_TIME;
    sb->llc2_explorer_retry = EXPLORER_RETRY;

    sb->llc2_l3_pid  = L3_PID;

    sb->llc2_dynwind = FALSE;
    sb->llc2_nw      = DYNWIND_NW_DEFAULT;
    sb->llc2_fr_dwc  = FRAME_RELAY_DWC_DEFAULT;

    sb->llc2_adm_timer_value = ADM_TIMER_DEFAULT;

    if (sb->llc2_sap_registry) {
      ; /* possible memory leak --ppearce*/
    }
    sb->llc2_sap_registry = NULL;

    return(sb);

} /* End llc_create_sb() */


/* 
 * llc_get_sb()
 *   Return LLC subblock if it exists
 *   Return NULL if LLC subblock does not exist
 *
 */
static inline 
llc_sb_t *llc_get_sb (idbtype *swidb)
{
  return(idb_get_swsb(swidb, SWIDB_SB_LLC2));
}


/* 
 * llc_get_or_create_sb()
 *   Return existing subblock (if it exists)
 *   if subblock does not exist,
 *     create the subblock
 *
 */
static inline 
llc_sb_t *llc_get_or_create_sb (idbtype *swidb)
{
  llc_sb_t *sb;

  sb = llc_get_sb(swidb);
  if (!sb) {
    sb = llc_create_sb(swidb);
  }
  return(sb);
}


/*
 * llc_destroy_sb()
 *   Delete LLC subblock - returns FALSE if not destroyed
 *
 */
static inline 
boolean llc_destroy_sb (idbtype *swidb)
{
    llc_sb_t *sb;
    swidb_sb_t sbtype = SWIDB_SB_LLC2;
 
    sb = llc_get_sb(swidb);
    if (!sb) {
        return(FALSE);
    }
 
    if (!idb_delete_swsb(swidb, sbtype)) {
        return(FALSE);
    }
 
    free(sb);
    return(TRUE);
}


/* LLC subblock utility routines 
 * -----------------------------
 *
 */


/*
 *llc_sb_copy()
 *  Copy all fields from source LLC subblock to destination LLC subblock
 *
 */
static inline 
boolean llc_sb_copy (idbtype *swidb_dst, idbtype *swidb_src)
{
    llc_sb_t *sb_dst;
    llc_sb_t *sb_src;

    sb_src = llc_get_sb(swidb_src);
    sb_dst = llc_get_or_create_sb(swidb_dst);

    if ( (sb_dst == NULL) || (sb_src == NULL) ) {
        return(FALSE);
    }

    sb_dst->llc2_wind             = sb_src->llc2_wind;
    sb_dst->llc2_akmax            = sb_src->llc2_akmax;
    sb_dst->llc2_akdelay_time     = sb_src->llc2_akdelay_time;
    sb_dst->llc2_n2               = sb_src->llc2_n2;
    sb_dst->llc2_T1_time          = sb_src->llc2_T1_time;
    sb_dst->llc2_trej_time        = sb_src->llc2_trej_time;
    sb_dst->llc2_tpf_time         = sb_src->llc2_tpf_time;
    sb_dst->llc2_tbusy_time       = sb_src->llc2_tbusy_time;
    sb_dst->llc2_idle_time        = sb_src->llc2_idle_time;
    sb_dst->llc2_txQ_max          = sb_src->llc2_txQ_max;
    sb_dst->llc2_xid_time         = sb_src->llc2_xid_time;
    sb_dst->llc2_xid_retry        = sb_src->llc2_xid_retry;
    sb_dst->llc2_xid_rtry_time    = sb_src->llc2_xid_rtry_time;
    sb_dst->llc2_xid_neg_val_time = sb_src->llc2_xid_neg_val_time;
    sb_dst->llc2_explorer_time    = sb_src->llc2_explorer_time;
    sb_dst->llc2_explorer_retry   = sb_src->llc2_explorer_retry;
    sb_dst->llc2_l3_pid           = sb_src->llc2_l3_pid;
    sb_dst->llc2_dynwind          = sb_src->llc2_dynwind;
    sb_dst->llc2_adm_timer_value  = sb_src->llc2_adm_timer_value;
    sb_dst->llc2_nw               = sb_src->llc2_nw;
    sb_dst->llc2_fr_dwc           = sb_src->llc2_fr_dwc;

    /* 
     * Don't copy sap_registry 
     *   Destination subblock can allocate its own if needed --ppearce
     *
     */
    sb_dst->llc2_sap_registry     = NULL;

    return(TRUE);

} /* End llc_sb_copy() */


/* LLC subblock accessor routines 
 * ------------------------------
 *
 */


/*
 * llc_sb_get_xxx()
 *   Returns specified field from LLC subbblock
 *
 */

static inline 
ulong llc_sb_get_wind (idbtype *swidb)
{
    llc_sb_t *sb = llc_get_sb(swidb);
    if (sb)
      return(sb->llc2_wind);
    else 
      return(LOC_WIND_DEFAULT);
}

static inline 
ulong llc_sb_get_akmax (idbtype *swidb)
{
    llc_sb_t *sb = llc_get_sb(swidb);
    if (sb)
      return(sb->llc2_akmax);
    else 
      return(AKMAX_DEFAULT);
}

static inline 
ulong llc_sb_get_akdelay_time (idbtype *swidb)
{
    llc_sb_t *sb = llc_get_sb(swidb);
    if (sb)
      return(sb->llc2_akdelay_time);
    else 
      return(AKDELAY_DEFAULT);
}

static inline 
ulong llc_sb_get_n2 (idbtype *swidb)
{
    llc_sb_t *sb = llc_get_sb(swidb);
    if (sb)
      return(sb->llc2_n2);
    else 
      return(N2_DEFAULT);
}

static inline 
ulong llc_sb_get_T1_time (idbtype *swidb)
{
    llc_sb_t *sb = llc_get_sb(swidb);
    if (sb)
      return(sb->llc2_T1_time);
    else 
      return(T1_DEFAULT);
}

static inline 
ulong llc_sb_get_trej_time (idbtype *swidb)
{
    llc_sb_t *sb = llc_get_sb(swidb);
    if (sb)
      return(sb->llc2_trej_time);
    else 
      return(TREJ_DEFAULT);
}

static inline 
ulong llc_sb_get_tpf_time (idbtype *swidb)
{
    llc_sb_t *sb = llc_get_sb(swidb);
    if (sb)
      return(sb->llc2_tpf_time);
    else 
      return(TPF_DEFAULT);
}

static inline 
ulong llc_sb_get_tbusy_time (idbtype *swidb)
{
    llc_sb_t *sb = llc_get_sb(swidb);
    if (sb)
      return(sb->llc2_tbusy_time);
    else 
      return(TBUSY_DEFAULT);
}

static inline 
ulong llc_sb_get_idle_time (idbtype *swidb)
{
    llc_sb_t *sb = llc_get_sb(swidb);
    if (sb)
      return(sb->llc2_idle_time);
    else 
      return(IDLE_DEFAULT);
}

static inline 
ulong llc_sb_get_txQ_max (idbtype *swidb)
{
    llc_sb_t *sb = llc_get_sb(swidb);
    if (sb)
      return(sb->llc2_txQ_max);
    else 
      return(TXQ_MAX_DEFAULT);
}

static inline 
ulong llc_sb_get_xid_time (idbtype *swidb)
{
    llc_sb_t *sb = llc_get_sb(swidb);
    if (sb)
      return(sb->llc2_xid_time);
    else 
      return(XID_TIME);
}

static inline 
ulong llc_sb_get_xid_retry (idbtype *swidb)
{
    llc_sb_t *sb = llc_get_sb(swidb);
    if (sb)
      return(sb->llc2_xid_retry);
    else 
      return(XID_RETRY);
}

static inline 
ulong llc_sb_get_xid_rtry_time (idbtype *swidb)
{
    llc_sb_t *sb = llc_get_sb(swidb);
    if (sb)
      return(sb->llc2_xid_rtry_time);
    else 
      return(XID_RTRY_TIME);
}

static inline 
ulong llc_sb_get_xid_neg_val_time (idbtype *swidb)
{
    llc_sb_t *sb = llc_get_sb(swidb);
    if (sb)
      return(sb->llc2_xid_neg_val_time);
    else 
      return(XID_NEG_TIME);
}

static inline 
ulong llc_sb_get_explorer_time (idbtype *swidb)
{
  llc_sb_t *sb = llc_get_sb(swidb);
  if (sb)
    return(sb->llc2_explorer_time);
  else 
    return(EXPLORER_TIME);
}

static inline 
ulong llc_sb_get_explorer_retry (idbtype *swidb)
{
    llc_sb_t *sb = llc_get_sb(swidb);
    if (sb)
      return(sb->llc2_explorer_retry);
    else 
      return(EXPLORER_RETRY);
}

static inline 
ulong llc_sb_get_l3_pid (idbtype *swidb)
{
    llc_sb_t *sb = llc_get_sb(swidb);
    if (sb)
      return(sb->llc2_l3_pid);
    else 
      return(L3_PID);
}

static inline 
boolean llc_sb_get_dynwind (idbtype *swidb)
{
    llc_sb_t *sb = llc_get_sb(swidb);
    if (sb)
      return(sb->llc2_dynwind);
    else 
      return(FALSE);
}

static inline 
ulong llc_sb_get_llc2_nw (idbtype *swidb)
{
    llc_sb_t *sb = llc_get_sb(swidb);
    if (sb)
      return(sb->llc2_nw);
    else 
      return(DYNWIND_NW_DEFAULT);
}

static inline 
ulong llc_sb_get_fr_dwc (idbtype *swidb)
{
    llc_sb_t *sb = llc_get_sb(swidb);
    if (sb)
      return(sb->llc2_fr_dwc);
    else 
      return(FRAME_RELAY_DWC_DEFAULT);
}


/*
 *
 * llc_sb_get_sap_registry()
 *   Returns SAP registry pointer
 *
 */
static inline 
sap_registry_t *llc_sb_get_sap_registry (idbtype *swidb)
{
    llc_sb_t *sb = llc_get_sb(swidb);
    if (sb)
      return(sb->llc2_sap_registry);
    else 
      return(NULL);
}


/*
 * llc2_setidb_sap_registry()
 *   Sets SAP registry pointer - returns FALSE if "set" fails
 *
 */
static inline 
boolean llc_sb_set_sap_registry (idbtype *swidb, sap_registry_t *sap_registry)
{
    llc_sb_t *sb = llc_get_sb(swidb);
    if (sb) {
        sb->llc2_sap_registry = sap_registry;
        return(TRUE);
    } else {
        return(FALSE);
    }
}

#endif __LLC2_SB_H__
