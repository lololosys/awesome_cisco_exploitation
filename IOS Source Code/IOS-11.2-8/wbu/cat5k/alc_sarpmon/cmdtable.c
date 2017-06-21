/* $Id: cmdtable.c,v 1.1.4.1 1996/05/09 15:00:16 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/cmdtable.c,v $
 *------------------------------------------------------------------
 * cmdtable.c -- PMON source.
 *
 * 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cmdtable.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:16  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:13  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:26  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:22  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:37:57  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:53  rlowe
 * Placeholders for California.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <mips.h>
#include "pmon.h"

int setbp(),copy(),dump(),fill(),cont(),go(),help(),dis(),modify();
int registers(),search(),trace(),clrbp(),sdump(),load(),flush();
int dohi(),do_sym(),do_ls(),do_set(),transp();
int stty(),debug(),memtst(),call(),no_cmd();

extern Optdesc load_opts[];
extern Optdesc m_opts[];
extern Optdesc b_opts[];
extern Optdesc debug_opts[];
extern Optdesc g_opts[];
extern Optdesc t_opts[];
extern Optdesc d_opts[];
extern Optdesc more_opts[];
extern Optdesc ls_opts[];
extern Optdesc l_opts[];
extern Optdesc stty_opts[];
extern Optdesc r_opts[];
extern Optdesc sh_opts[];
extern Optdesc mt_opts[];
extern Optdesc flush_opts[];

Cmd CmdTable[] = {
	{"h","[*|cmd..]",0,"on-line help",help,1,99},
	{"hi","[cnt]",0,"display command history",dohi,1,2},

	{"m","[adr [val|-s str]..]",m_opts,"modify memory",modify,1,99},
	{"r","[reg* [val|field val]]",r_opts,"display/set register",
							registers,1,4},

	{"d","[-bhws] adr [cnt]",d_opts,"display memory",dump,2,4},
	{"l","[-bct][adr [cnt]]",l_opts,"list (disassemble) memory",dis,1,5},

	{"copy","from to siz",0,"copy memory",copy,4,4},
	{"fill","from to {val|-s str}..",0,"fill memory",fill,4,99},

	{"search","from to {val|-s str}..",0,"search memory",search,4,99},
	{"tr","[-2][hostport]",0,"transparent mode",transp,1,3},

	{"g","[-st] [adr [bptadr]] [-c args]",g_opts,"start execution (go)",
								go,1,99},
	{"c","[bptadr]",0,"continue execution",cont,1,2},

	{"t","[-vibcmMrR] [cnt]",t_opts,"trace (single step)",trace,1,99},
	{"to","[-vibc] [cnt]",t_opts,"trace (step over)",trace,1,99},

	{"b","[-idkurwms] [adr]..",b_opts,"set break point(s)",setbp,1,99},
	{"db","[numb|*]..",0,"delete break point(s)",clrbp,1,99},

	{"load","[-vbeasti][-baud][offset][-c cmdstr]",load_opts,
				"load memory from hostport",load,1,5},
	{"dump","adr siz",0,"dump memory to hostport",sdump,3,3},

	{"set","[name [val]]",0,"display/set variable",do_set,1,3},
	{"stty","[device] [opts]",stty_opts,"set terminal options",stty,1,99},

	{"sym","name val",0,"define symbol",do_sym,3,5},
	{"ls","[-ln sym*|-va adr]",ls_opts,"list symbols",do_ls,1,99},

	{"flush","[-di]",flush_opts,"flush caches",flush,1,3},
	{"debug","[-svV] [-c args]",debug_opts,"enter dbx mode",debug,1,99},

	{"mt","[-c][[adr] siz]",mt_opts,"memory test",memtst,1,4},
	{"call","adr [val|-s str]..",0,"call function",call,2,99},

	{"sh","",sh_opts,"command shell",no_cmd,1,99},
	{"more","",more_opts,"paginator",no_cmd,1,99},

	{0}};

