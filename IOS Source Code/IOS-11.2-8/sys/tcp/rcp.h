/* $Id: rcp.h,v 3.3.28.2 1996/04/19 15:33:32 che Exp $
 * $Source: /release/112/cvs/Xsys/tcp/rcp.h,v $
 *------------------------------------------------------------------
 * RCP - Berkeley Remote Copy
 *
 * March 1993, Paul Traina/Andy Heffernan
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rcp.h,v $
 * Revision 3.3.28.2  1996/04/19  15:33:32  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.4  1996/04/16  03:56:57  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 2.1.6.3  1996/03/30  03:20:03  carrel
 * cvs fixes
 *
 * Revision 2.1.6.2  1996/03/05  04:44:32  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 2.1.6.1  1996/01/23  06:50:09  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 3.3.28.1  1996/04/15  21:23:35  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/16  03:37:53  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.2  1995/11/17  18:48:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:05:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TCP_RCP_H__
#define __TCP_RCP_H__

#define RCMD_AUTH_NONE       0
#define RCMD_AUTH_PASSWORD   1
#define RCMD_AUTH_TACACS     2

#define RCMD_AUTH_ATTEMPTS   3

#define USERNAMESIZE         16
#define RCMD_SIZE            4
#define FIRST_TOKEN          20

#define SENT_FILE_MODE	0644

#define CHUNK_SIZE	TFTP_MAXDATA	/* Size of data chunk to try
					   and send in one go. */

typedef struct rhost_entry_ {
    struct rhost_entry_   *next;
    char                  *local_user;   /* "account" request is coming from */
    addrtype		  addr;
    char		  *remote_user; /* target "user" on the router */
    int			  priv_level;
} rhost_entry;

typedef struct rsh_disabled_cmd_entry_ {
    struct rsh_disabled_cmd_entry_ *next;
    char			   *command;   /* command that's not allowed */
} rsh_disabled_cmd_entry;

typedef boolean (*rcp_callback)(uchar **, long *, long, void *);

typedef struct kerberos_tcbtype_ {
    tcbtype *tcb;
    boolean use_kerb;
} kerberos_tcbtype;

/*
 * Externs and prototypes
 */

/* Stuff private to rcp/rsh */
extern void rcmd_error(tcbtype *, char *);
extern char *rcmd_get_local_username(void);
extern tcbtype *rcmd(tt_soc *, addrtype *, char *, char *, char *, 
		     tcbtype **, boolean, int *);
extern boolean rcp_enable, rsh_enable;
extern forktype rshd_server(kerberos_tcbtype *tcb);

#endif /* __TCP_RCP_H__ */
