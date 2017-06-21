/* $Id: rmon_util.c,v 3.2 1996/02/09 07:57:22 jjohnson Exp $
 * $Source: /release/111/cvs/Xsys/rmon/rmon_util.c,v $
 *------------------------------------------------------------------
 * rmon_util.c:  General purpose rmon utilities
 *
 * February 1996, Jeffrey T. Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rmon_util.c,v $
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

#include "master.h"
#include "sr_rmonmib.h"

const char *
rmon_status_string (long entrystatus)
{
    switch (entrystatus) {
    case ES_UNINITIALIZED:
	return "uninitialized";
    case ES_VALID:
	return "active";
    case ES_CREATEREQUEST:
    case ES_UNDERCREATION:
	return "under creation";
    case ES_INVALID:
	return "pending deletion";
    default:
	return "unknown";
    }
}
