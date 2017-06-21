/* $Id: cfg_int_carrier-delay.h,v 3.4 1996/02/08 03:07:03 anke Exp $
 * $Source: /release/111/cvs/Xsys/parser/cfg_int_carrier-delay.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ C A R R I E R - D E L A Y . H
 *
 * Copyright (c) 1994,1996,1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_carrier-delay.h,v $
 * Revision 3.4  1996/02/08  03:07:03  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.3  1996/01/22  06:57:47  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  18:43:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:04:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/************************************************************
 *	carrier-delay [msec] <delay>
 *	no carrier-delay [msec] [<delay>]
 *
 *      OBJ(int, 1) = <seconds> or <millisec>
 *      OBJ(int, 2) = keyword "msec" is sepcified or not
 */
#define MIN_CARRIER_DELAY 0
#define MAX_CARRIER_DELAY 60
#define MIN_CARRIER_DELAY_MS    0
#define MAX_CARRIER_DELAY_MS    ONESEC

EOLS (ci_carrier_delay_eol, carrier_delay_command, 0);
NUMBER (ci_carrier_delay_ms, ci_carrier_delay_eol, no_alt,
        OBJ(int, 1), MIN_CARRIER_DELAY_MS, MAX_CARRIER_DELAY_MS,
        "Carrier Transitions delay milliseconds");
NUMBER (ci_carrier_delay_sec, ci_carrier_delay_eol, no_alt,
        OBJ(int, 1), MIN_CARRIER_DELAY, MAX_CARRIER_DELAY,
        "Carrier Transitions delay seconds");
KEYWORD_ID (ci_carrier_delay_unit, ci_carrier_delay_ms, ci_carrier_delay_sec,
            OBJ(int, 2), TRUE, "msec", "delay specified in milliseconds",
            PRIV_CONF);
NOPREFIX (ci_carrier_delay_noprefix, ci_carrier_delay_unit,
          ci_carrier_delay_eol);
NVGENS (ci_carrier_delay_nvgen, ci_carrier_delay_noprefix,
        carrier_delay_command, 0);
KEYWORD (ci_carrier_delay, ci_carrier_delay_nvgen, ALTERNATE,
         "carrier-delay", "Specify delay for interface transitions",
         PRIV_CONF|PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	ci_carrier_delay
