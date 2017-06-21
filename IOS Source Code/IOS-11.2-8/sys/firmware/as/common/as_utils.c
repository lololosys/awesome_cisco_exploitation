/* $Id: utils.c,v 1.2 1996/05/09 21:37:12 towu Exp $
 * $Source: /cscka/t1_neat/src/common/utils.c,v $
 *------------------------------------------------------------------
 * One line description here.
 *
 * April, 1996 Tony Wu
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: utils.c,v $
 * Revision 1.2  1996/05/09 21:37:12  towu
 * Added new features to support printf and buginfo.
 *
 * Revision 1.1  1996/04/21  00:11:39  towu
 * Created to support Channelized T1 signaling.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef T1_FIRMWARE
#include 	"t1_headers.h"
#else
#include 	"e1_headers.h"
#endif
#include	"as_ipc_section.h"

volatile unsigned char hkeepflags;
volatile unsigned berr_addr;
unsigned short mstat_snapshot;
static stack_trace_info_t stack_sav[NUM_STACK_FRAMES];
cpu_context_t *entry_context;

typedef char		*va_list;

#define __va_rounded_size(TYPE)                                         \
    (((sizeof (TYPE) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))

#define va_start(AP, LASTARG) \
   (AP = ((char *) &(LASTARG) + __va_rounded_size (LASTARG)))

#define va_end(AP)

#define va_arg(AP, TYPE)  \
 (AP += __va_rounded_size (TYPE),    \
   *((TYPE *) (AP - ((sizeof(TYPE) < 4)  \
		       ? sizeof(TYPE)  \
		       : __va_rounded_size (TYPE)))))
static void doprintc (
    char **cp,
    char ch)
{
    if (*cp)                           /* if writing to a string */
	*(*cp)++ = ch;                  /* don't do any formatting */
    return;
}

/*
 * _doprnt - master printf routine.
 * The conversions %o, %x, %d, %u, %s, %c, and %% are implemented.
 * The -, 0, *, and # options are supported for numbers only.
 * The <width> option is supported for numbers, strings, and characters.
 * The number of characters printed is returned.
 *
 * E/e prints a dotted 48 bit Ethernet address. Takes a string pointer.
 *
 * T/t prints a terminal line number in octal or decimal.  Integer arg.
 *
 */

static int _doprnt (char *cp, char *fmt, va_list argp, bool do_precision)
{
    uchar c, *q;
    int radix, d, length;
    int charcount;
    int digitcount;
    ulong n;
    uchar digits[25];
    int hash,rjust,width,size,temp,plus,precision;
    char fill;
    bool spaces;
    bool is_negative;


    charcount = 0;			/* nothing printed yet */
    while ((c = *fmt++)) {		/* work until end of string */
	if (c != '%') {
	    charcount++;
	    doprintc(&cp, c);	/* output normal characters */
	} else {
	    width = -1;			/* default width is unspecified */
	    length = 0;			/* no length yet */
	    fill = ' ';			/* default fill is space */
	    rjust = 0;			/* default is left justified */
	    hash = 0;			/* default is no leading 0 or 0x */
	    size = 0;			/* default is that args are short */
	    plus = 0;			/* default is no plus sign */
	    precision = -1;              /* no precision yet */
	    if (*fmt == '-') {
		rjust++;		/* want right justified */
		fmt++;
	    }
	    if (*fmt == '+') {
		plus++;			/* add a plus if positive integer */
		fmt++;
	    }
	    if (*fmt == '0') {
		fill = '0';		/* set fill char to '0' */
		fmt++;
	    }
	    if (*fmt == '#') {
		hash++;			/* want leading 0 or 0x */
		fmt++;
	    }
	    if (*fmt == '*') { 		/* look to args for width */
		width = va_arg(argp, int);
		fmt++;
	    } else			/* width possibly specified */
		while ((*fmt >= '0') && (*fmt <= '9')) {
		    if (width == -1)
			width = 0;
		    width = width*10 + (*fmt++ - '0');
		}

	    if (do_precision) {          /* Check for precision */
		if (*fmt == '.') { /* precision possibly specified */
		    fmt++;
		    precision = 0;
		    if (*fmt == '*') { /* look to args for precision */
			precision = va_arg(argp, int);
			if (precision < 0) /* check for nonsense precision */
			    precision = 0;
			fmt++;
		    } else
			while ((*fmt >= '0') && (*fmt <= '9'))
			    precision = precision*10 + (*fmt++ - '0');
		}
	    } else
		precision = width;      /* cisco convention derives */
				        /* precision from width */

 	    if (*fmt == 'l') {		/* use Longs instead of shorts */
 		size++;
 		fmt++;
 	    }
 	    if (*fmt == 'l') {		/* use Long Longs instead of longs */
 		size++;
 		fmt++;
 	    }

	    if (!*fmt)
	      break;		/* Check for trailing nulls - bad format */

	    switch (*fmt++) {		/* get char after the %, switch */


		case 'o':		 /* octal */
		case 'O': 
		    radix = 8;
		    if (precision == -1)
			precision = 1;
		    if (size) {
			n = va_arg(argp, ulong);
		    } else {
			n = va_arg(argp, uint);
		    }
		    if (hash && (n > 0)) {
			doprintc(&cp,'0');
			length++;
			charcount++;
		    }
		    goto compute;

	      case 'x':		 /* hexadecimal */
	      case 'X': 
		radix = 16;
		if (precision == -1)
		    precision = 1;
		if (size) {
		    n = va_arg(argp, ulong);
		} else {
		    n = va_arg(argp, uint);
		}
		if (hash) {
		    doprintc(&cp,'0');
		    doprintc(&cp,'x');
		    length += 2;
		    charcount += 2;
		}
		goto compute;
		
	    case 'u':		/* unsigned decimal */
	    case 'U':
	    	radix = 10;
		if (precision == -1)
		    precision = 1;
		if (size) {
		    n = va_arg(argp, ulong);
		} else {
		    n = va_arg(argp, uint);
		}
		goto compute;
		
	    case 'd': 		 /* signed decimal */
	    case 'D': 
	        radix = 10;
		if (precision == -1)
		    precision = 1;
		if (size) {
		    n = va_arg(argp, long);
		    is_negative = ((long)n < 0);
		} else {
		    n = va_arg(argp, int);
		    /*
		     * For backward compatibility reasons, this test for a
		     * negative number is based upon a 'long', not an 'int'.
		     */
		    is_negative = ((long)n < 0);
		}
		if (is_negative) {
		    doprintc(&cp, '-');
		    length++; charcount++;
		    n = -n;
		} else if (plus) {
		    doprintc(&cp, '+');
		    length++; charcount++;
		}
	    compute: 
		digitcount = 0;
		if (n == 0) {
		    q = &digits[0];
		    if (!do_precision) {
			*q++ = '0';
			length++;
		    }			
		} else
		    for (q = &digits[0]; n != 0; n = n / radix) {
			d = n % radix;  length++; digitcount++;
			*q++ = d + (d < 10 ? '0' : '7');
		    }
		if (do_precision && (precision > 0)
		    && (digitcount < precision))
		    for (d = digitcount; d < precision; d++) {
			*q++ = '0';
			length++;
		    }
		q--;		/* back up the pointer a bit */
		if ((rjust || fill == '0') && width != -1 && (length < width))
		    for (d = 0; d < (width - length); d++) {
			doprintc(&cp,fill);	/* leading fill */
			charcount++;
		    }
		while (q >= &digits[0]) {
		    doprintc(&cp, *q--);
		    charcount++;
		}
		if (!rjust && fill != '0' && width != -1 && (length < width))
		    for (d = 0; d < (width-length); d++) {
			doprintc(&cp,' '); /* trailing fill - spaces*/
			charcount++;
		    }
		break;
		
	    case 'c':			 /* character */
		if (rjust && width != -1)
		    for (d = 0; d < width-1; d++) {
			doprintc(&cp,' '); /* leading fill - spaces */
			charcount++;
		    }
		temp = va_arg(argp, int);
		doprintc(&cp,temp);
		charcount++;
		if (!rjust && width != -1)
		    for (d = 0; d < width-1; d++) {
			doprintc(&cp,' '); /* trailing fill - spaces*/
			charcount++;
		    }
		break;
		
	    case 's': 			 /* string */ 
	    case 'S': 			 /* string parsed by STRING macro */ 
		q = va_arg(argp, uchar *);
		if (q == NULL)		/* insure proper formatting */
		    q = "";
		spaces = FALSE;		/* No spaces */
		temp = (int) q;   	/* stash copy of pointer */
		while (*q) {
		    if (*(fmt-1) == 'S') {
			if (*q == ' ') {
			    spaces = TRUE;  /* Contains spaces */
			} else if (*q == '"') {
			    length++;       /* Escape quotes */
			}
		    }
		    ++q;
		    ++length;	/* figure its length */
		}
		if (spaces) {
		    length += 2;
		}
		if (rjust && (width != -1) && (length < width)) {
		    for (d = 0; d < (width - length); d++) {
			doprintc(&cp,fill); /* leading fill */
			charcount++;
		    }
		}
		if (spaces) {
		    doprintc(&cp,  '"');
		    charcount++;
		}
		q = (uchar *) temp;	/* get pointer again */
		while (*q != 0) {
		    if ((precision != -1) &&
			((int)(q - (uchar *)temp) >= precision)) {
			break;
		    }
		    if ((*(fmt-1) == 'S') && (*q == '"')) {
			doprintc(&cp, '\\');
			charcount++;
		    }
		    doprintc(&cp, *q++);
		    charcount++;
		}
		if (spaces) {
		    doprintc(&cp,  '"');
		    charcount++;
		}
		if (!rjust && (width != -1) && (length < width)) {
		    for (d = 0; d < (width-length); d++) {
			doprintc(&cp,' ');/* trailing fill - spaces */
			charcount++;
		    }
		}
		break;
		
	    case 'r':			/* 'raw' display string */
		q = va_arg(argp, uchar *); /* point to string */
		if (q == NULL)		/* check for null pointer */
		    break;
		while (*q != 0) {		/* field widths don't make */
		    if (cp)			/* any sense for displays */
		      *cp++ = *q;
		    charcount++;
		    q++;
		}
		break;
		
	    case '%': 			 /* percent sign */ 
		doprintc(&cp, '%');
		charcount++;
		break;

	    default:
		break;
	    }
	}
    }
    return(charcount);
}

/*
 * sprintf - Ported from IOS.
 * String version of printf.
 * NOTE:  It does not support type B/b, E/e.
 */

int sprintf (char *cp, char *fmt, ...)
{
    va_list ap;
    int count;

    va_start(ap, fmt);
    count = _doprnt(cp, fmt, ap, FALSE);
    va_end(ap);
    cp[count] = '\0';
    return(count);
}
/*
 * strlen
 * Return string length
 */
    
size_t strlen (const char *s)
{
    const char *ptr = s;
    if (ptr == NULL)
         return(0);
    
    while (*ptr++);
    return(ptr - s - 1);
}



/*
 * neat_status_report - Ported from IOS.
 * send an message to mpu and will be used with errmsg.
 * NOTE:  It does not support type B/b, E/e.
 */

void neat_status_report (char *fmt, ...)
{
    char cp[128];
    va_list ap;
    int count;

    va_start(ap, fmt);
    count = _doprnt(cp, fmt, ap, FALSE);
    va_end(ap);
    cp[count] = '\0';
    neat_ipc_message_send(MSG_NEAT_ERROR_REPORT, cp, count+1);
}

void buginf (char *fmt, ...)
{
    char cp[128];
    va_list ap;
    int count;

    va_start(ap, fmt);
    count = _doprnt(cp, fmt, ap, FALSE);
    va_end(ap);
    cp[count] = '\0';
    neat_ipc_message_send(MSG_NEAT_BUGINFO, cp, count+1);
}

void save_stack_trace(unsigned long *fp, stack_trace_info_t *stip,
                int numframes)
{
    int i;

    /* process requested number of frames */
    for(i=0; i < numframes; stip++, i++) {
        if(fp) {
            stip->ra = *(fp + 1);
            stip->fp = fp;
            buginf("ra = %#x  fp = %#x", stip->ra, stip->fp);
            fp = (unsigned long *)*fp;
        } else {
            fp = (unsigned long *)0;
            stip->ra = 0;
            stip->fp = fp;
        }
    }
}

#define DUMP_1REG(register)   buginf(#register " = %x", ccp->register);
#define DUMP_2REG(reg1, reg2) buginf(#reg1 " = %x, " #reg2 "= %x", ccp->reg1, ccp->reg2);

void exception(unsigned long stack_frame)
{
    cpu_context_t *ccp = (cpu_context_t *)&stack_frame;
    struct excpt_stack_frame *esfptr = &ccp->esf;
    int exceptnum = (esfptr->offset >> 2);
    unsigned long pc = esfptr->pc;

    if (exceptnum == 2)
      buginf("Bus error occurred on Neat processor !!!");
    else
      buginf("Bad Exception Vector occurred on Neat processor !!!");

    buginf("------ Begin Neat Crash Dump ------");
    buginf("vector number is %d", exceptnum);
    buginf("program counter = %x", pc);
    buginf("status register = %x", esfptr->sr);

    DUMP_2REG(d0, d1);
    DUMP_2REG(d2, d3);
    DUMP_2REG(d4, d5);
    DUMP_2REG(d6, d7);

    DUMP_2REG(a0, a1);
    DUMP_2REG(a2, a3);
    DUMP_2REG(a4, a5);
    DUMP_2REG(a6, a7);

    /* and save our stack trace */
    save_stack_trace((ulong *)ccp->a6, stack_sav, NUM_STACK_FRAMES);
    buginf("------ End of Neat Crash Dump^G ------");

    {
        /*
         * delay some time for the host to process the debug info
         * before the neat processor gets reset ...
         */
      int ix;
      for(ix=0; ix < 1000000; ix++) {
        asm("nop"); asm("nop"); asm("nop"); asm("nop");
      }
    }
}

/*
 * Return the current value of the status register
 */
void stack_dump(char *debug_text)
{
    ulong retval;

    buginf("------ Begin Neat Stack Dump ------");
    asm ("movl     a6,%0" : "=hd" (retval));
    if (debug_text)
        buginf("%s", debug_text);
    save_stack_trace((ulong *)retval, stack_sav, NUM_STACK_FRAMES);
    buginf("------ End of Neat Stack Dump^G ------");
}

/*
 * Set the interrupt level to the specified value, and return the
 * current contents of the status register.
 */
ushort set_ipl(register ushort newlevel)
{
    ushort retval;

    asm ("movw     sr,%0" : "=hd" (retval));
    newlevel = IPL_TO_SR(newlevel) | (retval & ~SR_IPL);
    asm ("movw     %0,sr" : /* no outputs */ : "d" (newlevel));

    return retval;
}

void hara_kiri_mesg(void)
{
   buginf("\nfirmware restart due to hara_kiri");
}

