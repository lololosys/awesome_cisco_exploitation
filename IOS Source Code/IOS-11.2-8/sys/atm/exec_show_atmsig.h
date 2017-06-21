/* $Id: exec_show_atmsig.h,v 3.2 1995/11/17 08:46:47 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atm/exec_show_atmsig.h,v $
 *------------------------------------------------------------------
 * parser/exec_show_atmsig.h: SHOW command parsing 
 *
 * November, 1993,  Percy P. Khabardar
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_atmsig.h,v $
 * Revision 3.2  1995/11/17  08:46:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  09:35:16  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.1  1995/06/07  22:22:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*****************************************************************************
 * show atmsig summary
 */
EOLS    (show_atmsig_summary_eol, atmsig_show_commands, SHOW_ATMSIG_SUMMARY);
KEYWORD (show_atmsig_summary, show_atmsig_summary_eol, show_atmsig_summary_eol,
            "summary", "ATM Signalling summary", PRIV_USER);

/*****************************************************************************
 * show atmsig vc [<num>]
 */
EOLS    (show_atmsig_vc_eol, atmsig_show_commands, SHOW_ATMSIG_VC);
/* <vc no.> */
NUMBER  (show_atmsig_vcno, show_atmsig_vc_eol, show_atmsig_vc_eol,
        OBJ(int,1), 1, 1024, "ATM Sig VC number");
KEYWORD (show_atmsig_vc, show_atmsig_vcno, show_atmsig_summary,
            "vc", "ATM Signalling VC Information", PRIV_USER);

/*****************************************************************************
 * show atmsig
 */
KEYWORD (show_atmsig, show_atmsig_vc, ALTERNATE,
         "atmsig", "ATM Sig Information", PRIV_USER);


#undef  ALTERNATE
#define ALTERNATE       show_atmsig
