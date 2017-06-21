/* $Id: dinstr.c,v 3.1.58.1 1996/03/18 18:34:49 gstovall Exp $
 * $Source: /release/111/cvs/Xboot/src-68-c7000/dinstr.c,v $
 *------------------------------------------------------------------
 * dinstr.c -- decode instructions in 68k format
 *
 * February 1987, Greg Satz
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dinstr.c,v $
 * Revision 3.1.58.1  1996/03/18  18:34:49  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.26.1  1996/03/16  06:10:32  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.1  1995/11/09  09:10:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:34:19  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.1  1993/01/30  01:11:45  whp4
 * Add RP1 support.
 *
 *
 * $Endlog$
 */

#include "../defs.h"
#include "../stdio.h"
#include "../reason.h"
#include "../extern.h"
#include "opcodes.h"


u_char *print_insn_arg();
u_char *print_indexed();
void print_base();
int fetch_arg();
void print_address();


/* Number of elements in the opcode table.  */
#define NOPCODES (sizeof m68k_opcodes / sizeof m68k_opcodes[0])

char *reg_names[] = 
 {"d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7",
  "a0", "a1", "a2", "a3", "a4", "a5", "fp", "sp",
  "ps", "pc",
  "fp0", "fp1", "fp2", "fp3", "fp4", "fp5", "fp6", "fp7",
  "fpcontrol", "fpstatus", "fpiaddr", "fpcode", "fpflags"};

char *fpcr_names[] = { "", "fpiar", "fpsr", "fpiar/fpsr", "fpcr",
		       "fpiar/fpcr", "fpsr/fpcr", "fpiar-fpcr"};

struct { char *name; int value; } magic_names[] =
 {{"sfc", 0x000}, {"dfc", 0x001}, {"cacr", 0x002},
 {"usp", 0x800}, {"vbr", 0x801}, {"caar", 0x802},
 {"msp", 0x803}, {"isp", 0x804}};

char *scales[] = {"", "*2", "*4", "*8"};

#define NEXTBYTE(p)  (p += 2, ((char *)p)[-1])

#define NEXTWORD(p)  \
  (p += 2, ((((char *)p)[-2]) << 8) + p[-1])

#define NEXTLONG(p)  \
  (p += 4, (((((p[-4] << 8) + p[-3]) << 8) + p[-2]) << 8) + p[-1])

#define NEXTSINGLE(p) \
  (p += 4, *((float *)(p - 4)))

#define NEXTDOUBLE(p) \
  (p += 8, *((/*double*/float *)(p - 8)))  /* don't use double for now */

#define NEXTEXTEND(p) \
  (p += 12, 0.0)	/* Need a function to convert from extended to double
			   precision... */

#define NEXTPACKED(p) \
  (p += 12, 0.0)	/* Need a function to convert from packed to double
			   precision.   Actually, it's easier to print a
			   packed number than a double anyway, so maybe
			   there should be a special case to handle this... */

/*
 * Print the m68k instruction at address MEMADDR in debugged memory
 */

void dinstr (memaddr)
    long memaddr;
{
    u_char *buffer;
    register int i;
    register u_char *p;
    register char *d;
    register int bestmask;
    int best;
    
    printf("%x: ", memaddr);
    buffer = (u_char *)memaddr;
    
    bestmask = 0;
    best = -1;
    for (i = 0; i < NOPCODES; i++) {
	register uint opcode = m68k_opcodes[i].opcode;
	register uint match = m68k_opcodes[i].match;
	if (((0xff & buffer[0] &
	      (match >> 24)) == (0xff & (opcode >> 24))) &&
	    ((0xff & buffer[1] &
	      (match >> 16)) == (0xff & (opcode >> 16))) &&
	    ((0xff & buffer[2] &
	      (match >> 8)) == (0xff & (opcode >> 8))) &&
	    ((0xff & buffer[3] & match) == (0xff & opcode))) {
	    
	    /*
	     * Don't use for printout the variants of divul and divsl
	     * that have the same register number in two places.
	     * The more general variants will match instead.
	     */
	    for (d = m68k_opcodes[i].args; *d; d += 2)
		if (d[1] == 'D')
		    break;
	    
	    /*
	     * Don't use for printout the variants of most floating
	     * point coprocessor instructions which use the same
	     * register number in two places, as above.
	     */
	    if (*d == 0)
		for (d = m68k_opcodes[i].args; *d; d += 2)
		    if (d[1] == 't')
			break;
	    
	    if (*d == 0 && match > bestmask) {
		best = i;
		bestmask = match;
	    }
	}
    }
    
    /* Handle undefined instructions.  */
    if (best < 0) {
	printf ("%#x", (buffer[0] << 8) + buffer[1]);
	instrsize = 2;
    } else {
	
	printf("%s", m68k_opcodes[best].name);
	
	/* Point at first word of argument data,
	   and at descriptor for first argument.  */
	p = buffer + 2;
	
	/* Why do this this way? -MelloN */
	for (d = m68k_opcodes[best].args; *d; d += 2) {
	    if (d[0] == '#') {
		if (d[1] == 'l' && p - buffer < 6)
		    p = buffer + 6;
		else if (p - buffer < 4 && d[1] != 'C' && d[1] != '8' )
		    p = buffer + 4;
	    }
	    if (d[1] >= '1' && d[1] <= '3' && p - buffer < 4)
		p = buffer + 4;
	    if (d[1] >= '4' && d[1] <= '6' && p - buffer < 6)
		p = buffer + 6;
	}
	
	d = m68k_opcodes[best].args;
	
	if (*d)
	    putchar (' ');
	
	while (*d) {
	    p = print_insn_arg (d, buffer, p, memaddr + p - buffer);
	    d += 2;
	    if (*d && *(d - 2) != 'I' && *d != 'k')
		putchar (',');
	}
	instrsize = p - buffer;
    }
}



u_char *print_insn_arg (d, buffer, p, addr)
    char *d;
    u_char *buffer;
    register u_char *p;
    uint addr;		/* PC for this arg to be relative to */
{
    register int val;
    register int place = d[1];
    int regno;
    register char *regname;
    register u_char *p1;
    register double flval;
    int flt_p;
    
    val = 0;
    switch (*d) {
      case 'C':
	puts("ccr");
	break;
	
      case 'S':
	puts("sr");
	break;
	
      case 'U':
	puts("usp");
	break;
	
      case 'J':
	val = fetch_arg (buffer, place, 12);
	for (regno = sizeof magic_names / sizeof magic_names[0] - 1;
	     regno >= 0; regno--)
	    if (magic_names[regno].value == val) {
		puts(magic_names[regno].name);
		break;
	    }
	if (regno < 0)
	    printf("%d", val);
	break;
	
      case 'Q':
	val = fetch_arg (buffer, place, 3);
	if (val == 0) val = 8;
	printf("#%d", val);
	break;
	
      case 'M':
	val = fetch_arg (buffer, place, 8);
	if (val & 0x80)
	    val = val - 0x100;
	printf("#%d", val);
	break;
	
      case 'T':
	val = fetch_arg (buffer, place, 4);
	printf("#%d", val);
	break;
	
      case 'D':
	printf("%s", reg_names[fetch_arg (buffer, place, 3)]);
	break;
	
      case 'A':
	printf("%s", reg_names[fetch_arg (buffer, place, 3) + 010]);
	break;
	
      case 'R':
	printf("%s", reg_names[fetch_arg (buffer, place, 4)]);
	break;
	
      case 'F':
	printf("fp%d", fetch_arg (buffer, place, 3));
	break;
	
      case 'O':
	val = fetch_arg (buffer, place, 6);
	if (val & 0x20)
	    printf("%s", reg_names [val & 7]);
	else
	    printf("%d", val);
	break;
	
      case '+':
	printf("(%s)+", reg_names[fetch_arg (buffer, place, 3) + 8]);
	break;
	
      case '-':
	printf("-(%s)", reg_names[fetch_arg (buffer, place, 3) + 8]);
	break;
	
      case 'k':
	if (place == 'k')
	    printf("{%s}", reg_names[fetch_arg (buffer, place, 3)]);
	else if (place == 'C') {
	    val = fetch_arg (buffer, place, 7);
	    if ( val > 63 )		/* This is a signed constant. */
		val -= 128;
	    printf("{#%d}", val);
	}
	else
	    printf ("Invalid arg format in opcode table: \"%c%c\".",
		    *d, place);
	break;
	
      case '#':
	p1 = buffer + 2;
	if (place == 's')
	    val = fetch_arg (buffer, place, 4);
	else if (place == 'C')
	    val = fetch_arg (buffer, place, 7);
	else if (place == '8')
	    val = fetch_arg (buffer, place, 3);
	else if (place == '3')
	    val = fetch_arg (buffer, place, 8);
	else if (place == 'b')
	    val = NEXTBYTE (p1);
	else if (place == 'w')
	    val = NEXTWORD (p1);
	else if (place == 'l')
	    val = NEXTLONG (p1);
	else
	    printf ("Invalid arg format in opcode table: \"%c%c\".",
		    *d, place);
	printf("#%d", val);
	break;
	
      case '^':
	if (place == 's')
	    val = fetch_arg (buffer, place, 4);
	else if (place == 'C')
	    val = fetch_arg (buffer, place, 7);
	else if (place == '8')
	    val = fetch_arg (buffer, place, 3);
	else if (place == 'b')
	    val = NEXTBYTE (p);
	else if (place == 'w')
	    val = NEXTWORD (p);
	else if (place == 'l')
	    val = NEXTLONG (p);
	else
	    printf ("Invalid arg format in opcode table: \"%c%c\".",
		    *d, place);
	printf("#%d", val);
	break;
	
      case 'B':
	if (place == 'b')
	    val = NEXTBYTE (p);
	else if (place == 'w')
	    val = NEXTWORD (p);
	else if (place == 'l')
	    val = NEXTLONG (p);
	else if (place == 'g') {
	    val = ((char *)buffer)[1];
	    if (val == 0)
		val = NEXTWORD (p);
	    else if (val == -1)
		val = NEXTLONG (p);
	}
	else if (place == 'c') {
	    if (buffer[1] & 0x40) /* If bit six is one, long offset */
		val = NEXTLONG (p);
	    else
		val = NEXTWORD (p);
	}
	else
	    printf ("Invalid arg format in opcode table: \"%c%c\".",
		    *d, place);
	
	print_address (addr + val);
	break;
	
      case 'd':
	val = NEXTWORD (p);
	printf("%d(%s)", val, fetch_arg (buffer, place, 3));
	break;
	
      case 's':
	printf("%s", fpcr_names[fetch_arg (buffer, place, 3)]);
	break;
	
      case 'I':
	val = fetch_arg (buffer, 'd', 3); /* Get coprocessor ID... */
	if (val != 1)		/* Unusual coprocessor ID? */
	    printf("(cpid=%d) ", val);
	if (place == 'i')
	    p += 2;		/* Skip coprocessor extended operands */
	break;
	
      case '*':
      case '~':
      case '%':
      case ';':
      case '@':
      case '!':
      case '$':
      case '?':
      case '/':
      case '&':
	
	if (place == 'd') {
	    val = fetch_arg (buffer, 'x', 6);
	    val = ((val & 7) << 3) + ((val >> 3) & 7);
	} else
	    val = fetch_arg (buffer, 's', 6);
	
	/* Get register number assuming address register.  */
	regno = (val & 7) + 8;
	regname = reg_names[regno];
	switch (val >> 3) {
	  case 0:
	    printf("%s", reg_names[val]);
	    break;
	    
	  case 1:
	    printf("%s", regname);
	    break;
	    
	  case 2:
	    printf("(%s)", regname);
	    break;
	    
	  case 3:
	    printf("(%s)+", regname);
	    break;
	    
	  case 4:
	    printf("-(%s)", regname);
	    break;
	    
	  case 5:
	    val = NEXTWORD (p);
	    printf("%d(%s)", val, regname);
	    break;
	    
	  case 6:
	    p = print_indexed (regno, p, addr);
	    break;
	    
	  case 7:
	    switch (val & 7) {
	      case 0:
		val = NEXTWORD (p);
		puts("@#");
		print_address (val);
		break;
		
	      case 1:
		val = NEXTLONG (p);
		puts("@#");
		print_address (val);
		break;
		
	      case 2:
		val = NEXTWORD (p);
		print_address (addr + val);
		break;
		
	      case 3:
		p = print_indexed (-1, p, addr);
		break;
		
	      case 4:
		flt_p = 1;	/* Assume it's a float... */
		switch( place ) {
		  case 'b':
		    val = NEXTBYTE (p);
		    flt_p = 0;
		    break;
		    
		  case 'w':
		    val = NEXTWORD (p);
		    flt_p = 0;
		    break;
		    
		  case 'l':
		    val = NEXTLONG (p);
		    flt_p = 0;
		    break;
		    
		  case 'f':
		    flval = NEXTSINGLE(p);
		    break;
		    
		  case 'F':
		    flval = NEXTDOUBLE(p);
		    break;
		    
		  case 'x':
		    flval = NEXTEXTEND(p);
		    break;
		    
		  case 'p':
		    flval = NEXTPACKED(p);
		    break;
		    
		  default:
		    printf ("Invalid arg format in opcode table: \"%c%c\".",
			    *d, place);
		}
#ifdef notdef
		if ( flt_p )	/* Print a float? */
		    printf("#%g", flval);
		else
#endif
		    printf("#%d", val);
		break;
		
	      default:
		printf("<invalid address mode %#x>", val);
	    }
	}
	break;
	
      default:
	printf ("Invalid arg format in opcode table: \"%c\".", *d);
    }
    
#ifdef	lint
    flval = flval;
    flt_p = flt_p;
#endif
    
    return (u_char *) p;
}



/* Fetch BITS bits from a position in the instruction specified by CODE.
   CODE is a "place to put an argument", or 'x' for a destination
   that is a general address (mode and register).
   BUFFER contains the instruction.  */

int fetch_arg (buffer, code, bits)
    u_char *buffer;
    char code;
    int bits;
{
    register int val;

    val = 0;
    switch (code) {
      case 's':
	val = buffer[1];
	break;
	
      case 'd':			/* Destination, for register or quick.  */
	val = (buffer[0] << 8) + buffer[1];
	val >>= 9;
	break;
	
      case 'x':			/* Destination, for general arg */
	val = (buffer[0] << 8) + buffer[1];
	val >>= 6;
	break;
	
      case 'k':
	val = (buffer[3] >> 4);
	break;
	
      case 'C':
	val = buffer[3];
	break;
	
      case '1':
	val = (buffer[2] << 8) + buffer[3];
	val >>= 12;
	break;
	
      case '2':
	val = (buffer[2] << 8) + buffer[3];
	val >>= 6;
	break;
	
      case '3':
      case 'j':
	val = (buffer[2] << 8) + buffer[3];
	break;
	
      case '4':
	val = (buffer[4] << 8) + buffer[5];
	val >>= 12;
	break;
	
      case '5':
	val = (buffer[4] << 8) + buffer[5];
	val >>= 6;
	break;
	
      case '6':
	val = (buffer[4] << 8) + buffer[5];
	break;
	
      case '7':
	val = (buffer[2] << 8) + buffer[3];
	val >>= 7;
	break;
	
      case '8':
	val = (buffer[2] << 8) + buffer[3];
	val >>= 10;
	break;
    }
    
    switch (bits) {
      case 3:
	return val & 7;
      case 4:
	return val & 017;
      case 5:
	return val & 037;
      case 6:
	return val & 077;
      case 7:
	return val & 0177;
      case 8:
	return val & 0377;
      case 12:
	return val & 07777;
    }
    return 0;
}



/*
 * print_indexed:
 * Print an indexed argument.  The base register is BASEREG (-1 for pc).
 * P points to the address extension word, in buffer.
 * ADDR is the nominal core address of the address extension word.
 */

u_char *print_indexed (basereg, p, addr)
    int basereg;
    u_char *p;
    uint addr;
{
    register int word;
    register int base_disp;
    register int outer_disp;
    char index_string[40];
    
    /*
     * Get the address extension word:
     */
    word = NEXTWORD (p);

    /*
     * Generate the text for the index register (Where this will be
     * output is not yet determined):
     */
    sprintf (index_string, "[%s.%c%s]",
	     (reg_names[(word >> 12) & 0xf]),
	     ((word & 0x0800) ? 'l' : 'w'),
	     (scales[(word >> 9) & 3]));

    /*
     * Handle the 68020 Brief Format address extension word:
     */
    if ((word & 0x0100) == 0) {
	print_base (basereg,
		    (((word & 0x80) ? word | 0xff00 : word & 0xff)
		     + ((basereg == -1) ? addr : 0)));
	puts(index_string);
	return p;
    }
    
    /*
     * Handle the 68020 Full Format address extension word:
     */
    if (word & 0x0080)		/* Is base suppress set? */
	basereg = -2;		/* Yes, suppress base register */
    if (word & 0x0040)		/* Is index suppress set? */
	index_string[0] = 0;	/* Yes, suppress index string */

    /*
     * Compute the base register displacement:
     */
    base_disp = 0;
    switch ((word >> 4) & 3) {	/* Base displacement size */
      case 2:
	base_disp = NEXTWORD (p); /* Word displacement */
	break;
      case 3:
	base_disp = NEXTLONG (p); /* Long displacement */
    }
    if (basereg == -1)
	base_disp += addr;
    
    /*
     * Handle no indirection case:
     */
    if ((word & 7) == 0) {	/* I/IS field */
	print_base (basereg, base_disp);
	puts(index_string);
	return p;
    }
    
    /*
     * Compute outer displacement (add after indirection):
     */
    outer_disp = 0;
    switch (word & 3) {
      case 2:
	outer_disp = NEXTWORD (p); /* Word displacement */
	break;
      case 3:
	outer_disp = NEXTLONG (p); /* Long displacement */
    }
    
    printf("%d(", outer_disp);
    print_base (basereg, base_disp);
    
    /*
     * If postindexed, print the closeparen before the index:
     */
    if (word & 4)
	printf(")%s", index_string);

    /*
     * Else preindexed, print the closeparen after the index:
     */
    else
	printf("%s)", index_string);

    return p;
}

/*
 * Print a base register REGNO and displacement DISP, on output.
 * REGNO = -1 for pc, -2 for none (suppressed):
 */

void print_base (regno, disp)
    int regno;
    int disp;
{
    if (regno == -2)
	printf("%d", disp);
    else if (regno == -1)
	printf("%#x", disp);
    else
	printf("%d(%s)", disp, reg_names[regno]);
}



/* Print address ADDR symbolically on output.
   First print it as a number.  Then perhaps print
   <SYMBOL + OFFSET> after the number. (maybe someday) */

void print_address (addr)
    uint addr;
{
    printf("%#x", addr);
}
