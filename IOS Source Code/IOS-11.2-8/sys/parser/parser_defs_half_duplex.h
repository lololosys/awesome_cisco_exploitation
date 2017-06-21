/* $Id: parser_defs_half_duplex.h,v 3.2.20.1 1996/06/17 08:35:26 jhernand Exp $
 * $Source: /release/112/cvs/Xsys/parser/parser_defs_half_duplex.h,v $
 *------------------------------------------------------------------
 * parser_defs_half_duplex.h - Parser definitions for half-duplex,
 *                             full-duplex, and related parameters.
 *
 * February, 1996, Bob Batz
 * Modification of Suresh Sangiah's version in 11.0.
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_half_duplex.h,v $
 * Revision 3.2.20.1  1996/06/17  08:35:26  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.2  1996/02/13  21:25:31  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.1  1996/02/06  17:15:29  rbatz
 * Placeholder for half-duplex changes.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

enum {  CFG_HDX_RESET_PARAMS = 1,
        CFG_HDX_SET_HALF_DUPLEX,
        CFG_HDX_SET_FULL_DUPLEX,
        CFG_HDX_CONTROLLED_CARRIER,
        CFG_HDX_RTS_TIMEOUT,
        CFG_HDX_CTS_DELAY,
        CFG_HDX_RTS_DROP_DELAY,
        CFG_HDX_CTS_DROP_TIMEOUT,
        CFG_HDX_DCD_TXSTART_DELAY,
        CFG_HDX_DCD_DROP_DELAY,
        CFG_HDX_TRANSMIT_DELAY,
        CFG_HDX_TEST_RTS,
        CFG_HDX_TEST_CTS,
        CFG_HDX_TEST_DTR,
        CFG_HDX_TEST_DSR,
        CFG_HDX_TEST_DCD,
        CFG_HDX_TEST_LOOPBACK,
        CFG_HDX_TEST_TRANSMIT,
        CFG_HDX_LOG_ENABLE,
        CFG_HDX_LOG_CLEAR
      };


