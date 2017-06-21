/* $Id: srsp_stubs.c,v 3.5.28.4 1996/08/07 18:03:01 kmitchel Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/srsp_stubs.c,v $
 *------------------------------------------------------------------
 * Stubs to make the RSP slave image link
 *
 * August 1995, John Foster
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: srsp_stubs.c,v $
 * Revision 3.5.28.4  1996/08/07  18:03:01  kmitchel
 * CSCdi49854:  DBUSINTERR after RSP dbus write; ineffective recvry of
 * disabled card
 * Branch: California_branch
 *
 * Revision 3.5.28.3  1996/06/21  18:21:11  kmitchel
 * CSCdi47923:  DBUS-3-NORESP DBUS-3-BADCTRLRTYPE msgs when booting on RSP
 * Branch: California_branch
 *
 * Revision 3.5.28.2  1996/03/27  07:47:54  dbath
 * CSCdi51848:  Pulled slave remains in chassis MIB.
 * Branch: California_branch
 * Rip the slave out of the chassis MIB once we hear
 * the user has ripped it out of the chassis.
 *
 * Revision 3.5.28.1  1996/03/18  22:02:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.1  1996/03/09  05:13:10  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.5  1996/01/22  07:15:59  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1996/01/15  18:05:17  schaefer
 * CSCdi45761: support RSP in new chassis
 *
 * Revision 3.3  1995/12/05  07:06:39  dbath
 * CSCdi43178:  rommon boot info not kept in sync on slave
 * Ship the three boot related environment variables, and the config
 * register setting, across to the slave whenever autosync is enabled,
 * or whenever the user manually syncs the slave config.
 *
 * Revision 3.2  1995/11/17  18:50:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:28:59  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/24  20:48:53  foster
 * placeholder for future development
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface.h"
#include "subsys.h"
#include "../hes/dgbus.h"

/* ***************************** from rsp_if.c ************************** */

/*
 * referenced by DCL timer interrupt wrapper
 */
void (*dcl_timer_handler)(void);	/* throttle-off interrupt handler */
extern uint dcl_timer_count;		/* count of throttle-off interrupts */

boolean    rsp_debug;
boolean    disable_auto_interface_restart;

void
rsp_fs_init (void) {

#ifdef DEBUG
    rsp_debug = TRUE;
#endif

}


/* ****************************** from hes/cbus.c ************************ */

void 
disable_slot (uint slot) {}


/* ************************ from src-rsp/rsp_init.c ********************** */
void
dbus_reset_IP_slots (void) {}

void
dbus_startup (void) {}

void
dbus_prot_wr (uint slot, protected_uchar *reg, uchar value) {}

/* ********************** from src-rsp/rsp_chasint.c ********************* */

#include "rsp_chasint.h"

/*
 * array of booleans indicating which slots we should pretend to be empty
 */

slot_mask_t slot_present_mask;

int
chasint_interrupt (void) {
    return 1;			/* unexpected interrupt, crash */
}

boolean
ci_present (void)
{
    return FALSE;
}

uchar
compute_rsp_slot (void)
{
    return RSP_NO_SLOT;
}

uchar
adjust_rsp_slot (uchar slot)
{
    return slot;
}

/* *****************************rsp_if.c *********************************** */

/*
 * handle "scheduler allocate" command
 *
 *	OBJ(int,1) = microseconds for network interrupt handler
 *	OBJ(int,2) = microseconds with network interrupts masked
 */
void
sched_alloc_command (parseinfo *csb){}

void
netint_throttle_show (void) {}

void
platform_req_complex_restart (hwidbtype *hwidb) {}

void
rsp_process_rawq (void) {}

int
rsp_qa_error_interrupt (void)
{
    return 0;			/* crash not necessary */
}

/*
 * Handle network status interrupts from the Event queue in the QA ASIC
 */
void
rsp_process_eventq (void) {}


/* ******************************************************************* */

void 
rsp_sched_loop_end (boolean x) {}

/* ****************************** rsp_macddr_init ************************** */

void
macaddr_init (void) {}


/*
 * read & write the DS1201
 */
void
test_rsp_ds1201 (parseinfo *csb) {}


/* *************************** dbus.c ***************************/

/* 
 * dgtype* pslot_to_dbus (int slot)
 */
#include "../src-rsp/rsp_pcmap.h"
#include "../hes/dgbus.h"

/*
 * The slots[] array, and it's dbus related contents, don't exist on
 * the slave, since the slave can't be a dbus master.  Returning NULL
 * should make any errant callers sit up and take notice.
 */
slottype* get_slotp (int slot)
{
    return NULL;
}

dgtype* 
pslot_to_dbus (int slot) {
    return &((dgtype*) DGBUSSTART)[slot];
}
boolean 
board_has_instr_error (uint slot) {

    /* XXX bug inf?? */
    return FALSE;
}

boolean 
rsp_present (uint pslot){ 
    return FALSE;
}

void 
dbus_slot_reset (uint pslot) {}

/* needed in platform_rsp */

uint chassis_serial = 0;

static void dbus_init (subsystype* subsys)
{
}

/* 
 * dbus subsystem header
 */

#define DBUS_MAJVERSION 1
#define DBUS_MINVERSION 0
#define DBUS_EDITVERSION 1

SUBSYS_HEADER(dbus, DBUS_MAJVERSION, DBUS_MINVERSION, DBUS_EDITVERSION,
	      dbus_init, SUBSYS_CLASS_DRIVER,
	      "seq: flash_devices",
	      NULL);

void flash_copy_command (parseinfo *csb)
{
}

void auto_set_slave_confreg (ushort number, boolean vocal)
{
}
