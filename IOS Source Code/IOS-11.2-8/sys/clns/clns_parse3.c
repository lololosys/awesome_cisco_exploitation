/* $Id: clns_parse3.c,v 3.2.60.2 1996/08/07 08:57:12 snyder Exp $
 * $Source: /release/112/cvs/Xsys/clns/clns_parse3.c,v $
 *------------------------------------------------------------------
 * clns_parse3.c -- Parse NSAP strings
 *
 * July 1992, Dave Katz
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clns_parse3.c,v $
 * Revision 3.2.60.2  1996/08/07  08:57:12  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  19:03:02  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:40:15  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:40:15  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:52:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/18  19:39:40  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:17:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ctype.h>
#undef   toupper			/* use library function call */
#undef   tolower			/* use library function call */
#include <ciscolib.h>
#include "packet.h"
#include "../clns/clns.h"

#define ISHEXDIGIT(c) (((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'F')))
#define CVHTOB(c) ((int) c - (((c >= '0') && (c <= '9')) ? '0' : 'A' - 10))
#define NOTE_ADDR_ERR_POSN() if (!addr_err_posn) addr_err_posn = endptr
#define NOTE_PFX_ERR_POSN() if (!pfx_err_posn) pfx_err_posn = endptr

/*
 * The following describes a state machine for parsing NSAP strings.  The
 * machine is a hybrid Mealy/Moore with a bunch of state variables on the
 * side as needed to keep the size of the transition table down.
 *
 * The result can be one of three types:
 *  1) An NSAP address (no wildcards, and an even byte multiple length)
 *  2) An NSAP prefix (only trailing wildcards)
 *  3) A masked NSAP (anything else)
 *
 * The masked NSAP can be of two subtypes, namely, an exact match and a
 * prefix match.  The former first type is particular about the length
 * of the value being matched, whereas the second is not.
 *
 * The parsing strategy is to parse for the most general form (a masked NSAP)
 * and then to determine after the fact whether the parsed expression would
 * qualify as an NSAP address or an NSAP prefix.  This is done since we
 * might not know from context what type of string is being parsed.
 */


/*
 * State types
 */
#define ST_START 0		/* start state */
#define ST_HEXDIGITS 1		/* processing hex digits */
#define ST_BINDIGITS 2		/* processing binary digits */
#define ST_DOTS 3		/* processing dots */

#define NUMSTATES 4

/*
 * Input character equivalence classes
 */
#define EQ_01 0			/* 0 and 1 */
#define EQ_29 1			/* 2 through 9 */
#define EQ_AF 2			/* A through F */
#define EQ_STAR 3		/* Asterisk */
#define EQ_DOT 4		/* Period */
#define EQ_LPAR 5		/* Left parenthesis */
#define EQ_RPAR 6		/* Right parenthesis */
#define EQ_END 7		/* End of string */
#define EQ_BAD 8		/* Illegal character */

#define NUMEQCLASSES 9		/* Number of equivalence classes */

/*
 * State entry semantic routines
 */
static int geteqclass(char inchar);
static void enter_start(char inchar);
static void enter_hex(char inchar);
static void enter_bin(char inchar);
static void enter_dots(char inchar);

/*
 * State transition semantic routines
 */
static void startdot(char inchar);
static void done(char inchar);
static void endbinstring(char inchar);
static void enddots(char inchar);
static void error(void);
static void cerror(char);


/*
 * Transition table
 *
 * The transition table is a bit weird;  The state semantic entry is called
 * as we enter the state, with a calling parameter of the *previous*
 * character seen;  the next character is then examined, the equivalence
 * class determined, and then the transition semantic (if any) is called
 * with a calling parameter of the *new* character.
 */

struct state_table_entry {
    void (*semantic)(char);	/* State entry semantic */
    struct transition {
	int next_state;		/* Next state */
	void (*semantic)(char);	/* Transition semantic */
    } transitions[NUMEQCLASSES];
} static const state_table[] = {
    /* ST_START */
    {enter_start, {
	/* Start state.  We start and end here. */
	
	/*  0-9.  Process as hex digits. */
	
	{ST_HEXDIGITS, NULL},	/* 0-1 */
	{ST_HEXDIGITS, NULL},	/* 2-9 */
	
	/*
	 * A-F.  It's illegal to start an NSAP with a non-decimal digit,
	 * since the AFI is decimal abstract syntax, so we flag an error.
	 */
	{ST_START, cerror}, /* A-F */
	
	/* Asterisk.  The hex digit routine processes this. */
	{ST_HEXDIGITS, NULL},	/* asterisk */
	
	/* Period.  Go to the DOTS state, and note that we're starting. */
	{ST_DOTS, startdot},	/* period */
	
	/* Left paren.  Start processing binary digits. */
	{ST_BINDIGITS, NULL},	/* left paren */
	
	/* Right paren.  Illegal here. */
	{ST_START, cerror}, /* right paren */
	
	/* EOS.  Error; there has to be *something*. */
	{ST_START, cerror}, /* EOS */
	
	/* Illegal character.  Complain. */
	{ST_START, cerror}}}, /* bad character */
    
    /* ST_HEXDIGITS */
    {enter_hex, {
	
	/* Hex digits.  We process hex digits and asterisks here. */
	
	/*  0-F.  Process as hex digits. */
	{ST_HEXDIGITS, NULL},	/* 0-1 */
	{ST_HEXDIGITS, NULL},	/* 2-9 */
	{ST_HEXDIGITS, NULL},	/* A-F */
	
	/*  Asterisk.  Process as a hex digit. */
	{ST_HEXDIGITS, NULL},	/* asterisk */
	
	/* Period.  Start processing periods. */
	{ST_DOTS, startdot},	/* period */
	
	/* Left paren.  Start processing binary digits. */
	{ST_BINDIGITS, NULL},	/* left paren */
	
	/* Right paren.  Error. */
	{ST_START, cerror}, /* right paren */
	
	/* EOS.  We're out of here. */
	{ST_START, done},	/* EOS */
	
	/* Illegal character.  Complain. */
	{ST_START, cerror}}}, /* bad character */
    
    /* ST_BINDIGITS */
    {enter_bin, {
	
	/* Binary digits.  We process 0, 1, and asterisks here. */
	
	/* 0-1. Process as binary digits. */
	{ST_BINDIGITS, NULL},	/* 0-1 */
	
	/* 2-f.  Error--we haven't seen a right paren yet. */
	{ST_START, cerror}, /* 2-9 */
	{ST_START, cerror}, /* A-F */
	
	/* Asterisk.  Process as a binary digit. */
	{ST_BINDIGITS, NULL},	/* asterisk */
	
	/* Period.  Process as a binary digit (we special case this one). */
	{ST_BINDIGITS, NULL},	/* period */
	
	/* Left paren.  Syntax error. */
	{ST_START, cerror}, /* left paren */
	
	/* Right paren.  Go back to processing hex digits. */
	{ST_HEXDIGITS, endbinstring}, /* right paren */
	
	/* EOS.  Error--we haven't seen a right paren yet. */
	{ST_START, cerror}, /* EOS */
	
	/* Illegal character.  Complain. */
	{ST_START, cerror}}}, /* bad character */
    
    /* ST_DOTS */
    {enter_dots, {
	
	/* Periods.  We process strings of periods here. */
	
	/* 0-F, asterisk.  Go back to processing hex digits. */
	{ST_HEXDIGITS, enddots}, /* 0-1 */
	{ST_HEXDIGITS, enddots}, /* 2-9 */
	{ST_HEXDIGITS, enddots}, /* A-F */
	{ST_HEXDIGITS, enddots}, /* asterisk */
	
	/* Period.  Continue processing periods. */
	{ST_DOTS, NULL},	/* period */
	
	/* Left paren.  Start processing binary digits. */
	{ST_BINDIGITS, enddots}, /* left paren */
	
	/* Right paren.  Syntax error. */
	{ST_START, cerror}, /* right paren */
	
	/* EOS.  We're out of here. */
	{ST_START, done},	/* EOS */
	
	/* Illegal character.  Complain. */
	{ST_START, cerror}}} /* bad character */
};

/*
 * Working variables.  We put 'em in a struct so it's easy to zap 'em all.
 */
struct working_vars {
    int curstate;	/* Current state */
    boolean exit;	/* We should exit the parse loop */
    char lastchar;      /* Previous character processed */
    char *endptr;	/* Ending string pointer */
    boolean errorfound;	/* TRUE if error found */
    char *addr_err_posn;/* Position of error if we want nsap_addr */
    char *pfx_err_posn;	/* Position of error if we want nsap prefix */
    
    boolean someleft;	/* Some digits to the left of ... */
    boolean someright;	/* Some digits to the right of ... */
    boolean founddots;	/* Found ... */
    int dotcount;	/* Count of consecutive periods */
    
    boolean inbindig;	/* Have seen binary digits */
    int bincount;	/* Count of consecutive binary digits, mod 4 */
    
    boolean starseen;	/* Found at least one * */
    boolean embeddedstar;/* Found at least one embedded * */
    
    int leftlen;	/* Length of lefthand side, in bits */
    int leftposition;	/* Current left position, in bits */
    int rightposition;	/* Current right position, in bits */

    /*
     * The following are one byte longer than necessary to make error checking
     * easier (we can spill over the end and then figure it out).
     */
    uchar leftside[MAX_CLNS_ADDR_LEN + 1]; /* Left side under construction */
    uchar leftmask[MAX_CLNS_ADDR_LEN + 1]; /* Left mask under construction */
    uchar rightside[MAX_CLNS_ADDR_LEN + 1]; /* Right side under construction */
    uchar rightmask[MAX_CLNS_ADDR_LEN + 1]; /* Right mask under construction */
};

static struct working_vars vars;

#define curstate vars.curstate
#define exit vars.exit
#define lastchar vars.lastchar 
#define endptr vars.endptr	
#define errorfound vars.errorfound
#define addr_err_posn vars.addr_err_posn
#define pfx_err_posn vars.pfx_err_posn
#define someleft vars.someleft
#define someright vars.someright
#define founddots vars.founddots
#define dotcount vars.dotcount
#define inbindig vars.inbindig
#define bincount vars.bincount
#define starseen vars.starseen
#define embeddedstar vars.embeddedstar
#define leftlen vars.leftlen
#define leftposition vars.leftposition
#define rightposition vars.rightposition
#define leftside vars.leftside
#define leftmask vars.leftmask
#define rightside vars.rightside
#define rightmask vars.rightmask

#define getnibble(array, position) (((array)[(position) / 8] >> (4 - ((position) % 8))) & 0xf)
#define putbit(value, array, position) {(array)[(position) / 8] |= (value) << (7 - ((position) % 8));}
#define putnibble(value, array, position) {(array)[(position) / 8] |= (value) << (4 - ((position) % 8));}
    
static const char default_string[] = {"default"};

/*
 * Parse the NSAP string.  We leave the results scattered about in the statics
 * defined above.
 */
static void parse_nsap_string (char *string)
{
    register char nextchar = '\0';
    const char *defptr;

    memset(&vars, 0, sizeof(struct working_vars));

    curstate = ST_START;

    /*
     * Bail out if the pointer is NULL or points to a null string.
     */
    if ((!string) || (!*string)) {
	errorfound = TRUE;
	endptr = string;
	return;
    }

    /* Deblank the string. */

    string = (char *) deblank(string);
    endptr = string;

    /* If the string is "default", fake up a zero-length prefix. */

    defptr = default_string;
    NOTE_ADDR_ERR_POSN();	/* So error pointer comes out right */
    for (;;) {
	if (*defptr == '\0') { /* We found the end! */
	    founddots = TRUE;
	    endptr = string;
	    return;
	}
	if (tolower(*string++) != *defptr++)
	    break;
    }
    string = endptr;
    addr_err_posn = NULL;	/* Undo the above... */

    /* 
     * Loop until we run out of input or get an error.
     */
    while (!exit) {
	int eqclass;
	const struct state_table_entry *state = &state_table[curstate];
	const struct transition *trans;

	/* Process the state entry semantic. */
	
	(*state_table[curstate].semantic)(nextchar);
	
	/* Ensure that things haven't gotten too long. */

	if (leftposition > MAX_CLNS_ADDR_LEN * 8 ||
	    rightposition > MAX_CLNS_ADDR_LEN * 8 ||
	    leftposition + rightposition > MAX_CLNS_ADDR_LEN * 8)
	    error();

	if (exit)
	    break;
	
	/* Figure out the input equivalence class. */

	lastchar = nextchar;
	endptr = string;
	nextchar = toupper(*string++);
	eqclass = geteqclass(nextchar);
	trans = &state->transitions[eqclass];
	
	/* Get the next state and execute the transition semantic. */
	
	curstate = trans->next_state;
	if (trans->semantic)
	    (*trans->semantic)(nextchar);
    }
}

/*
 * Determine the equivalence class of a character.
 */
static int geteqclass (char inchar)
{
    int eqclass;
    switch (inchar) {
    case '0':
    case '1':
	eqclass = EQ_01;
	break;
    case '.':
	eqclass = EQ_DOT;
	break;
    case '*':
	eqclass = EQ_STAR;
	break;
    case '(':
	eqclass = EQ_LPAR;
	break;
    case ')':
	eqclass = EQ_RPAR;
	break;
    case '\n':
    case '\0':
    case ' ':
    case '\t':
	eqclass = EQ_END;
	break;
    default:
	if (inchar >= '2' && inchar <= '9')
	    eqclass = EQ_29;
	else if (inchar >= 'A' && inchar <= 'F')
	    eqclass = EQ_AF;
	else
	    eqclass = EQ_BAD;
	break;
    }
    return eqclass;
}


/*
 * State semantics.  These semantics are executed whenever the associated
 * states are entered (or reentered).
 */


/*
 * START state.  Nothing to do, since we block zeroed everything.
 */
static void enter_start (char inchar)
{
}


/*
 * HEXDIGITS state.  The input character should be a hex digit or
 * a "*".  Add the nibble to the address string and adjust the mask.
 * We also get right parens here, since the BINDIGIT machine exits
 * to this state--we just ignore them.
 */
static void enter_hex (char inchar)
{
    boolean onleftside;

    /* Ignore right parens. */

    if (inchar == ')')
	return;

    /* Give an error if a previous binary string wasn't completed. */

    if (bincount != 0) {
	error();
	return;
    }

    /* Note that we've gotten some digits, and remember which side we're on. */

    if (founddots) {
	someright = TRUE;
	NOTE_PFX_ERR_POSN();
	onleftside = FALSE;
    } else {
	someleft = TRUE;
	onleftside = TRUE;
    }

    /*
     * Note if we're seeing a "*".  If we're on the right side, we know that
     * the star is embedded.
     */

    if (inchar == '*') {
	starseen = TRUE;
	NOTE_ADDR_ERR_POSN();
	if (!onleftside) {
	    embeddedstar = TRUE;
	    NOTE_PFX_ERR_POSN();
	}
    } else {

	/*
	 * Not a "*".  If we've seen one before, we now know that we
	 * have a mask (leading or embedded don't-cares).
	 */
	if (starseen) {
	    embeddedstar = TRUE;
	    NOTE_PFX_ERR_POSN();
	}
    }

    if (onleftside) {
	/*
	 * Adding to the left side.  If a digit, put in the value, extend
	 * the mask, and bump the length.  Otherwise, we just bump the
	 * position.
	 */
	if (ISHEXDIGIT(inchar)) {
	    putnibble(CVHTOB(inchar), leftside, leftposition);
	    putnibble(0xf, leftmask, leftposition);
	    leftlen = leftposition + 4;
	}
	leftposition += 4;
    } else {
	/*
	 * Adding to the right side.  Pretty much the same as the left, 
	 * except that there's no distinction between length and position.
	 */
	if (ISHEXDIGIT(inchar)) {
	    putnibble(CVHTOB(inchar), rightside, rightposition);
	    putnibble(0xf, rightmask, rightposition);
	    
	}
	rightposition += 4;
    }
}


/*
 * BINDIGITS state.  We handle left paren, "*", ".", and the digits 0 and 1.
 */
static void enter_bin (char inchar)
{
    boolean onleftside;
    switch (inchar) {

    case '(':
	/*
	 * Left paren.  It's the start of the string.  If bincount is nonzero,
	 * it means that a previous sequence wasn't completed, which is an
	 * error. */
	if (bincount != 0)
	    error();
	break;

    case '.':
	/* Period.  We only allow single periods within binary strings. */
	if (lastchar == '.')
	    error();
	break;

    case '*':
    case '0':
    case '1':

	/*
	 * Binary digits or asterisk.  Note which side of the "..." string
	 * we've found digits on.
	 */
	if (founddots) {
	    someright = TRUE;
	    NOTE_PFX_ERR_POSN();
	    onleftside = FALSE;
	} else {
	    someleft = TRUE;
	    onleftside = TRUE;
	}

	/*
	 * Note if we're seeing an asterisk.  If we're on the right side,
	 * treat it as embedded.
	 */
	if (inchar == '*') {
	    starseen = TRUE;
	    NOTE_ADDR_ERR_POSN();
	    if (!onleftside) {
		embeddedstar = TRUE;
		NOTE_PFX_ERR_POSN();
	    }
	} else {
	    /*
	     * Not an asterisk.  If we've seen one before, the asterisk was
	     * embedded, so we know we've got a mask.
	     */
	    if (starseen) {
		embeddedstar = TRUE;
		NOTE_PFX_ERR_POSN();
	    }
	}

	/* Count the digits, modulo 4, and note that we've seen one. */
	bincount = (bincount + 1) % 4;
	inbindig = TRUE;
	if (onleftside) {
	    /*
	     * Adding to the left side.  If a digit, put in the value, extend
	     * the mask, and bump the length.  Otherwise, we just bump the
	     * position.
	     */
	    if (inchar != '*') {
		putbit(inchar - '0', leftside, leftposition);
		putbit(1, leftmask, leftposition);
		leftlen = leftposition + 1;
	    }
	    leftposition++;
	} else {
	    /*
	     * Adding to the right side.  Pretty much the same as the left, 
	     * except that there's no distinction between length and position.
	     */
	    if (inchar != '*') {
		putbit(inchar - '0', rightside, rightposition);
		putbit(1, rightmask, rightposition);
		
	    }
	    rightposition++;
	}
	break;

    default:
	error();
	break;
    }
}


/*
 * DOTS state.  Count the latest dot, and barf if we've seen more than
 * three in a row.
 */
static void enter_dots (char inchar)
{
    dotcount++;
    if (dotcount > 3)
	error();
}



/*
 * Transition semantics.  The following routines are executed when traversing
 * an arc in the FSA, and thus are associated with both the previous state
 * and the current input.
 */


/*
 * Startdot.  Executed when we see the first "." as we enter DOTS state for
 * the first time.
 */
static void startdot (char inchar)
{
    dotcount = 0;
}


/*
 * Enddots.  Executed when we are exiting DOTS state, having received a
 * valid input other than a "."
 */
static void enddots (char inchar)
{
    /* Figure out how many periods we've seen in a row. */
    switch (dotcount) {

    case 1:
	/* One period.  It's a placeholder, and thus a no-op. */
	break;

    case 2:
	/* Two periods.  Bad news, this is a syntactic bummer. */
	error();
	break;

    case 3:
	/*
	 * Three periods.  This is the spongy part of a mask.  Bitch if
	 * we have seen three dots earlier, as there's a limit of one
	 * sponge per customer.
	 */
	if (founddots) {
	    error();
	} else {
	    founddots = TRUE;

	    /*
	     * Clear "bincount", since we can have a right side after a
	     * partial binary string.
	     */
	    bincount = 0;

	    /*
	     * Note the potential error offset if this is supposed to be an
	     * NSAP address.
	     */
	    NOTE_ADDR_ERR_POSN();
	}
	break;

    default:
	error();
    }
    dotcount = 0;
}


/*
 * Endbinstring.  Executed when we hit a right paren at the end of a string
 * of binary digits.
 */
static void endbinstring (char inchar)
{
    /* Complain if we didn't see any digits at all. */
    if (!inbindig)
	error();
    inbindig = FALSE;
}


/*
 * Done.  Executed when we've hit end-of-string and we think that we've
 * succeeded.
 */
static void done (char inchar)
{
    /* Minor kludge - If we're trailing some dots, process them. */

    if (dotcount > 0)
	enddots(inchar);
    exit = TRUE;
}


/*
 * Error routine.  Set the "exit" flag so we won't go any further.
 */


static void error (void)
{
    errorfound = TRUE;
    exit = TRUE;
}

static void cerror (char not_used)
{
    error();
}


/*
 * Return TRUE if what we just parsed is a valid NSAP address.
 */
static boolean is_nsap (void)
{
    /* If there's an error or a "..." or any wildcards, it's not an NSAP. */

    if (errorfound || founddots || starseen)
	return(FALSE);

    /* 
     *  If the length is zero (when just a '.' is entered) or when the
     *  length is not modulo 8, it's not an NSAP. 
     */

    if (!leftposition || (leftposition & 0x7))
	return(FALSE);

    /* Guess it's an NSAP. */

    return(TRUE);
}

/*
 * Return TRUE if what we just parsed is a valid NSAP prefix.
 */
static boolean is_nsap_pfx (boolean require_dots)
{
    /* If there's an error or a righthand side, it's not an NSAP prefix. */

    if (errorfound || someright)
	return(FALSE);

    /* If there's any embedded wildcards, it's not an NSAP prefix. */

    if (embeddedstar)
	return(FALSE);

    /* If there's no ... and require_dots is true, it's not an NSAP prefix. */

    if (require_dots && !founddots)
	return(FALSE);

    /* Guess it's an NSAP prefix. */

    return(TRUE);
}

/*
 * Return TRUE if what we just parsed is a valid masked NSAP.
 */
static boolean is_masked_nsap (void)
{
    /* If there's an error, it's not a masked NSAP; otherwise, it is. */

    return (!errorfound);
}

/*
 * Extract an NSAP from a parsed NSAP string.
 */
static void extract_nsap (nsap_addr *nsap)
{
    memset(nsap, 0, sizeof(nsap_addr));
    nsap->length = leftposition / 8;
    bcopy(leftside, nsap->addr, nsap->length);
}


/*
 * Extract an NSAP prefix from a successfully parsed string.
 */
static void extract_nsap_pfx (nsap_pfx *prefix)
{
    memset(prefix, 0, sizeof(nsap_pfx));
    prefix->length = leftlen;
    bcopy(leftside, prefix->prefix, (prefix->length + 7) / 8);
}


/*
 * Extract a masked NSAP from a successfully parsed string.
 */
static void extract_masked_nsap (masked_nsap *mskdnsap)
{
    int i;
    int rightoffset;
    int leftbytecount;

    memset(mskdnsap, 0, sizeof(masked_nsap));

    /* Get the left side. */

    leftbytecount = (leftposition + 7) / 8;
    mskdnsap->leftlength = leftposition;
    bcopy(leftside, mskdnsap->addr, leftbytecount);
    bcopy(leftmask, mskdnsap->mask, leftbytecount);

    /*
     * Get the right side.  We do this a nibble at a time, since we may need
     * to slide the right side to the right by a nibble so that it ends on
     * a byte boundary.
     */

    rightoffset = leftbytecount * 8;
    mskdnsap->rightlength = (rightposition + 7) / 8;
    if ((rightposition & 0x4) != 0) {
	rightoffset += 4;
    }

    for (i=0; i< rightposition; i+=4) {
	putnibble(getnibble(rightside, i), mskdnsap->addr, rightoffset);
	putnibble(getnibble(rightmask, i), mskdnsap->mask, rightoffset);
	rightoffset += 4;
    }

    /*
     * Now set the exact flag.  This will be true if there is a null
     * righthand side and a "..." string was not present.
     */

    if (rightposition == 0 && !founddots)
	mskdnsap->exact = TRUE;

    /*
     * Set the applymask flag.  This will be true if there is an embedded
     * zero bit in the mask (an embedded "*), or if the left or right hand
     * sides were not on octet boundaries, or if the "exact" flag is set.
     */
    if (embeddedstar || (leftposition & 0x7) != 0 || 
	(rightposition & 0x7) != 0 || mskdnsap->exact) {
	mskdnsap->applymask = TRUE;
    }
}


/*
 * Get an NSAP from an input string.  Returns FALSE if the string is not a
 * valid NSAP.
 */
boolean get_nsap_addr (
    char *string,
    nsap_addr *nsap,
    int *offset)
{
    parse_nsap_string(string);
    if (offset)
	*offset = endptr - string;
    if (is_nsap()) {
	extract_nsap(nsap);
	return(TRUE);
    } else {
	if (offset) {
	    if (addr_err_posn) {
		*offset = addr_err_posn - string;
	    }
	}
	return(FALSE);
    }
}


/*
 * Get an NSAP prefix from an input string.  Returns FALSE if the string is
 * not a valid NSAP prefix.  This routine does not require a trailing "..."
 * for the string to be a valid prefix.
 */

#ifdef NOBODY_CALLS_ME
boolean get_nsap_pfx (
    char *string,
    nsap_pfx *prefix,
    int *offset)
{
    parse_nsap_string(string);
    if (offset)
	*offset = endptr - string;
    if (is_nsap_pfx(FALSE)) {
	extract_nsap_pfx(prefix);
	return(TRUE);
    } else {
	if (offset) {
	    if (pfx_err_posn) {
		*offset = pfx_err_posn - string;
	    }
	}
	return(FALSE);
    }
}
#endif

/*
 * Get a masked NSAP from an input string.  Returns FALSE if the string is
 * not a valid masked NSAP.
 */

#ifdef NOBODY_CALLS_ME
boolean get_masked_nsap (
    char *string,
    masked_nsap *mskdnsap,
    int *offset)
{
    parse_nsap_string(string);
    if (offset)
	*offset = endptr - string;
    if (is_masked_nsap()) {
	return(TRUE);
    } else {
	return(FALSE);
    }
}
#endif

/*
 * Get an NSAP structure given an input string.  We return the most
 * conservative version possible;  first an NSAP, then an NSAP prefix,
 * and finally a masked NSAP.  Returns:
 *
 *   NSAP_STRUCT_ADDR if it's an NSAP address
 *   NSAP_STRUCT_PFX if it's an NSAP prefix
 *   NSAP_STRUCT_MASKED if it's a masked NSAP
 *   NSAP_STRUCT_BAD if it doesn't parse correctly
 *   NSAP_STRUCT_NOMEM if memory isn't available
 *
 * The "require_dots" parameter is set if a trailing "..." is required
 * for a string to be interpreted as an NSAP prefix.  If it is set, the
 * string "47.0005.1234.5" will be returned as an exact-length masked
 * NSAP;  if it is false, that string would be returned as a prefix.
 *
 * Memory will be malloced for the structure and returned in *value.
 * The caller should cast this appropriately according to the return
 * value, and is responsible for freeing the structure.
 */

int get_nsap_structure (
    char *string,
    void **value,
    int *offset,
    boolean require_dots)
{
    parse_nsap_string(string);
    if (offset)
	*offset = endptr - string;

    if (is_nsap()) {
	*value = malloc(sizeof(nsap_addr));
	if (*value == NULL) {
	nomem:
	    return(NSAP_STRUCT_NOMEM);
	}
	extract_nsap((nsap_addr *) *value);
	return(NSAP_STRUCT_ADDR);
    } else if (is_nsap_pfx(require_dots)) {
	*value = malloc(sizeof(nsap_pfx));
	if (*value == NULL) goto nomem;
	extract_nsap_pfx((nsap_pfx *) *value);
	return(NSAP_STRUCT_PFX);
    } else if (is_masked_nsap()) {
	*value = malloc(sizeof(masked_nsap));
	if (*value == NULL) goto nomem;
	extract_masked_nsap((masked_nsap *) *value);
	return(NSAP_STRUCT_MASKED);
    } else {
	return(NSAP_STRUCT_BAD);
    }
}

