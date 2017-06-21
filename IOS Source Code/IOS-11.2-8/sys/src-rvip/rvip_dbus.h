/* $Id: rvip_dbus.h,v 3.1.62.1 1996/03/21 23:50:45 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-rvip/rvip_dbus.h,v $
 *------------------------------------------------------------------
 * rvip_dbus.h : RVIP DBus support
 *
 * September 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rvip_dbus.h,v $
 * Revision 3.1.62.1  1996/03/21  23:50:45  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.1  1996/02/10  00:19:05  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/27  20:19:46  getchell
 * Placeholder for VIPER development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __RVIP_DBUS_H__
#define __RVIP_DBUS_H__

typedef struct data_port_type_ {
    volatile uchar dbus_data;     /* On reads, the high byte is the data */
    union {
	volatile uchar addrs;     /* On reads, the low byte is the address */
	volatile uchar data_out;  /* On writes, the low byte is data */
    } ad;
} data_port_type;

/*
 * DBus Hardware registers
 */
typedef struct dbus_hardware_regs_ {
    volatile ushort physical_slot;    	/* reg0 - physical slot         0x00 */
    volatile ushort pad0;
    volatile ushort top_reg1;          	/* reg1		       	        0x04 */
    volatile ushort pad1;
    volatile ushort status;            	/* reg2 - status register       0x08 */
    volatile ushort pad2; 
    volatile ushort top_reg3;          	/* reg3 		        0x0C */
    volatile ushort pad3;                   
    volatile ushort logical_slot;     	/* reg4 - logical slot          0x10 */
    volatile ushort pad4;    
    volatile ushort reset_reg;        	/* reg5 - reset register VIP    0x14 */
				      	/* scratch pad, not dbus 	     */
    volatile ushort pad5;
    volatile ushort cbus_attn_clear;  	/* reg6 - clear CBUS attn, intr	0x18 */
    volatile ushort pad6;       
    data_port_type data_port;   	/* reg7 - dbus data port 	0x1C */
    volatile ushort pad7;
} dbus_hardware_regs;

/*
 * Prototypes 
 */
extern void install_rvip_dbus_handler(void);

#endif /* !__RVIP_DBUS_H__ */
