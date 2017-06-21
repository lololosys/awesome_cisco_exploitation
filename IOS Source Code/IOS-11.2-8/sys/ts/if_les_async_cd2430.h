/* $Id: if_les_async_cd2430.h,v 3.1.16.3 1996/08/28 13:18:04 thille Exp $
 * $Source: /release/112/cvs/Xsys/ts/if_les_async_cd2430.h,v $
 *------------------------------------------------------------------
 * if_les_async_cd2430.h - Definitions for the the terminal port
 *              driver for the Cirrus CL-CD2430 quadart (common
 *              to all platforms)
 *
 * May 1996, Samuel Li
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_les_async_cd2430.h,v $
 * Revision 3.1.16.3  1996/08/28  13:18:04  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.16.2  1996/06/20  16:16:16  widmer
 * CSCdi60890:  Bogus includes cause unnecessary compiles
 * Branch: California_branch
 *
 * Revision 3.1.16.1  1996/06/17  08:36:05  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.1  1996/05/13  18:52:14  samuell
 * Placeholder files for porting Modular 2500 Cobra platform from 11.1.
 * Modular 2500 Cobra files were derived from the original Cobra 11.1
 * Release by code separation into platform-independent code and 2500
 * platform-dependent code files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __IF_LES_ASYNC_CD2430_H__
#define __IF_LES_ASYNC_CD2430_H__

/*
 * Private status flags in cd2430_priv.status
 *   we keep a copy of current flow control on channel
 *      to avoid having to set same flow control type
 */
#define SEEINGBREAK      0x00000001  /* A break was received but no */
                                     /*            end of break yet */
#define SFLOW_IN         0x00000002  /* SW flow control in */
#define SFLOW_OUT        0x00000004  /* SW flow control out */
#define HFLOW_IN         0x00000008  /* SW flow control in */
#define HFLOW_OUT        0x00000010  /* SW flow control out */
 
 
/*
 * Maximum number of buffers a cd2430 channel can
 * pull from the system pool
 */
#define CD2430_MAX_SYS_BUFFS 8
#define CD2430_DEF_RXBUFF  3
#define CD2430_DEF_TXBUFF  2
 
/*
 * This is used to identify the packet memory which we have allocated
 * for dedicated CD2430 use.
 */
#define CD2430_POOL (cd2430_buffpool)
 
/*
 * Buffers in this driver are of this size
 */
#define CD2430_PAKSIZE (BIGDATA)
 
/*
 * Divisor of clock/2048 which drives timer register: rtpr/gt1/gt2/ttr
 */
#define CD2430_PRESCALE (10)
 
/*
 * 2430-specific routines shared among more than one source file
 */
extern void tty_cd2430_parser_init(void);
extern void cd2430_show_buffer_info(tt_soc *tty);
extern void cd2430_show_errors(tt_soc *tty);

/*
 * Async interrupt service routines that get called from outside.
 */
extern void cd2430_async_rxint(void *ttyptr);
extern void cd2430_async_txint(void *ttyptr);
extern void cd2430_async_modint(void *ttyptr);
extern void cd2430_async_rxgoodint(void *ttyptr);

#ifdef PAN
#include "../ts/if_c3000_async_cd2430.h"
#endif

extern tt_soc *cd2430_async_tty_init (int unit, cd2430_quadart *csr);
extern void cd2430_async_vector_init (void);
extern void cd2430_registers_init (tt_soc *tty, cd2430_quadart *csr);
extern void async_alloc_rx_buffers (tt_soc *tty, cd2430_quadart *csr, int idx);
extern boolean cd2430_async_show_controller (hwidbtype *idb, parseinfo *csb);
extern void show_contr (void);
extern boolean cd2430_async_allow_pppmode (void);
extern void cd2430_pppmode (tt_soc *tty, int on);	/* switch to PPP mode */
extern void cd2430_pppmap (tt_soc *tty, ulong txmap, ulong rxmap);
extern cd2430_quadart *cd2430_async_show_controller_init (hwidbtype *idb, parseinfo *csb);
extern void print_modemcontrol (tt_soc *tty);	/* Show the current state of modnem control */
extern boolean tty_xon(tt_soc *);
extern boolean cd2430_setup_break_packet(void);
	
extern cd2430_priv *cd2430info_base;   /* Pointer to allocated array of flags */
extern paktype *break_pak;      /* Data encoding to ask for async BREAK */
extern pooltype *cd2430_buffpool; /* cd2430 permanent buffers pool */
extern int cd2430_buffpool_group; /* group # of above buff pool    */
extern char cd2430_buffpool_name[];      /* Name of pool */
extern int cd2430_perm_buffs;     /* Total permanent buffs we will need */
extern boolean can_ppp;         /* Supports HW async HDLC */
extern vectortype cd2430_vector;
extern vectortype cd2430_dummy_vector;

#endif /* __IF_LES_ASYNC_CD2430_H__ */
