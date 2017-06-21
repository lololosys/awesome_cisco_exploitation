/* $Id: comp_sys.c,v 3.9.12.10 1996/08/28 13:20:54 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/comp_sys.c,v $
 *------------------------------------------------------------------
 * comp_sys.c -- Main file for compression subsystem.
 *
 * November 1994, George Taylor
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: comp_sys.c,v $
 * Revision 3.9.12.10  1996/08/28  13:20:54  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.12.9  1996/08/07  18:28:48  turadek
 * CSCdi63479:  Turn off payload compression does not turn on fast
 * switching
 * Branch: California_branch
 * Allow fast-switching to be re-enabled when payload compression is
 * disabled.
 *
 * Revision 3.9.12.8  1996/07/23  13:31:01  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.9.12.7  1996/06/24  09:27:06  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.9.12.6  1996/05/13  14:47:54  fred
 * CSCdi56825:  RSP box crashes when configuring a virtual interface
 *         check for cbus/mci usage before invoking cbus_mci_txlimit().
 *         Also, change name of serial_cfg_modified registry to
 *         interface_cfg_modified
 * Branch: California_branch
 *
 * Revision 3.9.12.5  1996/05/11  06:10:48  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Attach lex_ncp subsystem to lex subsystem as lex requires PPP.
 * Add registry calls to eliminate PPP's dependancy on CCP/Compress.
 *
 * Revision 3.9.12.4  1996/05/01  14:33:25  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Make PPP NCPs into real subsystems.  Add them to all images where
 * PPP and the related network protocol are located.  NCPs are dependant
 * on PPP and the related network protocol subsystem but not vice versa.
 * Remove dependancies between PPP and the compression code.
 *
 * Revision 3.9.12.3  1996/04/29  09:45:20  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.9.12.2  1996/04/11  21:23:40  turadek
 * CSCdi51869:  Router hangs when hdlc stacker serial is dis/ena repeatedly
 * Branch: California_branch
 * Prevent decompression of zero-length packets.
 *
 * Revision 3.9.12.1  1996/03/18  22:47:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.4  1996/03/16  07:54:52  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.3  1996/03/13  02:11:36  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.2.2  1996/03/07  11:06:48  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/02/20  21:50:00  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/15  18:03:05  gtaylor
 * CSCdi45779:  Leased Line ISDN work.
 *
 * Revision 3.8  1996/02/08  03:08:02  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.7  1996/01/26  20:03:38  dino
 * CSCdi47621:  IP multicast fastswitching doesnt work on dialerless ISDN
 *              interface. Also, packets are not process level switched
 *              when PPP compression is enabled in incoming interface.
 *
 * Revision 3.6  1996/01/16  22:52:54  atsao
 * CSCdi46647:  Update N1 when MTU, X25 modulo or LAPB compression is
 * changed
 * Use lapb_update_n1 as a registry instead of a function call.
 *
 * Revision 3.5  1996/01/15  04:48:20  atsao
 * CSCdi46647:  Update N1 when MTU, X25 modulo or LAPB compression is
 * changed
 * Dynamically update N1 value whenever there is configuration change on
 * hardware MTU, LAPB/X25 modulo, or lapb compression.
 *
 * Revision 3.4  1995/12/13  18:51:59  turadek
 * CSCdi45308:  hdlc compression statistics not updated/displayed
 * make sure correct global variables are updated when processing
 * hdlc compressed frames.
 *
 * Revision 3.3  1995/11/21  20:44:02  turadek
 * CSCdi43841:  add frame relay payload compression feature
 * syncing code to 11.0
 *
 * Revision 3.2  1995/11/17  18:01:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/09/18  18:13:19  gtaylor
 * CSCdi29250:  PPP compression should support the existing compress
 * command. For ppp encapsulation compression now appears without the
 * ppp prefix. The command can still be entered with the prefix.
 *
 * Revision 2.6  1995/09/01  18:23:22  turadek
 * CSCdi32109:  Memory Corruption - running ip and decnet
 * fixes a potential buffer overrun problem that could corrupt memory
 *
 * Revision 2.5  1995/06/29  17:22:06  gtaylor
 * CSCdi35161:  PPP compression should support the show compression stats
 * command.
 * Support for PPP compression stats added. ISDN B channel compression
 * stats
 * display also included.
 *
 * Revision 2.4  1995/06/28  09:32:44  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/21  03:30:45  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.2  1995/06/20  20:38:00  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:17:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "subsys.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "logger.h"
#include "../x25/msg_comp.c"		/* Not a typo, see logger.h */
#include "../ui/debug.h"
#include "../parser/parser_defs_compress.h"
#include "../x25/comp_debug.h"
#include "../x25/lapb.h"
#include "stac_comp.h"
#include "comp_sys.h"
#include "isdn.h"
#include "../les/isdn_bri.h"
#include "ppp.h"
#include "ncp.h"
#include "ccp.h"
#include "ppp_registry.h"

/* forwards */
static boolean pbp_compress(paktype *, long dict_cntl);
static boolean pbp_comp_default(paktype *, long dict_cntl);
static paktype *pbp_uncompress(paktype *, long dict_cntl); 
static paktype *pbp_uncomp_default(paktype *, long dict_cntl); 

static void compress_initialize_rand(hwidbtype *);
static void compress_initialize_stac(hwidbtype *);
static void compress_initialize_default(hwidbtype *);
static void compress_init(subsystype *);
static void update_compress_stats(hwidbtype *);
static boolean init_stac_context (hwidbtype *);

/*
 * This file is currently the only resident of the sub_compress subsystem so
 * it's being given the subsystem header for compression for now.
 */
#define COMP_MAJVERSION 1
#define COMP_MINVERSION 0
#define COMP_EDITVERSION 1

SUBSYS_HEADER(compress, COMP_MAJVERSION, COMP_MINVERSION, COMP_EDITVERSION,
		      	compress_init, SUBSYS_CLASS_KERNEL,
			NULL,
			NULL);

ushort comp_stat_clks[COMP_STAT_PERIODS_MAX] = {1, 5, 10};


/*
 * compression initialization
 */

static void compress_init (subsystype *subsys)
{
    /* Set up compress flags */
    compress_debug_init();

    /* Register some functions call */
    reg_add_clear_counters(compress_clearcounters, "compress_clearcounters");

    /* Register some function calls for PPP modularity sake */
    reg_add_compress_clearcounters(compress_clearcounters,
				   "compress_clearcounters");
    reg_add_ppp_compression_config(ppp_compression_config,
				   "ppp_compression_config");

    /* Initialize compress parser support */
    compress_parser_init();

    /* Initialize compress parser support */
    reg_add_onemin(comp_timer, "compress_timer");

    /* init hdlc compression registries */
    reg_add_late_q_compress(COMP_STAC, pbp_compress, "pbp_compress");
    reg_add_late_q_uncompress(COMP_STAC, pbp_uncompress, "pbp_uncompress");
    reg_add_default_late_q_compress(pbp_comp_default, "pbp_comp_default");
    reg_add_default_late_q_uncompress(pbp_uncomp_default, "pbp_uncomp_default");

    /* the default lapb stuff (this could go into lapb.c, but doesn't hurt here... */
    reg_add_default_compress_reset(
	(service_compress_reset_type)compress_initialize_default, "default reset");
    reg_add_compress_reset(
	COMP_RAND, (service_compress_reset_type)compress_initialize_rand, "predictor reset");
    reg_add_compress_reset(
	COMP_STAC, (service_compress_reset_type)compress_initialize_stac, "stac reset");

    /* so vc-based schemes (like frame-relay) can init compression contexts */
    reg_add_init_stac_context(init_stac_context, "init_stac_context");
    reg_add_remove_comp_context(no_compression, "no_compression");
}


/*
 * rand compression routines used from all over
 */

uchar compress_rand (uchar *guess_table, ushort *hash_p, uchar *source, 
			ulong len, uchar *dest, ulong *dest_len)
{
uchar *flagdest, flags, *orgdest = dest;
uchar *maxdest = dest + len;
ushort hash = *hash_p;
uchar *guess_hash_p;
  uchar  bitmask;  

/***************************************************************
 *  The following original RAND code is present to document the
 *  macro'ized inline optimized code below.  Do not remove these
 *  comments.
 ***************************************************************
 *
 *   while (len > 0) {
 *      flagdest = dest++; flags = 0;
 *      for (bitmask=1, i=0; i < 8 && (len > 0); i++, bitmask <<= 1) {
 ***BEGIN INLINE_RAND_COMP()
 *	    if (guess_table[hash] == *source) {
 *              flags |= bitmask; 
 *          } else {
 *              guess_table[hash] = *source;
 *              *dest++ = *source;
 *          }
 *          RAND_HASH(*source++, hash); len--;
 *      }
 *      *flagdest = flags;
 ***END INLINE_RAND_COMP()
 *   }
 */

  while ( (len >= 32) && (dest + 36 <= maxdest) ) { /* '36' = 32 bytes + 4 flags */
    flagdest = dest++; flags = 0; 
    INLINE_RAND_COMP (0x1);
    INLINE_RAND_COMP (0x2);
    INLINE_RAND_COMP (0x4);
    INLINE_RAND_COMP (0x8);
    INLINE_RAND_COMP (0x10);
    INLINE_RAND_COMP (0x20);
    INLINE_RAND_COMP (0x40);
    INLINE_RAND_COMP (0x80);
    *flagdest = flags;
    flagdest = dest++; flags = 0; 
    INLINE_RAND_COMP (0x1);
    INLINE_RAND_COMP (0x2);
    INLINE_RAND_COMP (0x4);
    INLINE_RAND_COMP (0x8);
    INLINE_RAND_COMP (0x10);
    INLINE_RAND_COMP (0x20);
    INLINE_RAND_COMP (0x40);
    INLINE_RAND_COMP (0x80);
    *flagdest = flags;
    flagdest = dest++; flags = 0; 
    INLINE_RAND_COMP (0x1);
    INLINE_RAND_COMP (0x2);
    INLINE_RAND_COMP (0x4);
    INLINE_RAND_COMP (0x8);
    INLINE_RAND_COMP (0x10);
    INLINE_RAND_COMP (0x20);
    INLINE_RAND_COMP (0x40);
    INLINE_RAND_COMP (0x80);
    *flagdest = flags;
    flagdest = dest++; flags = 0; 
    INLINE_RAND_COMP (0x1);
    INLINE_RAND_COMP (0x2);
    INLINE_RAND_COMP (0x4);
    INLINE_RAND_COMP (0x8);
    INLINE_RAND_COMP (0x10);
    INLINE_RAND_COMP (0x20);
    INLINE_RAND_COMP (0x40);
    INLINE_RAND_COMP (0x80);
    *flagdest = flags;
    len -= 32;
  }
  while ( (len >= 16) && (dest + 18 <= maxdest) ) {
    flagdest = dest++; flags = 0; 
    INLINE_RAND_COMP (0x1);
    INLINE_RAND_COMP (0x2);
    INLINE_RAND_COMP (0x4);
    INLINE_RAND_COMP (0x8);
    INLINE_RAND_COMP (0x10);
    INLINE_RAND_COMP (0x20);
    INLINE_RAND_COMP (0x40);
    INLINE_RAND_COMP (0x80);
    *flagdest = flags;
    flagdest = dest++; flags = 0; 
    INLINE_RAND_COMP (0x1);
    INLINE_RAND_COMP (0x2);
    INLINE_RAND_COMP (0x4);
    INLINE_RAND_COMP (0x8);
    INLINE_RAND_COMP (0x10);
    INLINE_RAND_COMP (0x20);
    INLINE_RAND_COMP (0x40);
    INLINE_RAND_COMP (0x80);
    *flagdest = flags;
    len -= 16;
  }
  while ( (len >= 8) && (dest + 9 <= maxdest) ) {
    flagdest = dest++; flags = 0; 
    INLINE_RAND_COMP (0x1);
    INLINE_RAND_COMP (0x2);
    INLINE_RAND_COMP (0x4);
    INLINE_RAND_COMP (0x8);
    INLINE_RAND_COMP (0x10);
    INLINE_RAND_COMP (0x20);
    INLINE_RAND_COMP (0x40);
    INLINE_RAND_COMP (0x80);
    *flagdest = flags;
    len -= 8;
  }
  if  ( (len < 8) && (dest + len + 1 <= maxdest) ) {
    flagdest  = dest++;  flags = 0;  bitmask = 1;
    while ( len > 0 ) {
      INLINE_RAND_COMP (bitmask);
      bitmask =  bitmask<<1;
      len -= 1;
    }
    *flagdest = flags;
  }
  
  if ( len ) {
    while (len > 0) {
      guess_table[hash] = *source;
      RAND_HASH(*source++, hash);
      len--;
    }
    *hash_p = hash;
    return (COMP_ANTI_EXPANSION);
  }
  
  *hash_p = hash;
  *dest_len = dest - orgdest;
  return (COMP_NO_ANTI_EXPANSION);
}/* end compress_rand()*/

/* processes input frames to keep the compression dictionary in 
 * sync with the decompressor in the case where getbuffer() failed
 * in compress().
 */
void compress_rand_fixup (uchar *guess_table, ushort *hash_p, uchar *source,
                        ulong len)
{
ushort hash = *hash_p;

     while (len > 0) {
	guess_table[hash] = *source;
	RAND_HASH(*source++, hash);
	len--;
     }
     *hash_p = hash;
}/* end compress_rand_fixup() */

/* processes anti expansion frames to keep the decompression dictionary in 
 * sync with the compressor.
 */
void decompress_rand_fixup (uchar *guess_table, ushort *hash_p, uchar *source, 
			ulong len, uchar *dest)
{
ushort hash = *hash_p;

     while (len > 0) {
	guess_table[hash] = *source;
	*dest++ = *source;
	RAND_HASH(*source++, hash);
	len--;
     }
     *hash_p = hash;
}/* end decompress_rand_fixup()*/

void decompress_rand (uchar *guess_table, ushort *hash_p, uchar *source, 
			ulong len, uchar *dest, ulong *dest_len)
{
uchar flags, *orgdest = dest;
ushort hash = *hash_p;

/***************************************************************
 *  The following original RAND code is present to document the
 *  macro'ized inline optimized code below.  Do not remove these
 *  comments.
 ***************************************************************
 *
 *   while (len > 0) {
 *      flags = *source++;
 *      len--;
 *      for (i=0; i < 8 ; i++, flags >>= 1) {
 ***BEGIN INLINE_RAND_DECOMP_END()
 *          if (flags & 1) {
 *              *dest = guess_table[hash];
 *          } else {
 *              if (len <= 0)
 *		    break;
 *              guess_table[hash] = *source;
 *              *dest = *source++;
 *		len--;
 *          }
 *          RAND_HASH(*dest++, hash);
 ***END   INLINE_RAND_DECOMP_END()
 *      }
 *   }
 */
     while (len >= 36) { /* 4 flag 0 bytes could consume a len of 36 */
        flags = *source++;
	/*process 32 flag bits    */
        len -= 4; /* 4 flag bytes */
	INLINE_RAND_DECOMP (0x1)
	INLINE_RAND_DECOMP (0x2)
	INLINE_RAND_DECOMP (0x4)
	INLINE_RAND_DECOMP (0x8)
	INLINE_RAND_DECOMP (0x10)
	INLINE_RAND_DECOMP (0x20)
	INLINE_RAND_DECOMP (0x40)
	INLINE_RAND_DECOMP (0x80)
        flags = *source++;
	INLINE_RAND_DECOMP (0x1)
	INLINE_RAND_DECOMP (0x2)
	INLINE_RAND_DECOMP (0x4)
	INLINE_RAND_DECOMP (0x8)
	INLINE_RAND_DECOMP (0x10)
	INLINE_RAND_DECOMP (0x20)
	INLINE_RAND_DECOMP (0x40)
	INLINE_RAND_DECOMP (0x80)
        flags = *source++;
	INLINE_RAND_DECOMP (0x1)
	INLINE_RAND_DECOMP (0x2)
	INLINE_RAND_DECOMP (0x4)
	INLINE_RAND_DECOMP (0x8)
	INLINE_RAND_DECOMP (0x10)
	INLINE_RAND_DECOMP (0x20)
	INLINE_RAND_DECOMP (0x40)
	INLINE_RAND_DECOMP (0x80)
        flags = *source++;
	INLINE_RAND_DECOMP (0x1)
	INLINE_RAND_DECOMP (0x2)
	INLINE_RAND_DECOMP (0x4)
	INLINE_RAND_DECOMP (0x8)
	INLINE_RAND_DECOMP (0x10)
	INLINE_RAND_DECOMP (0x20)
	INLINE_RAND_DECOMP (0x40)
	INLINE_RAND_DECOMP (0x80)
     }
     while (len >= 18) { /*1st iteration range of len is 35 - 18
			  *2 flag 0 bytes could consume an len of 18
			  */
        flags = *source++;
	/*process 16 flag bits    */
        len -= 2; /* 2 flag bytes - 2nd iteration len could be 33 */
	INLINE_RAND_DECOMP (0x1)
	INLINE_RAND_DECOMP (0x2)
	INLINE_RAND_DECOMP (0x4)
	INLINE_RAND_DECOMP (0x8)
	INLINE_RAND_DECOMP (0x10)
	INLINE_RAND_DECOMP (0x20)
	INLINE_RAND_DECOMP (0x40)
	INLINE_RAND_DECOMP (0x80)
        flags = *source++;
	INLINE_RAND_DECOMP (0x1)
	INLINE_RAND_DECOMP (0x2)
	INLINE_RAND_DECOMP (0x4)
	INLINE_RAND_DECOMP (0x8)
	INLINE_RAND_DECOMP (0x10)
	INLINE_RAND_DECOMP (0x20)
	INLINE_RAND_DECOMP (0x40)
	INLINE_RAND_DECOMP (0x80)
     }
     while (len >= 9) { /*1st iteration range of len is 17 - 9
			 *a flag byte could consume an len of 9
			 */
        flags = *source++;
	/*process 8 flag bits */
        len--; /* a flag byte - 2nd iteration len could be 16 */ 
	INLINE_RAND_DECOMP (0x1)
	INLINE_RAND_DECOMP (0x2)
	INLINE_RAND_DECOMP (0x4)
	INLINE_RAND_DECOMP (0x8)
	INLINE_RAND_DECOMP (0x10)
	INLINE_RAND_DECOMP (0x20)
	INLINE_RAND_DECOMP (0x40)
	INLINE_RAND_DECOMP (0x80)
     }
     while (len > 0) {/*1st iteration range of len is 8 - 1
			 *2nd iteration len could be 7
			 */
	    flags = *source++;
	    /*process up to 8 flag bits */
	    len--; /* a flag byte*/
	    INLINE_RAND_DECOMP_END (0x1)
	    INLINE_RAND_DECOMP_END (0x2)
	    INLINE_RAND_DECOMP_END (0x4)
	    INLINE_RAND_DECOMP_END (0x8)
	    INLINE_RAND_DECOMP_END (0x10)
	    INLINE_RAND_DECOMP_END (0x20)
	    INLINE_RAND_DECOMP_END (0x40)
	    INLINE_RAND_DECOMP_END (0x80)
     }
     *hash_p = hash;
     *dest_len = dest - orgdest;
} /* end decompress_rand()*/


/*
 * pbp_compress
 * Compress the given packet starting at pak->info_start.  Copy the compressed
 * data back into the packet, and update the datagramsize.  Returns TRUE if
 * we compressed the packet, FALSE if expanded or other failures.
 * on entry, info_start points to the begining of the payload, and datagramsize
 * reflects the length of that payload.  compress it, and update datagramsize
 * to indicate length of new payload.  that is all.  return to your cubicle
 */

static boolean pbp_comp_default(paktype *pak, long dict_cntl)
{
    return FALSE;
}

static boolean pbp_compress(paktype *pak, long dict_cntl)
{
#define LZS_GOOD_COMP_STATUS  (LZS_SOURCE_FLUSH | LZS_FLUSHED)
  uchar *tmpsrc = pak->info_start;
  uchar *tmpdest;
  uchar *dest;
  long destcnt = SCRATCH_LEN-4;
  long sourcecnt, pDestCnt;
  hwidbtype *hwidb = pak->if_output->hwptr;
  ulong status;
  ulong sizeOut;
  
  sourcecnt =  pDestCnt = pak->datagramsize;
  if ((tmpsrc == NULL) || (sourcecnt <= 0))
    return FALSE;

  if ( !hwidb || (hwidb->comp_db->comp_configed != COMP_STAC) ) {
    buginf("\nCOMPRESS: bad hwidb or comp_db not set up! %x, %d", hwidb, hwidb->comp_db);
    return FALSE;
  }
  dest = tmpdest = hwidb->comp_db->pak_scratch;
  if (!dest)
    return FALSE;

  status = LZS_Compress(&tmpsrc, &tmpdest, &sourcecnt, &destcnt, 
			hwidb->comp_db->stac_comp_history,
			LZS_SOURCE_FLUSH | LZS_DEST_FLUSH |
			LZS_PERFORMANCE_MODE_1, 3);

  /* okay?  copy compressed payload back into pak and update size */
  sizeOut = SCRATCH_LEN - destcnt;
  if (((status & LZS_GOOD_COMP_STATUS) == LZS_GOOD_COMP_STATUS)
      && (sizeOut < pDestCnt)
      && (sizeOut <= hwidb->maxmtu)) {
    pak->datagramsize = sizeOut;
    bcopy(dest, pak->info_start, pak->datagramsize);
     
    hwidb->comp_db->compressed_sent += pak->datagramsize; /* update stats */
    hwidb->comp_db->comp_xmt_in_bytes += pDestCnt;
    return TRUE;
  } else {	/* something went bad.  reset for the next one */
    if (compress_debug)
      buginf("\nbad status from compress: %d", status);
    hwidb->comp_db->uncompressed_sent += pak->datagramsize;
    /* no need to reset histories here...this is pak-by-pak */
    return FALSE;
  }
#undef LZS_GOOD_COMP_STATUS  
}

/*
 * pbp_uncompress
 * Uncompress a packet that is self contained (no past history)
 * Returns a pak in case the implementation chooses to allocate 
 * a larger packet for the uncompressed packet.
 * It is assumed that the compressed data starts at network_start, and
 * that the caller will put whatever encapsulation header they need back
 * on the packet when it is returned.
 */
static paktype *pbp_uncomp_default(paktype *pak, long dict_cntl)
{
    return pak;
}

static paktype *pbp_uncompress(paktype *pak, long dict_cntl)
{
    paktype *newpak;
    uchar *tmpsrc;
    uchar *tmpdest;
    long sourcecnt;
    long destcnt;
    hwidbtype *hwidb;
    ulong status;

    hwidb = pak->if_input->hwptr;
    if ((hwidb->comp_db == NULL) || 
            (hwidb->comp_db->comp_configed == COMP_NOT_ENABLED)) {
        pak->flags |= PAK_ABORT;
	return(pak);
    }

    if (pak->datagramsize > hwidb->maxmtu) {
      if (compress_debug) {
        buginf("\nDECOMP pak is *way* too big!! hwidb: %x, pak: %x", hwidb, pak);
      }
      pak->flags |= PAK_ABORT;
      return(pak);
    }

    newpak = getbuffer(hwidb->maxmtu);
    /*
     * If we couldn't get a new packet to decompress into, then we have
     * to flush this packet, as a compressed packet that can't be
     * decompressed isn't worth much.
     */
    if (!newpak) {
        pak->flags |= PAK_ABORT;
	newpak = pak;
    } else {
        pak_copy(pak, newpak, 0);
	tmpsrc = pak->network_start;
	tmpdest = pak_center(newpak);
	newpak->datagramstart = tmpdest;
 	sourcecnt = pak->datagramsize - (pak->network_start - pak->datagramstart);
	if ( sourcecnt <= 0 ) {		/* for zero-length incoming paks */
	  pak->flags |= PAK_ABORT;
	  datagram_done(newpak);
	  return(pak);
	}

	destcnt = hwidb->maxmtu;

        status = LZS_Decompress(&tmpsrc, &tmpdest, &sourcecnt, &destcnt,
                       hwidb->comp_db->stac_decomp_history, 0);
	if ( !(status & LZS_END_MARKER) || !(status & LZS_SOURCE_EXHAUSTED) ) {
	  if ( compress_debug )
	    errmsg(&msgsym(SIZEBAD, COMP), hwidb->hw_namestring);
	  hwidb->comp_db->uncompressed_rcvd += newpak->datagramsize;
	  LZS_InitDecompressionHistory(hwidb->comp_db->stac_decomp_history);
	}
        newpak->datagramsize = hwidb->maxmtu - destcnt;
	/* update stats */
        hwidb->comp_db->compressed_rcvd += pak->datagramsize;
	hwidb->comp_db->comp_rcv_out_bytes += newpak->datagramsize;
	datagram_done(pak);
    }

    return(newpak);
}

/* 
 * create a valid stac context.  any previously-existing context is destoyed.
 * return F if something went bad, else return T
 */
boolean init_stac_context (hwidbtype *hwidb) {
  leveltype level;

  if (hwidb->comp_db) {
    no_compression(hwidb);
  }
  level = raise_interrupt_level(NETS_DISABLE);
  if ((hwidb->comp_db = malloc(sizeof(struct compress_db_))) == NULL){ 
    reset_interrupt_level(level);
    printf(nomemory);
    return FALSE;
  }
  hwidb->comp_db->comp_configed = COMP_NOT_ENABLED;
  hwidb->comp_db->stac_comp_history =
    malloc(LZS_SizeOfCompressionHistory() + LZS_DEST_MIN + 1);
  hwidb->comp_db->stac_decomp_history =
    malloc(LZS_SizeOfDecompressionHistory() + LZS_DEST_MIN + 1);
  hwidb->comp_db->pak_scratch = malloc(SCRATCH_LEN);	/* *** evil magic number *** */
  if 
    (!hwidb->comp_db->stac_comp_history || 
     !hwidb->comp_db->stac_decomp_history ||
     !hwidb->comp_db->pak_scratch) {
      printf(nomemory);
      no_compression(hwidb);
      reset_interrupt_level(level);
      return FALSE;
    }
  LZS_InitCompressionHistory(hwidb->comp_db->stac_comp_history);
  LZS_InitDecompressionHistory(hwidb->comp_db->stac_decomp_history);
  if (hwidb->enctype != ET_FRAME_RELAY) {
    reg_invoke_fast_setup(hwidb);
  }
  hwidb->comp_db->vc_contexts = 0;
  hwidb->comp_db->comp_configed = COMP_STAC; /*this last*/	
  
  compress_clearcounters(hwidb);
  hwidb->comp_db->comp_restarts = 0;
  hwidb->comp_db->comp_xmt_sync = 0;
  hwidb->comp_db->comp_rcv_sync = 0;
  hwidb->comp_db->comp_xmt_no_bufs = 0;
  hwidb->comp_db->comp_rcv_no_bufs = 0;
  reset_interrupt_level(level);
  return TRUE;
}
/*
 *
 * compression parser stuff
 *
 */
/*
 * compress_command
 * set or unset compression for a serial interface.
 */
void compress_command (parseinfo *csb)
{
  hwidbtype *hwidb = csb->interface->hwptr;
  leveltype level;
  
  if (csb->nvgen) {
    if (hwidb->comp_db == NULL || hwidb->comp_db->comp_configed == COMP_NOT_ENABLED)
      return;
    nv_write(TRUE, "compress ");
    switch (hwidb->comp_db->comp_configed) {
    case COMP_RAND:
      nv_add(TRUE, "predictor ");
      break;
    case COMP_STAC:
      nv_add(TRUE, "stac");
      break;
    default:
      errmsg(&msgsym(CONFIGBAD, COMP), hwidb->hw_namestring);
      break;
    }
  } else {
    if (!(hwidb->status & IDB_SERIAL)) {
      printf("\n%% Interface %s does not support compression.",
	     hwidb->hw_namestring);
      return;
    }
    if ((!is_lapb(hwidb)&&!is_hdlc(hwidb)&&!is_ppp(hwidb)) || is_x25(hwidb)) {
      printf("\n%% Encapsulation on interface %s does not support compression.",
	     hwidb->hw_namestring);
      return;
    }
    if (is_ppp(hwidb)){
      ppp_compression_config(hwidb, csb->sense, GETOBJ(int,1));
      return;
    }
    if (csb->sense) {
      switch (GETOBJ(int,1)) {

      case COMP_RAND:
	if (hwidb->comp_db) {
	  no_compression(hwidb);
	}
	level = raise_interrupt_level(NETS_DISABLE);
	if ((hwidb->comp_db = malloc(sizeof(struct compress_db_))) == NULL){ 
	  reset_interrupt_level(level);
	  printf(nomemory);
	  return;
	}
	hwidb->comp_db->comp_configed = COMP_NOT_ENABLED;
	reset_interrupt_level(level);
	hwidb->comp_db->comp_xmt_guess_table = NULL;
	hwidb->comp_db->comp_rcv_guess_table = NULL;
	
	hwidb->comp_db->comp_xmt_guess_table = 
	  malloc(GUESS_TABLE_SIZE);
	hwidb->comp_db->comp_rcv_guess_table = 
	  malloc(GUESS_TABLE_SIZE);
	if (hwidb->comp_db->comp_xmt_guess_table == NULL ||
	    hwidb->comp_db->comp_rcv_guess_table == NULL) {
	  printf(nomemory);
	  if (hwidb->comp_db->comp_xmt_guess_table)
	    free(hwidb->comp_db->comp_xmt_guess_table);
	  if (hwidb->comp_db->comp_rcv_guess_table)
	    free(hwidb->comp_db->comp_rcv_guess_table);
	  no_compression(hwidb);
	  return;
	}
	compress_initialize_rand(hwidb);
	reg_invoke_fast_setup(hwidb);
	hwidb->comp_db->comp_configed = COMP_RAND; /*this last*/	
	break;

      case COMP_STAC:
	init_stac_context(hwidb);
	break;

      default:
	errmsg(&msgsym(CONFIGBAD, COMP), hwidb->hw_namestring);
	break;
      }
      compress_clearcounters(hwidb);
      hwidb->comp_db->comp_restarts = 0;
      hwidb->comp_db->comp_xmt_sync = 0;
      hwidb->comp_db->comp_rcv_sync = 0;
      hwidb->comp_db->comp_xmt_no_bufs = 0;
      hwidb->comp_db->comp_rcv_no_bufs = 0;
    } else { /* no prefix */
      if (hwidb->comp_db) {
	no_compression(hwidb);
      }
    }
    if (is_lapb(hwidb)) {
	/*
	 * Update LAPB N1 value.
	 */
	reg_invoke_lapb_update_n1(hwidb);
    }	   
  }
} /* compress_command() */

void compress_clearcounters (hwidbtype *hwidb)
{
ushort i;

    if (hwidb->comp_db) {
      hwidb->comp_db->comp_xmt_in_bytes = 0;
      hwidb->comp_db->comp_rcv_out_bytes = 0;
      for (i=0; i < COMP_STAT_PERIODS_MAX; i++) {
	hwidb->comp_db->comp_next_clk[i] = comp_stat_clks[i];

	hwidb->comp_db->comp_rx_interv[i] = hwidb->counters.rx_cumbytes; 
	hwidb->comp_db->comp_rcv_out_interv[i] = 0;
	hwidb->comp_db->comp_tx_interv[i] = hwidb->counters.tx_cumbytes; 
	hwidb->comp_db->comp_xmt_in_interv[i] = 0;

	hwidb->comp_db->comp_rx_interv_l[i] = hwidb->counters.rx_cumbytes; 
	hwidb->comp_db->comp_rcv_out_interv_l[i] = 0;
	hwidb->comp_db->comp_tx_interv_l[i] = hwidb->counters.tx_cumbytes; 
	hwidb->comp_db->comp_xmt_in_interv_l[i] = 0;
      }
      hwidb->comp_db->uncompressed_sent = 0;
      hwidb->comp_db->compressed_sent = 0;
      hwidb->comp_db->compressed_rcvd = 0;
      hwidb->comp_db->uncompressed_rcvd = 0;
    }
}

boolean no_compression (hwidbtype *hwidb)
{
  leveltype level;
  
  if (hwidb->comp_db == NULL)
    return FALSE;
  level = raise_interrupt_level(NETS_DISABLE);
  switch (hwidb->comp_db->comp_configed) {
  case COMP_RAND:
    hwidb->comp_db->comp_configed = COMP_NOT_ENABLED;/*this 1st*/	
    if (hwidb->comp_db->comp_xmt_guess_table)
      free(hwidb->comp_db->comp_xmt_guess_table);
    if (hwidb->comp_db->comp_rcv_guess_table)
      free(hwidb->comp_db->comp_rcv_guess_table);
    break;
  case COMP_STAC:
    hwidb->comp_db->comp_configed = COMP_NOT_ENABLED;/*this 1st*/
    if (hwidb->comp_db->stac_comp_history)
      free(hwidb->comp_db->stac_comp_history);
    if (hwidb->comp_db->stac_decomp_history)
      free(hwidb->comp_db->stac_decomp_history);
    if (hwidb->comp_db->pak_scratch)
      free(hwidb->comp_db->pak_scratch);
    break;
  default:
    break;
  }
  free(hwidb->comp_db);
  hwidb->comp_db = NULL;
  reset_interrupt_level(level);
  reg_invoke_fast_setup(hwidb);		/* re-enable fast-switching */    
  return TRUE;
}

void compress_restart (hwidbtype *hwidb)
{
leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    if (hwidb->comp_db == NULL || hwidb->comp_db->comp_configed == 
      COMP_NOT_ENABLED) {
	reset_interrupt_level(level);
	return;
    }
    hwidb->comp_db->comp_restarts++;
    hwidb->comp_db->comp_xmt_sync = 0;
    hwidb->comp_db->comp_rcv_sync = 0;
    reg_invoke_compress_reset (hwidb->comp_db->comp_configed, hwidb);
    compress_clearcounters(hwidb);
    reset_interrupt_level(level);
    if (compress_debug) {
      buginf("\n COMP (CALLED compress_restart())");
    }

}/*end compress_restart()*/

/*
 * display_stac_stats
 * Display what little info we gather for Stac
 */
static void display_stac_stats (hwidbtype *hwidb)
{
    printf("\n       Additional Stacker Stats:");
    printf("\n       Transmit bytes:  Uncompressed = %-8lu Compressed = %lu",
            hwidb->comp_db->uncompressed_sent,
	    hwidb->comp_db->compressed_sent);
    printf("\n       Received bytes:  Compressed =   %-8lu Uncompressed = %lu\n",
            hwidb->comp_db->compressed_rcvd,
	    hwidb->comp_db->uncompressed_rcvd);
}

void comp_bf_to_a (ulong int_f, uchar *ratio_f)
{
short i;
    for (i=COMP_RATIO_F - 1; i >= 0 ;i--) {/*%d won't print leading 0's*/
	ratio_f[i] = '0' + (uchar) (int_f % 10);     
	int_f /= 10;
    }
    ratio_f[COMP_RATIO_F] = '\0';
}/* end comp_bf_to_a() */

static void display_compression(char *hw_namestring, compress_db *comp_db,
						ulong tx_cumbytes, ulong rx_cumbytes)
{
	ulong ratio_i_xmt, ratio_i_rcv, int_f;
	uchar ratio_f_xmt[COMP_RATIO_F + 1];
	uchar ratio_f_rcv[COMP_RATIO_F + 1];
	ulong numerator, divisor;
	ushort i;
  
    printf("\n %s", hw_namestring);
    printf("\n\t\t uncompressed bytes xmt/rcv %u/%u ",
			comp_db->comp_xmt_in_bytes,
			comp_db->comp_rcv_out_bytes);
    
    for (i=0; i < COMP_STAT_PERIODS_MAX; i++) {
		numerator = comp_db->comp_xmt_in_bytes - comp_db->comp_xmt_in_interv[i];
		divisor = tx_cumbytes - comp_db->comp_tx_interv[i];
		if ((long) divisor <= 0) {
			ratio_i_xmt = 0;
			int_f = 0;
		} else {
			ratio_i_xmt = numerator/divisor;
			int_f = ((numerator % divisor)*COMP_RATIO_M)/divisor;
		}
		comp_bf_to_a(int_f, ratio_f_xmt);
		numerator = comp_db->comp_rcv_out_bytes -
					comp_db->comp_rcv_out_interv[i];
		divisor = rx_cumbytes - comp_db->comp_rx_interv[i];
		if ((long) divisor <= 0) {
			ratio_i_rcv = 0;
			int_f = 0;
		} else {
			ratio_i_rcv = numerator/divisor;
			int_f = ((numerator % divisor)*COMP_RATIO_M)/divisor;
		}
		comp_bf_to_a(int_f, ratio_f_rcv);
      
		printf("\n\t\t %2d min avg ratio xmt/rcv %d.%s/%d.%s ", 
				comp_stat_clks[i], ratio_i_xmt, ratio_f_xmt, ratio_i_rcv,
				ratio_f_rcv);
	}
	printf("\n\t\t no bufs xmt %u no bufs rcv %u", comp_db->comp_xmt_no_bufs,
			comp_db->comp_rcv_no_bufs);
    printf("\n\t\t restarts %u", comp_db->comp_restarts);
}

void show_compress (void)
{
	hwidbtype *hwidb;
	hwidbtype *b_hwidb;
	int b;

	/* on malloc'ed structure x use:
	 *   ---  printf(gives up control on more & bad guy free's(x) & you crash ---
	 *          mem_lock(x); 
	 *   ---  printf(gives up control on more & bad guy can't free(x) ---
	 *          free(x); 
	 */
  
	automore_enable(NULL);
	FOR_ALL_HWIDBS(hwidb) {
		if (hwidb->comp_db == NULL ||
			hwidb->comp_db->comp_configed == COMP_NOT_ENABLED) {
			continue;
		}
		mem_lock(hwidb->comp_db);
		if((hwidb->isdn_info) && (!hwidb->isdn_info->leased_line)) {
			for(b = B1; b < ISDN_MAX_CHAN; b ++){
				b_hwidb = hwidb->isdn_info->b_chan_idb[b];
				if ((!b_hwidb) || b_hwidb->comp_db == NULL ||
					b_hwidb->comp_db->comp_configed == COMP_NOT_ENABLED)
					continue;
				mem_lock(b_hwidb->comp_db);
		   		display_compression(b_hwidb->hw_namestring,
						    b_hwidb->comp_db,
						    b_hwidb->counters.tx_cumbytes,
						    b_hwidb->counters.rx_cumbytes);
	    		if (b_hwidb->comp_db->comp_configed == COMP_STAC)
					display_stac_stats(b_hwidb);
			    free(b_hwidb->comp_db);
			}
		}else{
	   		display_compression(hwidb->hw_namestring,
					    hwidb->comp_db,
					    hwidb->counters.tx_cumbytes,
					    hwidb->counters.rx_cumbytes);
		    if (hwidb->comp_db->comp_configed == COMP_STAC)
				display_stac_stats(hwidb);
		}
	    free(hwidb->comp_db);
	}
	automore_disable();
} /* end show_compress() */

/*
 *
 * periodic routines used to support statistics and parser show-commands
 *
 */

/*
 *called every minute 
 */
void comp_timer (void)
{
	hwidbtype *hwidb;
	hwidbtype *b_hwidb;
	int b;

	FOR_ALL_HWIDBS(hwidb) {
	if (hwidb->comp_db != NULL &&
		hwidb->comp_db->comp_configed != COMP_NOT_ENABLED) {
		if(hwidb->isdn_info){
			for(b = B1; b < ISDN_MAX_CHAN; b ++){
				b_hwidb = hwidb->isdn_info->b_chan_idb[b];
				if ((!b_hwidb) || b_hwidb->comp_db == NULL ||
					b_hwidb->comp_db->comp_configed == COMP_NOT_ENABLED)
					continue;
				update_compress_stats(b_hwidb);
			}
		}
		update_compress_stats(hwidb);
		}  /* if (hwidb */
	} /*  FOR_ALL */
}  /* end comp_timer() */

static void update_compress_stats (hwidbtype *hwidb)
{
	ushort i;

	for (i=0; i < COMP_STAT_PERIODS_MAX; i++) {
		if (--hwidb->comp_db->comp_next_clk[i] <= 0) {
			hwidb->comp_db->comp_next_clk[i] = comp_stat_clks[i];
			hwidb->comp_db->comp_xmt_in_interv[i] = 
				hwidb->comp_db->comp_xmt_in_interv_l[i];
			hwidb->comp_db->comp_tx_interv[i] = 
				hwidb->comp_db->comp_tx_interv_l[i];
			hwidb->comp_db->comp_rcv_out_interv[i] = 
				hwidb->comp_db->comp_rcv_out_interv_l[i];
			hwidb->comp_db->comp_rx_interv[i] = 
				hwidb->comp_db->comp_rx_interv_l[i];
			hwidb->comp_db->comp_xmt_in_interv_l[i] = 
				hwidb->comp_db->comp_xmt_in_bytes;
			hwidb->comp_db->comp_tx_interv_l[i]
			    = hwidb->counters.tx_cumbytes;
			hwidb->comp_db->comp_rcv_out_interv_l[i] = 
				hwidb->comp_db->comp_rcv_out_bytes;
			hwidb->comp_db->comp_rx_interv_l[i]
			    = hwidb->counters.rx_cumbytes;
		}  /* if */
	}  /* for */
}

static void compress_initialize_default (hwidbtype *hwidb)
{
  errmsg(&msgsym(CONFIGBAD, COMP), hwidb->hw_namestring);
  return;
}

static void compress_initialize_stac (hwidbtype *hwidb)
{
  if (hwidb->comp_db == NULL)
    return;

  LZS_InitCompressionHistory(hwidb->comp_db->stac_comp_history);
  LZS_InitDecompressionHistory(hwidb->comp_db->stac_decomp_history);
}

static void compress_initialize_rand (hwidbtype *hwidb)
{
    if (hwidb->comp_db == NULL)
	return;
    memset(hwidb->comp_db->comp_xmt_guess_table, 0, GUESS_TABLE_SIZE);
    memset(hwidb->comp_db->comp_rcv_guess_table, 0, GUESS_TABLE_SIZE);
    hwidb->comp_db->comp_xmt_hash = HASH_MASK;
    hwidb->comp_db->comp_rcv_hash = HASH_MASK;
}

/*
 * Handle configuration of compression for PPP.
 */
void ppp_compression_config(hwidbtype *hwidb, boolean sense, int type)
{
    fsm *f;
    ccp_options *wo;
    leveltype level;

    if (!hwidb) {
	printf("\n%% Bad parameters.  Compression not enabled.");
        return;
    }

    f = reg_invoke_ppp_link2fsm(LINK_PPP_COMPRESSION, hwidb);

    if (!f) {
	printf("\n%% CCP fsm not found.  Compression not enabled.");
	return;
    }

    wo = f->wantoptions;

    /*
     * IP multicast fastswitching flag always has oppose sense of compression.
     */
    hwidb->firstsw->ip_multicast_fs = !sense;

    if (sense){
	    level = raise_interrupt_level(NETS_DISABLE);
		if (!hwidb->comp_db)
			hwidb->comp_db = malloc(sizeof(struct compress_db_));
		else
			memset(hwidb->comp_db, 0, sizeof(struct compress_db_));
		if (!hwidb->comp_db){
			reset_interrupt_level(level);
			printf("\n%% malloc() failed.  compression not enabled.");
			return;
		}
	    hwidb->comp_db->comp_configed = COMP_NOT_ENABLED;
	    reset_interrupt_level(level);
		switch (type){
			case COMP_RAND:
				wo->user_preference = CI_PREDICTOR1;
				wo->negotiate_compression = TRUE;
				hwidb->comp_db->comp_configed = COMP_RAND;
            break;

            case COMP_STAC:
                wo->user_preference = CI_LZSDCP;
                wo->negotiate_compression = TRUE;
			    hwidb->comp_db->comp_configed = COMP_STAC;
            break;

            default:
                printf("\n%%Unsupported compression type");
            break;
        }
	    compress_clearcounters(hwidb);
	    hwidb->comp_db->comp_restarts = 0;
	    hwidb->comp_db->comp_xmt_sync = 0;
	    hwidb->comp_db->comp_rcv_sync = 0;
	    hwidb->comp_db->comp_xmt_no_bufs = 0;
	    hwidb->comp_db->comp_rcv_no_bufs = 0;
    }else{
        wo->user_preference = CI_COMPRESSION_NONE;
        wo->negotiate_compression = FALSE;
		if (hwidb->comp_db) {
		    free(hwidb->comp_db);
		    hwidb->comp_db = NULL;
        }
    }
    reg_invoke_interface_cfg_modified(hwidb);
}

