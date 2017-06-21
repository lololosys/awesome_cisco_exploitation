/* $Id: rmon_sanitize.h,v 3.2 1996/01/18 01:14:17 jjohnson Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/rmon/rmon_sanitize.h,v $
 *------------------------------------------------------------------
 * rmon_sanitize.h: prototype for security scrub for captured packets
 *
 * January 1996, Jeffrey T. Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rmon_sanitize.h,v $
 * Revision 3.2  1996/01/18  01:14:17  jjohnson
 * CSCdi47080:  Enable limited RMON packet capture
 *
 * Revision 3.1  1996/01/04  20:13:57  jjohnson
 * Initial Placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * function to sanitize a captured packet by applying
 * a security posture to the packet such that sensitive
 * information is zeroed out of the packet
 */
extern void rmon_sanitize(paktype *pak);
