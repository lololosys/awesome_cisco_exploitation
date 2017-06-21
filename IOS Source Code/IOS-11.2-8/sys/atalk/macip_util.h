/* $Id: macip_util.h,v 3.2.60.2 1996/04/19 14:54:52 che Exp $
 * $Source: /release/112/cvs/Xsys/atalk/macip_util.h,v $
 *------------------------------------------------------------------
 * macip_util.h	- include file for users of macip_util.c
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
 * Modifications Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: macip_util.h,v $
 * Revision 3.2.60.2  1996/04/19  14:54:52  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.2.68.2  1996/04/10  23:06:00  lol
 * Branch: IosSec_branch
 *         Incorporate code review comments.
 *
 * Revision 3.2.68.1  1996/04/09  19:52:43  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 3.2.60.1  1996/03/18  18:53:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/09  05:04:08  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.26.1  1996/02/20  13:34:40  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:45:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:59:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:51:19  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/10/20  20:24:37  lol
 * CSCdi42253:  callback causes IPCP authorization failures
 *              Only return callback info when doing local authorization of
 *              ppp/lcp, slip, arap or exec
 *
 * Revision 2.1  1995/06/07  20:10:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__MACIP_UTIL_H__
#define	__MACIP_UTIL_H__

#define NILVAL  -1
#define NIL(TYPE)   ((TYPE *) NILVAL)
#define NILSTR  NIL(char)

/* universal success code: */
#define SUCCESS 0

typedef ulong		sys_addr;
typedef sys_addr	*sysptr;

#define NILSYSADDR  0

typedef ushort		*wordptr;
typedef uchar		*byteptr;

#define NILPTR  ((byteptr) NILVAL)

/* a pointer to a function taking an int returning an int: */
typedef int (*PFI)(int);
#define NILPFI  ((PFI) NILVAL)

struct server_rec_;
/* a "pointer to a function taking a struct * returning void" */
typedef void (*PFV)(struct server_rec_ *);
#define NILPFV  ((PFV) NILVAL)

#define INRANGE(L,X,U)  ((X)>=(L) && (X)<=(U))
#define EVEN(X) (((X) & 1) == 0)
#define ODD(X)  (((X) & 1) == 1)
#define ODDPTR(PTR) (((sys_addr)(PTR) & 1) == 1)
#define EVENPTR(PTR)    (((sys_addr)(PTR) & 1) == 0)
/* moved STREQ to string.h for use by a wider audience -- Lol */
#define STRCPY(to,from) bcopy((byteptr)from,(byteptr)to,strlen(from)+1)
#define STRCAT(old,new) bcopy((byteptr)new,(byteptr)(old+strlen(old)),strlen(new)+1)
#define EMPTYSTR(s) (s[0] == '\0')
#define BSTREQ(b1,b2,len)   (bcmp(b1,b2,len) == 0)
#define ATOI(S,P)     parse_unsigned(S,P)

#define ISDISPLAY_ASCII(C)	INRANGE(' ',C,'~')
#define ISGRAPHIC_ASCII(C)	INRANGE('!',C,'~')
#define ISDIGIT(C)  (INRANGE('0',C,'9'))
#define ISHEXDIGIT(C)\
    (ISDIGIT(C) || INRANGE('A',C,'F') || INRANGE('a',C,'f'))

/* convert a know ascii hex char to binary: */
#define HEXTOBIN(C)\
    (int) (ISDIGIT(C)?((C)-'0'):(INRANGE('A',C,'F')?((C)-'7'):((C)-'W')))
/* convert a number between 0-15 to hex char equivalent: */
#define BINTOHEX(X)\
    (char) (INRANGE(0,X,9)?((X)+'0'):(INRANGE(10,X,15)?(((X)-10)+'A'):'~'))

#define HI_NIBBLE(X) (((X)>>4) & 0x0f)
#define LO_NIBBLE(X) ((X) & 0x0f)

#if (CPU_68K || CPU_MIPS || CPU_SPARC)

/* macros to return most/least significant word: */
#define HI_16(I32)  ((ushort *)&(I32))[0]
#define LO_16(I32)  ((ushort *)&(I32))[1]

#endif  /* (CPU_68K || CPU_MIPS || CPU_SPARC) */

#ifndef	MODID
	/* also defined in atalk_private.h */
#define	MODID	__FILE__,__LINE__,DEBUGSWITCH
void at_err(char *file, int line, int debugsw, char *msgin, ...);
#endif


extern boolean debug_set(int n);
extern boolean debug(int n);


typedef byteptr bitarray;   /* exported type */

#define NILBITARRAY ((bitarray)(NILVAL))

extern boolean bit_new(bitarray *b, int size);
    /* return a new bit array pointer in <*b> of <size> bits.
     * clear all bits initially.
     */

extern void bit_free(bitarray *b);
    /* free bitarray <b>, NIL out caller's handle */

extern boolean bit_val(bitarray b, int n);
    /* true if nth bit of <b> is set, o'wise false */

extern void bit_set(bitarray b, int n);
    /* set bit <n> of <b> to 1 */

extern void bit_clr(bitarray b, int n);
    /* clear bit <n> of <b> */

extern void bit_clrall(bitarray b);
    /* clear all bits in <b> */

extern boolean bit_next (bitarray b, boolean bittest, ushort *nn);
	/* scan <b> in increasing order, return first index in <*nn>
	 * to satisfy <bittest> */

extern boolean bit_last (bitarray b, boolean bittest, ushort *nn);
	/* scan <b> in decreasing order, return first index in <*nn>
	 * to satisfy <bittest> */

extern int bit_nbits(bitarray b);
    /* how many bits in <b>? */
extern int bit_nclr(bitarray b);
    /* how many clear bits in <b>? */
extern int bit_nset(bitarray b);
    /* how many set bits in <b>? */


typedef byteptr iddesc;     /* exported type */

#define NILIDDESC   ((iddesc) NILVAL)

/* reserve one id for out-of range value: */
#define NILID   ((ushort) MAXUSHORT)

extern boolean id_new(iddesc *idh, ushort low, ushort hi, boolean delcache);
    /* allocate new id handle, <*idh>, which will allocate
     * id's in the range <low>..<hi>.  If delcache is true,
     * will reallocate deleted id's.
     */

extern void id_free(iddesc *idh);
    /* free an id descriptor and its storage */

extern void id_reset(iddesc idh);
    /* reset an id pool */

extern boolean id_getid(iddesc idh, ushort *newid);
    /* set <*newid> to the next available from pool <idh> */

extern void id_delid(iddesc idh, ushort oldid);
    /* mark <oldid> from pool <idh> as available for reallocation */

extern ushort id_first(iddesc idh);
    /* return the first id allocated from pool <idh>.
     * if none allocated, then return NILID.  */

extern ushort id_next(iddesc idh, ushort idx);
    /* return the next id allocated from pool <idh>, after <idx>.
     * if not allocated, then return NILID.  */

extern ushort id_last(iddesc idh);
    /* return the last id allocated from pool <idh>.
     * if none allocated, then return NILID.  */




typedef struct str32_ {
    u_char str[1+32+1];   /* str[0] is length, extra byte for NULL */
} str32;

typedef struct str160_ {
    u_char str[1+160+1];   /* str[0] is length, extra byte for NULL */
} str160;

extern boolean str160init(str160 *pstr, char *cstr);
extern boolean str32init(str32 *pstr, char *cstr);
extern boolean str32ninit(str32 *pstr, char *buf, int length);
extern char *str32ptr(str32 *pstr);
extern int str32len(str32 *pstr);
extern boolean str32cpy(str32 *dst, str32 *src);
extern boolean str32match(str32 *s1, str32 *s2);

#define STR32PTR(PP)    ((char *) &(PP)->str[1])
#define STR32LEN(PP)    ((int) ((PP)->str[0]))
#define EMPTYSTR32(PP)  (STR32LEN(PP) == 0)

/* true if a str32 is null terminated: */
#define ISCSTR(PP)  ((PP)->str[STR32LEN(PP)+1] == '\0')

#define STR32EQ(PP,QQ)  ((STR32LEN(PP)==STR32LEN(QQ)) &&\
    bcmp((byteptr)STR32PTR(PP),(byteptr)STR32PTR(QQ),STR32LEN(PP)) == 0)

/* copy pascal-style strings (without NULL): */
#define STR32CPY(PP,QQ) bcopy((byteptr)&((QQ)->str[0]),\
                (byteptr)&((PP)->str[0]),STR32LEN(QQ)+1)
            /* note: +1 for length byte */

/* copy pascal-style strings including NULL: */
#define CSTR32CPY(PP,QQ) bcopy((byteptr)&((QQ)->str[0]),\
                (byteptr)&((PP)->str[0]),STR32LEN(QQ)+2)
            /* note: +2 for length & null bytes */

/* general case of above macros: */
#define PSTRPTR(PP)    STR32PTR(PP)
#define PSTRLEN(PP)    STR32LEN(PP)
#define EMPTYPSTR(PP)  (STR32LEN(PP) == 0)
#define PSTREQ(PP,QQ)	STR32EQ(PP,QQ)
#define PSTRCPY(PP,QQ)	STR32CPY(PP,QQ)
#define CPSTRCPY(PP,QQ)	CSTR32CPY(PP,QQ)


/* exported types: */
typedef uchar *symbol_table;
typedef ushort symbol;

typedef enum {
    SY_OK,
    SY_NO_MEMORY
} sy_result;

#define NILSYM  ((symbol) NILVAL)

/* macros to operate on a symbol table local to a given module, e.g.:
 *  symbol_table    foo_symtab;
 *  #define MY_SYMTAB   foo_symtab
 */
#define SY_APPEND(STR)      sy_append(MY_SYMTAB, STR)
#define SY_APPEND32(PSTR)   sy_append32(MY_SYMTAB, PSTR)
#define SY_SYMBOL(STR)      sy_symbol(MY_SYMTAB, STR)
#define SY_SYMBOL32(PSTR)   sy_symbol32(MY_SYMTAB, PSTR)
#define SY_STRING(SYM)      sy_string(MY_SYMTAB, SYM)
#define SY_DELETE(SYM)      sy_delete(MY_SYMTAB, SYM)

extern symbol sy_append(symbol_table st, char *s);
extern symbol sy_append32(symbol_table st, str32 *pstr);
    /* store string <s> or <pstr> in symbol table.  ok if symbol has
     * already been entered.
     * Symbols are allocated in increasing order.
     */

extern symbol sy_symbol(symbol_table st, char *s);
extern symbol sy_symbol32(symbol_table st, str32 *pstr);
    /* look up symbol of string <s> or <pstr>.
     * return NILSYM if not entered.
     */

extern char *sy_string(symbol_table st, symbol sym);
    /* look up string of symbol <sym>.
     * return NIL(char) if not entered.
     */

extern void sy_delete(symbol_table st, symbol sym);
extern symbol_table sy_init(char *tablename, PFI canfunc, sy_result *err);
extern void sy_destroy(symbol_table st);
extern char *sy_resultStr(sy_result err);

typedef short database;

#define MAXKEYS 7

typedef struct {
    short   nkeys;
    short   keys[MAXKEYS];
} key;

/* macro to calculate user key size: */
#define UKSIZE(K) ((K)->nkeys * sizeof((K)->keys[0]) + sizeof((K)->nkeys))

typedef enum {
    DB_OK,
    DB_NAME_INUSE
} db_result;

/* macros for modules only using a single global database: */
#define DB_APPEND(KEY, VAL, VALSIZE)\
        db_append(MY_DATABASE, KEY, VAL, VALSIZE)
#define DB_REPLACE(KEY, VAL, VALSIZE)\
        db_replace(MY_DATABASE, KEY, VAL, VALSIZE)
#define DB_RETRIEVE(VAL,  KEY, VALSIZE)\
        db_retrieve(MY_DATABASE, VAL,  KEY, VALSIZE)
#define DB_DELETE(KEY)\
        db_delete(MY_DATABASE, KEY)

extern boolean db_append(database db, key *keyptr, byteptr valptr, int size);
extern boolean db_replace(database db, key *keyptr, byteptr valptr, int size);
extern boolean db_retrieve(database db, byteptr valptr, key *keyptr, int size);
    /* note - if <valptr> is NIL, don't copy ("exists" operation) */
extern boolean db_delete(database db, key *keyptr);

extern void db_mkkey(key *kptr, int nkeys, ...);
    /* form an n-ary key */
extern char *db_keyStr(key *keyptr);
    /* (debug) return a string representation of an n-ary key */

extern database db_init(char *dbname, db_result *err);
extern char *db_resultStr(db_result err);
void db_check(database db);

/* NOTES:
 *
 * in db_apppend(), db_replace(), & db_retrieve(), the arg order
 * is the same as strncpy(), (after database arg.)
 *
 * the key combination is up to the user to supply, and must be unique
 * amoung all stored records.
 *
 * the <size> arg in db_replace() & db_retrieve() is included as a
 * error check only.
 */

#endif
