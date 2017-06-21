/*
 *	P A R S E R _ D E F S _ S D L C . H
 *
 * $Id: parser_defs_sdlc.h,v 3.1 1996/02/01 04:58:17 hampton Exp $
 * $Source: /release/111/cvs/Xsys/ibm/parser_defs_sdlc.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_sdlc.h,v $
 * Revision 3.1  1996/02/01  04:58:17  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:52:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:31:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  Sdlc commands
 */
typedef enum _sdlc_parser_cmd_t {
    SDLC_CMD_ADDRESS = 0,
    SDLC_CMD_FRMR_DISABLE,
    SDLC_CMD_HOLDQ,
    SDLC_CMD_K,
    SDLC_CMD_N1,
    SDLC_CMD_N2,
    SDLC_CMD_POLL_LIMIT,
    SDLC_CMD_POLL_PAUSE,
    SDLC_CMD_POLL_WAIT,
    SDLC_CMD_T1,
    SDLC_CMD_SLOW_POLL,
    SDLC_CMD_NORMAL_TRANSMIT,
    SDLC_CMD_LINE_SPEED,
    SDLC_CMD_HDX,
    SDLC_CMD_CTS_DELAY,
    SDLC_CMD_RTS_TIMEOUT,
    SDLC_CMD_QLLC_PRTNR,
    SDLC_CMD_SIMULTANEOUS,
    SDLC_CMD_VIRTUAL_MULTIDROP,
    SDLC_CMD_ROLE,			/* SDLC role support */
    SDLC_CMD_RNR_LIMIT,                 /*SZ*/        
    SDLC_DLSW,				/* Attach SDLC to DLSW */
    SDLC_VMAC,				/* Virtual MAC address for IDB */
    SDLC_CMD_GROUP_POLL,		/* Group Poll support */
    SDLC_CFG_XID,			/* CLS xid */
    SDLC_PRTNR,				/* CLS remote partner's
					   address */
    SDLC_SAPS,				/* CLS destination and source
					   saps */
    SDLC_SDLCLF				/* Largest frame size for this
					   address */
} sdlc_parser_cmd_t;
 
/*
 * Sdllc commands
 *
 *  Start them after the sdlc commands just in case
 *  someone makes a typo
 */
#define SDLLC_TRADDR		30
#define SDLLC_XID		31
#define SDLLC_TRLF		32	/* ring side largest frame */
#define SDLLC_SDLCLF		33	/* sdlc size largest frame */
#define	SDLLC_PRTNR		34
#define	SDLLC_SAPS		35

/*
 * SDLC Exec command definitions
 */

#define EXEC_SDLC_TEST 1

#define SDLC_TEST_ITERATIONS  1
#define SDLC_TEST_CONTINUOUS  2
#define SDLC_TEST_STOP        3
