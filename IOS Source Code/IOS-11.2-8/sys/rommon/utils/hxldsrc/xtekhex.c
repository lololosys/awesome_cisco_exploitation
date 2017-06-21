/* $Id: xtekhex.c,v 3.2 1995/11/17 18:49:26 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/utils/hxldsrc/xtekhex.c,v $
 *------------------------------------------------------------------
 * xtekhex.c
 *
 * June 1993, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Output in Extended TEK hex format
 *------------------------------------------------------------------
 * $Log: xtekhex.c,v $
 * Revision 3.2  1995/11/17  18:49:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:10:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:43:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include <stdio.h>

extern FILE *outfile;
extern unsigned long loadaddr;

#define ADDRCOUNT (sizeof(unsigned long) * 2) /* number of nibs in an addr */
#define HDRCOUNT (ADDRCOUNT + 6) /* number of nibbles in a header */
#define TERMBLK  8
#define DATABLK  6

union lasb { /* handle a long quantity as a number of bytes */
    unsigned long lval;
    unsigned char bval[sizeof(unsigned long)];
};

xtekhexout(d_size,ptr) /* output a 'record' in extended TEK hex format */
    char *ptr;
    register d_size;
{
    register i, chksum, type, char_count;
    union lasb ld_addr;
    register char *cptr;
    
    ld_addr.lval = loadaddr;
    char_count = HDRCOUNT + (d_size << 1); /* figure the total nibble count */
    if(d_size == 0) type = TERMBLK; /* termination block */
    else type = DATABLK; /* data block */
    chksum = ADDRCOUNT + type; /* add all nibbles for checksum */
    chksum += addnibs(char_count);
    for(i=0; i<(ADDRCOUNT/2); i++) chksum += addnibs(ld_addr.bval[i]);
    for(i=0, cptr = ptr; i<d_size; i++) chksum += addnibs(*cptr++);
    /* send the TEK hex record */
    fprintf(outfile,"%%%.2x%x%.2x%x%.8x",char_count,type,chksum & 0xff,
	    ADDRCOUNT,loadaddr);
    for(i=0, cptr = ptr; i<d_size; i++) fprintf(outfile,"%.2x",*cptr++ & 0xff);
    fprintf(outfile,"\r\n"); /* send the TEK hex record terminator */
    loadaddr += d_size; /* bump our load address for next record */
}

addnibs(c)
    unsigned char c;
{
    return((c >> 4) + (c & 0x0f));
}
