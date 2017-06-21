/* $Id: dirresp_private.h,v 3.1.18.2 1996/08/15 06:49:49 raj Exp $
 * $Source: /release/112/cvs/Xsys/servers/dirresp_private.h,v $
 *------------------------------------------------------------------
 * Director-Responder private definitions
 *
 * Feburary 1996, Paul Traina
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dirresp_private.h,v $
 * Revision 3.1.18.2  1996/08/15  06:49:49  raj
 * CSCdi61197:  Part of  CSCdi49395 patches missing from 11.2
 * Branch: California_branch
 * Update Director Responder Agent to the latest version.
 *
 * Revision 3.1.18.1  1996/07/16  08:01:27  raj
 * CSCdi61197:  Part of  CSCdi49395 patches missing from 11.2
 * Branch: California_branch
 * Inserted patches which were missing and update dirresp code as well.
 *
 * Revision 3.1  1996/02/20  22:46:01  pst
 * Initial incorporation of distributed directory agent support
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Parser information
 *
 * ip drp
 */
enum drp_cmd_enum { 
    DRP_CMD_ACCESS,
    DRP_CMD_AUTH_KEYCHAIN,
    DRP_CMD_ENABLE
};

/*
 * show ip drp
 */
enum drp_show_enum {
    DRP_SHOW_STAT
};

/*
 * Shared functions
 */
extern void dirresp_parser_init(void);

extern boolean dirresp_debug;
