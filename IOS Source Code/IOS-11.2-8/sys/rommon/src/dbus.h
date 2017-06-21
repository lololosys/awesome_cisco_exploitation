/* $Id: dbus.h,v 3.4.18.2 1996/07/08 22:42:36 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src/dbus.h,v $
 *------------------------------------------------------------------
 * dbus.h
 *
 * Oct, 95, Steve J. Zhang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dbus.h,v $
 * Revision 3.4.18.2  1996/07/08  22:42:36  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.4.18.1  1996/03/21  23:26:52  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/19  01:00:58  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/02/08  08:57:52  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:03  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.4  1996/02/09  00:01:48  tkam
 * CSCdi47873:  HSA ROMMON needs to add flow control to dbus download
 * Add download flow control to HSA ROMMON.
 *
 * Revision 3.3  1996/01/17  23:31:12  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.2  1995/11/17  18:41:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:23:43  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  23:11:17  szhang
 * placeholder for DBUS handlers in RSP2
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __DBUS_H__
#define __DBUS_H__

#include "command.h"
#include "queue.h"
#if defined(RSP2)
#include "dclasic.h"
#endif

/* defines for dbus reset register */
#define RESET_MASK             0xe000
#define COLD_RESET             0x8000
#define DBUS_REG_RESET         0x4000
#define DBUS_PIN_RESET         0x2000

#if defined(SVIP)
#define DBUS_REGS              0xB0041000 /* physical addr of dbus */
#endif
#if defined(RSP2)
#define DBUS_REGS              0xBE843004 /* physical addr of dbus */
#endif

#ifdef ASM          /* ASM version of definitions */

/*
 * asm Defines for dbus hardware registers
 */

#define DB_RESET_REG  	0x14   /* HW register #5, 0x05 * 0x04 */
#define DB_STATUS_REG  	0x08   /* HW register #2, 0x02 * 0x04 */
#define INTERNAL_ERROR_PLUS_ONE 0x41  /* When setting internal error
					 from CIP, add one so that the
					 RP will know that software 
					 found the bug. */

#else              /* C version of definitions */

/* DBUS Commands--registers */
#define DB_PHYSICAL_SLOT_NUMBER     0x00  /* READ ONLY */
#define DB_LAST_CMDADDR             0x01  /* READ ONLY */
#define DB_STATUS                   0x02  /* on READ */
#define RESET_ACTION                0x02  /* on WRITE */

#define DB_CONTROL                  0x05  /* software emulated */
#define DB_MICROCODE                0x07  /* software emulated */
#define DB_EEPROM                   0x20  /* 0x20 thruy 0x3f */


#define DB_PORT_STATUS              0x08  
#define DB_CONSOLE                  0x09
#define DB_DEBUG                    0x0a
#define DB_LOGGER                   0x0b

/* end of DBUS Commands-registers */

/* data across the dbus */
#define PORT_DATA                   0xff00
#define PORT_DBUS_READ              0x0080
#if defined(SVIP)
#define PORT_CMD_FAIL               0x0040 /* used by the status reg */
#define PORT_DBUS_ADDRS             0x003f /* last bit is cmd_fail bit */
#endif
#if defined(RSP2)
#define PORT_CMD_FAIL               0x0001 /* used by the status reg */
#define PORT_DBUS_ADDRS             0x007f /* last bit is cmd_fail bit */
#endif



/* IO port for DBUS regsiters:  command, control, and eeprom */
typedef struct data_port_type_ {
    uchar dbus_data;     /* On reads, the high byte is the data */
    union {
	uchar addrs;     /* On reads, the low byte is the address */
	uchar data_out;  /* On writes, the low byte is data */
    } ad;
} data_port_type;


/* defines for command_and_data register */

#define OPCODE   0xf0
#define LOGCOUNT 0x0f

#define PHYSICAL_SLOT_MASK  0x000f
#define LOGICAL_SLOT_MASK   0x000f


/* defines for status register */
#define INTERNAL_ERROR         0x40
#define INTERNAL_ERROR_PLUS_ONE 0x41  /* When setting internal error
					 from CIP, add one so that the
					 RP will know that software 
					 found the bug. */
					 
#define WCS_PARITY_ERROR       0x20
#define WCS_CNTRL_INSTR_ERROR  0x10
#define MICROCODE_STATUS       0x0f
#define DBUS_INVALID_REQUEST   0x02    /* RSP has made an invalid request */
#define DBUS_RSP_READY         0x04    /* CIP is ready for RSP to issue commands */
#define DBUS_PORT_IO_AVAIL     0x08    /* Download flow control is on,
					  bit in status register */
#define DBUS_ERROR_MASK        0xf0


/* Define valid ports for output I/O thru DBUS */
#define CONSOLE_PORT           0
#define DEBUG_PORT             1
#define LOGGER_PORT            2
#define MAX_PORTS              3

/* 
 * The value of these #defines is equal to the bit position in the 
 * dbus port_status register.
 */
#define DBUS_PORT_CONSOLE        0x01
#define DBUS_PORT_DEBUG          0x02
#define DBUS_PORT_LOGGER         0x04 /* corresponds to IO_MSG in
					system code */
#define TERM_CHAR                0x00 /* termination character for
					 console */

#define BAD_DATA                 0xff

#define TICKS_PER_USEC 50 /* 1000 us / ns, 20 ns per Tick, 1000/20 = 50 */

/* defines for control register */
#define CNTL_MASTER_ENABLE     0x80
#define CNTL_LED               0x40
#define CNTL_WCS_CMD_ENABLE    0x04
#define CNTL_WCS_LOADED        0x02
#define CNTL_WCS_LOADING       0x01


/*
 * Struct for dbus hardware registers 
 * 
 * The dbus hardware registers are made up of 7 ushort (16 bits)
 * registers.  However, Addressability is 32 bits apart.  Thus, the
 * high half word of each word is the register.  The low half word is
 * ignored.
 */
#if defined(SVIP)
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

#endif
#if defined(RSP)
typedef struct dbus_hardware_regs_ {
    volatile ushort physical_slot;       /* reg0, dbus physical slot */
    volatile ushort ignore0[3];        
    volatile ushort last_cmd;            /* reg1, last Cmd/Adr register */
    volatile ushort ignore1[3];
    volatile ushort status;              /* reg2, dbus status register */
    volatile ushort ignore2[3]; 
    volatile ushort reg3;                /* reg3 */
    volatile ushort ignore3[3];                   
    volatile data_port_type data_port;   /* reg4, dbus data port */
    volatile ushort ignore4[3];
} dbus_hardware_regs;
#endif


/*
 * Struct for local psuedo dbus registers
 */
typedef struct io_ports_ {
    uchar reserved    :3;     /* First 3 bits are not used */
    uchar debug_act   :1;     /* 1 = RP connected to debug port, GDB */
    uchar console_act :1;     /* 1 = RP connected to console port */
    uchar out_avail   :3;     /* output pending on console, debug or logger */
} io_ports;

    
typedef struct dgtype_ {
    volatile uchar slot_and_comp;
    volatile uchar last_command_or_address;
    volatile uchar status;
    volatile uchar ignore1;
    volatile uchar ignore2;
    volatile uchar ctrl;
    volatile uchar ignore3;
    volatile uchar cmd; 
    volatile io_ports port_status;
    volatile uchar console_port;
    volatile uchar debug_port;
    volatile uchar logger_port;
    volatile uchar dummy2[20];
    volatile uchar eeprom[32];
} dgtype;


/* Pointer Basing Addresses,
 * KSEG0: cached,   unmapped addressing
 * KSEG1: uncached, unmapped addressing
 */

#define PHYSICAL_MASK       0x1fffffff

typedef enum control_state_ {cold, stopped, loading_from_rom, running,
                     cmd_enabled} control_state;

/* structure must be 30 words long */
typedef struct dbus_globals_ {
    uint dbus_handler_p;        /* 4 ptr to current dbus handler */
    wcs_state wstate;           /* 4 enum for current wcs state */
    union {
	volatile dgtype regs;            /* 64 Local copy of RPs Dbus structure */
	volatile uchar r_array[0x40];    /* uchar array of registers */
    } r;

    control_state cstate;       /* 4 enum for current control reg state */
    uint com_reg;               /* 4 current wcs command register */

    /* wcs state information */
    wcs_image_header wh;        /* 20 current image header */
    uint  wcs_data_count;       /* 4 offset to next data byte to be
				   written */
    uchar wcs_parity;           /* 1 parity wcs commands  */
    uchar wcs_head_count;       /* 1 next byte to be written to header */
    uchar wcs_check_count;      /* 1 pointer into checksum half word */
    char cksum_add;             /* 1 state for odd addition in cksum */
    data_port_type data_port;   /* 2 bytes */
    uchar spare1[2];            /* 2 */
    uint wcs_cksum;             /* 4 Current wcs cksum */
    uint start_addr;            /* 4 start address for kernel */
} dbus_globals;


/* sturcture dbus global data */
typedef struct dbuscore_ {
    dbus_globals db;            /* Dbus globals. 30 word long */
    ushort software_version;
    queue_t queue[MAX_PORTS];
} dbuscore_t;


/* 
 * Pointer to dbuscore area
 */
extern dbuscore_t *dbuscore_p;
extern dbuscore_t dbuscore;


/*
 * Returns the dbus address.  addrs_byte(5:0) contains the address.
 */
static inline uchar dbus_addrs (uchar addrs_byte)
{
#if defined(SVIP)
    return((addrs_byte & PORT_DBUS_ADDRS)) ;
#endif
#if defined(RSP2)
    return((addrs_byte & PORT_DBUS_ADDRS) >> 1) ;
#endif
}


/* 
 * Returns opcode of 8 bit WCS command 
 */
static inline uint return_opcode (uint arg)
{
    return( (arg & OPCODE) >> 4);  /* Shift right by one nibble */
}


/* 
 * Returns log of count of WCS command.
 */
static inline uchar return_logcount (char byte)
{
    return(byte & LOGCOUNT);
}


/* 
 * Disable DCL int level 
 */
static inline void disable_dcl_int(uint level)
{
/*    DCL_ASIC->int_mask = DCL_ASIC->int_mask | (uchar)INT_LEV(level); */
}


/* 
 * Enable DCL int level 
 */
static inline void enable_dcl_int(uint level)
{
/*    DCL_ASIC->int_mask = DCL_ASIC->int_mask & (uchar)~INT_LEV(level); */
}


/*
 *  All of the pointers to data, registers, ports, ram addresses . . .
 */
static volatile dbus_hardware_regs * const db_hw = 
       (dbus_hardware_regs *)  DBUS_REGS;  /* DBUS HW registrs. */

/* extern constant declarations */
extern volatile data_port_type * const dpr_ptr;

/* global variable external declarations */

/* function prototypes */ 
void dbus_handler(void);
void dbus_early_init(void);
void ucode_command_data(void);
void command_initialization(void);
void control(data_port_type *);
uint image_transfer(uchar, dbus_globals *);
void port_ready_for_read(uint);
boolean output_pending(void);
void read_port (uint);
void write_port (uint);
uchar port_output(uint);
void dbus_msg_init (void);
/* uint check_dbus_ints(boolean); */
void dbuscore_initialization(void);
void dbus_msg_init(void);
boolean dbus_txempty (uint);
boolean dbus_txfull (uint);
boolean dbus_txhalf_full (uint);
void dbus_txflush (uint);
int console_read_buffer (char *, int , uint);
void decomp_image_mode(void); 
void update_checksum(uchar, dbus_globals *);
uint get_byte_comp(void);
void clear_dram_line(void);
void poll_cause(void);
uint get_count(void);
void console_putc (uchar);
int console_getc (void);
void debug_putc (uchar);
int debug_getc (void);
void dbus_init(void);
#endif
#endif







