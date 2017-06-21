/* $Id: rsp_macaddr.c,v 3.4.28.1 1996/03/21 23:32:28 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-rsp/rsp_macaddr.c,v $
 *------------------------------------------------------------------
 * rsp_macaddr.c 
 *
 * 05 95, mansonw( Man Sang )
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_macaddr.c,v $
 * Revision 3.4.28.1  1996/03/21  23:32:28  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/02/08  08:59:08  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1995/12/01  04:24:19  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.4  1996/01/17  23:31:50  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.3  1996/01/10  03:30:44  tkam
 * CSCdi34476:  r4k ROM monitor doesnt save crashed system version
 * Bug fixes and adding RSP7000 support.
 *
 * Revision 3.2  1995/11/17  18:46:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:09:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:41:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "monitor.h"
#include "dgbus.h"
#include "cookie.h"
#include "mon_ralib.h"

/*
 * DS1201 definitions
 */
#define DS1201_NSEC_TDC		35	/* DATA to CLOCK setup */
#define DS1201_NSEC_TCL		125	/* CLOCK low */
#define DS1201_NSEC_TCH		125	/* CLOCK high */
#define DS1201_NSEC_TCC		1000	/* RESET to CLOCK setup */
#define DS1201_1USEC            1000    /* 1 us delay */

#define DS1201_READ_CMD		0x62	/* binary 01100010 */
#define DS1201_WRITE_CMD	0x9d	/* binary 10011101 */

#define DS1201_BURST_ADDR	0x8000

#define DS1201_BURST_BYTES	128	/* 1024 bits */

#define IEEEBYTES               6
#define R4K_COUNT_PERIOD_NS     20       /* 50MHz / 20ns */

#define MAX_MODEL_TYPE          6
#define DS1201_VERSION	        1

static const char *chass_model[MAX_MODEL_TYPE] = {"7506","7505","7513","----","7507",
						  "RSP7000"};
/*
 * DRAM copy of DS1201 NVRAM contents
 */
struct ds1201_format 
{
    uchar version;
    uchar model;			/* 7500, 7510, etc. */
    uchar serial[4];
    uchar mac_base[IEEEBYTES];
    uchar mac_block_size[2];
    uchar :8;			/* reserved */
    uchar :8;			/* reserved */
    uchar RMA_failure;
    uchar RMA_number[3];
    uchar mfg_date[3];
};


static inline ulong getcp0_count_inline (void)
{
    ulong reg;

    asm volatile("mfc0 %0, $9" : "=r" (reg));
    return reg;
}


static inline zero_cp0_count_inline (void)
{
    asm volatile("mtc0 $0, $9");
}


/*
 * ieee_copy
 */
static inline void ieee_copy (uchar const *src, uchar *dst)
{
    PUTSHORT(dst+0, GETSHORT(src+0));
    PUTLONG(dst+2,  GETLONG(src+2));
}


/*
 * very approximate nanosecond-based spin-wait routine
 */
static void nsecdelay (uint nanoseconds)
{
    ulong target, count;

    zero_cp0_count_inline();
    target = (nanoseconds + (R4K_COUNT_PERIOD_NS - 1)) / R4K_COUNT_PERIOD_NS;

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
static uchar ds1201_sysctl (void)
{
    return(REG_FPGA->sys_ctrl_reg & ~(CONTROL_DALLAS_CLK |
		                     CONTROL_DALLAS_DATA |
			             CONTROL_DALLAS_RESET |
			             CONTROL_DALLAS_WRITE));
}

/*
 * reset and enable the DS1201
 */
static void ds1201_reset (uchar sysctl_value)
{
    REG_FPGA->sys_ctrl_reg = sysctl_value | CONTROL_DALLAS_RESET;
    REG_FPGA->sys_ctrl_reg;
    REG_FPGA->sys_ctrl_reg = sysctl_value;
    REG_FPGA->sys_ctrl_reg;			/* flush write */
    nsecdelay(DS1201_NSEC_TCC);
    return;
}

/*
 * write a byte to the DS1201
 */
static void ds1201_write (uchar sysctl_value, uchar write_value)
{
    uchar bit, nbits;

    sysctl_value |= CONTROL_DALLAS_WRITE;
    REG_FPGA->sys_ctrl_reg = sysctl_value;
    REG_FPGA->sys_ctrl_reg;
    nsecdelay(DS1201_1USEC);

    for (nbits = 0; nbits < 8; ++nbits) {
	if (write_value & (1 << nbits))
	    bit = CONTROL_DALLAS_DATA;
	else
	    bit = 0;

	REG_FPGA->sys_ctrl_reg = sysctl_value | bit;
	REG_FPGA->sys_ctrl_reg;		/* flush write */
	nsecdelay(DS1201_NSEC_TDC);

	REG_FPGA->sys_ctrl_reg = sysctl_value | bit | CONTROL_DALLAS_CLK;
	REG_FPGA->sys_ctrl_reg;		/* flush write */
	nsecdelay(DS1201_NSEC_TCH);

	REG_FPGA->sys_ctrl_reg = sysctl_value | bit;
	REG_FPGA->sys_ctrl_reg;		/* flush write */
	nsecdelay(DS1201_NSEC_TCL);
    }
    return;
}

/*
 * read a byte from the DS1201
 */
static uchar ds1201_read (uchar sysctl_value)
{
    uchar bit, byte, nbits;

    REG_FPGA->sys_ctrl_reg = sysctl_value;
    REG_FPGA->sys_ctrl_reg;
    nsecdelay(1000);  

    byte = 0;
    for (nbits = 0; nbits < 8; ++nbits) {
	bit = REG_FPGA->sys_ctrl_reg & CONTROL_DALLAS_DATA;
	if (bit)
	    byte |= (1 << nbits);

	REG_FPGA->sys_ctrl_reg = sysctl_value | CONTROL_DALLAS_CLK;
	REG_FPGA->sys_ctrl_reg;		/* flush write */
	nsecdelay(DS1201_NSEC_TCH);

	REG_FPGA->sys_ctrl_reg = sysctl_value;
	REG_FPGA->sys_ctrl_reg;		/* flush write */
	nsecdelay(DS1201_NSEC_TCL);
    }
    return byte;
}

/*
 * assert the DS1201 reset signal
 */
static void ds1201_disable (uchar sysctl_value)
{
    REG_FPGA->sys_ctrl_reg = sysctl_value | CONTROL_DALLAS_RESET;
    return;
}


/*
 * print the content of the dallas data
 */
void prt_ds1201(const struct ds1201_format *ds1201_copy)
{
    int i;

    printf("EEPROM Version: %d\n", ds1201_copy->version);

    if (ds1201_copy->model >= MAX_MODEL_TYPE)
	printf("Model : Unknown\n");
    else
	printf("Model : %s\n", chass_model[ds1201_copy->model]);

    printf("Serial Number: %d\n", (ds1201_copy->serial[0] << 24) + 
	                          (ds1201_copy->serial[1] << 16) +
	                          (ds1201_copy->serial[2] << 8) +
	                          (ds1201_copy->serial[3]));

    printf("MAC base : ");
    for (i = 0; i < IEEEBYTES; ++i)
        printf("%.2x ",ds1201_copy->mac_base[i]);

    printf("\nMAC block size : %d\n", ds1201_copy->mac_block_size[0] << 8
	   + ds1201_copy->mac_block_size[0]);

    printf("RMA failure : %d \n", ds1201_copy->RMA_failure);

    printf("RMA Number : %d\n", (ds1201_copy->RMA_number[0] << 16) +
	                       (ds1201_copy->RMA_number[1] << 8) +
	                       ds1201_copy->RMA_number[2]);

    printf("mfg_date : %.2d %.2d %.2d\n", ds1201_copy->mfg_date[0],
	                                  ds1201_copy->mfg_date[1],
	                                  ds1201_copy->mfg_date[2]);
    return;
}


/*
 * read in data (e.g. MAC addresses) from backplane NVRAM
 */
void dump_ds1201 (void)
{
    uint ix;
    uchar sysctl_value;
    uchar* ucp;
    struct ds1201_format ds1201_copy;

    sysctl_value = ds1201_sysctl();
    ds1201_reset(sysctl_value);
    ds1201_write(sysctl_value, DS1201_READ_CMD);
    ds1201_write(sysctl_value, DS1201_BURST_ADDR & 0xff);
    ds1201_write(sysctl_value, DS1201_BURST_ADDR >> 8);

    ucp = (uchar*) &ds1201_copy;
    for (ix = 0; ix < sizeof(ds1201_copy); ++ix)
	*ucp++ = ds1201_read(sysctl_value);
    ds1201_disable(sysctl_value);
    prt_ds1201(&ds1201_copy);
    return;
}










