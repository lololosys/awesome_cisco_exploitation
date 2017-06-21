/* $Id: binary.c,v 3.2 1995/11/17 18:48:58 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/utils/hxldsrc/binary.c,v $
 *------------------------------------------------------------------
 * binary.c
 *
 * June 1993, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * A binary data download format.
 *------------------------------------------------------------------
 * $Log: binary.c,v $
 * Revision 3.2  1995/11/17  18:48:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:10:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:42:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** Binary download data format.
** Everything is in binary for maximum efficiency.
** Lines are addressed and checksummed, unlike most binary formats.
** Two options to this format are available:
**   Format
**     B - addressed download - each line is addressed for the target
**     C - coff file download - send entire coff file a.out header and all
**                            - lines are not addressed
*/
#include <stdio.h>

extern char format;
extern FILE *outfile;
extern unsigned packnum;
extern unsigned long loadaddr;

binaryout(size,ptr)
    register char *ptr;
{
    register i, checksum;
    register unsigned char c;

    packnum++;
    if(size == 0) {  /* this is the end */
	/* terminator has 0 address length and 0 record length */
	fputs("\177\0\0\0\0",outfile);
	return;
    } else if(packnum == 1) {  /* this is the first record */
	/* first record consists of at least 4 occurrances of <rub><null> */
	fputs("\177\0\177\0\177\0\177\0\177\0\177\0",outfile);
	fputc(format,outfile);   /* followed by the file type */
	fputc(~format,outfile);  /* and checksum */
    }
    checksum = size;
    if(format == 'C')  {
	fputc('\0',outfile);  /* no load address for coff files */
    } else {
	for(i=3; i>=0; i--) {
	    c = (loadaddr >> (8 * i));
	    checksum += c;
	    fputc(c,outfile);
	}
    }
    fputc(size,outfile);
    while(size--) {
	c = *ptr++;
	checksum += c;
	fputc(c,outfile);
    }
}
