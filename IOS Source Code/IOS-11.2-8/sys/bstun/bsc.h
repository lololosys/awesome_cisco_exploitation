/* $Id: bsc.h,v 3.3.50.3 1996/09/10 08:47:51 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc.h,v $
 *------------------------------------------------------------------
 * bsc.h - Common defines for BSC
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bsc.h,v $
 * Revision 3.3.50.3  1996/09/10  08:47:51  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.3.50.2  1996/07/22  18:22:09  snyder
 * CSCdi63677:  fix a pointer problem, and move more text to data
 *              moves 2692 bytes to data, 16 out of image completely
 * Branch: California_branch
 *
 * Revision 3.3.50.1  1996/05/17  10:39:59  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.38.2  1996/04/22  08:57:31  jbalestr
 * CSCdi54541:  system gets stuck in BSC-3-BADLINESTATE
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.38.1  1996/04/03  13:49:50  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3  1995/12/04  02:34:46  jbalestr
 * CSCdi41360:  BSC local-ack, slow response to poll.
 *
 * Revision 3.2  1995/11/17  08:48:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/09/03  01:03:51  arothwel
 * CSCdi39397:  Fix:
 *
 *         o Add specific poll support for local-ack pollee.
 *         o Remove superfluous fields from lcb.
 *         o Generalize protocol main handler.
 *         o Finish off replacing all deprecated process mgnt calls.
 *         o Multiple poll-adds causing hard-on polling.
 *         o Add safety code to ignore 'spurious' Tx completes.
 *
 * Revision 2.4  1995/08/11  08:21:45  arothwel
 * CSCdi38625:  Fix:
 *              1. Broken timer s/ware stopping Tx/Rx.
 *              2. Incorrect bsc-mib sub-system dependencies.
 *              3. Premature starting of bstun-event-process.
 *              4. Removal of timestamp info from debugging messages.
 *              5. Replace deprecated process mngmt s/ware w/ new s/ware.
 *
 * Revision 2.3  1995/06/28  09:22:39  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/16  06:24:19  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:13:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __BSC_H
#define __BSC_H


#include "master.h"
#include "interface.h"


/*---- Exported constants. */

#define BSC_PROTOCOL_NAME      "bsc"
#define BSC_LACK_PROTOCOL_NAME "bsc-local-ack"
#define BSC_HEADER_SIZE        3
#define BSC_SYN_PREFIX_SIZE    2

#define ONE_SECOND             1000     /* milli-secs. */


/*---- Exported structures. */

extern const uchar ascii_dev_to_unit[];


/*---- Exported functions. */

extern void
bsc_init(subsystype *);

extern void
bsc_hwif_goingdown(hwidbtype* idb);

extern void
bsc_hwif_comingup(hwidbtype* idb);

extern void
bsc_cstate(hwidbtype* idb);

extern void
pp_idb_init(hwidbtype* idb);

extern boolean
bsc_query_local_ack_capable(hwidbtype* idb);

extern void
bsc_send_to_bstun(hwidbtype* idb,
		  paktype* pak,  
		  uchar cu_addr,
		  uchar cu_cmd,
		  uchar dev_addr);

extern void
bsc_send_to_line(hwidbtype* idb,
		 paktype* pak);

extern void
bsc_txComp_Notify(hwidbtype* idb);

extern void
lcb_start_io_timer(lcb_t* lcb,
		   ulong timeout_val);

extern boolean
lcb_conditional_restart_io_timer(lcb_t* lcb,
				 paktype* pak);

extern void
lcb_stop_io_timer(lcb_t* lcb);

#endif  /* __BSC_H. */
/*---- End include file: bsc.h. */
