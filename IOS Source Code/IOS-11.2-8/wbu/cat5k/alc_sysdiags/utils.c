/* $Id: utils.c,v 1.1.4.1 1996/05/09 15:01:19 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/utils.c,v $
 *------------------------------------------------------------------
 * utils.c -- 
 *
 * ?
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: utils.c,v $
 * Revision 1.1.4.1  1996/05/09  15:01:19  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:18  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:43  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:28  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:54:14  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:47  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************/
/*                                                                            */
/* DIAGNOSTICS DRIVER                                                         */
/*                                                                            */
/* This file contains procedures related to "Utility Functions" that are      */
/* included as part of Diagnostics Driver.                                    */
/*                                                                            */
/* NOTE TO THE REVIEWER:                                                      */
/* --------------------                                                       */
/* You may find that Comments & Remarks are kept to the minimum extent, which */
/* eventually would be updated in due course of time.  The author believes in */
/* "Self-documentation" by way of meaningful names for procedures, variables  */
/* etc. and "Hierarchical programming" style which would work out better in   */
/* terms of time & productivity.  In the meantime, if you encounter any       */
/* problems in going thru the code or have any comments/suggestions, do let   */
/* the author know about it.                                                  */
/*                                                                            */
/******************************************************************************/

#include	"ctype.h"
#include	"types.h"
#include	"constant.h"
#include	"globdef.h"

	enum	STRCMP_RC {
	STRCMP_OK = 0,
	STRCMP_FAIL = 1
	};

int strcmp_nocase(
	char		*str1,
	char		*str2)
{
	char		c1, c2;

	while (*str1 != '\0' && *str2 != '\0') {
	   c1 = *str1++; c2 = *str2++;
	   if (isalpha(c1) && isupper(c1))
	      c1 = tolower(c1);
	   if (isalpha(c2) && isupper(c2))
	      c2 = tolower(c2);
	   if (c1 != c2)
	      return(STRCMP_FAIL);
	}

	if (*str1 != '\0' || *str2 != '\0')
	   return(STRCMP_FAIL);

	return(STRCMP_OK);
}

int strcmp_v(
	char		*str1,
	char		*str2)
{
	char		c1, c2;

	while (*str1 != '\0' && *str2 != '\0') {
	   c1 = *str1++; c2 = *str2++;
	   if (isalpha(c1) && isupper(c1))
	      c1 = tolower(c1);
	   if (isalpha(c2) && isupper(c2))
	      c2 = tolower(c2);
	   if (c1 != c2)
	      return(STRCMP_FAIL);
	}

	if (*str1 != '\0')
	   return(STRCMP_FAIL);

	return(STRCMP_OK);
}

int strcmp_rv(
	char		*str1,
	char		*str2,
	char		str_buf[])
{
	register int	i, index;
	char		c1, c2;

	for (index = 0, str2++; *str2 != '\0'; index++, str2++) {
	   for (i = 0; *(str1+i) != '\0'; i++) {
	      c1 = *(str1+i); c2 = *str2;
	      if (isalpha(c1) && isupper(c1))
	         c1 = tolower(c1);
	      if (isalpha(c2) && isupper(c2))
	         c2 = tolower(c2);
	      if (c1 != c2)
	         break;
	      str_buf[i] = *str2++;
	   }
	   if (*(str1+i) == '\0') {
	      while (*str2 != '/')
	         str_buf[i++] = *str2++;
	      str_buf[i] = '\0';
	      return(index); 
	   }
	   while (*str2 != '/')
	      str2++;
	}

	return(-1);
}

int strncmp_v(
	char		*str1,
	char		*str2,
	int		len)
{
	char		c1, c2;

	while (*str1 != '\0' && *str2 != '\0') {
	   c1 = *str1++; c2 = *str2++;
	   if (isalpha(c1) && isupper(c1))
	      c1 = tolower(c1);
	   if (isalpha(c2) && isupper(c2))
	      c2 = tolower(c2);
	   if (c1 != c2)
	      return(STRCMP_FAIL);
	   if (len > 0)
	      len--;
	}

	if (*str1 != '\0' || len > 0)
	   return(STRCMP_FAIL);

	return(STRCMP_OK);
}

int sscanf_x(
	char		*str,
	char		*fmt,
	uInt32		*datap)
{
	register int	i, x_flag;

	if (!strcmp(fmt, fmt_xInt32) ||
	    !strcmp(fmt, fmt_xInt16) ||
	    !strcmp(fmt, fmt_xInt08)) {
	   x_flag = TRUE;
	   if (*str == '0' && (*(str+1) == 'x' || *(str+1) == 'X'))
	      str += 2;
	}
	else
	   x_flag = FALSE;

	if (*str == '\0')
	   return(0);

	for (i = 0; i < 8; i++) {

	   if (*(str+i) == '\0')
	      break;

	   if (x_flag) {
	      if (!isxdigit(*(str+i)))
	         break;
	   }
	   else {
	      if (!isdigit(*(str+i)))
	         break;
	   }

	   if ((!strcmp(fmt, fmt_xInt16) || !strcmp(fmt, fmt_uInt16)) && i == 4)
	      break;

	   if ((!strcmp(fmt, fmt_xInt08) || !strcmp(fmt, fmt_uInt08)) && i == 2)
	      break;
	}
	if (*(str+i) != '\0')
	   return(0);

	while (*str == '0')
	   str++;
	if (*str == '\0') {
	   *((uInt32 *) datap) = 0;
	   return(1);
	}

	if (x_flag)
	   return(sscanf(str, fmt_xInt32, datap));
	else
	   return(sscanf(str, fmt_uInt32, datap));
}

