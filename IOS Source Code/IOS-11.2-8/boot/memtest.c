/* $Id: memtest.c,v 3.2.58.1 1996/06/01 01:10:37 clev Exp $
 * $Source: /release/112/cvs/Xboot/memtest.c,v $
 *------------------------------------------------------------------
 * memtest.c -- memory and bus diagnostic routines
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: memtest.c,v $
 * Revision 3.2.58.1  1996/06/01  01:10:37  clev
 * CSCdi55171:  32m on 4000m breaks uncompress in the monitor
 * Branch: California_branch
 *
 * Add code to handle the discontiguous memory on a 32m 4000m.
 *
 * Revision 3.2  1995/11/17  07:41:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "defs.h"
#include "stdio.h"
#include "ctype.h"
#include "m68vectors.h"
#include "reason.h"
#include "extern.h"
#include "test.h"
#include "flh.h"
#include "flh_log.h"

#define	DELAY		250	/* ms to wait during refresh test */
#define MAXTESTS	18	/* One more than the maximum test number */
#define SIXTEENMEG      0x1000000

#define SHORT(a)	(a & 0xFFFF)
#define CHAR(a)		(a & 0xFF)

#define SHORT_ALIGN(a)	((u_short *)(((int)a) & 0xFFFFFFFE))
#define LONG_ALIGN(a)	((u_int *)(((int)a) & 0xFFFFFFFC))

union addr_ptrs {
    volatile u_char	*cp;	/* Char current address pointer */
    volatile ushort	*sp;	/* Short current address pointer */
    volatile uint	*ip;	/* Integer current address pointer */
} tp;

short testno;			/* current test number */
int seed;			/* default seed for random data tests */
short noiselevel;		/* message controls: 0 = final summary
				 * 1 = end pass only
				 * 2 = normal */
volatile u_short *trigger;	/* Read this address upon failure. Very
				   useful for hardware debugging. */

void buserring();
#ifdef notdef
long multibusmemory();
#endif

memtest()
{
    extern int reset_io();
    short selecttest[MAXTESTS];
    int curerrs, toterrs;	/* error counters */
    u_char *base, *top;
    ushort testpattern;
    register int i;
#ifdef notdef
    long l4_save;		/* where to save current level 4 vector */
#endif
    long location, passes;
    
    reset_io();			/* Reset all DMA devices so they stop */
    printf("Memory/Bus diagnostic\n");

#ifdef DCACHE
    dcacheoff();		/* Turn off data cache */
#endif DCACHE

    while (TRUE) {
	base = (u_char *)INITSTACKP;
	printf("\nStarting Address [%#x]? ", base);
	gets(bufu.buf);
	if ( tolower(bufu.buf[1]) == 'x' ) 
	    bufu.buf[0] = bufu.buf[1] = '0';
	if ((location = gethexnum(bufu.buf)) >= 0) {
	    base = (u_char *)location;
	}
	if (base < (u_char *) INITSTACKP) { 
	    printf("Base address must be >= 0x%x\n",INITSTACKP);
	    printf("Starting Address set to 0x%x\n",INITSTACKP);
	    base = (u_char *)INITSTACKP;
	}


	top = (u_char *)memsize;
#if defined(PAN) && !defined(XX)
	top = (u_char *)start_flh;
#endif /* defined(PAN) && !defined(XX) */
#if defined(XX)
	/*
	** The 4000M can have 32mb of main memory but it is non-contiguous.
	** The user must test the memory in 2 chunks.
	*/
	if (base < (u_char *)ADRSPC_PROM && top > (u_char *)ADRSPC_PROM) {
	    top = (u_char *)ADRSPC_PROM;
	} else if(base >= (u_char *)ADRSPC_EXP_RAM && top > (u_char *)ADRSPC_PROM) {
	    top = (u_char *)ADRSPC_SHAREDMEM;
	}

#endif  /* defined(XX) */

	printf("Ending Address [%#x]? ", top);
	gets(bufu.buf);
	if ( tolower(bufu.buf[1]) == 'x' ) 
	    bufu.buf[0] = bufu.buf[1] = '0'; 
	if ((location = gethexnum(bufu.buf)) >= 0)
	    top = (u_char *)location;
	if (top <= base) {
	    printf("Invalid test range 0x%x to 0x%x\n",base,top);
	    continue;
	}
#if defined(XX)
	/*
	** The 4000M can have 32mb of main memory but it is non-contiguous.
	** The user must test the memory in 2 chunks.
	*/
	if (memsize > SIXTEENMEG &&
	    (base < (u_char *)ADRSPC_PROM && top > (u_char *)ADRSPC_PROM) ||
	    (base >= (u_char *)ADRSPC_EXP_RAM && top > (u_char *)ADRSPC_SHAREDMEM)) {
	    printf("32mb of memory is non-contiguous and must be tested in 2 pieces\n");
	    continue;
	}
#endif  /* defined(XX) */
#if defined(PAN) && !defined(XX)
	if (top > (u_char *)start_flh &&
	   base < (u_char *)((long)end_flh + sizeof(struct _flhlog_end)) ){
	    printf("Address range can not write over flh buffer 0x%x to 0x%x\n",
		   start_flh, ((long)end_flh + sizeof(struct _flhlog_end)));
	    continue;
	}

#endif /* defined(PAN) && !defined(XX) */
	break;
    }

    testpattern = 0xFFFF;
    printf("Hex argument for variable tests [%#x]? ", testpattern);
    gets(bufu.buf);
    if ((location = gethexnum(bufu.buf)) >= 0)
	testpattern = (ushort) location;

    for (i = 0; i < MAXTESTS; i++)
	selecttest[i] = TRUE;
    printf("Select Tests [all]? ");
    gets(bufu.buf);
    while(bufu.buf[0] != '\0') {
	switch(bufu.buf[0]) {
	  case 'H':
	  case '?':
	    printf("\nC CLEAR selection (deselect all tests)\n");
	    printf("A select ALL tests\n");
	    printf("Q QUIT to command level\n");
	    printf("P PRINT selection\n");
	    printf("n add test n\n\n");
	    printf("Test Description\n");
	    printf("0  Readonly\n");
	    printf("1  writeonly spec data\n");
	    printf("2  memory bus size test\n");
	    printf("3  constant 0\n");
	    printf("4  constant 0xFFFF\n");
	    printf("5  constant 0xAAAA\n");
	    printf("6  constant 0xC11C\n");
	    printf("7  constant 0x5555\n");
	    printf("8  constant spec data\n");
	    printf("9  random short data\n");
	    printf("10 address pattern\n");
	    printf("11 write-delay-read short\n");
	    printf("12 write-delay-read byte\n");
	    printf("13 random byte data\n");
	    printf("14 bit float test\n");
	    printf("15 march test with 0xAAAA and 0x5555\n");
	    printf("16 march test with 0xC11C and 0x8F8F\n");
	    printf("17 random int data\n");
	    break;
	  case 'C':
	    for (i = 0; i < MAXTESTS; i++)
		selecttest[i] = FALSE;
	    break;
	  case 'A':
	    for (i = 0; i < MAXTESTS; i++)
		selecttest[i] = TRUE;
	    break;
	  case 'Q':
#ifdef DCACHE
	    dcacheon();
#endif
	    return;
	    break;
	  case 'P':
	    printf("Tests Selected: ");
	    for (i = 0; i < MAXTESTS; i++)
		if (selecttest[i])
		    printf("%d ",i);
	    printf("\n");
	    break;
	    
	  default:
	    if ((location = getnum(bufu.buf)) >= 0) {
		if (location <= MAXTESTS )
		    selecttest[location] = TRUE;
		else
		    printf("Illegal test %d.\n",location);
	    }
	    else printf("Type H for help.\n");
	    break;
	}
	printf("Select? ");
	gets(bufu.buf);
    }

    passes = PASSES;
    printf("Number of passes to run [%d]? ", passes);
    gets(bufu.buf);
    if ((location = getnum(bufu.buf)) > 0)
	passes = location;

    trigger = (u_short *)0;
    printf("Trigger word for hardare debugging [%d]? ", trigger);
    gets(bufu.buf);
    if ((location = getnum(bufu.buf)) > 0)
	trigger = (u_short *)location;

    noiselevel = 2;
    printf("Message Level (0=silence, 1=summary, 2=normal)[%d]? ",
	   noiselevel);
    gets(bufu.buf);
    if ((location = getnum(bufu.buf)) >= 0)
	noiselevel = location;

    abortstate = ABORT_INTERNAL;
    buserrcnt = 0;
    buserrhandler = buserring;
    toterrs = 0;
#ifdef notdef
    l4_save = *EVEC_LEVEL4;
    *EVEC_LEVEL4 = multibusmemory;
#endif

    printf("\nTesting addresses between %#x and %#x\n", base, top);
    for (i = 0; i < passes && !abort; i++) {
	if (noiselevel >= 1)
	    printf("\nBegin pass %d, test ", i);
	curerrs = 0;

	/*
	 * Readonly Test:
	 */
	testno = 0;
	if (selecttest[testno])
	    curerrs += test_read_only(base, top);

	/*
	 * Writeonly Special Pattern Test:
	 */
	testno = 1;
	if (selecttest[testno])
	    curerrs += test_write_only(base, top, testpattern);

	testno = 2;
	if (selecttest[testno])
	    curerrs += test_bus_size(base, top);

	/*
	 * Write/Read 0x0 Test:
	 */
	testno = 3;
	if (selecttest[testno])
	    curerrs += test_short(base, top, 0);

	/*
	 * Write/Read 0xFFFF Test:
	 */
	testno = 4;
	if (selecttest[testno])
	    curerrs += test_short(base, top, -1);

	/*
	 * Write/Read 0xAAAA Test:
	 */
	testno = 5;
	if (selecttest[testno])
	    curerrs += test_short(base, top, 0xAAAA);

	/*
	 * Write/Read 0xC11C Test:
	 */
	testno = 6;
	if (selecttest[testno])
	    curerrs += test_short(base, top, 0xC11C);

	/*
	 * Write/Read 0x5555 Test:
	 */
	testno = 7;
	if (selecttest[testno])
	    curerrs += test_short(base, top, 0x5555);

	/*
	 * Write/Read Special Pattern Test:
	 */
	testno = 8;
	if (selecttest[testno])
	    curerrs += test_short(base, top, testpattern);

	/*
	 * Write/Read Psuedo-random short test pattern:
	 */
	testno = 9;
	if (selecttest[testno])
	    curerrs += random_short(base, top);

	/*
	 * Write/Read address+1 into each short:
	 */
	testno = 10;
	if (selecttest[testno])
	    curerrs += test_addr(base, top);

	/*
	 * Write/Delay/Read short test for refresh:
	 */
	testno = 11;
	if (selecttest[testno])
	    curerrs += test_refresh_short(base, top);

	/*
	 * Write/Delay/Read byte test for refresh:
	 */
	testno = 12;
	if (selecttest[testno])
	    curerrs += test_refresh_byte(base, top);

	/*
	 * Write/Read Psuedo-random byte test pattern:
	 */
	testno = 13;
	if (selecttest[testno])
	    curerrs += random_byte(base, top);

	/*
	 * Bit Float Test:
	 */
	testno = 14;
	if (selecttest[testno])
	    curerrs += bit_float(base, top);

	/*
	 * March Test with patterns that verify that each bit of each memory
	 * cell can retain a zero and a one:
	 */
	testno = 15;
	if (selecttest[testno])
	    curerrs += march_test(base, top, (u_short)0xAAAA, (u_short)0x5555);

	/*
	 * March Test again with patterns that have different parity values
	 * to indirectly test the parity RAMs:
	 */
	testno = 16;
	if (selecttest[testno])
	    curerrs += march_test(base, top, (u_short)0xC11C, (u_short)0x8F8F);

	testno = 17;
	if (selecttest[testno])
	    curerrs += random_int(base, top);

	toterrs += curerrs;
	if (noiselevel >= 1){
	    printf("  End pass ");
	    if (curerrs)
		printf("  Errors this pass: %d", curerrs);
	    if (toterrs)
		printf("  Total errors: %d", toterrs);
	}
    }

    if (toterrs)
	printf("\n%d total errors", toterrs);
    else
	printf("\nNo errors");
    printf(" during %d passes\n", i);

#ifdef notdef
    *EVEC_LEVEL4 = l4_save;
#endif
    abortstate = ABORT_IGNORE;
    abort = FALSE;
    buserrhandler = 0;
#ifdef DCACHE
    dcacheon();
#endif
}

/*
 * Bus Error handler:
 */

void buserring()
{
#if defined(MC68020) || defined(MC68040)
    extern print_control();
    register u_short control;
    register char processor_version;

    control = *((volatile u_short *)ADRSPC_CONTROL);
#endif

    if ((noiselevel != 0) && (buserrcnt++ > MAXBUSERRORS))
	abort = TRUE;

    if (noiselevel >= 2){
	printf("\nBus Error");
#if defined(MC68020) || defined(MC68040)
	print_control(control);
#endif
#ifdef CSC2
        processor_version = sys_status_reg->version;
        if (processor_version == VER_CSC4)
           printf("at address %#x in test number %d\n while attempting %s at address: %#x\n",
                  ERROR_ADDRESS_040, testno, function, tp.cp);
        else
           printf("at address %#x in test number %d\n while attempting %s at address: %#x\n",
                  ERROR_ADDRESS, testno, function, tp.cp);
#else
           printf("at address %#x in test number %d\n while attempting %s at address: %#x\n",
                  ERROR_ADDRESS, testno, function, tp.cp);
#endif

#ifdef MC68000
	printf("    R/W flag: %#x, I/N flag: %#x, Instruction register: %#x\n",
	       error.rw, error.instr, error.instrreg);
#else
#ifdef MC68010
	printf("    SSW: %#x, Outbuf: %#x, Inbuf: %#x, Insbuf: %#x\n",
	       error->stack.ssw, error->stack.outbuf,
	       error->stack.inbuf, error->stack.insbuf);
#else
#ifdef MC68020
# ifdef CSC2
        processor_version = sys_status_reg->version;
        if (processor_version == VER_CSC4)
           printf("    SSW: %#x, EA: %#x\n",
                  error->stack.type7.ssw,
                  error->stack.type7.ea);
        else
           printf("    SSW: %#x, B pipe: %#x, C pipe: %#x\n",
                  error->stack.typea.ssw,
                  error->stack.typea.ipsb,
                  error->stack.typea.ipsc);
# else
           printf("    SSW: %#x, B pipe: %#x, C pipe: %#x\n",
                  error->stack.typea.ssw,
                  error->stack.typea.ipsb,
                  error->stack.typea.ipsc);
# endif
#else
#ifdef MC68300
	printf("    SSW: %#x, Return PC: %#x, Fault addr: %#x, Dbuf: %#x,\nm"
	       "Current PC: %#x, Int xfr: %#x\n",
	       error->sr,
	       error->pc,
	       error->faddr,
	       error->info.t123.dbuf,
	       error->info.t123.cpc,
	       error->ixfr);
#else
#ifdef MC68040
	   printf("    SSW: %#x, EA: %#x\n",
                  error->stack.type7.ssw,
                  error->stack.type7.ea);
#else
#error "No fault decoder"
#endif MC68040
#endif MC68300
#endif MC68020
#endif MC68010
#endif MC68000
    }
}



#ifdef notdef
/*
 * Here on a Level 4 interrupt
 * Theoretically this is a Multibus Memory interrupt?
 */
multibusmemory()
{
    printf("\nMultiBus memory parity error at location %#x, bank %d\n",
	   tp.cp, (~*(u_char *)IOPORT)&07);
    *IOPORT = 1;
}
#endif

/*
 * Read the specified address range:
 */

test_read_only(a, b)
    register ushort *a, *b;
{
    int ierrcnt = buserrcnt;
    volatile ushort t;
    
    if (abort)
	return(0);
    if (noiselevel >= 1) {
	putnum(testno);
	putchar(' ');
    }

    b = SHORT_ALIGN(b);
    function = "short read";
    for (tp.sp = a; (tp.sp < b) && (!abort); tp.sp++)
	t = *tp.sp;

#ifdef	lint
    t = t;
#endif
    
    return(buserrcnt - ierrcnt);
}



/*
 * Write the specified address range:
 */

test_write_only(a, b, n)
    register ushort *a, *b, n;
{
    int ierrcnt = buserrcnt;
    
    if (abort)
	return(0);
    if (noiselevel >= 1) {
	putnum(testno);
	putchar(' ');
    }

    b = SHORT_ALIGN(b);
    function = "short write";
    for (tp.sp = a; (tp.sp < b) && (!abort); tp.sp++)
	*tp.sp = n;

    return(buserrcnt - ierrcnt);
}

/*
 * Test the memory bus size signals every 1024 locations:
 *
 * Algorithm:
 * a) Write a long integer value.
 *	i)  Verify by reading it as bytes.
 *	ii) Verify by reading it as shorts.
 * b) Write two consecutive short values.
 *	i)  Verify by reading it as a long.
 *	ii) Verify by reading it as characters.
 * c) Write four consecutive character values.
 *	i)  Verify by reading as two short values.
 *	ii) Verify by reading as one long value.
 */

test_bus_size(start, end)
    register uint *start, *end;
{
    int errors = 0;
    charlong *ptr;
    uint integer;
    ushort word;
    uchar byte;

    if (abort)
	return(0);
    if (noiselevel >= 1) {
	putnum(testno);
	putchar(' ');
    }
    end = LONG_ALIGN(end);

    for (tp.ip = start; (tp.ip < end) && (!abort); tp.cp += 1024) {
	ptr = (charlong *)tp.cp; /* Structure pointer */

	/*
	 * Write a long value, verify that it reads correctly as bytes,
	 * then verify that it reads correctly as shorts:
	 */
	asm("	.globl tbs_write_int");
	asm("tbs_write_int:");
	function = "integer write";
	ptr->d.lword = 0x01234567; /* Write long value */

	function = "byte read ";
	byte = ptr->d.byte[0];	 /* Read as bytes and verify */
	if (byte != 0x01) {
	    errors++;
	    if (noiselevel >= 2)
		report("bus size byte", 0x01, byte, &ptr->d.byte[0]);
	}
	byte = ptr->d.byte[1];
	if (byte != 0x23) {
	    errors++;
	    if (noiselevel >= 2)
		report("bus size byte", 0x23, byte, &ptr->d.byte[1]);
	}
	byte = ptr->d.byte[2];
	if (ptr->d.byte[2] != 0x45) {
	    errors++;
	    if (noiselevel >= 2)
		report("bus size byte", 0x45, byte, &ptr->d.byte[2]);
	}
	byte = ptr->d.byte[3];
	if (byte != 0x67) {
	    errors++;
	    if (noiselevel >= 2)
		report("bus size byte", 0x67, byte, &ptr->d.byte[3]);
	}

	function = "short read";
	word = ptr->d.sword[0]; /* Read as shorts and verify */
	if (word != 0x0123) {
	    errors++;
	    if (noiselevel >= 2)
		report("bus size short", 0x0123, word, &ptr->d.sword[0]);
	}
	word = ptr->d.sword[1]; /* Read as shorts and verify */
	if (word != 0x4567) {
	    errors++;
	    if (noiselevel >= 2)
		report("bus size short", 0x4567, word, &ptr->d.sword[1]);
	}

	/*
	 * Write two short values, verify that it reads correctly as a long
	 * value, then verify that it reads correctly as bytes:
	 */
	asm("	.globl tbs_write_shorts");
	asm("tbs_write_shorts:");
	function = "short write";
	ptr->d.sword[0] = 0x9876; /* Write long value */
	ptr->d.sword[1] = 0x5432;

	function = "long read";
	integer = ptr->d.lword;	/* Read as long word and verify */
	if (integer != 0x98765432) {
	    errors++;
	    if (noiselevel >= 2)
		report("bus size long", 0x98765432, integer, &ptr->d.lword);
	}
	
	function = "byte read";
	byte = ptr->d.byte[0];	/* Read as bytes and verify */
	if (byte != 0x98) {
	    errors++;
	    if (noiselevel >= 2)
		report("bus size byte", 0x98, byte, &ptr->d.byte[0]);
	}
	byte = ptr->d.byte[1];
	if (byte != 0x76) {
	    errors++;
	    if (noiselevel >= 2)
		report("bus size byte", 0x76, byte, &ptr->d.byte[1]);
	}
	byte = ptr->d.byte[2];
	if (byte != 0x54) {
	    errors++;
	    if (noiselevel >= 2)
		report("bus size byte", 0x54, byte, &ptr->d.byte[2]);
	}
	byte = ptr->d.byte[3];
	if (byte != 0x32) {
	    errors++;
	    if (noiselevel >= 2)
		report("bus size byte", 0x32, byte, &ptr->d.byte[3]);
	}

	/*
	 * Write four byte values, verify that the result reads correctly
	 * as short and long values:
	 */
	asm("	.globl tbs_write_bytes");
	asm("tbs_write_bytes:");
	function = "byte write";
	ptr->d.byte[0] = 0xf1;	/* Write byte values */
	ptr->d.byte[1] = 0xa2;
	ptr->d.byte[2] = 0x53;
	ptr->d.byte[3] = 0x04;

	function = "long read";
	integer = ptr->d.lword;	/* Read as long word and verify */
	if (integer != 0xf1a25304) {
	    errors++;
	    if (noiselevel >= 2)
		report("bus size long", 0xf1a25304, integer, &ptr->d.lword);
	}

	function = "short read";
	word = ptr->d.sword[0]; /* Read as shorts and verify */
	if (word != 0xf1a2) {
	    errors++;
	    if (noiselevel >= 2)
		report("bus size short", 0xf1a2, word, &ptr->d.sword[0]);
	}
	word = ptr->d.sword[1];
	if (word != 0x5304) {
	    errors++;
	    if (noiselevel >= 2)
		report("bus size short", 0x5304, word, &ptr->d.sword[1]);
	}
    }
    return(errors);
}

/*
 * Fill memory with "n", read it back, and verify:
 */

test_short(a, b, n)
    register ushort *a, *b, n;
{
    volatile ushort t;
    int errs = 0;
    
    if (abort)
	return(0);
    if (noiselevel >= 1) {
	putnum(testno);
	putchar(' ');
    }

    b = SHORT_ALIGN(b);
    function = "short write";
    asm("	.globl test_short_write");
    asm("test_short_write:");
    for (tp.sp = a; (tp.sp < b) && (!abort); tp.sp++)
	*tp.sp = n;

    function = "short read";
    asm("	.globl test_short_read");
    asm("test_short_read:");
    for (tp.sp = a; (tp.sp < b) && (!abort); tp.sp++)
	if ((t = *tp.sp) != n) {
	    errs++;
	    if (noiselevel >= 2)
		report("constant", SHORT(n), SHORT(t), tp.sp);
	}
    return(errs);
}



/*
 * Fill each short with its address+1, read it back, and verify:
 */

test_addr(a, b)
    register ushort *a, *b;
{
    volatile ushort t;
    int errs = 0;
    
    if (abort)
	return(0);
    if (noiselevel >= 1) {
	putnum(testno);
	putchar(' ');
    }

    b = SHORT_ALIGN(b);
    function = "short write";
    asm("	.globl test_addr_write");
    asm("test_addr_write:");
    for (tp.sp = a; (tp.sp < b) && (!abort); tp.sp++)
	*tp.sp = (ushort)SHORT((u_int)(tp.sp+1));

    function = "short read";
    asm("	.globl test_addr_read");
    asm("test_addr_read:");
    for (tp.sp = a; (tp.sp < b) && (!abort); tp.sp++)
	if ((t = *tp.sp) != (ushort)SHORT((u_int)(tp.sp+1))) {
	    errs++;
	    if (noiselevel >= 2)
		report("address", SHORT((u_int)(tp.sp+1)), SHORT(t), tp.sp);
	}
    return(errs);
}



/*
 * Test random short data:
 */

random_short(a, b)
    register ushort *a, *b;
{
    volatile ushort t;
    register int rand;
    int errs = 0;
    u_short dummy;
    
    if (abort)
	return(0);
    if (noiselevel >= 1) {
	putnum(testno);
	putchar(' ');
    }

    b = SHORT_ALIGN(b);
    seed = 433;
    function = "short write";
    asm("	.globl random_short_write");
    asm("random_short_write:");
    for (tp.sp = a, rand = seed; (tp.sp < b) && (!abort); tp.sp++) {
	rand = (rand << 2) + rand + 17623;
	*tp.sp = (ushort)SHORT(rand);
    }

    function = "short read";
    asm("	.globl random_short_read");
    asm("random_short_read:");
    for (tp.sp = a, rand = seed; (tp.sp < b) && (!abort); tp.sp++) {
	rand = (rand << 2) + rand + 17623;
	if ((t = *tp.sp) != (ushort)SHORT(rand)) {
	    errs++;
	    if (trigger)
		dummy = *trigger;
	    if (noiselevel >= 2)
		report("random short", SHORT(rand), SHORT(t), tp.sp);
	}
    }
    return(errs);
}

/*
 * Create bus errors until the next clock tick (4 ms apart):
 */
void buserror_until_tick()
{
    u_long first_clock;

    first_clock = clock;
    while (clock == first_clock)
	make_buserror();	/* Make bus errors while spin waiting */
}

/*
 * wait_for_memory_decay
 * 	Delay long enough for memory contents to decay if refresh is not
 *	working. Create bus errors while spin waiting
 */

wait_for_memory_decay() {
    register int timer;

    buserror_until_tick();	/* Synchronize with real time clock */
    for (timer = DELAY; timer > 0 && (!abort); timer = timer - 4) {
	buserror_until_tick();	/* Wait for next 4 ms clock tick */
    }
}

/*
 * test_refresh_short - A refresh and buserror test:
 *
 * Fill each short with 0xFFFF, delay long enough for data to change if
 * refresh is not working, read it back and verify.
 * Then, fill each short with 0x0, delay, read it back and verify.
 *
 * While we are waiting, generate bus errors so that we can verify that
 * memory is not corrupted by the buserror exception process.
 * Some boards have exhibited failure mechanisms whereby a buserror
 * exception sequence corrupts memory.
 */

test_refresh_short(a, b)
    register ushort *a, *b;
{
    volatile ushort t;
    int errs = 0;
    ushort v;
    
    if (abort)
	return(0);
    if (noiselevel >= 1) {
	putnum(testno);
	putchar(' ');
    }

    b = SHORT_ALIGN(b);
    function = "short write ones";
    asm("	.globl test_refresh_write_short_ones");
    asm("test_refresh_write_short_ones:");
    v = (ushort)-1;
    for (tp.sp = a; (tp.sp < b) && (!abort); tp.sp++)
	*tp.sp = v;

    wait_for_memory_decay();

    function = "short read ones";
    asm("	.globl test_refresh_read_short_ones");
    asm("test_refresh_read_short_ones:");
    for (tp.sp = a; (tp.sp < b) && (!abort); tp.sp++)
	if ((t = *tp.sp) != v) {
	    errs++;
	    if (noiselevel >= 2)
		report("delayed ones", SHORT(v), SHORT(t), tp.sp);
	}

    function = "short write zeros";
    asm("	.globl test_refresh_write_short_zeros");
    asm("test_refresh_write_short_zeros:");
    v = (ushort)0;
    for (tp.sp = a; (tp.sp < b) && (!abort); tp.sp++)
	*tp.sp = v;

    wait_for_memory_decay();

    function = "short read zeros";
    asm("	.globl test_refresh_read_short_zeros");
    asm("test_refresh_read_short_zeros:");
    for (tp.sp = a; (tp.sp < b) && (!abort); tp.sp++)
	if ((t = *tp.sp) != v) {
	    errs++;
	    if (noiselevel >= 2)
		report("delayed zeros", SHORT(v), SHORT(t), tp.sp);
	}

    return(errs);
}

/*
 * test_refresh_byte - A refresh and buserror test:
 *
 * Fill each byte with 0xFFFF, delay long enough for data to change if
 * refresh is not working, read it back and verify.
 * Then, fill each byte with 0x0, delay, read it back and verify.
 *
 * While we are waiting, generate bus errors so that we can verify that
 * memory is not corrupted by the buserror exception process.
 * Some boards have exhibited failure mechanisms whereby a buserror
 * exception sequence corrupts memory.
 */

test_refresh_byte(a, b)
    u_char *a, *b;
{
    int errs = 0;
    u_char t1;
    
    if (abort)
	return(0);
    if (noiselevel >= 1) {
	putnum(testno);
	putchar(' ');
    }

    /*
     * Constants used instead of variables so
     * compiler will generate a CLR instruction
     * which does read-modify-write
     */
    function = "byte write ones";
    asm("	.globl test_refresh_write_char_ones");
    asm("test_refresh_write_char_ones:");
    for (tp.cp = a; (tp.cp < b) && (!abort); tp.cp++)
	*tp.cp = -1;

    wait_for_memory_decay();

    function = "byte read ones";
    asm("	.globl test_refresh_read_char_ones");
    asm("test_refresh_read_char_ones:");
    for (tp.cp = a; (tp.cp < b) && (!abort); tp.cp++)
	if ((t1 = *tp.cp) != 0xFF) {
	    errs++;
	    if (noiselevel >= 2)
		report("delayed ones", 0xFF, CHAR(t1), tp.cp);
	}

    function = "byte write zeros";
    asm("	.globl test_refresh_write_char_zeros");
    asm("test_refresh_write_char_zeros:");
    for (tp.cp = a; (tp.cp < b) && (!abort); tp.cp++)
	*tp.cp = 0;

    wait_for_memory_decay();

    function = "byte read zeros";
    asm("	.globl test_refresh_read_char_zeros");
    asm("test_refresh_read_char_zeros:");
    for (tp.cp = a; (tp.cp < b) && (!abort); tp.cp++)
	if ((t1 = *tp.cp) != 0) {
	    errs++;
	    if (noiselevel >= 2)
		report("delayed zeros", 0, CHAR(t1), tp.cp);
	}

    return(errs);
}

/*
 * Random byte data test:
 */

random_byte(a, b)
    register u_char *a, *b;
{
    volatile u_char t;
    register int rand;
    int errs = 0;
    
    if (abort)
	return(0);
    if (noiselevel >= 1) {
	putnum(testno);
	putchar(' ');
    }

    seed = 37;
    function = "byte write";
    asm("	.globl random_byte_write");
    asm("random_byte_write:");
    for (tp.cp = a, rand = seed; (tp.cp < b) && (!abort); tp.cp++) {
	rand = (rand << 2) + rand + 327;
	*tp.cp = (u_char)CHAR(rand);
    }

    function = "byte read";
    asm("	.globl random_byte_read");
    asm("random_byte_read:");
    for (tp.cp = a, rand = seed; (tp.cp < b) && (!abort); tp.cp++) {
	rand = (rand << 2) + rand + 327;
	if ((t = *tp.cp) != (u_char)CHAR(rand)) {
	    errs++;
	    if (noiselevel >= 2)
		report("random byte", CHAR(rand), CHAR(t), tp.sp);
	}
    }
    return(errs);
}

/*
 * bit_float - Look for shorts between the bits in a memory cell
 *
 * Chris Shaker, August 29, 1989
 *
 * Algorithm:
 *  o From Start to End test one memory cell in each 64 KB:
 *	From first bit to last bit in memory cell:
 *		Set current bit in bit pattern and clear other bits
 *		Write bit pattern to memory
 *		Read bit pattern from memory and verify
 *  o From Start to End test one memory cell in each 64 KB:
 *	From first bit to last bit in memory cell:
 *		Clear current bit in bit pattern and clear other bits
 *		Write bit pattern to memory
 *		Read bit pattern from memory and verify
 */

#define KB64		(64 * 1024)
#define BITS_IN_WORD	32

bit_float(start, end)
    register u_int *start, *end;
{
    volatile u_int pattern, data;
    int bit_position;
    int errs = 0;
    
    if (abort)
	return(0);
    if (noiselevel >= 1) {
	putnum(testno);
	putchar(' ');
    }
    end = LONG_ALIGN(end);

    /*
     * Float a one through each bit of a word at every 64 KB through the
     * specified region:
     */
    asm("	.globl bit_float_float_one");
    asm("bit_float_float_one:");
    for (tp.ip = start; (tp.ip < end) && (!abort); tp.cp += KB64)
	for (bit_position = 0; bit_position < BITS_IN_WORD; bit_position++) {
	    pattern = (1 << bit_position);
	    function = "integer write";
	    *tp.ip = pattern;
	    function = "integer read ";
	    data = *tp.ip;
	    if (data != pattern) {
		errs++;
		if (noiselevel >= 2)
		    report("bit pattern", pattern, data, tp.ip);
	    }
	}

    /*
     * Float a zero through each bit of a word at every 64 KB through the
     * specified region:
     */
    asm("	.globl bit_float_float_zero");
    asm("bit_float_float_zero:");
    for (tp.ip = start; (tp.ip < end) && (!abort); tp.ip += KB64)
	for (bit_position = 0; bit_position < BITS_IN_WORD; bit_position++) {
	    pattern = ~(1 << bit_position);
	    function = "integer write ";
	    *tp.ip = pattern;
	    function = "integer read ";
	    data = *tp.ip;
	    if (data != pattern) {
		errs++;
		if (noiselevel >= 2)
		    report("bit pattern", pattern, data, tp.ip);
	    }
	}

    return(errs);
}

/*
 * march_test - looks for shorts between memory cells
 *
 * Chris Shaker, August 18, 1989
 *
 * Algorithm:
 *  o From Start to End:
 *     Write background pattern into memory.
 *  o From Start to End:
 *     Read and verify background pattern, then write test pattern.
 *  o From Start to End:
 *     Read and verify test pattern, then write background pattern.
 *  o From End to Start:
 *     Read and verify background pattern, then write test pattern.
 *  o From End to Start:
 *     Read and verify test pattern, then write background pattern.
 *  o From End to Start:
 *     Read and verify background pattern.
 */

march_test(start, end, background, test)
    register u_short *start, *end;
    u_short background, test;
{
    volatile u_short data;
    u_short dummy;
    int errs = 0;
    
    if (abort)
	return(0);
    if (noiselevel >= 1) {
	putnum(testno);
	putchar(' ');
    }

    end = SHORT_ALIGN(end);
    function = "short write";
    asm("	.globl march_test_write_background_up");
    asm("march_test_write_background_up:");
    for (tp.sp = start; (tp.sp < end) && (!abort); tp.sp++)
	*tp.sp = background;

    asm("	.globl march_test_read_background_write_test_up");
    asm("march_test_read_background_write_test_up:");
    for (tp.sp = start; (tp.sp < end) && (!abort); tp.sp++) {
	function = "short read";
	data = *tp.sp;
	if (data != background) {
	    errs++;
	    if (trigger)
		dummy = *trigger;
	    if (noiselevel >= 2)
		report("background", SHORT(background), SHORT(data), tp.sp);
	}
	function = "short write";
	*tp.sp = test;
    }

    asm("	.globl march_test_read_test_write_background_up");
    asm("march_test_read_test_write_background_up:");
    for (tp.sp = start; (tp.sp < end) && (!abort); tp.sp++) {
	function = "short read";
	data = *tp.sp;
	if (data != test) {
	    errs++;
	    if (trigger)
		dummy = *trigger;
	    if (noiselevel >= 2)
		report("test", SHORT(test), SHORT(data), tp.sp);
	}
	function = "short write";
	*tp.sp = background;
    }

    asm("	.globl march_test_read_background_write_test_down");
    asm("march_test_read_background_write_test_down:");
    for (tp.sp = end - 1; (tp.sp >= start) && (!abort); tp.sp--) {
	function = "short read";
	data = *tp.sp;
	if (data != background) {
	    errs++;
	    if (trigger)
		dummy = *trigger;
	    if (noiselevel >= 2)
		report("background", SHORT(background), SHORT(data), tp.sp);
	}
	function = "short write";
	*tp.sp = test;
    }

    asm("	.globl march_test_read_test_write_background_down");
    asm("march_test_read_test_write_background_down:");
    for (tp.sp = end - 1; (tp.sp >= start) && (!abort); tp.sp--) {
	function = "short read";
	data = *tp.sp;
	if (data != test) {
	    errs++;
	    if (trigger)
		dummy = *trigger;
	    if (noiselevel >= 2)
		report("test", SHORT(test), SHORT(data), tp.sp);
	}
	function = "short write";
	*tp.sp = background;
    }

    asm("	.globl march_test_read_background_down");
    asm("march_test_read_background_down:");
    function = "short read";
    for (tp.sp = end - 1; (tp.sp >= start) && (!abort); tp.sp--) {
	data = *tp.sp;
	if (data != background) {
	    errs++;
	    if (trigger)
		dummy = *trigger;
	    if (noiselevel >= 2)
		report("background", SHORT(background), SHORT(data), tp.sp);
	}
    }

    return(errs);
}

/*
 * Random int data test:
 */

random_int(a, b)
    register uint *a, *b;
{
    volatile uint t;
    register uint rand;
    int errs = 0;
    
    if (abort)
	return(0);
    if (noiselevel >= 1) {
	putnum(testno);
	putchar(' ');
    }

    b = LONG_ALIGN(b);
    seed = 4317513;
    function = "integer write";
    asm("	.globl random_int_write");
    asm("random_int_write:");
    for (tp.ip = a, rand = seed; (tp.ip < b) && (!abort); tp.ip++) {
	rand = (rand << 2) + rand + 1069424310L;
	*tp.ip = rand;
    }

    function = "integer read";
    asm("	.globl random_int_read");
    asm("random_int_read:");
    for (tp.ip = a, rand = seed; (tp.ip < b) && (!abort); tp.ip++) {
	rand = (rand << 2) + rand + 1069424310L;
	if ((t = *tp.ip) != rand) {
	    errs++;
	    if (noiselevel >= 2)
		report("random int", rand, t, tp.sp);
	}
    }
    return(errs);
}

/*
 * report - report an error:
 */

report(type, expect, got, addr)
    char *type;
    uint expect, got, *addr;
{
    printf("\nError: %s data test; expected %#x, got %#x at address %#x\n",
	   type, expect, got, addr);
}
