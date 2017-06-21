/* $Id: vip_dbus.h,v 3.3.6.3 1996/07/16 17:27:42 getchell Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/vip_dbus.h,v $
 *------------------------------------------------------------------
 * vip_dbus.h : VIP DBus support
 *
 * January 1995, Michael Beesley 
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vip_dbus.h,v $
 * Revision 3.3.6.3  1996/07/16  17:27:42  getchell
 * CSCdi63024:  vip console output blocks processor
 * Branch: California_branch
 *
 * Revision 3.3.6.2  1996/07/10  02:38:14  getchell
 * Added crash context mechanism which is shown via diag command and test
 * cbus (vc command)
 * CSCdi60415:  VIP difficult to debug in the field
 * Branch: California_branch
 *
 * Revision 3.3.6.1  1996/03/21  23:52:24  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/03/18  23:36:37  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.1  1996/02/10  00:22:32  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.3  1996/03/06  10:20:52  feisal
 * CSCdi47327:  Need means of getting RVIP stack trace
 *
 * Revision 3.2  1995/11/17  18:51:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:39  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if defined(RVIP)
# include "../src-rvip/rvip_dbus.h"
#endif /* RVIP */

#if defined(SVIP)
# include "../src-svip/svip_dbus.h"
#endif /* SVIP */

/*
 * Polling ring sizes
 */
#define MAX_DBUS_CHARS		1024
#define MAX_DBUS_CHARS_MASK	(MAX_DBUS_CHARS-1)

/*
 * DBus control structure
 */
typedef struct dbus_cntl_t_
{
    dbus_hardware_regs *dbus_reg;		/* pointer to hw registers */
    tt_soc *tty;				/* pointer to console tty */

    volatile char tx_r[MAX_DBUS_CHARS];		/* character Tx ring */
    volatile char rx_r[MAX_DBUS_CHARS];		/* character Rx ring */

    volatile ulong tx_h, tx_t;			/* Tx ring head, tail */
    volatile ulong rx_h, rx_t;			/* Rx ring head, tail */

    volatile ulong charsin, charsout;		/* Total in/out counts */

    union {
	volatile dgtype regs;			/* copy of RP's registers */
	volatile uchar r_array[0x40];		/* RP's registers as uchars */
    } r;

    volatile ulong lost_count;			/* missed interrupt count */
    volatile ulong flush_count;			/* flush count */
    volatile boolean stop_out;			/* no more output */
    volatile uchar rx_state;			/* receiver flag */

} dbus_cntl_t;

extern dbus_cntl_t dbus_cntl;
extern void *vip_dbus_context;

/*
 * DBus Registers
 */
#define DB_PHYSICAL_SLOT_NUMBER	0x00
#define DB_RELOAD_VPLDS         0x01

#define DB_STATUS               0x02

#define DB_LOGICAL_SLOT_NUMBER  0x04
#define DB_CONTROL              0x05
#define DB_MICROCODE            0x07

#define DB_PORT_STATUS          0x08  
#define DB_CONSOLE              0x09
#define DB_DEBUG                0x0a
#define DB_LOGGER               0x0b
#define DB_CRASH                0x0c

#define DB_EEPROM               0x20  /* 0x20 thruy 0x3f */

/*
 * DBus Reset Register definitions
 */
#define RESET_MASK             	0xe000
#define COLD_RESET             	0x8000
#define DBUS_REG_RESET         	0x4000
#define DBUS_PIN_RESET         	0x2000

/*
 * DBus Command Register definitions
 */
#define PORT_DATA            	0xff00
#define PORT_CMD_FAIL          	0x0040
#define PORT_DBUS_ADDRS        	0x003f
#define PORT_DBUS_READ         	0x0080

/*
 * DBus command_and_data definitions
 */
#define OPCODE   0xf0
#define LOGCOUNT 0x0f

#define INTERNAL_ERROR         	0x40
#define INTERNAL_ERROR_PLUS_ONE 0x41  	/* When setting internal error
					   from VIP, add one so that the
					   RP will know that software 
					   found the bug. */

#define DBUS_VIP_READY          0x04    /* VIP is up and available */

#define DBUS_PORT_IO_AVAIL     	0x08    /* We have port data for RP,
					   bit in status register */

/*
 * DBus physical/logical slot definitions
 */
#define DBUS_PHYSICAL_SLOT_MASK	0x000f	/* mask to get physical slot number */
#define DBUS_LOGICAL_SLOT_MASK	0x000f	/* mask to get logical slot number */

/* 
 * DBus port_status bit masks
 */
#define DBUS_PORT_CONSOLE     	 0x01
#define DBUS_PORT_DEBUG       	 0x02
#define DBUS_PORT_LOGGER       	 0x04
#define DBUS_PORT_CONSOLE_ACTIVE 0x08
#define DBUS_PORT_DEBUG_ACTIVE   0x10
#define DBUS_PORT_ACTIVE_MASK    0x18
#define DBUS_PORT_CRASH      	 0x20
#define DBUS_PORT_MASK		 0x27


#define DBUS_TERM_CHAR        	0x00
#define DBUS_BAD_DATA          	0xff

/*
 * Defines for the control register
 */
#define CNTL_MASTER_ENABLE     0x80
#define CNTL_LED               0x40
#define CNTL_WCS_CMD_ENABLE    0x04
#define CNTL_WCS_LOADED        0x02
#define CNTL_WCS_LOADING       0x01

/* 
 * Defines for eeprom array
 */
#define VIP_HW_REV_MAJ       	0x02
#define VIP_HW_REV_MIN         	0x03
#define VIP_SERIAL             	0x04

/*
 * X2444 Definitions
 */
#define X2444_DELAY		10	/* wastetime for 10us */

/*
 * Determine if there is any output waiting to go to the console port.
 * Note that this also requires that someone be connected to the
 * console port of the CIP.
 */
static inline boolean console_io_pending (void)
{
    return ((dbus_cntl.r.regs.io_status &
             (DBUS_PORT_CONSOLE | DBUS_PORT_CONSOLE_ACTIVE)) ==
            (DBUS_PORT_CONSOLE | DBUS_PORT_CONSOLE_ACTIVE));
}
 
/*
 * Determine if there is any output waiting to go to the debug port.
 * Note that this also requires that someone be connected to the
 * debug port of the CIP.
 */
static inline boolean debug_io_pending (void)
{
    return ((dbus_cntl.r.regs.io_status &
             (DBUS_PORT_DEBUG | DBUS_PORT_DEBUG_ACTIVE)) ==
            (DBUS_PORT_DEBUG | DBUS_PORT_DEBUG_ACTIVE));
}
/*
 * vip_console_write_char:
 * Write one character to the DBUS tx ring
 */
static inline void vip_console_write_char (uchar c)
{
    leveltype level;
    volatile ulong new_head;

    if (!(dbus_cntl.r.regs.io_status & DBUS_PORT_CONSOLE_ACTIVE)) {
        /*
         * No console connected - just dump character
         */
        return;
    }

    GLOBAL_INTERRUPT_BLOCK(level);
    if (dbus_cntl.tx_h == dbus_cntl.tx_t) {
	dbus_cntl.r.regs.io_status |= DBUS_PORT_CONSOLE;
	dbus_cntl.r.regs.diag_status.value |= DBUS_PORT_IO_AVAIL;
	dbus_cntl.dbus_reg->status = dbus_cntl.r.regs.diag_status.value;
    }
    new_head = (dbus_cntl.tx_h + 1) & MAX_DBUS_CHARS_MASK;
    if (new_head != dbus_cntl.tx_t) {
	dbus_cntl.tx_r[dbus_cntl.tx_h] = c;
	dbus_cntl.tx_h = new_head;
    }
    GLOBAL_INTERRUPT_UNBLOCK(level);
}

extern uchar    vip_crash_context[];
/*
 * Prototypes
 */
extern int 	console_getc(void);

extern void	vip_logger_write_str(uchar, uchar *);
extern void     vip_crash_dump(void *);

extern uint 	vip_physical_slot(void);
extern uint	vip_logical_slot(void);
extern uint 	vip_board_revision(void);

extern void 	vip_set_ready(boolean);
extern void 	vip_set_enable_led(boolean);
extern void	vip_set_internal_error(void);

extern void     vip_dbus_interrupt_handler(void);
extern boolean	vip_dbus_txfull(void);
extern boolean	vip_dbus_txempty(void);
extern boolean 	vip_dbus_txhalf_full(void);
extern void	vip_dbus_txflush(void);
extern int	vip_console_read_buffer(char *, int);
extern void	vip_stop_microcode(void);

/* end of file */
