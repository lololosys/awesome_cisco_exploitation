/* $Id: sysmgt_cipcsna.h,v 3.2 1995/11/17 09:21:19 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/sysmgt_cipcsna.h,v $
 *------------------------------------------------------------------
 * sysmgt_cipcsna.h - system management data structures for SNA on the CIP.
 *
 * April 1995, Mike Otto
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sysmgt_cipcsna.h,v $
 * Revision 3.2  1995/11/17  09:21:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:43:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/18  21:46:35  motto
 * Update MIBs to current level of CIP support
 * Cleaned up CSNA show command support
 * CSCdi35714:  Some show commands for CIP-SNA are not working
 *
 * Revision 2.1  1995/06/07  20:45:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SYSMGT_CIPCSNA_H__
#define __SYSMGT_CIPCSNA_H__

/*
 * Special Note:  Keep this file in sync between the the CIP microcode
 *                and RP/RSP system code. The file will have the same
 *                name in both repositories. You must edit the changes
 *                so the cvs header does not get screwed up.   /motto
 */
 
 
#include "sysmgt_snallc.h"

#define MAX_SAPS_PER_CIP   256

typedef enum sysmgt_csna_slowdown_state_ {
    CSNA_SLOWDOWN_NORMAL,
    CSNA_SLOWDOWN_SENT,
    CSNA_SLOWDOWN_RECEIVED,
    CSNA_SLOWDOWN_SENT_AND_RECEIVED
} sysmgt_csna_slowdown_state_t;

typedef enum sysmgt_csna_state {
    CSNA_CLOSED,
    CSNA_PENDING_OPEN,
    CSNA_OPEN,
    CSNA_PENDING_SETUP,
    CSNA_SETUP_COMPLETE,
    CSNA_PENDING_CLOSE
} sysmgt_csna_state_t;

typedef enum sysmgt_csna_table_ {
    CSNA_DEVICE,
    CSNA_LLC2_CONN
} sysmgt_csna_table_t;

typedef struct csna_dev_index_ {
    unsigned long      vcn;
} csna_dev_index_t;

typedef struct csna_dev_ {
    csna_dev_index_t   index;
    long               cipCardCsnaOperState;
    long               cipCardCsnaOperSlowDownState;
    long               cipCardCsnaOperBlockDelayTime;
    long               cipCardCsnaOperBlockDelayLength;
    long               cipCardCsnaOperMaxBlockLength;
    unsigned long      cipCardCsnaStatsBlocksTxd;
    unsigned long      cipCardCsnaStatsBlocksRxd;
    unsigned long      cipCardCsnaStatsBytesTxd_msw;
    unsigned long      cipCardCsnaStatsBytesTxd_lsw;
    unsigned long      cipCardCsnaStatsBytesRxd_msw;
    unsigned long      cipCardCsnaStatsBytesRxd_lsw;
    unsigned long      cipCardCsnaStatsBlocksTxByBlockDelayTime;
    unsigned long      cipCardCsnaStatsBytesTxByBlockDelayTime_msw;
    unsigned long      cipCardCsnaStatsBytesTxByBlockDelayTime_lsw;
    unsigned long      cipCardCsnaStatsBlocksTxByBlockDelayLength;
    unsigned long      cipCardCsnaStatsBytesTxByBlockDelayLength_msw;
    unsigned long      cipCardCsnaStatsBytesTxByBlockDelayLength_lsw;
    unsigned long      cipCardCsnaStatsBlocksTxByMaxBlockLength;
    unsigned long      cipCardCsnaStatsBytesTxByMaxBlockLength_msw;
    unsigned long      cipCardCsnaStatsBytesTxByMaxBlockLength_lsw;
    unsigned long      cipCardCsnaStatsSlowDownsSent;
    unsigned long      cipCardCsnaStatsSlowDownsReceived;
} csna_dev_t;

typedef struct csna_handle_ {
    unsigned long csna_handle;
} csna_handle_t;

typedef struct csna_llc2_conn_ {
    csna_handle_t   index;
    unsigned long   cipCardCsnaPort;
    unsigned long   cipCardCsnaConnPath;
    unsigned long   cipCardCsnaConnDevice;
} csna_llc2_conn_t;

#define CSNA_DEVICE_INDEX_SIZE     sizeof(csna_dev_index_t)
#define CSNA_DEVICE_SIZE           sizeof(csna_dev_t)
#define CSNA_LLC2_CONN_SIZE        sizeof(csna_llc2_conn_t) 
#define CSNA_HANDLE_SIZE           sizeof(csna_handle_t)

#endif /* __SYSMGT_CIPCSNA_H__ */
