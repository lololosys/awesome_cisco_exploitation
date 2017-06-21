/* $Id: mlpvt_chain.h,v 1.1.42.1 1996/04/27 06:34:25 syiu Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/mlpvt_chain.h,v $
 *------------------------------------------------------------------
 * mlpvt_chain.h -- MLPVT parser support
 *
 * January 1996, Ashwin Baindur
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mlpvt_chain.h,v $
 * Revision 1.1.42.1  1996/04/27  06:34:25  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.1  1996/01/26  20:28:39  agh
 * Branch: Dial1_branch
 * Initial Code Commit for MLPVT.
 *
 * Revision 1.1  1996/01/26  19:22:36  abaindur
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MLPVT_CHAIN_H__
#define __MLPVT_CHAIN_H__

void mlpvt_parser_init(void);
void mlpvtgroup_command(parseinfo *);

#endif
