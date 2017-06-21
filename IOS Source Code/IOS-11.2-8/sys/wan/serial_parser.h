/* $Id: serial_parser.h,v 3.3.12.1 1996/06/17 08:36:58 jhernand Exp $
 * $Source: /release/112/cvs/Xsys/wan/serial_parser.h,v $
 *------------------------------------------------------------------
 * serial_parser.h -- common serial interface parser routines
 *
 * Sept 1995, Greg Stovall
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: serial_parser.h,v $
 * Revision 3.3.12.1  1996/06/17  08:36:58  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.3  1996/02/13  21:27:04  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.2  1995/11/17  00:50:38  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:51:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/17  23:06:48  gstovall
 * Placeholders for Talisker_branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

void serial_setencap(parseinfo *csb, idbtype *idb, int enctype,
		     boolean output);
void serial_encapsulation(parseinfo *csb);
void serial_half_duplex_command(parseinfo *csb);
void serial_half_duplex_translate_old_cmds(parseinfo *csb);
void physical_layer_command (parseinfo *csb);

#define IGNORE_LOCAL_LOOPBACK 1
