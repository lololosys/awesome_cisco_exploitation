/* $Id: rlogin.h,v 3.2.62.1 1996/04/19 15:34:59 che Exp $
 * $Source: /release/112/cvs/Xsys/ts/rlogin.h,v $
 *------------------------------------------------------------------
 * Prototypes for rlogin.c
 *
 * January 1993, Dave Katz
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: rlogin.h,v $
 * Revision 3.2.62.1  1996/04/19  15:34:59  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.3  1996/03/30  03:33:26  carrel
 * cvs fixes
 *
 * Revision 2.1.6.2  1996/03/05  04:45:12  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 2.1.6.1  1995/11/17  18:39:15  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.2  1995/11/17  18:55:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/01  23:09:52  irfan
 * CSCdi37754:  rlogin should allow various sources of remote username
 * Allow a /user switch for rlogin command. Also let the current username
 * be passed as remote username, if rlogin trusted-remoteuser-source is
 * local. Besides, send the local username or "ciscoTS" if the current
 * username was authenticated via one of trusted methods.
 *
 * Revision 2.1  1995/06/07  23:10:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * Methods for obtaining the Remote Username string to send
 *         to the remote rlogin server
 */
enum RLOGIN_TRUSTEDHOST_METHODS {
    RLOGIN_TRUSTEDHOST_DEFAULT,      /* Compatibility mode. Ask user */
    RLOGIN_TRUSTEDHOST_LOCAL,        /* Use Exec user name */
    RLOGIN_TRUSTEDHOST_TACACS,       /* Use Tacacs */
    RLOGIN_TRUSTEDHOST_RADIUS        /* Use Radius */
};

/*
 * Possible methods of figuring out the Local Username
 * string to send to the remote rlogin server
 */

                                /* Use hostname / ciscoTS string */
#define RLOGIN_TRUSTED_LNAME_DEFAULT 0x0
                                /* Current sign-on name. Any source */
#define RLOGIN_TRUSTED_LNAME_LOCAL   0x1
                                /* Only TACACS authenticated name */
#define RLOGIN_TRUSTED_LNAME_TACACS  0x2
                                /* Only Kerberos authenticated name */
#define RLOGIN_TRUSTED_LNAME_KRB5    0x3
                                /* Only Radius authenticated name */
#define RLOGIN_TRUSTED_LNAME_RADIUS  0x4

extern void rlogin_init(void);
extern void ts_parser_init(void);
extern void rlogin_trust_remoteuser_command(parseinfo *);
extern void rlogin_trusted_localuser_source_command(parseinfo *);
