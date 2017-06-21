/* $Id: rif_util.c,v 3.2.44.2 1996/05/17 11:23:35 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/if/rif_util.c,v $
 *------------------------------------------------------------------
 * rif_util.c Utility routines for dealing with rif's.
 *
 * May 1995, Richard Livingston
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rif_util.c,v $
 * Revision 3.2.44.2  1996/05/17  11:23:35  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.32.2  1996/04/26  15:20:35  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.32.1  1996/04/03  14:38:51  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.44.1  1996/03/18  20:14:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.14.2  1996/03/07  09:44:32  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.14.1  1996/01/24  22:19:04  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/12/16  22:48:17  motto
 * CSCdi35413:  Alignment errors in R/SRB process level code
 *
 * Revision 3.1  1995/11/09  11:52:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/07  05:11:05  richl
 * CSCdi38315:  makefile rework
 *              add new files for libraries
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * NOTE:  This module is not the place for code which works with the
 *        rif table used by multiring routing and proxy explorer. 
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_generic.h"
#include "ieee.h"
#include "rif.h"

/*
 * rc_code_to_bytes 
 *
 * Convert back and forth between maximum packet lengths, and the largest
 * frame bits in a routing descriptor.
 */

int rc_code_to_bytes (int code)
{
    switch (code) {
	case RC_LF516:	   return(RC_LF516_VAL);
	case RC_LF1500:	   return(RC_LF1500_VAL);
	case RC_LF2052:	   return(RC_LF2052_VAL);
	case RC_LF4472:	   return(RC_LF4472_VAL);
	case RC_LF8144:	   return(RC_LF8144_VAL);
	case RC_LF11407:   return(RC_LF11407_VAL);
	case RC_LF17800:   return(RC_LF17800_VAL);
        case RC_LFINITIAL: return(0);
	default:	   return(-1);
    }
}

static struct {
    int byte_val;
    int lf_bits;
} bytes_to_lf[] = {
    { RC_LF17800_VAL,	RC_LF17800 },
    { RC_LF11454_VAL,	RC_LF11454 },
    { RC_LF11407_VAL,	RC_LF11407 },
    { RC_LF8144_VAL,	RC_LF8144  },
    { RC_LF4472_VAL,	RC_LF4472  },
    { RC_LF2052_VAL,	RC_LF2052  },
    { RC_LF1500_VAL,	RC_LF1500  },
    { RC_LF1470_VAL,	RC_LF1470  },
    { RC_LF516_VAL,	RC_LF516   },
    { 0,		0 },
};

/*
 * b y t e s _ t o _ r c _ c o d e
 *
 * Convert a number of bytes into the largest LF value that is smaller than
 * that number of bytes.
 *
 * Note the behaviour of this function has been changed.  Formerly return -1
 * if the number bytes was not an exact match to one of the LF_VALS.  Now we
 * return the floor of the value passed in.  This has the effect in
 * configuration commands that previously illegal values will be coerced
 * into legal values.
 */

int bytes_to_rc_code (int bytes)
{
    int i;

    if (bytes < RC_LF_MIN || bytes > RC_LF_INFINITY)
	return(-1);
    for (i = 0; bytes_to_lf[i].byte_val != 0; i++)
	if (bytes >= bytes_to_lf[i].byte_val)
	    return(bytes_to_lf[i].lf_bits);
    return(-1);
}

/*
 * get_rif_str
 *
 * Print a single rif entry into the buffer passsed by the caller.  
 * It is the caller's responsibility to make sure the buffer is big enough.
 * Copy by shorts for speed.
 */

char *get_rif_str (uchar *ptr, char *buff)
{
    int i, riflen, length;

    if (buff == NULL)
	return(buff);
    buff[0] = '\0';
    if (!ptr)
      return(buff);

    riflen = (ptr[0] & RC_LTHMASK) / 2;
    if (riflen == 0)
	return(buff);
    if (riflen > SRB_MAXRIF/2)
      riflen = SRB_MAXRIF/2;
    length = sprintf(buff, "%-04x",GETSHORT(ptr));
    if (riflen > 1) {
	for (i = 1; i < riflen; i++)
	  length += sprintf(buff+length, ".%-04x", GETSHORT((ptr+(2*i))));
    }
    buff[length] = '\0';
    return(buff);
}


/*
 * reverse_rif
 *
 * Produce a new rif written in the reverse direction from the supplied
 * rif.  No this does not flip the direction bit.
 */
static void reverse_rif (srbroute_t *source, srbroute_t *dest)
{
    ushort *rd_s, *rd_d;
    int i;

    dest->rc_blth = source->rc_blth;
    dest->rc_dlf = source->rc_dlf ^ RC_D;
    if ((source->rc_blth & RC_LTHMASK) == 2)
	return;

    i = ((source->rc_blth & RC_LTHMASK) - 2) / 2 - 1;
    rd_s = &source->rd[i];
    rd_d = &dest->rd[0];
    for ( ; i > 0; i--) {
	*rd_d = (GETSHORT(rd_s--) & RD_RNMASK);
	*rd_d++ |= (GETSHORT(rd_s) & RD_BNMASK);
    }
    *rd_d = (GETSHORT(rd_s) & RD_RNMASK);
}


/*
 * flip_rif
 *
 * Flip a RIF in-place.
 * Reverses order of route descriptors and changes D bit
 */
void flip_rif (srbroute_t *source)
{
    uchar  flipped_rif[SRB_MAXRIF];
    int    rc_len;

    rc_len = source->rc_blth & RC_LTHMASK;
    reverse_rif(source, (srbroute_t*) &flipped_rif[0]);
    bcopy(&flipped_rif[0], source, rc_len);
    
}

/*
 * make_forward_rif
 *
 * Given any rif, create a new rif that is written in the forward
 * direction.
 */
void make_forward_rif (srbroute_t *source, srbroute_t *dest)
{
    if ((source->rc_dlf & RC_D) == 0) {
	bcopy(source, dest, source->rc_blth & RC_LTHMASK);
	return;
    }
    reverse_rif(source, dest);
}

/* 
 * rif_contains_rif
 * 
 */
boolean rif_contains_rif (srbroute_t *long_route, srbroute_t *short_route) 
{
    int short_length;
    int long_length;
    uchar long_forward[SRB_MAXRIF];
    uchar short_forward[SRB_MAXRIF];
    srbroute_t *plong_forward = (srbroute_t *)&long_forward;
    srbroute_t *pshort_forward = (srbroute_t *)&short_forward;

    short_length = short_route ? short_route->rc_blth & RC_LTHMASK : 0;
    if (short_length <= 2) {
	return (TRUE);
    }
    long_length = long_route ? long_route->rc_blth & RC_LTHMASK : 0;
    if (short_length >= long_length) {
	return (FALSE);
    }
    make_forward_rif(long_route, plong_forward);
    make_forward_rif(short_route, pshort_forward);
    return (!bcmp(&pshort_forward->rc_dlf,
		  &plong_forward->rc_dlf, short_length - 1));
}

