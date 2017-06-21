/* $Id: peer_util.c,v 3.1 1995/11/09 11:45:51 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/peer_util.c,v $
 *------------------------------------------------------------------
 * peer_util.c - common utility routines for dlsw, rsrb, and stun 
 *               peers
 *
 * May 1995, Richard Livingston
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: peer_util.c,v $
 * Revision 3.1  1995/11/09  11:45:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/07  05:11:00  richl
 * CSCdi38315:  makefile rework
 *              add new files for libraries
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"

char *priority_peername (
    int priority_num)
{
    switch (priority_num) {
      case 0:
	return("   High");
	break;
      case 1:
        return(" Medium");
	break;
      case 2:
	return(" Normal");
	break;
      case 3:
	return("    Low");
	break;
      default:
	return("Unknown");
	break;
    }
}

char *tcpuser_open_fail_str (int stat)
{
    switch (stat) {
      case NO_MEMORY:	return("not enough memory");
      case REJECTED:	return("rejected");
      case FOREIGN_RESET:
			return("foreign reset");
      case LOCAL_RESET:	return("local reset");
      case TIMEDOUT:	return("timed out");
      case ABORTED:	return("aborted");
      case UNREACHABLE:	return("unreachable");
      case OPTSTOOBIG:	return("options too big");
      default:		return("internal error");
    }
}

