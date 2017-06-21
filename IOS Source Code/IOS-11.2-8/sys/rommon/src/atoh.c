/* $Id: atoh.c,v 3.2 1995/11/17 18:40:51 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/atoh.c,v $
 *------------------------------------------------------------------
 * atoh.c
 *
 * November 1992, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * ASCII to hex routine.
 *------------------------------------------------------------------
 * $Log: atoh.c,v $
 * Revision 3.2  1995/11/17  18:40:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:04:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:36:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "proto.h"

/*
** Return the value for the ascii hex character or -1 if invalid.
*/
int
atoh(char c)
{
  if(c >= '0' && c <= '9') return(c - '0');
  if(c >= 'A' && c <= 'F') return(c - ('A' - 10));
  if(c >= 'a' && c <= 'f') return(c - ('a' - 10));
  return(-1);
}
