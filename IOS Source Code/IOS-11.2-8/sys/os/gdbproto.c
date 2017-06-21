/* $Id: gdbproto.c,v 3.2.60.2 1996/07/10 22:22:12 dstine Exp $
 * $Source: /release/112/cvs/Xsys/os/gdbproto.c,v $
 *------------------------------------------------------------------
 * gdbproto.c  --  GDB protocol handling
 *  
 * October 1, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gdbproto.c,v $
 * Revision 3.2.60.2  1996/07/10  22:22:12  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  21:29:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.3  1996/03/09  05:09:41  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.24.2  1996/03/07  10:18:41  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  16:39:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:48:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:46:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:55:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "string.h"
#include "gdb.h"
#include "../dev/monitor1.h"

/* Local storage */

boolean gdb_reqreload = FALSE;
boolean gdb_debug = FALSE;
jmp_buf gdb_env;

/*
 *
 *    The following gdb commands are supported:
 * 
 * command          function                               Return value
 * 
 *    g             return the value of the CPU registers  hex data or ENN
 *    G             set the value of the CPU registers     OK or ENN
 * 
 *    mAA..AA,LLLL  Read LLLL bytes at address AA..AA      hex data or ENN
 *    MAA..AA,LLLL: Write LLLL bytes at address AA.AA      OK or ENN
 * 
 *    c             Resume at current address              SNN   ( signal NN)
 *    cAA..AA       Continue at address AA..AA             SNN
 * 
 *    s             Step one instruction                   SNN
 *    sAA..AA       Step one instruction from AA..AA       SNN
 * 
 *    k             kill
 *
 *    ?             What was the last sigval ?             SNN   (signal NN)
 * 
 * All commands and responses are sent with a packet which includes a 
 * checksum.  A packet consists of 
 * 
 * $<packet info>#<checksum>.
 * 
 * where
 * <packet info> :: <characters representing the command or response>
 * <checksum>    :: < two hex digits computed as modulo 256 sum of <packetinfo>>
 * 
 * When a packet is received, it is first acknowledged with either '+' or '-'.
 * '+' indicates a successful transfer.  '-' indicates a failed transfer.
 * 
 * Example:
 * 
 * Host:                  Reply:
 * $m0,10#2a               +$00010203040506070809101112131415#42
 * 
 ****************************************************************************/

/*
 * gethexnum - parse a hex string returning a value
 *
 * This is a low-level routine for converting hex values into binary. It
 * is used by the 'parse' routines to actually perform the conversion.
 *
 * Entry : srcstr	- Pointer to string to convert
 *	   retstr	- Pointer to cell which will contain the address 
 *			  of the first byte not converted
 *			- Pointer to cell to return value
 */

boolean gethexnum(char *srcstr, char **retstr, int *retvalue)
{
    char *str = srcstr;
    ulong value = 0;

    /* Convert all of the digits until we get a non-hex digit */

    while (*str && (((*str >= 'a') && (*str <= 'f')) ||
		    ((*str >= '0') && (*str <= '9')))) {
	value = value*16 + (*str <= '9' ? (*str++ -'0') : 
			    (*str++ -'a'+10));
    }
    
    /* Return failure if we are still pointing at the start */
    
    if (str == srcstr) {
	return(FALSE);
    }
    
    /* Set up the return values and return success */
    
    *retvalue = value;
    *retstr = str;
    return(TRUE);
}

/*
 * parsehexnum - Parse a single hex number
 *
 * This routine is used to convert a hex value into binary. It uses gethexnum
 * to perform the actual conversion, and ignores any invalid characters at
 * the end of the string.
 */

boolean parsehexnum(char *srcstr, int *retvalue)
{
    char *dummy;
    
    return(gethexnum(srcstr, &dummy, retvalue));
}

/*
 * parse2hexnum - Parse two hex numbers
 *
 * This routine converts a string of two numbers, seperated by commas,
 * into two binary values. Note that if either of the values can not
 * be returned, this routine will return failure and not update either
 * return value.
 */
boolean parse2hexnum(char *srcstr, int *retvalue1, int *retvalue2)
{
    char *str;
    int value1, value2;
    
    if (!gethexnum(srcstr, &str, &value1) || (*str++ != ',') ||
	!gethexnum(str, &str, &value2)) {
	return(FALSE);
    }
    
    *retvalue1 = value1;
    *retvalue2 = value2;
    return(TRUE);
}

/* 
 * scan for the sequence $<data>#<checksum>
 */
boolean getpacket(debug_context *ctx)
{
    unsigned char checksum;
    unsigned char xmitcsum = -1;
    int  i;
    int  count;
    int ch = -1;
  
    do {
	/* wait around for the start character, ignore all other characters */
	while (ch != '$') {
	    ch = (*(ctx->getchar))();
	    if (ch == -1)
		return(FALSE);
	}

	checksum = 0;
	count = 0;
    
	/* now, read until a # or end of buffer is found */
	while (count < BUFMAX) {
	    ch = (*(ctx->getchar))();
	    if (ch == -1)
		return(FALSE);
	    if (ch == '#')
		break;
	    checksum = checksum + ch;
	    ctx->scratchbuf[count++] = ch;
	}
	ctx->scratchbuf[count] = 0;
	gdb_expand(ctx->scratchbuf, ctx->inbuf);
	
	if (ch == '#') {
	    ch = (*(ctx->getchar))();
	    if (ch == -1)
		return(FALSE);
	    xmitcsum = chartohex(ch) << 4;
	    
	    ch = (*(ctx->getchar))();
	    if (ch == -1)
		return(FALSE);
	    xmitcsum += chartohex(ch);
	    if ((gdb_debug ) && (checksum != xmitcsum)) {
		gdb_printf("bad checksum.  My count = 0x%x, sent=0x%x. buf=%s\n",
			   checksum,xmitcsum,ctx->inbuf);
	    }
      
	    if (checksum != xmitcsum)
		(*(ctx->putchar))('-');  /* failed checksum */ 
	    else {
		(*(ctx->putchar))('+');  /* successful transfer */
		/* if a sequence char is present, reply the sequence ID */
		if (ctx->inbuf[2] == ':') {
		    (*(ctx->putchar))(ctx->inbuf[0]);
		    (*(ctx->putchar))(ctx->inbuf[1]);
		    /* remove sequence chars from buffer */
		    count = strlen(ctx->inbuf);
		    for (i=3; i <= count; i++)
			ctx->inbuf[i-3] = ctx->inbuf[i];
		} 
	    } 
	} 
	(*(ctx->flush))();
    } while (checksum != xmitcsum);
    return(TRUE);
}

/* 
 * send the packet in buffer.  The host get's one chance to read it.  
 * This routine does not wait for a positive acknowledge.
 */

void putpacket(debug_context *ctx)
{
    unsigned char checksum;
    int  count;
    char ch;
  
    gdb_compress(ctx->outbuf, ctx->scratchbuf);

    /*  $<packet info>#<checksum>. */

    (*(ctx->putchar))('$');
    checksum = 0;
    count    = 0;
  
    while ((ch = ctx->scratchbuf[count])) {
	(*(ctx->putchar))(ch);
	checksum += ch;
	count += 1;
    }
	
    (*(ctx->putchar))('#');
    (*(ctx->putchar))(tohexchar(checksum >> 4));
    (*(ctx->putchar))(tohexchar(checksum));
    (*(ctx->flush))();
}

/* 
 * convert the memory pointed to by mem into hex, placing result in buf
 * return a pointer to the last char put in buf (null)
 */
char* mem2hex(char *mem, char *buf, int count)
{
    int i;
    unsigned char ch;

    for (i=0;i<count;i++) {
	ch = *mem++;
	*buf++ = tohexchar(ch >> 4);
	*buf++ = tohexchar(ch);
    }
    *buf = 0; 
    return(buf);
}

/*
 * convert the hex array pointed to by buf into binary to be placed in mem
 * return a pointer to the character AFTER the last byte written
 */

char* hex2mem(char *buf, char *mem, int count)
{
    int i;
    unsigned char ch;

    for (i=0;i<count;i++) {
	ch = chartohex(*buf++) << 4;
	ch = ch + chartohex(*buf++);
	*mem++ = ch;
    }
    return(mem);
}

/* These procedures manage a semaphore that controls the protocol for sending
 * relocation packets.  gdb_set_reloc_sem should be called to set the
 * semaphore whenever the router enters a debugging session to force the
 * protocol to transmit a relocation packet in response to the first '?'
 * prompt from the gdb client.
 *
 * We use a semaphore instead of a parameter because the connection between
 * gdb_command and gdb_interface *may* be through the breakpoint exception
 * handler, which prohibits knowing how we were invoked.
 */

static boolean gdb_send_reloc;

void gdb_set_reloc_sem(void)
{
  gdb_send_reloc = TRUE;
}
boolean gdb_test_reloc_sem(void)
{
  return gdb_send_reloc;
}
void gdb_clear_reloc_sem(void)
{
  gdb_send_reloc = FALSE;
}

/*
 * This function does all command procesing for interfacing to gdb.
 */
boolean gdb_interface(debug_context *ctx, boolean readonly)
{
    int    length;

    if (gdb_debug) 
	gdb_show_exception_info(ctx);

    /* 
     * Indicate that we've gone back to debug mode
     */
    for (length = 0; length < 4; length++)
	(*(ctx->putchar))('|');
    (*(ctx->flush))();

    while (getpacket(ctx)) { 

	ctx->outbuf[0] = 0;

        switch (ctx->inbuf[0]) {

	/* Tell the gdb client our signal number */
	case '?' :
	    if (gdb_test_reloc_sem()) {
		gdb_clear_reloc_sem();
		if (gdb_debug)
		    gdb_printf ("N%02x%08x;%08x;%08x",
				ctx->signal,
				TEXT_START,
				DATA_START,
				DATA_END);
		  
		sprintf(ctx->outbuf,"N%02x%08x;%08x;%08x",
			ctx->signal,
			TEXT_START,
			DATA_START,
			DATA_END);
	    } else {
		if (gdb_debug)
		    gdb_printf ("S%02x", ctx->signal);

		sprintf(ctx->outbuf,"S%02x", ctx->signal);
	    }

	    for (length=1;ctx->outbuf[length];length++) {
		if ((ctx->outbuf[length] >= 'A') && 
		    (ctx->outbuf[length] <='Z'))
		    ctx->outbuf[length]=ctx->outbuf[length]+('a'-'A');
	    }

	    break;

	/* toggle debug flag */
	case 'd' :
	    gdb_debug = !(gdb_debug);
	    break; 
	
	/* return the value of the CPU registers */
        case 'g' : 
	    gdb_get_cpuregs(ctx);
	    break;

	/* set the value of the CPU registers - return OK */
	case 'G' : 
	    if (!readonly)
		gdb_set_cpuregs(ctx);
	    strcpy(ctx->outbuf,"OK");
	    break;
      
	/* mAA..AA,LLLL  Read LLLL bytes at address AA..AA */
	case 'm' : 
	    gdb_read_membytes(ctx);
	    break;
      
	/* MAA..AA,LLLL: Write LLLL bytes at address AA.AA return OK */
	case 'M' : 
	    if (readonly) {
		strcpy(ctx->outbuf, "OK");
	    } else {
		gdb_write_membytes(ctx);
	    }
	    break;
     
	/* cAA..AA    Continue at address AA..AA(optional) */
	case 'c' : 
	    if (gdb_reqreload) {
		mon_reload();
	    }

	    gdb_proc_continue(ctx, 0);	/* continue the process */
	    cache_flush();		/* Flush the cache */
	    return(TRUE);

	/* sAA..AA   Step one instruction from AA..AA(optional) */
	case 's' : 
	    if (gdb_reqreload) {
		mon_reload();
	    }

	    gdb_proc_continue(ctx, 1);	/* step one instruction */
	    cache_flush();		/* Flush the cache */
	    return(TRUE);
          
	} /* switch */ 
    
	/* reply to the request and flush the caches */
	putpacket(ctx);
	cache_flush();
    }
    return(FALSE);
}
