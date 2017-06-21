/* $Id: nm_tty.c,v 3.3.44.2 1996/05/21 09:46:10 thille Exp $
 * $Source: /release/112/cvs/Xsys/dspu/nm_tty.c,v $
 *------------------------------------------------------------------
 * This module contains all the low level code which hooks
 * tty output onto (major/sub)-vectors for SNA transport across
 * a SSCP-PU (focalpoint) session to the host.
 *
 * In order to support "interactive" commands from the (Netview)
 * host, we implement a getc() function which takes its input from
 * the sub-system command queue. (The calling of getc() implies
 * interactiveness.) getc() should flush any pending output to the
 * host, then wait for an entry to be placed on the sub-system
 * command queue. It will return each character in the run command
 * buffer to its caller, until '\0' is reached, which is then
 * translated into a newline.
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nm_tty.c,v $
 * Revision 3.3.44.2  1996/05/21  09:46:10  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.44.1  1996/05/17  10:48:59  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.32.2  1996/04/26  07:35:22  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.3.32.1  1996/04/03  14:03:44  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3  1995/12/17  18:26:22  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:06:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:56:40  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/06/22  07:57:31  bchan
 * CSCdi35751:  service point runcmd long data handling
 *
 * Revision 2.2  1995/06/13  08:01:26  bchan
 * CSCdi35751:  service point runcmd long data handling
 *
 * Revision 2.1  1995/06/07  20:30:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "../ui/common_strings.h"
#include "../ibm/ibm_types.h"

#include "interface_generic.h"
#include "ttysrv.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "snautil.h"

#include "nm_tty.h"
#include "nm_runcmd.h"

#define MAX_PUTC_BUFFERS_TOUSE 5
#define PUTC_BUFFER_SIZE       1024


static vectortype NMvector;

static void
nm_tty_putc (tt_soc* tty, int charCount);


static void
nm_tty_setup (tt_soc* tty);

static int                              /* return true if there is message */
nm_tty_outputpending (tt_soc* tty);

/*----------------------------------------------------------------------------*/
/* nm_tty_init: */

void
nm_tty_init (tt_soc* tty)

{
   vectortype* vector;

   vector = &NMvector;
   vector_init(vector);

   vector->vsetup = nm_tty_setup;
   vector->vgetc = GetInteractiveChar;
   vector->vputc = nm_tty_putc;
   vector->vinputpending = NmInputPending;
   vector->voutputpending = nm_tty_outputpending;
/*   vector->vstartoutput = Nm_startoutput; */

   tty->ivector = tty->ovector = vector;
   tty->tty_length = 0;                /* no automore */
   tty->outstart = 0;                  /* pointer to current buffer */
   tty->outhead = 0;                   /* current message, NmOutbuffer */
   tty->outtail = 0;                   /* pointer to the control vector */
   tty->outend  = 0;                   /* pointer to the current message */
   queue_init(&tty->txpakq, 0);          /* output queue */

}  /* nm_tty_init. */


static void
nm_tty_setup (tt_soc* tty)
{
   tty->overflow = 0;                  /* there is no overflow */
}


/*=====================================================================*
* nm_tty_outputpending
*======================================================================*
*
* Description:
*    Save the message at the output buffer queue.
*/


static int                              /* return true if there is message */
nm_tty_outputpending (tt_soc* tty)

{
   if (tty->outhead){
        ((NmOutbuffer*)tty->outhead)->length = tty->outend - tty->outstart;
        enqueue(&tty->txpakq, tty->outhead);
        tty->outhead = 0;
     }
   return (tty->txpakq.qhead != 0);
}  /* nm_tty_outputpending. */


/*
 * The implementation of nm_tty_putc() has a problem with breaking the contents
 *    of a 0x31 vector into two parts when the NmOutbuffer is depleted.
 *   
 * The resolution of this problem is to remove the incomplete vector
 *   when the NmOutbuffer is depleted and put the incomplete vector
 *   in the new NmOutbuffer that will be allocated.
 *
 * Logic is as follows:
 *
 *   When end of NmOutbuffer is detected (i.e. NmOutbuffer is depleted),
 *     invoke nm_tty_remove_incomplete_vector() to remove the incomplete 
 *     vector from the current NmOutbuffer and save the vector contents 
 *
 *   Invoke nm_tty_outputpending() to send NmOutbuffer.
 *
 *   Allocate a new NmOutbuffer and invoke nm_tty_insert_incomplete_vector()
 *     to insert the saved incomplete vector into the new NmOutbuffer 
 *
 *   Now, we're ready to continue processing characters for nm_tty_putc()
 *   
 */

/*
 * nm_tty_insert_incomplete_vector()
 *
 *  Insert incomplete 0x31 vector into tty buffer
 *
 *
 */
static void nm_tty_insert_incomplete_vector (tt_soc *tty, uchar *vectorBufP) 
{
    if (!vectorBufP) {
        tty->outtail = 0;
    } else {
        ushort vectorLen = *vectorBufP;
        memcpy(tty->outend, vectorBufP, vectorLen);
        tty->outtail = tty->outend;  /* outtail points to vector length  */
        tty->outend += vectorLen; /* update end-of-vector ptr */
        free(vectorBufP);
    }
}

/*
 * nm_tty_remove_incomplete_vector()
 *
 *  Remove incomplete 0x31 vector from tty buffer
 *    and save contents in allocated buffer
 *
 *  The address of the allocated buffer is returned
 *
 */
static uchar *nm_tty_remove_incomplete_vector (tt_soc *tty) 
{
    uchar *vectorBufP = NULL;
    ushort vectorLen;

    /*
     * Do we have have a tty buffer to remove the incomplete vector from?
     *
     */
    if (!tty->outend) {
        return(NULL);
    }

    /* 
     * Search backwards in the tty buffer to find the last 0x31 vector
     *   (the last vector is the only one that may be incomplete)
     *
     */
    while (*tty->outend != 0x31) {
        tty->outend--;
    }
    tty->outend--;

    /*
     * Save the contents of the incomplete vector into an alloc'd buffer
     *   and clear the vector data from the tty buffer
     *
     */
    vectorLen = *tty->outend;
    if (vectorLen > 0) {
        vectorBufP = malloc(vectorLen);
        if (!vectorBufP) {
            return(NULL);
        }
        memcpy(vectorBufP, tty->outend, vectorLen);
        memset(tty->outend, 0, vectorLen);
    }
    return(vectorBufP);
}

/*
 * nm_tty_malloc_buffer()
 *
 */
static void nm_tty_malloc_buffer (tt_soc *tty, uchar *vectorBufP)
{

    if (tty->outhead != 0) {
        return;
    }

    /* last buffer is used up */
    if (tty->txpakq.count >= MAX_PUTC_BUFFERS_TOUSE) {
          /* use too many buffers, indicating overflow */
          tty->overflow = MessageTooLong;  /* The reply is too big */
          return;
    }

    /* get a buffer */
    if ((tty->outhead = malloc(sizeof(NmOutbuffer))) == 0){
          tty->overflow = MallocFailure;   /* no buffer, system overflow */
          return;
    } 

    tty->outstart = ((NmOutbuffer*)tty->outhead)->buffer;
    if (tty->txpakq.count == MAX_PUTC_BUFFERS_TOUSE - 1) {
         tty->tty_length = MaxAutomoreLine;  /* no more after MaxAutomoreLine */
         tty->tty_exectimeout = 0;           /* no time out for more */
         tty->automore_count = 0;
    }

    /* initialize buffer pointers */
    tty->outend = tty->outstart;
    nm_tty_insert_incomplete_vector(tty, vectorBufP);

} /* End nm_tty_malloc_buffer() */


/*=====================================================================*
* nm_tty_putc
*======================================================================*
*
* Description:
*    Output a character to a buffer. The buffer is defined as NmOutbuffer which
* is queued in tty->txpakq. The buffer is saved at tty->outhead. The message is
* pointed by three pointers, outstart is the start of the message, outend is the
* end of the message and outtail is the location of counter control vector 31.
* The newline is mapped to one control vector 31.
*/


static void
nm_tty_putc (tt_soc* tty, int chr)
{

    uchar *incomplete_vectorBufP;

    if (tty->overflow) {  /* if overflow */
        return;           /* sink to the black hole */
    }

    if (tty->outhead == 0) {
        nm_tty_malloc_buffer(tty, NULL);
    }

    if (chr < ' ' && chr != '\n') {
       return;             /* filter out unwanted char */
    }

    if (chr == '\n') {
        /* map newline to vector 31 */
        tty->outtail = 0;      /* to add vector 31 later */
    } else {
       if (tty->outtail == 0) {           /* Any 0x31 control vector ? */
           tty->outtail = tty->outend;    /* outtail points to control vector count */
           *tty->outend++ = 2;             /* minimum count = 2 */
           *tty->outend++ = 0x31;          /* control vector */
       }
       *tty->outend++ = asciiToEbcdic[chr - ' '];
       
       ++(*tty->outtail);              /* increment control vector count */
       if ((tty->outend - tty->outstart) >  PUTC_BUFFER_SIZE - 4) {
          incomplete_vectorBufP = nm_tty_remove_incomplete_vector(tty);
          nm_tty_outputpending(tty);
          nm_tty_malloc_buffer(tty, incomplete_vectorBufP);
       }
    }

}  /* nm_tty_putc() */


