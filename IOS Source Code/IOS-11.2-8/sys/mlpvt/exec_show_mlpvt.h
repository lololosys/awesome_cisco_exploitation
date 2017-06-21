/* $Id: exec_show_mlpvt.h,v 1.1.42.2 1996/05/18 22:34:44 perryl Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/exec_show_mlpvt.h,v $
 *------------------------------------------------------------------
 * exec_show_mlpvt.h -- Show commands for MLPVT
 *
 * January 1996, Ashwin Baindur
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_mlpvt.h,v $
 * Revision 1.1.42.2  1996/05/18  22:34:44  perryl
 * CSCdi57939:  MLPVT is a bad name for Stack Group Bidding Protocol
 * Branch: California_branch
 * Rename it to SGBP, Stack Group Bidding Protocol
 *
 * Revision 1.1.42.1  1996/04/27  06:34:17  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.1  1996/01/26  20:28:36  agh
 * Branch: Dial1_branch
 * Initial Code Commit for MLPVT.
 *
 * Revision 1.1  1996/01/26  19:22:30  abaindur
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __EXEC_SHOW_MLPVT_H__
#define __EXEC_SHOW_MLPVT_H__

/******************************************************************
 * "show sgbp"  : 
 * "show sgbp mqstate"  : For MLPVT mastership query state
 */

/*---------------------** show mlpvt commands **------------------------*/
EOLS    (show_mlpvt_eol, show_mlpvt, SHOW_MLPVT);

KEYWORD_ID(show_mlpvt_mqstate, show_mlpvt_eol, show_mlpvt_eol,
           OBJ(int,1), TRUE,
           "queries", "show SGBP mastership query state",
           PRIV_USER);

KEYWORD (show_mlpvt_kwd, show_mlpvt_mqstate, ALTERNATE,
         "sgbp", "SGBP group information", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_mlpvt_kwd

#endif /* __EXEC_SHOW_MLPVT_H__ */

