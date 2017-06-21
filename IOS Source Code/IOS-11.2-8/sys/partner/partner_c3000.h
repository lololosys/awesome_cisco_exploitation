/* $Id: partner_c3000.h,v 3.1 1996/03/09 00:12:19 ljiang Exp $
 * $Source: /release/111/cvs/Xsys/partner/partner_c3000.h,v $
 *------------------------------------------------------------------
 * partner_c3000.h -- c3000 header
 *
 * Dec 1995, Li Jiang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: partner_c3000.h,v $
 * Revision 3.1  1996/03/09  00:12:19  ljiang
 * CSCdi50986:  Add mailbox support for  Microcom Terminator(c2511
 * derivative)
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _partner_c3000_h_
#define _partner_c3000_h_

void microcom_subsys_init(subsystype *subsys);
void chipcom_subsys_init(subsystype *subsys);

#endif
