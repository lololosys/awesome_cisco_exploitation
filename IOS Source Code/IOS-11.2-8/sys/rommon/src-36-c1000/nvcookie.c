/* $Id: nvcookie.c,v 3.3.44.1 1996/03/21 23:29:09 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-36-c1000/nvcookie.c,v $
 *------------------------------------------------------------------
 * nvcookie.c
 *
 * November 1994, Rob Clevenger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * NVRAM Cookie routines for Sapphire.
 *------------------------------------------------------------------
 * $Log: nvcookie.c,v $
 * Revision 3.3.44.1  1996/03/21  23:29:09  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/02/08  08:58:34  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.2  1996/01/16  01:59:39  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.1  1995/12/01  04:22:06  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.3  1995/12/19  01:20:45  dcoli
 * CSCdi45647:  ROM Monitor should handle cookie properly for ciscoPro
 * Don't allow clearing of CPU_CISCOPRO_MASK bit in NVRAM copy of the
 * processor field of the cookie.
 *
 * Revision 3.2  1995/11/17  18:44:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:39:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "cookie.h"
#include "proto.h"
#include "../../../boot/cpu.h"
#include "extern.h"

/*
** Special cookie code for Sapphire.
** New platforms will have the cookie information programmed into
** the beginning of NVRAM during ICT in a special structure (nvcookie)
** with a magic number and a checksum.  The routines in this module
** are used to validate and access this info.  Two copies of the
** cookie are maintained because the cookie info is in NVRAM and
** subject to possible corruption.
*/

static char cookie_copy_valid;

/*
** Validate the integrity of a nvcookie structure.
*/
int
nvcookie_valid(struct nvcookie *nvcp)
{
    if(nvcp->magic == NVCOOKIE_MAGIC &&
       (unsigned short)chksum((unsigned short *)nvcp,
	      sizeof(struct nvcookie) - sizeof(nvcp->chksum))
       == nvcp->chksum) {
	return(1);
    }
    return(0);
}


/*
** Convenient routine for the outside world.
** Actually read the cookie the first time called.
** Make this call quick after initial invokation
** since it gets called from EMT.
** Return a buffer of all zeroes if cookie info is
** not available.
*/
struct cookie *
get_cookie(void)
{
    struct nvcookie *icnvcp;  /* in-core nvcookie pointer */

    icnvcp = &(NVRAM)->nvcookie;
    if(!cookie_copy_valid) {
	cookie_copy_valid = 1;
	movbyte((uchar *)&(NVRAM_SAV)->nvcookie,
		    (uchar *)icnvcp, sizeof(struct nvcookie));
	if(!nvcookie_valid(icnvcp)) {
	    printf("\nBad checksum on cookie structure, "
		   "resorting to backup copy\n");
	    movbyte((uchar *)&(NVRAM_SAV)->nvcookie_bkup,
		    (uchar *)icnvcp, sizeof(struct nvcookie));
	    if(!nvcookie_valid(icnvcp)) {
		printf("\nWarning: Cookie information is corrupt\n");
		bzero(icnvcp, sizeof(struct nvcookie));
		cookie_copy_valid = 0;
	    } else {
		printf("\nCorrecting primary cookie from backup\n");
		put_cookie(&icnvcp->cookie);
	    }
	}
    }
    return(&icnvcp->cookie);
}


/*
** Write cookie information back out to NVRAM.
** Write both the primary and backup copies.
** Write only when necessary.
** Retry once if first attempt fails.
*/
void
put_cookie(struct cookie *cptr)
{
    struct nvcookie *icnvcp = &(NVRAM)->nvcookie;  /* in-core nvcookie */
    struct cookie *tmp_cookie_ptr;
    struct nvcookie tmp_nvcookie;
    int times;

    if(cptr != &icnvcp->cookie) {  /* copy only if necessary */
	movbyte((uchar *)cptr, (uchar *)&icnvcp->cookie,
		sizeof(struct cookie));
    }
    icnvcp->magic = NVCOOKIE_MAGIC;
    icnvcp->chksum = chksum((unsigned short *)icnvcp,
	      sizeof(struct nvcookie) - sizeof(icnvcp->chksum));

    /* 
     * If this is a ciscoPro box, don't allow clearing of the 
     * CPU_CISCOPRO_MASK bit in the NVRAM copy of the processor field of 
     * the cookie.  This changes the box into a classic router.
     * So copy in-core cookie contents to a temporary copy.
     * If ciscopro_flag, set CPU_CISCOPRO_MASK.
     * Then use the temporary copy to copy back out to NVRAM
     */
    movbyte((uchar *)&icnvcp->cookie, (uchar *)&tmp_nvcookie.cookie, 
		sizeof(struct cookie));
    if (ciscopro_flag) {
	tmp_cookie_ptr = &tmp_nvcookie.cookie;
	tmp_cookie_ptr->processor |= CPU_CISCOPRO_MASK;
    }
    tmp_nvcookie.magic = NVCOOKIE_MAGIC;
    tmp_nvcookie.chksum = chksum((unsigned short *)&tmp_nvcookie,
	      sizeof(struct nvcookie) - sizeof(tmp_nvcookie.chksum));

    if(bcmp((uchar *)&tmp_nvcookie, (uchar *)&(NVRAM_SAV)->nvcookie,
	    sizeof(struct nvcookie)) != 0) {
	for(times = 0; times < 2; times++) {
	    nvwrite((uchar *)&tmp_nvcookie, (uchar *)&(NVRAM_SAV)->nvcookie,
		    sizeof(struct nvcookie));
	    if(nvcookie_valid(&(NVRAM_SAV)->nvcookie)) break;
	}
    }
    if(bcmp((uchar *)&tmp_nvcookie, (uchar *)&(NVRAM_SAV)->nvcookie_bkup,
	    sizeof(struct nvcookie)) != 0) {
	for(times = 0; times < 2; times++) {
	    nvwrite((uchar *)&tmp_nvcookie,
		    (uchar *)&(NVRAM_SAV)->nvcookie_bkup,
		    sizeof(struct nvcookie));
	    if(nvcookie_valid(&(NVRAM_SAV)->nvcookie_bkup)) break;
	}
    }
}

/* end of module */
