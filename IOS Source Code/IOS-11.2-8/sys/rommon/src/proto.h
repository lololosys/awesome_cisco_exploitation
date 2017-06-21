/* $Id: proto.h,v 3.2.58.1 1996/03/21 23:27:48 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/proto.h,v $
 *------------------------------------------------------------------
 * proto.h
 *
 * May 1994, Rob Clevenger
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Miscellaneous Monitor prototypes.
 *------------------------------------------------------------------
 * $Log: proto.h,v $
 * Revision 3.2.58.1  1996/03/21  23:27:48  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/05  23:12:21  mbeesley
 * Branch: ELC_branch
 * Add complete support for discontigous NVRAM.
 *
 * Revision 3.2  1995/11/17  18:42:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/17  01:01:01  clev
 * CSCdi35421:  Add cache tests to ROM monitor core board diagnostics
 *
 * Revision 2.1  1995/06/07  22:38:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __PROTO_H__
#define __PROTO_H__

/*
** prototype file
*/

/* nvram.c */
extern void *nv_read_ptr(void *ptr);
extern short nv_read_short(short *ptr);
extern unsigned short nv_read_config(void);

/* berrtest.c */
extern int berrtest();
extern int chkberr(volatile unsigned long *address, int size, unsigned readonly);

/* builtin.c */
extern int echo(int argc,char *argv[]);
extern int unset(int argc, char *argv[]);
extern int setalias(int argc, char *argv[]);
extern int sleep_cmd(int argc, char *argv[]);
extern int cat(int argc, char *argv[]);
extern int directory(int argc, char *argv[]);
extern int reset(int argc, char *argv[]);

/* diagtools.c */
extern int pstat(unsigned long *location, int size, int eq, unsigned long mask, unsigned long cmpval, int msecs, unsigned long *retptr);

/* main.c */
extern int main();

/* main_net.c */
extern int _start (int code, unsigned char *arg);

/* memops.c */
extern int cmp_mem(int argc, char *argv[]);
extern int cmpbyte(unsigned char *addr0, unsigned char *addr1, int length);
extern int cmpword(unsigned short *addr0, unsigned short *addr1, int length);
extern int cmplword(unsigned *addr0, unsigned *addr1, int length);
extern int mov_mem(int argc, char *argv[]);
extern void movbyte(unsigned char *addr0, unsigned char *addr1, int length);
extern void movword(unsigned short *addr0, unsigned short *addr1, int length);
extern void movlword(unsigned *addr0, unsigned *addr1, int length);
extern int fil_mem(int argc, char *argv[]);
extern void filbyte(unsigned char *addr, int length, unsigned char val);
extern void filword(unsigned short *addr, int length, unsigned short val);
extern void fillword(unsigned *addr, unsigned length, unsigned val);
extern int dis_mem(int argc, char *argv[]);
extern void dismem(unsigned char *addr, int length, unsigned disaddr, int fldsize);
extern int alt_mem(int argc, char *argv[]);
extern int jump(int argc, char *argv[]);
extern int call(int argc, char *argv[]);
extern int memdebug(int argc, char *argv[]);
extern int memtest(int argc, char *argv[]);
extern int memloop(int argc, char *argv[]);
extern void addrtest(unsigned char *addr, int length);
extern int addrloop(int argc, char *argv[]);
extern int berrscan(int argc, char *argv[]);
extern int paritytest(int argc, char *argv[]);
extern void quitmsg();
extern unsigned int u_chksum(unsigned char *addr, int size);
extern int memory_checksum(int argc, char *argv[]);

/* tcalibrate.c */
extern long timer_calibrate (long t); 
extern int time_it (int cnt);
extern void wastetime (long n);
extern int tcal(int argc, char *argv[]);

/* library type routines (excluding the string routines and setjmp) */
extern atoh(char c);
extern int bcmp(unsigned char *bp1, unsigned char *bp2, int length);
extern int bcopy(unsigned char *source, unsigned char *destation, int bytes);
extern int getline(char *buffer, int bufsiz);
extern int getnum(char *cptr, int base, long *longret);
extern int getnnum(char *cptr, int base, long *longret, int maxchars);
extern int getopt(int argc, char *argv[], char *optstr);
extern int sprintf(char *destbuf, char *fmtptr, ...);
extern int printf(char *fmtptr, ...);
extern void puts(char *cptr);

/* second_cache.c for Sierra */
extern int level2_cache_detect(void);

/* cache_mips.c for Sierra */
extern int cache_test(void);

/* extern prototypes for assembly functions in start.S for Sierra */
extern void dcache_wback(unsigned int index);
extern void dload_tag(unsigned int index);
extern void hit_wb_inval(unsigned long vaddr);

/* cookie stuff */
extern int toss_cookie(int argc, char *argv[]);
#ifdef __COOKIE_H__
extern struct cookie *get_cookie(void);
extern void put_cookie(struct cookie *cptr);
#endif  /* __COOKIE_H__ */

#endif   /* __PROTO_H__ */
