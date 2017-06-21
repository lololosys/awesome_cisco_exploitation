/* $Id: exec_debug_kerberos.h,v 3.2 1995/11/17 17:47:04 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/exec_debug_kerberos.h,v $
 *------------------------------------------------------------------
 * exec_debug_kerberos.h - Toggle Kerberos debug flag
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_kerberos.h,v $
 * Revision 3.2  1995/11/17  17:47:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug Kerberos
 *
 */
EOLS    (debug_kerberos_eol, debug_command, DEBUG_KERBEROS);
KEYWORD_DEBUG(debug_kerberos, debug_kerberos_eol, ALTERNATE,
              OBJ(pdb,1), os_debug_arr,
              "kerberos", "KERBEROS authentication and authorization", PRIV_OPR);

#undef  ALTERNATE
#define ALTERNATE       debug_kerberos

