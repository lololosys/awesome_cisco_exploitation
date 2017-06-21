/* $Id: parser_defs_modem.h,v 3.1.64.3 1996/09/09 20:56:54 jchlin Exp $
 * $Source: /release/112/cvs/Xsys/les/parser_defs_modem.h,v $ 
 *------------------------------------------------------------------
 *	P A R S E R _ D E F S _ M O D E M _ M G M T . H
 * 
 * December, 1995 - Viet Nguyen
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: parser_defs_modem.h,v $
 * Revision 3.1.64.3  1996/09/09  20:56:54  jchlin
 * CSCdi66525:  ct1/rbs cannot busy back the CO trunk when theres no free
 * modem
 * Branch: California_branch
 * o       The bugfix will busy dsx0s back to CO thru RBS when there's
 *         no free modem to accept incoming calls.
 * o       The modem pool is changed to have multiple pools for different
 *         types of modems.
 * o       Add hidden debug commands.
 *
 * Revision 3.1.64.2  1996/09/02  08:37:10  styang
 * CSCdi61183:  Cannot tell hardware revision of cards from command line
 * Branch: California_branch
 * To show hardware version info for the modem card and T1/E1 card.
 *
 * Revision 3.1.64.1  1996/06/16  21:16:42  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/21  23:39:43  vnguyen
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __PARSER_DEFS_MODEM_H__
#define __PARSER_DEFS_MODEM_H__

extern void modem_parser_init(void);

/* Modem Clear commands */
enum {
      CLEAR_MODEM,
      CLEAR_MODEM_GROUP,
      CLEAR_MODEM_HW,
      CLEAR_MODEM_AT_MODE,
      CLEAR_MODEM_COUNTERS
     };

/* Modem Show commands */
enum {
      SHOW_MODEM,
      SHOW_MODEM_GROUP,
      SHOW_MODEM_STATS,
      SHOW_MODEM_CSM,
      SHOW_MODEM_LOG,
      SHOW_MODEM_VERSION,
      SHOW_MODEM_SUMMARY,
      SHOW_MODEM_AT_MODE
     };

/* Modem Debug commands */
enum {
      DEBUG_MODEM,
      DEBUG_MODEM_GROUP,
     };
#define DEBUG_TRACE_NORM	0x01
#define DEBUG_TRACE_ABNORM	0x02
#define DEBUG_TRACE_ALL		0x04

/* Modem Managment commands */
enum {
      MODEM_BACK2BACK_TEST = 1,
      MODEM_AT_MODE,

      MODEM_STARTUP_TEST,
      MODEM_RECOVERY_TIME,
      MODEM_BUFFER_SIZE,
      MODEM_POLL_RETRY,
      MODEM_POLL_TIME,

      MODEM_SHUTDOWN,
      MODEM_BAD,
      MODEM_HOLD_RESET, 
      MODEM_BUSYOUT, 
      MODEM_STATUS_POLL,
      MODEM_AT_MODE_PERMIT, 
      MODEM_A_LAW, 
      MODEM_FAST_ANSWER,
      MODEM_BUSYOUT_RBS_DSX0,

      COPY_FLASH_MODEM,
      COPY_TFTP_MODEM,
      COPY_RCP_MODEM,
     };

#define MIN_WDT_VALUE             1
#define MAX_WDT_VALUE            10
#define MIN_POLL_TIME             2
#define MAX_POLL_TIME           120
#define MIN_POLL_RETRY            0
#define MAX_POLL_RETRY           10
#define MIN_MODEM_BUFFER_SIZE    50
#define MAX_MODEM_BUFFER_SIZE   500
#define MIN_FREE_MODEM_THRESHOLD  0
#define MAX_FREE_MODEM_THRESHOLD  64

#endif /* __PARSER_DEFS_MODEM_H__ */
