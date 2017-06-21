/* $Id: ppp_auth.h,v 3.4.32.4 1996/07/30 20:18:40 dblair Exp $
 * $Source: /release/112/cvs/Xsys/wan/ppp_auth.h,v $
 *------------------------------------------------------------------
 * ppp_auth.h - PPP authentication protocol support
 *
 * 23-March-92 	W. Miskovetz
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ppp_auth.h,v $
 * Revision 3.4.32.4  1996/07/30  20:18:40  dblair
 * CSCdi59050:  multilink broken when dialing Ascend with 2 phone numbers
 * Branch: California_branch
 * Adding support for PPP LCP endpoint discriminator
 *
 * Revision 3.4.32.3  1996/05/30  18:04:00  fox
 * CSCdi57978:  calls stay up after line protocol of b channel go
 * down(under stress)
 * Branch: California_branch
 * Call lcp_close() instead of lcp_down() when max CHAP timeouts reached.
 * Save copy of CHAP Response packet for next Authentication timeout.
 * Resend if no CHAP Success or Failure packet received.
 *
 * Revision 3.4.32.2  1996/04/27  06:37:42  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.4.2.4  1996/04/26  00:00:16  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.4.2.3  1996/01/21  06:08:45  syiu
 * Branch: Dial1_branch
 * More PPP L2F support
 *
 * Revision 3.4.2.2  1996/01/19  02:15:50  syiu
 * Branch: Dial1_branch
 * PPP L2F support
 *
 * Revision 3.4.2.1  1996/01/16  15:45:29  dblair
 * CSCdi42375:  Move Multilink PPP into PPP subsystem
 * Branch: Dial1_branch
 *
 * Revision 3.4.32.1  1996/03/27  10:36:48  irfan
 * CSCdi36933:  Customer desires configurable PAP/CHAP retries
 * Branch: California_branch
 * Add PPP UI to make us accept more than one PAP/CHAP Authentication
 * attempt. Instead of hanging up the physical layer right away.
 *
 * Revision 3.4  1996/01/11  10:52:49  billw
 * CSCdi43115:  PPP CHAP should use larger challenge for radius
 * compatibility
 * Also implement random_fill() which will fill an arbitrary string of
 * bytes with random and unpredictable data bytes
 *
 * Revision 3.3  1995/11/19  04:50:48  fox
 * CSCdi43514:  PAP authentication is broken
 * Clean up logic used to prevent premature LCP Opens and extend the
 * logic to all possible cases.
 *
 * Revision 3.2  1995/11/17  18:04:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:50:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/15  22:58:51  syiu
 * CSCdi35686:  Async callback AAA authorization and accounting phase not
 * working
 *
 * Revision 2.1  1995/06/07  23:19:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __PPP_AUTH_H__
#define __PPP_AUTH_H__


#define CHAP_CHALLENGE	1
#define CHAP_RESPONSE	2
#define CHAP_SUCCESS	3
#define CHAP_FAILURE	4

#define PAP_AUTH_REQ	1
#define PAP_AUTH_ACK	2
#define PAP_AUTH_NAK	3

#define DEFCHAP_TIMEOUT	(10*ONESEC)
#define MAXCHAP_FAILURES 10

/*
 * CHAP challenge size in bytes.  This must be 16 to work with RADIUS.
 */
#define CHAP_CHALLENGE_SIZE 16
#define CHAP_RANDOM_FILL_SIZE (CHAP_CHALLENGE_SIZE - (2 * 4))

/*
 * auth_state data structure
 * Note: PAP  value_size = size of peersecret
 *       CHAP value_size = size of md_stream
 */
struct auth_state_ {
    hwidbtype 		*idb;		/* my idb */
    uchar    		*challenge_value;/* value sent */
    uchar               md_stream[MD5_LEN]; /* value received in response */
    char                peersecret[PASSWORDLEN+1];
    uchar               challenge_size; /* size of challenge value */
    uchar               value_size;     /* value size of response received */
    uchar    	     	chap_id;	/* unique ID sent on CHAP Challenge */
    uchar    	     	pap_id;		/* unique ID sent on PAP Auth Request */
    uchar		peer_pap_id;	/* ID from peer's PAP Auth Request */
    tinybool		we_said_ok;	/* have we sent an ack/success */
    tinybool		they_said_ok;	/* have they sent an ack/success */
    sys_timestamp	retry_timer;	/* timer for retrying */
    uint     		num_failures;	/* count of attempts to challenge */
    int                 bad_auth_attempts; /* count of rem's failed auths */
    uchar               *remote_name;   /* username of remote system */
    /*
     * We save the peer's host name from its first challenge to limit all
     * subsequent challenges to the same host name.  This prevents the peer
     * from getting in under one name and seeking a response for another
     * name.  It does not prevent the peer from renegotiating LCP and going
     * thru authentication again, but this should cause audit records.  We
     * could tighten this up by
     *  a) limiting the peer to the same name until the physical connection
     *     drops but we may have problems recognizing a legitimate drop, the
     *     peer may have had its name reconfigured, etc.
     *  b) limiting the peer to the same name that it used to authenticate
     *     for us, but this may break some desirable configurations.
     * Obviously, this is an area that deserves more consideration.  For now
     * this approach appears to be reasonably safe.
     */
    uchar               *challenge_name; /* peer name from first challenge */
    /*
     * Saved CHAP response packet.  We save the CHAP response packet because
     * we may need to send it at the next timeout and we will won't have the
     * challenge packet around to process at that time.  This packet must be
     * freed up if the remote CHAP auth succeeds or fails or if the interface
     * is closed or PPP is de-encapsulated or... well you get the idea.
     */
    paktype		*chap_resp_pkt;	/* fully formed CHAP response packet */
};

extern void auth_init(hwidbtype *);
extern void auth_cleanup(hwidbtype *);
extern void auth_protrej(hwidbtype *, ushort);
extern void auth_timeout(hwidbtype *);
extern void auth_print(paktype *);
extern void chap_input(paktype *, hwidbtype *);
extern void pap_input(paktype *, hwidbtype *);
extern void auth_start(hwidbtype *);
extern boolean lcp_do_author(userstruct *, authorstruct *, hwidbtype *, fsm *, usernametype *);
extern char * ppp_hostname(hwidbtype *);
extern void flush_chap_resp_pkt(hwidbtype *);
extern void chap_hash(char *, uchar, uchar *, int, uchar *);

/*
 * We need to save the remote's name.  Since we allow the peer to have a
 * different challenge name, we may need two strings.  But since the usual
 * case is either unidirectional authentication or the same name string, why
 * waste storage space.  We will point both pointers at the same string in
 * memory if they are the same string.  This can be very dangerous so we
 * will run all operations thru these functions.
 */

extern uchar *chap_new_peer_name(uchar **, uchar **, uchar *);

static inline uchar *new_remote_name (auth_state *chapstate, uchar *name)
{
    return(chap_new_peer_name(&chapstate->remote_name,
			      &chapstate->challenge_name,
			      name));
}

static inline uchar *new_challenge_name (auth_state *chapstate, uchar *name)
{
    return(chap_new_peer_name(&chapstate->challenge_name,
			      &chapstate->remote_name,
			      name));
}

#endif
