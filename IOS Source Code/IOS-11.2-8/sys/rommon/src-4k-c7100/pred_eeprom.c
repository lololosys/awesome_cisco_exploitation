/* $Id: pred_eeprom.c,v 3.1.68.1 1996/03/21 23:31:33 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-c7100/pred_eeprom.c,v $
 *------------------------------------------------------------------
 * pred_eeprom.c -- Predator EEPROM support
 *
 * November 1995, Steve Zhang
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pred_eeprom.c,v $
 * Revision 3.1.68.1  1996/03/21  23:31:33  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.3  1996/01/17  05:54:48  szhang
 * Branch: ELC_branch
 * Fixed the problem with chip select to mid-plane eeprom.
 * NVRAM size is also set in the FPGA I/O control register.
 *
 * Revision 3.1.2.2  1995/12/01  04:23:48  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.1.2.1  1995/11/23  19:14:36  szhang
 * Branch: ELC_branch
 * Initial source code commit to ELC branch for Predator (C7100) ROM
 * monitor.
 *
 * Revision 3.1  1995/11/23  00:55:09  mbeesley
 * Add placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "proto.h"
#include "pred_eeprom.h"
#include "../src/nmc93c46.h"

static pas_management_t c7100_mp_eeprom = {
    -1,						/* Not a bay */

    sizeof(KSEG1_PRED_FPGA->serial_eprom_ctrl),	/* 8 bit register */
    SK2_CLOCK_MIDPLANE,				/* Clock mask */
    &KSEG1_PRED_FPGA->serial_eprom_ctrl,	/* Clock control register */

    sizeof(KSEG1_PRED_FPGA->serial_eprom_ctrl),	/* 8 bit register */
    CS2_CHIP_SEL_MIDPLANE,			/* Select mask */
    &KSEG1_PRED_FPGA->serial_eprom_ctrl,	/* Select control register */

    sizeof(KSEG1_PRED_FPGA->serial_eprom_ctrl), /* 8 bit register */
    DI2_DATA_IN_MIDPLANE,			/* DATAIN mask */
    &KSEG1_PRED_FPGA->serial_eprom_ctrl,	/* DATAIN register */

    sizeof(KSEG1_PRED_FPGA->serial_eprom_ctrl),	/* 8 bit register */
    DO2_DATA_OUT_MIDPLANE,			/* DATAOUT mask */
    &KSEG1_PRED_FPGA->serial_eprom_ctrl		/* DATAOUT register */
};

static pas_management_t c7100_cpu_eeprom = {
    -1,						/* Not a bay */
 
    sizeof(KSEG1_PRED_FPGA->serial_eprom_ctrl),	/* 8 bit register */
    SK1_CLOCK_CPU,				/* Clock mask */
    &KSEG1_PRED_FPGA->serial_eprom_ctrl,	/* Clock control register */
 
    sizeof(KSEG1_PRED_FPGA->serial_eprom_ctrl),	/* 8 bit register */
    CS1_CHIP_SEL_CPU,				/* Select mask */
    &KSEG1_PRED_FPGA->serial_eprom_ctrl,	/* Select control register */
 
    sizeof(KSEG1_PRED_FPGA->serial_eprom_ctrl),	/* 8 bit register */
    DI1_DATA_IN_CPU,				/* DATAIN mask */
    &KSEG1_PRED_FPGA->serial_eprom_ctrl,	/* DATAIN register */
 
    sizeof(KSEG1_PRED_FPGA->serial_eprom_ctrl),	/* 8 bit register */
    DO2_DATA_OUT_CPU,				/* DATAOUT mask */
    &KSEG1_PRED_FPGA->serial_eprom_ctrl		/* DATAOUT register */
};

/*
 * c7100_read_mpeeprom:
 * Read the EEPROM data
 */
static void c7100_read_mpeeprom (c7100_mpeeprom_copy_t *eeprom)
{
    pas_access_sys_eeprom(&c7100_mp_eeprom, eeprom, PAS_EEPROM_NMC93C46,
			  PAS_EEPROM_CMD_READ, (sizeof(c7100_mpeeprom_copy_t) >> 1));
}

/*
 * c7100_write_mpeeprom:
 * Write the EEPROM data
 */
static void c7100_write_mpeeprom (c7100_mpeeprom_copy_t *eeprom)
{
    pas_access_sys_eeprom(&c7100_mp_eeprom, eeprom, PAS_EEPROM_NMC93C46,
			  PAS_EEPROM_CMD_WRITE, 0);

}

/*
 * c7100_read_cpueeprom:
 * Read the EEPROM data
 */
static void c7100_read_cpueeprom (c7100_cpueeprom_copy_t *eeprom)
{
    pas_access_sys_eeprom(&c7100_cpu_eeprom, eeprom, PAS_EEPROM_NMC93C46,
			  PAS_EEPROM_CMD_READ, (sizeof(c7100_cpueeprom_copy_t) >> 1));
}
 
/*
 * c7100_write_cpueeprom:
 * Write the EEPROM data
 */
static void c7100_write_cpueeprom (c7100_cpueeprom_copy_t *eeprom)
{
    pas_access_sys_eeprom(&c7100_cpu_eeprom, eeprom, PAS_EEPROM_NMC93C46,
			  PAS_EEPROM_CMD_WRITE, 0);
}

#define IN_BUFFER_SIZE 128

/*
 * rw_c7100_mpeeprom:
 * read & write the NMC93 Mid plane eeprom
 */
int rw_c7100_mpeeprom (void)
{
    uchar addr, value, *eeprom_ptr;
    c7100_mpeeprom_copy_t eeprom;
    char in_buffer[IN_BUFFER_SIZE];
    ulong ret_val;

    /*
     * Read the complete eeprom
     */
    c7100_read_mpeeprom(&eeprom);
    eeprom_ptr = (uchar *)&eeprom;

    printf("Enter Midplane EPROM Address: ");
    getline(in_buffer, IN_BUFFER_SIZE);
    getnum(in_buffer, 16, &ret_val);
    addr = ret_val;
    
    printf("Read/Write[r/w]: ");
    getline(in_buffer, IN_BUFFER_SIZE);

    if (in_buffer[0] == 'r') {
	value = eeprom_ptr[addr];
	printf("Value: %02x\n", value);
    } else if (in_buffer[0] == 'w') {
	printf("Enter Write Value: ");
	getline(in_buffer, IN_BUFFER_SIZE);
	getnum(in_buffer, 16, &ret_val);
	eeprom_ptr[addr] = ret_val;
	c7100_write_mpeeprom(&eeprom);
    } else {
	printf("Enter Option Error\n");
    }
} 

/*
 * rw_c7100_cpueeprom:
 * read & write the NMC93 CPU assembly eeprom
 */
int rw_c7100_cpueeprom (void)
{
    uchar addr, value, *eeprom_ptr;
    c7100_cpueeprom_copy_t eeprom;
    char in_buffer[IN_BUFFER_SIZE];
    ulong ret_val;
 
    /*
     * Read the complete eeprom
     */
    c7100_read_cpueeprom(&eeprom);
    eeprom_ptr = (uchar *)&eeprom;
 
    printf("Enter CPU EPROM Address: ");
    getline(in_buffer, IN_BUFFER_SIZE);
    getnum(in_buffer, 16, &ret_val);
    addr = ret_val;
 
    printf("Read/Write[r/w]: ");
    getline(in_buffer, IN_BUFFER_SIZE);
 
    if (in_buffer[0] == 'r') {
	value = eeprom_ptr[addr];
	printf("Value: %02x\n", value);
    } else if (in_buffer[0] == 'w') {
	printf("Enter Write Value: ");
	getline(in_buffer, IN_BUFFER_SIZE);
	getnum(in_buffer, 16, &ret_val);
	eeprom_ptr[addr] = ret_val;
	c7100_write_cpueeprom(&eeprom);
    } else {
	printf("Enter Option Error\n");
    }
}

/* end of file */
