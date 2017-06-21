/* $Id: if_as5200_t1.h,v 3.1.68.6 1996/09/07 22:37:04 jliu Exp $
 * $Source: /release/112/cvs/Xsys/les/if_as5200_t1.h,v $
 *------------------------------------------------------------------
 * if_as5200_t1.h - AS5200 T1 framer defines.
 *
 * Nov 1995, Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_as5200_t1.h,v $
 * Revision 3.1.68.6  1996/09/07  22:37:04  jliu
 * CSCdi64205:  Unprovisioned T1 plugged into AS5200 causes box to hang
 * Branch: California_branch
 *     o The fix included display warning message to console port and used
 *       SNMP notification to send the message to network management
 *       station
 *
 * Revision 3.1.68.5  1996/08/30  19:53:11  snyder
 * CSCdi67027:  as5200 firmware wastes 45,204 bytes DRAM
 *              declare large firmware arrays const
 *              include extern file with declarations
 *              see /nfs/csc/Docs/extern.flame
 * Branch: California_branch
 *
 * Revision 3.1.68.4  1996/08/28  12:57:16  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.68.3  1996/07/11  22:37:01  jliu
 * CSCdi62357:  need to support AT&T and ANSI facility data link standard
 * Branch: California_branch
 *           o Added new fdl command for t1 controller in order to support
 *             CSU function
 *
 * Revision 3.1.68.2  1996/07/10  21:40:44  styang
 * CSCdi55120:  nm: cardTable objects need work
 * Branch: California_branch
 * To provide the t1/e1 firmware version number in the chassis mib.
 *
 * Revision 3.1.68.1  1996/06/16  21:14:58  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/09  23:55:57  jliu
 * Create placeholder for new files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Offset of the external registers on T1 card
 */

#define  NEATMEM_SIZE      0x10000 /* 64k words for NEAT memory space */

/*
 * Prototypes
 */
void bt8360_analyze_mcn_port_adaptor(int, int, int);
void t1_txAIS(uint, uint, boolean);
void t1_linecode(uint, uint, short);
void t1_fdl(uint, uint, short);
void t1_local_up(uint, uint);
void t1_no_loop(uint, uint);
void t1_framing(uint, uint, short);
void t1_interrupt(dsx1_instance_t *);
void t1_clocksource(dsx1info *, uint, short);
void t1_loopback_clk_switch(dsx1info *, uint, uchar);
void t1_cablelength(dsx1info *, uint, short);
void t1_sec_update(dsx1_instance_t *, paktype *);
void t1_loopup_code(uint, uint);
void t1_carrier_update(dsx1_instance_t *);
void t1_lp_print(dsx1_instance_t *);
void t1_l4_interrupt(uint);
void t1_l6_interrupt(uint);
void t1_mail_to_neat(ushort event, uchar mid, uchar slot, uchar unit,
                             uchar *phonenum);
void t1_wr_neat_mem (uchar , uint , short);
short t1_rd_neat_mem(uchar , uint);

extern void t1_neat_int( uchar slot);
extern void t1_cas_init(uchar slot);
extern void dsx1_mail_from_neat(ushort , uchar , uchar , uchar);

extern const unsigned short t1_firmware[];
extern const int t1_firmware_words;
extern const unsigned short t1_busytone[];
extern const int t1_busytone_words;
extern const unsigned short t1_dialtone[];
extern const int t1_dialtone_words;

