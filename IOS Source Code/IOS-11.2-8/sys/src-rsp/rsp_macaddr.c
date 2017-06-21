/* $Id: rsp_macaddr.c,v 3.2.58.1 1996/08/28 13:14:12 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_macaddr.c,v $
 *------------------------------------------------------------------
 * read MAC addresses from NVRAM on backplane
 *
 * December 1994, Carl Schaefer
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This NVRAM contains MAC addresses and other informantion programmed
 * by manufacturing, as well as saved environmental measurements
 * written by the system code.
 *------------------------------------------------------------------
 * $Log: rsp_macaddr.c,v $
 * Revision 3.2.58.1  1996/08/28  13:14:12  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  18:48:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:28:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:58:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "ttysrv.h"
#include "logger.h"
#include "../src-rsp/msg_rsp.c"		/* Not a typo, see logger.h */
#include "config.h"
#include "ieee.h"
#include "rsp_pcmap.h"
#include "rsp_macaddr.h"
#include "../snmp/sr_old_lchassismib.h"

/*
 * public data
 */
uchar chassis_model;
uint chassis_serial;

/*
 * DS1201 definitions
 */
#define DS1201_NSEC_TDC		35	/* DATA to CLOCK setup */
#define DS1201_NSEC_TCL		125	/* CLOCK low */
#define DS1201_NSEC_TCH		125	/* CLOCK high */
#define DS1201_NSEC_TCC		1000	/* RESET to CLOCK setup */
#define DS1201_NSEC_TCCH	40	/* CLOCK to RESET hold */
#define DS1201_NSEC_TCWH	125	/* RESET inactive */

#define DS1201_READ_CMD		0x62	/* binary 01100010 */
#define DS1201_WRITE_CMD	0x9d	/* binary 10011101 */

#define DS1201_BURST_ADDR	0x8000

/*
 * DRAM copy of DS1201 NVRAM contents
 */
struct {
    uchar version;
    uchar model;			/* 7500, 7510, etc. */
    uchar serial[4];
    uchar mac_base[IEEEBYTES];
    uchar mac_block_size[2];
    uchar :8;			/* reserved */
    uchar :8;			/* reserved */
    uchar RMA_failure;
    uchar RMA_number[3];
    uchar mfg_date;
} ds1201_copy;

#define DS1201_VERSION	1

/*
 * MAC addr info
 */
static uint mac_count;
static uchar mac_base[IEEEBYTES];

/*
 * very approximate nanosecond-based spin-wait routine
 */
static void
nsecdelay (uint nanoseconds)
{
    ulong target, count;

    target = getcp0_count_inline();
    target += (nanoseconds + (R4K_COUNT_PERIOD_NS - 1)) / R4K_COUNT_PERIOD_NS;
    for (;;) {
	count = getcp0_count_inline();
	if ((signed) (target - count) <= 0)
	    break;
    }
    return;
}

/*
 * get SYSCONTROL value to use with DS1201 interface routines
 */
static uchar
ds1201_sysctl (void)
{
    volatile uchar *sysctl;
    uchar sysctl_value;

    sysctl = (uchar*) ADRSPC_SYSCONTROL;
    sysctl_value = *sysctl & ~(CONTROL_DALLAS_CLK |
			       CONTROL_DALLAS_DATA |
			       CONTROL_DALLAS_RESET |
			       CONTROL_DALLAS_WRITE);
    return sysctl_value;
}

/*
 * reset and enable the DS1201
 */
static void
ds1201_reset (uchar sysctl_value)
{
    volatile uchar *sysctl;

    sysctl = (uchar*) ADRSPC_SYSCONTROL;
    *sysctl = sysctl_value | CONTROL_DALLAS_RESET;
    *sysctl;			/* flush write */
    nsecdelay(DS1201_NSEC_TCL);
    *sysctl = sysctl_value;
    *sysctl;			/* flush write */
    nsecdelay(DS1201_NSEC_TCC);
    return;
}

/*
 * write a byte to the DS1201
 */
static void
ds1201_write (uchar sysctl_value, uchar write_value)
{
    uchar bit, nbits;
    volatile uchar *sysctl;

    sysctl = (uchar*) ADRSPC_SYSCONTROL;
    sysctl_value |= CONTROL_DALLAS_WRITE;
    *sysctl = sysctl_value;
    *sysctl;
    usecdelay(1);

    for (nbits = 0; nbits < 8; ++nbits) {
	if (write_value & (1 << nbits))
	    bit = CONTROL_DALLAS_DATA;
	else
	    bit = 0;

	*sysctl = sysctl_value | bit;
	*sysctl;		/* flush write */
	nsecdelay(DS1201_NSEC_TDC);

	*sysctl = sysctl_value | bit | CONTROL_DALLAS_CLK;
	*sysctl;		/* flush write */
	nsecdelay(DS1201_NSEC_TCH);

	*sysctl = sysctl_value | bit;
	*sysctl;		/* flush write */
	nsecdelay(DS1201_NSEC_TCL);
    }
    return;
}

/*
 * read a byte from the DS1201
 */
static uchar
ds1201_read (uchar sysctl_value)
{
    volatile uchar *sysctl;
    uchar bit, byte, nbits;

    sysctl = (uchar*) ADRSPC_SYSCONTROL;
    *sysctl = sysctl_value;
    *sysctl;
    usecdelay(1);

    byte = 0;
    for (nbits = 0; nbits < 8; ++nbits) {
	bit = *sysctl & CONTROL_DALLAS_DATA;
	if (bit)
	    byte |= (1 << nbits);

	*sysctl = sysctl_value | CONTROL_DALLAS_CLK;
	*sysctl;		/* flush write */
	nsecdelay(DS1201_NSEC_TCH);

	*sysctl = sysctl_value;
	*sysctl;		/* flush write */
	nsecdelay(DS1201_NSEC_TCL);
    }
    return byte;
}

/*
 * assert the DS1201 reset signal
 */
static void
ds1201_disable (uchar sysctl_value)
{
    volatile uchar *sysctl;

    nsecdelay(DS1201_NSEC_TCCH);
    sysctl = (uchar*) ADRSPC_SYSCONTROL;
    *sysctl = sysctl_value | CONTROL_DALLAS_RESET;
    nsecdelay(DS1201_NSEC_TCWH);
    return;
}

/*
 * read a byte from the DS1201
 */
uchar
ds1201_read_byte (uchar addr)
{
    uchar sysctl_value, value;

    sysctl_value = ds1201_sysctl();
    ds1201_reset(sysctl_value);
    ds1201_write(sysctl_value, DS1201_READ_CMD);
    ds1201_write(sysctl_value, addr);
    ds1201_write(sysctl_value, 0);
    value = ds1201_read(sysctl_value);
    ds1201_disable(sysctl_value);
    return value;
}

/*
 * write any byte in the DS1201
 */
static void
ds1201_write_any_byte (uchar addr, uchar value)
{
    uchar sysctl_value;

    sysctl_value = ds1201_sysctl();
    ds1201_reset(sysctl_value);
    ds1201_write(sysctl_value, DS1201_WRITE_CMD);
    ds1201_write(sysctl_value, addr);
    ds1201_write(sysctl_value, 0);
    ds1201_write(sysctl_value, value);
    ds1201_disable(sysctl_value);
    return;
}

/*
 * write any byte in the DS1201 not reserved to manufacturing
 */
void
ds1201_write_byte (uchar addr, uchar value)
{
    if (addr <= DS1201_MFG_TOP)
	return;
    ds1201_write_any_byte(addr, value);
    return;
}

/*
 * read & write the DS1201
 */
void
test_rsp_ds1201 (parseinfo *csb)
{
    uchar addr, value;

    addr = GETOBJ(int,2);
    switch (GETOBJ(int,1)) {
      case TEST_RSP_DS1201_READ:
	value = ds1201_read_byte(addr);
	printf("Value: %02x", value);
	break;

      case TEST_RSP_DS1201_WRITE:
	value = GETOBJ(int,3);
	ds1201_write_any_byte(addr, value);
	break;
    }
    return;
}

/*
 * read in data (e.g. MAC addresses) from backplane NVRAM
 */
void
macaddr_init (void)
{
    uint ix;
    uchar sysctl_value;
    uchar* ucp;

    sysctl_value = ds1201_sysctl();
    ds1201_reset(sysctl_value);

    ds1201_write(sysctl_value, DS1201_READ_CMD);
    ds1201_write(sysctl_value, DS1201_BURST_ADDR & 0xff);
    ds1201_write(sysctl_value, DS1201_BURST_ADDR >> 8);

    ucp = (uchar*) &ds1201_copy;
    for (ix = 0; ix < sizeof(ds1201_copy); ++ix)
	*ucp++ = ds1201_read(sysctl_value);

    ds1201_disable(sysctl_value);

    if ((ds1201_copy.version == DS1201_VERSION) &&
	((ds1201_copy.mac_base[0] & 0x01) == 0)) {
	ieee_copy(ds1201_copy.mac_base, mac_base);
	mac_count = GETSHORT(ds1201_copy.mac_block_size);
	chassis_model = ds1201_copy.model;
	chassis_serial = GETLONG(ds1201_copy.serial);
    } else {
	ulong base;

	do
	    base = getcp0_count_inline();
	while (base == 0);

	mac_count = MAX_SLOTS * MAX_CTRLR_INTERFACES;
	mac_base[0] = 0x00;
	PUTLONG(&mac_base[1], base);
	errmsg(&msgsym(COOKIE, RSP), mac_base);
	chassis_model = NO_CHASSIS_MODEL;
	chassis_serial = 0;
    }

    return;
}

/*
 * assign a MAC address to a port in a slot
 */
static void
get_mac_addr (uint slot, uint port, uchar *addr)
{
    ulong value;
    uchar byte2;
    uint offset;

    offset = slot * MAX_CTRLR_INTERFACES + port;
    if (offset >= mac_count) {
	errmsg(&msgsym(NOMAC, RSP), slot, port);
	return;
    }

    value = GETLONG(&mac_base[2]);
    byte2 = value >> 24;
    value += offset;
    if (byte2 != (value >> 24)) {
	errmsg(&msgsym(NOMAC, RSP), slot, port);
	return;
    }

    ieee_copy(mac_base, addr);
    PUTLONG(&addr[2], value);

    return;
}

boolean
rsp_assign_mac_addr (hwidbtype *hwidb)
{
    /*
     * already got one?
     */
    if (!is_ieee_zero(hwidb->bia))
	return TRUE;
    
    get_mac_addr(hwidb->slot, hwidb->slotunit, hwidb->bia);
    ieee_copy(hwidb->bia, hwidb->hardware);
    return TRUE;
}
