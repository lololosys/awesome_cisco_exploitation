/* $Id: macip_util.c,v 3.4.26.4 1996/07/10 22:19:38 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/macip_util.c,v $
 *------------------------------------------------------------------
 * macip_util.c	- general utilities used by macip sub-system.
 *
 *	NOTE:  these are general standalone utilities, formally
 *		contained in independent modules as indicated.
 *
 * Copyright (c) 1983-1990, Russ Tremain.
 * May be used with permission.
 * 
 * Modified, September 1991, by Russ Tremain, Tremain Laboratories,
 * under contract for cisco Systems, Inc.
 * 
 * Modifications copyright (c) 1991 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Copyright (c) 1988-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: macip_util.c,v $
 * Revision 3.4.26.4  1996/07/10  22:19:38  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.4.26.3  1996/06/28  23:05:00  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.26.2  1996/03/23  01:28:39  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.4.26.1  1996/03/18  18:53:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.1  1996/03/07  08:30:02  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4  1996/01/18  19:39:36  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/12/07  22:27:27  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.2  1995/11/17  08:45:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:59:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:51:18  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:10:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include <string.h>
#include <ciscolib.h>
#include "packet.h"
#include <interface.h>
#include "atalk_private.h"

#include "macip_util.h"

/* definitions for at_err(): */
#define DEBUGSWITCH TRUE


#define MAXDEBUG    98

/*
 * str32.c - routines for handling pascal str32 objects
 */


/*************************  EXPORTED FUNCTIONS  ***********************/

/*
 * str160init -
 *	
 *	initialize a str160 from a null-terminated c-string
 *	if <cstr> too long, truncates at 160 and returns false.
 *	
 */
boolean
str160init (str160 *pstr, char *cstr)
{
    register ii;

    register char *pp = PSTRPTR(pstr);

    /* while not done and more space... */
    for (ii=0; ii<160; ii++) {
        if ((pp[ii] = cstr[ii]) == '\0') {
	    pstr->str[0] = (uchar) ii;	/* length is position of NULL */
            return(TRUE);
	}
    }

    /* ran our of space, truncate to length 160: */
    pp[160] = '\0';
    pstr->str[0] = (uchar) 160;
    return(FALSE);		/* FALSE if truncated */
}

/*
 * str32init -
 *	
 *	initialize a str32 from a null-terminated c-string
 *	false if <cstr> too long.
 *	
 */
boolean
str32init (str32 *pstr, char *cstr)
{
    register ii;
    register char *pp = STR32PTR(pstr);

    /* while not done and more space... */
    for (ii=0; ii<32; ii++) {
        if ((pp[ii] = cstr[ii]) == '\0') {
	    pstr->str[0] = (uchar) ii;	/* length is position of NULL */
            return(TRUE);
	}
    }

    /* ran our of space, truncate to length 32: */
    pp[32] = '\0';
    pstr->str[0] = (uchar) 32;
    return(FALSE);		/* FALSE if truncated */
}

/*
 * str32ninit -
 *	
 *	same as above, except init to exactly <length> bytes.
 *	NULL terminate.
 *	
 */
boolean
str32ninit (str32 *pstr, register char *cstr, register int len)
{
    register char *pp = STR32PTR(pstr);

    if (!INRANGE(0,len,32))
        return(FALSE);

    pstr->str[0] = (uchar) len;

    while (len--)
        *pp++ = *cstr++;

    *pp ='\0';
    return(TRUE);
}

/*
 * str32cpy -
 *	
 *	copy <src> into <dst>, w/o null termination.  true if okay.
 *	
 */
boolean
str32cpy (str32 *dst, str32 *src)
{
    if (STR32LEN(src) > 32)	/* src->str[0] is a uchar and never negative */
        return(FALSE);  /* error */

    STR32CPY(dst, src);

    return(TRUE);
}

/*
 * str32len -
 *	
 *	return the length of <pstr>.
 *	
 */
int
str32len (str32 *pstr)
{
    return(pstr->str[0]);
}

/*
 * str32ptr -
 *	
 *	return a pointer to the beginning of the null-terminated
 *	C string stored in <pstr>.
 *	
 */
char *
str32ptr (str32 *pstr)
{
    return(STR32PTR(pstr));
}

/*
 * str32match -
 *	
 *	true if <s1> matches <s2>.  case insensitive.
 *	
 */
boolean
str32match (str32 *s1, str32 *s2)
{
    int ii;

    if (STR32LEN(s1) != STR32LEN(s2))
        return (FALSE);
    
    for (ii = 1; ii <= STR32LEN(s1); ii++)
        if (atalk_upper(s1->str[ii]) != atalk_upper(s2->str[ii]))
            return (FALSE);

    return (TRUE);
}


/*
 * bitmod.c - implement bit arrays.
 */


typedef uchar bitword;
#define ELEM_SIZE   8   /* size in bits of bitword */
#define ALL_ONES    0xff    /* a bitword full of ones */

typedef struct {
    ushort valid;      /* set to BITMOD_VALID for safety check */
    ushort bsize;      /* size in bits from bit_new() call */
    ushort esize;      /* size of bitstor in elements */
    bitword *bitstor;   /* pointer to bitarray */
} t_bitarray;

#define BITMOD_VALID    0x0B17

/* bit position masks: */
static bitword const bitmask[] = {
    0x80,       /* bit 0 */
    0x40,       /* bit 1 */
    0x20,       /* bit 2 */
    0x10,       /* bit 3 */
    0x08,       /* bit 4 */
    0x04,       /* bit 5 */
    0x02,       /* bit 6 */
    0x01        /* bit 7 */
};

#define BITVAL(P,X)\
    ((P)->bitstor[(X)/ELEM_SIZE] & bitmask[(X)%ELEM_SIZE]?TRUE:FALSE)

#define BITSET(P,X)\
    (P)->bitstor[(X)/ELEM_SIZE] |= bitmask[(X)%ELEM_SIZE]

#define BITCLR(P,X)\
    (P)->bitstor[(X)/ELEM_SIZE] &= ~(bitmask[(X)%ELEM_SIZE])


/*************************  PRIVATE FUNCTIONS  ************************/

/*
 * bit_validate -
 *	check users handle
 */
static boolean
bit_validate (t_bitarray *bb, char *msg)
{
    if (ODDPTR(bb)) {
        at_err(MODID,"%s: oddpointer, %lx", msg, bb);
        return (FALSE);
    }

    if (bb->valid != BITMOD_VALID) {
        at_err(MODID,"%s: bad handle, %lx", msg, bb);
        return (FALSE);
    }

    return (TRUE);
}


/*************************  EXPORTED FUNCTIONS  ***********************/

/*
 * bit_new -
 *	return a new bit array pointer in <*b> of <size> bits.
 *	clear all bits initially.
 *	
 */
boolean
bit_new (bitarray *b, int size)
{
    register t_bitarray *bb;

    bb = malloc(sizeof(t_bitarray));
    if (bb ==  (t_bitarray *) NULL) {
        *b = NILBITARRAY;
        return(FALSE);
    }

    bb->esize = size/ELEM_SIZE;

    /* allocate one more if not evenly divisible: */
    if (size%ELEM_SIZE > 0)
        ++bb->esize;

    bb->bitstor = malloc(bb->esize);
    if (bb->bitstor ==  (bitword *) NULL) {
        free(bb);
        *b = NILBITARRAY;
        return(FALSE);
    }

    /* set remaining fields: */
    bb->valid = BITMOD_VALID;
    bb->bsize = size;

    *b = (bitarray) bb;

    /* clear all bits: */
    bit_clrall(*b);
    return(TRUE);
}

/*
 * bit_free -
 *	free bitarray <b>, NIL out caller's handle
 */
void
bit_free (bitarray *b)
{
    register t_bitarray *bb = (t_bitarray *) *b;

    if (!bit_validate(bb, "bit_free"))
        return;

    /* free storage: */
    free(bb->bitstor);
    free(bb);

    *b = NILBITARRAY;
}

/*
 * bit_nclr -
 *	how many bits are clear in bitarray <b>?
 */
int
bit_nclr (bitarray b)
{
    register t_bitarray *bb = (t_bitarray *) b;
    register int ii,nclr;

    if (!bit_validate(bb, "bit_nclr"))
        return(-1);

    for (ii=0,nclr=0; ii<bb->bsize; ii++)
        if (!BITVAL(bb,ii))
            ++nclr;

    return(nclr);
}

/*
 * bit_nset -
 *	how many bits are set in bitarray <b>?
 */
int
bit_nset (bitarray b)
{
    register t_bitarray *bb = (t_bitarray *) b;
    register int ii,nset;

    if (!bit_validate(bb, "bit_nset"))
        return(-1);

    for (ii=0,nset=0; ii<bb->bsize; ii++)
        if (BITVAL(bb,ii))
            ++nset;

    return(nset);
}

/*
 * bit_nbits -
 *	how many bits are available in bitarray <b>?
 */
int
bit_nbits (bitarray b)
{
    register t_bitarray *bb = (t_bitarray *) b;

    if (!bit_validate(bb, "bit_nbits"))
        return(-1);

    return(bb->bsize);
}

/*
 * bit_clrall -
 *	clear all bits in bitarray <b>.
 */
void
bit_clrall (bitarray b)
{
    register t_bitarray *bb = (t_bitarray *) b;
    register int ii;

    if (!bit_validate(bb, "bit_clrall"))
        return;

    for (ii=0; ii<bb->esize; ii++)
        bb->bitstor[ii] = 0;
}

/*
 * bit_val -
 *	true if nth bit of <b> is set, o'wise false
 */
boolean
bit_val (bitarray b, int n)
{
    register t_bitarray *bb = (t_bitarray *) b;

    if (!bit_validate(bb, "bit_val"))
        return(FALSE);

    if (!INRANGE(0, n, bb->bsize-1)) {
        at_err(MODID,"bit_val:  bit out of range");
        return(FALSE);
    }

    /* get the value of the nth bit: */
    return(BITVAL(bb,n));
}

/*
 * bit_set -
 *	set bit <n> of <b> to 1
 */
void
bit_set (bitarray b, int n)
{
    register t_bitarray *bb = (t_bitarray *) b;

    if (!bit_validate(bb, "bit_set"))
        return;

    if (!INRANGE(0, n, bb->bsize-1)) {
        at_err(MODID,"bit_set:  bit out of range");
        return;
    }

    /* set the nth bit: */
    BITSET(bb,n);
}

/*
 * bit_clr -
 *	clear bit <n> of <b>
 */
void
bit_clr (bitarray b, int n)
{
    register t_bitarray *bb = (t_bitarray *) b;

    if (!bit_validate(bb, "bit_clr"))
        return;

    if (!INRANGE(0, n, bb->bsize-1)) {
        at_err(MODID,"bit_clr:  bit out of range");
        return;
    }

    /* clear the nth bit: */
    BITCLR(bb,n);
}

/*
 * bit_next -
 *	find the next bit satisfying <bittest> starting at <*nn>,
 *	scanning the bit array in INCREASING order.
 *
 *	if <bittest> is true, find the next SET bit, otherwise,
 *	find the next CLEAR bit.
 *	
 */
boolean
bit_next (bitarray b, register boolean bittest, ushort *nn)
{
    register t_bitarray *bb = (t_bitarray *) b;
    register bitword elemtest;
    register int ii, jj, base;

    if (!bit_validate(bb, "bit_next"))
        return(FALSE);

    if (*nn > bb->bsize-1) {    /* *nn is unsigned and never negative */
        at_err(MODID,"bit_next:  %d not in {%d, %d}", 0, bb->bsize-1);
        return(FALSE);
    }

    if (bittest) {
	bittest = TRUE;		/* can't just be non-zero */
	elemtest = 0;		/* continue if elem. has opposite */
    }
    else {
	bittest = FALSE;
	elemtest = ALL_ONES;
    }

    /* foreach element in bit array... */
    for (ii= *nn/ELEM_SIZE; ii < bb->esize; ++ii) {

	/* if passes element test... */
	if (bb->bitstor[ii] == elemtest)
	    continue;

	/* adjust starting bit if lower bound: */
        base = ii*ELEM_SIZE;
	if (base < *nn)
	    jj = *nn - base;
	else
	    jj = 0;

        /* foreach bit in iith element... */
        for(; jj<ELEM_SIZE && base+jj< bb->bsize; jj++) {
            /* if passes bit test: */
            if (BITVAL(bb,base+jj) == bittest) {
                *nn = (base+jj);
                return(TRUE);
            }
        }
    }

    return(FALSE);
}

/*
 * bit_last -
 *	find the next bit satisfying <bittest> starting at <*nn>,
 *	scanning the bit array in DECREASING order.
 *
 *	if <bittest> is true, find the next SET bit, otherwise,
 *	find the next CLEAR bit.
 *	
 */
boolean
bit_last (bitarray b, register boolean bittest, ushort *nn)
{
    register t_bitarray *bb = (t_bitarray *) b;
    register int ii, jj, base;
    register bitword elemtest;

    if (!bit_validate(bb, "bit_last"))
        return(FALSE);

    if (*nn > bb->bsize-1) {    /* *nn is unsigned and never negative */
        at_err(MODID,"bit_last:  %d not in {%d, %d}", 0, bb->bsize-1);
        return(FALSE);
    }

    if (bittest) {
	bittest = TRUE;		/* can't just be non-zero */
	elemtest = 0;		/* continue if elem. has opposite */
    }
    else {
	bittest = FALSE;
	elemtest = ALL_ONES;
    }

    /* foreach element in bit array... */
    for (ii=*nn/ELEM_SIZE; ii >= 0; --ii) {

	/* if passes element test... */
	if (bb->bitstor[ii] == elemtest)
	    continue;

	/* adjust starting bit if upper bound: */
        base = ii*ELEM_SIZE;
	if ((base+ELEM_SIZE) > *nn)
	    jj = *nn - base;
	else
	    jj = ELEM_SIZE-1;
        /* foreach bit within bounds in iith element... */
        for( ; jj>=0; jj--) {
            /* if passes bit test: */
            if (BITVAL(bb,base+jj) == bittest) {
                *nn = (base+jj);
                return(TRUE);
            }
        }
    }

    return(FALSE);
}


/*--	END MODULE bitmod.c	--*/


/*--	BEGIN MODULE id.c	--*/

/*
 * id.c - implement id allocation pools.
 */


/* true id descriptor: */
typedef struct {
    ushort valid;      /* set to ID_PATTERN for safety check */
    ushort lodcl;      /* lower bound of declared range */
    ushort hidcl;      /* upper bound of declared range */
    ushort lowest;	/* lowest id allocated */
    ushort highest;	/* highest id allocated */
    ushort cursor;     /* next id available for assignment */
    bitarray delcache;  /* the delete cache, NILBITARRAY if none */
} t_iddesc;

#define ID_PATTERN  0x1DEF

/* macro to zero adjust an id range for bitmap operations: */
#define ZADJUST(I,H)    ((I) - ((H)->lodcl))

/* macro to adjust a bitmap range to an ID range: */
#define RANGE_ADJUST(I,H)    ((I) + ((H)->lodcl))

/* macro to set cursor to next available id, and
 * update lowest..highest allocated range.
 */
#define INCR_CURSOR(B) {\
	if ((B)->cursor == NILID)\
	    (B)->highest = (B)->lowest = (B)->cursor = (B)->lodcl;\
	else if ((B)->cursor < (B)->hidcl) {\
	    ++(B)->cursor;\
	    if ((B)->cursor > (B)->highest)\
		(B)->highest = (B)->cursor;\
	}\
	else if ((B)->cursor >= (B)->hidcl) {\
	    (B)->cursor = (B)->lodcl;\
	    if ((B)->cursor < (B)->lowest)\
		(B)->lowest = (B)->cursor;\
	}\
    }
#define ADJUST_CURSOR(B,I) {\
	if (!INRANGE((B)->lodcl,(I),(B)->hidcl))\
	    at_err(MODID,"(B)->cursor adjust, %d:{%d..%d}",\
		(I), (B)->lodcl, (B)->hidcl);\
	else {\
	    (B)->cursor = (I);\
	    if ((B)->cursor > (B)->highest)\
		(B)->highest = (B)->cursor;\
	    if ((B)->cursor < (B)->lowest)\
		(B)->lowest = (B)->cursor;\
	}\
    }


/*************************  PRIVATE FUNCTIONS  ************************/

/*
 * id_validate -
 *	check users handle
 */
static boolean
id_validate (t_iddesc *hh, char *msg)
{
    if (ODDPTR(hh)) {
        at_err(MODID,"%s: oddpointer, %lx", msg, hh);
        return(FALSE);
    }

    if (hh->valid != ID_PATTERN) {
        at_err(MODID,"%s: bad handle, %lx", msg, hh);
        return(FALSE);
    }

    return(TRUE);
}


/*************************  EXPORTED FUNCTIONS  ***********************/

/*
 * id_delid -
 *	mark <id> from pool <idh> as available for reallocation.
 */
void
id_delid (iddesc idh, ushort id)
{
    register t_iddesc *hh = (t_iddesc *) idh;
    ushort ii;

    if (!id_validate(hh, "id_delid"))
        return;

    if (hh->delcache == NILBITARRAY)
        return;     /* no cache */

    if (hh->cursor == NILID) {
        return;		/* none allocated */
    }

    if (!INRANGE(hh->lowest,id,hh->highest)) {
        return;		/* id not allocated */
    }

    /* clear the bit in the delete cache: */
    ii = ZADJUST(id,hh);    /* zero adjust */
    bit_clr(hh->delcache, ii);

    /* if highest was cleared, find next highest: */
    if (id == hh->highest) {
	if (bit_last(hh->delcache, TRUE, &ii)) {
	    hh->highest = RANGE_ADJUST(ii, hh);
	}
	else {
	    id_reset(idh);	/* nothing allocated, reset */
	}
    }
    else	/* if lowest was cleared... */
    if (id == hh->lowest) {
	if (bit_next(hh->delcache, TRUE, &ii)) {
	    hh->lowest = RANGE_ADJUST(ii, hh);
	}
	else {
	    id_reset(idh);	/* nothing allocated, reset */
	}
    }
}

/*
 * id_free -
 *	free an id descriptor and its storage, NIL user's handle.
 */
void
id_free (iddesc *idh)
{
    register t_iddesc *hh = (t_iddesc *) *idh;

    if (!id_validate(hh, "id_free"))
        return;

    if (hh->delcache != NILBITARRAY) {
        bit_free(&hh->delcache);
    }

    free(hh);
    *idh = NILIDDESC;
}

/*
 * id_getid -
 *	get the next <*id> from pool <idh>
 */
boolean
id_getid (iddesc idh, ushort *id)
{
    register t_iddesc *hh = (t_iddesc *) idh;
    ushort ii;

    if (!id_validate(hh, "id_getid"))
        return(FALSE);

    INCR_CURSOR(hh);

    /* if no cache... */
    if (hh->delcache == NILBITARRAY) {
        *id = hh->cursor;
        return(TRUE);
    }

    /* otherwise, have a delete cache: */
    ii = ZADJUST(hh->cursor, hh);    /* zero adjust */
    if (!bit_next(hh->delcache, FALSE, &ii)) {
	/* try again, going the other way: */
	if (!bit_last(hh->delcache, FALSE, &ii))
	    return(FALSE);	/* no id's available */
    }

    /* found one, mark as allocated: */
    bit_set(hh->delcache, ii);

    /* update cursor & allocated range: */
    ii  = RANGE_ADJUST(ii, hh);		/* adjust bit to range */
    ADJUST_CURSOR(hh, ii);
    *id = hh->cursor;			/* set result */
    return(TRUE);
}

/*
 * id_new -
 *	allocate new id handle, <*idh>, which will allocate
 *	id's in the range <low>..<hi>.  If delcache is true,
 *	will reallocate deleted id's.
 *	
 */
boolean
id_new (iddesc *idh, ushort low, ushort hi, boolean delcache)
{
    register t_iddesc *hh;

    if (low >= hi || hi == NILID) {
        return(FALSE);
    }

    hh = malloc(sizeof(t_iddesc));
    if (hh ==  (t_iddesc *) NULL) {
        /* out of memory: */
        *idh = NILIDDESC;
        return(FALSE);
    }


    /* if want delete cache... */
    if (delcache) {
        if (!bit_new(&hh->delcache, (hi-low)+1)) {
            /* out of memory: */
            free(hh);
            *idh = NILIDDESC;
            return(FALSE);
        }
    }
    else {
        hh->delcache = NILBITARRAY;
    }

    hh->valid = ID_PATTERN;
    hh->lodcl = low;
    hh->hidcl = hi;

    *idh = (iddesc) hh;

    id_reset(*idh);     /* initialize remaining fields */
    return(TRUE);
}

/*
 * id_reset -
 *	reset an id pool
 */
void
id_reset (iddesc idh)
{
    register t_iddesc *hh = (t_iddesc *) idh;

    if (!id_validate(hh, "id_reset"))
        return;

    /* initialize cursor, allocate range */
    hh->cursor = hh->highest = hh->lowest = NILID;

    /* reset bit-map of allocated ID's */
    if (hh->delcache != NILBITARRAY)
        bit_clrall(hh->delcache);
}

/*
 * id_first -
 *	
 *	return the first id allocated from pool <idh>.
 *	if none allocated, then return NILID.
 *	
 */
ushort
id_first (iddesc idh)
{
    register t_iddesc *hh = (t_iddesc *) idh;

    if (!id_validate(hh, "id_first"))
        return(NILID);

    return(hh->lowest);
}

/*
 * id_next -
 *	return the next id allocated from pool <idh>, after <idx>.
 *	if not allocated, then return NILID, unless <idx> is below
 *	declared range, in which case return id_first().
 *
 *	if simple range (no delete cache), then just return
 *	next in range.
 */
ushort
id_next (iddesc idh, register ushort idx)
{
    register t_iddesc *hh = (t_iddesc *) idh;
    ushort ii;

    if (!id_validate(hh, "id_next"))
        return(NILID);

    /* next is only defined for the range lodcl..highest-1 */
    if (!INRANGE(hh->lowest, idx, hh->highest - 1)) {
	/* if in declared range, but below allocated range... */
	if (INRANGE(hh->lodcl, idx, hh->lowest-1))
	    return(hh->lowest);		/* ... return first */
	else
	    return(NILID);
    }

    ++idx;

    /* if no cache... */
    if (hh->delcache == NILBITARRAY) {
	    return(idx);		/* ... done */
    }

    /* otherwise, have a delete cache. scan forward to next allocated: */
    ii = ZADJUST(idx,hh);
    if (bit_next(hh->delcache, TRUE, &ii)) {
        return(RANGE_ADJUST(ii, hh));
    }

    /* next is out of range: */
    return(NILID);
}

/*
 * id_last -
 *	
 *	return the last id allocated from pool <idh>.
 *	if none allocated, then return NILID.
 *	
 */
ushort
id_last (iddesc idh)
{
    register t_iddesc *hh = (t_iddesc *) idh;

    if (!id_validate(hh, "id_last"))
        return(FALSE);

    return(hh->highest);
}

/*--	END MODULE id.c	--*/


/*--	BEGIN MODULE symbols.c	--*/

/*
 * symbols.c - storage for symbol to string mappings
 */


typedef struct symbol_entry_ {
    struct symbol_entry_ *link;
    symbol  sym;
    short   refcnt;
    short   length;
    char    *canonical;
    char    *original;
} symbol_entry;

#define SYM_HASH_SIZE   128

/* the true symbol table structure: */
typedef struct t_symtab_ {
    PFI canonicalFunc;      /* make a character canonical */
    iddesc symidh;          /* symbol allocator */
    char *name;         /* name assigned to table */
    symbol_entry *table[SYM_HASH_SIZE];
} t_symtab;

/* for now, use one id allocator for all symbol tables. */
#define FIRST_SYM   (ushort) 1
#define LAST_SYM    (ushort) 0xfffe
    /* warning:  NILID is MAX_USHORT */

/* check for absurd lengths: */
#define MAX_SYM_LEN 128

static iddesc gsymidh;          /* global symbol allocator (see sy_new()) */


/*************************  PRIVATE FUNCTIONS  ************************/

/*
 * def_canFunc -
 *	default canonical mapping function:  map to upper-case
 */
static int
def_canFunc (int c)
{
    if ('a' <= c && c <= 'z')
    return(c - 040);
    else
    return(c);
}

/*
 * sy_hash -
 *	hash algorithm for symbol lookups
 */
static inline int
sy_hash (char *str, int len)
{
return(((u_long) ((len*str[0]) + str[len-1]) & 0x7fffffff) % SYM_HASH_SIZE);
}

/*
 * mk_canonical -
 *	convert <str> to canonical form, save in <can>
 */
static void
mk_canonical (char *can, char *str, int length, t_symtab *tt)
{
    register int ii;

    for (ii=0; ii<length; ii++)
        can[ii] = (char) (*tt->canonicalFunc)(str[ii]);

    can[length] = '\0';
}

/*
 * str_lookup -
 *	return the symbol_entry for <str> in <tt> or NULL
 */
static symbol_entry *
str_lookup (t_symtab *tt, register char *str, register int ll)
{
    int slot;
    register symbol_entry *se;
    static char bufstor[MAX_SYM_LEN];	/* save stack space */
    register char *buff = bufstor;

    if (ll >= MAX_SYM_LEN)
        return(NULL);	     /* string is too long */

    mk_canonical(buff, str, ll, tt);

    /* get hash slot: */
    slot = sy_hash(buff, ll);

    /* search chain: */
    for (se = tt->table[slot]; se != NULL; se = se->link) {
	/* don't compare unless lengths are the same equal: */
        if (se->length == ll) {
	    if (STREQ(se->canonical, buff))
		return(se);     /* found! */
        }
    }

    return(NULL);
}

/*
 * sym_lookup -
 *	return string associated with <sym>, in original form
 */
static symbol_entry *
sym_lookup (t_symtab *tt, symbol sym)
{
    register int ii;
    register symbol_entry *se;

    /* if symbol not allocated... */
    if (!INRANGE(id_first(tt->symidh), sym, id_last(tt->symidh)))
        return(NULL);

    /* o'wise, find the entry: */
    for (ii=0; ii<SYM_HASH_SIZE; ii++) {
        for (se = tt->table[ii]; se != NULL; se = se->link)
            if (se->sym == sym)
                return(se);
    }

    return(NULL);
}

/*
 * new_symbol -
 *	
 *	allocate and initialize a new symbol table entry.
 *	return symbol, or NILSYM if no memory.
 *	
 */
static symbol
new_symbol (t_symtab *tt, char *str, register int ll)
{
    symbol sym;
    register symbol_entry *se;
    int slot;

    if (ll >= MAX_SYM_LEN)
        return(NILSYM);     /* string is too long */

    /* allocate storage: */
    if (!id_getid(tt->symidh, &sym))
        return(NILSYM);     /* no more id's! */

    se = malloc(sizeof(symbol_entry));
    if (se == NULL) {
        /* backout: */
        id_delid(tt->symidh, sym);
        return(NILSYM);
    }

    /* allocate string storage: */
    se->canonical = malloc(ll+1);
    if (se->canonical == NULL) {
        /* backout: */
        id_delid(tt->symidh, sym);
        free(se);
        return(NILSYM);
    }
    se->original = malloc(ll+1);
    if (se->original == NULL) {
        /* backout: */
        id_delid(tt->symidh, sym);
        free(se->canonical);
        free(se);
        return(NILSYM);
    }

    se->refcnt = 1;
    se->sym = sym;
    se->length = ll;

    /* copy and make sure we're NULL terminated: */
    bcopy(str, se->original, ll);
    se->original[ll] = '\0';

    /* make the canonical version: */
    mk_canonical(se->canonical, str, ll, tt);

    /* get hash slot: */
    slot = sy_hash(se->canonical, ll);

    /* store in symbol table: */
    if (tt->table[slot] == NULL) {
        se->link = NULL;
        tt->table[slot] = se;
    }
    else {
        /* insert in front of chain: */
        se->link = tt->table[slot];
        tt->table[slot] = se;
    }

    return(sym);
}

static void
free_symbol(register symbol_entry *se)
{
    free(se->original);
    free(se->canonical);
    se->link = NULL;
    free(se);
}


/*************************  EXPORTED FUNCTIONS  ***********************/

/*
 * sy_append -
 *	store string <str> in symbol table.  if symbol has
 *	already been entered, increment reference count.
 *	
 */
symbol
sy_append (symbol_table st, register char *str)
{
    t_symtab *tt = (t_symtab *) st;
    register symbol_entry *se;
    register int ll = strlen(str);

    /* is string already entered? */
    if ((se = str_lookup(tt, str, ll)) != NULL) {
        ++se->refcnt;
        return(se->sym);
    }

    /* otherwise, add: */
    return(new_symbol(tt, str, ll));
}

/*
 * sy_append32 -
 *	add a possibly unterminated str32 to the table.
 *	
 */
symbol
sy_append32 (symbol_table st, register str32 *pstr)
{
    t_symtab *tt = (t_symtab *) st;
    register symbol_entry *se;

    /* is string already entered? */
    if ((se = str_lookup(tt, STR32PTR(pstr), STR32LEN(pstr))) != NULL) {
        ++se->refcnt;
        return(se->sym);
    }

    /* otherwise, add: */
    return(new_symbol(tt, STR32PTR(pstr), STR32LEN(pstr)));
}

/*
 * sy_delete -
 *	decrement ref counts to symbol. deallocate if no more refs
 */
void
sy_delete (symbol_table st, symbol sym)
{
    t_symtab *tt = (t_symtab *) st;
    register symbol_entry *se, *pp;
    int slot;

    /* is symbol entered? */
    if ((se = sym_lookup(tt, sym)) == NULL)
        return;     /* not entered */

    --se->refcnt;
    if (se->refcnt > 0) /* must have as many deletes as appends */
        return;

    /* otherwise, free symbol: */
    slot = sy_hash(se->canonical, strlen(se->canonical));

    /* if first in chain... */
    if (se == tt->table[slot]) {
        tt->table[slot] = se->link;
        free_symbol(se);
        return;
    }

    /* not first in chain, find element pointing to us: */
    for (pp = tt->table[slot]; pp != NULL; pp = pp->link) {
        if (pp->link == se) {
            pp->link = se->link;
            free_symbol(se);
            return;
        }
    }

    /* error!  symbol entered, but we can't find! */
    at_err(MODID,"sy_delete:  symtab '%s' corrupted! (%d: '%s')",
        tt->name,
        se->sym,
        se->original);
}

/*
 * sy_symbol -
 *	look up symbol of canonical form of string <str>.
 *	return NILSYM if not entered.
 *	
 */
symbol
sy_symbol (symbol_table st, register char *str)
{
    register t_symtab *tt = (t_symtab *) st;
    register symbol_entry *se;

    /* is string already entered? */
    if ((se = str_lookup(tt, str, strlen(str))) != NULL)
        return(se->sym);

    return(NILSYM);
}

/*
 * sy_symbol32 -
 *	
 *	retrieve symbol for a str32.
 *	
 */
symbol
sy_symbol32 (symbol_table st, register str32 *pstr)
{
    register t_symtab *tt = (t_symtab *) st;
    register symbol_entry *se;

    /* is string already entered? */
    if ((se = str_lookup(tt, STR32PTR(pstr), STR32LEN(pstr))) != NULL)
        return(se->sym);

    return(NILSYM);
}

/*
 * sy_string -
 *	look up original string of symbol <sym>.
 *	return NILSTR if not entered.
 *	
 */
char *
sy_string (symbol_table st, symbol sym)
{
    t_symtab *tt = (t_symtab *) st;
    symbol_entry *se;

    if ((se = sym_lookup(tt,sym)) != NULL)
        return(se->original);

    return(NILSTR);	/* return someting to notice */
}

/*
 * sy_init -
 *	
 *	initialize storage for symbols module.
 *	
 */
symbol_table
sy_init (char *tablename, PFI canfunc, sy_result *err)
{
    register int ii;
    t_symtab *tt;

    /* malloc new symbol table: */
    tt = malloc_named(sizeof(t_symtab), "MacIP Symbols");
    if (tt == NULL) {
        *err = SY_NO_MEMORY;
        return((symbol_table) NULL);
    }

    /* init symbol-id allocator:
     * WARNING: the symbol allocator has to be global for
     * all symbol tables to ensure unique database keys.
     * (id allocator can become local when database module is
     * converted to allow multiple databases, right now is
     * implemented as one global db. RT 6/29/92)
     */
    if (gsymidh == NULL) {
	if (!id_new(&gsymidh, FIRST_SYM, LAST_SYM, FALSE)) {
	    *err = SY_NO_MEMORY;
	    free(tt);
	    return((symbol_table) NULL);
	}
    }
    tt->symidh = gsymidh;

    tt->name = strdup(tablename);
    if (tt->name == NULL) {
        /* back out: */
	id_free(&tt->symidh);
        free(tt);
        *err = SY_NO_MEMORY;
        return((symbol_table) NULL);
    }

    /* init new symbol_table: */
    if (canfunc == NILPFI)
        tt->canonicalFunc = (PFI) def_canFunc;
    else
        tt->canonicalFunc = canfunc;

    for (ii=0; ii<SYM_HASH_SIZE; ii++)
        tt->table[ii] = NULL;

    *err = SY_OK;
    return((symbol_table) tt);
}



char *sy_resultStr(sy_result err)
{
    switch(err) {
    case SY_OK:     return("SY_OK");
    case SY_NO_MEMORY:  return("SY_NO_MEMORY");
    default:        return("sy_resultStr:  bad arg.");
    }
}

/*
 * sy_destroy -
 *	
 *	deallocate a symbol table and all it's symbols
 *	
 */
void
sy_destroy (symbol_table st)
{
    register int ii;
    register symbol_entry *se, *sav;
    register t_symtab *tt = (t_symtab *) st;

    /* foreach symbol entry... */
    for (ii=0; ii<SYM_HASH_SIZE; ii++) {
        se = tt->table[ii];
	while (se) {
            sav = se;
	    se = se->link;
            /* free the entry: */
	    free_symbol(sav);
        }
    }

    /* free the table: */
    free(tt->name);
#ifdef	notdef
    /* see comments in sy_new() */
    id_free(&tt->symidh);
#endif
    free(tt);
}
/*--	END MODULE symbols.c	--*/


/*--	BEGIN MODULE db.c	--*/

/*
 * db.c - memory-based database manager.
 */


/* the structure of an internal storage key */
struct dbkey {
    struct dbkey *next;     /* next key in chain or NILDBKEY */
    key ukey;       /* user-defined key */
    byteptr data;       /* user data */
    short datasize;     /* user data size */
};

typedef struct dbkey dbkey;
#define NILDBKEY    (dbkey *) NILVAL

#define TABLESIZE   256
static dbkey **keytab;		/* the hash table */

/* when to give up on a chain: */
#define ABSURD_CHAIN    1000


/*************************  PRIVATE FUNCTIONS  ************************/

/*
 * hash -
 *	hashing algorithim for db module
 *	
 */
static int
hash (key *ukey)
{
    register long sum = 0;
    register int ii;

    /*
     * Results of hash tests for various keys:
     *
     * TABLESIZE=256
     * <slots_in_use>/<total_keys>, max=<maxchain>
     * (109/258, max=5):    sum += (ukey->keys[ii]);
     * (84/258, max=9): sum += (ukey->keys[ii])<<ii; sum = sum>>1;
     * (148/258, max=4):    sum += (ukey->keys[ii]) << ii;
     *
     * TABLESIZE=512
     * (109/258, max=5):    sum += (ukey->keys[ii]);
     * (151/258, max=4):    sum += (ukey->keys[ii]) << ii;
     */

    for (ii=0; ii < ukey->nkeys; ii++) {
        sum += (ukey->keys[ii]) << ii;
    }

    sum = sum % (long) TABLESIZE;

    /* force key to be positive: */
    return ((int) (sum < 0) ? -sum : sum);
}


/*************************  EXPORTED FUNCTIONS  ***********************/

/*
 * db_append -
 *	algorithim for db module
 *	
 */
boolean
db_append (database db, register key *keyptr, byteptr valptr, int size)
{
    register dbkey *dk;
    register int slot, jj;

    if (!INRANGE(1,keyptr->nkeys,MAXKEYS)) {
        at_err(MODID,"db_append, bad key");
        return(FALSE);
    }

    /* get the hash slot for our key: */
    slot = hash(keyptr);

    /* look for duplicates: */
    dk = keytab[slot];
    jj = 0;
    while (dk != NILDBKEY && jj < ABSURD_CHAIN) {
        if (dk->ukey.nkeys != keyptr->nkeys) {
            ;				/* key size must agree for match */
        } else {
	    if (BSTREQ((byteptr)&(dk->ukey),(byteptr)keyptr,
		       UKSIZE(keyptr))) {
		if (at_debug(atalkerr_debug, ATALK_NULL_MASK))
		    buginf("\nAT: db_append: duplicate keys\n");
		return (FALSE);		/* allow no duplicate keys */
	    }
	}
	dk = dk->next;
        ++jj;
    }

    if (jj >= ABSURD_CHAIN) {
        at_err(MODID,"db_append, absurd hash chain");
        return (FALSE);
    }

    /* get a new key: */
    if ((dk = malloc(sizeof(dbkey))) == (dbkey *) NULL) {
        at_err(MODID,"db_append - out of memory! (dbkey)");
        return (FALSE);
    }

    if ((dk->data = malloc(size)) == (byteptr) NULL) {
        at_err(MODID,"db_append - out of memory! (user data %d)", size);
        return (FALSE);
    }

    /* initialize new key: */
    bcopy((byteptr)keyptr,(byteptr)&(dk->ukey), UKSIZE(keyptr));
        /* user key */
    bcopy(valptr,dk->data, size);       /* user data */
    dk->datasize = size;		/* size of user data */

    /* insert new key at front of slot's list: */
    dk->next = keytab[slot];		/* NILDBKEY the first time */
    keytab[slot] = dk;

    if (at_debug(atalkerr_debug, ATALK_NULL_MASK))
        db_check(db);

    return (TRUE);			/* SUCCESS */
}

/*
 * db_retrieve -
 *	
 *	retrieve value based on key.  if <valptr> is NIL, don't
 *	copy.
 *	
 */
boolean
db_retrieve (database db, byteptr valptr, key *keyptr, int size)
{
    register dbkey *dk;
    register int slot, jj;

    if (!INRANGE(1,keyptr->nkeys,MAXKEYS)) {
        at_err(MODID,"db_retrieve, bad key");
        return(FALSE);
    }

    /* get the hash slot for our key: */
    slot = hash(keyptr);

    /* look up data based on key.
     * note that the user must correctly specify the
     * size of the object.
     */
    dk=keytab[slot];
    jj=0;
    while (dk != NILDBKEY && jj < ABSURD_CHAIN) {
        if (dk->ukey.nkeys != keyptr->nkeys) {
            ;   /* key size must agree for match */
        } else {
	    if (BSTREQ((byteptr)&(dk->ukey),(byteptr)keyptr,
		       UKSIZE(keyptr))) {
		/* found - copy data: */
		if (dk->datasize != size) {
		    at_err(MODID,"db_retrieve:  datasize mismatch %d!=%d",
			   dk->datasize,size);
		    return(FALSE);
		}
                
		/* okay to have NILPTR arg for "exists" operation */
		if (valptr != NILPTR)
		    bcopy(dk->data,valptr,size);

#if 0
		if (at_debug(atalkmacip_debug, ATALK_NULL_MASK)) {
		    buginf("\nAT: db_retrieve, got dbkey=%s, user key=%s",
			   db_keyStr(&dk->ukey), db_keyStr(keyptr));
		}
#endif
		return (TRUE);
	    }
	}

        dk = dk->next;
        ++jj;
    }
    if (at_debug(atalkerr_debug, ATALK_NULL_MASK))
        db_check(db);

    if (jj >= ABSURD_CHAIN) {
        at_err(MODID,"db_retrieve, absurd hash chain");
        return (FALSE);
    }

    return (FALSE);			/* not found */
}

/*
 * db_delete -
 *
 *	value based on key.  if <valptr> is NIL, don't
 *
 *
 */
boolean
db_delete (database db, key *keyptr)
{
    dbkey 	*dk;
    dbkey	*last;
    int		slot;
    int		jj;

    if (!INRANGE(1,keyptr->nkeys,MAXKEYS)) {
        at_err(MODID,"db_delete, bad key");
        return (FALSE);
    }

    /* get the hash slot for our key: */
    slot = hash(keyptr);

    /*
     * look up data based on key.
     * note that the user must correctly specify the
     * size of the object.
     */
    dk = keytab[slot];
    jj = 0;
    last = NILDBKEY;
    while (dk != NILDBKEY && jj < ABSURD_CHAIN) {
        if (dk->ukey.nkeys != keyptr->nkeys) {
            ;   /* key size must agree for match */
        } else {
	    if (BSTREQ((byteptr)&(dk->ukey),(byteptr)keyptr, UKSIZE(keyptr))) {
		/* found - delete data and key: */
		free(dk->data);
		
		/* unlink key: */
		if (last == NILDBKEY) {
		    /* first in chain: */
		    keytab[slot] = dk->next;
		} else {
		    last->next = dk->next;
		}
		
		/* free key: */
		free(dk);
		return (TRUE);
	    }
	}
	
        last = dk;
        dk = dk->next;
        ++jj;
    }
    if (at_debug(atalkerr_debug, ATALK_NULL_MASK))
        db_check(db);

    if (jj >= ABSURD_CHAIN) {
        at_err(MODID,"db_delete, absurd hash chain");
        return (FALSE);
    }

    return (FALSE);			/* not found */
}

/*
 * db_replace -
 *	
 *	based on key.  if <valptr> is NIL, don't
 *	
 *	
 */
boolean
db_replace (database db, key *keyptr, byteptr valptr, int size)
{
    register dbkey *dk;
    register int slot, jj;

    if (!INRANGE(1,keyptr->nkeys,MAXKEYS)) {
        at_err(MODID,"db_replace, bad key");
        return(FALSE);
    }
    if (valptr == NILPTR) {
        at_err(MODID,"db_replace, NIL value");
        return(FALSE);
    }

    /* get the hash slot for our key: */
    slot = hash(keyptr);

    /* look up data based on key.
     * note that the user must correctly specify the
     * size of the object.
     */
    dk=keytab[slot];
    jj=0;
    while (dk != NILDBKEY && jj < ABSURD_CHAIN) {
        if (dk->ukey.nkeys != keyptr->nkeys) {
            ;   /* key size must agree for match */
        } else {
	    if (BSTREQ((byteptr)&(dk->ukey),(byteptr)keyptr,
		       UKSIZE(keyptr))) {
		    /* found - replace data: */
		    if (dk->datasize != size) {
			at_err(MODID,"db_replace:  datasize mismatch %d!=%d",
			       dk->datasize,size);
			return(FALSE);
		    }
                
		    bcopy(valptr,dk->data, size);

		    return(TRUE);
		}
	}

        dk=dk->next;
        ++jj;
    }
    if (at_debug(atalkerr_debug, ATALK_NULL_MASK))
        db_check(db);

    if (jj >= ABSURD_CHAIN) {
        at_err(MODID,"db_replace, absurd hash chain");
        return (FALSE);
    }

    return (FALSE);			/* not found */
}

/*
 * db_mkkey -
 *	form an n-ary key of up to MAXKEYS
 *	
 */
void
db_mkkey (key *kptr, int nkeys, ...)
{
    va_list	args;
    int		ii;

    if (!INRANGE(1,nkeys,MAXKEYS)) {
        at_err(MODID,"db_mkkey:  bad arg");
        return;
    }

    va_start(args, nkeys);

    for (ii = 0; ii < nkeys; ii++)
        kptr->keys[ii] = va_arg(args,short);

    kptr->nkeys = nkeys;
#if 0
    if (at_debug(atalkerr_debug, ATALK_NULL_MASK))
        buginf("\nAT: db_mkkey  returns (%s)\n", db_keyStr(kptr));
#endif
    va_end(args);
}

/*
 * db_init -
 *	TODO:  implement multiple db's
 */
database
db_init (char *dbname, db_result *err)
{
    int		ii;
    static int	ninits = 0;

    if (ninits > 0) {
        *err = DB_NAME_INUSE;
        return ((database) 0);
    }

    keytab = malloc_named(sizeof(dbkey *) * TABLESIZE, "MacIP Database");
    if (keytab == NULL) {
	errmsg(AT_ERR_NOMEM, "db hash table", __LINE__, __FILE__);
	return ((database) 0);
    }
	
    /* for each slot: */
    for (ii = 0; ii < TABLESIZE; ii++) {
        keytab[ii] = NILDBKEY;
    }

    *err = DB_OK;
    ++ninits;
    return ((database) 1);
}

/*
 * db_check -
 *	check the db
 */
void
db_check (database db)
{
    int		ii;
    int		jj;
    dbkey 	*dk;
    long	occupied = 0;
    long	maxchain = 0;
    long	tkeys = 0;
    long	tbytes = 0;

    for (ii = 0; ii < TABLESIZE; ii++) {
        dk = keytab[ii];
        if (dk != NILDBKEY) {
            ++occupied;
        }
        jj = 0;
        while (dk != NILDBKEY) {
            ++jj;
            if (jj >= ABSURD_CHAIN)
                break;

            ++tkeys;
            tbytes += dk->datasize;

            dk = dk->next;
        }

        if (jj >= ABSURD_CHAIN)
	    if (at_debug(atalkerr_debug, ATALK_NULL_MASK))
		buginf("\nAT: db_check:  bad slot, %d", ii);

        if (jj > maxchain)
            maxchain = jj;
    }
}

/*
 * db_keyStr -
 *	
 *	return string representation of a db key
 *	
 */
char *
db_keyStr (key *keyptr)
{
    register ii;
    char *buff = string_getnext();
    register char *pp = &buff[0];

    sprintf(pp, "%dn: ", keyptr->nkeys);
    pp += strlen(pp);

    for (ii=0; ii< keyptr->nkeys; ii++) {
        sprintf(pp," %d", keyptr->keys[ii]);
        pp += strlen(pp);
    }

    return(buff);
}

/*
 * db_resultStr -
 *	
 *	string representation of a db key
 *	
 */
char *
db_resultStr (db_result err)
{
    switch(err) {
    case    DB_OK:      return("DB_OK");
    case    DB_NAME_INUSE:  return("DB_NAME_INUSE");
    default:        return("db_resultStr:  bad arg");   
    }
}

/*--	END MODULE db.c	--*/
