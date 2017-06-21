/* $Id: exec.h,v 3.2.62.1 1996/03/18 22:29:07 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/ui/exec.h,v $
 *------------------------------------------------------------------
 * exec.c  -- Public prototypes
 *
 * september 1992
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec.h,v $
 * Revision 3.2.62.1  1996/03/18  22:29:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/14  01:36:11  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.2.26.1  1996/02/20  21:25:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:47:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:40:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/24  00:54:59  dkatz
 * CSCdi40898:  Synchronized reload needed
 *
 * Revision 2.1  1995/06/07  23:11:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void exec_parser_init(void);
int default_parse_cli_prompt(parseinfo *, char *, int *);
extern void reload_command(parseinfo *);
extern void (*reload_command_ptr)(parseinfo *);
extern void reload_emergency_message(emergency_msg_time_type);

extern void Print_Hardware(void);
extern void show_controllers(parseinfo *csb);
void BuildPrompt(parseinfo *csb, tt_soc *tty);
