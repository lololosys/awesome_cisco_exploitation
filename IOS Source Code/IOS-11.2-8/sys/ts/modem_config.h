/* $Id: modem_config.h,v 3.4.20.2 1996/06/16 21:20:15 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/ts/modem_config.h,v $
 *------------------------------------------------------------------
 * modem_config.h  -- header info for auto-configuratoin of modems
 *
 * September 1995, Tim Kolar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: modem_config.h,v $
 * Revision 3.4.20.2  1996/06/16  21:20:15  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.4.20.1  1996/03/29  18:11:01  sjackson
 * CSCdi52586:  Modem busyout cannot be used without autoconfig
 * Busyout can be achieved via the allowed registry call
 * Branch: California_branch
 *
 * Revision 3.4  1996/01/30  00:05:51  sjackson
 * CSCdi47778:  Requirement for modem busyout without modem autoconfig
 * Registry added to allow busyout without autoconfig
 *
 * Revision 3.3  1996/01/12  20:03:28  sjackson
 * CSCdi44704:  modem busyout command is needed
 * Command added as an extension of the line modem autoconfigure
 *
 * Revision 3.2  1995/11/17  18:55:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:33:18  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/27  04:18:54  tkolar
 * Files for the modemcap and modem_discovery subsystems.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*  
 *  Amount of room we leave for return codes from the modem 
 */
#define RETURNBUFFERLEN 30

extern void modem_configure(tt_soc *);
extern void modem_autoconfigure_command(parseinfo *);
extern void modem_busyout_command(parseinfo *);

