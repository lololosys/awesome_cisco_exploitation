/* $Id: getline.c,v 3.2 1995/11/17 18:42:01 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/getline.c,v $
 *------------------------------------------------------------------
 * getline.c
 *
 * November 1992, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Get a line from the standard input device.
 *------------------------------------------------------------------
 * $Log: getline.c,v $
 * Revision 3.2  1995/11/17  18:42:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "proto.h"

/*
** Get a line from the standard input device.
** Return or linefeed terminates the line.
** Note that there must be room in the buffer for a terminator.
** Return the number of characters entered except the terminator.
*/
int
getline(char *buffer, int bufsiz)
     /* buffer : character buffer pointer */
     /* bufsiz : character buffer size */
{
  char c, *bptr;
  int count;

  bufsiz -= 1;  /* make room for the terminator */
  bptr = buffer;
  count = 0;
  while(1) {
    c = getchar();
    switch(c) {
    case '\r':
    case '\n':
      goto terminate;
    case '\b':  /* backspace character */
    case '\177':  /* delete key */
      if(count) {  /* are there characters in the buffer? */
	puts("\b \b");  /* erase one */
	--count;
	--bptr;
      } else putchar('\007');  /* ring the bell */
      break;
    case '\003':  /* control C */
    case '\026':  /* control U */
      bptr = buffer;
      count = 0;
      goto terminate;  /* effectively kills the line */
    default:
      if(count < bufsiz) {  /* there is room in the buffer */
	++count;
	putchar(c);
	*bptr++ = c;
	break;
      } else {
	puts("\007\n*** line too large ***\n");
	goto terminate;
      }
    }
  }
terminate:
  *bptr = '\0';  /* terminate the line */
  putchar('\n');
  return(count);
}
