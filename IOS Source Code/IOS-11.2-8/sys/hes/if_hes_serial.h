/* $Id: if_hes_serial.h,v 3.3.60.2 1996/08/29 02:24:10 xtang Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_hes_serial.h,v $
 *------------------------------------------------------------------
 * if_hes_serial.h -- protypes for the FSIP and HSSI specific code in 
 *                    if_hes_serial.c
 *
 * September, 1989,  Robert Fletcher
 *
 * Copyright (c) 1988-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_hes_serial.h,v $
 * Revision 3.3.60.2  1996/08/29  02:24:10  xtang
 * CSCdi60447:  FSIP max MTU=14816 not 18000
 * Branch: California_branch
 *              Implemented new FSIP command 93 to pass the available
 *              buffer size in the controller to the system.
 *              The corresponding microcode change are in
 *              cvs -d /micro fsip_q (fsip187-0)
 *              and cvs -d /micro fsip_i (rsp_fsip204-0).
 *
 * Revision 3.3.60.1  1996/08/23  04:46:25  xtang
 * CSCdi59553:  clockrate 250000 is not configurable on FSIP in 75xx
 * Branch: California_branch
 *              added 250000 Hz to the FSIP interface card. Kept
 *              the AGS+ mciclock[] and mciclock_rev2[] untouched.
 *
 * Revision 3.3  1995/11/17  09:18:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:28:11  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:40:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:41:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_HES_SERIAL_H__
#define __IF_HES_SERIAL_H__

#define APP_COUNT 11
extern const ushort app_cmds[];
extern const uchar  app_uses[];

#define NCLOCKS  19	/* Number of clocks */
#define FCI_NCLOCKS 20        /* Number of clocks: newer platform
				 after AGS+ */

/* 
 * Serial interface clocking structure.
 */
typedef struct mciclocktype_ {
    int rate;
    ushort cookie;
    ushort prescalar;
} mciclocktype;

extern const mciclocktype fciclock[];
extern const mciclocktype mciclock[];
extern const mciclocktype mciclock_rev2[];

void hssi_loop_request(hwidbtype *idb);
void fsip_show_serial_state(hwidbtype* idb, char* leader);
void cbus_mci_serial_reset(hwidbtype *hwidb);
void hes_serial_idb_init(hwidbtype *hwidb);
boolean fsip_check_mtu(hwidbtype *hwidb, ushort mtu);

/*
 * Platform specific extern 's defined here for convenience sake.
 */
extern void platform_serial_reset_encsize(hwidbtype *hwidb);
extern void platform_serial_reset_action(hwidbtype *hwidb);
extern void platform_serial_reset_init(hwidbtype *hwidb);
extern void platform_serial_reset_clockrate(hwidbtype *hwidb);
extern void serial_platform_init(subsystype*);

/*
 * Externs for the parser stuff
 */
extern void clockrate_command(parseinfo *csb);
extern void hes_serial_parser_init(void);
#endif /* __IF_HES_SERIAL_H__ */
