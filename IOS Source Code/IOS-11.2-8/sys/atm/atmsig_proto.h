/* $Id: atmsig_proto.h,v 3.1.64.1 1996/09/06 16:40:10 jwjang Exp $
 * $Source: /release/112/cvs/Xsys/atm/atmsig_proto.h,v $
 *------------------------------------------------------------------
 * atmsig_proto.h
 *
 * ATM signalling stack proto types file.
 *
 * Oct 1995, Percy/Phil/Pandian
 *
 * Copyright (c) 1996-1998 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmsig_proto.h,v $
 * Revision 3.1.64.1  1996/09/06  16:40:10  jwjang
 * CSCdi66728:  ATMSIG: SVCs are not torn-down after peer-switch reload
 * Branch: California_branch
 *
 * Revision 3.1  1995/11/09  11:00:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/19  23:19:21  gpandian
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern boolean atmSig_find_trParamQos(vc_info_t *, atm_trParamQos_t *,
                                     boolean);
