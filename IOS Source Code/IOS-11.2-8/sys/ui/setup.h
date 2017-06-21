/* $Id: setup.h,v 3.4.4.2 1996/08/13 13:29:45 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ui/setup.h,v $
 *------------------------------------------------------------------
 * Prototypes for setup.c
 *
 * December 1992, Dave Katz
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: setup.h,v $
 * Revision 3.4.4.2  1996/08/13  13:29:45  widmer
 * CSCdi64261:  setup dialog should purge all input before initial prompt
 * Branch: California_branch
 * Require user input to enter setup instead of defaulting to yes
 *
 * Revision 3.4.4.1  1996/03/18  22:29:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.22.3  1996/03/16  07:46:03  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.22.2  1996/03/14  01:36:29  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.3.22.1  1996/02/20  21:26:44  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/03/09  22:58:15  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.3  1995/11/28  03:59:12  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.2  1995/11/17  17:49:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:41:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:11:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Defines
 */
#define MAXBUF 100
#define MAXINTERFACES  32

/*
 *  It is useful to track new assignments to interfaces as they happen
 *  in setup.  This way we can give defaults of newly assigned interfaces.
 *  This will also allow us to to support VLSM in setup later.
 */
struct setup_ip_history_ {
    idbtype *idb;
    ipaddrtype addr;
    ipaddrtype mask;
    struct setup_ip_history_ *next;
}; 


/*
 * The setup structure
 */
struct setup_struct_ {
    char *cmds;
    idbtype *idb;
    boolean run_ip;
    boolean run_ip_snmp;
    struct setup_ip_history_ *ip_history_head;
    int defnetwork;
    ipaddrtype addrlist[MAXINTERFACES], masklist[MAXINTERFACES];
    httpstate *http_state;
    uint http_request;
    void *args[LINK_MAXLINKTYPE];
};

/*
 * setup.c
 */
extern const char setup_iphelp[];

extern char *setup_string(char *, const char *, char *);
extern void resolve_all_protocols(void);
extern boolean interface_protocol(const char *, boolean);
extern void start_setup(void);
extern boolean system_protocol(setup_struct *, const char *, boolean);
extern boolean setup_running(void);
extern void setup_abort(void);
extern int setup_printf(setup_struct *, const char *, ...);
