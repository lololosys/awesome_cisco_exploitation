/* $Id: dbus.c,v 3.29.4.27 1996/09/04 18:54:04 kmitchel Exp $
 * $Source: /release/112/cvs/Xsys/hes/dbus.c,v $
 *------------------------------------------------------------------ 
 *  dbus.c - Diagnostic bus support routines
 *
 *  William H. Palmer, Tue Oct  6 23:29:48 1992  
 *
 *  Copyright (c) 1992-1997 by cisco Systems, Inc.  
 *  All rights reserved.  
 *------------------------------------------------------------------
 * $Log: dbus.c,v $
 * Revision 3.29.4.27  1996/09/04  18:54:04  kmitchel
 * CSCdi67547:  Reflect DBUS timeout recovery participation
 * Branch: California_branch
 *
 * Revision 3.29.4.26  1996/09/04  18:12:16  kmitchel
 * CSCdi66418:  IOS overreacts to DBUS io port-whine
 * Branch: California_branch
 *
 * Revision 3.29.4.25  1996/08/29  23:31:20  snyder
 * CSCdi67619:  facility obsoleted by CSCdi67083, dump it, save memory
 *              saves 404 bytes from image, 360 from data section
 *              also pre-commit hamptons error message doc macros, so
 *              between the 2 of us we only touch logger.h one time
 * Branch: California_branch
 *
 * Revision 3.29.4.24  1996/08/28  12:43:45  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.29.4.23  1996/08/22  06:00:58  pitargue
 * CSCdi66791:  reserve fci type and idb type for Yosemite
 * Branch: California_branch
 *
 * Revision 3.29.4.22  1996/08/20  00:34:18  cheilek
 * CSCdi58374:  More show diag info needed
 * Branch: California_branch
 * Add CCB_CMD_VIP_GET_CONFIG, vip_ll_sw.
 * Add sw_version_str to slot structure.
 *
 * Revision 3.29.4.21  1996/08/15  18:06:04  ptran
 * Add ESA's idb to VIP and RSP hwidb/swidb chain.
 * CSCdi64021:  show diag doesnt display EPA info when the next PA is empty
 * Branch: California_branch
 *
 * Revision 3.29.4.20  1996/08/07  17:59:35  kmitchel
 * CSCdi49854:  DBUSINTERR after RSP dbus write; ineffective recvry of
 * disabled card
 * Branch: California_branch
 *
 * Revision 3.29.4.19  1996/08/01  20:08:37  sjacobso
 * CSCdi42087:  sho diag 31 not working for CI board in 75xx systems
 * Branch: California_branch:  Modified show diag to accept 31.
 *
 * Revision 3.29.4.18  1996/07/26  01:09:39  feisal
 * CSCdi61016:  Need to add CardType MIB definition for different flavors
 * of 1FE PA
 * Branch: California_branch
 *
 * Revision 3.29.4.17  1996/07/20  22:29:25  cheilek
 * CSCdi62018:  show diag crashes when printing Slot 31 (virtual)
 *              information
 * Branch: California_branch
 * Respect idprom_only flag in print_dbus_card.
 *
 * Revision 3.29.4.16  1996/07/10  02:37:41  getchell
 * Added crash context mechanism which is shown via diag command and test
 * cbus (vc command)
 * CSCdi60415:  VIP difficult to debug in the field
 * Branch: California_branch
 *
 * Revision 3.29.4.15  1996/07/09  21:14:49  getchell
 * Modify read bus exception handler and DCL interrrupt handler to deal
 * with dbus accesses to non-existent card.
 * CSCdi58555:  bus exception caused by dbus access to non-existant card
 * Branch: California_branch
 *
 * Revision 3.29.4.14  1996/07/09  20:59:39  getchell
 * CSCdi61290:  code review comments for dbus code
 * Branch: California_branch
 *
 * Revision 3.29.4.13  1996/07/08  22:25:02  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.29.4.12  1996/07/02  19:28:58  cheilek
 * CSCdi58160:  All Port Adapters board revision in hex format vs
 * alphanumeric
 * Branch: California_branch
 * Change display from decimal (!) to usual decoding algorithm.
 *
 * Revision 3.29.4.11  1996/06/30  18:10:48  ptran
 * CSCdi61257:  snmp and show diag support for encryption PA
 * Branch: California_branch
 *
 * Revision 3.29.4.10  1996/06/21  18:19:35  kmitchel
 * CSCdi47923:  DBUS-3-NORESP DBUS-3-BADCTRLRTYPE msgs when booting on RSP
 * Branch: California_branch
 *
 * Revision 3.29.4.9  1996/05/21  06:29:02  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.29.4.8  1996/05/06  07:21:16  jwelder
 * CSCdi51998:  Serial Number and Part Number not printed correctly for PAs
 * Branch: California_branch
 *
 * Revision 3.29.4.7  1996/04/23  14:02:23  banderse
 * CSCdi50739:  CIP and other swdbus cards do not need extra reset
 * Branch: California_branch
 * add dbus_ctrlr_start() routine.
 *
 * Revision 3.29.4.6  1996/04/13  00:44:28  cheilek
 * CSCdi48224:  -FX FE Port Adapter, reported as -TX
 * Branch: California_branch
 * Add code to show diag output to identify precisely which Fast Ethernet
 * PA is installed.  Fix whitespace problems.
 *
 * Revision 3.29.4.5  1996/04/06  19:28:25  foster
 * CSCdi52285:  OIR causes HSA system to panic with %DBUS-3-BADHSACONF
 * Branch: California_branch
 *
 * Revision 3.29.4.4  1996/04/03  22:48:56  jwelder
 * CSCdi47348:  lingering references to rvip
 * Branch: California_branch
 *
 * Revision 3.29.4.3  1996/03/27  07:44:24  dbath
 * CSCdi51848:  Pulled slave remains in chassis MIB.
 * Branch: California_branch
 * Add access routine to the slots[] array.
 *
 * Revision 3.29.4.2  1996/03/21  22:39:52  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.29.4.1  1996/03/18  19:40:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.7  1996/03/18  23:18:09  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.6  1996/03/14  01:02:02  fsunaval
 * CSCdi50801:  SW-VIP1: Show diag has wrong board type for FDDI PA
 * Branch: ELC_branch
 *
 * Revision 3.3.2.5  1996/02/13  23:44:55  mbeesley
 * CSCdi48865:  Fix compilation of dbus.c
 * Branch: ELC_branch
 *
 * Revision 3.3.2.4  1996/02/10  00:04:02  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.3.2.3  1996/02/08  07:20:29  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3.2.2  1996/01/15  10:18:44  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.1  1995/12/11  18:46:42  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.29  1996/03/11  11:46:10  ogrady
 * CSCdi51224:  RSP emulation code is obsolete
 *              Deterge the code.
 *
 * Revision 3.28  1996/03/07  05:56:14  dbath
 * CSCdi50895:  HSA master sometimes hangs on dbus flow control
 * If while downloading the slave, he gets seriously wedged, don't
 * panic.  Log a message and keep trucking, otherwise neither of
 * them will route packets.
 *
 * Revision 3.27  1996/03/06  22:35:13  jwelder
 * CSCdi50800:  Show Diagbus changed to show VIP1 Dram Size
 *
 * Revision 3.26  1996/03/06  10:20:26  feisal
 * CSCdi47327:  Need means of getting RVIP stack trace
 *
 * Revision 3.25  1996/03/06  01:23:04  jwelder
 * CSCdi50800:  Show Diagbus changed to show VIP1 Dram Size
 *
 * Revision 3.24  1996/03/03  07:32:33  dbath
 * CSCdi50569:  CCB timeouts during EOIR events.
 * In dbus_wait_for_sw_ready(), take blocking_disabled into
 * account when deciding whether to suspend, or to go into
 * a busy CPU loop.
 *
 * Revision 3.23  1996/03/02  19:53:04  dbath
 * CSCdi50576:  dbus.c could use some tidying up
 * Remove a big wad of RSP2 specific code; collapsing it back
 * into the general case, by use of a generic "controller needs
 * flow control" concept.
 *
 * Revision 3.22  1996/03/01  09:20:10  tboyle
 * CSCdi50488:  WCSLDERR on 32mb RVIP
 *
 * Revision 3.21  1996/02/29  05:22:06  foster
 * CSCdi47230:  HSA slave should come up after crash
 *
 * Revision 3.20  1996/02/27  21:04:35  kmitchel
 * CSCdi42373:  Require CIP hw ver of 4.2 or later for 7500 compatibility
 *              check
 *
 * Revision 3.19  1996/02/27  18:23:01  foster
 * CSCdi47705:  HSA master got the message slave software no ready
 *
 * Revision 3.18  1996/02/23  10:15:21  feisal
 * CSCdi45132:  Crash when remove VIP when on IF-CON to VIP
 *
 * Revision 3.17  1996/02/20  04:00:07  kmitchel
 * CSCdi46899:  CIP ucode download failure if CIP is in lowest numbered
 *              slot
 *
 * Revision 3.16  1996/02/09  10:03:38  foster
 * CSCdi47873:  HSA ROMMON needs to add flow control to dbus download
 * when you pick this change up you have to run with "125" ROMs or later
 * in an HSA system.
 *
 * Revision 3.15  1996/02/06  06:49:54  foster
 * CSCdi48053:  show diag 7 causes HSA master to bus error
 *
 * Revision 3.14  1996/02/06  05:28:52  foster
 * CSCdi47230:  HSA slave should come up after crash
 *
 * Revision 3.13.2.1  1996/01/24  21:45:08  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.13  1996/01/19  22:54:27  foster
 * CSCdi47104:  HSA proms need some timing changes in system code
 * A temporary timing fix is needed until the proms can be sorted out.
 *
 * Revision 3.12  1996/01/18  17:12:23  foster
 * CSCdi47070:  show diag crash when master gets to the slave
 * RSP2 does not have io_regs!
 *
 * Revision 3.11  1996/01/18  06:46:32  foster
 * CSCdi47104:  HSA proms need some timing changes in system code
 *
 * Revision 3.10  1996/01/15  18:03:56  schaefer
 * CSCdi45761: support RSP in new chassis
 *
 * Revision 3.9  1995/12/30  09:30:42  shaker
 * CSCdi46228:  Interfaces get shutdown after removal & reinsertion
 * Do NOT poll the dbus for errors while a hotstall is active. We will
 * cause DBUS errors if we try to poll a card that was just removed,
 * for instance.
 *
 * Revision 3.8  1995/12/28  08:18:46  foster
 * CSCdi46157:  Dbus down-load needs to work on rsp2 slave down-load
 *
 * Revision 3.7  1995/12/26  23:08:13  dlobete
 * CSCdi42282:  chassis mib incomplete for VIP
 * Add support for PA MIB information to be relayed from VIP to RP/RSP
 * via love-letter.
 * Rework some of the 4T PA's data structures as a result of above.
 *
 * Revision 3.6  1995/12/15  08:01:25  rramacha
 * CSCdi42101:  sh ver does not recognize CX-SMIP or CX-SSIP cards
 *
 * Revision 3.5  1995/12/04  20:15:51  schaefer
 * CSCdi44946: add enums for new hardware
 * add controller types for rsp/4, rsp/7000, ci/7000
 *
 * Revision 3.4  1995/11/29  18:45:30  banderse
 * CSCdi43438:  CIP needs default microcode flash image
 * Added release specific default flash image; changes to ensure flash file
 * system is initialized completely before IP download.
 *
 * Revision 3.3  1995/11/17  09:15:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:25:56  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:37:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.14  1995/11/08  20:59:42  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.13  1995/10/11  07:07:57  mcieslak
 * Reload the microcode for a slot that has failed and allow
 * for IPs that are disabled to be removed cleanly.
 * CSCdi41907:  complex restart / EOIR of disabled cards broken
 *
 * Revision 2.12  1995/10/03  00:18:57  mcieslak
 * Don't attempt to load microcode for controller types that are greater
 * than or equal to FCI_NUM_CONTROLLERS.
 * CSCdi40862:  RSP2 SegV exception at boot with SSIP and SMIP installed
 *
 * Revision 2.11  1995/09/12  18:22:07  kramling
 * CSCdi37548:  FEIP Parity error requires OIR or Power cycle to clear it
 *
 * Revision 2.10  1995/08/21  07:39:50  hampton
 * Transition the hes directory code to use the passive timer macros for
 * all its timers.  It no longer references the system clock directly.
 * [CSCdi39033]
 *
 * Revision 2.9  1995/08/10 19:11:40  schaefer
 * CSCdi38464: need branch patch in 11.0
 * don't crash if two RSPs are installed
 *
 * Revision 2.8  1995/08/09  00:19:22  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.7  1995/08/08  16:46:28  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.6  1995/07/28 18:24:29  schaefer
 * CSCdi37798: use enum in controller compatibility test
 *
 * Revision 2.5  1995/07/27  04:45:12  hampton
 * The 7000 RP serial number should be accessible via the platform_get_xxx
 * routines.  [CSCdi37744]
 *
 * Revision 2.4  1995/07/21 03:56:11  mcieslak
 * Added code to ensure that only hardware versions 1.3 and higher
 * are considered RSP compatible.
 * CSCdi33730:  %CBUS-3-CMDTIMEOUT:  Timeouts, with traceback
 *
 * Revision 2.3  1995/07/17  08:21:09  schaefer
 * CSCdi36181: more robust microcode management (on RSP)
 *
 * Revision 2.2  1995/06/09  13:04:27  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:38:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "subsys.h"
#include "interface_private.h"
#ifdef C7000
#include "../../boot/src-68-c7000/pcmap.h"
#endif
#include "../hes/dgbus.h"
#include "../hes/if_hes_common.h"
#include "if_fci.h"
#include "logger.h"
#include "ucode.h"
#include "ucode_hdr.h"
#include "../dev/flash_dvr_spec.h"
#include "../dev/flash_spec.h"
#include "../hes/slot.h"
#include "../hes/if_vip.h"
#include "dbus.h"
#include "dbus_io.h"
#include "config.h"
#include "../snmp/chassismib_registry.h"
#include "msg_dbus.c"			/* Not a typo, see logger.h */
#include "msg_ucode.c"			/* Not a typo, see logger.h */
#include "../os/clock.h"
#include "../dev/monitor1.h"
#include "../pas/pas_id.h"

#if defined(RSP)
#include "../src-rsp/rsp_pcmap.h"
#endif
#define C7500_CHASSIS_INTERFACE_SLOT	31

#define CIP2_MIN_HWVER 0x0500

slottype slots[MAX_SLOTS];

chassis_type_t c7000_chassis_type;	/* is it clean or spinnaker or ... */

static void
dbus_write (void *p, uchar value)
{
    protected_uchar *reg;
    
    reg = p;
    reg->value = value;
}

static void
dbus_write_cbus_disabled (void *p, uchar value)
{
    protected_uchar *reg;
    dbus_protect_t state;

    dbus_protect(&state);
    reg = p;
    reg->value = value;
    dbus_restore(&state);
}
/*
 * Invoke the vectored dbus register write routine. 
 * Note: a version of this function is duplicated in the 
 * 7000 rom monitor, which writes to these 'protected' dbus
 * registers as well, but does not use the slots array.
 *
 */
void dbus_prot_wr (uint slot, protected_uchar *reg, uchar value)
{
    (*slots[slot].dbus_write)(reg, value);
}
slottype* get_slotp (int slot)
{
    return &(slots[slot]);
}

inline dgtype* pslot_to_dbus (int slot)
{
    return &((dgtype*) DGBUSSTART)[slot];
}
					 

int dbus_to_pslot (dgtype *dbus)
{
    if (dbus == dbus_rpslot_to_dbus())
	return dbus_system_rp_slot();
    return dbus - (dgtype*) DGBUSSTART;
}
/*
 * dbus_read_reg
 */
static uchar
dbus_read_reg (uint slot, volatile uchar *p)
{
    return *p;
}

/* 
 * dbus_write_reg
 */
static void
dbus_write_reg (uint slot, volatile uchar *p, uchar data)
{
    *p = data;
}

/*
 * Value returned on a DBUS read when the operation times out
 */

#define DBUS_TIMEOUT            0xff
#define DBUS_RESET_CMDFAIL      0x00
#define DBUS_MAX_RETRY_ATTEMPTS    3
static boolean
dbus_timeout_recovery (dgtype *dbus, uint slot)
{
    uchar   status;
    int     count;
    
    count = 0;
    
    do {
	count++;
	dbus->io_msg = DBUS_RESET_CMDFAIL;
	status = dbus_prot_rd(&dbus->diag_status);
    } while ((status & DBUS_INTERNAL_ERROR) &&
	     (count < DBUS_MAX_RETRY_ATTEMPTS));
    
    if (status & DBUS_INTERNAL_ERROR) {
	slots[slot].dbus_fail_recovery_count++;
	return FALSE;
    }
    return TRUE;
}

static uchar
dbus_read_reg_recovery (uint slot, volatile uchar *p)
{
    dgtype *dbus;
    uchar   status;
    uchar   data;
    boolean retry, recovery;
    int     count;
    
    dbus = pslot_to_dbus(slot);
    recovery = TRUE;
    count = 0;
    
    do {
	count++;
	data = *p;
	status = dbus_prot_rd(&dbus->diag_status);
	if (data == DBUS_TIMEOUT) {
	    retry = TRUE;	    
	    recovery = dbus_timeout_recovery(dbus, slot);
	} else {
	    /* Possible for data to be good but DBUSINTERR to be on */
	    if (status & DBUS_INTERNAL_ERROR) {
		data = DBUS_TIMEOUT;
		retry = TRUE;
		recovery = dbus_timeout_recovery(dbus, slot);
	    } else {
		retry = FALSE;
	    }
	}
    } while (retry &&
	     recovery &&
	     (count < DBUS_MAX_RETRY_ATTEMPTS));
    
    if (count > 1)
	slots[slot].dbus_rd_retry_count++;
    
    return data;
}
static void
dbus_write_reg_recovery (uint slot, volatile uchar *p, uchar data)
{
    dgtype *dbus;
    uchar   status;
    boolean retry, recovery;
    int     count;
    
    dbus = pslot_to_dbus(slot);
    recovery = TRUE;
    count = 0;
    
    do {
	count++;
	*p = data;
	status = dbus_prot_rd(&dbus->diag_status);
	if (status & DBUS_INTERNAL_ERROR) {
	    retry = TRUE;
	    recovery = dbus_timeout_recovery(dbus, slot);
	} else { 
	    retry = FALSE;
	}
    } while (retry &&
	     recovery &&
	     (count < DBUS_MAX_RETRY_ATTEMPTS));
    
    if (count > 1)
	slots[slot].dbus_wr_retry_count++;
}
uchar
dbus_io_reg_read (uint slot, uint reg)
{
    uchar data;
    dgtype *dbus;
    volatile uchar *p;
    
    dbus = pslot_to_dbus(slot);
    p = NULL;
    data = 0;
    
    switch (reg) {
    case DBUS_IO_CONSOLE:
	p = &dbus->io_console;
	break;
    case DBUS_IO_DEBUG:
	p = &dbus->io_debug;
	break;
    case DBUS_IO_MSG:
	p = &dbus->io_msg;
	break;
    case DBUS_IO_CRASH:
	p = &dbus->io_crash;
	break;
    default:
	break;
    }
    if (p)
	data = (*slots[slot].dbus_io_reg_read)(slot, p);
    return data;
}
void
dbus_io_reg_write (uint slot, uint reg, uchar data)
{
    dgtype *dbus;
    volatile uchar *p;
    
    dbus = pslot_to_dbus(slot);
    p = NULL;
    
    switch (reg) {
    case DBUS_IO_CONSOLE:
	p = &dbus->io_console;
	break;
    case DBUS_IO_DEBUG:
	p = &dbus->io_debug;
	break;
    case DBUS_IO_MSG:
	p = &dbus->io_msg;
	break;
    case DBUS_IO_CRASH:
	p = &dbus->io_crash;
	break;
    default:
	break;
    }
    if (p)
	(*slots[slot].dbus_io_reg_write)(slot, p, data);
}
uchar
dbus_io_status_reg_read (uint slot)
{
    uchar data;
    dgtype *dbus;
    volatile uchar *p;
    
    dbus = pslot_to_dbus(slot);
    p = &dbus->io_status;

    data = (*slots[slot].dbus_io_reg_read)(slot, p);
    return data;
}
void
dbus_io_status_reg_write (uint slot, uchar data)
{
    dgtype *dbus;
    volatile uchar *p;
    
    dbus = pslot_to_dbus(slot);
    p = &dbus->io_status;
    
    (*slots[slot].dbus_io_reg_write)(slot, p, data);
}

static boolean
dbus_timeout_recovery_capable (uchar ctrlr_type)
{
    switch (ctrlr_type) {
    case FCI_CIP_CONTROLLER:
	return TRUE;
    default:
	return FALSE;
    }
}

void
dbus_io_status_bit (uint slot, uchar bits, boolean state)
{
    uchar data;

    data = dbus_io_status_reg_read(slot);
    if (data == DBUS_TIMEOUT)
	return;

    switch (state) {
    case DBUS_IO_BIT_ON:
	data |= bits;
	break;
    case DBUS_IO_BIT_OFF:
	data &= ~bits;
	break;
    }
    dbus_io_status_reg_write(slot, data);
}    

/*
 * dbus_restore_io_status_bits 
 *
 *   restore the dbus io_status register bits to 
 *   reflect the existence of an active console or
 *   debug connection
 *
 *   turn on the DBUS_IO_TO_RECOVERY bit if IOS
 *   will participate with this card in the
 *   retry/recovery of dBus timeouts.
 *
 */

void dbus_restore_io_status_bits (dgtype *dbus)
{
    uint slot;
    
    slot = dbus_to_pslot(dbus);
    
    if (slots[slot].dbus_io_tty[DBUS_IO_TTY_CONSOLE] != DBUS_IO_NO_TTY)
	dbus_io_status_bit(slot, DBUS_IO_CONSOLE_ACTIVE, DBUS_IO_BIT_ON);

    if (slots[slot].dbus_io_tty[DBUS_IO_TTY_DEBUG] != DBUS_IO_NO_TTY)
	dbus_io_status_bit(slot, DBUS_IO_DEBUG_ACTIVE, DBUS_IO_BIT_ON);
    
    if (dbus_timeout_recovery_capable(slots[slot].ctrlr_type)) {
	dbus_io_status_bit(slot, DBUS_IO_TO_RECOVERY, DBUS_IO_BIT_ON);
    }
}

static inline void dbus_enabled_led_on (dgtype* dbus)
{
    uchar control;
    uint slot;
    
    slot = dbus_to_pslot(dbus);

    if (dbus_present(dbus)) {
	control = dbus_prot_rd(&dbus->diag_control);
	dbus_prot_wr(slot, &dbus->diag_control, control | ENABLE_LED);
    }
}

dgtype* dbus_rpslot_to_dbus (void)
{
    return ((dgtype*)(IDPROM_BASE-offsetof(dgtype,eeprom)));
}

/*
 * dbus_parallel_load_controller
 *
 * Returns TRUE if card operations should be performed in parallel
 */
inline boolean dbus_parallel_inline (ushort card_type)
{
    switch (card_type) {
    case FCI_SVIP_CONTROLLER:
	return TRUE;
    default:
	return FALSE;
    }
}

/*
 * dbus_swdbus_inline
 *
 * Returns TRUE if card_type has a software dbus.
 */
static inline boolean dbus_swdbus_inline (ushort card_type)
{
    switch (card_type) {
    case FCI_CIP_CONTROLLER:
    case FCI_FEIP_CONTROLLER:
    case FCI_RVIP_CONTROLLER:
    case FCI_SVIP_CONTROLLER:
    case FCI_RSP2_CONTROLLER:
    case FCI_POSIP_CONTROLLER:
	return TRUE;
    default:
	return FALSE;
    }
}

/*
 * dbus_wait_for_sw_ready
 *
 * The CIP/FEIP/RVIP/SVIP/RSP2/POSIP have dbus that is partially implemented in 
 * software.  Certain events require the RP to wait before issuing any 
 * further dbus/cbus commands.  Those events are after a dbus reset, 
 * MASTER_ENABLE, and ucode download.
 */
boolean dbus_wait_for_sw_ready (dgtype *dbus, char *event)
{
    sys_timestamp timer, poll_timer;
    boolean ok_to_suspend = process_suspends_allowed();

    GET_TIMESTAMP(poll_timer);
    GET_TIMESTAMP(timer);
    while (TRUE) {

	/* check if we are done */
	if (dbus_prot_rd(&dbus->diag_status) & DBUS_SW_READY) {
	    break;
	}

	/* check if we have timed out */
	if (ELAPSED_TIME(timer) > DBUS_SW_MAXDELAY) {
	    errmsg(&msgsym(SW_NOTRDY, DBUS), 
		   event, ELAPSED_TIME(timer),
		   dbus_prot_rd(&dbus->diag_status));
	    return(FALSE);
	}

	if (ok_to_suspend) {
	    process_sleep_for(DBUS_SW_POLL_TIME);
	} else {
	    /* wait for polling time */
	    while (ELAPSED_TIME(poll_timer) < DBUS_SW_POLL_TIME)
		;
	}
	GET_TIMESTAMP(poll_timer);
    }

#ifdef DEBUG_TIMING
    buginf("\nDBUS_SW_READY delay for %s took %d ms", event, ELAPSED_TIME(timer));
#endif
    
    return(TRUE);
}

#define DBUS_NO_RESPONSE_VAL 0xff
#define DBUS_NO_RESPONSE_TIMEOUT (3*ONESEC)

/* 
 * If this is the RP/RSP or the CI, read the controller type from the eeprom.
 * Otherwise, return the value previously retrieved from the dbus read.
 *    Returning the value from the slots array, rather than rereading it
 *    over the dbus, avoids the potential for a dbus timeout which would
 *    result in the return of an invalid controller type.
 *    Note that this field is initialized in the dbus_validate() routine
 *    which skips the RP/RSP slot(s) and is never called for the RSP chassis
 *    interface slot.
 */
static ushort get_dbus_controller_type (dgtype* dbus)
{
    uint slot;
    
    slot = dbus_to_pslot(dbus);
    if ((dbus_cpu_slot(slot)) ||
	(slot == C7500_CHASSIS_INTERFACE_SLOT))
	return(dbus->eeprom.controller_type);
    else
	return(slots[slot].ctrlr_specific);
} 
/* 
 * return controller type from the controller, and handle soft dbus funkyness.
 *
 * This routine handles the read a the dbus eprom, which  can be a problem on  
 * cards with a software dbus. The problem is the card has to be ready in 
 * order to read the dbus, but the system cannot tell is the if ready needs 
 * to be checked (ready may not be checked on a "hard" dbus card) until the 
 * erprom accessed, and it is known the card has a "soft" dbus.  Solution is
 * to assume an eeprom.controller_type of FF is a "soft" dus card, and then 
 * reset it and wait for the card to come up again.
 *
 */
static ushort
read_dbus_controller_type (uint slot, dgtype* dbus)
{
    sys_timestamp dally;
    ushort card_type;

    card_type = dbus->eeprom.controller_type; 
    if (card_type == DBUS_NO_RESPONSE_VAL){
	TIMER_START(dally, DBUS_NO_RESPONSE_TIMEOUT);
	do {
	    card_type = dbus->eeprom.controller_type;
	    if (AWAKE(dally)) {
		errmsg(&msgsym(NORESP, DBUS), dbus_to_pslot(dbus));
		return DBUS_NO_RESPONSE_VAL;
	    }
	} while (card_type == DBUS_NO_RESPONSE_VAL);
	/*
	 * Clear the DBUSINTERR error bit which was set when the
	 * dbus read timed out 
	 */
	dbus_write_cbus_disabled(&dbus->diag_status, DBUS_RESET_VALUE);	/* reset! */
	/*	
	 * MASTER_ENABLE was wrong to do here because after a reset
	 * there is no microcode to start :-)
	 */
	if (dbus_swdbus_inline(card_type)) {
	    dbus_wait_for_sw_ready(dbus, "dbus_controller_type()");
	    dbus_restore_io_status_bits(dbus);
	}
    }
    return card_type;
}

/*
 * dbus_io_regs
 *
 * Used to identify those controllers which support dbus i/o registers
 */
static inline boolean dbus_io_regs (uchar ctrlr_type)
{
    switch (ctrlr_type) {
    case FCI_CIP_CONTROLLER:
    case FCI_FEIP_CONTROLLER:
    case FCI_RVIP_CONTROLLER:
    case FCI_SVIP_CONTROLLER:
    case FCI_POSIP_CONTROLLER:
	return TRUE;
    default:
	return FALSE;
    }
}
 

/*
 * slot_enabled_led_on
 * Turns on board enabled LED for a given slot.
 * (LED is turned off by reset)
 */
void slot_enabled_led_on (uint slot)
{
    dgtype *dbus;
    
    dbus = pslot_to_dbus(slot);
    dbus_enabled_led_on(dbus);
}

/*
 * worker routine for dbus_ctrlr_start
 */
static inline uchar dbus_slot_enable_start (dgtype* dbus)
{
    uchar control;
    uchar card_type = DBUS_NO_RESPONSE_VAL;
    uint slot;

    slot = dbus_to_pslot(dbus);

    if (dbus_present(dbus)) {
	card_type = dbus_controller_type(dbus);
	control = dbus_prot_rd(&dbus->diag_control);
	dbus_prot_wr(slot, &dbus->diag_control,
		     control | MASTER_ENABLE | WCS_LOADED);
    }
    return card_type;
}

/*
 * worker routine for dbus_ctrlr_start
 */
inline void dbus_slot_enable_finish (dgtype* dbus, uchar card_type)
{
    if (dbus_present(dbus)) {
	if (dbus_swdbus_inline(card_type)) {
	    /*
	     * Delay 1 ms to cover up for VIP2 rom monitor bug - it erroneously
	     * sets "Ready" in the status register in response to the write
	     * of the control register, then clears ready 3us later
	     */
            DELAY(1);

	    if (dbus_wait_for_sw_ready(dbus, "dbus_slot_enable()")) {
		dbus_restore_io_status_bits(dbus);
	    }
	}
    }
}
    
/*
 * dbus_slot_enable
 * Sets WCS_LOADED and MASTER_ENABLE bits in dbus controller,
 * starting card without reloading microcode
 */
inline uchar dbus_slot_enable (dgtype* dbus)
{
    uchar card_type;

    card_type = dbus_slot_enable_start (dbus);
    dbus_slot_enable_finish (dbus, card_type);
    return card_type;
}

static inline void dbus_set_loaded (dgtype* dbus)
{
    uint slot;
    
    slot = dbus_to_pslot(dbus);
    dbus_prot_wr(slot, &dbus->diag_control, WCS_LOADED);
}

/*
 * dbus_xilinx_reload
 * issue command to reload VPLD parts on TRIP, FIP
 */

void dbus_xilinx_reload (dgtype* dbus, boolean wait) 
{
    int i = DBUS_XILINX_TIME;
    uint slot;
    
    slot = dbus_to_pslot(dbus);

    switch (dbus_controller_type(dbus)) {
    case FCI_ETHER_CONTROLLER:
    case FCI_SP_CONTROLLER:
    case FCI_LINK_CONTROLLER:
    case FCI_SERIAL_CONTROLLER:
    case FCI_MIP_CONTROLLER:
    case FCI_CIP_CONTROLLER:
    case FCI_FEIP_CONTROLLER:
    case FCI_RVIP_CONTROLLER:
    case FCI_SVIP_CONTROLLER:
    case FCI_RSP2_CONTROLLER:
    case FCI_POSIP_CONTROLLER:
	return;			/* don't do anything for these */
    case FCI_FDDIT_CONTROLLER:
    case FCI_SSP_CONTROLLER:
    case FCI_CTR_CONTROLLER:
    case FCI_FSIP_CONTROLLER:
    case FCI_AIP_CONTROLLER:
    default:
	break;			/* reload parts for others */
    }

    dbus_prot_wr(slot, &dbus->diag_control, WCS_LOADED);
    dbus_prot_wr(slot, &dbus->last_command_or_address, 0);
    DELAY(2);		/* allow for pulse stretching by dbus XILINX */
    if (wait) {
	while ((dbus->CxBus_and_logical_slot & XILINX_READY) == 0) {
	    DELAY(1);
	    if (--i == 0) {
		if ((dbus->CxBus_and_logical_slot & XILINX_READY) == 0)
		    errmsg(&msgsym(XILINX, DBUS), dbus->slot_and_comp >> 4);
		return;		/* stop waiting, it should be done by now */
	    }
	}
#ifdef DEBUG_TIMING
	ttyprintf(CONTTY, "\ndbus_xilinx_reload (0x%x) counted down %d",
		  dbus, DBUS_XILINX_TIME - i);
#endif
    }
}

/*
 * dbus_wcs_loaded
 * report whether WCS_LOADED is set
 */

static boolean dbus_wcs_loaded (dgtype* dbus)
{
    if (dbus_prot_rd(&dbus->diag_control) & WCS_ERROR_MASK)
	return(FALSE);
    return ((dbus_prot_rd(&dbus->diag_control) & WCS_LOADED) ? TRUE : FALSE);
}

/*
 * worker routine for dbus_ctrlr_reset or for dbus_load_all_specific
 */
static inline ushort dbus_soft_reset_start (dgtype* dbus)
{
    int slot;
    ushort card_type;

    card_type = dbus_controller_type(dbus);
    slot = ((dbus->slot_and_comp>>4) & 0xf);
    if (dbus_io_regs(card_type))
	while (dbus_check_msg(dbus, slot))
	    DELAY(DBUS_LOGGER_DELAY);
    if (card_type == FCI_RSP2_CONTROLLER){
	dbus_write_cbus_disabled(&dbus->diag_status, DBUS_RESET_VALUE);	/* reset! */
    } else {
	dbus_set_loaded (dbus);	/* avoid_loading ROM */
	dbus_write_cbus_disabled(&dbus->diag_status, DBUS_RESET_VALUE);
    }
    return card_type;
}

/*
 * worker routine for dbus_ctrlr_reset or for dbus_load_all_specific
 */
static inline int dbus_compute_soft_reset_delay (uchar card_type, dgtype* dbus)
{
    if (card_type == FCI_RSP2_CONTROLLER)
	return ONESEC;
    else
	return 0;
}
    
/*
 * worker routine for dbus_ctrlr_reset or for dbus_load_all_specific
 */
static inline void dbus_soft_reset_finish (uchar card_type, dgtype* dbus)
{
    if (dbus_swdbus_inline(card_type) ||
	(card_type == DBUS_NO_RESPONSE_VAL)) {
	dbus_wait_for_sw_ready(dbus, "RESET");
    }
}

/*
 * dbus_ctrlr_reset
 * issue board reset via dbus controller
 */

inline void dbus_ctrlr_reset (dgtype* dbus)
{
    ushort card_type;

    if (dbus_present(dbus)) {
	card_type = dbus_soft_reset_start (dbus);
	DELAY(dbus_compute_soft_reset_delay (card_type, dbus));
	dbus_soft_reset_finish (card_type, dbus);
    }
}

/*
 * worker routine for dbus_hard_ctrlr_reset or dbus_load_all_specific
 */
static inline ushort dbus_hard_reset_start (dgtype* dbus)
{
    int slot;
    ushort card_type;

    slot = dbus_to_pslot(dbus);
    card_type = read_dbus_controller_type(slot, dbus);

    /* wait for SWDBUS interfaces to be ready */
    if ((card_type != DBUS_NO_RESPONSE_VAL) &&
        (dbus_prot_rd(&dbus->diag_status) & DBUS_SW_READY)) {
        slot = ((dbus->slot_and_comp>>4) & 0xf);
        if (dbus_io_regs(card_type))
            while (dbus_check_msg(dbus, slot))
                DELAY(DBUS_LOGGER_DELAY);
    }
    if( card_type == FCI_RSP2_CONTROLLER ){ 
	dbus_write_cbus_disabled(&dbus->diag_status, DBUS_HARD_REG_RESET);
    } else {
	dbus_set_loaded (dbus); /* avoid_loading ROM */
	dbus_write_cbus_disabled(&dbus->diag_status, DBUS_HARD_REG_RESET);
    }
    return card_type;
}

/*
 * worker routine for dbus_hard_ctrlr_reset or dbus_load_all_specific
 */
static inline int dbus_compute_hard_reset_delay (uchar card_type, dgtype* dbus)
{
    /* memory init delays required before microcode load for specific IPs */
    if (card_type == FCI_RSP2_CONTROLLER)
	return ONESEC;

    /* the following delay is good for 32mb with the rommon on these IPs */
    else if ((card_type == FCI_RVIP_CONTROLLER) ||
	     (card_type == FCI_FEIP_CONTROLLER) ||
	     (card_type == FCI_POSIP_CONTROLLER))
	return 2 * ONESEC;
    else 
	return 0;
}

/*
 * worker routine for dbus_hard_ctrlr_reset or dbus_load_all_specific
 */
static inline void dbus_hard_reset_finish (uchar card_type, dgtype* dbus)
{
    if ((dbus_swdbus_inline (card_type)) ||
	(card_type == DBUS_NO_RESPONSE_VAL)) {
	dbus_wait_for_sw_ready (dbus, "HARD_RESET");
    }
}

inline void dbus_hard_ctrlr_reset (dgtype* dbus)
{
    ushort card_type;

    card_type = dbus_hard_reset_start (dbus);
    DELAY(dbus_compute_hard_reset_delay (card_type, dbus));
    dbus_hard_reset_finish (card_type, dbus);
}

/*
 * dbus_slot_reset
 * reset the board in *physical* slot
 */
void dbus_slot_reset (uint pslot) 
{
    dgtype *dbus;
    
    dbus = pslot_to_dbus(pslot);
    dbus_ctrlr_reset(dbus);
}

/*
 * worker routine for dbus_ctrlr_start
 */
static uchar dbus_ctrlr_start_common (dgtype * dbus, uchar card_type, boolean wait)
{
    if (!dbus_swdbus_inline(card_type)) {
	/*
	 * These resets are extraneous for swdbus cards and result in subsystem
	 * initialization running again.
	 */
	dbus_xilinx_reload(dbus, TRUE /* wait for xilinx to come up */);
	dbus_ctrlr_reset(dbus);
    }
    if (wait) return dbus_slot_enable(dbus);
    else return dbus_slot_enable_start(dbus);
}

/*
 * dbus_ctrlr_start
 *
 * Routine starts the card.
 * Performs xilinx and controller resets on non-swdbus cards.
 */
uchar dbus_ctrlr_start (dgtype* dbus, uchar card_type)
{
    return dbus_ctrlr_start_common (dbus, card_type, TRUE /* perform waits to completion */);
}

/*
 * worker routine for dbus_slot_enable, to start controller
 */
uchar dbus_ctrlr_start_nowaited (dgtype* dbus, uchar card_type)
{
    return dbus_ctrlr_start_common (dbus, card_type, FALSE /* return after operation started */);
}

/*
 * worker routine for cbus_analyze, to wait for controller to be started
 */
void dbus_ctrlr_start_nowaited_finish (dgtype* dbus, uchar card_type)
{
    dbus_slot_enable_finish (dbus, card_type);
}

boolean slot_enabled (uint slot) 
{
    if (slots[slot].flags & (BOARD_DISABLED|BOARD_NONEXIST))
	return FALSE;
    else
	return TRUE;
}

/*
 * return TRUE if this physical slot contains a CxBus agent, FALSE otherwise
 */

boolean cxbus_card_present (uint pslot) 
{
    boolean result = FALSE;

    switch (slots[pslot].ctrlr_type) {
    case FCI_ETHER_CONTROLLER:
    case FCI_LINK_CONTROLLER:
    case FCI_CTR_CONTROLLER:
    case FCI_FDDIT_CONTROLLER:
    case FCI_SP_CONTROLLER:
    case FCI_SSP_CONTROLLER:
    case FCI_FSIP_CONTROLLER:
    case FCI_MIP_CONTROLLER:
    case FCI_AIP_CONTROLLER:
    case FCI_CIP_CONTROLLER:
    case FCI_FEIP_CONTROLLER:
    case FCI_RVIP_CONTROLLER:
    case FCI_SVIP_CONTROLLER:
    case FCI_RSP2_CONTROLLER:
    case FCI_POSIP_CONTROLLER:
	result = TRUE;
	break;
    case FCI_SERIAL_CONTROLLER:
    default:
	result = FALSE;
    }
    return(result);
}

/*
 * return TRUE if this physical slot contains a CxBus agent
 * which is IPC capable, FALSE otherwise
 */
 
boolean cxbus_ipc_capable (uint pslot)
{
    boolean result = FALSE;
 
    switch (slots[pslot].ctrlr_type) {
    case FCI_CIP_CONTROLLER:
        result = TRUE;
        break;
    default:
        result = FALSE;
    }
    return(result);
}
 
/*
 * return TRUE if this physical slot contains any sort of RSP.
 */
 
boolean rsp_present (uint pslot)
{
    uchar card_type;
    dgtype * dbus;
    boolean result = FALSE;

    dbus = pslot_to_dbus(pslot);
    if (dbus_present(dbus)) {
	card_type = dbus_controller_type(dbus);
	if (card_type == FCI_RSP1_CONTROLLER ||
	    card_type == FCI_RSP2_CONTROLLER) {
	    result = TRUE;
	}
    }
    return(result);
}
 
/* 
 * return TRUE if this physical slot contains a switch processor
 */

inline boolean switchproc_present (dgtype* dbus)
{
    uchar card_type;
    
    card_type = dbus_controller_type(dbus);
    if (card_type == FCI_SP_CONTROLLER ||
	card_type == FCI_SSP_CONTROLLER)
	return TRUE;
    if (dbus->eeprom.hardware_version_high == SP_HARDWARE)
	return TRUE;
    if (dbus->eeprom.hardware_version_high == SSP_HARDWARE)
	return TRUE;
    return FALSE;
}

/*
 * return TRUE if this physical slot contains a multibus interface processor
 */
inline boolean swbus_present (dgtype* dbus) {
    uchar card_type;

    card_type = dbus_controller_type(dbus);
    if (card_type == FCI_SERIAL_CONTROLLER)
	return TRUE;
    else
	return FALSE;
}
boolean download_installed_microcode (dgtype* dbus, boolean wait) 
{
    int pslot = dbus_to_pslot(dbus);

    /*
     * Check if the card_type supports loading
     * operational code from ROM.
     */
    if (dbus_no_rom_ucode(slots[pslot].ctrlr_type)) {
	errmsg(&msgsym(NOROM, UCODE), pslot);
	return FALSE;
    }

    /*
     * To work around a bug in the dbus Xilinx, we first set WCS_LOADED,
     * issue the reset, then clear WCS_LOADED.  If we simply clear WCS_LOADED
     * and issue a reset, we may get a spurious WCS_CTRLR_ERR failure.
     * Don't issue reset for IP's with software dbus.  It appears that
     * this change could be made for all IP's but the consensus was we
     * wanted to limit exposure in the field with a change like the
     * following. 
     */
    if (!dbus_swdbus_inline(slots[pslot].ctrlr_type)) {
	dbus_prot_wr(pslot, &dbus->diag_control, WCS_LOADED);
	dbus_prot_wr(pslot, &dbus->diag_status, DBUS_RESET_VALUE); /* issue RESET */
    }
    dbus_prot_wr(pslot, &dbus->diag_control, 0);	/* now clear WCS_LOADED */

    if (wait) {
	DELAY(DBUS_ROM_LOAD_TIME); 
	if (!dbus_wcs_loaded(dbus)) {
	    if (dbus_prot_rd(&dbus->diag_status) & WCS_ERROR_MASK) {
		dbus_prot_wr(pslot, &dbus->diag_control, WCS_LOADED);
		dbus_prot_wr(pslot, &dbus->diag_status, DBUS_RESET_VALUE);
		dbus_prot_wr(pslot, &dbus->diag_control, 0);
	    }
	    DELAY(DBUS_ROM_LOAD_TIME);
	    if (!dbus_wcs_loaded(dbus))
		dbus_set_loaded(dbus); /* we'll catch broken card later */
	}
    }
    return TRUE;
}


ushort dbus_hw_version (dgtype* dbus)
{
    return ((ushort)dbus->eeprom.hardware_version_high << 8 |
	    (ushort)dbus->eeprom.hardware_version_low);
}

ushort dbus_hw_revision (dgtype* dbus)
{
    return (dbus->eeprom.board_revision);
}

/*
 * return controller type.
 * return DBUS_NO_RESPONSE_VAL if we don't want a board to be recognized.
 * If srswitch is not enabled we don't want srs fip or srs trip
 * be recognized.
 * If srswitch is enabled we allow only  trip, srs trip, fip and srs fip.
 */

ushort dbus_controller_type (dgtype* dbus)
{
    ushort card_type;

    card_type = get_dbus_controller_type(dbus);

    if (card_type >= FCI_NUM_CONTROLLERS)  {
	errmsg(&msgsym(BADCTRLRTYPE, DBUS), card_type, dbus_to_pslot(dbus));
	return (0xff);
    }
    if (srswitch_enable) {
	switch (card_type) {
	case FCI_CTR_SRS_CONTROLLER:
	case FCI_CTR_CONTROLLER:
	    return (FCI_CTR_CONTROLLER);
	case FCI_FDDIT_SRS_CONTROLLER:
	case FCI_FDDIT_CONTROLLER:
	    return(FCI_FDDIT_CONTROLLER);
	case FCI_SSP_CONTROLLER:
	case FCI_SP_CONTROLLER:
	case FCI_RP_CONTROLLER:
	    return (card_type);
	default:
	    return (DBUS_NO_RESPONSE_VAL);
	}
    }
    if (card_type == FCI_CTR_SRS_CONTROLLER ||
	card_type == FCI_FDDIT_SRS_CONTROLLER) {
	return (DBUS_NO_RESPONSE_VAL);
    }

    if (card_type == FCI_SSIP_CONTROLLER)
      return(FCI_FSIP_CONTROLLER);
    if (card_type == FCI_SMIP_CONTROLLER)
      return(FCI_MIP_CONTROLLER);
    return card_type;
}
/*
 * return TRUE if a board is srs trip or srs fip
 */

boolean srswitch_pslot (int pslot)
{
    dgtype* dbus = pslot_to_dbus(pslot);
    ushort card_type = get_dbus_controller_type(dbus);
    
    if (card_type == FCI_FDDIT_SRS_CONTROLLER ||
	card_type == FCI_CTR_SRS_CONTROLLER) {
	return(TRUE);
    }
    return(FALSE);
}

/*
 * return TRUE if a board is SSIP or SMIP
 */

boolean ssip_smip_pslot (int pslot)
{
    dgtype* dbus = pslot_to_dbus(pslot);
    ushort card_type = get_dbus_controller_type(dbus);
    
    if (card_type == FCI_SSIP_CONTROLLER ||
	card_type == FCI_SMIP_CONTROLLER)
	return(TRUE);
    return(FALSE);
}

boolean is_cip2_hw (int slot)
{
    slottype* slotp;
    ushort    hw_ver;
    
    slotp = &slots[slot];
    if (slotp->ctrlr_type == FCI_CIP_CONTROLLER) {
	hw_ver = slotp->hw_version;
	if (hw_ver >= CIP2_MIN_HWVER) {
	    return(TRUE);
	}
    }
    return(FALSE);
}

ulong dbus_serial_number (dgtype* dbus)
{
    return (dbus->eeprom.serial_number_31_24 << 24 |
	    dbus->eeprom.serial_number_23_16 << 16 |
	    dbus->eeprom.serial_number_15_8 << 8 |
	    dbus->eeprom.serial_number_7_0);
}

/*
 * verify_and_get_card_type
 * checks to make sure eeprom contents look good
 * returns TRUE if okay, returns FALSE (and scolds) if not
 */
static boolean verify_and_get_card_type (int slot, ushort* pcard_type) 
{
    dgtype* dbus = pslot_to_dbus(slot);
    ushort card_type = *pcard_type = dbus_controller_type(dbus);

    if ((card_type == DBUS_NO_RESPONSE_VAL) || 
	(card_type == 0 &&
	 (dbus->eeprom.hardware_version_high == SP_HARDWARE ||
	  dbus->eeprom.hardware_version_high == SSP_HARDWARE))) {
	errmsg(&msgsym(BADEEPROM, DBUS), slot);
	return(FALSE);
    }
    return(TRUE);
}

/*
 * name a controller type for a card found in a chassis slot
 * or a bundled microcode image
 */
char* card_type_to_str (ushort card_type, ushort hw_ver)
{
    switch ((ctrlr_type_t) card_type) {
    case FCI_SP_CONTROLLER:	if (hw_ver >> 8 != SSP_HARDWARE)
				    return "SP"; 
    case FCI_SSP_CONTROLLER:	return "SSP";
    case FCI_ETHER_CONTROLLER:	return "EIP";
    case FCI_SERIAL_CONTROLLER:	return "SIP";
    case FCI_LINK_CONTROLLER:	return "HIP";
    case FCI_CTR_CONTROLLER:	return "TRIP";
    case FCI_FDDIT_CONTROLLER:	return "FIP";
    case FCI_FSIP_CONTROLLER:	return "FSIP";
    case FCI_AIP_CONTROLLER:	return "AIP";
    case FCI_MIP_CONTROLLER:	return "MIP";
    case FCI_CIP_CONTROLLER:
	if (hw_ver >= CIP2_MIN_HWVER)
	    return "CIP2";
	return "CIP";
    case FCI_RSP1_CONTROLLER:	return "RSP1";
    case FCI_C5IP_CONTROLLER:   return "C5IP";
    case FCI_C5RSP2_CONTROLLER:
    case FCI_RSP2_CONTROLLER:	return "RSP2";
    case FCI_FEIP_CONTROLLER:	return "FEIP";
    case FCI_RVIP_CONTROLLER:	return "VIP";
    case FCI_SVIP_CONTROLLER:	return "VIP2";
    case FCI_RSP7000_CONTROLLER:return "RSP7000";
    case FCI_POSIP_CONTROLLER: return "POSIP";

    case FCI_UNKNOWN_CONTROLLER:	/* types not found in slots */
    case FCI_FDDI_CONTROLLER:		/* or bundles */
    case FCI_RP_CONTROLLER:
    case FCI_CI_CONTROLLER:
    case FCI_CI7000_CONTROLLER:
    case FCI_FDDIT_SRS_CONTROLLER:
    case FCI_CTR_SRS_CONTROLLER:
    case FCI_SSIP_CONTROLLER:
    case FCI_SMIP_CONTROLLER:
    case FCI_RSP4_CONTROLLER:
    case FCI_NUM_CONTROLLERS:
      break;
    }
    return "Unknown";
}

void
dbus_reset_recovery_counters (uint slot)
{
    slottype* slotp;
    
    slotp = &slots[slot];
    
    slotp->dbus_rd_retry_count = 0;
    slotp->dbus_wr_retry_count = 0;
    slotp->dbus_fail_recovery_count = 0;
}
/*
 * Set default vectors
 */
static void
dbus_reset_slot_vectors (uint slot)
{
    slottype* slotp;

    slotp = &slots[slot];
    
    slotp->dbus_write = dbus_write_cbus_disabled;
    
    slotp->dbus_io_reg_read = dbus_read_reg;
    slotp->dbus_io_reg_write = dbus_write_reg;

    dbus_reset_recovery_counters(slot);
}
/*
 * Override default vectors for card types that have overrides
 */
static void
dbus_set_slot_vectors (uint slot)
{
    slottype* slotp;

    slotp = &slots[slot];

    if (slotp->swdbus)
	slotp->dbus_write = dbus_write;
    else
	slotp->dbus_write = dbus_write_cbus_disabled;

    if (dbus_timeout_recovery_capable(slotp->ctrlr_type)) {
	slotp->dbus_io_reg_read = dbus_read_reg_recovery;
	slotp->dbus_io_reg_write = dbus_write_reg_recovery;
    }
}
static boolean
verify_driver_support (slottype *slotp)
{
    boolean result;
    
    switch (slotp->ctrlr_type) {
    case FCI_CIP_CONTROLLER:
	if (subsys_find_by_name("cip"))
	    result = TRUE;
	else
	    result = FALSE;
	break;
    default:
	result = TRUE;
    }

    if (!result) {
	errmsg(&msgsym(NOSUPPORT, DBUS),
	       slotp->card_name, slotp->slot_num, slotp->ctrlr_type);
    }
    return(result);

}

/*
 * return TRUE if a given slot appears to contain a well-formed IP or RSP
 */
boolean dbus_validate (uint slot_num)
{
    dgtype* dgbus;
    slottype* slotp;
    ushort card_type;
    uchar slot_and_complement;

    slotp = &slots[slot_num];
    slotp->flags |= BOARD_NONEXIST;
    slotp->consecutive_errs = 0;

    dgbus = pslot_to_dbus(slot_num);
    if (!dbus_present(dgbus))
	return FALSE;

    slot_and_complement = dgbus->slot_and_comp;
    if ((slot_num != (slot_and_complement >> 4 & 0xf)) ||
	((slot_and_complement >> 4 & 0xf) !=
	 (~slot_and_complement & 0xf))) {
	errmsg(&msgsym(SLOTCOMP, DBUS), slot_num,
	       slot_and_complement>>4 & 0xf,
	       ~slot_and_complement & 0xf);
	return FALSE;
    }
    slotp->flags &= ~BOARD_NONEXIST;
    slotp->mem_size = 0;
    slotp->io_size = 0;
    *slotp->sw_version_str = '\0';

    if (!TIMER_RUNNING(slotp->insert_time))
	GET_TIMESTAMP(slotp->insert_time);

    slotp->ctrlr_specific = read_dbus_controller_type(slot_num, dgbus);

    if (!verify_and_get_card_type(slot_num, &card_type)) {
	slotp->flags |= (BOARD_DISABLED | BOARD_INVALID);
	return FALSE;
    }
    slotp->flags &= ~(BOARD_DISABLED | BOARD_INVALID);

    slotp->ctrlr_type = card_type;

    slotp->serial_number = dbus_serial_number(dgbus);
    slotp->hw_version = dbus_hw_version(dgbus);

    slotp->swdbus = dbus_swdbus_inline(slotp->ctrlr_specific);

    dbus_set_slot_vectors(slot_num);
    
    strcpy(slotp->card_name,
	   card_type_to_str(card_type, slotp->hw_version));
    slotp->io_regs = dbus_io_regs(slotp->ctrlr_type);
    sprintf(slotp->msg_name, "%s%d", slotp->card_name, slot_num);

    if (!verify_driver_support(slotp)) {
	slotp->flags |= BOARD_DISABLED;
	slotp->ucode_source = UCODE_NONE;
	return FALSE;
    }

    switch (card_type) {
      case FCI_RSP2_CONTROLLER:
	/* 
	 * we have just discovered a second rsp, therefore we are a 
	 * HSA system, and  need a minimum ROMMON version to be viable.  
	 * note that we are fussing with master eproms here, and cannot get
	 * to the slave version.
	 */
	if (mon_version() < 2817) {
	    /* 
	     * 2817 is the first fully working version of HSA proms. 
	     * This is formed by ROM_MAJORVERSION 11*256
	     * plus ROM_MINORVERSION 1  (11.1)
	     * or HEX  0x601 (decimal 11 shifted left two hex digits plus 1)
	     */
	    errmsg(&msgsym(BADHSACONF, DBUS));
	    crashdump(0);
	}

	return TRUE;

      default:
	break;
    }

    dgbus->CxBus_and_logical_slot = slot_num; /* set logical slot!!! */

    return TRUE;
}


/*
 * mark_removed_cards
 * Scans the dbus for cards that were present and if the cards were removed
 * marks them as non-exsistent.
 * Called from hotswap.c
 */
void mark_removed_cards (void)
{
    int indx;
    dgtype* dgbus;

    for (indx = 0; indx < MAX_SLOTS; indx++) {

	dgbus = pslot_to_dbus(indx);

	/* if the card was present; see if its still present */
	if ((slots[indx].flags & BOARD_NONEXIST) == 0) {
	    if (!dbus_present(dgbus)) {
		slots[indx].flags |= BOARD_NONEXIST;
		if (oir_debug)
		    buginf("\nmark_removed_cards: marking slot %d NONEXIST",
			   indx);
	    }
	}
    }
}

/*
 * worker routine for dgbus_analyze. performs dbus hard reset on all
 * controllers in parallel.
 */
static void dbus_hard_reset_all_controllers (void)
{
    int indx;
    dgtype *dgbus;
    sys_timestamp etime; /* elapsed */
    sys_timestamp stime; /* subtime */
    uchar card_types[MAX_SLOTS];
    boolean dbus_is_present[MAX_SLOTS];
    ulong delay;
    
    /*
     * hard reset: collect card information
     * and calculate maximum required delay time
     */
    GET_TIMESTAMP (etime);
    delay = 0;
    for (indx = 0; indx < MAX_SLOTS; indx++) {
	dgbus = pslot_to_dbus(indx);
	if (dbus_present(dgbus)) {
	    card_types[indx] = dbus_hard_reset_start (dgbus);
	    delay = max(delay, dbus_compute_hard_reset_delay(card_types[indx],dgbus));
	    dbus_is_present[indx] = TRUE;
#ifdef DEBUG_PARALLEL
	    ttyprintf (CONTTY, "slot %d controller type %d\n",indx,card_types[indx]);
#endif
	} else {
	    dbus_is_present[indx] = FALSE;
	}
    }	
#ifdef DEBUG_PARALLEL
    ttyprintf (CONTTY, "hard reset start time took %d ms\n",ELAPSED_TIME(etime));
#endif

    /*
     * hard reset: delay for maximum required time
     */
    GET_TIMESTAMP (etime);
    DELAY (delay);
#ifdef DEBUG_PARALLEL
    ttyprintf (CONTTY, "hard reset delay, %d secs took %d ms\n", delay,
	     ELAPSED_TIME(etime));
#endif

    /*
     * hard reset: wait for all DBUS and SWDBUS readys
     */
    GET_TIMESTAMP (etime);
    for (indx = 0; indx < MAX_SLOTS; indx++) {
	dgbus = pslot_to_dbus(indx);
	if (dbus_is_present[indx]) {
	    GET_TIMESTAMP(stime);
	    dbus_hard_reset_finish(card_types[indx], dgbus);
#ifdef DEBUG_PARALLEL
	    ttyprintf (CONTTY, "DBUS ready for slot %d, took %d ms\n",
		     indx, ELAPSED_TIME(stime));
#endif
	}
    }
#ifdef DEBUG_PARALLEL
    ttyprintf (CONTTY, "hard reset wait for DBUS ready took %d ms\n",ELAPSED_TIME(etime));
#endif

}

/*
 * Worker routine for dbus_load_all_specific. Used to speed up loading
 * ucode to specific devices. All specific cards are soft reset in parallel and
 * specific workarounds are performed to match work done in 
 * downlaod_microcode_from_ram. Array of validated devices determined by
 * dgbus_analyze is passed in.
 */
void dbus_soft_reset_all_specific_controllers (boolean validated[])
{
    int indx;
    dgtype *dgbus;
    sys_timestamp etime; /* elapsed */
    sys_timestamp stime; /* subtime */
    uchar card_types[MAX_SLOTS];
    ulong delay;

    /*
     * SWDBUS soft reset: collect card information 
     */
    GET_TIMESTAMP (etime);
    for (indx = 0; indx < MAX_SLOTS; indx++) {
	dgbus = pslot_to_dbus(indx);
	if (!dbus_parallel_inline(slots[indx].ctrlr_type)) continue;
	if (!validated[indx] || !dbus_present(dgbus)) continue;
	card_types[indx] = dbus_soft_reset_start (dgbus);
#ifdef DEBUG_PARALLEL
	ttyprintf (CONTTY, "soft reset start slot %d controller type %d\n",indx,card_types[indx]);
#endif
    }	
#ifdef DEBUG_PARALLEL
    ttyprintf (CONTTY, "soft reset start time took %d ms\n",ELAPSED_TIME(etime));
#endif
    
    /*
     * SWDBUS soft reset: compute maximum delay, if any 
     */
    GET_TIMESTAMP (etime);
    delay = 0;
    for (indx = 0; indx < MAX_SLOTS; indx++) {
	if (!dbus_parallel_inline(slots[indx].ctrlr_type)) continue;
	if (!validated[indx]) continue;
#ifdef DEBUG_PARALLEL
	ttyprintf(CONTTY,"Checking delays for slot %d\n",indx);
#endif
	dgbus = pslot_to_dbus(indx);
	delay = max (delay, dbus_compute_soft_reset_delay (card_types[indx], dgbus));
    }
    DELAY (delay);
#ifdef DEBUG_PARALLEL
    ttyprintf (CONTTY, "soft reset delay, %d secs took %d ms\n", delay,
	     ELAPSED_TIME(etime));
#endif
    
    /*
     * SWDBUS soft reset: wait for all SWDBUS readys & perform Xilinx workaround
     */
    GET_TIMESTAMP (etime);
    for (indx = 0; indx < MAX_SLOTS; indx++) {
	dgbus = pslot_to_dbus(indx);
	if (!dbus_parallel_inline(slots[indx].ctrlr_type)) continue;
	if (!validated[indx] || !dbus_present(dgbus)) continue;
	GET_TIMESTAMP(stime);
	dbus_soft_reset_finish(card_types[indx], dgbus);
#ifdef DEBUG_PARALLEL
	ttyprintf (CONTTY, "SWDBUS ready for slot %d, took %d ms\n",
		 indx, ELAPSED_TIME(stime));
#endif

	/*
	 * Some dbus Xilinx's have broken reset logic and stretch the
	 * reset pulse too long
	 */
	DELAY(1);
	dbus_prot_wr(indx, &dgbus->diag_control, WCS_DBUS_CMD_ENABLE);
    }
#ifdef DEBUG_PARALLEL
    ttyprintf (CONTTY, "soft reset waits took %d ms\n",ELAPSED_TIME(etime));
#endif
}


/*
 * dgbus_analyze
 * Discover and initialize the dBus devices
 * Called from init_68k.c and rsp_if.c
 */
void dgbus_analyze (boolean eoir_active)
{
    int indx;
    dgtype *dgbus;
    boolean ucode_loaded;
    int sp_slot = 0;
    sys_timestamp etime; /* elapsed */
    sys_timestamp ttime; /* total */
    boolean validated[MAX_SLOTS]; /* validated controllers */
    
    if (oir_debug)
	buginf("\neoir_active is %s", eoir_active ? "TRUE" : "FALSE");

/*
 * For now, the logical slots will be set to the value of the physical slots.
 * Note: that the logical slots are part of the generation of sbus (multibus)
 * addresses.  So with log' = phy' easy access via the diag bus can be done
 * by transforming the sbus address into a diag address.
 * Also the assignment of logical slots must be done prior to the invocation
 * of mci_analyze.
 *
 * This code accesses all cards on the diag bus and for all those associated
 * with the "assumed" CxBus sets their logical slots to the value of their
 * physical slots and resets them.  The SP only is released from reset.  This
 * allows the SP to respond to "probe" commands from the RP.  
 *
 * This code does not protect against the possibility of multiple cards on 
 * the same CxBus reporting type SP.
 */
#ifdef DEBUG_PARALLEL
    ttyprintf(CONTTY,"dgbus_analyze starting\n");
#endif
    GET_TIMESTAMP (ttime);
#ifdef DEBUG_PARALLEL
    if (eoir_active) ttyprintf (CONTTY, "\nEOIR active TRUE\n");
    if (!eoir_active) ttyprintf (CONTTY, "\nEOIR active FALSE\n");
#endif
    for (indx = 0; indx < MAX_SLOTS; indx++) {
	if (slots[indx].flags & BOARD_NONEXIST) {
	    TIMER_STOP(slots[indx].insert_time);
	    slots[indx].serial_number = UNKNOWN_SERIAL_NUMBER;
	    slots[indx].ucode_loaded = FALSE;

	    dbus_reset_slot_vectors(indx);
	} 
    }

    /* perform hard reset on ALL controllers */
    if (!eoir_active) {
	dbus_hard_reset_all_controllers();
    }

    /* check state of all dbus devices present */
    for (indx = 0; indx < MAX_SLOTS; indx++) {
	ucode_loaded = slots[indx].ucode_loaded; /* save value for later  */
	slots[indx].ucode_loaded = FALSE;
	validated[indx] = dbus_validate(indx);
	if (!validated[indx]) continue;

	/* once we know this card is here, restore its ucode_loaded state */
	slots[indx].ucode_loaded = ucode_loaded;
    }

    if (!eoir_active) {

	/*
	 * load all specific controllers where we
	 * overlap the overhead in parallel
	 */
	dbus_load_ucode_all_specific (validated);

	/*
	 * load all other controllers in standard fashion
	 */
	for (indx = 0; indx < MAX_SLOTS; indx++) {
	    if (!validated[indx]) continue;	
	    dgbus = pslot_to_dbus(indx);
	    if (!dbus_parallel_inline(slots[indx].ctrlr_type)) {
		GET_TIMESTAMP(etime);
		dbus_load_ucode(indx); /* load microcode normally */
#ifdef DEBUG_PARALLEL
		ttyprintf (CONTTY, "waited download slot %d, took %d ms\n",
			 indx, ELAPSED_TIME(etime));
#endif
		dbus_reset_recovery_counters(indx);
		if (switchproc_present(dgbus)) {
#ifdef DEBUG_PARALLEL
		    ttyprintf (CONTTY, "Start switchproc slot, slot %d\n",indx);
#endif
		    dbus_slot_enable(dgbus); /* run */
		    slot_enabled_led_on(indx); 
		    if (indx == SPINNAKER_SP_SLOT)
			c7000_chassis_type = D_chassisType_c7010;
		    else
			c7000_chassis_type = D_chassisType_c7000;
		    sp_slot = indx;
		}
		/*
		 * if this is a multibus card, start it as well
		 */
		if (swbus_present(dgbus)) {
#ifdef DEBUG_PARALLEL
		    ttyprintf (CONTTY, "DANGER Starting multibus slot, slot %d\n",indx);
#endif
		    GET_TIMESTAMP(ttime);
		    dbus_slot_enable(dgbus);
#ifdef DEBUG_PARALLEL
		    ttyprintf (CONTTY, "dbus_slot_enable slot %d took %d\n",indx,ELAPSED_TIME(ttime));
#endif
		    slot_enabled_led_on(indx);
		}
	    }
	}
    } else {		/* eoir_active */
	for (indx = 0; indx < MAX_SLOTS; indx++) {
	    if (!validated[indx]) continue;	
	    /* if this card has not been loaded, do so */
	    if (!slots[indx].ucode_loaded) {
		if (oir_debug) 
		    buginf("\nslot %d about to load, reset, ucode", indx);
		GET_TIMESTAMP(ttime);
		dgbus = pslot_to_dbus (indx);
		dbus_hard_ctrlr_reset(dgbus); /* reset controller */
#ifdef DEBUG_PARALLEL
		ttyprintf (CONTTY, "dgbus analyze EOIR loading slot %d took %d\n",indx,ELAPSED_TIME(ttime));
#endif
		dbus_load_ucode(indx);
		if (oir_debug)
		    buginf("\nslot %d did ucode load", indx);
	    }
	}
    }

    if (!eoir_active) {
	DELAY(FCI_SOFT_RESET_TIME);	/* delay once after all cards reset */
	set_eoir_capable(sp_slot);
    }
#ifdef DEBUG_PARALLEL
    ttyprintf (CONTTY, "Dgbus analyze elapsed time %d ms\n",ELAPSED_TIME(ttime));
#endif
}

/*
 * which_7000_series
 */
chassis_type_t which_7000_series (void)
{
    return c7000_chassis_type;
}

static void slot_init (subsystype* subsys) {
    uint slot;

    for (slot = 0; slot < MAX_SLOTS; slot++) {
	slots[slot].slot_num = slot;
	slots[slot].flags = 0;
	slots[slot].serial_number = UNKNOWN_SERIAL_NUMBER;
	TIMER_STOP(slots[slot].insert_time);
	slots[slot].ctrlr_type = FCI_UNKNOWN_CONTROLLER;
	slots[slot].card_name[0] = '\0';
	slots[slot].ucode_loaded = FALSE;
	slots[slot].ucode_source = UCODE_SYSTEM;
	slots[slot].flash_file[0] = '\0';
	slots[slot].io_regs = FALSE;
	slots[slot].swdbus = FALSE;
	slots[slot].msg_new = TRUE;
	slots[slot].msg_name[0] = '\0';
	slots[slot].msg[0] = '\0';
	slots[slot].msg_timestamp = 0;
	slots[slot].consecutive_errs = 0;
	slots[slot].dbus_io_tty[DBUS_IO_TTY_CONSOLE] = DBUS_IO_NO_TTY;
	slots[slot].dbus_io_tty[DBUS_IO_TTY_DEBUG] = DBUS_IO_NO_TTY;
	slots[slot].mem_size = 0;

	dbus_reset_slot_vectors(slot);
    }
}

void disable_slot (uint slot)
{
    dgtype *dbus;
    uchar control;

    dbus = (dgtype*) pslot_to_dbus(slot);
    if (dbus_present(dbus)) {
	if (slots[slot].ctrlr_type != FCI_RSP2_CONTROLLER) {
	    /* 
	     * the RSP2 will bus error if not reset after it has posted and 
	     * error and has no need to set any LEDS
	     */

	    control = dbus_prot_rd(&dbus->diag_control);
	    if (control) { 
		/* 
		 * this is to avoid a bug in CIP eeprom 
		 * the CIP  hangs if the system writes a  zero to
		 * diag_control, and  diag_control is already zero 
		 */
		dbus_prot_wr(slot, &dbus->diag_control,
			     control & ~(MASTER_ENABLE|ENABLE_LED));
	    }
	}
    } else {
	TIMER_STOP(slots[slot].insert_time);
	slots[slot].serial_number = UNKNOWN_SERIAL_NUMBER;
    }
    slots[slot].flags |= BOARD_DISABLED;
}

/*
 * dbus_reset_IP_slots
 * 
 * Issue a hard reset to each interface processor card.
 * 
 * There is no certainty that the rom monitor has reset each card prior to
 * invoking the bootldr or system image.  This routine assures all cards are
 * reset when the system starts and when it terminates through normal shutdown
 * procedure; ie. not via an exception.
 *
 * At startup, it is called by dbus_startup().  At termination, it is invoked
 * as part of the reg_invoke_hardware_shutdown sequence on both
 * the 7000 and the 7500 platforms. 
 *
 * Cards that implement a software dbus require a wait after the reset.
 * This is because the card is loading and initializing its dbus handler.
 * The card will be unresponsive to accesses of any of the software implemented
 * dbus registers until the card has turned on the DBUS_SW_READY bit in the
 * diag_status register.  Note that the diag_status register is a hardware
 * register, and as such, is accessible while the card is being reset.
 *
 * All cards are reset prior to waiting.  This allows the wait period
 * to be significantly reduced over the approach of resetting and waiting
 * for each card serially.
 *
 * The eeprom is read to determine what kind of card is in the slot.
 * If the read returns DBUS_NO_RESPONSE_VAL, this is an indication that
 * the card is a swdbus card which is hung.  
 *
 */
void
dbus_reset_IP_slots (void)
{
    uint           slot;
    dgtype        *dbus;
    ushort         card_type;
    sys_timestamp  timer, wait_timer;
    
    GET_TIMESTAMP(timer);
    /* 
     * Determine what kind of card is in each occupied slot.
     */
    for (slot = 0; slot < MAX_SLOTS; slot++) {
	dbus = pslot_to_dbus(slot);
	if (dbus_present(dbus)) {
	    if (slots[slot].ctrlr_specific == DBUS_NO_RESPONSE_VAL) {
		card_type = dbus->eeprom.controller_type; 
		GET_TIMESTAMP(wait_timer);
		while ((card_type == DBUS_NO_RESPONSE_VAL) &&
		       (ELAPSED_TIME(wait_timer) < DBUS_NO_RESPONSE_TIMEOUT)) {
		    card_type = dbus->eeprom.controller_type;
		}
		slots[slot].ctrlr_specific = card_type;
	    }
	}
    }
    /*
     * Issue a hard reset unless the card is a slave RSP
     */
    for (slot = 0; slot < MAX_SLOTS; slot++) {
	dbus = pslot_to_dbus(slot);
	if (dbus_present(dbus)) {
	    if (! FCI_TYPE_RSP(slots[slot].ctrlr_specific)) {
		if (!dbus_swdbus_inline(slots[slot].ctrlr_specific) &&
		    (slots[slot].ctrlr_specific != DBUS_NO_RESPONSE_VAL)) {
		    dbus_set_loaded(dbus);
		}
		dbus_write_cbus_disabled(&dbus->diag_status, DBUS_HARD_REG_RESET);
	    }
	}
    }
    /*
     * Wait for each card to become ready.
     */
    for (slot = 0; slot < MAX_SLOTS; slot++) {
	dbus = pslot_to_dbus(slot);
	if (dbus_present(dbus)) {
	    if (! FCI_TYPE_RSP(slots[slot].ctrlr_specific)) {
		if (dbus_swdbus_inline(slots[slot].ctrlr_specific) || 
		    (slots[slot].ctrlr_specific == DBUS_NO_RESPONSE_VAL)) {
		    dbus_wait_for_sw_ready(dbus, "HARD RESET");
		}
	    }    
	}
    }
#ifdef DEBUG_TIMING
    buginf("\nTime to reset all cards = %d ms", ELAPSED_TIME(timer));
#endif
}

/*
 * dbus_slot_init
 *
 * Initialize the slots array entry for the given slot.
 * This routine is called for each slot when the system
 * initializes.
 *
 * Nothing is known about the card in the slot at this
 * point.  Initialize relevant fields as such.
 */
static void
dbus_init_slot (uint slot)
{
    slots[slot].ctrlr_specific = DBUS_NO_RESPONSE_VAL;
    dbus_reset_slot_vectors(slot);
}

/*
 * dbus_startup
 *
 * This is called prior to subsystem initialization.
 *
 * Assure dbus structures are sufficiently initialized
 * and then call dbus_reset_IP_slots to reset each card.
 *
 */
void
dbus_startup (void)
{
    uint slot;
    
    for (slot = 0; slot < MAX_SLOTS; slot++) {
	dbus_init_slot(slot);
    }
    
    dbus_reset_IP_slots();
}
static void
dbus_print_recovery_data (uint slot)
{
    slottype* slotp;
    
    slotp = &slots[slot];
    printf("\n\tdBus recovery read: %d write: %d timeouts: %d",
	   slotp->dbus_rd_retry_count,
	   slotp->dbus_wr_retry_count,
	   slotp->dbus_fail_recovery_count);
}


/*
 * neatly format information about card in given slot with dbus interface
 * assumes touch_device has been done on card to verify operational status
 */

static void print_dbus_card (dgtype* dbus, uint slot, boolean idprom_only)
{
    uint pslot, cpslot;
    uint cxbus, lslot;
    uchar control, status;
    uchar io_status;
    boolean comma;
    volatile uchar *ep;
    ushort ctrlr_type;
    uchar hwver_hi, hwver_lo, revision;
    ulong serial_number;
    uint part_number_pt1, part_number_pt2, part_number_pt3;
    uint test_history, rma_number_pt1, rma_number_pt2, rma_number_pt3;
    uint flags;
    uint i,j;
    slottype *slot_ptr;
    pa_info_slots *pa_info_ptr;

    printf("\nSlot %u%s:",
	   slot, (slot >= MAX_SLOTS) ? " (virtual)" : "", slot);

    if (!idprom_only) {
	pslot = (dbus->slot_and_comp >> 4) & 0xf;
	cpslot = dbus->slot_and_comp & 0xf;
	cxbus = (dbus->CxBus_and_logical_slot >> 4) & 0x3;
	lslot = dbus->CxBus_and_logical_slot & 0xf;
	
	printf("\n\tPhysical slot %d, ~physical slot 0x%x, "
	       "logical slot %d, CBus %d\n\t",
	       pslot, cpslot, lslot, cxbus);
	
	status = dbus_prot_rd(&dbus->diag_status);
	control = dbus_prot_rd(&dbus->diag_control);
	
	if (status & WCS_PARITY_ERROR) {
	    printf("WCS Parity Error, ");
	}
	if (status & WCS_CNTRL_INSTR_ERROR) {
	    printf("WCS Controller Instruction Error, ");
	}
	if (status & CXBUS_ERROR) {
	    printf("CBus Error, ");
	}
	if (status & DBUS_INTERNAL_ERROR) {
	    printf("Internal Instruction Error, ");
	}
	printf("Microcode Status 0x%x\n\t", status & UCODE_STATUS_MASK);
	
	comma = FALSE;
	if (control & MASTER_ENABLE) {
	    printf("%sMaster Enable", comma ? ", " : "");
	    comma = TRUE;
	}
	if (control & ENABLE_LED) {
	    printf("%sLED", comma ? ", " : "");
	    comma = TRUE;
	}
	if (control & WCS_DBUS_CMD_ENABLE) {
	    printf("%sWCS DBUS Cmd Enable", comma ? ", " : "");
	    comma = TRUE;
	}
	if (control & WCS_LOADING) {
	    printf("%sWCS Loading", comma ? ", " : "");
	    comma = TRUE;
	}
	if (control & WCS_LOADED) {
	    printf("%sWCS Loaded", comma ? ", " : "");
	    comma = TRUE;
	}
	
	flags = slots[slot].flags;
	if (flags) {
	    printf("\n\tBoard is %s%s%s%s%s",
		   (flags & BOARD_DISABLED)   ? "disabled "   : "",
		   (flags & BOARD_ANALYZED)   ? "analyzed "   : "",
		   (flags & BOARD_WEDGED)     ? "wedged "     : "",
		   (flags & BOARD_UNSUITABLE) ? "unsuitable " : "",
		   (flags & BOARD_INVALID)    ? "invalid "    : "");
	}
    }
    /*
     * Slots array does not contain an entry for the
     * RSP 7500_CHASSIS_INTERFACE_SLOT
     */
    if (slot < MAX_SLOTS) {
	comma = FALSE;
	if (slots[slot].io_regs) {
	    io_status = dbus_io_status_reg_read(slot);
	    printf("\n\tPending I/O Status: ");
	    if (io_status & DBUS_IO_CONSOLE) {
		printf("%sConsole I/O", comma ? ", " : "");
		comma = TRUE;
	    }
	    if (io_status & DBUS_IO_DEBUG) {
		printf("%sDebug I/O", comma ? ", " : "");
		comma = TRUE;
	    }
	    if (io_status & DBUS_IO_MSG) {
		printf("%sMsg I/O", comma ? ", " : "");
		comma = TRUE;
	    }
	    if (!comma)
		printf("None");
	}
    }

    printf("\n\tEEPROM format version %d\n\t", dbus->eeprom.eeprom_version);
    ctrlr_type = dbus_controller_type(dbus);
    hwver_hi = dbus->eeprom.hardware_version_high;
    hwver_lo = dbus->eeprom.hardware_version_low;
    revision = dbus->eeprom.board_revision;

    switch ((ctrlr_type_t) ctrlr_type) {
    case FCI_ETHER_CONTROLLER:
	printf("EIP controller");
	goto not_unknown;
    case FCI_SERIAL_CONTROLLER:
	printf("Serial controller");
	goto not_unknown;
    case FCI_LINK_CONTROLLER:
	printf("HIP controller");
	goto not_unknown;
    case FCI_CTR_CONTROLLER:
	printf("TRIP controller");
	goto not_unknown;
    case FCI_FDDIT_CONTROLLER:
	printf("FIP controller");
	goto not_unknown;
    case FCI_FSIP_CONTROLLER:
	if (get_dbus_controller_type(dbus) == FCI_SSIP_CONTROLLER) 
	    printf("SSIP controller");
	else
	    printf("FSIP controller");
	goto not_unknown;
    case FCI_MIP_CONTROLLER:
	if (get_dbus_controller_type(dbus) == FCI_SMIP_CONTROLLER) 
	    printf("SMIP controller");
	else
	    printf("MIP controller");
	goto not_unknown;
    case FCI_SP_CONTROLLER:
	if (hwver_hi == SSP_HARDWARE)
	    printf("Silicon Switch Processor");	    
	else
	    printf("Switch Processor");
	goto not_unknown;
    case FCI_SSP_CONTROLLER:
	printf("Silicon Switch Processor");
	goto not_unknown;
    case FCI_RP_CONTROLLER:
	printf("Route Processor");
	goto not_unknown;
    case FCI_CI_CONTROLLER:
	printf("Chassis Interface");
	goto not_unknown;
    case FCI_CI7000_CONTROLLER:
	printf("7000 Chassis Interface");
	goto not_unknown;
    case FCI_RSP1_CONTROLLER:
	printf("Route/Switch Processor");
	goto not_unknown;
    case FCI_RSP7000_CONTROLLER:
	printf("7000 Route/Switch Processor");
  	goto not_unknown;
    case FCI_C5IP_CONTROLLER:
	printf("C5IP controller");
	goto not_unknown;
    case FCI_C5RSP2_CONTROLLER:
    case FCI_RSP2_CONTROLLER:
	printf("Route/Switch Processor (dual bus)");
	goto not_unknown;
    case FCI_AIP_CONTROLLER:
	printf("ATM Interface Processor");
	goto not_unknown;
    case FCI_CIP_CONTROLLER:
	printf("%s controller", slots[slot].card_name);
	goto not_unknown;
    case FCI_FEIP_CONTROLLER:
	printf("FEIP controller");
	goto not_unknown;
    case FCI_RVIP_CONTROLLER:
	printf("VIP controller");
	goto not_unknown;
    case FCI_SVIP_CONTROLLER:
	printf("VIP2 controller");
	goto not_unknown;
    case FCI_POSIP_CONTROLLER:
	printf("POSIP controller");
	goto not_unknown;

    case FCI_FDDI_CONTROLLER:
    case FCI_CTR_SRS_CONTROLLER:
    case FCI_FDDIT_SRS_CONTROLLER:
    case FCI_SSIP_CONTROLLER:
    case FCI_SMIP_CONTROLLER:
    case FCI_RSP4_CONTROLLER:
    case FCI_NUM_CONTROLLERS:
    case FCI_UNKNOWN_CONTROLLER:
	break;
    }
    printf("unknown (0x%x)", ctrlr_type);

  not_unknown:

    printf(", HW rev %d.%d", hwver_hi, hwver_lo);
    if (revision < 0x50 || revision == 0xff) {
	printf(", board revision UNKNOWN");
    } else {
	printf(", board revision %c%d", 'A' + ((revision - 0x50) / 8), revision & 0x7);
    }
    serial_number = dbus_serial_number(dbus);
    part_number_pt1 = dbus->eeprom.part_number_31_24;
    part_number_pt2 = dbus->eeprom.part_number_23_16 << 8 | dbus->eeprom.part_number_15_8;
    part_number_pt3 = dbus->eeprom.part_number_7_0;
    test_history = dbus->eeprom.test_history;
    rma_number_pt1 = dbus->eeprom.rma_number_23_16;
    rma_number_pt2 = dbus->eeprom.rma_number_15_8;
    rma_number_pt3 = dbus->eeprom.rma_number_7_0;
    flags = dbus->eeprom.flags;
    
    printf("\n\tSerial number: %08lu  Part number: %02u-%04u-%02u",
	   serial_number, part_number_pt1, part_number_pt2, part_number_pt3);
    printf("\n\tTest history: 0x%02x        RMA number: %02d-%02d-%02d",
	   test_history, rma_number_pt1, rma_number_pt2, rma_number_pt3);
    printf("\n\tFlags: ");
    if ((flags & 0x80) == 0) {
	printf("cisco 7000 board");
    }
    if (flags & ~0x80) {
	printf(" unknown flags 0x%x", flags & ~0x80);
    }
    if (ctrlr_7500_compat(ctrlr_type, (hwver_hi << 8) | hwver_lo)) {
	printf("; 7500 compatible");
    }
    ep = &dbus->eeprom.eeprom_version;
    
    printf("\n\n\tEEPROM contents (hex):");
    printf("\n\t  0x20: %02x %02x %02x %02x %02x %02x %02x %02x "
	   "%02x %02x %02x %02x %02x %02x %02x %02x",
	   ep[0], ep[1], ep[2], ep[3], ep[4], ep[5], ep[6], ep[7],
	   ep[8], ep[9], ep[10], ep[11], ep[12], ep[13], ep[14], ep[15]);
    printf("\n\t  0x30: %02x %02x %02x %02x %02x %02x %02x %02x "
	   "%02x %02x %02x %02x %02x %02x %02x %02x",
	   ep[16], ep[17], ep[18], ep[19], ep[20], ep[21], ep[22], ep[23],
	   ep[24], ep[25], ep[26], ep[27], ep[28], ep[29], ep[30], ep[31]);
    
    if (!idprom_only) {
	printf("\n\n\tSlot database information:");
	printf("\n\tFlags: 0x%lx\t", slots[slot].flags);
	if (TIMER_RUNNING(slots[slot].insert_time)) {
	    printf("Insertion time: %#Ta (%TE ago)", slots[slot].insert_time,
		   slots[slot].insert_time);
	} else {
	    printf("Insertion time: unknown");
	}
    	if (dbus_timeout_recovery_capable(slots[slot].ctrlr_type))
	    dbus_print_recovery_data(slot);
	
	/*
	 * Display Dram Size
	 */
	slot_ptr = &slots[slot];
	if (slot_ptr->mem_size != 0) {
	    printf("\n\n\tController Memory Size: %d MBytes", 
		   slot_ptr->mem_size/ONE_MEG);
	    if (slot_ptr->io_size != 0) {
		printf (" DRAM, %d KBytes SRAM", 
			(slot_ptr->io_size/ONE_K));
	    }
	} else {
	    if (vip_controller((ctrlr_type_t) ctrlr_type))
		printf("\n\n\tVIP Controller Memory Size: Unknown"); 
	}

	if (ctrlr_type == FCI_POSIP_CONTROLLER) {
	    ep = slot_ptr->pa_info[0];
	    if (ep) {
		printf("\n\n\tPA EEPROM contents (hex):");
		for (i=0; i < 2; i++) {
		    if (i == 0) printf("\n\t  0x00:");
		    else
			printf("\n\t  0x10:");
		    for (j=0; j < 16; j++) printf(" %02x",ep[i*16+j]);
		}
	    }
	} else if (vip_controller(ctrlr_type) ||
	    (ctrlr_type == FCI_FEIP_CONTROLLER)) {
	    /*
	     * Display PA EEPROM information for FEIP/VIP cards.
	     */
	    slot_ptr = &slots[slot];
	    for (i=0; i < SLOT_NUM_VIP_PAS; i++) {
		pa_info_ptr = slot_ptr->pa_info[i];
		if (!pa_info_ptr)
		    continue;
	    if (vip_controller(ctrlr_type)) {
		printf("\n\n\tPA Bay %d Information:", i);
		switch (pa_info_ptr->pa_type) {
		case FCI_TYPE_VIP_FETHER:
		case FCI_TYPE_VIP_FETHER_TX_NISL:
		case FCI_TYPE_VIP_FETHER_TX_ISL:
		case FCI_TYPE_VIP_FETHER_FX_ISL:
		    printf("\n\t\tFast-Ethernet PA, %d ports", 
			   pa_info_ptr->pa_ifcount);
		    switch ((pa_hardware_type_t) pa_info_ptr->controller_type) {
		    case PA_HARDWARE_1FE_TX_ISL:
			printf(", 100BaseTX-ISL");
			break;
		    case PA_HARDWARE_1FE_FX_ISL:
			printf(", 100BaseFX-ISL");
			break;
		    case PA_HARDWARE_1FE_TX_NISL:
		    case PA_HARDWARE_1FE:
			printf(", 100BaseTX-nISL");
			break;
		    default:
			break;
		    }
		    break;
		case FCI_TYPE_VIP_ETHER8:
		    printf("\n\t\tEthernet PA, %d ports", 
			   pa_info_ptr->pa_ifcount);
		    break;
		case FCI_TYPE_VIP_SERIAL4:
		    printf("\n\t\tFast-Serial PA, %d ports", 
			   pa_info_ptr->pa_ifcount);
		    break;
		case FCI_TYPE_VIP_TRING4:
		    printf("\n\t\tToken Ring PA, %d ports", 
			   pa_info_ptr->pa_ifcount);
		    break;
		case FCI_TYPE_VIP_FDDI:
		    printf("\n\t\tFDDI PA, %d ports",
			   pa_info_ptr->pa_ifcount);
		    break;
		case FCI_TYPE_VIP_ESA:
		    printf("\n\t\tEncryption SA");
		    break;
		}
        } else {
            /* FEIP PA information */
            printf("\n\n\tPA %d Information:", i);
            printf("\n\t\tFast-Ethernet PA, 1 port");
            switch ((pa_hardware_type_t) pa_info_ptr->controller_type) {
            case PA_HARDWARE_1FE_TX_ISL:
            printf(", 100BaseTX-ISL");
            break;
            case PA_HARDWARE_1FE_FX_ISL:
            printf(", 100BaseFX-ISL");
            break;
            case PA_HARDWARE_1FE_TX_NISL:
            case PA_HARDWARE_1FE:
            printf(", 100BaseTX-nISL");
            break;
            case PA_HARDWARE_1FE_FX_NISL:
            printf(", 100BaseFX-nISL");
            break;
            default:
            break;
            }
        }
		printf("\n\t\tEEPROM format version %d", 
		       pa_info_ptr->eeprom_version);
		printf("\n\t\tHW rev %x.%x", 
		       (pa_info_ptr->hardware_version & 0xFF00) >> 8,
		       pa_info_ptr->hardware_version & 0xFF);
		if (pa_info_ptr->board_revision < 0x50 || 
		    pa_info_ptr->board_revision == 0xff) {
		    printf(", Board revision UNKNOWN");
		} else {
		    printf(", Board revision %c%d", 
			   'A' + ((pa_info_ptr->board_revision - 0x50) / 8), 
			   pa_info_ptr->board_revision & 0x7);
		}
		printf("\n\t\tSerial number: %08lu  Part number: %02u-%04u-%02u ",
		       pa_info_ptr->serial_number,
		       pa_info_ptr->part_number>>24,
		       ((pa_info_ptr->part_number>>8) & 0xFFFF),
		       pa_info_ptr->part_number & 0xFF);
	    }
	}

	if (vip_controller(ctrlr_type)) {

	    if (slot_ptr->crash_timestamp != 0) {
		char timestamp_string[UNIX_TIME_STRING_2_MINBUF];

		if (slot_ptr->crash_count > 1) {
		    printf("\n\n\t%u crashes since restart.", 
			   slot_ptr->crash_count);
		} else {
		    printf("\n\n\t1 crash since restart.");
		}
		unix_time_string_2(timestamp_string, 
				   slot_ptr->crash_timestamp);
		printf("\n\tLast crash context (%s):\n", timestamp_string);

		vip_print_context("\t", slot_ptr->crash_cntxt);
	    }
	}
    }
}

void show_diagbus (parseinfo *csb) {
    dgtype* dbus;
    int slot;
    int cpu_slot = dbus_system_rp_slot();

    slot = GETOBJ(int,1);   /* -1 if no slot supplied, slot number otherwise */
    
    if (slot != -1) {
	if (slot >= MAX_SLOTS && slot != C7500_CHASSIS_INTERFACE_SLOT) {
	    printf("\n%% slot number must be from 0 to %d, or %d",
                 MAX_SLOTS-1, C7500_CHASSIS_INTERFACE_SLOT);
	} else {
	    if (slot == cpu_slot)
		dbus = dbus_rpslot_to_dbus();
            else
		dbus = pslot_to_dbus(slot);
            if (slot == C7500_CHASSIS_INTERFACE_SLOT) {
                if (touch_device(dbus)) {
                    print_dbus_card(dbus, C7500_CHASSIS_INTERFACE_SLOT,
                                    TRUE /* print idprom info only */);
                } else {
                    printf("\n%% CI not present");
                }
            } else {
                if ((slot != cpu_slot) && !dbus_present(dbus))
                    printf("\n%% No response from slot %d", slot);
                else
                    print_dbus_card(dbus, slot, dbus_cpu_slot(slot));
            }
	}
    } else {
	automore_enable(NULL);
	for (slot = 0; slot < MAX_SLOTS; slot++) {
	    if (slot == cpu_slot)
		dbus = dbus_rpslot_to_dbus();
	    else
		dbus = pslot_to_dbus(slot);
	    if (dbus_cpu_slot(slot) || dbus_present(dbus))
		print_dbus_card(dbus, slot, dbus_cpu_slot(slot));
	}
	dbus = pslot_to_dbus(C7500_CHASSIS_INTERFACE_SLOT);
	if (touch_device(dbus))
	    print_dbus_card(dbus,
			    C7500_CHASSIS_INTERFACE_SLOT,
			    TRUE /* print idprom info only */);
	automore_disable();
    }
}


static boolean aip_download (dgtype *dbus, uint slot, uchar *mem, uint size)
{
    uint i;
    int   j, addr;
    char  buf[8], *ip, *dp;
    uchar lastone = 0x00;
    uchar status;

    /*
     * Initialize start counter.
     */
    buf[0] = AIP_WCS_CMD_REG;
    buf[1] = AIP_WCS_INIT_CNT;
    buf[2] = AIP_WCS_WR_DAT_4;
    buf[3] = buf[4] = buf[5] = buf[6] = addr = 0;

    for (i=0; i<7; i++) {
	dbus->ucode_command_and_data = lastone = buf[i];
	status = dbus_prot_rd(&dbus->diag_status);
	if (status & WCS_ERROR_MASK) {
	    errmsg(&msgsym(WCSLDERR, DBUS), slot, status, lastone, 0);
	    return(FALSE);
	}
    }
	
    /*
     * This is repeated for every contiguous four byte segment.
     */
    buf[0]  = AIP_WCS_CMD_REG;
    buf[1]  = AIP_WCS_WR_RAM;
    buf[2]  = AIP_WCS_WR_DAT_4;
    /*
     * buf[3] through buf[6] contains actual data, so this is
     * filled in on the fly.
     */
    /*
     * Convert image to download format. 
     */
    for (i=0, ip=(char *)mem; i<size; i+=4) {
	for (dp = &buf[3]; dp < &buf[7]; ) {
	    *dp++ = *ip++;
	}
	for (j=0; j<7; j++) {
	    dbus->ucode_command_and_data = lastone = buf[j];
	    status = dbus_prot_rd(&dbus->diag_status);
	    if (status & WCS_ERROR_MASK) {
		errmsg(&msgsym(WCSLDERR, DBUS),
		       slot, status, lastone, addr+i);
		return(FALSE);
	    }
	}
    }

    dbus->ucode_command_and_data = lastone = AIP_WCS_LOADED;
    status = dbus_prot_rd(&dbus->diag_status);
    if (status & WCS_ERROR_MASK) {
	errmsg(&msgsym(WCSLDERR, DBUS), slot, status, lastone, addr+i);
	return(FALSE);
    }
    return(TRUE);
}


/*
 * answers the question "Does this controller type required download flow
 * control?".
 * XXX RSP4 alert, addd it to the list, or change to use "family"
 */
static boolean ctrlr_needs_flowcntrl (ctrlr_type_t ctrlr_type)
{
    switch (ctrlr_type) {
      case FCI_RSP2_CONTROLLER:
	return(TRUE);
      default:
	return(FALSE);
    }
}


/*
 * ctrlr_caught_breath()
 *
 * For controllers that need flow control during their dbus download,
 * give them a break, until they're ready to proceed.  If they
 * become wedged, give up on them, and return false.
 */

#define MAX_FLOW_ATTEMPTS 10
static boolean ctrlr_caught_breath (uint slot, dgtype *dbus, 
				    boolean ok_to_suspend)
{
    int attempt = 0;
    /*
     * Controllers that don't need flow control are always caught up.
     */
    if (!ctrlr_needs_flowcntrl(slots[slot].ctrlr_type))
	return TRUE;

    while(TRUE) {
	uchar status;

	status = dbus_prot_rd(&dbus->diag_status);

	/*
	 * If it's ready, we're done.
	 */
	if (status & FLOW_START)
	    return TRUE;

	/*
	 * If we've been waiting too long, give up on
	 * it and return an error.
	 */
	if (attempt++ > MAX_FLOW_ATTEMPTS)
	    return FALSE;

	/*
	 * It needs a rest, so sleep or delay.
	 */
	if (ok_to_suspend)
	    process_sleep_for(DBUS_SW_POLL_TIME);
	else
	    DELAY(DBUS_SW_POLL_TIME);
    }
}


/*
 * download_microcode_from_ram - load microcode from ram
 */

boolean download_microcode_from_ram (uint slot, uchar *mem, uint size,
				     boolean wait)
{
    dgtype *dbus;
    uchar old_status;	/* board's original status register contents */
    uchar old_control;	/* board's original control register contents */
    uint i;
    uchar status, control;
    uchar lastone = 0x00;
    sys_timestamp start;
    int   j, parity, m, addr;
    char  c, buf[12], p, pmask, *ip, *dp;
    int byte_cnt;
    boolean ok_to_suspend;
    sys_timestamp ttime;

    dbus = pslot_to_dbus(slot);
    ok_to_suspend = process_suspends_allowed();

    GET_TIMESTAMP(start);
    /*
     * save original state of board (enabled, LED, etc.)
     */
    old_status = dbus_prot_rd(&dbus->diag_status);
    old_control = dbus_prot_rd(&dbus->diag_control);
#ifdef DEBUG_PARALLEL
    ttyprintf (CONTTY, "Old Control Reg 0x%x\n",old_control);
#endif

    /*
     * if nowaited, the following is performed appropriately by the caller
     */
    if (wait) {
#ifdef DEBUG_PARALLEL
	ttyprintf(CONTTY,"download from ram WAITED slot %d\n",slot);
#endif
	if (old_status & WCS_ERROR_MASK) {
	    errmsg(&msgsym(WCSERR, DBUS), slot, old_status);
	    dbus_slot_reset(slot);
	}

	/*
	 * halt microcode and instruct WCS controller to accept commands from dBus
	 */
	dbus_prot_wr(slot, &dbus->diag_control, WCS_DBUS_CMD_ENABLE);

	/* 
	 * Would like to avoid issuing a reset for IP's with SWDBUS but, seems
         * slot reset is required for SWDBUS otherwise fails each 2nd microload 
	 */
	GET_TIMESTAMP(ttime);
        dbus_slot_reset(slot);
#ifdef DEBUG_PARALLEL
	ttyprintf (CONTTY,"slot reset slot %d took %d\n",slot,ELAPSED_TIME(ttime));
#endif
	/* 
	 * Some dbus Xilinx's have broken reset logic and stretch the
	 * reset pulse too long
	 */
	DELAY(1);
	dbus_prot_wr(slot, &dbus->diag_control, WCS_DBUS_CMD_ENABLE);
    }

    switch(slots[slot].ctrlr_type) {
    case FCI_AIP_CONTROLLER:
	if (!aip_download(dbus, slot, mem, size)) {
	    return(FALSE);
	}
	break;
    case FCI_FSIP_CONTROLLER:
        for (m=0, addr=0; m<FSIP_NUM_MODULES; m++, addr+=FSIP_WCS_SIZE) {
            /*
             * Initialize start counter.
             */
            buf[0]  = FSIP_WCS_CMD_REG;
            buf[1]  = FSIP_WCS_INIT_CNT;
            buf[2]  = FSIP_WCS_WR_DAT_4;
            buf[3]  = addr & 0xff;
            buf[4]  = addr >> 8;
            buf[5]  = addr >> 16;
            buf[6]  = addr >> 24;
            for (i=0; i<7; i++) {
                dbus->ucode_command_and_data = lastone = buf[i];
                status = dbus_prot_rd(&dbus->diag_status);
                if (status & WCS_ERROR_MASK) {
                    errmsg(&msgsym(WCSLDERR, DBUS), slot, status, lastone, 0);
                    return(FALSE);
                }
            }

            /*
             * This is repeated for every contiguous four byte segment.
             */
            buf[0]  = FSIP_WCS_CMD_REG;
            buf[1]  = FSIP_WCS_WR_RAM;
            buf[2]  = FSIP_WCS_WR_DAT_4;
            /*
             * buf[3] through buf[6] contains actual data, so this is
             * filled in on the fly.
             */
            buf[7]  = FSIP_WCS_CMD_REG;
            buf[8]  = FSIP_WCS_WR_PAR;
            buf[9]  = FSIP_WCS_WR_DAT_1;
            /*
             * The low-order bits of buf[10] contains parity for
             * preceeding 4 bytes of data.  Bit 0 is parity for byte 0,
             * ..., bit 3 is parity for byte 3.
             * Convert image to download format.
             */
            for (i=0, ip=(char *)mem; i<size; i+=4) {
                buf[10] = 0;
                for (dp = &buf[3], p=0, pmask=1; dp < &buf[7]; pmask<<=1) {
                    c = *dp++ = *ip++;
                    for (parity=0; c; c<<=1)
                        if (c<0)
                            parity++;
                    if (parity & 1)
                        buf[10] |= pmask;
                }
                for (j=0; j<11; j++) {
                    dbus->ucode_command_and_data = lastone = buf[j];
                    status = dbus_prot_rd(&dbus->diag_status);
                    if (status & WCS_ERROR_MASK) {
                        errmsg(&msgsym(WCSLDERR, DBUS),
                               slot, status, lastone, addr+i);
                        return(FALSE);
                    }
                }
            }
        }
        dbus->ucode_command_and_data = lastone = FSIP_WCS_LOADED;
        status = dbus_prot_rd(&dbus->diag_status);
        if (status & WCS_ERROR_MASK) {
            errmsg(&msgsym(WCSLDERR, DBUS), slot, status, lastone, addr+i);
            return(FALSE);
        }
	break;

#define DBUS_MAX_BYTES_PER_DL_PASS 10000
    case FCI_CIP_CONTROLLER:
    case FCI_FEIP_CONTROLLER:
    case FCI_RVIP_CONTROLLER:
    case FCI_SVIP_CONTROLLER:
    case FCI_RSP2_CONTROLLER:
    case FCI_POSIP_CONTROLLER:
	byte_cnt = 0;
        for (i = 0; i < size; i++) {
            dbus->ucode_command_and_data = lastone = *mem++;
	    if (byte_cnt++ > DBUS_MAX_BYTES_PER_DL_PASS) {
		byte_cnt = 0;

		/*
		 * Give others a chance to run.
		 */
		if (ok_to_suspend) {
		    process_may_suspend(); 
		}

		/*
		 * If this is a card that needs download flow control,
		 * now would be a good time to check if it needs a rest.
		 * If this guy returns FALSE, it means he timed out on
		 * flow control, and we should give up on this card.
		 */
		if (!ctrlr_caught_breath(slot, dbus, ok_to_suspend)) {
		    errmsg(&msgsym(WCSFLOW, DBUS), slot, lastone, i);
#ifdef DEBUG_PARALLEL
		    ttyprintf(CONTTY,"RSP WCSFLOW error return\n");
#endif
		    return(FALSE);
		}
	    }
        }
	/* 
	 * CIP/FEIP/RVIP/SVIP/RSP2 are slow at getting going after 
	 * ucode download (since they may be uncompressing on the 
	 * fly), so give them a chance to catch their breath 
	 * (i.e. wait till john has set the status bit). 
	 *
	 * If nowaited, the following and the code to the end of the routine
	 * must be handled appropriately by the caller for SWDBUS devices.
	 */
	if (!wait) return(TRUE); /* return now */
#ifdef DEBUG_PARALLEL
	ttyprintf(CONTTY,"download from ram post code WAITED slot %d\n",slot);
#endif

	GET_TIMESTAMP(ttime);
	dbus_wait_for_sw_ready(dbus, "DOWNLOAD");
#ifdef DEBUG_PARALLEL
	ttyprintf(CONTTY,"Wait for decompress for slot %d took %d\n",slot,ELAPSED_TIME(ttime));
#endif
	status = dbus_prot_rd(&dbus->diag_status);
	if (status & (WCS_ERROR_MASK | DBUS_INTERNAL_ERROR)) {
	    errmsg(&msgsym(WCSLDERR, DBUS), slot, status, lastone, i);
	    return(FALSE);
	}
	break;

    default:
        for (i = 0; i < size; i++) {
            dbus->ucode_command_and_data = lastone = *mem++;
            status = dbus_prot_rd(&dbus->diag_status);
            if (status & WCS_ERROR_MASK) {
                errmsg(&msgsym(WCSLDERR, DBUS), slot, status, lastone, i);
                return(FALSE);
            }
        }
	break;
    }

/* bear in mind, any changes from here to end have to be incorporated in 
   the nowaited caller (dgbus_analyze) */

#ifdef DEBUG_TIMING
    buginf("\ndownload took %d ms", ELAPSED_TIME(start));
#endif
    dbus_prot_wr(slot, &dbus->diag_control, old_control |  WCS_LOADED);
#ifdef DEBUG_PARALLEL
    ttyprintf(CONTTY,"Wrote control to slot %d control %x\n",slot,old_control | WCS_LOADED);
#endif
    DELAY(1); /* XXX temp, needed for the rsp2 slave prom until the */
	      /* timing is sorted out */

    control = dbus_prot_rd(&dbus->diag_control);
    status = dbus_prot_rd(&dbus->diag_status);

    if ((control & WCS_LOADING) ||
	!(control & WCS_LOADED) ||
	(status & WCS_ERROR_MASK)) {
	errmsg(&msgsym(WCSLDERR, DBUS), slot, status, 0xdead, 99);	
	return(FALSE);
    }
    return(TRUE);
} 

/*
 * download_microcode_chunk_from_ram
 */
boolean download_microcode_chunk_from_ram (uchar **buf, long *bytesleft, 
					   long len, void *opaque_args)
{
    dgtype *dbus;
    ulong i;
    uchar status, control;
    uchar lastone = 0x00;
    ucode_downloadtype *args;
    boolean last_chunk;
    uchar *mem;
    sys_timestamp timer;
    boolean ok_to_suspend;
    
    /*
     * Handle a request to download a zero-lengthed block
     */
    if (len == 0) {
	return TRUE;
    }

    ok_to_suspend = process_suspends_allowed();
    GET_TIMESTAMP(timer);
    args = (ucode_downloadtype *)opaque_args;
    dbus = pslot_to_dbus(args->slot);
    last_chunk = (*bytesleft == 0) ? TRUE : FALSE;
    mem = *buf;
    args->checksum = update_cksum(args->checksum, mem, len);

    if (args->chunks_done == 0) {
	/*
	 * Here we must call check_microcode_header() to peel of the
	 * microcode header and adjust 'len'.  Normally it would also
	 * subtract off the size of the trailer, but since we may only
	 * have a chunk of ucode we will do the subtraction ourselves
	 * when we have the last_chunk.
	 */
	if (!check_microcode_header(&mem, &len, args->filename,
				    args->slot, FALSE)) {
	    return FALSE;
	}

	/*
	 * save original state of board (enabled, LED, etc.)
	 */
	args->old_status = dbus_prot_rd(&dbus->diag_status);
	args->old_control = dbus_prot_rd(&dbus->diag_control);
#ifdef DEBUG_PARALLEL
	ttyprintf (CONTTY, "Old Control Reg 0x%x\n",args->old_control);
#endif

	if (args->wait) {
	    if (args->old_status & WCS_ERROR_MASK) {
		errmsg(&msgsym(WCSERR, DBUS), args->slot, args->old_status);
		dbus_slot_reset(args->slot);
		args->old_status = dbus_prot_rd(&dbus->diag_status);
		args->old_control = dbus_prot_rd(&dbus->diag_control);
	    }

	    /*
	     * halt microcode and instruct WCS controller to accept
	     * commands from dBus 
	     */
	    dbus_prot_wr(args->slot, &dbus->diag_control, WCS_DBUS_CMD_ENABLE);
	    
	    /* Would like to avoid issuing a reset for IP's with SWDBUS but, seems
	       slot reset is required for SWDBUS otherwise fails each 2nd microload */
	    dbus_slot_reset(args->slot);
	    /* 
	     * Some dbus Xilinx's have broken reset logic and stretch the
	     * reset pulse too long
	     */
	    DELAY(1);
	    dbus_prot_wr(args->slot, &dbus->diag_control, WCS_DBUS_CMD_ENABLE);
	}

	args->downloaded_bytes = 0;
	args->download_time = 0;
    }
    
    if (last_chunk) {
	len -= sizeof(dlu_trlr_t);  /* don't send the trailer */
    }

    switch(args->card_type) {
    case FCI_AIP_CONTROLLER:
    case FCI_FSIP_CONTROLLER:
	return FALSE;
    case FCI_CIP_CONTROLLER: 
    case FCI_FEIP_CONTROLLER: 
    case FCI_RVIP_CONTROLLER:
    case FCI_SVIP_CONTROLLER:
    case FCI_RSP2_CONTROLLER:
    case FCI_POSIP_CONTROLLER:

        for (i = 0; i < len; i++) {
            dbus->ucode_command_and_data = lastone = *mem++;
        }

	/*
	 * If this is a card that needs download flow control,
	 * now would be a good time to check if it needs a rest.
	 * If this guy returns FALSE, it means he timed out on
	 * flow control, and we should give up on this card.
	 */
	if (!ctrlr_caught_breath(args->slot, dbus, ok_to_suspend)) {
	    errmsg(&msgsym(WCSFLOW, DBUS), args->slot, lastone, 
		   args->downloaded_bytes + i);
	    return(FALSE);
	}

	status = dbus_prot_rd(&dbus->diag_status);
	if (status & (WCS_ERROR_MASK | DBUS_INTERNAL_ERROR)) {
	    errmsg(&msgsym(WCSLDERR, DBUS), args->slot, status,
		   lastone, args->downloaded_bytes + i);
	    return(FALSE);
	}
	
	if (last_chunk) {
	    /* 
	     * Wait till the CIP/FEIP/RVIP/SVIP are ready.
	     */
	    if (!args->wait) {
		args->checksum = ~args->checksum;
		if (args->checksum) {
		    errmsg(&msgsym(BADCHKSUM, UCODE), 
			   args->filename, (ulong)args->checksum, 0);
		    return FALSE;
		} else {
		    return TRUE;
		}
	    }
	    /*
	     * If nowaited, the following and the code to the end of the routine
	     * must be handled appropriately by the caller for SWDBUS devices.
	     */
	    dbus_wait_for_sw_ready(dbus, "DOWNLOAD");
	    status = dbus_prot_rd(&dbus->diag_status);
	    if (status & (WCS_ERROR_MASK | DBUS_INTERNAL_ERROR)) {
		errmsg(&msgsym(WCSLDERR, DBUS), args->slot, status,
		       lastone, args->downloaded_bytes + i);
		return(FALSE);
	    }
	}
	break;
    default:
        for (i = 0; i < len; i++) {
            dbus->ucode_command_and_data = lastone = *mem++;
            status = dbus_prot_rd(&dbus->diag_status);
            if (status & WCS_ERROR_MASK) {
                errmsg(&msgsym(WCSLDERR, DBUS), args->slot, status,
		       lastone, args->downloaded_bytes + i);
                return(FALSE);
            }
        }
    }

    args->downloaded_bytes += len;

    if (last_chunk) {
	args->checksum = ~args->checksum;
	if (args->checksum) {
	    errmsg(&msgsym(BADCHKSUM, UCODE), 
		   args->filename, (ulong)args->checksum, 0);
	    return FALSE;
	}

	/* bear in mind, any changes from here to end should be incoroprated in
	   the nowaited caller (dbus_analyze) */

	dbus_prot_wr(args->slot, &dbus->diag_control, args->old_control | WCS_LOADED);
	control = dbus_prot_rd(&dbus->diag_control);
	status = dbus_prot_rd(&dbus->diag_status);
	
	if ((control & WCS_LOADING) ||
	    !(control & WCS_LOADED) ||
	    (status & WCS_ERROR_MASK)) {
	    errmsg(&msgsym(WCSLDERR, DBUS), args->slot, status, 0xdead, 98);	
	    return(FALSE);
	}
#ifdef DEBUG_TIMING
	buginf("\ndownloaded %d chunks. Total took %d ms",
	       args->chunks_done, args->download_time);
#endif

    }

    args->chunks_done++;
    args->download_time += ELAPSED_TIME(timer);

    return(TRUE);
} 

/*
 * dbus_slot_enabled
 * return TRUE iff slot is loaded and running
 * MASTER_ENABLE & WCS_LOADED asserted in control register
 * no error bits asserted in status register
 */
boolean dbus_slot_enabled (dgtype* dbus)
{
    ushort reg;
    if (!touch_device(dbus))
	return FALSE;
    reg = dbus_prot_rd(&dbus->diag_control);
    if ((reg & (MASTER_ENABLE|WCS_LOADED|WCS_DBUS_CMD_ENABLE)) !=
	(MASTER_ENABLE|WCS_LOADED))
	return FALSE;
    reg = dbus_prot_rd(&dbus->diag_status);
    if (reg & DBUS_ERROR_MASK)
	return FALSE;
    return TRUE;
}

void dbus_mci_internal_clock_enable (dgtype* dbus, uint channel)
{
    if (dbus_present(dbus)) {
	dbus->fip_leds |= (1<<channel); /* internal clocking enable */
    }
}

void dbus_mci_internal_clock_disable (dgtype* dbus, uint channel)
{
    if (dbus_present(dbus)) {
	dbus->fip_leds &= ~(1<<channel); /* internal clocking disable */
    }
}

void dbus_mci_nrzi_enable (dgtype* dbus, uint channel)
{
    if (dbus_present(dbus)) {
	dbus->ssp_jumpers |= (1<<channel);
    }
}

void dbus_mci_nrzi_disable (dgtype* dbus, uint channel)
{
    if (dbus_present(dbus)) {
	dbus->ssp_jumpers &= ~(1<<channel);
    }
}
void dbus_mci_invert_txc_enable (dgtype* dbus, uint channel)
{
    if (dbus_present(dbus)) {
	dbus->ssp_jumpers |= (1<<(channel+4));
    }
}

void dbus_mci_invert_txc_disable (dgtype* dbus, uint channel)
{
    if (dbus_present(dbus)) {
	dbus->ssp_jumpers &= ~(1<<(channel+4));
    }
}

/*
 * dbus_write_fip_leds
 * Write dbus LED register for the FIP, called from
 * fip_leds in FDDI SMT code.  3 LSB of each nibble
 * of argument are left, middle, right leds respectively.
 * See also fip_leds in fddi_smt_c7000.c.
 */

void dbus_write_fip_leds (uint slot, uchar leds)
{
    dgtype* dbus;
    int level;

    dbus = pslot_to_dbus(slot);
    /*
     * make sure card hasn't vanished via hotswap before we touch it
     */
    level = raise_interrupt_level(LEVEL_CONSOLE);
    if (dbus_present(dbus)) {
	dbus->fip_leds = ~leds;	/* these leds are active low */
    }
    reset_interrupt_level(level);
}

/*
 * board_has_instr_error (slot)
 * return true if WCS_CNTRL_INSTR_ERROR is set for the board
 * in this slot.  Used by the RSP master/slave arbitration code.
 * Assumes the board is installed, and that the caller is the dbus
 * master.  Do not call this routine on the slave, and do not call
 * this routine, if there is no board present in the provided slot.
 */
boolean board_has_instr_error (uint slot)
{
    dgtype	*dbus;
    uchar	status;

    dbus = pslot_to_dbus(slot);
    status = dbus_prot_rd(&dbus->diag_status);
    return(status & WCS_CNTRL_INSTR_ERROR);
}

/*
 * dbus_error_scan
 * periodically scan diagnostic bus for error conditions
 */
void dbus_error_scan (void)
{
    dgtype	*dbus;
    int		slot;
    uchar	status;
    int		msgcnt;

    if (hotstall_flag)
	return;

    for (slot = 0; slot < MAX_CONTROLLERS; slot++) {

	if (slots[slot].flags & (BOARD_NONEXIST | BOARD_DISABLED ))
	    continue;

	dbus = pslot_to_dbus(slot);
	if (!dbus_present(dbus)) {

	    if (hotstall_flag)
		return;

	    /* board didn't respond, bad */
	    errmsg(&msgsym(NORESP, DBUS), dbus_to_pslot(dbus));

	    /* better not to risk trying to reset it */
	    slots[slot].flags |= BOARD_DISABLED;
	    continue;
	}
	
	status = dbus_prot_rd(&dbus->diag_status);
	if (status & DBUS_ERROR_MASK) {

	    if (hotstall_flag)
		return;

	    /* some sort of error detected */
	    if (status & CXBUS_ERROR) {
		errmsg(&msgsym(CXBUSERR, DBUS), slot);
	    }
	    if (status & DBUS_INTERNAL_ERROR) {
		errmsg(&msgsym(DBUSINTERR, DBUS), slot);
	    }
	    if (status & WCS_PARITY_ERROR) {
		errmsg(&msgsym(WCSPARERR, DBUS), slot);
	    }
	    if (status & WCS_CNTRL_INSTR_ERROR) {
		errmsg(&msgsym(WCSCTRLRERR, DBUS), slot);
	    }
	    dbus_slot_error(slot);
	} else {
	    slots[slot].consecutive_errs = 0;
	}

	/*
 	 * check if the card support new i/o dbus registers 
         */
 	if (slots[slot].io_regs) {
	    msgcnt = 0;
	    for (msgcnt = 20; msgcnt; msgcnt--) { 
		if (hotstall_flag)
		    return;
		if (!dbus_check_msg(dbus, slot))
		    break;
	    }
  	}
    }
}

/*
 * answers the question "Is the specified controller with the specified
 * hardware version suitable for use in a 7500 chassis?"
 */
boolean ctrlr_7500_compat (uchar ctrlr, ushort hwver)
{
    ctrlr_type_t ctrlr_type;

    ctrlr_type = ctrlr;
    switch (ctrlr_type) {
      case FCI_ETHER_CONTROLLER:
	/*
	 * Check for 1.3 or later hardware revision.
	 * Prior revisions had marginal CyBus timing.
	 */
	if (hwver < 0x103)
	    return FALSE;
	return TRUE;

      case FCI_FDDIT_CONTROLLER:
	/*
	 * We require FIP hardware revision 2.5 - 2.7 or 2.9 and later
	 * which has a Xylinx that can write to buffer headers anywhere
	 * in the first 64 KB of MEMD. Older versions can only write to
	 * the first 8 KB of MEMD.
	 */
	if ((hwver < 0x0205) || (hwver == 0x0208))
	    return FALSE;
	return TRUE;

      case FCI_SSIP_CONTROLLER:
      case FCI_FSIP_CONTROLLER:
	/*
	 * Hardware versions less than 1.1 will wedge if they do
	 *   not receive a cbus GRANT within 2-3us.
	 */
	if (hwver < 0x101)
	    return FALSE;
	return TRUE;

      case FCI_AIP_CONTROLLER:
	/*
	 * Check for 1.3 or later hardware revision.
	 * Prior revisions had marginal Attention timing.
	 */
	if (hwver < 0x103)
	    return FALSE;
	return TRUE;

      case FCI_CIP_CONTROLLER:
	/*
	 * Check for 4.2 or later hardware revision.
	 */
	if (hwver < 0x402)
	    return FALSE;
	return TRUE;

      case FCI_LINK_CONTROLLER:
      case FCI_CTR_CONTROLLER:
      case FCI_MIP_CONTROLLER:
      case FCI_FEIP_CONTROLLER:
      case FCI_RVIP_CONTROLLER:
      case FCI_SVIP_CONTROLLER:
      case FCI_SMIP_CONTROLLER:
      case FCI_RSP2_CONTROLLER:
      case FCI_RSP4_CONTROLLER:
      case FCI_POSIP_CONTROLLER:
      case FCI_C5RSP2_CONTROLLER:
      case FCI_C5IP_CONTROLLER:
	return TRUE;		/* no restrictions on these cards */

      case FCI_FDDI_CONTROLLER:
      case FCI_SERIAL_CONTROLLER:
      case FCI_SSP_CONTROLLER:
      case FCI_SP_CONTROLLER:
      case FCI_RP_CONTROLLER:
      case FCI_RSP1_CONTROLLER:
      case FCI_RSP7000_CONTROLLER:
      case FCI_CI_CONTROLLER:
      case FCI_CI7000_CONTROLLER:
      case FCI_CTR_SRS_CONTROLLER:
      case FCI_FDDIT_SRS_CONTROLLER:
      case FCI_UNKNOWN_CONTROLLER:
      case FCI_NUM_CONTROLLERS:
	return FALSE;		/* not applicable as 75xx IPs */
    }
    return FALSE;
}

/*
 * dbus_check_crash:
 * Read and save VIPER crash context 
 */
static void dbus_check_crash (dgtype *dbus, int slot)
{
    uchar data, *ptr;
    int i;

    slots[slot].crash_timestamp = unix_time();
    ptr = slots[slot].crash_cntxt;
    slots[slot].crash_count++;

    for (i = 0; i< DBUS_CRASH_LEN; i++) {
	data = dbus_io_reg_read(slot, DBUS_IO_CRASH);
	if (ptr) {
	    /*
	     * Crash context has been malloc'ed
	     */
	    *ptr++ = data;
	}
	if (dbus_prot_rd(&dbus->diag_status) & DBUS_INTERNAL_ERROR) 
	    break;
    }
}


/*
 * dbus_check_msg
 */
boolean dbus_check_msg (dgtype *dbus, int slot)
{
    static message msg;
    uchar data, *ptr;
    int *len;
    uchar io_status;

    /*
     * check if we're still OK
     */
    if (dbus_prot_rd(&dbus->diag_status) & DBUS_INTERNAL_ERROR) 
	return FALSE;
    ptr = 0;

    /* 
     * check dbus hardware register 1st 
     */
    if (!(dbus_prot_rd(&dbus->diag_status) & DBUS_IO_STATUS_SET)) 
	return FALSE;
    io_status = dbus_io_status_reg_read(slot);

    if (io_status & DBUS_IO_CRASH) {
	dbus_check_crash(dbus, slot);
    }
    if (!(io_status & DBUS_IO_MSG))
	return FALSE;

    if (slots[slot].msg_new) {
	slots[slot].msg_len = 0;
	if ((slots[slot].msg_severity = dbus_io_reg_read(slot, DBUS_IO_MSG)) != 0) {
	    /*
	     * If the dbus h/w responds before the dbus handler, the
	     * data will be 0xff.
	     */
	    if (slots[slot].msg_severity == 0xff)
	          return FALSE; 
	    if (slots[slot].msg_severity != DBUS_IO_BUGINF)  {
		/*
		 * Normally msg.severity is a value between 0 - 7
		 * but since a read of 0 indicates no data,
		 * the CIP dbus is adding 1 to the real severity.
		 * So here we must subtract 1.
		 */
		slots[slot].msg_severity = slots[slot].msg_severity - 1;  
	    }
	    slots[slot].msg_new = FALSE;
	} else {
	    slots[slot].msg_new = TRUE;
	    return FALSE;
	}
    }
    
    if (dbus_prot_rd(&dbus->diag_status) & DBUS_INTERNAL_ERROR) 
	return FALSE;

    data = dbus_io_reg_read(slot, DBUS_IO_MSG);
    len = &slots[slot].msg_len;
    ptr = &slots[slot].msg[*len];
    while (data != DBUS_IO_EMPTY) {
	if (data == 0xff) {
	    return FALSE;
	}

	    *ptr++ = data;
	    *len = *len + 1;
	    if ((slots[slot].msg_severity != DBUS_IO_BUGINF) &&
		(data == '%')) {
		*ptr++ = '%';
		*len = *len + 1;
	    }
	    if (*len >= DBUS_IO_MAXLEN-2) 
		break;

	if (dbus_prot_rd(&dbus->diag_status) & DBUS_INTERNAL_ERROR) 
	    return FALSE;
	data = dbus_io_reg_read(slot, DBUS_IO_MSG);
    }
    
    /*
     * log message 
     */
    slots[slot].msg_timestamp = unix_time();
    *ptr = '\0';
    if (slots[slot].msg_severity != DBUS_IO_BUGINF) { 
	memset(&msg, 0,sizeof(msg));
	msg.severity = slots[slot].msg_severity;
	if (msg.severity > 7) {
	    buginf("\nInvalid severity (%d) [%s]\n", msg.severity, slots[slot].msg);
	    msg.severity = 0;
	}
	msg.fac = slots[slot].msg_name;
	msg.name = "MSG";
	msg.format = slots[slot].msg;
	errmsg(&msg);
    } else {
	buginf("\n%%%s: %s", slots[slot].msg_name, slots[slot].msg);
    }
    if (*len >= DBUS_IO_MAXLEN-2) {
	/* use same severity for next message */
	*len = 0;
    } else {
	slots[slot].msg_new = TRUE;
    }
    
    return TRUE;
}
  
void dbus_logger_drain (uint slot)
{
    dgtype *dbus;
    ushort card_type;
    boolean ok_to_suspend;

    ok_to_suspend = process_suspends_allowed();
    dbus = (dgtype*) pslot_to_dbus(slot);
    card_type = dbus_controller_type(dbus);
    if (dbus_io_regs(card_type)) {
	while (dbus_check_msg(dbus, slot)) {
	    if (ok_to_suspend) {
		process_sleep_for(DBUS_LOGGER_DELAY);
	    } else {
		DELAY(DBUS_LOGGER_DELAY);
	    }
	}
    }
}


/* 
 * Slot subsystem header
 */

#define SLOT_MAJVERSION 1
#define SLOT_MINVERSION 0
#define SLOT_EDITVERSION 1

SUBSYS_HEADER(slot, SLOT_MAJVERSION, SLOT_MINVERSION, SLOT_EDITVERSION,
	      slot_init, SUBSYS_CLASS_KERNEL,
	      NULL,
	      NULL);



static void dbus_init (subsystype* subsys)
{
    c7000_chassis_type = D_chassisType_c7000;  /* default in case no SP/SSP */

    /*
     * Register some functions
     */
    reg_add_onesec(dbus_error_scan, "dbus_error_scan");

    /*
     * Initialize dbus parser routines
     */
    dbus_parser_init();

    dgbus_analyze(FALSE);	/* initial scan, EOIR not active */
}

/* 
 * dbus subsystem header
 */

#define DBUS_MAJVERSION 1
#define DBUS_MINVERSION 0
#define DBUS_EDITVERSION 1

SUBSYS_HEADER(dbus, DBUS_MAJVERSION, DBUS_MINVERSION, DBUS_EDITVERSION,
	      dbus_init, SUBSYS_CLASS_DRIVER,
	      "seq: rp_flash_soft flash_services_wrappers flash_devices",
	      NULL);
