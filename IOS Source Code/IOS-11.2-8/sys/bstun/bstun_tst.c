/* $Id: bstun_tst.c,v 3.8.8.2 1996/05/17 10:41:06 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bstun_tst.c,v $
 *------------------------------------------------------------------
 * Test version of bstun.c BSTUN management/encapsulation routines
 *
 * Oct 95. Saritha Roach
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bstun_tst.c,v $
 * Revision 3.8.8.2  1996/05/17  10:41:06  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.4.3  1996/04/11  08:19:50  jbalestr
 * CSCdi51807:  need a test command to transmit raw bisync frames
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.4.2  1996/04/03  13:50:45  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.4.1  1996/03/17  17:36:00  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.8.8.1  1996/03/18  19:01:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.4  1996/03/16  06:29:09  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.2.3  1996/03/13  01:12:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.2.2  1996/03/07  08:39:16  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.2.1  1996/02/20  00:19:54  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/29  20:34:49  jbalestr
 * CSCdi48520:  BSTUN route command parses DIRECT
 * stops invalid configuration
 *
 * Revision 3.7  1996/02/28  09:32:27  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.6  1996/02/08  18:01:42  ahh
 * CSCdi48380:  TCP: TCP Driver close callback needs more context
 * Have some t with your cookie.
 *
 * Revision 3.5  1996/02/07  16:10:20  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/02/01  06:00:17  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1996/01/16  03:37:14  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.2  1995/12/13  03:22:45  jbalestr
 * CSCdi45429:  bsc lack poller stops transmiting after shutdown/ no shut
 *
 * Revision 3.1  1995/11/09  11:04:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/20  06:21:15  sroach
 * CSCdi42500:  BSC test command needs to log messages to an internal
 * buffer
 *
 * Revision 2.2  1995/10/10  07:56:18  sroach
 * CSCdi41879:  Add BSC test command support
 *
 * Revision 2.1  1995/10/10  05:43:03  sroach
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
