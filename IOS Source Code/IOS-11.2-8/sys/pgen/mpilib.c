/* $Id: mpilib.c,v 3.2.60.1 1996/03/18 21:35:12 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/pgen/mpilib.c,v $
 *------------------------------------------------------------------
 * M P I L I B . C
 *
 * Multiple precision integer arithmetic. This code was shamelessly ripped
 * off from PGP 2.0. The author has given the world permission to use the
 * code, and even to distribute derivative works, so this should be OK for
 * internal use. If we were ever to ship PaGenT with a product, we'd have
 * to take this out; it's far more library than PaGenT needs anyway.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mpilib.c,v $
 * Revision 3.2.60.1  1996/03/18  21:35:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2  1995/11/17  18:56:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:59:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:34:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/* Cisco-ization of this library uses some things we need to get other files */
#include "master.h"

/*	C source code for multiprecision arithmetic library routines.
	Implemented Nov 86 by Philip Zimmermann
	Last revised 27 Nov 91 by PRZ

	Boulder Software Engineering
	3021 Eleventh Street
	Boulder, CO 80304
	(303) 541-0140

	(c) Copyright 1986-92 by Philip Zimmermann.  All rights reserved.
	The author assumes no liability for damages resulting from the use 
	of this software, even if the damage results from defects in this 
	software.  No warranty is expressed or implied.  The use of this 
	cryptographic software for developing weapon systems is expressly 
	forbidden.

	These routines implement all of the multiprecision arithmetic 
	necessary for number-theoretic cryptographic algorithms such as 
	ElGamal, Diffie-Hellman, Rabin, or factoring studies for large 
	composite numbers, as well as Rivest-Shamir-Adleman (RSA) public 
	key cryptography.

	Although originally developed in Microsoft C for the IBM PC, this code 
	contains few machine dependancies.  It assumes 2's complement 
	arithmetic.  It can be adapted to 8-bit, 16-bit, or 32-bit machines, 
	lowbyte-highbyte order or highbyte-lowbyte order.  This version
	has been converted to ANSI C.


	The internal representation for these extended precision integer
	"registers" is an array of "units".  A unit is a machine word, which
	is either an 8-bit byte, a 16-bit unsigned integer, or a 32-bit
	unsigned integer, depending on the machine's word size.  For example,
	an IBM PC or AT uses a unit size of 16 bits.  To perform arithmetic
	on these huge precision integers, we pass pointers to these unit
	arrays to various subroutines.  A pointer to an array of units is of
	type unitptr.  This is a pointer to a huge integer "register".

	When calling a subroutine, we always pass a pointer to the BEGINNING
	of the array of units, regardless of the byte order of the machine.
	On a lowbyte-first machine, such as the Intel 80x86, this unitptr
	points to the LEAST significant unit, and the array of units increases
	significance to the right.  On a highbyte-first machine, such as the
	Motorola 680x0, this unitptr points to the MOST significant unit, and
	the array of units decreases significance to the right.

	Modified 8 Apr 92 - HAJK
	Implement new VAX/VMS primitive support.

*/

/* #define COUNTMULTS */ /* count modmults for performance studies */

#ifdef DEBUG
#ifdef MSDOS
#include <conio.h>
#define poll_for_break() {while (kbhit()) getch();}
#endif	/* MSDOS */
#endif	/* DEBUG */

#ifndef poll_for_break
#define poll_for_break()  /* stub */
#endif

#include "mpilib.h"

short global_precision = 0; /* units of precision for all routines */
/*	global_precision is the unit precision last set by set_precision.
	Initially, set_precision() should be called to define global_precision
	before using any of these other multiprecision library routines.
	i.e.:   set_precision(MAX_UNIT_PRECISION);
*/

#ifdef PORTABLE
/*************** multiprecision library primitives ****************/
/*	The following portable C primitives should be recoded in assembly.
	The equivalent assembly primitives are externally defined under
	different names, unless PORTABLE is defined.  See the header file
	"mpilib.h" for further details.

	The carry bit mechanism we use for these portable primitives
	will only work if the size of unit is smaller than the size of
	a long integer.  In most cases, this means UNITSIZE must be
	less than 32 bits -- if you use the C portable primitives.
*/

typedef unsigned long int ulint;
#define carrybit ((ulint) 1 << UNITSIZE)
/* ...assumes sizeof(unit) < sizeof(unsigned long) */

boolean mp_addc
	(register unitptr r1,register unitptr r2,register boolean carry)
	/* multiprecision add with carry r2 to r1, result in r1 */
	/* carry is incoming carry flag-- value should be 0 or 1 */
{	register ulint x;	/* won't work if sizeof(unit)==sizeof(long) */
	short precision;	/* number of units to add */
	precision = global_precision;
	make_lsbptr(r1,precision);
	make_lsbptr(r2,precision);
	while (precision--)	
	{	x = (ulint) *r1 + (ulint) *post_higherunit(r2) + (ulint) carry;
		*post_higherunit(r1) = x;
		carry = ((x & carrybit) != 0L);
	}
	return(carry);		/* return the final carry flag bit */
}	/* mp_addc */


boolean mp_subb
	(register unitptr r1,register unitptr r2,register boolean borrow)
	/* multiprecision subtract with borrow, r2 from r1, result in r1 */
	/* borrow is incoming borrow flag-- value should be 0 or 1 */
{	register ulint x;	/* won't work if sizeof(unit)==sizeof(long) */
	short precision;	/* number of units to subtract */
	precision = global_precision;
	make_lsbptr(r1,precision);
	make_lsbptr(r2,precision);
	while (precision--)	
	{	x = (ulint) *r1 - (ulint) *post_higherunit(r2) - (ulint) borrow;
		*post_higherunit(r1) = x;
		borrow = ((x & carrybit) != 0L);
	}
	return(borrow);		/* return the final carry/borrow flag bit */
}	/* mp_subb */

#undef carrybit


boolean mp_rotate_left(register unitptr r1,register boolean carry)
	/* multiprecision rotate left 1 bit with carry, result in r1. */
	/* carry is incoming carry flag-- value should be 0 or 1 */
{	register short precision;	/* number of units to rotate */
	register boolean nextcarry = FALSE;
	precision = global_precision;
	make_lsbptr(r1,precision);
	do {	
		nextcarry = (((signedunit) *r1) < 0);
		/* nextcarry = ((*r1 & uppermostbit) != 0); */
		*r1 <<= 1 ;
		if (carry) *r1 |= 1;
		carry = nextcarry;
		pre_higherunit(r1);
	} while (precision--);
	return(nextcarry);	/* return the final carry flag bit */
}	/* mp_rotate_left */

/************** end of primitives that should be in assembly *************/
#endif	/* PORTABLE */


/* 	The mp_shift_right_bits function is not called in any time-critical
	situations in public-key cryptographic functions, so it doesn't 
	need to be coded in assembly language.
*/
void mp_shift_right_bits(register unitptr r1,register short bits)
	/*	multiprecision shift right bits, result in r1. 
		bits is how many bits to shift, must be < UNITSIZE.
	*/
{	register short precision;	/* number of units to shift */
	register unit carry,nextcarry,bitmask;
	register short unbits;
	if (bits==0) return;	/* shift zero bits is a no-op */
	carry = 0;
	bitmask = power_of_2(bits)-1;
	unbits = UNITSIZE-bits;		/* shift bits must be < UNITSIZE */
	precision = global_precision;
	make_msbptr(r1,precision);
	while (precision--)	
	{	nextcarry = *r1 & bitmask;
		*r1 >>= bits;
		*r1 |= carry << unbits;
		carry = nextcarry;
		pre_lowerunit(r1);
	}
}	/* mp_shift_right_bits */


#ifndef VMS

short mp_compare(register unitptr r1,register unitptr r2)
/*	Compares multiprecision integers *r1, *r2, and returns:	
	-1 iff *r1 < *r2
	 0 iff *r1 == *r2
	+1 iff *r1 > *r2
*/
{	register short precision;	/* number of units to compare */

	precision = global_precision;
	make_msbptr(r1,precision);
	make_msbptr(r2,precision);
	do	
	{	if (*r1 < *r2) 
			return(-1);
		if (*post_lowerunit(r1) > *post_lowerunit(r2)) 
			return(1);
	} while (--precision);	
	return(0);	/*  *r1 == *r2  */
}	/* mp_compare */

#endif /* VMS */

boolean mp_inc(register unitptr r)
	/* Increment multiprecision integer r. */
{	register short precision;
	precision = global_precision;
	make_lsbptr(r,precision);
	do	
	{	if ( ++(*r) ) return(0);	/* no carry */
		post_higherunit(r);	
	} while (--precision);
	return(1);		/* return carry set */
}	/* mp_inc */


boolean mp_dec(register unitptr r)
	/* Decrement multiprecision integer r. */
{	register short precision;
	precision = global_precision;
	make_lsbptr(r,precision);
	do	
	{	if ( (signedunit) (--(*r)) != (signedunit) -1 )
			return(0);	/* no borrow */
		post_higherunit(r);	
	} while (--precision);
	return(1);		/* return borrow set */
}	/* mp_dec */


void mp_neg(register unitptr r)
	/* Compute 2's complement, the arithmetic negative, of r */
{	register short precision;	/* number of units to negate */
	precision = global_precision;
	mp_dec(r);	/* 2's complement is 1's complement plus 1 */
	do	/* now do 1's complement */
	{	*r = ~(*r);
		r++;	
	} while (--precision);
}	/* mp_neg */

#ifndef VAXC /* Replaced by a macro under VAX C */

void mp_move(register unitptr dst,register unitptr src)
{	register short precision;	/* number of units to move */
	precision = global_precision;
	do { *dst++ = *src++; } while (--precision);
}	/* mp_move */

#endif /* VAXC */
void mp_init(register unitptr r, word16 value)
	/* Init multiprecision register r with short value. */
{	/* Note that mp_init doesn't extend sign bit for >32767 */
	register short precision;	/* number of units to init */
	precision = global_precision;
	make_lsbptr(r,precision);
	*post_higherunit(r) = value; precision--;
#ifdef UNIT8
	*post_higherunit(r) = value >> UNITSIZE; precision--;
#endif
#ifdef VAXC

	unitfill0( r, precision); /* Use character insts. on a VAX */

#else /* VAXC */

	while (precision--)
		*post_higherunit(r) = 0;

#endif /* VAXC */

}	/* mp_init */

void mp_initl(register unitptr r, word32 value)
	/* Init multiprecision register r with long value. */
{	/* Note that mp_init doesn't extend sign bit for negatives */
	register short precision;	/* number of units to init */
	precision = global_precision;
	make_lsbptr(r,precision);
	*post_higherunit(r) = value; precision--;
#ifdef UNIT16
	*post_higherunit(r) = value >> UNITSIZE; precision--;
#endif
#ifdef UNIT8
mp_initl doesn't work for UNIT8, so let's not let the program compile.
#endif
#ifdef VAXC

	unitfill0( r, precision); /* Use character insts. on a VAX */

#else /* VAXC */

	while (precision--)
		*post_higherunit(r) = 0;

#endif /* VAXC */

}	/* mp_init */


short significance(register unitptr r)
	/* Returns number of significant units in r */
{	register short precision;
	precision = global_precision;
	make_msbptr(r,precision);
	do	
	{	if (*post_lowerunit(r)) 
			return(precision);
	} while (--precision);
	return(precision);
}	/* significance */


#ifndef VAXC	/* Replaced by a macro under VAX C */

void unitfill0(unitptr r,word16 unitcount)
	/* Zero-fill the unit buffer r. */
{	while (unitcount--) *r++ = 0;
}	/* unitfill0 */

#endif /* VAXC */

/* The macro normalize(r,precision) "normalizes" a multiprecision integer 
   by adjusting r and precision to new values.  For Motorola-style processors 
   (MSB-first), r is a pointer to the MSB of the register, and must
   be adjusted to point to the first nonzero unit.  For Intel/VAX-style
   (LSB-first) processors, r is a  pointer to the LSB of the register,
   and must be left unchanged.  The precision counter is always adjusted,
   regardless of processor type.  In the case of precision = 0,
   r becomes undefined.
*/


/* The macro rescale(r,current_precision,new_precision) rescales
   a multiprecision integer by adjusting r and its precision to new values.  
   It can be used to reverse the effects of the normalize 
   routine given above.  See the comments in normalize concerning 
   Intel vs. Motorola LSB/MSB conventions.
   WARNING:  You can only safely call rescale on registers that 
   you have previously normalized with the above normalize routine, 
   or are known to be big enough for the new precision.  You may
   specify a new precision that is smaller than the current precision.
   You must be careful not to specify a new_precision value that is 
   too big, or which adjusts the r pointer out of range.
*/


/*	The "bit sniffer" macros all begin sniffing at the MSB.
	They are used internally by all the various multiply, divide, 
	modulo, exponentiation, and square root functions.
*/


int mp_udiv(register unitptr remainder,register unitptr quotient,
	register unitptr dividend,register unitptr divisor)
	/* Unsigned divide, treats both operands as positive. */
{	int bits;
	short dprec;
	register unit bitmask;
	if (testeq(divisor,0))
		return(-1);	/* zero divisor means divide error */
	mp_init0(remainder);
	mp_init0(quotient);
	/* normalize and compute number of bits in dividend first */
	init_bitsniffer(dividend,bitmask,dprec,bits);
	/* rescale quotient to same precision (dprec) as dividend */
	rescale(quotient,global_precision,dprec);
	make_msbptr(quotient,dprec); 

	while (bits--)
	{	mp_rotate_left(remainder,(boolean)(sniff_bit(dividend,bitmask)!=0));
		if (mp_compare(remainder,divisor) >= 0)
		{	mp_sub(remainder,divisor);
			stuff_bit(quotient,bitmask);
		}
		bump_2bitsniffers(dividend,quotient,bitmask); 
	}
	return(0);
} /* mp_udiv */


#define RECIPMARGIN 0	/* extra margin bits used by mp_recip() */

int mp_recip(register unitptr quotient,register unitptr divisor)
	/* Compute reciprocal (quotient) as 1/divisor.  Used by faster modmult. */
{	int bits;
	short qprec;
	register unit bitmask;
	unit remainder[MAX_UNIT_PRECISION];
	if (testeq(divisor,0))
		return(-1);	/* zero divisor means divide error */
	mp_init0(remainder);
	mp_init0(quotient);

	/* normalize and compute number of bits in quotient first */
	bits = countbits(divisor) + RECIPMARGIN;
	bitmask = bitmsk(bits);		/* bitmask within a single unit */
	qprec = bits2units(bits+1);
	mp_setbit(remainder,(bits-RECIPMARGIN)-1);
	/* rescale quotient to precision of divisor + RECIPMARGIN bits */
	rescale(quotient,global_precision,qprec);
	make_msbptr(quotient,qprec); 

	while (bits--)
	{	mp_shift_left(remainder);
		if (mp_compare(remainder,divisor) >= 0)
		{	mp_sub(remainder,divisor);
			stuff_bit(quotient,bitmask);
		}
		bump_bitsniffer(quotient,bitmask);
	}
	mp_init0(remainder);	/* burn sensitive data left on stack */
	return(0);
} /* mp_recip */


int mp_div(register unitptr remainder,register unitptr quotient,
	register unitptr dividend,register unitptr divisor)
	/* Signed divide, either or both operands may be negative. */
{	boolean dvdsign,dsign;
	int status;
	dvdsign = (mp_tstminus(dividend)!=0);
	dsign = (mp_tstminus(divisor)!=0);
	if (dvdsign) mp_neg(dividend);
	if (dsign) mp_neg(divisor);
	status = mp_udiv(remainder,quotient,dividend,divisor);
	if (dvdsign) mp_neg(dividend);		/* repair caller's dividend */
	if (dsign) mp_neg(divisor);		/* repair caller's divisor */
	if (status<0) return(status);		/* divide error? */
	if (dvdsign) mp_neg(remainder);
	if (dvdsign ^ dsign) mp_neg(quotient);
	return(status);
} /* mp_div */


word16 mp_shortdiv(register unitptr quotient,
	register unitptr dividend,register word16 divisor)
/*	This function does a fast divide and mod on a multiprecision dividend
	using a short integer divisor returning a short integer remainder.
	This is an unsigned divide.  It treats both operands as positive.
	It is used mainly for faster printing of large numbers in base 10. 
*/
{	int bits;
	short dprec;
	register unit bitmask;
	register word16 remainder;
	if (!divisor)	/* if divisor == 0 */
		return(-1);	/* zero divisor means divide error */
	remainder=0;
	mp_init0(quotient);
	/* normalize and compute number of bits in dividend first */
	init_bitsniffer(dividend,bitmask,dprec,bits);
	/* rescale quotient to same precision (dprec) as dividend */
	rescale(quotient,global_precision,dprec);
	make_msbptr(quotient,dprec); 

	while (bits--)
	{	remainder <<= 1;
		if (sniff_bit(dividend,bitmask))
			remainder++;
		if (remainder >= divisor)
		{	remainder -= divisor;
			stuff_bit(quotient,bitmask);
		}
		bump_2bitsniffers(dividend,quotient,bitmask); 
	}	
	return(remainder);
} /* mp_shortdiv */


int mp_mod(register unitptr remainder,
	register unitptr dividend,register unitptr divisor)
	/* Unsigned divide, treats both operands as positive. */
{	int bits;
	short dprec;
	register unit bitmask;
	if (testeq(divisor,0))
		return(-1);	/* zero divisor means divide error */
	mp_init0(remainder);
	/* normalize and compute number of bits in dividend first */
	init_bitsniffer(dividend,bitmask,dprec,bits);

	while (bits--)
	{	mp_rotate_left(remainder,(boolean)(sniff_bit(dividend,bitmask)!=0));
		msub(remainder,divisor);
		bump_bitsniffer(dividend,bitmask);
	}	
	return(0);
} /* mp_mod */


word16 mp_shortmod(register unitptr dividend,register word16 divisor)
/*	This function does a fast mod operation on a multprecision dividend
	using a short integer modulus returning a short integer remainder.
	This is an unsigned divide.  It treats both operands as positive.
	It is used mainly for fast sieve searches for large primes. 
*/
{	int bits;
	short dprec;
	register unit bitmask;
	register word16 remainder;
	if (!divisor)	/* if divisor == 0 */
		return(-1);	/* zero divisor means divide error */
	remainder=0;
	/* normalize and compute number of bits in dividend first */
	init_bitsniffer(dividend,bitmask,dprec,bits);

	while (bits--)
	{	remainder <<= 1;
		if (sniff_bit(dividend,bitmask))
			remainder++;
		if (remainder >= divisor) remainder -= divisor;
		bump_bitsniffer(dividend,bitmask);
	}	
	return(remainder);
} /* mp_shortmod */



#ifdef COMB_MULT	/* use faster "comb" multiply algorithm */
	/* We are skipping this code because it has a bug... */

int mp_mult(register unitptr prod,
	register unitptr multiplicand, register unitptr multiplier)
	/*	Computes multiprecision prod = multiplicand * multiplier */
{	/*	Uses interleaved comb multiply algorithm.
		This improved multiply more than twice as fast as a Russian 
		peasant multiply, because it does a lot fewer shifts. 
		Must have global_precision set to the size of the multiplicand 
		plus UNITSIZE-1 SLOP_BITS.  Produces a product that is the sum 
		of the lengths of the multiplier and multiplicand.

		BUG ALERT:  Unfortunately, this code has a bug.  It fails for 
		some numbers.  One such example:
		x=   59DE 60CE 2345 8091 A02B 2A1C DBC3 8BE5 
		x*x= 59DE 60CE 2345 26B3 993B 67A5 2499 0B7D 
             52C8 CDC7 AFB3 61C8 243C 741B
		--which is obviously wrong.  The answer should be:
		x*x= 1F8C 607B 5EA6 C061 2714 04A9 A0C6 A17A 
             C9AB 6095 C62F 3756 3843 E4D0 3950 7AD9 
		We'll have to fix this some day.  In the meantime, we'll 
		just have the compiler skip over this code. 
	*/
	int bits;
	register unit bitmask;
	unitptr product, mplier, temp;
	short mprec,mprec2;
	unit mplicand[MAX_UNIT_PRECISION];
	mp_init(prod,0);		/* better clear full width--double precision */
	if (testeq(multiplicand,0))
		return(0);	/* zero multiplicand means zero product */

	mp_move(mplicand,multiplicand);	/* save it from damage */

	normalize(multiplier,mprec);
	if (!mprec)
		return(0);

	make_lsbptr(multiplier,mprec);
	bitmask = 1;	/* start scan at LSB of multiplier */

	do	/* UNITSIZE times */
	{	/* do for bits 0-15 */
		product = prod;
		mplier = multiplier;
		mprec2 = mprec;
		while (mprec2--)	/* do for each word in multiplier */
		{
			if (sniff_bit(mplier,bitmask))
			{	if (mp_addc(product,multiplicand,0))	/* ripple carry */
				{	/* After 1st time thru, this is rarely encountered. */
					temp = msbptr(product,global_precision);
					pre_higherunit(temp);
					/* temp now points to LSU of carry region. */
					unmake_lsbptr(temp,global_precision);
					mp_inc(temp);
				}	/* ripple carry */
			}
			pre_higherunit(mplier);
			pre_higherunit(product);
		}
		if (!(bitmask <<= 1))
			break;
		mp_shift_left(multiplicand);

	} while (TRUE);

	mp_move(multiplicand,mplicand);	/* recover */

	return(0);	/* normal return */	
}	/* mp_mult */

#endif	/* COMB_MULT */


/*	Because the "comb" multiply has a bug, we will use the slower
	Russian peasant multiply instead.  Fortunately, the mp_mult 
	function is not called from any time-critical code.
*/

int mp_mult(register unitptr prod,
	register unitptr multiplicand,register unitptr multiplier)
	/* Computes multiprecision prod = multiplicand * multiplier */
{	/* Uses "Russian peasant" multiply algorithm. */
	int bits;
	register unit bitmask;
	short mprec;
	mp_init(prod,0);
	if (testeq(multiplicand,0))
		return(0);	/* zero multiplicand means zero product */
	/* normalize and compute number of bits in multiplier first */
	init_bitsniffer(multiplier,bitmask,mprec,bits);

	while (bits--)
	{	mp_shift_left(prod);
		if (sniff_bit(multiplier,bitmask))
			mp_add(prod,multiplicand);
		bump_bitsniffer(multiplier,bitmask);
	}
	return(0);	
}	/* mp_mult */



/*	mp_modmult computes a multiprecision multiply combined with a 
	modulo operation.  This is the most time-critical function in
	this multiprecision arithmetic library for performing modulo
	exponentiation.  We experimented with different versions of modmult,
	depending on the machine architecture and performance requirements.
	We will either use a Russian Peasant modmult (peasant_modmult), 
	Charlie Merritt's modmult (merritt_modmult) or Jimmy Upton's
	modmult (upton_modmult).  On machines with a hardware atomic 
	multiply instruction, Upton's modmult is fastest, which depends
	on an assembly subroutine to speed up the hardware multiply logic.
	If the machine lacks a fast hardware multiply, Merritt's modmult
	is preferred, which doesn't call any assembly multiply routine.
	We use the alias names mp_modmult, stage_modulus, and modmult_burn 
	for the corresponding true names, which depend on what flavor of 
	modmult we are using.

	Before making the first call to mp_modmult, you must set up the 
	modulus-dependant precomputated tables by calling stage_modulus.
	After making all the calls to mp_modmult, you call modmult_burn to 
	erase the tables created by stage_modulus that were left in memory.
*/

#ifdef COUNTMULTS
/* "number of modmults" counters, used for performance studies. */
static unsigned int tally_modmults = 0;
static unsigned int tally_modsquares = 0;
#endif	/* COUNTMULTS */


#ifdef PEASANT
/* Conventional Russian peasant multiply with modulo algorithm. */

static unitptr modulus = 0;	/* used only by mp_modmult */

int stage_peasant_modulus(unitptr n)
/*	Must pass modulus to stage_modulus before calling modmult.
	Assumes that global_precision has already been adjusted to the
	size of the modulus, plus SLOP_BITS.
*/
{	/* For this simple version of modmult, just copy unit pointer. */
	modulus = n;
	return(0);	/* normal return */
}	/* stage_peasant_modulus */


int peasant_modmult(register unitptr prod,
	unitptr multiplicand,register unitptr multiplier)
{	/*	"Russian peasant" multiply algorithm, combined with a modulo 
		operation.  This is a simple naive replacement for Merritt's 
		faster modmult algorithm.  References global unitptr "modulus".
		Computes:  prod = (multiplicand*multiplier) mod modulus
		WARNING: All the arguments must be less than the modulus!
	*/
	int bits;
	register unit bitmask;
	short mprec;
	mp_init(prod,0);
/*	if (testeq(multiplicand,0))
		return(0); */	/* zero multiplicand means zero product */
	/* normalize and compute number of bits in multiplier first */
	init_bitsniffer(multiplier,bitmask,mprec,bits);

	while (bits--)
	{	mp_shift_left(prod);
		msub(prod,modulus);	/* turns mult into modmult */
		if (sniff_bit(multiplier,bitmask))
		{	mp_add(prod,multiplicand);
			msub(prod,modulus);	/* turns mult into modmult */
		}
		bump_bitsniffer(multiplier,bitmask);
	}
	return(0);
}	/* peasant_modmult */


/*	If we are using a version of mp_modmult that uses internal tables 
	in memory, we have to call modmult_burn() at the end of mp_modexp.
	This is so that no sensitive data is left in memory after the program 
	exits.  The Russian peasant method doesn't use any such tables.
*/
static void peasant_burn(void)
/*	Alias for modmult_burn, called only from mp_modexp().  Destroys
	internal modmult tables.  This version does nothing because no 
	tables are used by the Russian peasant modmult. */
{ }	/* peasant_burn */

#endif	/* PEASANT */


#ifdef MERRITT
/*=========================================================================*/
/*
	This is Charlie Merritt's MODMULT algorithm, implemented in C by PRZ.
	Also refined by Zhahai Stewart to reduce the number of subtracts.
	It performs a multiply combined with a modulo operation, without 
	going into "double precision".  It is faster than the Russian peasant
	method,	and still works well on machines that lack a fast hardware 
	multiply instruction.  
*/

/*	The following support functions, macros, and data structures
	are used only by Merritt's modmult algorithm... */

static void mp_lshift_unit(register unitptr r1)
/*	Shift r1 1 whole unit to the left.  Used only by modmult function. */
{	register short precision;
	register unitptr r2;
	precision = global_precision;
	make_msbptr(r1,precision);
	r2 = r1;
	while (--precision)
		*post_lowerunit(r1) = *pre_lowerunit(r2);
	*r1 = 0;
}	/* mp_lshift_unit */


/* moduli_buf contains shifted images of the modulus, set by stage_modulus */
static unit moduli_buf[UNITSIZE][MAX_UNIT_PRECISION] = {0};
static unitptr moduli[UNITSIZE+1] = /* contains pointers into moduli_buf */
{	0
	,&moduli_buf[ 0][0], &moduli_buf[ 1][0], &moduli_buf[ 2][0], &moduli_buf[ 3][0], 
	 &moduli_buf[ 4][0], &moduli_buf[ 5][0], &moduli_buf[ 6][0], &moduli_buf[ 7][0]
#ifndef UNIT8
	,&moduli_buf[ 8][0], &moduli_buf[ 9][0], &moduli_buf[10][0], &moduli_buf[11][0], 
	 &moduli_buf[12][0], &moduli_buf[13][0], &moduli_buf[14][0], &moduli_buf[15][0] 
#ifndef UNIT16	/* and not UNIT8 */
	,&moduli_buf[16][0], &moduli_buf[17][0], &moduli_buf[18][0], &moduli_buf[19][0], 
	 &moduli_buf[20][0], &moduli_buf[21][0], &moduli_buf[22][0], &moduli_buf[23][0], 
	 &moduli_buf[24][0], &moduli_buf[25][0], &moduli_buf[26][0], &moduli_buf[27][0], 
	 &moduli_buf[28][0], &moduli_buf[29][0], &moduli_buf[30][0], &moduli_buf[31][0]
#endif	/* UNIT16 and UNIT8 not defined */
#endif	/* UNIT8 not defined */
};

/*	To optimize msubs, need following 2 unit arrays, each filled
	with the most significant unit and next-to-most significant unit
	of the preshifted images of the modulus. */
static unit msu_moduli[UNITSIZE+1] = {0}; /* most signif. unit */
static unit nmsu_moduli[UNITSIZE+1] = {0}; /* next-most signif. unit */

/*	mpdbuf contains preshifted images of the multiplicand, mod n.
 	It is used only by mp_modmult.  It could be staticly declared
	inside of mp_modmult, but we put it outside mp_modmult so that 
	it can be wiped clean by modmult_burn(), which is called at the
	end of mp_modexp.  This is so that no sensitive data is left in 
	memory after the program exits.
*/
static unit mpdbuf[UNITSIZE-1][MAX_UNIT_PRECISION] = {0};


static void stage_mp_images(unitptr images[UNITSIZE],unitptr r)
/*	Computes UNITSIZE images of r, each shifted left 1 more bit.
	Used only by modmult function. 
*/
{	short int i;
	images[0] = r;	/* no need to move the first image, just copy ptr */
	for (i=1; i<UNITSIZE; i++)
	{	mp_move(images[i],images[i-1]);
		mp_shift_left(images[i]);
	}
}	/* stage_mp_images */


int stage_merritt_modulus(unitptr n)
/*	Computes UNITSIZE+1 images of modulus, each shifted left 1 more bit.
	Before calling mp_modmult, you must first stage the moduli images by
	calling stage_modulus.  n is the pointer to the modulus.
	Assumes that global_precision has already been adjusted to the
	size of the modulus, plus SLOP_BITS.
*/
{	short int i;
	unitptr msu;	/* ptr to most significant unit, for faster msubs */
	moduli[0] = n;	/* no need to move the first image, just copy ptr */

	/* used by optimized msubs macro... */
	msu = msbptr(n,global_precision);	/* needed by msubs */
	msu_moduli[0] = *post_lowerunit(msu);
	nmsu_moduli[0] = *msu;

	for (i=1; i<UNITSIZE+1; i++)
	{	mp_move(moduli[i],moduli[i-1]);
		mp_shift_left(moduli[i]);

		/* used by optimized msubs macro... */
		msu = msbptr(moduli[i],global_precision);	/* needed by msubs */
		msu_moduli[i] = *post_lowerunit(msu);	/* for faster msubs */
		nmsu_moduli[i] = *msu;
	}
	return(0);	/* normal return */
}	/* stage_merritt_modulus */


/* The following macros, sniffadd and msubs, are used by modmult... */
#define sniffadd(i) if (*multiplier & power_of_2(i))  mp_add(prod,mpd[i])

/* Unoptimized msubs macro (msubs0) follows... */
/* #define msubs0(i) msub(prod,moduli[i]) 
*/

/*	To optimize msubs, compare the most significant units of the 
	product and the shifted modulus before deciding to call the full 
	mp_compare routine.  Better still, compare the upper two units
	before deciding to call mp_compare.
	Optimization of msubs relies heavily on standard C left-to-right 
	parsimonious evaluation of logical expressions. 
*/

/* Partially-optimized msubs macro (msubs1) follows... */
/* #define msubs1(i) if ( \
  ((p_m = (*msu_prod-msu_moduli[i])) >= 0) && \
  (p_m || (mp_compare(prod,moduli[i]) >= 0) ) \
  ) mp_sub(prod,moduli[i])
*/

/* Fully-optimized msubs macro (msubs2) follows... */
#define msubs(i) if (((p_m = *msu_prod-msu_moduli[i])>0) || ( \
 (p_m==0) && ( (*nmsu_prod>nmsu_moduli[i]) || ( \
 (*nmsu_prod==nmsu_moduli[i]) && ((mp_compare(prod,moduli[i]) >= 0)) ))) ) \
 mp_sub(prod,moduli[i])


int merritt_modmult(register unitptr prod,
	unitptr multiplicand,register unitptr multiplier)
	/*	Performs combined multiply/modulo operation.  
		Computes:  prod = (multiplicand*multiplier) mod modulus
		WARNING: All the arguments must be less than the modulus!
		Assumes the modulus has been predefined by first calling 
		stage_modulus.
	*/
{
	/* p_m, msu_prod, and nmsu_prod are used by the optimized msubs macro...*/
	register signedunit p_m;
	register unitptr msu_prod;	/* ptr to most significant unit of product */
	register unitptr nmsu_prod;	/* next-most signif. unit of product */
	short mprec;		/* precision of multiplier, in units */
	/*	Array mpd contains a list of pointers to preshifted images of 
		the multiplicand: */
	static unitptr mpd[UNITSIZE] =
	{	 0,              &mpdbuf[ 0][0], &mpdbuf[ 1][0], &mpdbuf[ 2][0],
		 &mpdbuf[ 3][0], &mpdbuf[ 4][0], &mpdbuf[ 5][0], &mpdbuf[ 6][0]
#ifndef UNIT8
		,&mpdbuf[ 7][0], &mpdbuf[ 8][0], &mpdbuf[ 9][0], &mpdbuf[10][0],
		 &mpdbuf[11][0], &mpdbuf[12][0], &mpdbuf[13][0], &mpdbuf[14][0]
#ifndef UNIT16	/* and not UNIT8 */
		,&mpdbuf[15][0], &mpdbuf[16][0], &mpdbuf[17][0], &mpdbuf[18][0],
		 &mpdbuf[19][0], &mpdbuf[20][0], &mpdbuf[21][0], &mpdbuf[22][0],
		 &mpdbuf[23][0], &mpdbuf[24][0], &mpdbuf[25][0], &mpdbuf[26][0],
		 &mpdbuf[27][0], &mpdbuf[28][0], &mpdbuf[29][0], &mpdbuf[30][0]
#endif	/* UNIT16 and UNIT8 not defined */
#endif	/* UNIT8 not defined */
	};

	/* Compute preshifted images of multiplicand, mod n: */
	stage_mp_images(mpd,multiplicand);

	/* To optimize msubs, set up msu_prod and nmsu_prod: */
	msu_prod = msbptr(prod,global_precision); /* Get ptr to MSU of prod */
	nmsu_prod = msu_prod;
	post_lowerunit(nmsu_prod); /* Get next-MSU of prod */

	/*	To understand this algorithm, it would be helpful to first 
		study the conventional Russian peasant modmult algorithm.
		This one does about the same thing as Russian peasant, but
		is organized differently to save some steps.  It loops 
		through the multiplier a word (unit) at a time, instead of 
		a bit at a time.  It word-shifts the product instead of 
		bit-shifting it, so it should be faster.  It also does about 
		half as many subtracts as Russian peasant.
	*/

	mp_init(prod,0);	/* Initialize product to 0. */

	/*	The way mp_modmult is actually used in cryptographic 
		applications, there will NEVER be a zero multiplier or 
		multiplicand.  So there is no need to optimize for that 
		condition.
	*/
/*	if (testeq(multiplicand,0))
		return(0); */	/* zero multiplicand means zero product */
	/* Normalize and compute number of units in multiplier first: */
	normalize(multiplier,mprec);
	if (mprec==0)	/* if precision of multiplier is 0 */
		return(0);	/* zero multiplier means zero product */
	make_msbptr(multiplier,mprec); /* start at MSU of multiplier */

	while (mprec--)	/* Loop for the number of units in the multiplier */
	{
		/*	Shift the product one whole unit to the left.
			This is part of the multiply phase of modmult. 
		*/

		mp_lshift_unit(prod);

		/*	Sniff each bit in the current unit of the multiplier, 
			and conditionally add the the corresponding preshifted 
			image of the multiplicand to the product.
			This is also part of the multiply phase of modmult.

			The following loop is unrolled for speed, using macros...

		for (i=UNITSIZE-1; i>=0; i--)
		   if (*multiplier & power_of_2(i)) 
				mp_add(prod,mpd[i]);
		*/
#ifndef UNIT8
#ifndef UNIT16	/* and not UNIT8 */
		sniffadd(31); 
		sniffadd(30); 
		sniffadd(29); 
		sniffadd(28);
		sniffadd(27); 
		sniffadd(26); 
		sniffadd(25); 
		sniffadd(24);
		sniffadd(23); 
		sniffadd(22); 
		sniffadd(21); 
		sniffadd(20);
		sniffadd(19); 
		sniffadd(18); 
		sniffadd(17); 
		sniffadd(16);
#endif	/* not UNIT16 and not UNIT8 */
		sniffadd(15); 
		sniffadd(14); 
		sniffadd(13); 
		sniffadd(12);
		sniffadd(11); 
		sniffadd(10); 
		sniffadd(9); 
		sniffadd(8);
#endif	/* not UNIT8 */
		sniffadd(7); 
		sniffadd(6); 
		sniffadd(5); 
		sniffadd(4);
		sniffadd(3); 
		sniffadd(2); 
		sniffadd(1); 
		sniffadd(0);

		/*	The product may have grown by as many as UNITSIZE+1 
			bits.  That's why we have global_precision set to the
			size of the modulus plus UNITSIZE+1 slop bits.  
			Now reduce the product back down by conditionally 
			subtracting	the UNITSIZE+1 preshifted images of the 
			modulus.  This is the modulo reduction phase of modmult.

			The following loop is unrolled for speed, using macros...

		for (i=UNITSIZE; i>=0; i--)
			if (mp_compare(prod,moduli[i]) >= 0) 
				mp_sub(prod,moduli[i]); 
		*/
#ifndef UNIT8
#ifndef UNIT16	/* and not UNIT8 */
		msubs(32); 
		msubs(31); 
		msubs(30); 
		msubs(29); 
		msubs(28);
		msubs(27); 
		msubs(26); 
		msubs(25); 
		msubs(24);
		msubs(23); 
		msubs(22); 
		msubs(21); 
		msubs(20);
		msubs(19); 
		msubs(18); 
		msubs(17); 
#endif	/* not UNIT16 and not UNIT8 */
		msubs(16);
		msubs(15); 
		msubs(14); 
		msubs(13); 
		msubs(12);
		msubs(11); 
		msubs(10); 
		msubs(9); 
#endif	/* not UNIT8 */
		msubs(8);
		msubs(7); 
		msubs(6); 
		msubs(5); 
		msubs(4);
		msubs(3); 
		msubs(2); 
		msubs(1); 
		msubs(0);

		/* Bump pointer to next lower unit of multiplier: */
		post_lowerunit(multiplier);

	}	/* Loop for the number of units in the multiplier */

	return(0);	/* normal return */

}	/* merritt_modmult */


#undef msubs
#undef sniffadd


/*	Merritt's mp_modmult function leaves some internal tables in memory,
	so we have to call modmult_burn() at the end of mp_modexp.  
	This is so that no cryptographically sensitive data is left in memory 
	after the program exits.
*/
static void merritt_burn(void)
/*	Alias for modmult_burn, merritt_burn() is called only by mp_modexp. */
{	unitfill0(&(mpdbuf[0][0]),(UNITSIZE-1)*MAX_UNIT_PRECISION);
	unitfill0(&(moduli_buf[0][0]),(UNITSIZE)*MAX_UNIT_PRECISION);
	unitfill0(msu_moduli,UNITSIZE+1);
	unitfill0(nmsu_moduli,UNITSIZE+1);
} /* merritt_burn() */

/******* end of Merritt's MODMULT stuff. *******/
/*=========================================================================*/
#endif	/* MERRITT */


#ifdef UPTON	/* using Jimmy Upton's modmult algorithm */

/*	Jimmy Upton's multiprecision modmult algorithm in C.
	Performs a multiply combined with a modulo operation.  

	The following support functions and data structures
	are used only by Upton's modmult algorithm...
*/

/*	The MULTUNIT word is the biggest word size for the native atomic 
	multiply instruction.  It may or may not be smaller than UNITSIZE. 
	Many CPU's have 16-by-16-bit multiplies, yielding a 32-bit product.
	In that case, MULTUINIT should be a 16-bit word, even if the rest of
	the multiprecision arithmetic functions assume a 32-bit UNIT word.
*/
typedef unsigned short MULTUNIT;
#define	MULTUNITSIZE	(8*sizeof(MULTUNIT))	/* size in bits */

static short unit_prec;	/* global_precision expressed in MULTUNITs */


/*	Define MPORTABLE if there is no assembly version of the mp_smul
	function available.  An assembly version is much faster.
	Note that since the SPARC CPU has no hardware integer multiply 
	instruction, there is not that much advantage in having an 
	assembly version of mp_smul on that machine.  It might be faster
	to use Merritt's modmult instead of Upton's modmult on the SPARC.
*/
#ifdef MSDOS	/* we do have an assembly version available on the 8086 */
#undef MPORTABLE
#endif

#ifdef MPORTABLE  /* use slow portable C version instead of assembly */

/* 
	Multiply the single-word multiplier times the multiprecision integer 
	in multiplicand, accumulating result in prod.  The resulting 
	multiprecision prod will be 1 word longer than the multiplicand.   
	multiplicand is unit_prec words long.  We add into prod, so caller 
	should zero it out first.  For best results, this time-critical 
	function should be implemented in assembly.
	NOTE:  Unlike other functions in the multiprecision arithmetic 
	library, both multiplicand and prod are pointing at the LSB, 
	regardless of byte order of the machine.  On an 80x86, this makes 
	no difference.  But if this assembly function is implemented
	on a 680x0, it becomes important.
*/
void mp_smul (MULTUNIT *prod, MULTUNIT *multiplicand, MULTUNIT multiplier)
{
	short i;
	unsigned long p, carry;

	carry = 0;
	for (i=0; i<unit_prec; ++i)
	{	p = (unsigned long)multiplier * *post_higherunit(multiplicand);
		p += *prod + carry;
		*post_higherunit(prod) = (MULTUNIT)p;
		carry = p >> MULTUNITSIZE;
	}
	/* We know that the high-order word of prod will always be 0 */
	*prod = (MULTUNIT)carry;	/* copy carry to empty high word of prod */
}	/* mp_smul */

#else	/* not MPORTABLE-- use assembly routine */

#define mp_smul P_SMUL	/* use external assembly routine */

#endif /* MPORTABLE */

#ifdef VMS

#define mp_dmul p_dmul /* use external assembly routine */

void mp_dmul (unitptr prod, unitptr multiplicand, unitptr multiplier);

#else /* VMS */

/*	mp_dmul is a double-precision multiply multiplicand times multiplier, 
	result into prod.  prod must be pointing at a "double precision" 
	buffer.  E.g. If global_precision is 10 words, prod must be 
	pointing at a 20-word buffer.
*/
static void mp_dmul (unitptr prod, unitptr multiplicand, unitptr multiplier)
{
	register	int i;
	register	MULTUNIT *p_multiplicand, *p_multiplier;
	register	MULTUNIT *prodp;

	unitfill0(prod,global_precision*2);	/* Pre-zero prod */
	/* Calculate precision in units of MULTUNIT */
	unit_prec = global_precision * UNITSIZE / MULTUNITSIZE;
	p_multiplicand = (MULTUNIT *)multiplicand;
	p_multiplier = (MULTUNIT *)multiplier;
	prodp = (MULTUNIT *)prod;
	make_lsbptr(p_multiplicand,unit_prec);
	make_lsbptr(p_multiplier,unit_prec);
	make_lsbptr(prodp,unit_prec*2);
	/* Multiply multiplicand by each word in multiplier, accumulating prod: */
	for (i=0; i<unit_prec; ++i)
		mp_smul (post_higherunit(prodp), 
			p_multiplicand, *post_higherunit(p_multiplier));
}	/* mp_dmul */

#endif /* VMS */

/*	These scratchpad arrays are used only by upton_modmult (mp_modmult).
 	Some of them could be staticly declared inside of mp_modmult, but we 
	put them outside mp_modmult so that they can be wiped clean by 
	modmult_burn(), which is called at the end of mp_modexp.  This is 
	so that no sensitive data is left in memory after the program exits.
*/

#ifdef VAXC
/*
 * Operations on arrays are sped up on a VAX if the data is quadword (64-bit)
 * aligned. This is because many VAXen use a minimum of 64-bit data paths to
 * cache and memory. This technique is VAX C private, hence the conditional.
 */
static unit _align( quadword ) modulus[MAX_UNIT_PRECISION];
static unit _align( quadword ) reciprocal[MAX_UNIT_PRECISION];
static unit _align( quadword ) dhi[MAX_UNIT_PRECISION];
static unit _align( quadword ) d_data[MAX_UNIT_PRECISION*2];	 /* double-wide register */
static unit _align( quadword ) e_data[MAX_UNIT_PRECISION*2];	 /* double-wide register */
static unit _align( quadword ) f_data[MAX_UNIT_PRECISION*2];	 /* double-wide register */

#else /* VAXC */

static unit modulus[MAX_UNIT_PRECISION];
static unit reciprocal[MAX_UNIT_PRECISION];
static unit dhi[MAX_UNIT_PRECISION];
static unit d_data[MAX_UNIT_PRECISION*2];	/* double-wide register */
static unit e_data[MAX_UNIT_PRECISION*2];	/* double-wide register */
static unit f_data[MAX_UNIT_PRECISION*2];	/* double-wide register */

#endif /* VAXC */

static short nbits;
static short nbitsDivUNITSIZE;
static short nbitsModUNITSIZE;

/*	stage_upton_modulus() is aliased to stage_modulus().
	Prepare for an Upton modmult.  Calculate the reciprocal of modulus,
	and save both.  Note that reciprocal will have one more bit than 
	modulus.
	Assumes that global_precision has already been adjusted to the
	size of the modulus, plus SLOP_BITS.
*/
int stage_upton_modulus(unitptr n)
{
	mp_move(modulus, n);
	mp_recip(reciprocal, modulus);
	nbits = countbits(modulus);
	nbitsDivUNITSIZE = nbits / UNITSIZE;
	nbitsModUNITSIZE = nbits % UNITSIZE;
	return(0);	/* normal return */
}	/* stage_upton_modulus */


/* 	Upton's algorithm performs a multiply combined with a modulo operation.  
	Computes:  prod = (multiplicand*multiplier) mod modulus
	WARNING: All the arguments must be less than the modulus!
	References global unitptr modulus and reciprocal.
	The reciprocal of modulus is 1 bit longer than the modulus.  
	upton_modmult() is aliased to mp_modmult().
*/
int upton_modmult (unitptr prod, unitptr multiplicand, unitptr multiplier)
{
	unitptr d = d_data;
	unitptr d1 = d_data;
	unitptr e = e_data;
	unitptr f = f_data;
	short orig_precision;

	orig_precision = global_precision;
	mp_dmul (d,multiplicand,multiplier);
	
	/* Throw off low nbits of d */
#ifndef HIGHFIRST
	d1 = d + nbitsDivUNITSIZE;
#else
	d1 = d + orig_precision - nbitsDivUNITSIZE;
#endif
	mp_move(dhi, d1);	/* Don't screw up d, we need it later */
	mp_shift_right_bits(dhi, nbitsModUNITSIZE);

	mp_dmul (e,dhi,reciprocal);	/* Note - reciprocal has nbits+1 bits */

#ifndef HIGHFIRST
	e += nbitsDivUNITSIZE;
#else
	e += orig_precision - nbitsDivUNITSIZE;
#endif
	mp_shift_right_bits(e, nbitsModUNITSIZE);

	mp_dmul (f,e,modulus);

	/* Now for the only double-precision call to mpilib: */
	set_precision (orig_precision * 2);
	mp_sub (d,f);

	/* d's precision should be <= orig_precision */
	rescale (d, orig_precision*2, orig_precision);
	set_precision (orig_precision);

	/* Should never have to do this final subtract more than twice: */
	while (mp_compare(d,modulus) > 0)
		mp_sub (d,modulus);

	mp_move(prod,d);
	return(0);	/* normal return */
}	/* upton_modmult */


/*	Upton's mp_modmult function leaves some internal arrays in memory,
	so we have to call modmult_burn() at the end of mp_modexp.  
	This is so that no cryptographically sensitive data is left in memory 
	after the program exits.
	upton_burn() is aliased to modmult_burn().
*/
void upton_burn(void)
{
	unitfill0(modulus,MAX_UNIT_PRECISION);
	unitfill0(reciprocal,MAX_UNIT_PRECISION);
	unitfill0(dhi,MAX_UNIT_PRECISION);
	unitfill0(d_data,MAX_UNIT_PRECISION*2);
	unitfill0(e_data,MAX_UNIT_PRECISION*2);
	unitfill0(f_data,MAX_UNIT_PRECISION*2);
	nbits = nbitsDivUNITSIZE = nbitsModUNITSIZE = 0;
}	/* upton_burn */

#endif	/* UPTON */


int countbits(unitptr r)
	/* Returns number of significant bits in r */
{	int bits;
	short prec;
	register unit bitmask;
	init_bitsniffer(r,bitmask,prec,bits);
	return(bits);
} /* countbits */


char *copyright_notice(void)
/* force linker to include copyright notice in the executable object image. */
{ return ("(c)1986 Philip Zimmermann"); } /* copyright_notice */


int mp_modexp(register unitptr expout,register unitptr expin,
	register unitptr exponent,register unitptr modulus)
{	/*	Russian peasant combined exponentiation/modulo algorithm.
		Calls modmult instead of mult. 
		Computes:  expout = (expin**exponent) mod modulus
		WARNING: All the arguments must be less than the modulus!
	*/
	int bits;
	short oldprecision;
	register unit bitmask;
	unit product[MAX_UNIT_PRECISION];
	short eprec;

#ifdef COUNTMULTS
	tally_modmults = 0;	/* clear "number of modmults" counter */
	tally_modsquares = 0;	/* clear "number of modsquares" counter */
#endif	/* COUNTMULTS */
	mp_init(expout,1);
	if (testeq(exponent,0))
	{	if (testeq(expin,0))
			return(-1); /* 0 to the 0th power means return error */
		return(0);	/* otherwise, zero exponent means expout is 1 */
	}
	if (testeq(modulus,0))
		return(-2);		/* zero modulus means error */
#if SLOP_BITS > 0	/* if there's room for sign bits */
	if (mp_tstminus(modulus))
		return(-2);		/* negative modulus means error */
#endif	/* SLOP_BITS > 0 */
	if (mp_compare(expin,modulus) >= 0)
		return(-3); /* if expin >= modulus, return error */
	if (mp_compare(exponent,modulus) >= 0)
		return(-4); /* if exponent >= modulus, return error */

	oldprecision = global_precision;	/* save global_precision */
	/* set smallest optimum precision for this modulus */
	set_precision(bits2units(countbits(modulus)+SLOP_BITS));
	/* rescale all these registers to global_precision we just defined */
	rescale(modulus,oldprecision,global_precision);
	rescale(expin,oldprecision,global_precision);
	rescale(exponent,oldprecision,global_precision);
	rescale(expout,oldprecision,global_precision);

	if (stage_modulus(modulus))
	{	set_precision(oldprecision);	/* restore original precision */
		return(-5);		/* unstageable modulus (STEWART algorithm) */
	}

	/* normalize and compute number of bits in exponent first */
	init_bitsniffer(exponent,bitmask,eprec,bits);

	/* We can "optimize out" the first modsquare and modmult: */
	bits--;		/* We know for sure at this point that bits>0 */
	mp_move(expout,expin);		/*  expout = (1*1)*expin; */
	bump_bitsniffer(exponent,bitmask);
	
	while (bits--)
	{
		poll_for_break(); /* polls keyboard, allows ctrl-C to abort program */
#ifdef COUNTMULTS
		tally_modsquares++;	/* bump "number of modsquares" counter */
#endif	/* COUNTMULTS */
		mp_modsquare(product,expout);
		mp_move(expout,product);
		if (sniff_bit(exponent,bitmask))
		{	mp_modmult(product,expout,expin);
			mp_move(expout,product);
#ifdef COUNTMULTS
			tally_modmults++;	/* bump "number of modmults" counter */
#endif	/* COUNTMULTS */
		}
		bump_bitsniffer(exponent,bitmask);
	}	/* while bits-- */
	mp_burn(product);	/* burn the evidence on the stack */
	modmult_burn(); /* ask mp_modmult to also burn its own evidence */

#ifdef COUNTMULTS	/* diagnostic analysis */
	{	long atomic_mults;
		unsigned int unitcount,totalmults;
		unitcount = bits2units(countbits(modulus));
		/* calculation assumes modsquare takes as long as a modmult: */
		atomic_mults = (long) tally_modmults * (unitcount * unitcount);
		atomic_mults += (long) tally_modsquares * (unitcount * unitcount);
		printf("%ld atomic mults for ",atomic_mults);
		printf("%d+%d = %d modsqr+modmlt, at %d bits, %d words.\n",
			tally_modsquares,tally_modmults,
			tally_modsquares+tally_modmults,
			countbits(modulus), unitcount);
	}
#endif	/* COUNTMULTS */

	set_precision(oldprecision);	/* restore original precision */

	/* Do an explicit reference to the copyright notice so that the linker
	   will be forced to include it in the executable object image... */
	copyright_notice();	/* has no real effect at run time */

	return(0);		/* normal return */
}	/* mp_modexp */




/*********************************************************************
	RSA-specific routines follow.  These are the only functions that 
	are specific to the RSA public key cryptosystem.  The other
	multiprecision integer math functions may be used for non-RSA
	applications.  

	The RSA public key cryptosystem is patented by the Massachusetts
	Institute of Technology (U.S. patent #4,405,829).  This patent does
	not apply outside the USA.  Public Key Partners (PKP) holds the
	exclusive commercial license to sell and sub-license the RSA public
	key cryptosystem.  The author of this software implementation of the
	RSA algorithm is providing this software for educational use only. 
	Licensing this algorithm from PKP is the responsibility of you, the
	user, not Philip Zimmermann, the author of this software.  The author
	assumes no liability for any breach of patent law resulting from the
	unlicensed use of this software by the user.
*********************************************************************/


int rsa_decrypt(unitptr M, unitptr C,
	unitptr d, unitptr p, unitptr q, unitptr u)
	/*	Uses Quisquater's Chinese Remainder Theorem shortcut 
		for RSA decryption.
		M is the output plaintext message.
		C is the input ciphertext message.
		d is the secret decryption exponent.
		p and q are the secret prime factors of n.
		u is the multiplicative inverse of p, mod q.
		Note that u is precomputed on the assumption that p<q.
		n, the common modulus, is not used here because of the
		Chinese Remainder Theorem shortcut.
	*/
{
	unit p2[MAX_UNIT_PRECISION];
	unit q2[MAX_UNIT_PRECISION];
	unit temp1[MAX_UNIT_PRECISION];
	unit temp2[MAX_UNIT_PRECISION];
	int status;

	mp_init(M,1);	/* initialize result in case of error */

	if (mp_compare(p,q) >= 0)	/* ensure that p<q */
	{	/* swap the pointers p and q */
		unitptr t;
		t = p;  p = q; q = t;
	}

/*	Rather than decrypting by computing modexp with full mod n
	precision, compute a shorter modexp with mod p precision... */

/*	p2 = [ (C mod p)**( d mod (p-1) ) ] mod p		*/
	mp_move(temp1,p);
	mp_dec(temp1);			/* temp1 = p-1 */
	mp_mod(temp2,d,temp1);	/* temp2 = d mod (p-1) ) */
	mp_mod(temp1,C,p);		/* temp1 = C mod p  */
	status = mp_modexp(p2,temp1,temp2,p);
	if (status < 0)	/* mp_modexp returned an error. */
		return(status);	/* error return */

/*	Now compute a short modexp with mod q precision... */

/*	q2 = [ (C mod q)**( d mod (q-1) ) ] mod q		*/
	mp_move(temp1,q);
	mp_dec(temp1);			/* temp1 = q-1 */
	mp_mod(temp2,d,temp1);	/* temp2 = d mod (q-1) */
	mp_mod(temp1,C,q);		/* temp1 = C mod q  */
	status = mp_modexp(q2,temp1,temp2,q);
	if (status < 0)	/* mp_modexp returned an error. */
		return(status);	/* error return */

/*	Now use the multiplicative inverse u to glue together the
	two halves, saving a lot of time by avoiding a full mod n
	exponentiation.  At key generation time, u was computed
	with the secret key as the multiplicative inverse of
	p, mod q such that:  (p*u) mod q = 1, assuming p<q.
*/
	if (mp_compare(p2,q2) == 0)	/* only happens if C<p */
		mp_move(M,p2);
	else
	{	/*	q2 = q2 - p2;  if q2<0 then q2 = q2 + q  */
		if (mp_sub(q2,p2))	/* if the result went negative... */
			mp_add(q2,q);		/* add q to q2 */

		/*	M = p2 + ( p * [(q2*u) mod q] ) 	*/
		mp_mult(temp1,q2,u);		/* temp1 = q2*u  */
		mp_mod(temp2,temp1,q);		/* temp2 = temp1 mod q */
		mp_mult(temp1,p,temp2);	/* temp1 = p * temp2 */
		mp_add(temp1,p2);		/* temp1 = temp1 + p2 */
		mp_move(M,temp1);		/* M = temp1 */
	}

	mp_burn(p2);	/* burn the evidence on the stack...*/
	mp_burn(q2);
	mp_burn(temp1);
	mp_burn(temp2);
	return(0);	/* normal return */
}	/* rsa_decrypt */


/****************** end of MPI library ****************************/
