/* $Id: ms_xchng_private.h,v 3.8.12.1 1996/08/29 22:54:20 ganesh Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/ms_xchng_private.h,v $
 *------------------------------------------------------------------
 * ms_xchnge_private.h - private defs shared by master/slave_xchng.c
 *
 * September 1995, David Bath
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ms_xchng_private.h,v $
 * Revision 3.8.12.1  1996/08/29  22:54:20  ganesh
 * CSCdi62683:  HSA requires that both RSP2s have same amount of DRAM
 * Branch: California_branch
 * Modified MS_XCHNG_VERSION definition to 3 and added ms_memsize
 * field to slave_signin_t structure.
 *
 * Revision 3.8  1996/02/15  00:53:08  dbath
 * CSCdi38455:  HSA needs SNMP support
 * Include major and minor SW versions in the slave signin message.
 * On the master, be sure to update the slave's SW version in the
 * chassis MIB, when the slave signs in.
 *
 * Revision 3.7  1996/01/23  01:17:55  dbath
 * CSCdi47277:  HSA slave leaks IPC buffers when version mismatch
 * Generate some form of response to all rpc requests, no matter how
 * little we might understand them.  Also, add a simple m/s exchange
 * version check on sign-in.
 *
 * Revision 3.6  1995/12/14  07:42:18  dbath
 * CSCdi45563:  Slave filesystem timestamps are wrong.
 * Ship the current time over to the slave every minute.
 *
 * Revision 3.5  1995/12/12  09:09:46  dbath
 * CSCdi45422:  HSA systems forget what time it is on failover
 * Implement client and server code for setting the calendar
 * on the slave.  Also, clean up some other IPC respone code
 * that was allocating unused data space.
 *
 * Revision 3.4  1995/12/11  04:09:23  dbath
 * CSCdi45341:  Cant get crash stack on HSA systems.
 * Add client and server code to gather slave crash stack details,
 * to the master/slave exchange mechanism.
 *
 * Revision 3.3  1995/12/05  07:06:30  dbath
 * CSCdi43178:  rommon boot info not kept in sync on slave
 * Ship the three boot related environment variables, and the config
 * register setting, across to the slave whenever autosync is enabled,
 * or whenever the user manually syncs the slave config.
 *
 * Revision 3.2  1995/11/17  18:46:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:27:48  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/14  03:53:11  dbath
 * Placeholders for VIP_branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Master/slave exchange version number.  Bump this number whenever
 * any aspect of this interface (as defined in this file) changes in
 * a non backward compatible way.  When the slave signs in, he'll
 * provide the master with  this version number.  The master can do
 * a quick comparison and decide whether or not to continue exchanging
 * with the slave.
 */
#define MS_XCHNG_VERSION 3

/*
 * Stuff used for slave sign-in
 */
#define SIGNIN_PORT_NAME "Slave Registration Port"
typedef struct slave_signin_t_ {
    ulong ms_version;
    char  version_string[CW_MAX_DESCR_STRLEN];
    ushort sw_version;
    ulong ms_memsize;
} slave_signin_t;


/*
 * Stuff used for slaverom
 */

#define SLAVEROM_PORT_NAME "Slave ROM Port"

typedef enum slaverom_msg_type_ {
    SLAVEROM_UNKNOWN_RSP,
    SLAVEROM_GETMS_RSP,
    SLAVEROM_GETMS_REQ,
    SLAVEROM_SETMS_RSP,
    SLAVEROM_SETMS_REQ,
    SLAVEROM_GETVAR_REQ,
    SLAVEROM_GETVAR_RSP,
    SLAVEROM_GETCONF_REQ,
    SLAVEROM_GETCONF_RSP,
    SLAVEROM_SETVAR_REQ,
    SLAVEROM_SETVAR_RSP,
    SLAVEROM_SETCONF_REQ,
    SLAVEROM_SETCONF_RSP,
    SLAVEROM_GETSTACK_REQ,
    SLAVEROM_GETSTACK_RSP,
    SLAVEROM_SETCALENDAR_REQ,
    SLAVEROM_SETCALENDAR_RSP,
    SLAVEROM_SETTIME_REQ,
    SLAVEROM_SETTIME_RSP,
} slavems_msg_type;

typedef struct slaverom_t_ {
    uint    ms_setting;
    ushort  confreg_setting;
    int     monvar_code;
    boolean monvar_exists;
    char    monvar_value[MAX_BOOTVAR_LEN];
} slaverom_t;

/*
 * The ROM monitor doesn't define how large this can be, they're just
 * quoted strings in .c files.  What we're saying here is that if ever
 * the ROM monitor comes up with one larger than 64 bytes, we'll truncate
 * it to 64 for the purpose of the slave stack display.
 */
#define MAX_ROM_REASON_LEN       64

typedef struct whyreload_t_ {
    ulong   boot_data[BOOT_STACK_LAST+1];
    char    rom_string[MAX_ROM_REASON_LEN];
    char    prior_version_string[CW_MAX_DESCR_STRLEN];
} whyreload_t;

typedef struct slave_time_t_ {
    clock_epoch   current_time;
    boolean       valid;
    clock_source  source;
} slave_time_t;

