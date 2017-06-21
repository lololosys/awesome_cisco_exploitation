/* $Id: slot.h,v 3.11.6.7 1996/09/04 18:12:44 kmitchel Exp $
 * $Source: /release/112/cvs/Xsys/hes/slot.h,v $
 *------------------------------------------------------------------
 *  ../hes/slot.h - slot database definitions
 * 
 *  William H. Palmer, Wed Apr 29 17:01:17 PDT 1992
 * 
 *  Copyright (c) 1992-1996 by cisco Systems, Inc.  
 *  All rights reserved. 
 *------------------------------------------------------------------
 * $Log: slot.h,v $
 * Revision 3.11.6.7  1996/09/04  18:12:44  kmitchel
 * CSCdi66418:  IOS overreacts to DBUS io port-whine
 * Branch: California_branch
 *
 * Revision 3.11.6.6  1996/08/20  00:35:05  cheilek
 * CSCdi58374:  More show diag info needed
 * Branch: California_branch
 * Add CCB_CMD_VIP_GET_CONFIG, vip_ll_sw.
 * Add sw_version_str to slot structure.
 *
 * Revision 3.11.6.5  1996/08/07  17:59:51  kmitchel
 * CSCdi49854:  DBUSINTERR after RSP dbus write; ineffective recvry of
 * disabled card
 * Branch: California_branch
 *
 * Revision 3.11.6.4  1996/07/10  02:37:49  getchell
 * Added crash context mechanism which is shown via diag command and test
 * cbus (vc command)
 * CSCdi60415:  VIP difficult to debug in the field
 * Branch: California_branch
 *
 * Revision 3.11.6.3  1996/07/08  22:25:44  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.11.6.2  1996/05/22  18:54:25  dcarroll
 * CSCdi57774:  Indefinite retries if init fails.
 * Branch: California_branch
 * Add support to limit init retries.
 *
 * Revision 3.11.6.1  1996/03/21  22:40:53  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.4  1996/03/18  23:20:06  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.3  1996/02/08  07:22:38  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.2  1996/01/15  10:20:16  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.1  1995/11/30  01:05:17  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.11  1996/03/06  10:20:28  feisal
 * CSCdi47327:  Need means of getting RVIP stack trace
 *
 * Revision 3.10  1996/03/06  01:29:34  jwelder
 * mem_size added to slottype_ for to store Dram Size on IP's such as a VIP
 * CSCdi50800:  Show Diagbus changed to show VIP1 Dram Size
 *
 * Revision 3.9  1996/03/02  23:00:29  dbath
 * CSCdi50584:  mis-specified micro file results in disabled board
 * Fix broken logic whereby even after we'd successfully loaded
 * the system bundle ucode as a backup, we'd fail to realise we'd
 * done it, and so wouldn't start the board.  While the bonnet is
 * up, tidy up some enums, and shuffle some code to enable other
 * modules to manipulate the ucode database in the future.
 *
 * Revision 3.8  1996/02/06  05:28:55  foster
 * CSCdi47230:  HSA slave should come up after crash
 *
 * Revision 3.7  1996/01/15  18:04:16  schaefer
 * CSCdi45761: support RSP in new chassis
 *
 * Revision 3.6  1995/12/28  08:18:51  foster
 * CSCdi46157:  Dbus down-load needs to work on rsp2 slave down-load
 *
 * Revision 3.5  1995/12/26  23:08:26  dlobete
 * CSCdi42282:  chassis mib incomplete for VIP
 * Add support for PA MIB information to be relayed from VIP to RP/RSP
 * via love-letter.
 * Rework some of the 4T PA's data structures as a result of above.
 *
 * Revision 3.4  1995/12/15  08:01:37  rramacha
 * CSCdi42101:  sh ver does not recognize CX-SMIP or CX-SSIP cards
 *
 * Revision 3.3  1995/12/10  23:27:35  rpratt
 * CSCdi45343:  Chassis MIB enhancements for VIP & port adapters
 *
 * Revision 3.2  1995/11/17  09:19:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:41:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:01:12  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/10/11  07:08:06  mcieslak
 * Reload the microcode for a slot that has failed and allow
 * for IPs that are disabled to be removed cleanly.
 * CSCdi41907:  complex restart / EOIR of disabled cards broken
 *
 * Revision 2.3  1995/08/21  07:40:04  hampton
 * Transition the hes directory code to use the passive timer macros for
 * all its timers.  It no longer references the system clock directly.
 * [CSCdi39033]
 *
 * Revision 2.2  1995/07/17 08:21:23  schaefer
 * CSCdi36181: more robust microcode management (on RSP)
 *
 * Revision 2.1  1995/06/07  20:43:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#ifndef __SLOT_H__
#define __SLOT_H__
/*
 * REQUIRED INCLUDES:
 *     ../parser/parser_defs_ucode.h
 *   RSP-only
 *     if_fci.h
 *     ../src-rsp/rsp_qasic.h
 */

/*---------------------------------------------------------------------------*
 * Misc defines for use with the slots
 *---------------------------------------------------------------------------*/
#define CARDNAMELEN 20

#define SPINNAKER_SP_SLOT     3

#define UNKNOWN_SERIAL_NUMBER 0xffffffff
#define UNKNOWN_CARD_TYPE 0xffff
/* A timestamp of zero indicates the insert time is unknown */

#define DBUS_IO_MAXLEN 110
#define DBUS_MAX_ERRS  3

#define VERSION_STR_LEN 100

/*---------------------------------------------------------------------------*
 * Slot structure -- one per slot in the system.
 *---------------------------------------------------------------------------*/
/*
 * Slot vectors
 * *-------------------------------*
 *  Dbus register write Vector
 * *-------------------------------*
 *     (*dbus_wr_t)(void *, uchar)
 *         write a value to a dbus register for this slot.
 *         based on the card type in the slot, the cbus may
 *         be disabled for the duration of the dbus write
 *         (RSP platform)
 */

/* Prototypes for slot vectors */

typedef void (*dbus_wr_t)(void *, uchar);
typedef uchar (*dbus_io_rd_t)(uint, volatile uchar *);
typedef void (*dbus_io_wr_t)(uint, volatile uchar *, uchar);

#define DBUS_IO_TTY_CONSOLE 0
#define DBUS_IO_TTY_DEBUG   1
#define DBUS_IO_TTY_DEVICES 2

#define SLOT_NUM_VIP_PAS    2

struct slottype_ {
    ushort slot_num;                 /* slot number */
    tinybool ucode_loaded;           /* whether or not ucode has been loaded */
    tinybool io_regs;                /* does this card support new dbus i/o registers */
    tinybool swdbus;                 /* does this card have a software dbus */
    dbus_wr_t dbus_write;            /* vector for dbus writes */
    dbus_io_rd_t dbus_io_reg_read;   /* vector for dbus io read  */
    dbus_io_wr_t dbus_io_reg_write;  /* vector for dbus io write */
    uint dbus_rd_retry_count;        /* count of read timeouts */
    uint dbus_wr_retry_count;        /* count of write timeouts */
    uint dbus_fail_recovery_count;   /* count of timeout recovery failures */
    ulong serial_number;             /* card serial number */
    ushort sw_version;               /* sw version of card */
    ushort hw_version;               /* hw version of card  */
    ushort flags;                    /* magic flags -- see below #defines */
    sys_timestamp insert_time;       /* card discovery time */
    ctrlr_type_t ctrlr_type;         /* FCI_xxx_CONTROLLER for this card */
    ctrlr_type_t ctrlr_specific;     /* FCI_xxx_CONTROLLER specific to this card */
    char card_name[CARDNAMELEN];     /* text name of the card */
    ucode_src_t ucode_source;        /* where was ucode loaded from */
    char flash_file[FLASH_NAME_LEN]; /* name of flash ucode file */
    char msg_name[CARDNAMELEN];      /* facility name of the message */
    boolean msg_new;                 /* is this a new message */
    int msg_severity;                /* message severity level */
    int msg_len;                     /* current message length */
    uchar msg[DBUS_IO_MAXLEN];       /* message */
    ulong msg_timestamp;             /* message timestamp in unix format */
    uchar consecutive_errs;          /* count of consecutive dbus errors */
    int dbus_io_tty[DBUS_IO_TTY_DEVICES]; /* tty # of connected device */
    void *pa_info[SLOT_NUM_VIP_PAS]; /* pa-specific info for VIP */
    ulong mem_size;                  /* Dram size for IP's */
    ulong io_size;                   /* SRAM size for IP's */
    uchar sw_version_str[VERSION_STR_LEN]; /* (IOS) version string from IPs, 
					      e.g. VIP */
    uchar *crash_cntxt;              /* crash context area */
    ulong crash_count;               /* Number of crashes on this slot */
    ulong crash_timestamp;           /* message timestamp in unix format */

#if defined(PREDATOR)
    pa_hardware_type_t pa_type;      /* port adapter type */
    ushort interface_count;          /* number of possibly valid
					entries in interface_list */
    hwidbtype *interface_list[MAX_CTRLR_INTERFACES];
                                     /* an array of hwidb pointers for
					the interfaces on this card */
    boolean retry_ok;                /* if the init fails, retry */
    int retry_count;                 /* number of retries attempted */
#endif

#if defined(RSP)
    ushort interface_count;          /* number of possibly valid
					entries in interface_list */
    hwidbtype *interface_list[MAX_CTRLR_INTERFACES];
                                     /* an array of hwidb pointers for
					the interfaces on this card */

    qa_hwq_t *cmd_q_ptr;             /* the primary address of command queue
					for this card */
    ccbtype *ccb_ptr;                /* pointer to the CCB for this card */
    qa_hwq_t *event_q_ptr;           /* the primary address of the event queue
					for this card */
    qa_hwq_t *love_note_free_q_ptr;  /* the primary address of the queue
					of free love note buffer headers */
    qa_hwq_t *love_letter_free_q_ptr;/* the primary address of the queue
				        of free love note buffers */
    sys_timestamp crash_time;	     /* debounce timer for IP crash */
#endif
};

extern slottype slots[MAX_SLOTS];

/*
 * Macros to traverse all non-empty slots
 */

#define FOR_ALL_SLOTS(sp) \
    for ((sp) = &slots[0]; (sp) < &slots[sizeof(slots)/sizeof(*sp)]; ++(sp)) \


#define FOR_ALL_NONEMPTY_SLOTS(sp) \
    FOR_ALL_SLOTS(sp) \
	if (((sp)->ctrlr_type == FCI_UNKNOWN_CONTROLLER) || \
	    ((sp)->flags & (BOARD_NONEXIST | BOARD_INVALID))) \
	    continue; \
	else

#define FOR_ALL_ENABLED_SLOTS(sp) \
	FOR_ALL_NONEMPTY_SLOTS(sp) \
	    if ((sp)->flags & BOARD_DISABLED) \
		continue; \
	    else

#define FOR_ALL_ENABLED_IP_SLOTS(sp) \
	FOR_ALL_NONEMPTY_SLOTS(sp) \
            if( ((sp)->ctrlr_type == FCI_RSP2_CONTROLLER) ||  \
	       ((sp)->flags & BOARD_DISABLED))   \
                     continue; \
	    else
/*---------------------------------------------------------------------------*
 * Defines for use in the flags field in the slottype_ structure
 *---------------------------------------------------------------------------*/
#define BOARD_DISABLED		0x0001
#define BOARD_NONEXIST		0x0002
#define BOARD_ANALYZED		0x0004	/* successfully analyzed */
#define BOARD_CCBNEEDED		0x0008	/* stopped, needs reset & CCB */
#define BOARD_WEDGED		0x0010	/* command timed out, needs restart */
#define BOARD_UNSUITABLE        0x0020  /* bad rev, don't allow on bus */
#define BOARD_INVALID		0x0040	/* invalid EEPROM contents */
#define BOARD_IDBSREMOVED	0x0080	/* an error has occurred and the idbs
					 * have been removed (board must have
					 * previously been analyzed)
					 */
#define BOARD_RELOADED		0x0100	/* Board has just been reloaded */
#define BOARD_DEBOUNCING	0x0100  /* OIR debouncing at process level */

#endif /* __SLOT_H__ */
