/* $Id: arap_fcrglue.c,v 3.2.60.1 1996/03/18 18:50:14 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/arap/arap_fcrglue.c,v $
 *------------------------------------------------------------------
 * Glue logic for V.42bis
 *
 * October 1993, Tim Kolar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: arap_fcrglue.c,v $
 * Revision 3.2.60.1  1996/03/18  18:50:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:27:34  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:31:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:38:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/18  19:54:32  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:04:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "../atalk/macip_util.h"
#include "arap_public.h"

/* #define DEBUG_ENABLED */
#include "arap_mnpllvl.h"
#include "arap_mnpdat.h"

v42bis_connection_t *fcr_init_v42bis(void);
void my_push(void *, u_char *, int);
paktype *arap_getbuffer(int, int);


v42bis_connection_t *fcr_init_v42bis (void)
{
    v42bis_connection_t *conn;
    u_char *encodebuf, *decodebuf;

    encodebuf = malloc(1024);
    if (encodebuf == NULL) 
	return(NULL);
    decodebuf = malloc(1024 * 8 + 128);
    if (decodebuf == NULL) {
	free(encodebuf);
	return(NULL);
    }

    conn = malloc(sizeof(v42bis_connection_t));
    if (conn == NULL) {
	free(encodebuf);
	free(decodebuf);
	return(NULL);
    }

    conn->encode = malloc(sizeof(v42bis_t));
    if (conn->encode == NULL) {
	free(encodebuf);
	free(decodebuf);
	free(conn);
	return(NULL);
    }

    conn->decode = malloc(sizeof(v42bis_t));
    if (conn->decode == NULL) {
	free(encodebuf);
	free(decodebuf);
	free(conn->encode);
	free(conn);
	return(NULL);
    }

    conn->outbuffer = malloc(sizeof(mybuffer));
    if (conn->outbuffer == NULL) {
	free(encodebuf);
	free(decodebuf);
	free(conn->encode);
	free(conn->decode);
	free(conn);
	return(NULL);
    }

    v42bis_connection_init(conn);

    v42bis_connection_init_buffers(conn, encodebuf, 1024, decodebuf,
				   1024 * 8 + 128);

    conn->encode->transparent = TRUE;
    conn->decode->transparent = TRUE;
    conn->bufpak = NULL;


    conn->outbuffer->poll = 0;
    /* give the encode side a push routine */
    v42bis_connection_init_push(conn, (void *) conn->outbuffer,
				my_push, my_push);

    stdio->arap_mask &= ~ARAP_ABORTV42BIS;
    return (conn);
}

void fcr_destroy_v42bis(v42bis_connection_t * conn, int ttynum)
{
    if (conn == NULL)
	return;

    if (conn->bufpak)
	arap_datagram_done(conn->bufpak, ttynum);

    free(conn->encode->output_buffer);
    free(conn->decode->output_buffer);
    free(conn->encode);
    free(conn->decode);
    free(conn->outbuffer);
    free(conn);
}

int fcr_do_v42(v42bis_connection_t * c, u_char * buf, int len,
    queuetype * MNP4OutQptr, int ttynum)
{
    int i;
    int total = 0;
    paktype *pak = NULL;
    int count;

    if (stdio->arap_mask & ARAP_ABORTV42BIS)
	return(-1);

    if (ARAP_BUGINF(arapv42bis_debug, arav42tty_debug, ttynum)) {
	buginf("\nV42bis TTY %t: OUT uncomp (%d): ", ttynum,len); 
	for (count = 0; count < len; count++) {
	    buginf("%d ", buf[count]); 
	}
    }

    v42bis_encode_buffer(c->encode, buf, len);
    v42bis_encode_flush(c->encode);
    v42bis_push(c->encode);

    for (i = 0; i < c->outbuffer->buflen; i++) {
	if (pak == NULL) {
	    pak = arap_getbuffer(MEDDATA, ttynum);
	    if (pak == NULL) 
		return(-1);
	    pak->datagramsize = 0;
	}
	pak->datagramstart[pak->datagramsize++] = c->outbuffer->buf[i];

	if (pak->datagramsize > STRM_FDAT_SZ) {
	    total += pak->datagramsize;

	    if (ARAP_BUGINF(arapv42bis_debug, arav42tty_debug, ttynum)) {
		buginf("\nV42bis TTY %t: OUT comp (%d): ", ttynum,
		    pak->datagramsize); 
		for (count = 0; count < pak->datagramsize; count++) {
		    buginf("%d ", pak->datagramstart[count]); 
		}
	    }
	    enqueue(MNP4OutQptr, pak);
	    pak = NULL;
	}
    }

    if (pak) {
	total += pak->datagramsize;
	if (ARAP_BUGINF(arapv42bis_debug, arav42tty_debug, ttynum)) {
	    buginf("\nV42bis TTY %t: OUT comp (%d): ", ttynum,
		pak->datagramsize); 
	    for (count = 0; count < pak->datagramsize; count++) {
		buginf("%d ", pak->datagramstart[count]); 
	    }
	}
	enqueue(MNP4OutQptr, pak);
    }
    c->outbuffer->buflen = 0;
    if (c->encode->error) 
	return(-1);
    else
	return(total);
}



int fcr_undo_v42(v42bis_connection_t * c, u_char * buf, int len,
    queuetype * ARAPInQptr, int ttynum)
{
    int i;
    ushort mtlength = 0;
    int total = 0;
    int count;

    if (stdio->arap_mask & ARAP_ABORTV42BIS)
	return(-1);

    if (ARAP_BUGINF(arapv42bis_debug, arav42tty_debug, ttynum)) {
	buginf("\nV42bis TTY %t: IN comp (%d): ", ttynum,len); 
	for (count = 0; count < len; count++) {
	    buginf("%d ", buf[count]); 
	}
    }

    v42bis_decode_buffer(c->decode, buf, len);
    v42bis_push(c->decode);

    for (i = 0; i < c->outbuffer->buflen; i++) {
	if (c->bufpak == NULL) {
	    c->bufpak = arap_getbuffer(BIGDATA, ttynum);
	    if (c->bufpak == NULL) 
		return(-1);
	    c->bufpak->dataptr = c->bufpak->datagramstart;

	    /*
	     * set datagramsize to arbitrarily high number, it will get reset
	     * when we have enough bytes to set mtlength
	     */
	    c->bufpak->datagramsize = 100;
	}
	if (c->bufpak->dataptr - c->bufpak->datagramstart == 3) {
	    mtlength = c->bufpak->datagramstart[0];
	    mtlength <<= 8;
	    mtlength |= c->bufpak->datagramstart[1];
	    c->bufpak->datagramsize = mtlength + 2;
	    if (c->bufpak->datagramsize > MAX_MTLENGTH)
		return(-1);
	}
	*c->bufpak->dataptr = c->outbuffer->buf[i];
	c->bufpak->dataptr++;

	if (c->bufpak->dataptr == c->bufpak->datagramstart
	    + c->bufpak->datagramsize) {

	    if (ARAP_BUGINF(arapv42bis_debug, arav42tty_debug, ttynum)) {
		buginf("\nV42bis TTY %t: IN uncomp (%d): ", ttynum,
		    c->bufpak->datagramsize); 
		for (count = 0; count < c->bufpak->datagramsize; count++) {
		    buginf("%d ", c->bufpak->datagramstart[count]); 
		}
	    }

	    c->bufpak->dataptr = c->bufpak->datagramstart;
	    c->bufpak->length = c->bufpak->datagramsize;
	    total += c->bufpak->datagramsize;
	    enqueue(ARAPInQptr, c->bufpak);
	    c->bufpak = NULL;
	}
    }
    if (c->decode->error)
	return(-1);
    else
	c->outbuffer->buflen = 0;
    return(total);
}

void my_push(void *context, u_char * buffer, int size)
{
    mybuffer *ptr;

    ptr = (mybuffer *) context;

    bcopy(buffer, &(ptr->buf[ptr->buflen]), size);
    ptr->buflen += size;
}

void mnpreceive(void)
{
}

void mnpsend(void)
{
}

void v42bis_exit_handler(void)
{
   stdio->arap_mask |= ARAP_ABORTV42BIS;
}
