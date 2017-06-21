/* $Id: bsc_parse.h,v 3.2 1995/11/17 08:48:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/bstun/bsc_parse.h,v $
 *------------------------------------------------------------------
 * BSC parse - Parses the BSC configuration commands
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bsc_parse.h,v $
 * Revision 3.2  1995/11/17  08:48:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  06:24:41  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:14:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __BSC_PARSE_H
#define __BSC_PARSE_H

/*---- Exported prototypes. */

void
bsc_parse_primary(parseinfo *csb);

void
bsc_parse_secondary(parseinfo *csb);

void
bsc_parse_contention(parseinfo *csb);

void
bsc_parse_charset(parseinfo *csb);

void
bsc_parse_fdx(parseinfo *csb);

void
bsc_show_command(parseinfo *csb);

#endif  /* __BSC_PARSE_H. */
/*---- End include file: bsc_parse.h. */
