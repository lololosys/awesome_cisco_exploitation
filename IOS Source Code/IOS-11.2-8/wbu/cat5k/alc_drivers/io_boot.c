/* $Id: io_boot.c,v 1.1.4.1 1996/05/09 14:54:53 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/io_boot.c,v $
 *------------------------------------------------------------------
 * io_boot.c -- Upper-level console I/O interface routines.
 *
 * July 25, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * copyright 1992-1993 XLNT Designs, Inc.  All rights reserved.
 *------------------------------------------------------------------
 * $Log: io_boot.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:53  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:20  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:06  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:24  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:19  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:49  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.7.8.1  1996/01/18  05:02:23  rlowe
 * Manufacturability changes to bootrom and flash boot code.
 *
 * Branch: ATM21_branch
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "acp_basic_types.h"
#include "ctype.h"
#include "stdio.h"
#include "scc_if.h"
#include "mfp_acp_defs.h"
#include "acp_c_defs.h"

/*********************************************************************
	External functions.
*********************************************************************/

extern uInt32 get_cur_tty();
extern int scc_input ();
extern int scc_output();

#define DEBUGPORT 1

/*********************************************************************
	External Data.
*********************************************************************/

/*********************************************************************
	Data global to this module.
*********************************************************************/

#define	DONT_MAP_CRLF	(0)

#define	BS		0x08
#define CR		0x0d
#define NL		0x0a
#define	SP		' '	

static char lf = CR ;

/*
	Default maximum size for one output request.
*/
#define IOBUFSIZE 512

/*
	The next variable keeps track of which stdout_buf entry is 
	currently being used.  This allows nested calls to printf.
*/
static int cur_depth = 0;

/*
	Default maximum number of entries to printf.
*/
#define DEFAULT_DEPTH	4
/*
    Buffer for standard out.
*/
uChar  stdout_buf[DEFAULT_DEPTH][IOBUFSIZE];

struct _iobuf _iob [DEFAULT_DEPTH];


#ifdef ROMSETS
/* these routines are taken from Stevens' "UNIX Network Programming" */
int
readn(s, ptr, nbytes)
struct socket *s;
char *ptr;
int nbytes;
{
    int nleft, nread;

    nleft = nbytes;
    while(nleft > 0) {
	nread = read(s, ptr, nleft);
	if(nread < 0) 
	    return(nread);
	else 
	    if(nread == 0) break;
	nleft -= nread;
	ptr += nread;
    }

    return(nbytes - nleft);
}

int
writen(s, ptr, nbytes)
struct socket *s;
char *ptr;
int nbytes;
{
    int nleft, nwritten;

    nleft = nbytes;
    while(nleft > 0) {
	nwritten = write(s, ptr, nleft);
	if(nwritten <= 0) return(nwritten);
	nleft -= nwritten;
	ptr += nwritten;
    }

    return(nbytes - nleft);
}
#endif	/* ROMSETS */

/*
	getbuf - get a receive buffer off the queue associated with port

		port - CONSOLE
		buf  - buffer to be copied to
		cnt  - max bytes to be copied

	returns actual number of bytes read from buffer
*/
getbuf(port, buf, cnt, wait)
int port;
register uChar *buf;
register int cnt;
int	wait;
{
    extern struct sccinqueue conqueue;
    register struct sccinqueue *ifq;
    register int actual;
    int s;

    if (port == CONSOLE) {
	ifq = &conqueue;
    } 
	
    /* copy data off of the queue */
    actual = 0;
    s = spl7();

#if	1
    while(actual < cnt) {
	PUNCH_WD ;
	c_rbf_hdlr () ;

	if(ifq->head != ifq->tail) {
	    ifq->head = (ifq->head + 1) & ifq->mask;
	    *buf++ = ifq->buf[ifq->head];
	    actual++;
	} else {
	    /* no characters or end-of-input, sleep or get out */
	    if(actual == 0) {
		    actual = (-1);
	    }
	break;
	}
    }
#else
    actual = scc_input (CONSOLE, buf, cnt) ;
    if (actual >= 0)
    {
    	*buf++ = actual ;
        actual = 1;
    }
#endif

    splx(s);
    return(actual);
}

/* this routine checks to see if a character is ready to be read, and if
 * it is, returns it.  Otherwise, it returns -1. */
int
poll_char(port)
int port;
{
    int tty, retval;
    Int16 nwaiting;
    char buf[2];

    /* determine what our console is */
    tty = get_cur_tty();

    if((tty == CONSOLE) || (tty == DEBUGPORT))
	/* check for a character without waiting */
        return(getbuf(tty, buf, 1, 0) == (-1) ? (-1) : (unsigned int) (*buf));
}

/*
	getch - get 1 character from port

		port - either CONSOLE
		buf  - buffer for the 1 character
*/
int
getch(port, buf) 
int port;
uChar *buf;
{
    uInt32 tty;
    int retval = 0;

    tty = get_cur_tty();

    if((tty == CONSOLE) || (tty == DEBUGPORT))
	do {
        	retval = getbuf (tty, buf, 1, 1) ;
	} while (retval < 0) ;

    return (retval) ;
}

/*
    getdbgch - get 1 character from port

        port - either CONSOLE
        buf  - buffer for the 1 character
    This routine cannot sleep because it is running out of the debugger
    thread.
*/
int
getdbgch(port, buf)
int port;
uChar *buf;
{
    return(getbuf(port, buf, 1,0));
}

void 
putch( c ) 
uChar c;
{
	int s;
	uInt32 tty;

    /* Get current terminal.  */
    tty = get_cur_tty();

        if( (tty == CONSOLE) || (tty == DEBUGPORT) ){
            if ((c == NL) && ! (DONT_MAP_CRLF))
                scc_output(tty,&lf,1);
            scc_output(tty, &c, 1);     /* write the char to the console */
         }
}

void
puts( str ) 
register unsigned char *str;
{
	while( *str )
		putch( *str++ );
}

/* helper function for _doprnt(); this routine flushes the output buffer
 * when it gets full, and prevents dropping characters */
int
_flsbuf(ch, filep)
    char            ch;
    FILE           *filep;
{
    int             i = 0;
    char           *tmp;

    if (filep->_flag & _IOWRT) {
    /* this is an output buffer */
    tmp = filep->_base; /* record the base */

    /* walk the base up to the pointer */
    while (filep->_base != filep->_ptr) {
        i++;
        putch(*filep->_base);
        filep->_base++;
    }

    /* update the pointers */
    filep->_cnt = filep->_bufsiz;  /* full size buffer */
    filep->_ptr = filep->_base = tmp;   /* reload the base */

    /* output the character */
    putch(ch);
    }

    return (i);
}

printf(fmt, args)
    char *fmt;
    int args;
/*********************************************************************
Function: 
	Perform the printf function.  The dirty work is really
	done in doprnt in libc.  This routine calls doprnt to format the 
	output then use putch to write the formatted string.

Parameters: 
Input:      fmt	- Format string as called out in C manual.
			args - Arguements as called out in C manual.
Output:     
Return:     No value returned.
Notes:
	The destination device/socket is determined in the putch call.
Modification History:

	7-9-92 RSM  Allow nested calls to printf.  Ported from Andy's 
				ppfhost code.
*********************************************************************/

{
    int len;
	int	s;
	struct _iobuf *obuf;
	/*
		See if we are out of io buffers, this means that we have
		nested too deep (too many nest calls to printf).
	*/
	s = spl7();
	if(cur_depth == DEFAULT_DEPTH){
		/*
			Drop print.
		*/	
		splx(s);
		return;
	}
		
	obuf = &_iob[cur_depth];
    obuf->_base = obuf->_ptr = stdout_buf[cur_depth];
	cur_depth ++;
	splx(s);
    obuf->_bufsiz = obuf->_cnt = IOBUFSIZE;
    obuf->_flag = _IOWRT;
    len = _doprnt(fmt, &args, obuf);
    while(obuf->_base != obuf->_ptr){
        putch(*obuf->_base); 
        obuf->_base ++;
    }
	s = spl7();
	cur_depth --;
	splx(s);
}

/*--------------------------------------
 *	NAME:
 *		getline()
 *
 *	WHAT:
 *		This function gets a line of input from the console. The
 *		function allows input until a carriage return is entered.
 *		Non-printable characters are ignored. Accepted characters
 *		are echoed to the console. Input is not accepted after a
 *		maximum length has been reached. Backspace is valid input
 *		and produces a destructive backspace on the console.
 *
 *		The input is placed into a buffer supplied by the caller
 *		and is NULL terminated. Returns the length of the line.
 *
 */


unsigned short 
getline( buf, max )
register unsigned char buf[];		/* buffer to place line into */
register unsigned short	max;		/* max length of input to accept */
{
	register unsigned short	cnt;	/* char counter */
	unsigned char c;		/* input char */

	cnt = 0;
	for( getch(CONSOLE, &c); c != CR ; getch(CONSOLE, &c) ) /* while not CR */
	{
		if( isprint( c ) )
		{             
			if( cnt < max )
				{
				putch( c );				/* echo it */
				buf[ cnt++ ] = c;		/* copy to buffer */
				}
		}		

		/* if back space and buffer not empty, delete a char */
		if( c == BS && cnt != 0)  
		{
			putch( c );
			putch( SP );
			putch( BS );
			cnt--;
		}
	}

	buf[ cnt ] = '\0';		/* add a NULL to end of string  */
	putch( CR );
	putch( '\n' );
	return( cnt );			/* return length of string	*/

} /* End of getline() -----------------*/

/* ------------------------------------------------------------	*/
/*	For Synergy ATM line card, just indicate debug console.	*/
/* ------------------------------------------------------------	*/
uInt32 get_cur_tty ()
{
	return (CONSOLE) ;
}	/* get_cur_tty */

/* ------------------------------------------------------------	*/
/*	For Synergy ATM line card, initialize this module.	*/
/* ------------------------------------------------------------	*/
void IOBT_Init ()
{
	cur_depth = 0  ;		/* Init own data. */
	lf        = CR ;		/* Init own data. */
	scc_init (CONSOLE, 0) ;		/* Init h/w i/f.  */

}	/* IOBT_Init */
