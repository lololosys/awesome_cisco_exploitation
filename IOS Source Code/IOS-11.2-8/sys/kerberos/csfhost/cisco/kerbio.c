/* $Id: kerbio.c,v 3.2.60.2 1996/06/06 19:46:42 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/csfhost/cisco/kerbio.c,v $
 *------------------------------------------------------------------
 * kerbio.c - Routines for mapping CyberSAFE Kerberos->Cisco network I/O
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: kerbio.c,v $
 * Revision 3.2.60.2  1996/06/06  19:46:42  che
 * CSCdi57954:  router crashes attempting rlogin using kerberos
 * Branch: California_branch
 * Should call tcp_putstringblock() instead of tcp_putstring().
 *
 * Revision 3.2.60.1  1996/04/19  15:12:13  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.4  1996/04/16  03:54:29  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 3.1.2.3  1996/04/09  20:51:17  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 3.1.2.2  1996/04/02  08:56:45  carrel
 * cvs fixes
 *
 * Revision 3.1.2.1  1996/03/05  04:46:46  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.2  1995/11/17  17:45:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define CSF_HDRI_NET
#include <csfc5/header.h>
#include "../tcp/tcp.h"

/*
 * csfC5_net_read()
 *	Read data from TCP connection
 */
csf_sts
csfNETread(csfNET_handle *tcbarg, const void *bufarg,
	const csf_size_t len, csf_size_t *rlen)
{
    tcbtype *tcb = (void *)tcbarg;
    int count = 0;
    csf_sts rsts = CSF_STS_OK;
    int ret, segcnt;
    uchar *buf = NULL;

    while ((count < len) && (rsts == CSF_STS_OK)) {
	/*
	 * Try for more data
	 */
	ret = tcp_getstringblock(tcb, &buf, &segcnt, len - count);

	switch (ret) {
	case END_OF_FILE:	/* Connection closing */
	    rsts = CSFC5_STS_EOF;
	    break;

	case NO_DATA:		/* No more data to read */
	    while (!tcp_closing(tcb) && !tcp_unread(tcb)) {
		process_suspend();
	    }
	    break;

	case URGENT_DATA:	/* Data--accumulate in buffer */
	case OK:
	    memcpy((char *)bufarg + count, buf, segcnt);
	    count += segcnt;
	    break;

	default:		/* Unknown state? */
	    rsts = CSFC5_STS_EINVAL;
	    break;
	}
    }

    /*
     * Return read length, status
     */
    if (rlen) {
	*rlen = count;
    }
    return(rsts);
}

/*
 * csfC5_net_write() writes "len" bytes from "buf" to the file
 * descriptor "fd".  It returns the number of bytes written or
 * a write() error.  (The calling interface is identical to
 * write(2).)
 */
csf_sts
csfNETwrite (csfNET_handle *tcb, void *buf, const csf_size_t len,
	csf_size_t *rlen)
{
    int cc = 0;
    int wrlen = len;
    int error = 0;
    char *abuf = buf;
    csf_sts rsts = CSF_STS_OK;

    do {
	cc = tcp_putstringblock(tcb, abuf, wrlen, &error);
	if (tcp_closing(tcb) || (error != OK)) {
	    rsts = CSFC5_STS_EOF;
	    return(rsts);
	}
	(void) tcp_putpush(tcb, TRUE, FALSE);
	abuf += cc;
	wrlen -= cc;
    } while (wrlen > 0);

    if (rlen) {
	*rlen = (len - wrlen);
    }

    return(rsts);
}
