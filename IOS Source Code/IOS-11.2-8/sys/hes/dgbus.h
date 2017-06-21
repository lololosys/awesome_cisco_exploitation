/* $Id: dgbus.h,v 3.4.18.5 1996/08/07 17:59:43 kmitchel Exp $
 * $Source: /release/112/cvs/Xsys/hes/dgbus.h,v $
 *------------------------------------------------------------------
 *  ../hes/dgbus.h - "Diagnostic bus" definitions
 * 
 *  William H. Palmer, Thu Apr  9 17:25:10 1992 
 * 
 *  Copyright (c) 1992-1996 by cisco Systems, Inc.  
 *  All rights reserved. 
 *------------------------------------------------------------------
 * $Log: dgbus.h,v $
 * Revision 3.4.18.5  1996/08/07  17:59:43  kmitchel
 * CSCdi49854:  DBUSINTERR after RSP dbus write; ineffective recvry of
 * disabled card
 * Branch: California_branch
 *
 * Revision 3.4.18.4  1996/07/10  02:37:44  getchell
 * Added crash context mechanism which is shown via diag command and test
 * cbus (vc command)
 * CSCdi60415:  VIP difficult to debug in the field
 * Branch: California_branch
 *
 * Revision 3.4.18.3  1996/07/09  21:14:53  getchell
 * Modify read bus exception handler and DCL interrrupt handler to deal
 * with dbus accesses to non-existent card.
 * CSCdi58555:  bus exception caused by dbus access to non-existant card
 * Branch: California_branch
 *
 * Revision 3.4.18.2  1996/07/08  22:25:04  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.4.18.1  1996/03/21  22:39:56  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/18  23:18:15  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/02/10  00:04:09  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1996/02/08  07:20:35  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.4  1996/02/09  10:03:32  foster
 * CSCdi47873:  HSA ROMMON needs to add flow control to dbus download
 * when you pick this change up you have to run with "125" ROMs or later
 * in an HSA system.
 *
 * Revision 3.3  1996/01/18  06:46:35  foster
 * CSCdi47104:  HSA proms need some timing changes in system code
 *
 * Revision 3.2  1995/11/17  09:15:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  20:59:48  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/09/12  18:22:11  kramling
 * CSCdi37548:  FEIP Parity error requires OIR or Power cycle to clear it
 *
 * Revision 2.3  1995/08/14  15:50:34  kramling
 * CSCdi38714:  Use safer NMI reset in place of traditional dbus register
 * reset
 *
 * Revision 2.2  1995/08/09  00:19:24  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.1  1995/06/07  20:39:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */


#ifndef __DGBUS_H__
#define __DGBUS_H__

#define DGBUSSTART		ADRSPC_DIAGBUS
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
 * Values returned when dbus exception or interrupt occurs as a 
 * result of a non-responsive IP
*/
#define DBUS_REG_TIMEOUT                0xFF
#define DBUS_STATUS_TIMEOUT             0

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
 *RSP2 only
 */
#define FLOW_START                      0x08

/*
 * The longest we will wait for a s/w dbus to 
 * become ready.
 *
 * FIXME : This has been changed to 30 seconds for VIPER development.
 * This needs to be double checked for other sw dbus IP's.
 */
#define DBUS_SW_MAXDELAY                (30*ONESEC)	/* 30 seconds */

/*
 * Number of ms between polling for a s/w dbus ready
 */
#define DBUS_SW_POLL_TIME		(64)		/* 64 milliseconds */

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


/*
 * FIXME : This has been changes from 50 ms to 250 ms for 
 * VIPER development. This needs to be double checked for all other
 * IP's in both 7000 and 7500 products.
 */
#define DBUS_ROM_LOAD_TIME	250 /* time in ms to wait for board to load
				       after clearing WCS_LOADED */

#define DBUS_XILINX_TIME 	150 /* time in ms to wait for reconfig of
				       VPLD after writing to offset 1 */


#define IDPROM_BASE 		ADRSPC_IDPROM
#define IDPROM_MAGIC 		0x78
#define IDPROM_VERSION 		1

typedef struct dgbus_eeprom_ {
    volatile u_char eeprom_version;
    volatile u_char controller_type;
    volatile u_char hardware_version_high;
    volatile u_char hardware_version_low;
    volatile u_char serial_number_31_24;
    volatile u_char serial_number_23_16;
    volatile u_char serial_number_15_8;
    volatile u_char serial_number_7_0;
    volatile u_char part_number_31_24;
    volatile u_char part_number_23_16;
    volatile u_char part_number_15_8;
    volatile u_char part_number_7_0;
    volatile u_char test_history;
    volatile u_char rma_number_23_16;
    volatile u_char rma_number_15_8;
    volatile u_char rma_number_7_0;
    volatile u_char board_revision;
    volatile u_char dummy3[10];
    volatile u_char magic;
    volatile u_char macaddr[3];
    volatile u_char flags;
} dgbus_eeprom_t;

typedef struct rp1_eeprom_ {
    struct dgbus_eeprom_ dgbus_common;
    uchar version;
    uchar magic;
    uchar macaddr[6];
    uchar count;
} rp1_eeprom_t;


typedef struct {
    PACKED(volatile u_char value);
} protected_uchar;


/*
 * Struct overlaying the actual Diag registers
 * Any writes to the protected_uchar fields must be done
 * using dbus_prot_wr().
 */
struct dgtype_ {
    volatile u_char slot_and_comp;
    protected_uchar last_command_or_address;
    protected_uchar diag_status;
    volatile u_char fip_leds;
/*  volatile u_char xilinx_ready : 1;
    volatile u_char not_used : 1;
    volatile u_char CxBus : 2;
    volatile u_char logical_slot : 4; */ /* structure of CxBus_and_logical_slot */
    volatile u_char CxBus_and_logical_slot;
    protected_uchar diag_control;
    volatile u_char ssp_jumpers;
    volatile u_char ucode_command_and_data;
    volatile u_char io_status;
    volatile u_char io_console;
    volatile u_char io_debug;
    volatile u_char io_msg;
    volatile u_char io_crash;
    volatile u_char dummy2[19];
    struct dgbus_eeprom_ eeprom;
};

/*
 * Define the character used to reset an IP via a write cycle
 * to the diag_status DBUS register. On the Checkers, this value
 * has a meaning. If it is 0x00, an NMI reset is sent to the 
 * Checkers CPU. If it 0xff, a hard reset is sent.
 */
#define DBUS_SOFT_REG_RESET   	0x00
#define DBUS_HARD_REG_RESET	0xff
#define DBUS_RESET_VALUE	DBUS_SOFT_REG_RESET

/*
 * Hardware version definitions for cards known to be deficient at
 * various rev levels for EOIR.
 */

#define FSIP_NO_EOIR_VERSION 0x0100
#define AIP_NO_EOIR_VERSION  0x0100
#define MIP_NO_EOIR_VERSION  0x0100

/*
 * return value read from a protected dbus register
 */
static inline uchar
dbus_prot_rd (protected_uchar *reg)
{
    return reg->value;
}

/*
 * write a value to a protected dbus register, in between calling
 * platform-specific routines to handle protecting the write.
 * The RSP platform requires the cbus to be disabled when
 * writing to dbus registers which could cause the card to
 * become disabled or reset.
 * This is because a cbus access could be terminated without
 * completing, resulting in a error interrupt from the cbus
 * controller.  As such, the cbus is disabled for the duration
 * of the dbus write to prevent this error from occurring.
 * 
 */
void dbus_prot_wr(uint, protected_uchar *, uchar);

#endif /* !__DGBUS_H__ */

