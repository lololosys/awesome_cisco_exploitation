/* $Id: puts.c,v 3.2 1995/11/17 18:42:53 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/puts.c,v $
 *------------------------------------------------------------------
 * puts.c
 *
 * January 1993, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Put a string on the stdout device.
 *------------------------------------------------------------------
 * $Log: puts.c,v $
 * Revision 3.2  1995/11/17  18:42:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "proto.h"

/*
** Print out the null terminated string.
*/
void
puts(char *cptr)
{
  char c;

  while((c = *cptr++) != '\0') putchar(c);
}
