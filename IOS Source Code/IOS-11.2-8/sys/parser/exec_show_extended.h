/* $Id: exec_show_extended.h,v 3.3 1996/01/16 21:18:13 widmer Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/parser/exec_show_extended.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ E X T E N D E D . H
 *
 * July 1994, Stig Thormodsrud
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_extended.h,v $
 * Revision 3.3  1996/01/16  21:18:13  widmer
 * CSCdi33940:  show extended command doesnt work
 * Add keyword_trans macro that doesn't accept the keyword
 *  unless commands have been added to the link point.
 *
 * Revision 3.2  1995/11/17  18:47:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:23:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

KEYWORD_TRANS(show_extended, show_extended_extend_here, no_alt, ALTERNATE,
	      "extended", "Extended Interface Information",
	      PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_extended
