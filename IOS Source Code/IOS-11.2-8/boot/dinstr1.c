/* $Id: dinstr1.c,v 3.2 1995/11/17 07:41:05 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/dinstr1.c,v $
 *------------------------------------------------------------------
 * dinstr.c -- decode instructions in 68k format
 *
 * February 1987, Greg Satz
 *
 * Copyright (c) 1986-1993 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dinstr1.c,v $
 * Revision 3.2  1995/11/17  07:41:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:11:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../defs.h"
#include "../stdio.h"
#include "reason.h"
#include "extern.h"

#define IMDF "#%#x"		/* immediate data format */

typedef struct {
    unsigned short mask, match;
    int (*opfun)();
    char *farg;
} Optab;

char *aregs[] = { "a0", "a1", "a2", "a3", "a4", "a5", "a6", "sp" };

char *bname[16] = {
    "ra", "sr", "hi", "ls", "cc", "cs", "ne",
    "eq", "vc", "vs", "pl", "mi", "ge", "lt", "gt", "le"
};

char *cname[16] = {
    "ra", "f", "hi", "ls", "cc", "cs", "ne",
    "eq", "vc", "vs", "pl", "mi", "ge", "lt", "gt", "le"
};

char *dbname[16] = {
    "t", "ra", "hi", "ls", "cc", "cs", "ne",
    "eq", "vc", "vs", "pl", "mi", "ge", "lt", "gt", "le"
};

char *shro[4] = { "as", "ls", "rox", "ro" };

char *bit[4] = { "btst", "bchg", "bclr", "bset" };

int omove(),obranch(),oimmed(),oprint(),oneop(),soneop(),oreg(),ochk();
int olink(),omovem(),oquick(),omoveq(),otrap(),oscc(),opmode(),shroi();
int extend(),biti(),omovs(),omovc(),ostop(),orts(),ortspop(),jsrop();
int jmpop(),odbcc(),moneop();

/*
 * order is important here
 */

Optab optab[] = {
    0xF000, 0x1000, omove, "b",			/* move instructions */
    0xF000, 0x2000, omove, "l",
    0xF000, 0x3000, omove, "w",
    0xF000, 0x6000, obranch, 0,			/* branches   */
    0xFF00, 0x0000, oimmed, "or",		/* op class 0  */
    0xFF00, 0x0200, oimmed, "and",
    0xFF00, 0x0400, oimmed, "sub",
    0xFF00, 0x0600, oimmed, "add",
    0xFF00, 0x0A00, oimmed, "eor",
    0xFF00, 0x0C00, oimmed, "cmp",
    0xFF00, 0x0E00, omovs,  "movs",
    0xF100, 0x0100, biti, 0,
    0xF800, 0x0800, biti, 0,
    0xFFC0, 0x40C0, oneop, "move.w sr,",	/* op class 4 */
    0xFF00, 0x4000, soneop, "negx",
    0xFFC0, 0x42C0, oneop,  "move.w ccr,",
    0xFF00, 0x4200, soneop, "clr",
    0xFFC0, 0x44C0, moneop, "ccr",
    0xFF00, 0x4400, soneop, "neg",
    0xFFC0, 0x46C0, moneop, "sr",
    0xFF00, 0x4600, soneop, "not",
    0xFFC0, 0x4800, oneop, "nbcd ",
    0xFFF8, 0x4840, oreg, "swap d%d",
    0xFFC0, 0x4840, oneop, "pea ",
    0xFFF8, 0x4880, oreg, "ext.w d%d",
    0xFFF8, 0x48C0, oreg, "ext.l d%d",
    0xFB80, 0x4880, omovem, 0,
    0xFFC0, 0x4AC0, oneop, "tas ",
    0xFF00, 0x4A00, soneop, "tst",
    0xFFF0, 0x4E40, otrap, 0,
    0xFFF8, 0x4E50, olink, 0,
    0xFFFE, 0x4E7A, omovc, "movc ",
    0xFFF8, 0x4880, oreg, "ext.w d%d",
    0xFFF8, 0x48C0, oreg, "ext.l d%d",
    0xFFF8, 0x4E58, oreg, "unlk a%d",
    0xFFF8, 0x4E60, oreg, "move.l a%d,usp",
    0xFFF8, 0x4E68, oreg, "move.l usp,a%d",
    0xFFFF, 0x4E70, oprint, "reset",
    0xFFFF, 0x4E71, oprint, "nop",
    0xFFFF, 0x4E72, ostop, "stop %#x",
    0xFFFF, 0x4E73, oprint, "rte",
    0xFFFF, 0x4E74, ortspop, "rts %#x",
    0xFFFF, 0x4E75, orts, "rts",
    0xFFFF, 0x4E76, oprint, "trapv",
    0xFFFF, 0x4E77, oprint, "rtr",
    0xFFC0, 0x4E80, jsrop, "jsr ",
    0xFFC0, 0x4EC0, jmpop, "jmp ",
    0xF1C0, 0x4180, ochk, "chk",
    0xF1C0, 0x41C0, ochk, "lea",
    0xF0F8, 0x50C8, odbcc, "db%s d%d,",
    0xF0C0, 0x50C0, oscc,  0,
    0xF100, 0x5000, oquick, "addq",
    0xF100, 0x5100, oquick, "subq",
    0xF000, 0x7000, omoveq, 0,
    0xF1C0, 0x80C0, ochk, "divu",
    0xF1C0, 0x81C0, ochk, "divs",
    0xF1F0, 0x8100, extend, "sbcd",
    0xF000, 0x8000, opmode, "or",
    0xF1C0, 0x91C0, opmode, "sub",
    0xF130, 0x9100, extend, "subx",
    0xF000, 0x9000, opmode, "sub",
    0xF1C0, 0xB1C0, opmode, "cmp",
    0xF138, 0xB108, extend, "cmpm",
    0xF100, 0xB000, opmode, "cmp",
    0xF100, 0xB100, opmode, "eor",
    0xF1C0, 0xC0C0, ochk, "mulu",
    0xF1C0, 0xC1C0, ochk, "muls",
    0xF1F8, 0xC188, extend, "exg",
    0xF1F8, 0xC148, extend, "exg",
    0xF1F8, 0xC140, extend, "exg",
    0xF1F0, 0xC100, extend, "abcd",
    0xF000, 0xC000, opmode, "and",
    0xF1C0, 0xD1C0, opmode, "add",
    0xF130, 0xD100, extend, "addx",
    0xF000, 0xD000, opmode, "add",
    0xF100, 0xE000, shroi, "r",
    0xF100, 0xE100, shroi, "l",
    0, 0, 0, 0
};

void dinstr(loc)
	long loc;
{
	register Optab *op;
	unsigned short ins;

	printf("%x: ", loc);
	ins = *(unsigned short *)loc;
	loc += sizeof(ins);
	op = &optab[0];
	while (op->mask != 0 && (ins & op->mask) != op->match)
		op++;
	instrsize = sizeof(ins);
	instraddr = loc;
	if (op->mask == 0 || ins < 0x40)
		printf(".word %#x", ins);
	else
		instrsize += (*op->opfun)(loc, ins, op->farg);
}

/*
 * Print out the effective address for the given parameters.
 */

printea(loc, mode, reg, size)
long loc, mode, reg;
int size;
{
    long index, disp, n;
    char b;
    short w;
    long l;

    n = 0;
    switch (mode) {
	case 0:
	    printf("d%d", reg);
	    break;

	case 1:
	    printf("%s", aregs[reg]);
	    break;

	case 2:
	    printf("(%s)", aregs[reg]);
	    break;

	case 3:
	    printf("(%s)+", aregs[reg]);
	    break;

	case 4:
	    printf("-(%s)", aregs[reg]);
	    break;

	case 5:
	    w = *(short *)loc;
	    n += sizeof(w);
	    printf("%d(%s)", w, aregs[reg]);
	    break;

	case 6:
	    w = *(short *)loc;
	    n += sizeof(w);
	    index = w & 0xFFFF;
	    disp = (char)(index&0377);
	    printf("%d(%s,%c%d.%c)", disp, aregs[reg],
		(index&0100000)?'a':'d',(index>>12)&07,
		(index&04000)?'l':'w');
	    break;

	case 7:
	    switch (reg) {
		case 0:
		    w = *(short *)loc;
		    n += sizeof(w);
		    index = w & 0xFFFF;
		    printf("%#x", index);
		    break;

		case 1:
		    l = *(long *)loc;
		    n += sizeof(l);
		    index = l;
		    printf("%#x", index);
		    break;

		case 2:	
		    w = *(short *)loc;
		    n += sizeof(w);
		    disp = w;
		    printf("%#x", disp + instraddr + n);
		    break;

		case 3:
		    w = *(short *)loc;
		    n += sizeof(w);
		    index = w & 0xFFFF;
		    disp = (char)(index&0377);
		    printf("%d(pc,%c%d.%c)", disp,
			(index&0100000)?'a':'d',(index>>12)&07,
			(index&04000)?'l':'w');
		    break;

		case 4:
		    switch (size) {
			case sizeof(b):
			    w = *(short *)loc;
			    n += sizeof(w);
			    index = (w&0xff);
			    break;

			case sizeof(w):
			    w = *(short *)loc;
			    n += sizeof(w);
			    index = w & 0xFFFF;
			    break;

			case sizeof(l):
			    l = *(long *)loc;
			    n += sizeof(l);
			    index = l;
			    break;

			default:
			    printf("unexpected size %d in printea\n", size);
			    l = *(long *)loc;
			    n += sizeof(l);
			    index = l;
			    break;
		    }
		    printf(IMDF, index);
		    break;

		default:
		    printf("???");
		    break;
	    }
	    break;

	default:
	    printf("???");
	    break;
    }
    return n;
}

printEA(loc, ea, size)
long loc, ea;
int size;
{
    return printea(loc, (ea>>3)&07, ea&07, size);
}

mapsize(inst)
register long inst;
{
    int m;

    inst >>= 6;
    inst &= 03;
    switch (inst) {
	case 0:
	    m = 1;
	    break;

	case 1:
	    m = 2;
	    break;

	case 2:
	    m = 4;
	    break;

	default:
	    printf("illegal size %d in mapsize\n", inst);
	    m = 4;
	    break;
    }
    return m;
}

char suffix(size)
int size;
{
    char c;

    switch (size) {
	case 1:
	    c = 'b';
	    break;

	case 2:
	    c = 'w';
	    break;

	case 4:
	    c = 'l';
	    break;

	default:
	    printf("bad size %d in suffix", size);
	    c = 'l';
	    break;
    }
    return c;
}

/*
 * Instruction class specific routines.
 */

omove(loc, inst, s)
long loc, inst;
char *s;
{
    register int c;
    int size;
    int n;

    c = s[0];
    printf("move.%c ", c);
    size = ((c == 'b') ? 1 : (c == 'w') ? 2 : 4);
    n = printea(loc, (inst>>3)&07, inst&07, size);
    putchar(',');
    n += printea(loc+n, (inst>>6)&07, (inst>>9)&07, size);
    return n;
}

/* 
 * Two types: bsr (4 bytes) and bsrs (2 bytes)
 */

obranch(loc, inst, dummy)
long loc, inst;
long dummy;
{
    long disp;
    char *s;
    short w;
    char c;
    int branchtype;		/* type of branch (0 = unconditional) */
    int n = 0;

    c = inst&0377;
    s = ".s";
    if (c == (char)0xFF) {
	s = NULL;
	disp = *(unsigned long *)loc;
	n += sizeof(disp);
    } else if (c == 0) {
	s = NULL;
	w = *(unsigned short *)loc;
	n += sizeof(w);
	disp = w;
    } else
	disp = c;
    branchtype = (int)((inst>>8)&017);
    printf("b%s%s %#x", bname[branchtype], s, instraddr + disp);

#ifdef	lint
    dummy = dummy;
#endif

    return n;
}

odbcc(loc, inst, form)
long loc, inst;
char *form;
{
    short w;
    int n = 0;

    w = *(short *)loc;
    n += sizeof(w);
    printf(form, dbname[(int)((inst>>8)&017)], inst&07);
    printf("%#x", instraddr + w);
    return n;
}

oscc(loc, inst, dummy)
long loc, inst;
long dummy;
{
    printf("s%s ", cname[(int)((inst>>8)&017)]);

#ifdef	lint
    dummy = dummy;
#endif

    return printea(loc, (inst>>3)&07, inst&07, 1);
}

biti(loc, inst, dummy)
long loc, inst;
long dummy;
{
    unsigned short w;
    int n = 0;

    printf("%s ", bit[(int)((inst>>6)&03)]);
    if (inst&0x0100) {
	printf("d%d,", inst>>9);
    } else {
	w = *(unsigned short *)loc;
	n += sizeof(w);
	printf(IMDF, w & 0xFFFF);
	putchar(',');
    }
    n += printEA(loc+n, inst);

#ifdef	lint
    dummy = dummy;
#endif

    return n;
}

opmode(loc, inst, opcode)
long loc, inst;
long opcode;
{
    register int op_mode;
    register int reg;
    int size;
    int n = 0;

    op_mode = (int)((inst>>6) & 07);
    reg = (int)((inst>>9) & 07);
    if (op_mode == 0 || op_mode == 4) {
	size = 1;
    } else if (op_mode == 1 || op_mode == 3 || op_mode == 5) {
	size = 2;
    } else {
	size = 4;
    }
    printf("%s.%c ", opcode, suffix(size));
    if (op_mode >= 4 && op_mode <= 6) {
	printf("d%d,", reg);
	n += printea(loc, (inst>>3)&07, inst&07, size);
    } else {
	n += printea(loc, (inst>>3)&07, inst&07, size);
	if (reg == 7 && op_mode > 2)
	    printf(",sp");
	else
	    printf(",%c%d",(op_mode<=2) ? 'd' : 'a', reg);
    }
    return n;
}

shroi(loc, inst, ds)
long loc, inst;
char *ds;
{
    int rx, ry;
    char *opcode;
    int n = 0;

    if ((inst & 0xC0) == 0xC0) {
	opcode = shro[(int)((inst>>9)&03)];
	printf("%s%s ", opcode, ds);
	n += printEA(loc, inst);
    } else {
	opcode = shro[(int)((inst>>3)&03)];
	printf("%s%s.%c ", opcode, ds, suffix(mapsize(inst)));
	rx = (int)((inst>>9)&07); ry = (int)(inst&07);
	if ((inst>>5)&01) {
	    printf("d%d,d%d", rx, ry);
	} else {
	    printf(IMDF, (rx ? rx : 8));
	    printf(",d%d", ry);
	}
    }
    return n;
}		

oimmed(loc, inst, opcode)
long loc, inst;
register char *opcode;
{
    register int size;
    unsigned long constt;
    unsigned short w;
    int n = 0;

    size = mapsize(inst);
    if (size > 0) {
	if (size == 4) {
	    constt = *(unsigned long *)loc;
	    n += sizeof(constt);
	} else {
	    w = *(unsigned short *)loc;
	    n += sizeof(w);
	    constt = w & 0xFFFF;
	}
	printf("%s.%c ", opcode, suffix(size));
	printf(IMDF, constt);
	putchar(',');
	if (inst == 0x6C) {
	    puts("ccr");
	} else if (inst == 0x7C) {
	    puts("sr");
	} else
	    n += printEA(loc+n, inst, size);
    } else {
	puts("badop");
    }
    return n;
}

oreg(loc, inst, opcode)
long loc, inst;
register char *opcode;
{
    printf(opcode, (inst & 07));

#ifdef	lint
    loc = loc;
#endif

    return 0;
}

extend(loc, inst, opcode)
long loc, inst;
char *opcode;
{
    register int size;
    int ry, rx;
    char c;

    size = mapsize(inst);
    ry = (inst&07);
    rx = ((inst>>9)&07);
    c = ((inst & 0x1000) ? suffix(size) : ' ');
    printf("%s%s%c ", opcode, c == ' ' ? "" : ".", c);
    if (opcode[0] == 'e') {
	if (inst & 0x0080) {
	    printf("d%d,a%d", rx, ry);
	} else if (inst & 0x0008) {
	    printf("a%d,a%d", rx, ry);
	} else {
	    printf("d%d,d%d", rx, ry);
	}
    } else if ((inst & 0xF000) == 0xB000) {
	printf("(a%d)+,(a%d)+", ry, rx);
    } else if (inst & 0x8) {
	printf("-(a%d),-(a%d)", ry, rx);
    } else {
	printf("d%d,d%d", ry, rx);
    }

#ifdef	lint
    loc = loc;
#endif

    return 0;
}

olink(loc, inst, dummy)
long loc, inst;
long dummy;
{
    unsigned short w;
    int n = 0;

    w = *(unsigned short *)loc;
    n += sizeof(w);
    printf("link a%d,", inst&07);
    printf(IMDF, w & 0xFFFF);

#ifdef	lint
    dummy = dummy;
#endif

    return n;
}

otrap(loc, inst, dummy)
long loc, inst;
long dummy;
{
    printf("trap ");
    printf(IMDF, inst&017);

#ifdef	lint
    dummy = dummy;
    loc = loc;
#endif

    return 0;
}

oneop(loc, inst, opcode)
long loc, inst;
register char *opcode;
{
    printf("%s",opcode);
    return printEA(loc, inst);
}

moneop(loc, inst, reg)
long loc, inst;
char *reg;
{
    int n;

    puts("move.w ");
    n = printEA(loc, inst);
    printf(",%s", reg);
    return n;
}

jsrop(loc, inst, opcode)
long loc, inst;
register char *opcode;
{
    unsigned int startingaddr;	/* beginning of jsr instruction */
    unsigned int retaddr; /* can't call return_addr (frame not set up yet) */

    startingaddr = instraddr - 2;
    switch ((inst >> 3) & 07) {
	case 2:
	    retaddr = instraddr;		/* two byte instruction */
	    break;
	case 5:
	case 6:
	    retaddr = instraddr + 2;	/* four byte instruction */
	    break;
	case 7:
	default:
	    switch (inst & 07) {
		case 0:
		case 2:
		case 3:
		    retaddr = instraddr + 2;
		    break;
		case 1:
		default:
		    retaddr = instraddr + 4;	/* six byte instruction */
		    break;
	    }
	    break;
    }
    printf("%s",opcode);

#ifdef	lint
    retaddr = retaddr;
    startingaddr = startingaddr;
#endif

    return printEA(loc, inst);
}

jmpop(loc, inst, opcode)
long loc, inst;
register char *opcode;
{
    printf("%s",opcode);
    return printEA(loc, inst);
}

pregmask(mask)
register int mask;
{
    register int i;
    register int flag = 0;

    printf("#<");
    for (i=0; i<16; i++) {
	if (mask&1) {
	    if (flag) {
		putchar(',');
	    } else {
		++flag;
	    }
	    printf("%c%d",(i<8) ? 'd' : 'a', i&07);
	}
	mask >>= 1;
    }
    printf(">");
}

omovem(loc, inst, dummy)
long loc, inst;
long dummy;
{
    register int i, list, mask;
    register int reglist;
    unsigned short w;
    int n = 0;

    i = 0;
    list = 0;
    mask = 0100000;
    w = *(unsigned short *)loc;
    n += sizeof(w);
    reglist = w;
    if ((inst & 070) == 040) {	/* predecrement */
	for (i = 15; i > 0; i -= 2) {
	    list |= ((mask & reglist) >> i);
	    mask >>= 1;
	}
	for (i = 1; i < 16; i += 2) {
	    list |= ((mask & reglist) << i);
	    mask >>= 1;
	}
	reglist = list;
    }
    printf("movem.%c ",(inst&100)?'l':'w');
    if (inst&02000) {
	n += printEA(loc+n, inst);
	putchar(',');
	pregmask(reglist);
    } else {
	pregmask(reglist);
	putchar(',');
	n += printEA(loc+n, inst);
    }

#ifdef	lint
    dummy = dummy;
#endif

    return n;
}

ochk(loc, inst, opcode)
long loc, inst;
register char *opcode;
{
    int n = 0;

    printf("%s ", opcode);
    n += printEA(loc, inst, sizeof(char));
    printf(",%c%d", (opcode[0] == 'l') ? 'a' : 'd', (inst>>9)&07);
    return n;
}

soneop(loc, inst, opcode)
long loc, inst;
register char *opcode;
{
    register int size;
    int n = 0;

    size = mapsize(inst);
    if (size > 0) {
	printf("%s.%c ", opcode, suffix(size));
	n += printEA(loc, inst);
    } else {
	puts("badop");
    }
    return n;
}

oquick(loc, inst, opcode)
long loc, inst;
register char *opcode;
{
    register int size;
    register int data;
    int n = 0;

    size = mapsize(inst);
    data = (int)((inst>>9) & 07);
    if (data == 0) {
	data = 8;
    }
    if (size > 0) {
	printf("%s.%c ", opcode, suffix(size));
	printf(IMDF, data);
	putchar(',');
	n += printEA(loc, inst);
    } else {
	puts("badop");
    }
    return n;
}

omoveq(loc, inst, dummy)
long loc, inst;
long dummy;
{
    register int data;

    data = (int)(inst & 0377);
    if (data > 127) {
	data |= ~0377;
    }
    puts("moveq ");
    printf(IMDF, data);
    printf(",d%d", (inst>>9)&07);

#ifdef	lint
    dummy = dummy;
    loc = loc;
#endif

    return 0;
}

oprint(loc, inst, opcode)
long loc, inst;
register char *opcode;
{
    printf("%s",opcode);

#ifdef	lint
    inst = inst;
    loc = loc;
#endif

    return 0;
}

ostop(loc, inst, opcode)
long loc, inst;
register char *opcode;
{
    unsigned short w;
    int n = 0;

    w = *(unsigned short *)loc;
    n += sizeof(w);
    printf(opcode, w);

#ifdef	lint
    inst = inst;
#endif

    return n;
}

orts(loc, inst, opcode)
long loc, inst;
register char *opcode;
{
    printf("%s",opcode);

#ifdef	lint
    loc = loc;
    inst = inst;
#endif

    return 0;
}

/*
 * Not used by C compiler; does an rts but before doing so, pops
 * arg bytes from the stack.
 */

ortspop(loc, inst, opcode)
long loc, inst;
register char *opcode;
{
    unsigned short w;
    int n = 0;

    w = *(unsigned short *)loc;
    n += sizeof(w);
    printf(opcode, w);

#ifdef	lint
    inst = inst;
#endif

    return n;
}

omovs(loc, inst, opcode)
long loc, inst;
char *opcode;
{
    register int size;
    register unsigned int controlword;
    unsigned short w;
    int n = 0;

    size = mapsize(inst);
    w = *(unsigned short *)loc;
    n += sizeof(w);
    controlword = w >> 11;
    printf("%s%c ", opcode, suffix(size));
    if (controlword & 1){
	controlword >>= 1;
	printf((controlword&0x8) ? "a%d," : "d%d,", controlword&7);
	n += printEA(loc+n, inst&0xff, size);
    } else {
	controlword >>= 1;
	n += printEA(loc+n, inst&0xff, size);
	printf((controlword&0x8) ? ",a%d" : ",d%d", controlword&7);
    }
    return n;
}

omovc(loc, inst, opcode)
long loc, inst;
char *opcode;
{
    register unsigned int controlword;
    char *creg;
    unsigned short w;
    int n = 0;

    w = *(unsigned short *)loc;
    n += sizeof(w);
    controlword = w;
    switch (controlword & 0xfff) {
	case 0:
	    creg = "sfc";
	    break;

	case 1:
	    creg = "dfc";
	    break;

	case 0x800:
	    creg = "usp";
	    break;

	case 0x801:
	    creg = "vbr";
	    break;

	default:
	    creg = "???";
	    break;
	}
    controlword >>= 12;
    if (inst & 1) {
	printf((controlword&0x8) ? "%sa%d,%s" : "%sd%d,%s",
		opcode, controlword&7, creg );
    } else {
	printf((controlword&0x8) ? "%s%s,a%d" : "%s%s,d%d",
		opcode, creg, controlword&7 );
    }
    return n;
}
