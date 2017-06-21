/* $Id: getnum.c,v 3.2 1995/11/17 18:42:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/getnum.c,v $
 *------------------------------------------------------------------
 * getnum.c
 *
 * October 1992, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Convert an ascii string to binary according to base.
 *------------------------------------------------------------------
 * $Log: getnum.c,v $
 * Revision 3.2  1995/11/17  18:42:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "proto.h"

/*
** Convert the ascii string pointed to by cptr to binary according to base.
** Result is placed in *longret.
** Return value is the number of characters processed.
** Maxchars defines the maximum number of characters to process.  If
** maxchars == 0, process until an invalid character occurs.
** Getnum exists for historical reasons.
*/
int
getnum(char *cptr, int base, long *longret)
     /*cptr : character buffer pointer */
     /*longret : for the result */
{
  return(getnnum(cptr, base, longret, 0));
}

int
getnnum(char *cptr, int base, long *longret, int maxchars)
     /*cptr : character buffer pointer */
     /*longret : for the result */
{
  char cval;
  unsigned int value = 0; /* init */
  int count = 0; /* init */

  /* bump past possible 0x or 0X on a hex number */
  if(base == 16 && *cptr == '0' &&
     (*(cptr+1) == 'x' || *(cptr+1) == 'X')) {
      cptr += 2;
      count += 2;
  }
  while(1) {
    cval = atoh(*cptr);
    if(cval < 0 || cval >= base) break;  /* invalid character encountered */
    value = (value * base) + cval;
    cptr++;
    count++;
    if(maxchars && count == maxchars) break;
  }
  *longret = value;  /* place result */
  return(count);
}
