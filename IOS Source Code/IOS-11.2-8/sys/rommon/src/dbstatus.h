/* $Id: dbstatus.h,v 3.3 1996/01/17 23:31:02 mansonw Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/rommon/src/dbstatus.h,v $
 *------------------------------------------------------------------
 * dbstatus.h
 *
 * Oct, 95, Steve J. Zhang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dbstatus.h,v $
 * Revision 3.3  1996/01/17  23:31:02  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.2  1995/11/17  18:41:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:23:42  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  23:11:15  szhang
 * placeholder for DBUS handlers in RSP2
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __DBSTATUS_H__
#define __DBSTATUS_H__

#include "dbus.h"

/*
 * Write data to hardware.
 */
static inline void write_data (uchar c)
{
    db_hw->status = (ushort) c;
    /* fill the status value back to status register */
    /* This is due to the same status register being used */
    /* to return data and status in RSP2 DBUS FPGA */
    db_hw->status = (ushort) dbuscore_p->db.r.regs.status;
}

/*
 * Write status to hardware.
 */
static inline void write_status (void)
{
    db_hw->status = (ushort) dbuscore_p->db.r.regs.status;
}

/*
 * Get status from hardware and put it in dbuscore.
 */
static inline void read_status (void)
{
    dbuscore_p->db.r.regs.status = db_hw->status;
}

/*
 * Clear the hardware and dbuscore status registers.
 */
static inline void clear_status (void)
{ 
    dbuscore_p->db.r.regs.port_status.reserved = 0; 
    dbuscore_p->db.r.regs.port_status.out_avail = 0; 
    dbuscore_p->db.r.regs.port_status.debug_act = 0; 
    dbuscore_p->db.r.regs.port_status.console_act = 0; 
    dbuscore_p->db.r.regs.status = 0;
    write_status();
}

/*
 * Set Internal error and 0 bit in hardware status register.
 */
static inline void internal_error (void) 
{ 
    /* Turn on bits 0 and 6.  6 indicates internal error.  0 
       indicates that DBUS is setting the bit, i.e. It is not
       a command fail setting of the bit */
    dbuscore_p->db.r.regs.status |= INTERNAL_ERROR_PLUS_ONE;
    write_status();
}

/*
 * Set wcs parity error in hardware status register.
 */
static inline void wcs_parity_error (void)
{
    dbuscore_p->db.r.regs.status |= WCS_PARITY_ERROR;
    write_status();
}

/*
 * Set wcs instruction error in hardware status register.
 */
static inline void wcs_instr_error (void)
{
    dbuscore_p->db.r.regs.status |= WCS_CNTRL_INSTR_ERROR;
    write_status();
}

/*
 * Set invalid request error in hardware status register.
 */
static inline void dbus_invalid_request_error (int write)
{
    dbuscore_p->db.r.regs.status |= DBUS_INVALID_REQUEST;
    if(write)
	write_status();
}


/*
 * Turn port io avail bit off in hardware status register.
 * The RP periodicaly reads the dbus status register.  It
 * checks the DBUS_PORT_IO_AVAIL bit to know whether we have
 * messages pending for it to read.
 */
static inline void port_io_not_avail (void)
{
    dbuscore_p->db.r.regs.status &= ~DBUS_PORT_IO_AVAIL;
    write_status();
}

/*
 * Turn port io avail bit on in hardware status register.
 * The RP periodicaly reads the dbus status register.  It
 * checks the DBUS_PORT_IO_AVAIL bit to know whether we have
 * messages pending for it to read.
 */
static inline void port_io_avail (void)
{
    dbuscore_p->db.r.regs.status |= DBUS_PORT_IO_AVAIL;
    write_status();
}

/*
 * Let RP know that the CIP is ready.  This is done by
 * turning DBUS_CIP_READY on in the hardware status register.
 */
static inline void status_rsp_ready (int write)
{
    dbuscore_p->db.r.regs.status |= DBUS_RSP_READY;
    if(write)
	write_status();
}

/* 
 * Let RP know that the CIP is not ready.  This is done by
 * turning DBUS_CIP_READY off in the hardware status register.
 */
static inline void status_rsp_not_ready (int write)
{
    dbuscore_p->db.r.regs.status &= ~DBUS_RSP_READY;
    if(write)
	write_status();
}

#endif __DBSTATUS_H_











