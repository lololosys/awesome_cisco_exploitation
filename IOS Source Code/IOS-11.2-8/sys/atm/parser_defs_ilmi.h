/* $Id: parser_defs_ilmi.h,v 3.1.38.1 1996/04/25 23:02:57 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/atm/parser_defs_ilmi.h,v $
 *------------------------------------------------------------------
 * Parser Definitions for ILMI.
 *
 * October 1995, Kartik Chandrasekhar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_ilmi.h,v $
 * Revision 3.1.38.1  1996/04/25  23:02:57  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.42.1  1996/03/30  03:35:15  rzagst
 * ATM Modularity: Clean up and modularize by platform the ATM parser
 * commands.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.1  1996/02/01  04:57:12  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.1  1995/11/09  12:55:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/17  19:13:11  kartik
 * Placeholder for Rhino Branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#define ILMI_MAX_TIMER 4294967		/* 4294967 secs = 49.7 days */

#define ILMI_IF_CONFIG_ILMI_ENABLE             1
#define ILMI_IF_CONFIG_ADDRESS_REG             ILMI_IF_CONFIG_ILMI_ENABLE+1
#define ILMI_IF_CONFIG_KEEPALIVE               ILMI_IF_CONFIG_ADDRESS_REG+1
#define ILMI_IF_CONFIG_ESI                     ILMI_IF_CONFIG_KEEPALIVE+1
#define ILMI_SHOW_ATM_IF_ILMI                  ILMI_IF_CONFIG_ESI+1
#define ILMI_IF_DISABLE_ENTERPRISE_TRAPS       ILMI_SHOW_ATM_IF_ILMI+1
