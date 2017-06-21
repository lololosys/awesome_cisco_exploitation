/* $Id: if_as5200_e1.h,v 3.1.20.4 1996/08/30 19:53:10 snyder Exp $
 * $Source: /release/112/cvs/Xsys/les/if_as5200_e1.h,v $
 *------------------------------------------------------------------
 * if_as5200_e1.h - AS5200 E1 framer defines 
 *
 * March 1997, Simon Chen
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_as5200_e1.h,v $
 * Revision 3.1.20.4  1996/08/30  19:53:10  snyder
 * CSCdi67027:  as5200 firmware wastes 45,204 bytes DRAM
 *              declare large firmware arrays const
 *              include extern file with declarations
 *              see /nfs/csc/Docs/extern.flame
 * Branch: California_branch
 *
 * Revision 3.1.20.3  1996/08/28  12:57:07  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.20.2  1996/07/10  21:40:42  styang
 * CSCdi55120:  nm: cardTable objects need work
 * Branch: California_branch
 * To provide the t1/e1 firmware version number in the chassis mib.
 *
 * Revision 3.1.20.1  1996/06/16  21:14:46  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/03/13  23:56:50  sichen
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define  E1_FREQ_HZ			2048000	
#define  MIN_IN_SECS		60	
#define  MEGA_UNIT			1000000	

extern const unsigned short e1_firmware[];
extern const int e1_firmware_words;
extern const unsigned short t1_busytone[];
extern const int t1_busytone_words;
extern const unsigned short t1_dialtone[];
extern const int t1_dialtone_words;

/*
 * Prototypes
 */
void e1_framing(uint , uint , short );
void e1_mail_to_neat(ushort , uchar , uchar , uchar , uchar *);
void e1_linecode( uint , uint , short );
void e1_clocksource(dsx1info *, uint , short );
void e1_loopback_clk_switch(dsx1info *, uint, uchar);
void e1_local_up( uint , uint );
void e1_no_loop( uint , uint );
void e1_txAIS( uint , uint , boolean );
void e1_l6_interrupt(uint );
void e1_carrier_update( dsx1_instance_t *, uint);
void e1_l4_interrupt(uint );
void e1_sec_update( dsx1_instance_t *, uint, paktype *);
void ce1_analyze_mcn_port_adaptor(int , int , int );
void e1_homologation_type(uint , uint , enum dsx1_homologation_type );
void e1_wr_neat_mem (uchar , uint , short);
extern void dsx1_mail_from_neat(ushort event, uchar mid, uchar slot, uchar unit);
