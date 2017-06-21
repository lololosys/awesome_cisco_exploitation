/* $Id: msg_c7100.h,v 3.1.66.4 1996/08/28 13:11:43 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/msg_c7100.h,v $
 *------------------------------------------------------------------
 * msg_c7100.h - Support for C7100 messages
 *
 * November 1995, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_c7100.h,v $
 * Revision 3.1.66.4  1996/08/28  13:11:43  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.66.3  1996/05/31  23:44:32  mbeesley
 * CSCdi59046:  Implement config guidelines
 * Branch: California_branch
 *
 * Revision 3.1.66.2  1996/04/06  02:05:48  ssangiah
 * CSCdi53822:  Need to deal with product name change.
 * Make changes to deal with the change in the product name for Predator
 * from c7100 to c7200.
 * Branch: California_branch
 *
 * Revision 3.1.66.1  1996/03/21  23:48:25  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.3  1996/01/23  02:18:17  mbeesley
 * Branch: ELC_branch
 * Add support for the CPU ID EEPROM. Postpone PCI SRAM initialization
 * until the rev of the CPU card has been read and verified.
 *
 * Revision 3.1.2.2  1995/12/13  02:23:51  mbeesley
 * Branch: ELC_branch
 * Fix misdocumented register bit defintions. Add error interrupt
 * masks. Clear OIR interrupts at boot time. Ensure that PCI Signals
 * Invalid is not set at port adaptor power on.
 *
 * Revision 3.1.2.1  1995/11/22  21:43:38  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/21  01:37:54  mbeesley
 * Add placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file obsoleted as part of the cleanup of message externs.
 */
