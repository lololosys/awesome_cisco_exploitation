/* $Id: vip_ukernel.c,v 3.1.20.3 1996/07/10 02:38:15 getchell Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/vip_ukernel.c,v $
 *------------------------------------------------------------------
 * vip_ukernel.c -- VIP specific routines necessary to build
 *		    sub_ukernel.o 
 *
 * David Hampton, February 1996
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vip_ukernel.c,v $
 * Revision 3.1.20.3  1996/07/10  02:38:15  getchell
 * Added crash context mechanism which is shown via diag command and test
 * cbus (vc command)
 * CSCdi60415:  VIP difficult to debug in the field
 * Branch: California_branch
 *
 * Revision 3.1.20.2  1996/03/21  23:52:39  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.20.1  1996/03/18  22:03:52  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.10.1  1996/03/09  05:13:27  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.1  1996/02/21  21:52:35  hampton
 * Placeholders for Port Ready.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */

#include "master.h"
#include "interface_private.h"
#include "ttysrv.h"
#include "logger.h"

#include "../hes/dgbus.h"
#include "../os/gdb.h"
#include "../pas/if_pas.h"
#if defined(RVIP)
#include "../src-rvip/rvip_dbus.h"
#endif
#if defined(SVIP)
#include "../src-svip/svip_dbus.h"
#endif
#include "../src-vip/vip_dbus.h"

/*
 * The number of tracebacks we try and get after a crash
 */
#define CRASH_TRACEBACK_COUNT   10

/*
 * Global storage
 */
dbus_cntl_t dbus_cntl = {};
void *vip_dbus_context = NULL;

/*
 * vip_board_revision:
 * Return the hardware revision number (hardware_version_high)
 */
uint vip_board_revision (void)
{
    return (dbus_cntl.r.regs.eeprom.hardware_version_high);
}

/*
 * vip_set_ready :
 * Tell the RP/RSP that we are ready
 */
void vip_set_ready (boolean sense)
{
    leveltype level;

    GLOBAL_INTERRUPT_BLOCK(level);
    if (sense) {
	dbus_cntl.r.regs.diag_status.value |= DBUS_SW_READY;
    } else {
	dbus_cntl.r.regs.diag_status.value &= ~DBUS_SW_READY;
    }
    dbus_cntl.dbus_reg->status = dbus_cntl.r.regs.diag_status.value;
    GLOBAL_INTERRUPT_UNBLOCK(level);
}

/*
 * vip_set_internal_error:
 * Signal an internal error
 */
void vip_set_internal_error (void)
{
    leveltype level;

    GLOBAL_INTERRUPT_BLOCK(level);
    dbus_cntl.r.regs.diag_status.value |= INTERNAL_ERROR;
    dbus_cntl.dbus_reg->status = dbus_cntl.r.regs.diag_status.value;
    GLOBAL_INTERRUPT_UNBLOCK(level);
}

/*
 * vip_dbus_txempty:
 * Return whether the DBUS tx ring is completely empty
 */
boolean vip_dbus_txempty (void)
{
    leveltype level;
    boolean ret;

    GLOBAL_INTERRUPT_BLOCK(level);
    ret = (dbus_cntl.tx_h == dbus_cntl.tx_t);
    GLOBAL_INTERRUPT_UNBLOCK(level);
    return(ret);
}

/*
 * vip_context_dump:
 * 
 * Dump context to console and logger port.
 */
void vip_context_dump (int sig, int code, void *c)
{
    int i;
    ulong *pc, *frame;
    r4k_context_t *ptr = (r4k_context_t *)c;
    char trace_msg[130];
    char *trace_msg_ptr = trace_msg;
    
    *(volatile ulong *)(R4K_K1BASE + 0x24C) = 0x22222291;
    
    if (ptr) {

	/*
	 * Print out a message and CPU context
	 */
	ttyprintf(CONTTY, 
		  "\nSystem exception: sig=%d, code=%#x, context=%#x\n",
		  sig, code, ptr);
	sprintf(trace_msg, "System exception: sig=%d, code=%#x, context=%#x",
		sig, code, ptr);
	vip_logger_write_str(LOG_ALERT, trace_msg);
	for (i=0; i<8; i++) {
	    ttyprintf(CONTTY, 
		      "%s : %08x, %s : %08x, %s : %08x, %s : %08x, \n",
		      r4k_reg_name[(i*4)],   ptr->regs[(i*8)+1],
		      r4k_reg_name[(i*4)+1], ptr->regs[(i*8)+3],
		      r4k_reg_name[(i*4)+2], ptr->regs[(i*8)+5],
		      r4k_reg_name[(i*4)+3], ptr->regs[(i*8)+7]);
	    sprintf(trace_msg, 
		    "%s : %08x, %s : %08x, %s : %08x, %s : %08x,",
		    r4k_reg_name[(i*4)],   ptr->regs[(i*8)+1],
		    r4k_reg_name[(i*4)+1], ptr->regs[(i*8)+3],
		    r4k_reg_name[(i*4)+2], ptr->regs[(i*8)+5],
		    r4k_reg_name[(i*4)+3], ptr->regs[(i*8)+7]);
	    vip_logger_write_str(LOG_ALERT, trace_msg);
	}
	ttyprintf(CONTTY, "\nEPC : 0x%08x, SREG : 0x%08x, Cause : 0x%08x\n",
		  ptr->epc, ptr->sreg, ptr->cause);
	sprintf(trace_msg, "EPC : 0x%08x, SREG : 0x%08x, Cause : 0x%08x",
		ptr->epc, ptr->sreg, ptr->cause);
	vip_logger_write_str(LOG_ALERT, trace_msg);
	ttyprintf(CONTTY, "ErrorEPC : 0x%08x\n\n", ptr->err_epc);
	sprintf(trace_msg, "ErrorEPC : 0x%08x", ptr->err_epc);
	vip_logger_write_str(LOG_ALERT, trace_msg);

	/*
	 * See if we can get a stack trace using the EPC register
	 */
	pc = (ulong *)ptr->epc;
	frame = (ulong *)ptr->regs[(29*2)+1];
	ttyprintf(CONTTY, "Attempting traceback with FP=%#x, PC=%#x", 
		  frame, pc);
	sprintf(trace_msg, "Attempting traceback with FP=%#x, PC=%#x", 
		frame, pc);
	vip_logger_write_str(LOG_ALERT, trace_msg);
	trace_msg_ptr = trace_msg;
	for (i=0; i<CRASH_TRACEBACK_COUNT; i++) {
	    frame = trace_caller(frame, &pc);
	    if (frame == NULL)
		break;
	    if (!i) {
		ttyprintf(CONTTY, "\n-Traceback= ");
		trace_msg_ptr += sprintf(trace_msg_ptr, "Traceback= ");
	    }
	    ttyprintf(CONTTY, "0x%8x ",pc);
	    trace_msg_ptr += sprintf(trace_msg_ptr, "0x%8x ", pc);
	}
	if (!i) {
	    pc = (ulong *)r4k_get_cpu_reg(ptr, R4K_RA_REG);
	    frame = (ulong *)r4k_get_cpu_reg(ptr, R4K_SP_REG);
	    ttyprintf(CONTTY, " ... Failed\n");
	    sprintf(trace_msg, "Traceback ... Failed");
	    vip_logger_write_str(LOG_ALERT, trace_msg);
	    ttyprintf(CONTTY, "Attempting traceback with FP=%#x, PC=%#x", 
		      frame, pc);
	    sprintf(trace_msg, "Attempting traceback with FP=%#x, PC=%#x", 
		     frame, pc);
	    vip_logger_write_str(LOG_ALERT, trace_msg);
	    trace_msg_ptr = trace_msg;
	    for (i=0; i<CRASH_TRACEBACK_COUNT; i++) {
		frame = trace_caller(frame, &pc);
		if (frame == NULL)
		    break;
		if (!i) {
		    ttyprintf(CONTTY, "\n-Traceback= ");
		    trace_msg_ptr += sprintf(trace_msg_ptr, "Traceback= ");
		}
		ttyprintf(CONTTY, "0x%8x ",pc);
		trace_msg_ptr += sprintf(trace_msg_ptr, "0x%8x ", pc);
	    }
	}
	if (!i) {
	    ttyprintf(CONTTY, " ... Failed\n");
	    sprintf(trace_msg, "Traceback ... Failed");
	    vip_logger_write_str(LOG_ALERT, trace_msg);
	} else {
	    ttyprintf(CONTTY, "\n");
	    vip_logger_write_str(LOG_ALERT, trace_msg);
	}

    }
}
