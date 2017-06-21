/* $Id: flh_log.c,v 3.2 1995/11/17 07:41:15 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/flh_log.c,v $
 *------------------------------------------------------------------
 * flh_log.c routines to handle flh buffer logging in ROMMON
 *
 * Oct 1994, Eric Fronberg
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Flash Load Helper (FLH) automactically dives into rxboot to load
 * an image into flash then reloads as per boot bits (usually to a system
 * image).  The routines in this file provide a mechanism to store console
 * output into a log in system memory and to display it.  Thus rxboot console
 * message are retained to be displayed later.
 *------------------------------------------------------------------
 * $Log: flh_log.c,v $
 * Revision 3.2  1995/11/17  07:41:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 *------------------------------------------------------------------
 */

#include "types.h"
#include "stdio.h"
#include "flh.h"
#include "flh_log.h"

struct _flhlog_begin *start_flh; /* flh flh logging pointer */
struct _flhlog_end *end_flh ;
static uchar last_char;

/*
 * init_flh_buffer
 * This routine initializes a buffer in memory (at the top of Processor memory)
 * for the use of holding console output during flh/bootstrap operations.
 */
uint init_flh_buffer(uchar *mptr)
{
    uint flhbuf_size = 0;
    end_flh = (struct _flhlog_end *)((uint)mptr - 
					sizeof(struct _flhlog_end));
    start_flh = 0;
    last_char = 0;

	/* first determine if there is an flh buffer there */
    if (end_flh->magic1 == FLHLOGMAGIC1) {
	start_flh = (struct _flhlog_begin *)
	    ((uint) mptr - end_flh->length);
	if (start_flh->magic2 == FLHLOGMAGIC2) { /* we got a buffer! */
	    flhbuf_size = end_flh->length;
	}
 	return(flhbuf_size);
    } 
    /*
     * no log found, create one 
     */
    end_flh->magic1 =  FLHLOGMAGIC1;
    end_flh->length = DEFAULT_FLHLOG_SIZE;
    end_flh->dummy = 0;
    start_flh = (struct _flhlog_begin *) ((ulong)mptr -
					     DEFAULT_FLHLOG_SIZE);
    start_flh->start = (uchar *)((ulong) start_flh +
				    sizeof(struct _flhlog_begin));
    start_flh->end = (uchar *)((uint)end_flh - 1);
    start_flh->next_input =
	start_flh->output_ptr =
	    start_flh->start;
    start_flh->magic2 = FLHLOGMAGIC2;
    flhbuf_size = end_flh->length;
    *start_flh->next_input = 0;

    return(flhbuf_size);
}

/*
 * flh_putc -- put characters into buffer for later printing.
 * this routine compresses data by keeping track of the previous character
 * entered into the buffer.  If we match the previous charater (last_char) we
 * will mark the location we are storing with 'FLHREPEAT_FLAG' and hold a count
 * of the number of times the character has been repeated (up to a count of
 * 0x7f)
 * When entering characters into the buffer there are four possible situations
 * that need to be handled
 *
 * 'lc' = last character
 * 'c' = present character
 * FLHREPEAT_FLAG = 0x80
 * character count is in range of 1 to 0x7f
 *
 * Case 1: String = "XXXc"  eg c != lc
 * 
 * Previous Char
 * in Buffer
 *  |
 *  |   --Next char to be written
 *  |   |
 *  V   V
 * ---------
 * | X | 0 |
 * ---------
 * Result: just put c into slot
 * 
 * Case 2: String = "XXXcc"  c == lc
 * 
 * Previous Char
 * in Buffer
 *   |
 *   |   --Next char to be written
 *   |   |
 *   V   V
 * ---------
 * | c | 0 |
 * ---------
 * 
 * Result: put '0x81' into slot
 * 
 * Case 3: String = "XXXccc"
 * 
 * Previous Char
 * in Buffer
 *   |
 *   |    ---Count of Character c
 *   |    |
 *   |    |    --Next char to be written
 *   |    |    |
 *   V    V    V
 * ----------------
 * | c | 0x81 | 0 |
 * ----------------
 * Result: Increment count field  (0x81 -> 0x82) to maximum of 0xff
 * 
 * 
 * Case 4: String = "XXXccc...c" Where count of c's = 0x7f+1
 * 
 * Previous Char
 * in Buffer
 *   |
 *   |    ---Count of Character c
 *   |    |
 *   |    |    --Next char to be written
 *   |    |    |
 *   V    V    V
 * ----------------
 * | c | 0xff | 0 |
 * ----------------
 * Result put c into next slot (same as case 1 above)
 *
 */


void flh_putc(char c)
{
    uchar *previous_ptr;
    uchar ch2write = (uchar)(c & FLHASCIIMASK);
    ushort level;

    if (start_flh == (struct _flhlog_begin *) FLHLOGMAGIC1) {
	return;
    }
    level = sil(7);		/* dis intrs while logging */
    /* try to do some compression to squeze more space out of log */
    if (last_char == (uchar)c) {
	previous_ptr = flh_decr(start_flh->next_input);
	if (*previous_ptr == (uchar)c) {
	    ch2write = FLHREPEAT_FLAG + 1;
	} else if ((*previous_ptr & FLHREPEAT_FLAG) &&
		   ((*previous_ptr & FLHASCIIMASK) < FLHASCIIMASK)) {
	    start_flh->next_input = previous_ptr;
	    ch2write = (((*start_flh->next_input & FLHASCIIMASK) + 1)  |
			FLHREPEAT_FLAG);
	}
    }
    *start_flh->next_input = ch2write;
    start_flh->next_input =
	(uchar *) flh_incr(start_flh->next_input);
    *start_flh->next_input = 0;
    if (start_flh->next_input == start_flh->output_ptr) {
	start_flh->output_ptr =
	    (uchar *) flh_incr(start_flh->output_ptr);
    }
    last_char = (uchar)c;
    sil(level);
}

/* 
 * flh_incr -- bump the pointer to the flh buffer 
 */

uchar *flh_incr(ptr)
    uchar *ptr;
{
    ptr++;
    if (ptr > start_flh->end) {
	ptr = start_flh->start;
    }
    return(ptr);
}

/* 
 * flh_decrr -- decrement the pointer to the flh buffer 
 */

uchar *flh_decr(ptr)
    uchar *ptr;
{
    ptr--;
    if (ptr < start_flh->start) {
	ptr = start_flh->end;
    }
    return(ptr);
}

/*
 * printout the contents of our FLH log history.  Assumes compression
 * described above.
 */
void flhbuf_print(char *buf,int size)
{
    char print_last_char;
    uchar *outptr;
    int cnt;

    /* 
     * We will print the buffer in two passes.  The first will print from the 
     * 'output' pointer to the end of the buffer.  The second pass will
     * print from the beginning of the buffer to the 'input' pointer
     */

    if (start_flh != (struct _flhlog_begin *) FLHLOGMAGIC1) { 
				/* non Magic means valid pointer */
	print_last_char = 0;
	outptr = start_flh->output_ptr;
	if (*outptr & FLHREPEAT_FLAG) { /* we wrapped on top of a repeated */
				/* character -- don't print repeat count  */
	    outptr++;
	}
	while(*outptr && size) {
	    if (*outptr == '\r') {
		if (print_last_char == '\n') {
		    print_last_char = '\r';
		    outptr++;
		    continue;
		}
	    }
	    if (*outptr & FLHREPEAT_FLAG) {
		for (cnt = (*outptr & FLHASCIIMASK);size && cnt > 0; cnt --) {
		    *buf++ = print_last_char;
		    size--;
		}
	    } else {
		*buf++ = *outptr;
		size--;
		print_last_char = *outptr;
	    }
	    outptr++;
	}
	/* now let pick up first part of output */
	if (start_flh->output_ptr != start_flh->start) {
	    outptr = start_flh->start;
	    while(*outptr && size) {
		if (*outptr == '\r') {
		    if (print_last_char == '\n') {
			print_last_char = '\r';
			outptr++;
			continue;
		    }
		}
		if (*outptr & FLHREPEAT_FLAG) {
		    for (cnt = (*outptr & FLHASCIIMASK);size && cnt > 0; cnt--) {
			*buf++ = print_last_char;
			size--;
		    }
		} else {
		    *buf++ = *outptr;
		    size--;
		    print_last_char = *outptr;
		}
		outptr++;
	    }
	}
    }
}

/*
 * clear_flh_buffer
 */

void clear_flh_buffer()
{

    if (start_flh ) {
	start_flh->next_input =
	    start_flh->output_ptr =
		start_flh->start;
	*start_flh->next_input = 0;
    }
}

uint flh_log_size()
{
    if (end_flh) {
	return(end_flh->length);
    }
    return(0);
}



