/* $Id: random_fill.c,v 3.2 1996/03/04 22:00:13 lol Exp $
 * $Source: /release/111/cvs/Xsys/util/random_fill.c,v $
 *------------------------------------------------------------------
 * random_fill.c  - Generate long random numbers for security applications
 *
 * December, 1995 by Bill Westfeld
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: random_fill.c,v $
 * Revision 3.2  1996/03/04  22:00:13  lol
 * CSCdi48390:  radmon fill code fails to check getbuffer() return value
 *
 * Revision 3.1  1996/01/11  10:52:34  billw
 * CSCdi43115:  PPP CHAP should use larger challenge for radius
 * compatibility
 * Also implement random_fill() which will fill an arbitrary string of
 * bytes with random and unpredictable data bytes
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "md5.h"

/*
 * random_fill
 * Fill a region of memory with random bytes.  Be careful that the bytestream
 * generated is random in the "secure" sense, rather than just the statistical
 * sense.  See RFC1750.TXT.  Our approach is to take a bunch of somewhat
 * random numbers from around the system, concat them together and run them
 * through MD5 as a mixing function.  This means that this routine is NOT
 * very fast.  In addition, we'll block before each MD5 calculation if
 * blockflag is set (which it should be most of the time if the number of
 * bytes requested is more than MD5_LEN!!!)
 *
 * Note that this is md5 dependent, and all the modules
 * that are likely to use it will have md5 present anyway.
 */

static struct {
    ulong pseudorandom;		/* A pseudorandom number from random() */
    ulong unique_number;	/* created from current time/MACaddr/counter */
    uchar lastrandom[MD5_LEN];	/* our last random number */
    ulong microseconds;		/* current time */
    ulong sched_int_count;	/* stuff from scheduler */
    ulong sched_idle_count;	/* more sched stuff */
    ulong free_malloc_trace[2];	/* stuff from free */
    ulong buffer_pak;		/* some junk from a recent packet */
    uchar buffer_pakdata[10];	/*   more such junk */
}  randomstuff;

static volatile int rand_bytesleft = 0;

/*
 * terrible declarations of external randomish data
 */
#include "logger.h"
#include "../os/free.h"
#include "../util/random.h"
#include "../os/clock.h"
#include "../h/packet.h"
#include "../os/sched_private.h"
#include "../if/network.h"

void random_fill (uchar *dest, int l, boolean blockflag)
{
    while (l > 0) {
	if (rand_bytesleft <= 0) {
	    MD5_CTX ctx;
	    paktype *pak;

	    randomstuff.pseudorandom = random_gen();
	    randomstuff.unique_number = create_unique_number();
	    randomstuff.microseconds = clock_get_microsecs();
	    randomstuff.sched_int_count = int_count;
	    randomstuff.sched_idle_count = idle_count;
	    randomstuff.free_malloc_trace[0] = malloctrace[0].pc;
	    randomstuff.free_malloc_trace[1] = malloctrace[1].pc;
	    pak = getbuffer(1023 & random_gen());
	    /*
	     * If we didn't get a buffer, we are low on memory.  Don't
	     * worry about it.  We will still be random enough.
	     */
	    if (pak) {
		randomstuff.buffer_pak = (ulong) pak;
		bcopy(pak->datagramstart, randomstuff.buffer_pakdata,
		      sizeof randomstuff.buffer_pakdata);
		datagram_done(pak);
	    }
	    MD5Init(&ctx);
	    MD5Update(&ctx, (uchar *)&randomstuff, sizeof randomstuff);
	    MD5Final(&randomstuff.lastrandom[0], &ctx);
	    rand_bytesleft = MD5_LEN;
	    if (blockflag) {
		/*
		 * It's possible someone will used up all our new random
		 * bytes while we suspend.  So we need to get our data
		 * before suspending to ensure that we make some progress.
		 */
  	        *dest++ = randomstuff.lastrandom[--rand_bytesleft];
		l--;
		process_suspend();
		continue;
	    }
	}
	*dest++ = randomstuff.lastrandom[--rand_bytesleft];
	l--;
   }
}
