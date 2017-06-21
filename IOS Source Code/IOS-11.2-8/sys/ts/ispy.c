/* $Id: ispy.c,v 3.1.10.2 1996/08/28 13:18:11 thille Exp $
 * $Source: /release/112/cvs/Xsys/ts/ispy.c,v $
 *------------------------------------------------------------------
 * ISPY - spy on a TTY line
 *
 * Feb 1996, Andy Valencia
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ispy.c,v $
 * Revision 3.1.10.2  1996/08/28  13:18:11  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.10.1  1996/05/21  10:07:10  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1  1996/03/01  16:39:55  vandys
 * CSCdi50393:  No way to monitor TTY lines remotely
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "types.h"
#include "master.h"
#include "../ui/common_strings.h"
#include "globs.h"
#include "subsys.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/msg_parser.c"	/* Not a typo, see logger.h */
#include "ttysrv.h"

#define MAXSPY (4096)		/* Max data buffered while spying */

/*
 * Context maintained when spying on a line.
 * NOTE: the buffer is filled from ISR's, be careful.
 */
struct ispyctx {
    int count;		/* Bytes buffered */
    int head, tail;	/* FIFO queue */
    char buf[MAXSPY];	/* Data buffered */
    int watcher;	/* Who's spying */
};

/*
 * do_ispy()
 *	Entered from tty->ispy vector, to watch new data
 *
 * Called from the ISR for an async interface, so keep it simple
 * and fast.
 */
static void
do_ispy (tt_soc *tty, void *buf, int len)
{
    struct ispyctx *ctx = tty->ispyctx;
    int x;

    /*
     * Cap at what's left in the circular buffer
     */
    if (len > (MAXSPY - ctx->count)) {
	len = MAXSPY - ctx->count;
    }

    /*
     * Fill in what'll fit from head through end
     */
    x = len;
    if (x > (MAXSPY - ctx->head)) {
	x = MAXSPY - ctx->head;
    }
    bcopy(buf, ctx->buf + ctx->head, x);
    ctx->count += x;
    ctx->head += x;
    buf = x + (char *)buf;
    len -= x;
    if (ctx->head >= MAXSPY) {
	ctx->head = 0;
    }

    /*
     * If there's more, fill in at front of buffer
     */
    if (len > 0) {
	bcopy(buf, ctx->buf, len);
	ctx->count += len;
	ctx->head += len;
    }
}

/*
 * ispy_BLOCK()
 *	Block until ISPY data to consume, or TTY change
 */
static boolean
ispy_BLOCK(struct ispyctx *ctx)
{
    tt_soc *tty = MODEMS[ctx->watcher];

    if (ctx->count || modemcheck(tty) || inputpending(tty)) {
	return(FALSE);
    }
    return(TRUE);
}

/*
 * pushback()
 *	Push a character onto the TTY
 *
 * We make it look like an "unget" char, so only one char of pushback
 * at a time.
 */
inline static void
pushback(tt_soc *tty, int c)
{
    if (tty->isavedchar != -1) {
	putc(stdio, '\7');
    } else {
	tty->isavedchar = c;
    }
}

/*
 * ispy_command()
 *	EXEC command to watch a line
 */
static void
ispy_command(parseinfo *csb)
{
    tt_soc *tty;
    struct ispyctx *ctx;
    int x, count, keystate, done;
    char c;

    /*
     * Access the TTY, bail if it's already being spied upon
     */
    tty = MODEMS[GETOBJ(int,1)];
    if (tty->ispy) {
	printf("Line %d is already being monitored by line %d\n",
	    GETOBJ(int,1), ((struct ispyctx *)(tty->ispyctx))->watcher);
	return;
    }

    /*
     * Get a context struct, slap it onto the TTY
     */
    ctx = malloc_named(sizeof(struct ispyctx), "ISPY buffer");
    if (ctx == NULL) {
	return;
    }
    ctx->watcher = stdio->ttynum;
    tty->ispyctx = ctx;
    tty->ispy = do_ispy;

    /*
     * Let them know how to get out, and start watching
     */
    printf("\nISPY'ing on line %d.  Type %cx to end.\n",
	GETOBJ(int,1), stdio->tty_escapechar);
    startoutput(stdio);
    keystate = 0;
    done = 0;
    while (!done) {
	/*
	 * Wait for action
	 */
	edisms((void *)ispy_BLOCK, (long)ctx);

	/*
	 * TTY in trouble--done
	 */
	if (modemcheck(stdio)) {
	    done = 1;
	    continue;
	}

	/*
	 * Typing.  Either push data onto target as "typeahead", or
	 * interpret escape.
	 */
	if (inputpending(stdio)) {
	    c = getc(stdio);
	    switch (keystate) {
	    case 0:
		/*
		 * State 0, new data input.  If it's an escape char,
		 * go to state 1.  If there's already typeahead pushed
		 * on the TTY, just beep.  Otherwise push the data
		 * onto the target TTY.
		 */
		if (c == stdio->tty_escapechar) {
		    keystate = 1;
		} else {
		    pushback(tty, c);
		}
		break;

	    case 1:
		/*
		 * State 1, escape char seen.  If it's ESC-x, finish.
		 * Otherwise push this char onto the TTY.
		 */
		if (c == 'x') {
		    done = 1;
		    break;
		}
		keystate = 0;
		pushback(tty, c);
	    }
	}

	/*
	 * If there's buffered ISPY data, show it
	 */
	count = ctx->count;
	if (count) {
	    for (x = 0; x < count; ++x) {
		c = ctx->buf[ctx->tail];
		ctx->tail += 1;
		if (ctx->tail >= MAXSPY) {
		    ctx->tail = 0;
		}
		putc(stdio, c);
	    }

	    /*
	     * Update buffer count.  Because the driver can fill it
	     * in, interlock interrupts while adjusting.
	     */
	    {
		ulong olev;

		olev = disable_interrupts();
		ctx->count -= count;
		enable_interrupts(olev);
	    }

	    /*
	     * Kick off output
	     */
	    startoutput(stdio);
	}
    }

    /*
     * All done.  Disable ISPY, free memory, and return
     */
    printf("\nFinished.\n");
    tty->ispy = NULL;
    tty->ispyctx = NULL;
    free(ctx);
}

/*
 * Parse chain to add ispy command
 */
#define ALTERNATE NONE
#include "exec_ispy.h"
LINK_POINT(exec_ispy_command, ALTERNATE);
#undef ALTERNATE

parser_extension_request ispy_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(exec_ispy_command) },
    { PARSE_LIST_END, NULL }
};

/*
 * ispy_init()
 *	Initialize ISPY subsystem
 */
static void
ispy_init (subsystype *subsys)
{
    parser_add_command_list(ispy_chain_init_table, "ISPY");
}

/*
 * Subsys header
 */
#define ISPY_SCRIPT_MAJVERSION 1
#define ISPY_SCRIPT_MINVERSION 0
#define ISPY_SCRIPT_EDITVERSION  1

SUBSYS_HEADER(ispy, ISPY_SCRIPT_MAJVERSION, ISPY_SCRIPT_MINVERSION,
	ISPY_SCRIPT_EDITVERSION,
	ispy_init, SUBSYS_CLASS_MANAGEMENT,
	NULL, NULL);
