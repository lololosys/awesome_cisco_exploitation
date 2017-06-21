/* $Id: parser_defs_rtt.h,v 1.1.4.2 1996/06/07 22:25:53 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/parser_defs_rtt.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor parse defaults header file.
 *
 * March 1996, Larry Metzger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the defaults for
 * the RTT Monitor parser configuration.
 *------------------------------------------------------------------
 * $Log: parser_defs_rtt.h,v $
 * Revision 1.1.4.2  1996/06/07  22:25:53  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/17  11:40:53  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.2  1996/04/19  02:41:17  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:30  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:22  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  Base Configuration defines
 */
#define RTR_CFG_RESET_CMD                1
#define RTR_CFG_SCHEDULE_CMD             2
#define RTR_CFG_REACT_CONFIG_CMD         3
#define RTR_CFG_REACT_TRIG_CMD           4
#define RTR_CFG_ENTRY_CMD                5

/*
 *  Entry Configuration defines
 */
#define RTR_ENTRY_CFG_TYPE               1
#define RTR_ENTRY_CFG_OWNER              2
#define RTR_ENTRY_CFG_TAG                3
#define RTR_ENTRY_CFG_THRESHOLD          4
#define RTR_ENTRY_CFG_FREQUENCY          5
#define RTR_ENTRY_CFG_TIMEOUT            6
#define RTR_ENTRY_CFG_VERIFY_DATA        7
#define RTR_ENTRY_CFG_REQUEST_SIZE       8
#define RTR_ENTRY_CFG_RESPONSE_SIZE      9
#define RTR_ENTRY_CFG_HOURS_SIZE         10
#define RTR_ENTRY_CFG_PATHS_SIZE         11
#define RTR_ENTRY_CFG_HOPS_SIZE          12
#define RTR_ENTRY_CFG_DIST_SIZE          13
#define RTR_ENTRY_CFG_DIST_INTERVAL_SIZE 14
#define RTR_ENTRY_CFG_LIFE_SIZE          15
#define RTR_ENTRY_CFG_BUCKET_SIZE        16
#define RTR_ENTRY_CFG_SAMPLE_SIZE        17
#define RTR_ENTRY_CFG_HIST_FILTER        18

/*
 *  Exit Commands
 */

#define RTR_ENTRY_CFG_END                1
#define RTR_ENTRY_CFG_EXIT               2
