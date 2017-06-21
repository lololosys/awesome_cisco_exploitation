/* $Id: if_vtemplate.h,v 3.1.48.10 1996/09/13 23:15:27 perryl Exp $
 * $Source: /release/112/cvs/Xsys/if/if_vtemplate.h,v $
 *------------------------------------------------------------------
 * if_vtemplate.h - Virtual Template interface services
 *
 * January 1996, Perry Leung
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_vtemplate.h,v $
 * Revision 3.1.48.10  1996/09/13  23:15:27  perryl
 * CSCdi69162:  vtemplate cloning ignore the ppp_only flag after the 1st
 * clone
 * Branch: California_branch
 * Add a field to vaccess_info to keep track of the ppp_only filter on
 * a vaccess
 *
 * Revision 3.1.48.9  1996/08/28  00:11:26  perryl
 * CSCdi64970:  vtemplate hide printf is too specific
 * Branch: California_branch
 * For IO redirection, Vtemplate uses its own tty socket rather
 * than using registry calls in printf().
 *
 * Revision 3.1.48.8  1996/08/22  07:19:54  perryl
 * CSCdi66647:  Can not tell what a virtual access interface is used for
 * Branch: California_branch
 * Add an addition parameter to reg_invoke_vtemplate_create_vaccess, to
 * describe the type of the new virtual access interface.
 *
 * Revision 3.1.48.7  1996/08/20  22:09:50  perryl
 * CSCdi66605:  Max # of vtemplate and vaccess allowed are too limited
 * Branch: California_branch
 * Increase max# of vtemplate to 25 and max# of vaccess to 300
 *
 * Revision 3.1.48.6  1996/08/01  18:55:14  perryl
 * CSCdi61809:  Reconfiguring virtual template causes configuration errors
 * Branch: California_branch
 * Buginf error messages from vtemplate (un)cloning, if vtemplate debug
 * is on. In any case, do not print any error message directly to stdio.
 *
 * Revision 3.1.48.5  1996/07/22  20:10:56  perryl
 * CSCdi63633:  Max # of virtual access interfaces 30 are allocated
 * Branch: California_branch
 * Increase max# of virtual access interfaces to 100
 *
 * Revision 3.1.48.4  1996/07/08  15:57:53  syiu
 * CSCdi62107:  Multichassis config conflict with Dialer
 * Branch: California_branch
 *
 * Revision 3.1.48.3  1996/06/24  22:54:22  perryl
 * CSCdi59423:  Reconfiguring virtual template causes configuration errors
 * Branch: California_branch
 * Use "default" instead of "no" to unconfigure commands in
 * add_unconfigured_command().
 *
 * Revision 3.1.48.2  1996/06/22  07:39:02  syiu
 * CSCdi59165:  PPP Virtual-Access Int will not negotiate NCPs on
 * projected links
 * Branch: California_branch
 *
 * Revision 3.1.48.1  1996/04/27  05:49:44  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.1.2.6  1996/04/08  18:06:49  syiu
 * Branch: Dial1_branch
 * Synchronize Vaccess interface state changes. Otherwise route adjusts etc
 * won't work correctly.
 *
 * Revision 3.1.2.5  1996/03/19  22:39:15  perryl
 * Branch: Dial1_branch
 * Eliminate the vtemplate dirtyQ mgr process and the reset cmdlist
 * mgr. Replace them with vtemplate_onesec() which checks the dirtyQ
 * and the reset_cmdlist_flag every sec. If there are works to do
 * it forks off a process to do the works. The process will kill
 * itself when the works is done.
 *
 * Revision 3.1.2.4  1996/03/01  22:24:56  perryl
 * Branch: Dial1_branch
 * Enqueue dirtyQ managing, hide virtual access interface from UI.
 * Fixed cleanup virtual access interface race condition.
 * Fixed max virtual access interface boundary condition.
 * Cleanup debug messages
 *
 * Revision 3.1.2.3  1996/02/23  19:42:46  perryl
 * Branch: Dial1_branch
 * Add VTEMPLATE_MAXVTEMPLATE_UNIT constant
 *
 * Revision 3.1.2.2  1996/02/06  20:21:01  perryl
 * Branch: Dial1_branch
 *
 * Revision 3.1.2.1  1996/01/18  01:17:40  perryl
 * Branch: Dial1_branch
 * Virtual Access interface services
 *
 * Revision 3.1  1996/01/18  01:02:45  perryl
 * Placeholder for Dial1_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _IF_VTEMPLATE_H_
#define _IF_VTEMPLATE_H_

struct vaccess_info_block_ {
    struct vaccess_info_block_ *next;
    hwidbtype *hwidb;
    hwidbtype *vtemplate; 
    int type;
    int prevent_reset;
    tinybool ppp_only;
};

#define VTEMPLATE_LINEBUF_SIZE 81
struct vtemplate_curproc_ {
    pid_t pid;
    int   unit;
    int   retcode;
    char  linebuf[VTEMPLATE_LINEBUF_SIZE];
    int   lindex;
    int   clonesrc;
    boolean prev_caret;
};

#define VTEMPLATE_MAXVACCESS 300
#define VTEMPLATE_MAXVTEMPLATE 26
#define VTEMPLATE_MAXVTEMPLATE_UNIT (VTEMPLATE_MAXVTEMPLATE - 1)

/* hold queue size */
#define VTEMPLATE_HOLDQ 40
/* default pulse interval */
#define VTEMPLATE_DEFAULT_PULSEINTERVAL (5 * ONESEC)

/* maximum size of a virtual template or virtual access  interface name */
#define VTEMPLATE_NAME_SIZE 50

/* Constant for command list operation */
#define VTEMPLATE_CMDLIST_SIZE 1500

#define VTEMPLATE_CONFBUF_SIZE (VTEMPLATE_CMDLIST_SIZE + VTEMPLATE_NAME_SIZE)

/* max size buffer for reading configuration and uncloning */
#define VTEMPLATE_MAXBUF_SIZE 4000

/* max number of concurrent process running Vtemplate cloning */
#define VTEMPLATE_MAXPROC 5

/* Error code */
#define VTEMPLATE_SUCCESS      	0
#define VTEMPLATE_NO_VTEMPLATE	1
#define VTEMPLATE_BAD_VTEMPLATE	2
#define VTEMPLATE_NO_VACCESS	3
#define VTEMPLATE_BAD_VACCESS	4
#define VTEMPLATE_NOMEMORY	5
#define VTEMPLATE_BAD_CLONESRC  6
#define VTEMPLATE_PARSE_ERR     7
#define VTEMPLATE_BAD_PROC      8
#define VTEMPLATE_ALREADY_FREED 9

/* Status code */
#define VTEMPLATE_STATUS_FREED 0x01
#define VTEMPLATE_STATUS_CONFIG 0x02
#define VTEMPLATE_STATUS_CHANGE 0x04
#define VTEMPLATE_CREATING_HEAD 0x08

/* Various cloning configuration sources */
#define VTEMPLATE_CLONE_DIALER 0x01
#define VTEMPLATE_CLONE_VTEMPLATE 0x02  
#define VTEMPLATE_CLONE_AAA 0x04

/* Various vaccess types */
enum VACCESS_TYPE {
VTEMPLATE_TYPE_UNKNOWN,
VTEMPLATE_TYPE_MLP_BUNDLE, 
VTEMPLATE_TYPE_L2F, 
VTEMPLATE_TYPE_PPP_ATM_PVC, 
VTEMPLATE_TYPE_VP, 
VTEMPLATE_TYPE_PT, 
/* this should always be last define */
VTEMPLATE_TYPE_LASTDEFINE 
};

#define IS_VACCESS(hwidb) (hwidb && (hwidb->type == IDBTYPE_VACCESS))

static inline boolean is_vaccess_prevent_reset (idbtype *idb)
{
    
    if (idb && idb->vaccess_info)
	return(idb->vaccess_info->prevent_reset);

    return(FALSE);

}

static inline void set_vaccess_prevent_reset (idbtype *idb)
{

    vaccess_info_block *vaccess_info;

    if (idb && idb->vaccess_info) {
	vaccess_info = idb->vaccess_info;
	if (vaccess_info->prevent_reset != MAXINT) {
	    vaccess_info->prevent_reset++;
	} else {
	    buginf("\nVACCESS %s:Prevent reset is maximum value",
		   idb->namestring);
	}
    }
}

static inline void clear_vaccess_prevent_reset (idbtype *idb)
{

    vaccess_info_block *vaccess_info;

    if (idb && idb->vaccess_info) {
	vaccess_info = idb->vaccess_info;
	if (vaccess_info->prevent_reset > 0) {
	    vaccess_info->prevent_reset--;
	} else {
	    buginf("\nVACCESS %s:Prevent reset is minimum value",
		   idb->namestring);
	}
    }
}

#endif /* _IF_VTEMPLATE_H_ */
