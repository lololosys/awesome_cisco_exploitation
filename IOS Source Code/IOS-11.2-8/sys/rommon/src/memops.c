/* $Id: memops.c,v 3.3.28.2 1996/07/08 22:42:44 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src/memops.c,v $
 *------------------------------------------------------------------
 * memops.c
 *
 * April 1993, Rob Clevenger
 *
 * Copyright (c) 1994 - 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Various memory utility programs.
 *------------------------------------------------------------------
 * $Log: memops.c,v $
 * Revision 3.3.28.2  1996/07/08  22:42:44  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.3.28.1  1996/03/21  23:27:27  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/02/08  08:58:07  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.2  1995/12/09  20:49:45  mbeesley
 * Branch: ELC_branch
 * Fix monitor dump command.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:33  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.3  1996/01/17  23:31:21  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.2  1995/11/17  18:42:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** Memory operations.
** These utilities will access memory as words (16 bits) (default), bytes,
** or longwords (32 bits) with the exception of memtest which tests in
** longwords for efficiency.
*/

#include "monitor.h"
#include "proto.h"
#include "testmem.h"
#include "signal_4k.h"
#include "error.h"
#include "queryflags.h"
#include "mon_defs.h"

#define LINE_LEN 16

#define TOPRINT(p) ((((p) >= ' ') && ((p) < 0x7f)) ? (p) : '.')


typedef int(*PFI)();
static char optstr[] = "dlbw";
extern int optind;
extern long memsize;
extern long shmemsize;

static int
getopsize(int argc, char *argv[])
{
    register i;
    extern int optind;
    char memopsiz;
    
    memopsiz = BW_16BITS;  /* default is word size operations */
    while((i = getopt(argc,argv,optstr)) >= 0) {
	switch(i) {
	case 'd':
	    memopsiz = BW_64BITS;
	    break;
	case 'l':
	    memopsiz = BW_32BITS;
	    break;
	case 'b':
	    memopsiz = BW_8BITS;
	    break;
	case 'w':
	    memopsiz = BW_16BITS;
	    break;
	case '?':
	    return(-1);
	}
    }
    MEMOP_SIZE = memopsiz;
    return(0);
}

static int
getaddr(char *argptr, unsigned long *locptr)
{
    register unsigned short wval;
    
    if(getnum(argptr,16,(long *)locptr) == 0) return(-1);
    return(0);
}

int
cmp_mem(int argc, char *argv[])
{
    int (*cmpfunc)() = 0;    /* to supress warning from compiler  :^( */

    if(argc == 1) {
	query_user( QU_SOURCE | QU_DEST | QU_SIZE | QU_OPSIZ ,
		   &MEMOP_SRCADDR, &MEMOP_DESTADDR,
		   &MEMOP_LENGTH, &MEMOP_SIZE);
    } else {
	if(getopsize(argc,argv) < 0 ||
	   getaddr(argv[optind++],&MEMOP_SRCADDR) < 0 ||
	   getaddr(argv[optind++],&MEMOP_DESTADDR) < 0 ||
	   getnum(argv[optind++],16,(long *)&MEMOP_LENGTH) == 0 || optind != argc) {
	    printf("usage: %s [-%s] addr0 addr1 length\n",argv[0],optstr);
	    return(1);
	}
    }
    switch(MEMOP_SIZE) {
    case BW_8BITS:
	cmpfunc = cmpbyte;
	break;
    case BW_16BITS:
	cmpfunc = cmpword;
	break;
    case BW_32BITS:
	cmpfunc = cmplword;
	break;
    }
    (*cmpfunc)(MEMOP_SRCADDR,MEMOP_DESTADDR,MEMOP_LENGTH);
    return(0);
}

int
cmpbyte(unsigned char *addr0, unsigned char *addr1, int length)
{
    register unsigned char dat0, dat1, *end;
    int retval = 0;
    
    end = (unsigned char *)((unsigned)addr0 + length);
    while(addr0 < end) {
	dat0 = *addr0++;
	dat1 = *addr1++;
	if(dat0 != dat1) {
	    printf("%.6x = %.2x    %.6x = %.2x\n",addr0-1,dat0,addr1-1,dat1);
	    retval = 1;
	}
    }
    return(retval);
}

int
cmpword(unsigned short *addr0, unsigned short *addr1, int length)
{
    register unsigned short dat0, dat1, *end;
    int retval = 0;

    end = (unsigned short *)((unsigned)addr0 + length);
    while(addr0 < end) {
	dat0 = *addr0++;
	dat1 = *addr1++;
	if(dat0 != dat1) {
	    printf("%.6x = %.4x    %.6x = %.4x\n",addr0-1,dat0,addr1-1,dat1);
	    retval = 1;
	}
    }
    return(retval);
}

int
cmplword(unsigned *addr0, unsigned *addr1, int length)
{
    register unsigned dat0, dat1, *end;
    int retval = 0;

    end = (unsigned *)((unsigned)addr0 + length);
    while(addr0 < end) {
	dat0 = *addr0++;
	dat1 = *addr1++;
	if(dat0 != dat1) {
	    printf("%.6x = %.8x    %.6x = %.8x\n",addr0-1,dat0,addr1-1,dat1);
	    retval = 1;
	}
    }
    return(retval);
}

int
mov_mem(int argc, char *argv[])
{
    void (*movfunc)() = 0;    /* to supress warning from compiler  :^( */

    if(argc == 1) {
	query_user( QU_SOURCE | QU_DEST | QU_SIZE | QU_OPSIZ ,
		   &MEMOP_SRCADDR, &MEMOP_DESTADDR,
		   &MEMOP_LENGTH, &MEMOP_SIZE);
    } else {
	if(getopsize(argc,argv) < 0 ||
	   getaddr(argv[optind++],&MEMOP_SRCADDR) < 0 ||
	   getaddr(argv[optind++],&MEMOP_DESTADDR) < 0 ||
	   getnum(argv[optind++],16,(long *)&MEMOP_LENGTH) == 0 || optind != argc) {
	    printf("usage: %s [-%s] addr0 addr1 length\n",argv[0],optstr);
	    return(1);
	}
    }
    switch(MEMOP_SIZE) {
    case BW_8BITS:
	movfunc = movbyte;
	break;
    case BW_16BITS:
	movfunc = movword;
	break;
    case BW_32BITS:
	movfunc = movlword;
	break;
    }
    (*movfunc)(MEMOP_SRCADDR,MEMOP_DESTADDR,MEMOP_LENGTH);
    return(0);
}

void
movbyte(unsigned char *addr0, unsigned char *addr1, int length)
{
  register unsigned char *end;

  end = (unsigned char *)((unsigned)addr0 + length);
  while(addr0 < end) {
    *addr1++ = *addr0++;
  }
}

void
movword(unsigned short *addr0, unsigned short *addr1, int length)
{
  register unsigned short *end;

  end = (unsigned short *)((unsigned)addr0 + length);
  while(addr0 < end) {
    *addr1++ = *addr0++;
  }
}

void
movlword(unsigned *addr0, unsigned *addr1, int length)
{
  register unsigned *end;

  end = (unsigned *)((unsigned)addr0 + length);
  while(addr0 < end) {
    *addr1++ = *addr0++;
  }
}

int
fil_mem(int argc, char *argv[])
{
    int (*filfunc)() = 0;    /* to supress warning from compiler  :^( */

     /* 
    ** check for incrementing fill command
    ** note that argv is is chacked for validity - when this
    ** routine is called from the menu() routine, argv will
    ** be zero
    */
    if(argv && strcmp(argv[0],"ifill") == 0) hkeepflags |= H_INCFILL;
    else hkeepflags &= ~H_INCFILL;
    if(argc == 1) {
	query_user( QU_START | QU_SIZE | QU_VALUE | QU_OPSIZ ,
		   &MEMOP_START, &MEMOP_LENGTH,
		   &MEMOP_VALUE, &MEMOP_SIZE);
    } else {
	if(getopsize(argc,argv) < 0 ||
	   getaddr(argv[optind++],&MEMOP_START) < 0 ||
	   getaddr(argv[optind++],&MEMOP_LENGTH) < 0 ||
	   getnum(argv[optind++],16,(long *)&MEMOP_VALUE) == 0 || optind != argc) {
	    printf("usage: %s [-%s] addr length value\n",argv[0],optstr);
	    return(1);
	}
    }
    switch(MEMOP_SIZE) {
    case BW_8BITS:
	filfunc = (PFI)filbyte;
	break;
    case BW_16BITS:
	filfunc = (PFI)filword;
	break;
    case BW_32BITS:
	filfunc = (PFI)fillword;
	break;
    }
    (*filfunc)(MEMOP_START,MEMOP_LENGTH,MEMOP_VALUE);
    return(0);
}

void
filbyte(unsigned char *addr, int length, unsigned char val)
{
  register unsigned char *end = (unsigned char *)((unsigned)addr + length);

  if(hkeepflags & H_INCFILL) while(addr < end) *addr++ = val++;
  else while(addr < end) *addr++ = val;
}

void
filword(unsigned short *addr, int length, unsigned short val)
{
  register unsigned short *end = (unsigned short *)((unsigned)addr + length);
  if(hkeepflags & H_INCFILL) while(addr < end) *addr++ = val++;
  else while(addr < end) *addr++ = val;
}

void
fillword(unsigned *addr, unsigned length, unsigned val)
{
  register unsigned *end = (unsigned *)((unsigned)addr + length);

  if(hkeepflags & H_INCFILL) while(addr < end) *addr++ = val++;
  else while(addr < end) *addr++ = val;
}

int
dis_mem(int argc, char *argv[])
{
    int fldsiz = BW_8BITS;  /* init */

    if(argc == 1) {
	query_user( QU_START | QU_SIZE | QU_OPSIZ,
		   &MEMOP_START, &MEMOP_LENGTH, &MEMOP_SIZE);
    } else {
	if(getopsize(argc,argv) < 0 ||
	   (argc - optind) != 2 ||
	   getaddr(argv[optind++],&MEMOP_START) < 0 ||  /* and the physical address */
	   getnum(argv[optind],16,(long *)&MEMOP_LENGTH) == 0) {
	    printf("usage: %s [-%s] addr length\n",argv[0],optstr);
	    return(1);
	}
    }
    dismem((unsigned char *)MEMOP_START,MEMOP_LENGTH,MEMOP_START,MEMOP_SIZE);
    return(0);
}



void
dismem(unsigned char *addr, int length, unsigned disaddr, int fldsize)
{
  unsigned long value = 0, uvalue = 0;
  register unsigned char i, j, c, linepos, asciistart = 0;
  register unsigned char *end = (addr + length);  /* the end boundary */
  register unsigned char *linend, *linestart;
  char ascii[LINE_LEN + 1];
  int ix;

  ascii[sizeof(ascii) - 1] = '\0';   /* end of the string for full line */

  while(addr < end) {
      linepos = printf("%.6x  ",disaddr);  /* display the line address */
      linend = (addr + 16);
      ix = 0;
      
      while(addr < linend) {
	  switch(fldsize) {
	  case BW_8BITS:
	      value = *addr++;
	      break;
	  case BW_16BITS:
	      value = *(unsigned short *)addr;
	      addr += 2;
	      break;
	  case BW_32BITS:
	      value = *(unsigned *)addr;
	      addr += 4;
	      break;
	  case BW_64BITS:
	      LD_ACCESS((unsigned long *)addr, &value, &uvalue);
	      addr += 8;
	      break;
	  } /* switch */

	  if (fldsize == BW_64BITS) {
	      linepos += printf("%.*x",fldsize, value);  /* hex */
	      linepos += printf("%.*x ",fldsize, uvalue);  /* hex */
	  } else {
	      linepos += printf("%.*x ",fldsize * 2, value);  /* hex */
	  }

#ifdef NATIVE
	  for(j=0; j<(fldsize * 8); j+= 8) {
#else
	  for(j=(fldsize * 8); j;) {    
	      j -= 8;
#endif
	      c = (value >> j);
	      ascii[ix++] = TOPRINT(c);
	  } /* for */

	  if(addr >= end) {
	      ascii[ix] = '\0';        /* terminate the ascii string, not full line*/
	      break;
	  }
      } /* while */

      if(!asciistart) asciistart = linepos;  /* record start 1st line */
      else while(linepos++ < asciistart) putchar(' ');  /* pad w/spaces in last line*/

      printf("%s\n", ascii);
      disaddr += 16;
  } /* while */

}


int
alt_mem(int argc, char *argv[])
{
  union location {
    unsigned char byte;
    unsigned short word;
    unsigned lword;
  };
  register union location *addr;
  int opsiz, tmp;
  register char *c_ptr;
  char inbuf[16];
  unsigned val;

  if(argc == 1) {
      query_user( QU_START | QU_OPSIZ, &MEMOP_START,
		 &MEMOP_SIZE);
  } else {
      if(getopsize(argc,argv) < 0 ||
	 getaddr(argv[optind++],&MEMOP_START) < 0 || optind != argc) {
	  printf("usage: %s [-%s] addr\n",argv[0],optstr);
	  return(1);
      }
  }
  addr = (union location *)MEMOP_START;
  opsiz = MEMOP_SIZE;
  while(1) {
    printf("%.6x = ",addr);
    switch(opsiz) {
    case BW_8BITS:
      printf("%.2x",addr->byte);
      break;
    case BW_16BITS:
      printf("%.4x",addr->word);
      break;
    case BW_32BITS:
      printf("%.8x",addr->lword);
      break;
    }
    puts(" > ");
    c_ptr = inbuf;
    getline(c_ptr,sizeof(inbuf));
    switch(*c_ptr) {
    case 'x':
    case 'q': return(0);  /* quit */
    case ',':
    case 'p': /* prev location */
      addr = (union location *)((unsigned)addr - opsiz);
	/* fall through */
    case 'r':
    case '.': /* same location */
      continue;
    case 'n':
    case 0: break; /* next location */
    default:
	tmp = getnum(c_ptr,16,(long *)&val);
      if(tmp == 0) {
	printf("bad value \"%s\"\n",c_ptr);
	continue; /* same location again */
      } else {
	switch(opsiz) {
	case BW_8BITS:
	  addr->byte = val;
	  break;
	case BW_16BITS:
	  addr->word = val;
	  break;
	case BW_32BITS:
	  addr->lword = val;
	  break;
	}
	c_ptr += tmp;
	if(*c_ptr == '.')
	    continue;  /* same location */
      }
      break; /* next location */
    }
    addr = (union location *)((unsigned)addr + opsiz);
    /* bump address */
  }
}

/*
** Call a routine at specified address (location) passing argc and argv
** to represent argv[2...].
** The value returned from the routine is returned to the caller.
*/
int
jump(int argc, char *argv[])
{
  int (*location)();

  if(argc < 2 || getnum(argv[1],16,(long *)&location) == 0) {
    printf("usage: %s location [arg1, arg2 ... argN]\n",argv[0]);
    return(1);
  }
  return((*location)(argc - 2, &argv[2]));
}

/*
** Similar to above except that argv[2...5] are converted to hex numbers
** before they are passed to the routine and the return value is printed
** on the console upon return;
*/
int
call(int argc, char *argv[])
{
  register char i;
  long arg[5];

  if(argc < 2 || argc > 6) {
usage:
    printf("usage: %s address [arg0 arg1 arg2 arg3]\n",argv[0]);
    return(1);
  }
  for(i=1; i < argc; i++)
    if(getnum(argv[i], 16, &arg[i-1]) == 0) {
      puts("all arguments must be ascii hex numbers\n");
      goto usage;
    }
  i--;
  while(i < 5) arg[i++] = 0;
  printf("*** return(0x%02x) ***\n",
	 (*(int (* )())arg[0])(arg[1],arg[2],arg[3],arg[4]));
  return(0);
}

/*
** memdebug - write a longword location with 0x55555555, write the next
** location with 0xaaaaaaaa, read with verify the first location, clear
** the two locations and do over continuously.
** Report a verify error.
** Bus slam test consists of writing zeroes into the locations then
** verify and update with 0xffffffff;  This makes the data bus slam
** between all zeroes and all ones. Report a verify error with a 
** trigger.
*/
int
memdebug(int argc, char *argv[])
{
    int i;
    register volatile unsigned long *locptr, *nlocptr;
    register unsigned long zero = 0;
    register unsigned long pat1 = 0x55555555;
    register unsigned long pat2 = 0xaaaaaaaa;
    char slam = 0;
    register volatile unsigned char *trigger, temp;
    static char optstr[] = "st:";
    extern int optind;
    extern char *optarg;

    if(argc == 1) {
	query_user( QU_START | QU_TRIGGER, &MEMOP_START, &MEMOP_TRIGGER);
    } else {
	while((i = getopt(argc,argv,optstr)) >= 0) {
	    switch(i) {
	    case 't':  /* specify a trigger */
		getnum(optarg,16,(long *)&MEMOP_TRIGGER);
		break;
	    case 's':  /* bus slam test */
		slam = 1;
		break;
	    default:
usage:
		printf("usage: %s [-%s] startaddr\n",argv[0]);
		puts("-t:  specify a scope trigger address\n"
		     "-s   bus slam test\n");
		return(1);
	    }
	}
	if((argc - optind) != 1 ||
	   getaddr(argv[optind++],&MEMOP_START) < 0) goto usage;
    }
    locptr = (volatile unsigned long *)MEMOP_START;
    nlocptr = locptr + 1;
    trigger = (volatile unsigned char *)MEMOP_TRIGGER;
    quitmsg();
    if(slam) {
	pat1 = 0xffffffff;
	pat2 = 0x0;
	*locptr = pat1;
	*nlocptr = pat1;
	while(1) {
	    if(*locptr != pat1) temp = *trigger;
	    *locptr = pat2;
	    if(*nlocptr != pat1) temp = *trigger;
	    *nlocptr = pat2;
	    if(*locptr != pat2) temp = *trigger;
	    *locptr = pat1;
	    if(*nlocptr != pat2) temp = *trigger;
	    *nlocptr = pat1;
	}
    } else {
	while(1) {
	    *locptr = zero;
	    *nlocptr = zero;
	    *locptr = pat1;
	    *nlocptr = pat2;
	    if(*locptr != pat1) {
		temp = *trigger;
		puts("\nerror\n");
	    }
	}
    }
}

int
memtest(int argc, char *argv[])
{
    register i;
    struct testmem tmem;
    register struct testmem *tmemp = &tmem;
    static char optstr[] = "qmp:r:";
    extern int optind;
    extern char *optarg;

    tmemp->flag = 0;
    tmemp->passcount = 0;
    if(argc == 1) {
	query_user( QU_START | QU_SIZE | QU_PASSES,
		   &MEMOP_START, &MEMOP_LENGTH, &tmemp->passcount);
    } else {
	while((i = getopt(argc,argv,optstr)) >= 0) {
	    switch(i) {
	    case 'q':  /* quit on error */
		tmemp->flag |= QUITONERR;
		break;
	    case 'm':  /* mute */
		tmemp->flag |= MUTE;
		break;
	    case 'p':  /* specified pass count */
		getnum(optarg,16,&tmemp->passcount);
		break;
	    case 'r':
		getnum(optarg,16,(long *)&tmemp->rdaddr);
		tmemp->flag |= RDADDR;
		break;
	    default:
usage:
		printf("usage: %s [-%s] startaddr length\n",argv[0],optstr);
		puts("\
-q  quit on error\n\
-m  mute operation\n\
-p:  specify pass count in decimal\n\
-r:  specify read address separately\n");
		return(1);
	    }
	}
	if((argc - optind) != 2 ||
	   getaddr(argv[optind++],&MEMOP_START) < 0 ||
	   getnum(argv[optind],16,(long *)&MEMOP_LENGTH) == 0) goto usage;
    }
    tmemp->start = (unsigned *)MEMOP_START;
    tmemp->length = MEMOP_LENGTH;
    if(tmemp->length < 8) {
	printf("length must be at least 8\n");
	if(argc == 1) return(1);
	else goto usage;
    }
    testpass = 1;
    switch(tmemp->passcount) {
    case 0:
	quitmsg();  /* run until user escapes */
	break;
    case 1:
	testpass = 0;
	break;
    }
    testname("memory");
    if(testmem(tmemp) < 0) return(2);
    return(0);
}

int
memloop(int argc, char *argv[])
{
    register volatile unsigned *addr;
    register unsigned value;
    unsigned rw = 'r';  /* init */
    
    if(argc == 1) {
	query_user( QU_START | QU_R_WR | QU_VALUE | QU_OPSIZ,
		   &MEMOP_START, &rw, &MEMOP_VALUE,
		   &MEMOP_SIZE);
    } else {
	if(getopsize(argc,argv) < 0 ||
	   getaddr(argv[optind++],&MEMOP_START) < 0) {
usage:
	    printf("usage: %s [-%s] addr [writedata]\n",argv[0],optstr);
	    return(1);
	}
	switch(argc - optind) {
	case 0: break;  /* no more arguments */
	case 1:
	    if(getnum(argv[optind],16,(long *)&MEMOP_VALUE) == 0) goto usage;
	    rw = 'w';
	    break;
	default:
	    goto usage;
	}
    }
    quitmsg();
    addr = (unsigned *)MEMOP_START;
    value = MEMOP_VALUE;
    switch(MEMOP_SIZE) {
    case BW_8BITS:  /* byte operation */
	if(rw == 'w') {
	    while(1) *(volatile unsigned char *)addr = value;
	} else while(1) value = *(volatile unsigned char *)addr;
	break;
    case BW_16BITS:  /* word operation */
	if(rw == 'w') {
	    while(1) *(volatile unsigned short *)addr = value;
	} else while(1) value = *(volatile unsigned short *)addr;
	break;
    case BW_32BITS:  /* longword operation */
	if(rw == 'w') {
	    while(1) *addr = value;
	} else while(1) value = *addr;
	break;
    }
    return(0);
}

void
addrtest(unsigned char *addr, int length)
{
  register offset = 1;
  register unsigned char x;
  register volatile unsigned char *ptr;
  register unsigned char *end = addr + length;

  /* start with even addr (offset will make it odd) */
  if((int)addr & 1) addr--;
  while(1) {
    ptr = addr + offset;  /* add in our offset */
    if(ptr >= end) break;  /* we exceed our bound */
    x = *ptr;             /* read the address (non-destructive) */
    offset <<= 1;
  }
}

int
addrloop(int argc, char *argv[])
{
    if(argc == 1) {
	query_user(QU_START | QU_SIZE,
		   &MEMOP_START, &MEMOP_LENGTH);
    } else {
	if(getopsize(argc,argv) < 0 ||  /* done for initialization only? */
	   getaddr(argv[optind++],&MEMOP_START) < 0 ||
	   getnum(argv[optind++],16,(long *)&MEMOP_LENGTH) == 0 ||
	   optind != argc) {
usage:
	    printf("usage: %s addr length\n",argv[0]);
	    return(1);
	}
    }
    while(1) addrtest((unsigned char *)MEMOP_START, MEMOP_LENGTH);
}


/*
** Scan the specified range of addresses, byte by byte, and report the
** addresses that cause bus errors.
** The variable prev_acc_stat (previous access status) is used to keep
** state as follows:
**    -1 = initial value to signify first time through loop
**     0 = previous access did not cause a bus error
**     1 = previous access caused a bus error
*/
int
berrscan(int argc, char *argv[])
{
    char prev_acc_stat = -1;  /* init */
    SIG_PF savfcn;
    static char *berr_scan_msg = "bus error from location";
    register volatile unsigned char *addr, *end;
    unsigned char temp;
    char buffer[120];
    char *bptr;

    if(argc == 1) {
	query_user( QU_START | QU_SIZE ,
		   &MEMOP_START, &MEMOP_LENGTH);
    } else {
	if(getaddr(argv[++optind],&MEMOP_START) < 0 ||
	   getnum(argv[++optind],16,(long *)&MEMOP_LENGTH) == 0 || ++optind != argc) {
	    printf("usage: %s start length\n",argv[0]);
	    return(1);
	}
    }
    addr = (unsigned char *)MEMOP_START;
    end = addr + MEMOP_LENGTH;
    printf("scanning from %#x to %#x with logging\n", addr, end);
    hkeepflags &= ~H_BUSERR;  /* initialize flag */
    savfcn = signal(SIGBUS,SIG_IGN);  /* do not report bus errors to console */
    bptr = buffer;
    for(; addr < end; addr++) {
	temp = *addr;  /* read the address */
        BUSERR_NOP_X4();
	if(hkeepflags & H_BUSERR) {  /* this access caused an error */
	    hkeepflags &= ~H_BUSERR;  /* clear flag */
	    if(prev_acc_stat != 1) {   /* previous access did not cause a bus error */
		if(prev_acc_stat == 0) {  /* not first time through loop */
		    sprintf(bptr, "%#x\n", addr-1);
		    logprintf(buffer);
		    bptr = buffer;
		}
		bptr += sprintf(bptr, "%s %#x to ", berr_scan_msg, addr);
		prev_acc_stat = 1;  /* set for next access */
	    }
	} else {  /* this access did not cause an error */
	    if(prev_acc_stat != 0) {  /* previous access caused an error */
		if(prev_acc_stat > 0) {  /* not first time through loop */
		    bptr += sprintf(bptr, "%#x\n", addr-1);
		}
		bptr += sprintf(bptr, "no %s %#x to ", berr_scan_msg, addr);
		prev_acc_stat = 0;  /* set for next access */
	    }
	}
    }
    sprintf(bptr, "%#x\n", addr-1);
    logprintf(buffer);
    signal(SIGBUS,savfcn);
    return(0);
}

#if !defined(RSP) && !defined(RSP2) && !defined(PREDATOR) && !defined(SVIP)
/*
** Test parity on the user specified range.
** Longword aligned main and shared memory addresses are valid.
*/
int
paritytest(int argc, char *argv[])
{
    unsigned long location, end;
    int temp;

    if(argc == 1) {
	query_user( QU_START | QU_SIZE,
		   &MEMOP_START, &MEMOP_LENGTH);
    } else {
	if(getaddr(argv[++optind],&MEMOP_START) < 0 ||
	   getaddr(argv[++optind],&MEMOP_LENGTH) < 0 || ++optind != argc) {
	    printf("usage: %s addr length\n",argv[0]);
	    return(1);
	}
    }

    location = MEMOP_START;
    end = location + MEMOP_LENGTH;
    for( ; location < end; location += 4) {
	switch(test_parity(location, 1)) {
	case 0: break;
	case 2:
	    if(memsize == 0 || shmemsize == 0)
		printf("size memory before running\n");
	default:
	    return(1);
	}
    }
    return(0);
}
#endif

void quitmsg (void)
{
    printf("enter <break> to quit\n");
}

/*
** A universal checksum algorithm
*/
unsigned int
u_chksum(unsigned char *addr, int size)
{
    unsigned char *end;
    unsigned int chksum;

    end = addr + size;

    chksum = 0; /* init */
    for(; addr<end; addr++) {
        if(chksum & 01)
            chksum = (chksum >> 1) + 0x8000;
        else
            chksum >>= 1;
        chksum += *addr;
        chksum &= 0xffff;
    }
    return(chksum);
}

int
memory_checksum(argc, argv)
    char *argv[];
{
    unsigned long addr, size;

    if(argc != 3) {
usage:
	printf("usage: %s <start address> <size> \n",argv[0]);
	return(1);
    }

    getnum(argv[1],16,(long *)&addr);
    getnum(argv[2],16,(long *)&size);   

    printf("\nChecksum is %#x\n",
	   u_chksum((unsigned char *)addr, size));
    return(0);
}

/* end of module */
