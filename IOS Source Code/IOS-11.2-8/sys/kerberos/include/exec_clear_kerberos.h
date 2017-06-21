/* $Id: exec_clear_kerberos.h,v 3.2 1995/11/17 17:47:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/exec_clear_kerberos.h,v $
 *------------------------------------------------------------------
 * exec_clear_kerberos.h - Clear Kerberos user credentials
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_clear_kerberos.h,v $
 * Revision 3.2  1995/11/17  17:47:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../kerberos/csfhost/cisco/kparse.h"


/**************************************************
 *
 * clear kerberos creds
 * Removes Kerberos credentials from the current user's user struct.
 */

EOLS	(clear_kerberos_eol, kerberos_command, CLEAR_KERBEROS);
KEYWORD (clear_kerberos_creds, clear_kerberos_eol, ALTERNATE, "creds",
	 "Clear Kerberos Credentials", PRIV_USER);
KEYWORD	(clear_kerberos, clear_kerberos_creds, ALTERNATE, "kerberos",
	"Clear Kerberos Values", PRIV_USER);

#undef ALTERNATE
#define ALTERNATE clear_kerberos
