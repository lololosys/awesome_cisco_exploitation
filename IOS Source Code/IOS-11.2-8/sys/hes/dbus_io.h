/* $Id: dbus_io.h,v 3.3.20.3 1996/09/04 18:54:21 kmitchel Exp $
 * $Source: /release/112/cvs/Xsys/hes/dbus_io.h,v $
 *------------------------------------------------------------------
 * dbus_io.h:
 *
 * March 1994, Stig Thormodsrud
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: dbus_io.h,v $
 * Revision 3.3.20.3  1996/09/04  18:54:21  kmitchel
 * CSCdi67547:  Reflect DBUS timeout recovery participation
 * Branch: California_branch
 *
 * Revision 3.3.20.2  1996/07/10  02:37:43  getchell
 * Added crash context mechanism which is shown via diag command and test
 * cbus (vc command)
 * CSCdi60415:  VIP difficult to debug in the field
 * Branch: California_branch
 *
 * Revision 3.3.20.1  1996/06/03  17:37:33  getchell
 * modify if-con to use new process scheduling primitives
 * CSCdi57383:  dbus polling excessive when if-con open
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/29  16:28:35  banderse
 * CSCdi47646:  Add alternative quit string to CIP/VIP console
 *
 * Revision 3.2  1995/11/17  09:15:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:59:47  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* 
 * The value of these #defines is equal to the bit position in the 
 * dbus i/o status register.
 */
#define DBUS_IO_CONSOLE         0x01
#define DBUS_IO_DEBUG           0x02
#define DBUS_IO_MSG             0x04
#define DBUS_IO_CONSOLE_ACTIVE  0x08   /* Console connection active */
#define DBUS_IO_DEBUG_ACTIVE    0x10   /* Debug   connection active */
#define DBUS_IO_CRASH           0x20  
#define DBUS_IO_TO_RECOVERY     0x40   /* recover from dBus timeouts */

#define DBUS_IO_STATUS_SET    	0x08

#define DBUS_IO_NO_TTY          -1

#define DBUS_IO_QUIT_CHAR	0x03	/* ^C */
#define DBUS_IO_QUIT_THRESHOLD	3	/* three ^C's in a row */

#define DBUS_IO_EMPTY          	0x0
#define DBUS_IO_BUGINF         	0xfe

/*
 * dcbtype: data structure to manage a virtual console to an IP
 * across the DBUS. Supported by CIP, FEIP, Checkers, VIPER
 */
typedef struct dcbtype_ {
   dgtype          *dbus;
   tt_soc          *tty;
   uchar           reg;
   uchar           card_type;
   uchar           quit_count;
   uchar           alt_quit_count;
   watched_boolean *daemon_done;
} dcbtype;
