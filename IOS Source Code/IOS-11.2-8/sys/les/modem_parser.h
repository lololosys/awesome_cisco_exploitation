/* $Id: modem_parser.h,v 3.1.64.1 1996/06/16 21:16:35 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/modem_parser.h,v $ 
 *------------------------------------------------------------------
 * modem_parser.h - Modem Management Parser header file
 * 
 * December, 1995 - Viet Nguyen
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: modem_parser.h,v $
 * Revision 3.1.64.1  1996/06/16  21:16:35  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/21  23:39:38  vnguyen
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MODEM_PARSER_H__
#define __MODEM_PARSER_H__

typedef struct modem_counters modem_counters_t;
struct modem_counters {
    int   modem_count;
    ulong stat_ic_complete;
    ulong stat_ic_failure;
    ulong stat_ic_avail;
    ulong stat_oc_complete;
    ulong stat_oc_failure;
    ulong stat_oc_avail;
    ulong stat_busyout;
    ulong stat_dial_failure;
    ulong stat_ring_no_answer;
    ulong stat_total_call_duration;
};
 
extern void line_modem_command(parseinfo *);
extern void modem_mgmt_command(parseinfo *);
extern void show_modem_command(parseinfo *);
extern void clear_modem_command(parseinfo *);
extern void debug_modem_command(parseinfo *);
extern void modem_back2back_test_command(parseinfo *);
extern void modem_at_mode_command(parseinfo *);
extern void clear_modem_at_mode_command(parseinfo *);
extern void mai_command_parser(parseinfo *);

#endif /* ____MODEM_PARSER_H__ */
