/* $Id: parser_defs_bsend_tst.h,v 3.1.40.1 1996/05/17 10:41:13 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/bstun/parser_defs_bsend_tst.h,v $
 *------------------------------------------------------------------
 * BSEND configuration command constants for BSC test
 *
 * Oct 95. Saritha Roach
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_bsend_tst.h,v $
 * Revision 3.1.40.1  1996/05/17  10:41:13  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.26.1  1996/04/11  08:19:54  jbalestr
 * CSCdi51807:  need a test command to transmit raw bisync frames
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/02/01  04:57:16  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.1  1995/11/09  12:54:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/10  07:42:07  sroach
 * CSCdi41879:  Add BSC test command support
 *
 * Revision 2.1  1995/10/10  05:35:26  sroach
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  BSEND commands.
 */

#define BSEND_CMD_ADD             0
#define BSEND_CMD_DELETE          1
#define BSEND_CMD_ACTIVE          2
#define BSEND_CMD_INACTIVE        3
#define BSEND_CMD_DATA            4
#define BSEND_CMD_RAW             5
#define BSEND_CMD_SIZE            6

