/* $Id: bsc_conf.h,v 3.2.58.1 1996/09/10 08:47:59 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_conf.h,v $
 *------------------------------------------------------------------
 * bsc_conf.h - Commands to execute once parse identifies the command
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * Command to execute once the parse identifies the command
 *
 *------------------------------------------------------------------
 * $Log: bsc_conf.h,v $
 * Revision 3.2.58.1  1996/09/10  08:47:59  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:48:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/03  01:03:53  arothwel
 * CSCdi39397:  Fix:
 *
 *         o Add specific poll support for local-ack pollee.
 *         o Remove superfluous fields from lcb.
 *         o Generalize protocol main handler.
 *         o Finish off replacing all deprecated process mgnt calls.
 *         o Multiple poll-adds causing hard-on polling.
 *         o Add safety code to ignore 'spurious' Tx completes.
 *
 * Revision 2.2  1995/06/16  06:24:22  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:13:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __BSC_CONF_H
#define __BSC_CONF_H

#include "interface.h"

#include "bsc.h"
#include "bsc_lcb.h"

/*---- Exported constants. */

#define BSC_CHARSET_EBCDIC   "ebcdic"
#define BSC_CHARSET_ASCII    "ascii"

/*---- Exported prototypes. */

void
BscKeyPrimary(hwidbtype* idb,
               boolean local_ack);

void
BscNKeyPrimary(hwidbtype* idb);

void
BscKeySecondary(hwidbtype* idb, boolean local_ack);

void
BscNKeySecondary(hwidbtype* idb);

void
BscKeyContention(hwidbtype* idb);

void
BscNKeyContention(hwidbtype* idb);

void
BscKeyCharSet(lcb_t* lcb, char_set_e char_set);

void
BscNKeyCharSet(lcb_t* lcb, char_set_e char_set);

void
BscKeyFdx(hwidbtype* idb);

void
BscNKeyFdx(hwidbtype* idb);

void
BscKeyPause(lcb_t* lcb, int pause_timeout);

void
BscNKeyPause(lcb_t* lcb);

void
BscKeyServLim(lcb_t* lcb, int servlim);

void
BscNKeyServLim(lcb_t* lcb);

void
BscKeyPollTimeout(lcb_t* lcb, int poll_timeout);

void
BscNKeyPollTimeout(lcb_t* lcb);

void
BscKeyHostTimeout(lcb_t* lcb, int host_timeout);

void
BscNKeyHostTimeout(lcb_t* lcb);

void
BscKeyRetries(lcb_t* lcb, int retries);

void
BscNKeyRetries(lcb_t* lcb);

void
BscKeySpecPoll(lcb_t* lcb);

void
BscNKeySpecPoll(lcb_t* lcb);

void
BscWriteConfig(lcb_t* lcb, uint bsc_cmd);

void
bsc_show_interface(hwidbtype* idb, uint address);

#endif  /* __BSC_CONF_H. */
/*---- End include file: bsc_conf.h. */
