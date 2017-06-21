/*
 *	P A R S E R _ D E F S _ R I F . H
 *
 * $Id: parser_defs_rif.h,v 3.1 1996/02/01 04:58:24 hampton Exp $
 * $Source: /release/111/cvs/Xsys/if/parser_defs_rif.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_rif.h,v $
 * Revision 3.1  1996/02/01  04:58:24  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:52:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:31:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define RIF_ADDRESS                0
#define RIF_TIMEOUT                1
#define RIF_ALWAYS_FORWARD         2
#define RIF_VALIDATE               3
#define RIF_VALIDATE_ENABLE        4
#define RIF_VALIDATE_ENABLE_AGE    5

#define MULTIRING_ALL             0
#define MULTIRING_APOLLO          1
#define MULTIRING_APPLE           2
#define MULTIRING_CLNS            3
#define MULTIRING_DECNET          4
#define MULTIRING_IP              5
#define MULTIRING_NOVELL          6
#define MULTIRING_OTHER           7
#define MULTIRING_VINES           8
#define MULTIRING_XNS             9

#define EXPLORER_ALLROUTES        1
#define EXPLORER_SPANNING         2

