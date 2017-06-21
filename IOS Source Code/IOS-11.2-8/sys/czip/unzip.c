/* $Id: unzip.c,v 3.2 1995/11/17 08:58:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/czip/unzip.c,v $
 *------------------------------------------------------------------
 * unzip.c - zip uncompress
 *
 * December 1994, Michael Beesley
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: unzip.c,v $
 * Revision 3.2  1995/11/17  08:58:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:15:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:22:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* The code in this file is derived from the file funzip.c written
 * and put in the public domain by Mark Adler.
 *
 * It has been modified for use as a self extractor for zipped
 * router images by Cisco.
 */

#include "czip.h"

/* Globals */

/*
 * unzip :
 * Unzip input to output
 */
int unzip (void)
{
    int res;

    /* Decompress */

    if (method == CZIP_DEFLATED)  {
	res = inflate();
	if (res == 3) {
	    error("out of memory\n");
	} else if (res != 0) {
	    error("invalid compressed data--format violated\n");
	}
    } else {
	error("unsupported compression format\n");
    }
	
    return(exit_code);
}

/* end of file */
