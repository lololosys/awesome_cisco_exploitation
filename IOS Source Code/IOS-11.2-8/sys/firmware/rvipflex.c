/* $Id: rvipflex.c,v 3.5 1996/03/04 07:12:05 kao Exp $
 * $Source: /release/111/cvs/Xsys/firmware/rvipflex.c,v $
 *------------------------------------------------------------------
 * rvipflex.c - RVIP VPLD data array
 *
 * Febuary 1995, Michael Beesley
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc. 
 *------------------------------------------------------------------
 * $Log: rvipflex.c,v $
 * Revision 3.5  1996/03/04  07:12:05  kao
 * CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
 * VIP; VPLD rev22 has problem with PMAC PCI parity error, rollback to rev
 * 21 now
 *
 * Revision 3.4  1996/02/18  19:40:37  kao
 * CSCdi45136:  EOIR Problems with VIP card
 * rvip needs updated vpld image (22)
 *
 * Revision 3.3  1995/11/28  03:20:51  getchell
 * CSCdi43583:  rvip needs updated vpld image (21)
 *
 * Revision 3.2  1995/11/17  09:11:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:34:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:58:11  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 * Revision 1.1.2.10  1995/11/08  01:12:37  getchell
 * CSCdi43583:  rvip needs updated vpld image (21)
 * Branch: VIP_branch
 *
 * Revision 1.1.2.9  1995/09/29  20:55:42  mbeesley
 * CSCdi41304:  Integrate particle manager
 * Branch: VIP_branch
 *
 * Revision 1.1.2.8  1995/09/12  22:08:50  getchell
 * CSCdi40224:  version 20 vpld file needed
 * Branch: VIP_branch
 *
 * Revision 1.1.2.7  1995/07/24  23:39:25  getchell
 * Sync VIP_branch between VIP_baseline_950523 and VIP_baseline_950720
 * Branch: VIP_branch
 *
 * Revision 1.1.2.6  1995/06/09  02:43:56  rmontino
 * CSCdi33861:  delete CIP hardware support
 * Branch: VIP_branch
 *
 * Revision 1.1.2.5  1995/05/03  19:58:31  mbeesley
 * Branch: VIP_branch
 * Fix type in comment.
 *
 * Revision 1.1.2.4  1995/04/18  23:35:53  mbeesley
 * Branch: VIP_branch
 * Fix typo in including Checkers rev 2 firmware.
 *
 * Revision 1.1.2.3  1995/04/14  17:55:01  mbeesley
 * Branch: VIP_branch
 * Check in checkers rev 2 VPLD program.
 *
 * Revision 1.1.2.2  1995/03/25  22:07:56  mbeesley
 * Branch: VIP_branch
 * Add flex program for Checkers version 1.0 hardware.
 *
 * Revision 1.1.2.1  1995/03/17  05:14:06  mbeesley
 * Branch: VIP_branch
 * Add initial RVIP VPLD files (actually these are CIP versions).
 *
 * Revision 2.1  1995/06/07  20:34:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * VPLD data array
 */

unsigned char vpld_data[] =     {

/*
 * Include Checkers Rev 2.2 VPLD program, presently at rev 21
 */
#include "../firmware/checkers2.ttf.rev21"

    0,0,0,0,0,0,0,0,0,0  	/* Add 10 extra clock cycles */
};

int vpld_data_size = sizeof(vpld_data);

/* end of file */



