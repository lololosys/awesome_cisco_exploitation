/* $Id: kauth.h,v 3.3 1995/12/24 09:35:20 che Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/kerberos/csfhost/cisco/kauth.h,v $
 *------------------------------------------------------------------
 * kauth.h - AAA Kerberos definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: kauth.h,v $
 * Revision 3.3  1995/12/24  09:35:20  che
 * CSCdi46019:  Store Kerberos credentials in Kerberos library cred cache
 * Use Kerberos library credentials cache structures and functions to
 * manipulate user credentials cache.
 *
 * Revision 3.2  1995/11/17  17:45:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define CSFC5_KINIT_DEFAULT_OPTIONS	0
#define CSFC5_KINIT_DEFAULT_LIFE	60*60*8 /* 8 hours */

extern char *krb_enable_instance_name; 
extern int krb_port;
