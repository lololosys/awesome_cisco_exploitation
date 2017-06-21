/* $Id: atmsig_public.h,v 3.3.50.3 1996/09/06 16:40:12 jwjang Exp $
 * $Source: /release/112/cvs/Xsys/atm/atmsig_public.h,v $
 *------------------------------------------------------------------
 *
 * A T M S I G _ P U B L I C . H
 *
 * ATM Signalling public header file
 *
 * November 1993,  Percy P. Khabardar
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmsig_public.h,v $
 * Revision 3.3.50.3  1996/09/06  16:40:12  jwjang
 * CSCdi66728:  ATMSIG: SVCs are not torn-down after peer-switch reload
 * Branch: California_branch
 *
 * Revision 3.3.50.2  1996/05/24  23:32:47  achopra
 * CSCdi56238:  Linear search for glommable VCs in the vc HASH table has
 *         been changed to a binary search in a WAVL tree maintained per
 *         swidb, which is keyed on NSAPs, AAL5 encapsulation and
 *         protocol type if encapsulation is AAL5MUX.
 * Branch: California_branch
 *
 * Revision 3.3.50.1  1996/04/25  23:02:26  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.3.54.1  1996/04/02  07:56:43  awu
 * Branch: ATM_Cal_branch
 * add a function prototype.
 *
 * Revision 3.3  1995/11/22  20:44:36  ronnie
 * CSCdi44212:  Many fixes done at Telecom Finland
 *
 * Revision 3.2  1995/11/17  08:46:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/18  14:23:14  schrupp
 * CSCdi38181:  Add ATMSIG throttling and SSCOP changes
 *
 * Revision 2.1  1995/06/07  20:12:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ATMSIG_PUBLIC_H__
#define __ATMSIG_PUBLIC_H__

#define ATMSIG_SSCOP_RESET    0x80000001
#define ATMSIG_SSCOP_OUTPUT   0x80000002
#define ATMSIG_SSCOP_RELEASE  0x80000004
#define ATMSIG_SSCOP_ESTABLISH  0x80000008

extern pid_t atmsig_timer_pid;
extern void atmSig_hexout(uchar *, int,  char *);
extern void atmSig_parser_init(void);
extern void atmSig_exec_commands(parseinfo *);
extern void atmSig_interface_commands(parseinfo *);
extern void atmSig_clear_vc(parseinfo *);
extern boolean atmsig_interface_up(idbtype *);

#endif __ATMSIG_PUBLIC_H__
