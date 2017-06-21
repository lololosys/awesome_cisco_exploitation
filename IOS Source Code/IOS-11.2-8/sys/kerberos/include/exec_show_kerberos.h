/* $Id: exec_show_kerberos.h,v 3.2 1995/11/17 17:47:05 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/exec_show_kerberos.h,v $
 *------------------------------------------------------------------
 * exec_show_kerberos.h - Show Kerberos user credentials
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_kerberos.h,v $
 * Revision 3.2  1995/11/17  17:47:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../kerberos/csfhost/cisco/kparse.h"


/**************************************************
 *
 * show kerberos creds
 * Displays the current user's Kerberos credentials.
 */

EOLS	(show_kerberos_eol, kerberos_command, SHOW_KERBEROS);
KEYWORD (show_kerberos_creds, show_kerberos_eol, ALTERNATE, "creds",
	 "Show Kerberos Credentials", PRIV_USER);

KEYWORD	(show_kerberos, show_kerberos_creds, ALTERNATE, "kerberos",
	"Show Kerberos Values", PRIV_USER);

#undef ALTERNATE
#define ALTERNATE show_kerberos
