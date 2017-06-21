/* $Id: modem_discovery.h,v 3.3.20.1 1996/03/28 18:38:32 sjackson Exp $
 * $Source: /release/111/cvs/Xsys/ts/modem_discovery.h,v $
 *------------------------------------------------------------------
 * modem_discovery.h  -- header information for modem autorecognition
 *
 * September 1995, Tim Kolar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: modem_discovery.h,v $
 * Revision 3.3.20.1  1996/03/28  18:38:32  sjackson
 * CSCdi50364:  Modem autoconfig database should contain Microcom commands
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/25  02:16:19  sjackson
 * CSCdi46992:  modem auto discover doesnot recognize codex, usr_courier,
 * etc.
 * Added required ID strings
 *
 * Revision 3.2  1995/11/17  18:55:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:33:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/27  04:18:51  tkolar
 * Files for the modemcap and modem_discovery subsystems.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* 
 *  Modem recognition can get arbitrarily complex.  Our naive first pass
 *   is to send a single AT command and go by the results.  This will
 *   probably have to change to a more extended dialog as we start recognizing 
 *    more modems.
 */

struct autodetect {
	char *sendstring, *id, *modem_name;
};

static const struct autodetect autodetected_modems[] = 
{ 	
	{"ati7", "SMO28800", "hayes_optima"},  /* Hayes Smartmodem Optima */
	{"ati3", "T3000SA", "telebit_t3000"},   /* Telebit T3000 */
	{"ati0", "Global", "global_village"},   /* Global Village teleport */
	{"ati4", "Courier", "usr_courier"},	/* USR Courier */
	{"ati4", "Sportster", "usr_sportster"},	/* USR Sportster */
	{"at&i0", "71110045", "codex_3260" },	/* Codex 3260 */
	{"at*i\r", "ID:", "microcom_hdms"},	/* Microcom HDMS */
	{"", "", ""}
};
