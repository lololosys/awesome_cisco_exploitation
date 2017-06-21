/* $Id: parser_defs_modem_mgmt.h,v 3.1.68.2 1996/09/09 20:56:55 jchlin Exp $
 * $Source:
 *------------------------------------------------------------------
 * A dummy file for brasil branch.
 *      P A R S E R _ D E F S _ M O D E M _ M G M T . H
 *
 * November, 1995 - Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: parser_defs_modem_mgmt.h,v $
 * Revision 3.1.68.2  1996/09/09  20:56:55  jchlin
 * CSCdi66525:  ct1/rbs cannot busy back the CO trunk when theres no free
 * modem
 * Branch: California_branch
 * o       The bugfix will busy dsx0s back to CO thru RBS when there's
 *         no free modem to accept incoming calls.
 * o       The modem pool is changed to have multiple pools for different
 *         types of modems.
 * o       Add hidden debug commands.
 *
 * Revision 3.1.68.1  1996/06/16  21:16:44  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/06  19:12:26  jchlin
 * Placeholders for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __PARSER_DEFS_MODEM_MGMT_H__
#define __PARSER_DEFS_MODEM_MGMT_H__

extern void modem_mgmt_parser_init(void);

/* Modem management keyword */
#define MODEM_MGMT_KEYWORD "modem-mgmt"

/* Global Modem Management configuration commands */
enum {MODEM_MGMT_MODEM_POLL_PAUSE, MODEM_MGMT_MODEM_RETRY_COUNT,
      MODEM_MGMT_MODEM_RESPONSE_WAIT, MODEM_MGMT_AT_MODE_PERMIT};

/* Modem Management Clear commands */
enum {MODEM_MGMT_CLEAR_AT_MODE};

/* Modem Management Show commands */
enum {MODEM_MGMT_SHOW_CSM_MODEM_INFO, MODEM_MGMT_SHOW_CSM_LIST};

/* Modem Management Exec commands */
enum {MODEM_MGMT_EXEC_CSM_MODE};
enum {
    CSM_EXEC_ON_HOOK, CSM_EXEC_OUTBOUND_CALL, CSM_EXEC_INBOUND_CALL,
    CSM_EXEC_ISDN_IDLE, CSM_EXEC_BUSYOUT, CSM_EXEC_NO_BUSYOUT, 
    CSM_EXEC_RING_DELAY, CSM_EXEC_RINGING, CSM_EXEC_ENABLE_ROUND_ROBIN,
    CSM_EXEC_DISABLE_ROUND_ROBIN, CSM_EXEC_NO_DEBUG_RBS, CSM_EXEC_DEBUG_RBS
};

#define MODEM_MGMT_MIN_MODEM 0
#define MODEM_MGMT_MAX_MODEM 47
#define MODEM_MGMT_TOTAL_MODEMS (MODEM_MGMT_MAX_MODEM + 1)

#define MODEM_MGMT_MODEM_POLL_PAUSE_DEFAULT (2000)
#define MODEM_MGMT_MODEM_RETRY_COUNT_DEFAULT (3)
#define MODEM_MGMT_MODEM_RESPONSE_WAIT_DEFAULT (100)

#endif /* __PARSER_DEFS_MODEM_MGMT_H__ */

