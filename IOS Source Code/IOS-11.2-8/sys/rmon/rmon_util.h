/* $Id: rmon_util.h,v 3.2 1996/02/09 07:57:22 jjohnson Exp $
 * $Source: /release/111/cvs/Xsys/rmon/rmon_util.h,v $
 *------------------------------------------------------------------
 * rmon_util.h:  Prototypes for general purpose rmon utilities
 *
 * February 1996, Jeffrey T. Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rmon_util.h,v $
 * Revision 3.2  1996/02/09  07:57:22  jjohnson
 * CSCdi48524:  Need mechanism to display RMON tables
 * parse chains are in place.  checkpoint work so that anke can take over.
 *
 * Revision 3.1  1996/02/09  06:12:08  jjohnson
 * Initial placeholder for rmon user interface enhancements
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __RMON_UTIL_H__
#define __RMON_UTIL_H__

extern const char *rmon_status_string (long entrystatus);

#endif
