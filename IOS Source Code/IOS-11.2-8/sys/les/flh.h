/* $Id: flh.h,v 3.3.12.1 1996/03/18 20:40:50 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/les/flh.h,v $
 *------------------------------------------------------------------
 * flh.h -- header files for flh.c
 *
 * Oct 1994, Eric Fronberg
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flh.h,v $
 * Revision 3.3.12.1  1996/03/18  20:40:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/16  07:11:37  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.1  1996/01/24  22:22:41  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1996/02/19  17:28:28  abaindur
 * CSCdi30593:  SHOW VERSION underreports processor memory by 4 kilobytes
 *
 * Revision 3.2  1995/11/17  17:37:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:30:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:28:47  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  21:31:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LES_FLH_H__
#define __LES_FLH_H__

#include "../../boot/flh.h"
#include "../../boot/emt.h"

#define FLH_OP	0
#define FLH_RETRY_COUNT  3
#define FLH_SRCDST_ROM_BUFF_SIZE (80) /* 80 char buff -  */

#define FLH_MAX_IP_LENGTH	(17) /* (size of 'XXX.XXX.XXX.XXX' */
				/* plus 2 spaces */

#define FLH_LOGGING_ACTIVE	(have_flh_logging != -1)

#define FLH_ROMVERSION 	(emt_call(EMT_FLASH_LOAD_HELPER, FLH_GET_VERSION))
#define FLH_LOGBUFSIZE	(emt_call(EMT_FLASH_LOAD_HELPER, FLH_GET_LOGBUFF_LEN))
#define FLH_LOGROUTINES(x)	\
                        (emt_call(EMT_FLASH_LOAD_HELPER, FLH_GET_ROUTINES, x))
#define FLH_START(x)	(emt_call(EMT_FLASH_LOAD_HELPER, FLH_DO_IT, x))
#define FLH_PARMBUFSIZE	(emt_call(EMT_FLASH_LOAD_HELPER, FLH_GET_PARMBUFF_LEN))

extern void flhbuf_print(parseinfo *csb);
extern void flh_parser_init(void);

#endif /* __LES_FLH_H__ */



