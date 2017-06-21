/* $Id: dgbus.h,v 3.2 1995/11/17 18:46:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src-4k-rsp/dgbus.h,v $
 *------------------------------------------------------------------
 * dgbus.h
 *
 * Mar 1995, Steve J. Zhang
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dgbus.h,v $
 * Revision 3.2  1995/11/17  18:46:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:40:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __DGBUS_H__
#define __DGBUS_H__

#define DGBUSSTART		KSEG1_ADRSPC_DIAGBUS
#define DGBUSSHIFT		6 
#define DGBUSBYTES		0x40
#define DIAG_CTRLR_LOAD		5
#define ASSUMED_CXBUS_NUMBER	0

/*
 * This is a temporary number from the set evoked via HW_VERSION.  This will
 * be retrieved via the diag bus
 */
#define SP_HARDWARE		0xB	
#define SSP_HARDWARE		0xC

/*
 * Definitions for WCS Controller.
 */
#define WRITE_WCS_SPC_COMMAND_REGISTER	0x1
#define WRITE_WCS_SPC_DATA_REGISTER	0x2

#define SET_CONTROL_REGISTER_WCS_LOADED	0x4
#define RESET_ROM_ADDRESS_REGISTER	0x7

#define INCREMENT_ROM_ADDRESS_REGISTER	0x8
#define READ_ROM			0xb

#define READ_WCS_SPC_COMMAND_REGISTER	0xd
#define READ_WCS_SPC_DATA_REGISTER	0xe
/*
 * Definitions for the SPC command codes within the WCS.
 */
#define SPC_READ_REGISTER					0
#define SPC_WRITE_REGISTER					1
#define SPC_READ_REGISTER_AND_INCREMENT_INITIALIZE_COUNTER	2
#define SPC_WRITE_REGISTER_AND_INCREMENT_INITIALIZE_COUNTER	3
/*
 * Definitions for the SPC register codes within the WCS.
 */
#define SPC_REG_INITIALIZE_COUNTER				0
#define SPC_REG_RAM_OUTPUT					1
#define SPC_REG_SETUP_AND_STATUS				5

#define WCS_CMD_DOWNLOAD_IMAGE 0x25
#define WCS_DIRECTION_RP_TO_IP 0x00
#define WCS_DIRECTION_IP_TO_RP 0x01 
#define WCS_IMAGE_COMPRESSION_NONE 0x0
#define WCS_IMAGE_COMPRESSION_UNIX 0x1 /* unix compress */

#define AIP_WCS_CMD_REG  0x10

#define AIP_WCS_INIT_CNT 0x10 /*load address pointer */
#define AIP_WCS_WR_RAM   0x60       /* writes both processors */

#define AIP_WCS_WR_DAT_1 0x20
#define AIP_WCS_WR_DAT_2 0x21
#define AIP_WCS_WR_DAT_4 0x22

#define AIP_WCS_LOADED   0x40

#define WCS_CMD_REG	  0x10
#define FSIP_WCS_CMD_REG  0x10
#define FSIP_WCS_INIT_CNT 0x10
#define FSIP_WCS_WR_DAT_1 0x20
#define FSIP_WCS_WR_DAT_2 0x21
#define FSIP_WCS_WR_DAT_4 0x22
#define FSIP_WCS_WR_RAM   0x30
#define FSIP_WCS_WR_PAR   0x31
#define FSIP_WCS_LOADED   0x40

#define FSIP_NUM_MODULES  2
#define FSIP_WCS_SIZE     (128 * 1024)


/*
 * Bit definitions for the diag last command/address register.
 */
#define DBUS_READ			0x40

/*
 * Bit definitions for the diag_status register.
 */
#define CXBUS_ERROR			0x80
#define DBUS_INTERNAL_ERROR		0x40
#define WCS_PARITY_ERROR		0x20
#define WCS_CNTRL_INSTR_ERROR		0x10
#define DBUS_SW_READY                   0x04
#define UCODE_STATUS_MASK		0x0f
#define DBUS_INVALID_REQUEST            0x02

/*
 * The longest we will wait for a s/w dbus to 
 * become ready.
 */
#define DBUS_SW_MAXDELAY                (3*ONESEC)   

/*
 * Delay between calls to dbus_check_msg()
 */
#define DBUS_LOGGER_DELAY               4

#define DBUS_ERROR_MASK (CXBUS_ERROR|DBUS_INTERNAL_ERROR|WCS_PARITY_ERROR|WCS_CNTRL_INSTR_ERROR)
#define WCS_ERROR_MASK (WCS_PARITY_ERROR|WCS_CNTRL_INSTR_ERROR)

/* Bit definitions for the CxBus_number_and_logical_slot register */
#define XILINX_READY 			0x80
/*
 * Bit definitions for the diag_control register.
 */
#define MASTER_ENABLE			0x80
#define ENABLE_LED			0x40
#define WCS_DBUS_CMD_ENABLE		0x04
#define WCS_LOADED			0x02
#define WCS_LOADING			0x01


#define DBUS_ROM_LOAD_TIME	50 /* time in ms to wait for board to load
				       after clearing WCS_LOADED */

#define DBUS_XILINX_TIME 	150 /* time in ms to wait for reconfig of
				       VPLD after writing to offset 1 */


#define IDPROM_BASE 		ADRSPC_IDPROM
#define IDPROM_MAGIC 		0x78
#define IDPROM_VERSION 		1

typedef struct id_eeprom_ {
    u_char eeprom_version;
    u_char controller_type;
    u_char hardware_version_high;
    u_char hardware_version_low;
    u_char serial_number_31_24;
    u_char serial_number_23_16;
    u_char serial_number_15_8;
    u_char serial_number_7_0;
    u_char part_number_31_24;
    u_char part_number_23_16;
    u_char part_number_15_8;
    u_char part_number_7_0;
    u_char test_history;
    u_char rma_number_23_16;
    u_char rma_number_15_8;
    u_char rma_number_7_0;
    u_char board_revision;
    u_char deviation_number_15_8;
    u_char deviation_number_7_0;
    u_char board_config;
    u_char dummy[7];
    u_char magic;
    u_char macaddr[3];
    u_char router_sign;
    u_char DPU_rev;
    u_char dummy1[15];
} id_eeprom_t;

typedef struct dgbus_eeprom_ {
    u_char eeprom_version;
    u_char controller_type;
    u_char hardware_version_high;
    u_char hardware_version_low;
    u_char serial_number_31_24;
    u_char serial_number_23_16;
    u_char serial_number_15_8;
    u_char serial_number_7_0;
    u_char part_number_31_24;
    u_char part_number_23_16;
    u_char part_number_15_8;
    u_char part_number_7_0;
    u_char test_history;
    u_char rma_number_23_16;
    u_char rma_number_15_8;
    u_char rma_number_7_0;
    u_char board_revision;
    u_char dummy3[10];
    u_char magic;
    u_char macaddr[3];
    u_char flags;
} dgbus_eeprom_t;


typedef struct rp1_eeprom_ {
    struct dgbus_eeprom_ dgbus_common;
    uchar version;
    uchar magic;
    uchar macaddr[6];
    uchar count;
} rp1_eeprom_t;

/*
 * Struct overlaying the actual Diag registers
 */
struct dgtype_ {
    volatile u_char slot_and_comp;
    volatile u_char last_command_or_address;
    volatile u_char diag_status;
    volatile u_char fip_leds;
/*  volatile u_char xilinx_ready : 1;
    volatile u_char not_used : 1;
    volatile u_char CxBus : 2;
    volatile u_char logical_slot : 4; */ /* structure of CxBus_and_logical_slot */
    volatile u_char CxBus_and_logical_slot;
    volatile u_char diag_control;
    volatile u_char ssp_jumpers;
    volatile u_char ucode_command_and_data;
    volatile u_char io_status;
    volatile u_char io_console;
    volatile u_char io_debug;
    volatile u_char io_msg;
    volatile u_char dummy2[20];
    struct dgbus_eeprom_ eeprom;
};


/*
 * Hardware version definitions for cards known to be deficient at
 * various rev levels for EOIR.
 */

#define FSIP_NO_EOIR_VERSION 0x0100
#define AIP_NO_EOIR_VERSION  0x0100

#endif

