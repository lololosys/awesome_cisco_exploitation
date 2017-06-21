/* $Id: parser_defs_dlcsw.h,v 3.1 1996/02/01 04:57:23 hampton Exp $
 * $Source: /release/111/cvs/Xsys/cls/parser_defs_dlcsw.h,v $
 *------------------------------------------------------------------
 * DLC-SW Parser Definitions
 *
 * September 1994, Meng Lyu
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: parser_defs_dlcsw.h,v $
 * Revision 3.1  1996/02/01  04:57:23  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:50:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:20:35  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:29:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	PCDLU_LLC	1		/* llc2 protocol on Ethernet|Token Ring */
#define	PCDLU_SDLC	2		/* SDLC protocol			*/
#define	PCDLU_FR	3		/* llc2 on Frame Relay Link		*/

#define PCDLU_OPTS_PFID2 	1	/* for SNA Peripheral FID2		*/
#define PCDLU_OPTS_AFID2 	2	/* for APPN FID2			*/
#define PCDLU_OPTS_FID4 	3	/* for SNA Subarea FID2			*/

#define PCDLU_ROUTE		10
#define PCDLU_BACKUP    11
#define PCDLU_BAN    	12
#define PCDLU_BAN_SDLC  13
