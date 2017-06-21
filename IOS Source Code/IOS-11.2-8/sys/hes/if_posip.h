/* $Id: if_posip.h,v 3.1.2.1 1996/07/08 22:25:41 mwu Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_posip.h,v $
 *------------------------------------------------------------------
 * if_posip.h - POSIP interface support code
 *
 * January 1996, Robert Broberg
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_posip.h,v $
 * Revision 3.1.2.1  1996/07/08  22:25:41  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/06/07  05:29:43  rbroberg
 * Add placeholders for new project
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__IF_POSIP_H__
#define __IF_POSIP_H__

typedef struct posip_ll_ {
    ulong ll_type;
    ulong pos_rx_errors;
    ulong pos_crc_errors;
    ulong pos_giants;
    ulong pos_aborts;
    ulong pos_ignores;
    ulong pos_runts;
} posip_ll;

/*
 * Bandwidth defaults for both IGRP and the 'show int' display.
 */
#define OC3_DELAY 10			/* Delay (100 usec) */
#define OC3_VISIBLE_BANDWIDTH 155000	/* Bandwidth (100Mbps) */
 
/*
 * Time for system software to wait before issuing new commands
 * after a disable
 */
#define POSIP_DISABLE_TIME     5     
/*
 * Prototypes
 */

void posi_clock_cmd(parseinfo *);
void posi_loop_local_cmd(parseinfo *);
void posi_loop_ext_cmd(parseinfo *);
void posi_framing_cmd(parseinfo *);

void posip_parse_love_letter(hwidbtype *, uchar *, ulong);
void posip_platform_init(subsystype *);
void posip_parser_init(void);
void posip_show_util(hwidbtype *);
void posip_debug_init(void);
void posip_show_cbus(ushort);
void posip_reset(hwidbtype *);

#endif /* __IF_POSIP_H__ */

