/* $Id: svip_dbus.h,v 3.1.62.1 1996/03/21 23:51:25 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-svip/svip_dbus.h,v $
 *------------------------------------------------------------------
 * svip_dbus.h : SVIP DBus support
 *
 * September 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_dbus.h,v $
 * Revision 3.1.62.1  1996/03/21  23:51:25  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.1  1996/02/10  00:22:00  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/26  21:18:38  getchell
 * Placeholder files for Viper development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SVIP_DBUS_H__
#define __SVIP_DBUS_H__

#ifndef ASMINCLUDE

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
    uchar           pad0[6];
    volatile ushort physical_slot;     /* reg0 - physical slot         0x06 */
    uchar           pad1[6];
    volatile ushort top_reg1;          /* reg1                         0x0E */
    uchar           pad2[6];
    volatile ushort status;            /* reg2 - status register       0x16 */
    uchar           pad3[6];
    volatile ushort top_reg3;          /* reg3                         0x1E */
    uchar           pad4[6];
    volatile ushort logical_slot;      /* reg4 - logical slot          0x26 */
    uchar           pad5[6];
    volatile ushort reset_reg;         /* reg5 - reset register VIP    0x2E */
                                       /* scratch pad, not dbus             */
    uchar           pad6[6];
    volatile ushort unused1;           /* not used on Viper            0x36 */
    uchar           pad7[6];
    data_port_type data_port;          /* reg7 - dbus data port        0x3E */
} dbus_hardware_regs;

/*
 * Prototypes
 */
extern void install_svip_dbus_handler(void);

#endif
 
#endif /* !__SVIP_DBUS_H__ */

