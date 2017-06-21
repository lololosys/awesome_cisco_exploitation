/* $Id: strtoul.c,v 1.1.4.1 1996/05/09 14:56:10 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_libc/strtoul.c,v $
 *------------------------------------------------------------------
 * strtoul.c
 *
 * ?
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strtoul.c,v $
 * Revision 1.1.4.1  1996/05/09  14:56:10  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:41  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:46  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:50  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  00:30:54  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:54:03  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef lint
static char sccsid[]="@(#)strtoul.c	1.1 6/24/93 ";
#endif
#include "types.h"
#include "ctype.h"

uInt32
strtoul(s,ptr,base)
char	*s;
char	**ptr;
Int32		base;
{
  uInt32 total = 0, tmp = 0;
  uInt32 digit;
  Int32 radix;
  char *start=s;
  Int32 did_conversion=0;

  if (s==NULL)
    {
      if (!ptr)
	*ptr = (char *)start;
      return 0L;
    }

  while (isspace(*s))
    s++;
  if (*s == '+')
    s++;
  radix = base;
  if (base==0 || base==16)
    {
      /*
       * try to infer radix from the string
       * (assume decimal).
       */
      if (*s=='0')
	{
	  radix = 8;	/* guess it's octal */
	  s++;		/* (but check for hex) */
	  if (*s=='X' || *s=='x')
	    {
	      s++;
	      radix = 16;
	    }
	}
    }
  if (radix==0)
    radix = 10;

  while ( digit = *s )
    {
      if (digit >= '0' && digit < ('0'+radix))
	digit -= '0';
      else
	if (radix > 10)
	  {
	    if (digit >= 'a' && digit < ('a'+radix))
	      digit = digit - 'a' + 10;
	    else if (digit >= 'A' && digit < ('A'+radix))
	      digit = digit - 'A' + 10;
	    else
	      break;
	  }
	else
	  break;
      did_conversion = 1;
      tmp = (total * radix) + digit;
      if (tmp < total)	/* check overflow */
	{
	  if (ptr != NULL)
	    *ptr = (char *)s;
	  return (0xffffffff);
	}
      total = tmp;
      s++;
    }
  if (ptr != NULL)
    *ptr = (did_conversion) ? (char *)s : start;
  return (total);

}
