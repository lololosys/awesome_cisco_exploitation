/* $Id: nmc93c46.h,v 3.1.68.1 1996/03/21 23:27:42 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/nmc93c46.h,v $
 *------------------------------------------------------------------
 * nmc93c46.h -- NMC93c46 EEPROM Support
 *
 * November 1995, Steve Zhang
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nmc93c46.h,v $
 * Revision 3.1.68.1  1996/03/21  23:27:42  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.2  1995/12/01  04:20:48  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.1.2.1  1995/11/23  19:14:11  szhang
 * Branch: ELC_branch
 * Initial source code commit to ELC branch for Predator (C7100) ROM
 * monitor.
 *
 * Revision 3.1  1995/11/23  00:54:59  mbeesley
 * Add placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * PA management typedef
 */
typedef struct pas_management_t_ {

    uint pa_bay;			/* Port Adaptor bay */

    /*
     * PA ID eeprom clock register
     */
    ulong clk_reg_width;
    ulong clk_mask;
    volatile void *clk_reg;

    /*
     * PA ID eeprom select register
     */
    ulong select_reg_width;
    ulong select_mask;
    volatile void *select_reg;

    /*
     * PA ID eeprom datain register
     */
    ulong datain_reg_width;
    ulong datain_mask;
    volatile void *datain_reg;

    /*
     * PA ID eeprom dataout register
     */
    ulong dataout_reg_width;
    ulong dataout_mask;
    volatile void *dataout_reg;

} pas_management_t;

typedef int pas_eeprom_type;		/* EEPROM device type */
typedef int pas_eeprom_cmd_t;		/* EEPROM command type */

/*
 * Define the two supported ID eeproms
 */
#define PAS_EEPROM_X2444		1
#define PAS_EEPROM_NMC93C46		2

/*
 * Define the command set supported
 */
#define PAS_EEPROM_CMD_READ		1
#define PAS_EEPROM_CMD_WRITE		2
#define PAS_EEPROM_CMD_ERASE		3
#define PAS_EEPROM_CMD_ZERO		4

/*
 * Define timings constants for the EEPROM's
 */
#define PAS_EEPROM_DELAY		10	/* 10 us */
#define PAS_X2444_DELAY			5000	/* 5000 us */
#define PAS_NMC93C46_WRITE_DELAY        10000   /* 10 ms */
/* 
 * Define NMC93C46 command codes      	SB    OP    Addr[5,0]     
 */
#define NMC93C46_CMD_CONTROL   		(0x1 | 0x0)
#define NMC93C46_CMD_WRDS      		(0x1 | 0x0 | 0x00)
#define NMC93C46_CMD_ERASE_ALL 		(0x1 | 0x0 | 0x08)
#define NMC93C46_CMD_WRITE_ALL 		(0x1 | 0x0 | 0x10)
#define NMC93C46_CMD_WREN      		(0x1 | 0x0 | 0x18)
#define NMC93C46_CMD_READ      		(0x1 | 0x2)
#define NMC93C46_CMD_WRITE     		(0x1 | 0x4)
#define NMC93C46_CMD_ERASE     		(0x1 | 0x6)

/* end of file */
