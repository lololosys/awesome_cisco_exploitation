/* $Id: regfpga.h,v 3.3 1996/01/17 23:31:47 mansonw Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/rommon/src-4k-rsp/regfpga.h,v $
 *------------------------------------------------------------------
 * regfpga.h
 *
 * Mar 1995, Steve J. Zhang
 *
 * Copyright (c) 1995 - 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * REGISTER FPGA register address definition header file. 
 *------------------------------------------------------------------
 * $Log: regfpga.h,v $
 * Revision 3.3  1996/01/17  23:31:47  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.2  1995/11/17  18:46:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:09:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:41:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



#ifndef ASMINCLUDE

typedef struct {
    	/* Physical Base Address is 0x1E840200 */
    	u_char pad0[4];
    	volatile unsigned char sys_ctrl_reg;	/* System Control Register */
    	u_char pad1[7];
    	volatile unsigned char cbus_lock;	/* CBus Lock Register */
	u_char pad2[7];
	volatile unsigned char cbus0;		/* CBus0 Register */	
	u_char pad3[7];
        volatile unsigned char cbus1;		/* CBus1 Register */
	u_char pad4[7];
        volatile unsigned char reset_reason;	/* Reset Reason Register */
	u_char pad5[7];
        volatile unsigned char sw_sys_rst_reg;	/* S/W System Reset Register */
	u_char pad6[7];
	volatile unsigned char sw_perif_rst;	/* S/W Periphral Reset Reg */
	u_char pad7[7];
        volatile unsigned short flash_simm;	/* Flash SIMM Register */
	u_char pad8[6];
        volatile unsigned char led_reg;		/* LED Register */
	u_char pad9[7];
	volatile unsigned char sw_nmi_reg;      /* SW NMI Register */
} REG_FPGA_STRUCT;


#ifdef RSP2
typedef struct {
	/*
	** Physical Base Address is 0x1E843000
	*/
	u_char pad0[4];
	volatile u_short PhysSlot;				/* 0x3004 */
	u_char pad1[6];
	volatile u_char LastCmdAdrVPLD;			/* 0x300c */
	u_char pad2[6];
	volatile u_short StatusReset;			/* 0x3014 (DBus Slave Status)*/
	u_char pad3[6];
	volatile u_short LogicalSlot;			/* 0x301c */
	u_char pad4[6];
	volatile u_short IoDBusDataPort;		/* 0x3024 */
	u_char pad5[6];
	volatile u_short MasterSlaveCmd;		/* 0x302c */
	u_char pad6[6];
	volatile u_short MasterSlaveStatus;		/* 0x3034 */
	u_char pad7[6];
	volatile u_short MasterSlaveTimerEnable;	/* 0x303c */
	u_char pad8[6];
	volatile u_short MasterSlaveTimer;		/* 0x3044 */
	u_char pad9[6];
	volatile u_short DBusFPGAIntStatus;		/* 0x304c */
	u_char pad10[6];
	volatile u_short DBusFPGAIntMask;		/* 0x3054 */
	u_char pad11[6];
} REG_IRSP2DBUS_FPGA_STRUCT;


/*
** Struct to handle Data flow from the Master/Slave
** registers, used during Master/Slave transitions.
** There will be on structure per IRSP slot.
*/
struct MasterSlaveData {
	u_short	Slot;
	u_short	ThisIRSP;
	u_short	OtherIRSP;
	u_short Master;
	u_short Slave;
	u_short StatusChange;
	u_short Exception;
}; 


#define REG_IRSP2DBUS_FPGA ((REG_IRSP2DBUS_FPGA_STRUCT *)(KSEG1_ADRSPC_IRSP2_DBUS_FPGA))
#endif /* RSP2 */
#endif /* ASMINCLUDE */

/*
 * Define a pointer to the Register FPGA 
 */
#define REG_FPGA      ((REG_FPGA_STRUCT *)(KSEG1_ADRSPC_REG_FPGA))

#define FPGA_REG_ADDRESS(a)	(KSEG1_ADRSPC_REG_FPGA + a)

/* 
 * IRSP FPGA register addresses for assembly language code 
 */
#define FPGA_SYSCTRL_REG	FPGA_REG_ADDRESS(0x004)		/* sys_ctrl_reg */
#define FPGA_CBUSLCK_REG	FPGA_REG_ADDRESS(0x00C)		/* cbus_lock */
#define FPGA_CBUS0_REG		FPGA_REG_ADDRESS(0x014)		/* cbus0 */
#define FPGA_CBUS1_REG		FPGA_REG_ADDRESS(0x01C)		/* cbus1 */
#define FPGA_RSTRSN_REG		FPGA_REG_ADDRESS(0x024)		/* reset_reason */
#define FPGA_SYSRST_REG		FPGA_REG_ADDRESS(0x02C)		/* sw_sys_rst_reg */
#define FPGA_PERRST_REG		FPGA_REG_ADDRESS(0x034)		/* sw_perif_rst */
#define FPGA_FLGSIMM_REG	FPGA_REG_ADDRESS(0x03C)		/* flash_simm */
#define FPGA_LED_REG		FPGA_REG_ADDRESS(0x044)		/* led_reg */
#define FPGA_NMI_REG		FPGA_REG_ADDRESS(0x04C)		/* sw_nmi_reg */


/*
 * REGISTER FPGA register address offsets for assembly language code
 */
#define REG_FPGA_SYS_CTRL_OFS		0x004
#define REG_FPGA_CBUS_LOCK_OFS		0x00C
#define REG_FPGA_CBUS0_OFS		0x014
#define REG_FPGA_CBUS1_OFS		0x01C
#define REG_FPGA_RESET_REASON_OFS	0x024
#define REG_FPGA_SW_SYS_RST_OFS		0x02C
#define REG_FPGA_SW_PERIF_RST_OFS	0x034
#define REG_FPGA_FLASH_SIMM_OFS		0x03C
#define REG_FPGA_LED_OFS		0x044

#define REG_FPGA_CBUS_LOCK		(KSEG1_ADRSPC_REG_FPGA + REG_FPGA_CBUS_LOCK_OFS)
#define REG_FPGA_CBUS0			(KSEG1_ADRSPC_REG_FPGA + REG_FPGA_CBUS0_OFS)
#define REG_FPGA_CBUS1			(KSEG1_ADRSPC_REG_FPGA + REG_FPGA_CBUS1_OFS)
#define REG_FPGA_SW_NMI			(KSEG1_ADRSPC_REG_FPGA + REG_FPGA_SW_NMI_OFS)

#define REG_IRSP2DBUS_PHYS_SLOT		(KSEG1_ADRSPC_IRSP2_DBUS_FPGA + 4)
#define REG_IRSP2DBUS_LAST_CMD_ADDR	(REG_IRSP2DBUS_PHYS_SLOT + 8)
#define REG_IRSP2DBUS_STATUS		(REG_IRSP2DBUS_LAST_CMD_ADDR + 8)
#define REG_IRSP2DBUS_LOG_SLOT		(REG_IRSP2DBUS_STATUS + 8)
#define REG_IRSP2DBUS_DATA_PORT		(REG_IRSP2DBUS_LOG_SLOT + 8)
#define REG_IRSP2DBUS_MASTER_SLAVE_CMD	(REG_IRSP2DBUS_DATA_PORT + 8)
#define REG_IRSP2DBUS_MASTER_SLAVE_STATUS (REG_IRSP2DBUS_MASTER_SLAVE_CMD + 8)
#define REG_IRSP2DBUS_MASTER_SLAVE_TE	(REG_IRSP2DBUS_MASTER_SLAVE_STATUS+ 8)
#define REG_IRSP2DBUS_MASTER_SLAVE_TIMER (REG_IRSP2DBUS_MASTER_SLAVE_TE + 8)
#define REG_IRSP2DBUS_FPGA_INT_STATUS	(REG_IRSP2DBUS_MASTER_SLAVE_TIMER + 8)
#define REG_IRSP2DBUS_FPGA_INT_MASK	(REG_IRSP2DBUS_FPGA_INT_STATUS + 8)



/* System Control Register constants */
#define FLASH_PROG_MASK          0x1
#define SYSCTRL_ENA_DBUS_DRIVERS 0x2
#define SYSCTRL_ENA_CBUS_DRIVERS 0x4

#define CYBUS_RECV_PRESENT	 0x8	/* 0 = active */
#define CYBUS_RECV_PRESENT_MASK  0xF7

/* CBUS 0 register defines */
#define CBUS_0_WRITE_ENABLE        0x10
#define CBUS_0_GRANT_ENABLE        0x44
#define CBUS_0_UNLATCHED_STALL     0x2
#define CBUS_0_LATCHED_STALL       0x1

/*
** CBUS 1 register defines
*/
#define CBUS_1_WRITE_ENABLE     0x10
#define CBUS_1_GRANT_ENABLE	0x4
#define CBUS_1_UNLATCHED_STALL	0x2
#define CBUS_1_LATCHED_STALL	0x1


/* Mask for valid bits in registers */
#define SYS_CTRL_MASK		0xFF
#define CBUS0_MASK		0x5
#define CBUS_LOCK_MASK		0x3
#define CBUS1_MASK		0x7
#define RESET_REASON_MASK	0xF
#define FLASH_SIMM_MASK		0x3E0F
#define LED_MASK		0xFF
#define SW_PERIF_RST_MASK	0x1F

#define CBUS_LOCK_MASK		0x3
#define CBUS_GLOBAL_LOCK_REQ	0x01	/* Clear the bit to enable request */
#define CBUS_GLOBAL_LOCK_GRANT	0x02	/* The bit is Cleared when asserted */

/* Reset Reasons */
#define SW_PERIF_RST    0x8
#define SW_NMI	        0x4
#define PUSH_BUTTON_NMI 0x2
#define SW_WATCHDOG_RST 0x1


/* SW_Periheral Reset Reg */
#ifdef RSP2
#define SW_RESET_MASK	0x3F
#define PCMCIA_Rst 0x57
#define IO_FPGA_Rst 0x5B
#define QA_ASIC_Rst 0x5D
#define MemD_ASIC_Rst 0x5E
#else
#define SW_RESET_MASK	0x3F
#define PCMCIA_Rst 0x17
#define IO_FPGA_Rst 0x1B
#define QA_ASIC_Rst 0x1D
#define MemD_ASIC_Rst 0x1E
#endif


/* LED register */
#define LED0		0x1
#define LED1		0x2
#define LED2		0x4
#define LED3		0x8
#define LED4		0x10
#define LED5		0x20
#define LED6		0x40
#define LED7		0x80
#define LEDS_OFF        0xFF
#define LEDS_ON         0

/* SW_Periheral Reset Reg */
#ifdef RSP2
#define DBUS_FPGA_RST	0x3F
#define DBus_Rst	0x6F
#else /* IRSP1 */
#define DBus_Rst	0xF
#endif /* IRSP2 */

/*
** Physical Slot register defines
*/
#define IRSP2_PHYS_SLOT_MASK	0xF0
#define IRSP2_PHYS_SLOT_SHIFT   4
#define IRSP2_PHYS_SLOT_SIX	0x60
#define IRSP2_PHYS_SLOT_SEVEN	0x70
#define IRSP2_PHYS_SLOT_COMP_MASK	0x0F

/*
** DBUS Status Reset Register defines
*/
#define WCS_CTRL_INSTR_ERROR    0x10

/*
** IO DBus Data Port register defines
*/
#define	IRSP2_IODBUS_PORT_DATA_MASK	0xFF00
#define	IRSP2_IODBUS_PORT_READ_MASK	0x0080
#define	IRSP2_IODBUS_PORT_ADDR_MASK	0x007E
#define	IRSP2_IODBUS_PORT_CMD_FAIL	0x0001


/*
** Master/Slave Command register defines
*/
#define MS_CMD_IRSP2_NOP	0x03
#define MS_CMD_IRSP2_MASTER	0x02
#define MS_CMD_IRSP2_SLAVE	0x01
#define MS_CMD_IRSP2_NON_PART	0x00

/*
** Master/Slave Status register defines
*/
#define MS_STATUS_IRSP2_TIMER_ENABLED	0x20
#define MS_STATUS_OTHER_IRSP2_PRESENT	0x10
#define MS_STATUS_OTHER_IRSP2_MASK	0x0C
#define MS_STATUS_OTHER_IRSP2_NOPWR	0x0C
#define MS_STATUS_OTHER_IRSP2_MASTER	0x08
#define MS_STATUS_OTHER_IRSP2_SLAVE	0x04
#define MS_STATUS_OTHER_IRSP2_NON_PART	0x00
#define MS_STATUS_THIS_IRSP2_MASK	0x03
#define MS_STATUS_THIS_IRSP2_NOPWR	0x03
#define MS_STATUS_THIS_IRSP2_MASTER	0x02
#define MS_STATUS_THIS_IRSP2_SLAVE	0x01
#define MS_STATUS_THIS_IRSP2_NON_PART	0x00

#define MS_STATUS_OTHER_IRSP2_PRESENT_SHIFT 4
#define MS_STATUS_OTHER_IRSP2_SHIFT 2


/*
** Master/Slave Timer Enable register defines
*/
#define	MS_IRSP2_TIMER_ENABLE		0x1
#define	MS_IRSP2_TIMER_ENABLE_MASK	0x1

/*
** Master/Slave Timer register defines
*/
#define	MS_IRSP2_TIMER_MAX_COUNT	0xFF

/*
/*
** DBus FPGA Interrupt Status register defines
*/
#define INT_STATUS_IRSP2DBUS_FPGA_MCASIC		0x10
#define INT_STATUS_IRSP2DBUS_FPGA_MS_CHANGE		0x08
#define INT_STATUS_IRSP2DBUS_FPGA_MS_TIMER_TO		0x04
#define INT_STATUS_IRSP2DBUS_FPGA_DBUS_CMDFAIL		0x02
#define INT_STATUS_IRSP2DBUS_FPGA_DBUS_SLAVE_REQ	0x01


/*
** DBus FPGA Interrupt Mask register defines
*/
#define IRSP2DBUS_FPGA_INT_MCASIC_MASK		0x10
#define IRSP2DBUS_FPGA_INT_MS_CHANGE_MASK	0x08
#define IRSP2DBUS_FPGA_INT_MS_TIMER_TO_MASK	0x04
#define IRSP2DBUS_FPGA_INT_CMD_FAIL_MASK        0x02
#define IRSP2DBUS_FPGA_INT_SLAVE_REQ_MASK       0x01
#define IRSP2DBUS_FPGA_INT_MASK                 0x3f
#define IRSP2DBUS_FPGA_INT_UNMASK_MSCHANGE_AND_TIMER		0x13

/*
** Error defines for logging information.
*/
#define	NO_IRSP2_FOUND			0xf0
#define IRSP2_MASTER_NOT_SET_SLOT6	0x10
#define IRSP2_MASTER_NOT_SET_SLOT7	0x20
#define IRSP2_SLAVE_NOT_SET		0x30
#define OTHER_IRSP2_PRESENT_SLAVE_NOT_SET		0x4000
#define IRSP2_NOP_CMD_FAIL		0x5000
#define IRSP2_SLAVE_CMD_FAIL		0x6000
#define IRSP2_NON_PART_CMD_FAIL		0x7000
#define THIS_IRSP2_SLAVE_NOT_SET	0x8000
#define THIS_IRSP2_MASTER_NOT_SET	0x9000
#define THIS_IRSP2_NON_PART_NOT_SET	0xa000
#define THIS_IRSP2_NO_CHANGE_MASTER_TO_SLAVE	0xb000
#define THIS_IRSP2_NO_CHANGE_SLAVE_TO_NONPART	0xc000
#define THIS_IRSP2_NO_CHANGE_NONPART_TO_MASTER	0xd000
#define THIS_IRSP2_NO_CHANGE_MASTER_TO_SLAVE_2	0xe000
#define THIS_IRSP2_NO_CHANGE_SLAVE_TO_MASTER	0x0100
#define THIS_IRSP2_SLAVE_TO_MASTER_NOT_SET	0x0200
#define THIS_IRSP2_SLAVE_NOT_SET_2	0x0300
#define IRSP2_TIMER_NOT_ENABLED		0x0400
#define IRSP2_TIMER_TIMED_OUT		0x0500
#define IRSP2_SLAVE_STATE_CHANGED	0x0600
#define OTHER_IRSP2_NO_CHANGE_MASTER_TO_NON_PART	0x0700
#define IRSP2_SLOT6_NO_STATUS_CHANGE	0x0800
#define IRSP2_SLOT7_NO_STATUS_CHANGE	0x0900
#define THIS_IRSP2_NO_CHANGE_MASTER_TO_NON_PART	0x0a00
#define OTHER_IRSP2_NOT_PRESENT		0x0b00

/* end of module */
    
