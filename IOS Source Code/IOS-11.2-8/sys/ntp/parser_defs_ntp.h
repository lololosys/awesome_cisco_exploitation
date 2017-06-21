/* $Id: parser_defs_ntp.h,v 3.2.62.2 1996/08/19 18:55:29 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/parser_defs_ntp.h,v $
 *
 *	P A R S E R _ D E F S _ N T P . H
 *
 * 
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_ntp.h,v $
 * Revision 3.2.62.2  1996/08/19  18:55:29  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.2.62.1  1996/04/16  19:03:34  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:51:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:41:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:46:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Global NTP configuration commands */

typedef enum ntp_config_cmds {
    NTP_TRUSTED_KEY_CMD,
    NTP_AUTH_KEY_CMD,
    NTP_AUTHENTICATE_CMD,
    NTP_SOURCE_CMD,
    NTP_MASTER_CMD,
    NTP_CLOCK_PERIOD_CMD,
    NTP_PEER_CMD,
    NTP_BROAD_DELAY_CMD,
    NTP_MAXASS_CMD,
    NTP_UPDATE_CALENDAR_CMD,
    NTP_HIGH_ACC_CMD
} ntp_config_cmd;

/* NTP interface configuration commands */

typedef enum ntp_interface_config_cmd_ {
    NTP_BROAD_IF_CMD,
    NTP_DISABLE_IF_CMD,
    NTP_ENABLE_IF_CMD,
    NTP_BROAD_CLIENT_IF_CMD
} ntp_interface_config_cmd;


/* NTP line configuration commands */

typedef enum ntp_line_config_cmd_ {
    NTP_REFCLOCK_LINE_CMD,
    NTP_PPS_DIS_LINE_CMD
} ntp_line_config_cmd;
