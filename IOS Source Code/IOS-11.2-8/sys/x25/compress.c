/* $Id: compress.c,v 3.3.54.4 1996/08/28 13:23:03 thille Exp $
 * $Source: /release/112/cvs/Xsys/x25/compress.c,v $
 *------------------------------------------------------------------
 * COMPRESS.C
 *
 * July 29, 1993, Bill Thomas
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: compress.c,v $
 * Revision 3.3.54.4  1996/08/28  13:23:03  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.54.3  1996/07/23  13:32:25  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.3.54.2  1996/04/11  21:23:47  turadek
 * CSCdi51869:  Router hangs when hdlc stacker serial is dis/ena repeatedly
 * Branch: California_branch
 * Prevent decompression of zero-length packets.
 *
 * Revision 3.3.54.1  1996/03/18  22:49:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.2  1996/03/07  11:19:01  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.20.1  1996/02/20  21:54:36  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/21  20:44:34  turadek
 * CSCdi43841:  add frame relay payload compression feature
 * syncing code to 11.0
 *
 * Revision 3.2  1995/11/17  18:05:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/01  06:23:46  turadek
 * CSCdi39487:  stac compression broken for all encaps
 * fixes two stac library bugs (one data-dependent, one for anti-
 * compression feature.)  also adds enhanced x25 compression debug support.
 *
 * Revision 2.1  1995/06/07  23:20:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "lapb.h"
#include "logger.h"
#include "../x25/msg_comp.c"		/* Not a typo, see logger.h */
#include "../ui/debug.h"
#include "../parser/parser_defs_compress.h"
#include "compress.h"
#include "comp_debug.h"
#include "x25.h"
#include "../wan/stac_comp.h"


paktype *default_compress (hwidbtype *hwidb, paktype *pak)
{
    errmsg(&msgsym(CONFIGBAD, COMP), hwidb->hw_namestring);
    datagram_done(pak);
    return(NULL);
}


typedef struct {
  hwidbtype *hwidb;
  paktype *newpak, *oldpak;
  comp_hdr_t *header_p;
  uchar *src_p, *dest_p;
  ulong src_len; 
  ulong dest_len, destMax;
  ulong rval;
  uchar anti_expansion_bit;
  int lapb_overhead;		/* no need to ref the macro a zillion times... */
} comp_local_vars;


/* used for debugging only */
static long checksum_history(void *history, boolean compHistory)
{
 long len;
 long csum = 0;
 unsigned  char *h = history;

 if ( compHistory )
   len = LZS_SizeOfCompressionHistory();
 else
   len = LZS_SizeOfDecompressionHistory();
 while ( len-- )
   csum = csum ^ *h++;
 return csum;
}

static boolean comp_local_setup (comp_local_vars *clv, hwidbtype *hwidb, paktype *pak)
/* set up the local variables.  do packet sanity checks. return t if okay */
{
  clv->hwidb = hwidb;  clv->oldpak = pak;
  clv->newpak = NULL;
  clv->anti_expansion_bit = COMP_NO_ANTI_EXPANSION;
  clv->lapb_overhead = LAPB_OVERHEAD(clv->hwidb);
  if ((clv->src_len = clv->oldpak->datagramsize - clv->lapb_overhead) <= 0) {
    datagram_done(clv->oldpak);
    return FALSE;
  }
  return TRUE;
}


static boolean get_comp_buffer (comp_local_vars *clv, ulong compMethodOverhead)
/* get a buffer, and return true.  if no buffer, return false */
{
  clv->src_p = clv->oldpak->datagramstart + clv->lapb_overhead;

  clv->destMax = clv->dest_len = clv->src_len + compMethodOverhead;
  if ((clv->newpak = getbuffer(clv->dest_len)) == NULL) {
    clv->hwidb->comp_db->comp_xmt_no_bufs++;
    return FALSE;
  }
  return TRUE;
}


static void comp_copy_pak (comp_local_vars *clv, ulong compMethodOverhead)
{
  pak_copy(clv->oldpak, clv->newpak, 0);
  clv->newpak->datagramstart = (uchar *) ( (ulong)(clv->newpak->datagramstart -
		 (compMethodOverhead + COMPHEADERBYTES)) & BUFFER_WORD_ALIGN );
  clv->dest_p = clv->newpak->datagramstart + COMPHEADERBYTES + clv->lapb_overhead;
}


static void comp_set_comp_header (comp_local_vars *clv)
{
  if (clv->oldpak != clv->newpak) {
    datagram_done(clv->oldpak);			/* we're sending a copy...done with orig. */
    clv->dest_len = clv->destMax - clv->dest_len; /* fix up the length in the copy */
    clv->newpak->datagramsize = (ushort)clv->dest_len + COMPHEADERBYTES + clv->lapb_overhead;
  } else {
    clv->src_len += clv->lapb_overhead;		/* sending org. pak...unhack the length */
    clv->newpak->datagramstart -= COMPHEADERBYTES; /* add compression header to pak */
    clv->newpak->datagramsize += COMPHEADERBYTES;
  }

  clv->hwidb->comp_db->compressed_sent += clv->newpak->datagramsize;
  clv->hwidb->comp_db->uncompressed_sent += clv->src_len;
  clv->header_p = (comp_hdr_t *) (clv->newpak->datagramstart + clv->lapb_overhead);
  clv->header_p->org_size = (ushort) clv->src_len;
  clv->hwidb->comp_db->comp_xmt_in_bytes +=  clv->src_len + clv->lapb_overhead;
  clv->header_p->version = clv->hwidb->comp_db->comp_configed | clv->anti_expansion_bit; 
  clv->header_p->xmt_paks = (uchar) (clv->hwidb->comp_db->comp_xmt_sync & COMP_SYNC_MASK);
  if ((++clv->hwidb->comp_db->comp_xmt_sync & COMP_SYNC_MASK) == 0)
    clv->hwidb->comp_db->comp_xmt_sync = 1;
  if (compress_debug) {
    if (clv->anti_expansion_bit == COMP_UNCOMPRESSED)
      buginf("\n \t COMP ***NAKED*** xmt_paks %d", clv->header_p->xmt_paks);
    else if (clv->anti_expansion_bit == COMP_ANTI_EXPANSION)
      buginf("\n \t COMP ***ANTI*** xmt_paks %d", clv->header_p->xmt_paks);
    else
      buginf("\n \t COMP norm xmt_paks %d c-stat %x", clv->header_p->xmt_paks, clv->rval);
  }
}


paktype *stac_compress (hwidbtype *hwidb, paktype *pak)
{
  /* NOTE:-stacBeta has a 'backout' function on the receive-side.
     if a packet compresses larger than original, we'll send it raw
     and run it through the decompressor on the other end.
     */
  comp_local_vars clv;
  ulong sLen;
  long junk;			/* following are debug only */
  long junk2 = 0;
  long junk3 = 0;
  long jSlen = 0;
  uchar *j2, *jS = 0;
  uchar *jD = 0;

  
  if (!comp_local_setup(&clv, hwidb, pak))
    return NULL;				/* incoming pak too weird to use...bail */
  if (!get_comp_buffer(&clv, COMP_STAC_ENCAP_OFFSET)) {
    clv.anti_expansion_bit = COMP_UNCOMPRESSED; /* send orig. pak */
    clv.newpak = pak;
  } else {					/* compress it */
    comp_copy_pak(&clv, COMP_STAC_ENCAP_OFFSET);
    sLen = clv.src_len;

    if (compress_debug) {
      junk3 = clv.dest_len;
      buginf("\n  comp! src_len= %X, dest_len=%X history @ %X", 
	     sLen, junk3, hwidb->comp_db->stac_comp_history);
      junk2 = checksum_history(hwidb->comp_db->stac_comp_history, TRUE);
      jS = clv.src_p;  jD = clv.dest_p;
      jSlen = sLen;
    }

    clv.rval = LZS_Compress(&clv.src_p, &clv.dest_p, &sLen, &clv.dest_len,
			hwidb->comp_db->stac_comp_history,
			LZS_SAVE_HISTORY | LZS_SOURCE_FLUSH | LZS_DEST_FLUSH | LZS_PERFORMANCE_MODE_1,
			3);

    if (compress_debug) {
      junk = checksum_history(hwidb->comp_db->stac_comp_history, TRUE);
      buginf("\n  comp! stat=%4x len=%4x history(pre)=%X (post)=%X", 
	     clv.rval, junk3-clv.dest_len, junk2, junk);
      j2   = jS;
      while ( jSlen > 0 ) {
	buginf("\n  comp!    %2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x",
	       *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++,
	       *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++,
	       *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++,
	       *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++);
	jSlen = jSlen - 32;
      }
      junk = junk3 - clv.dest_len;
      j2 = jD;
      while ( junk > 0 ) {
	buginf("\n  comp(RAW)%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x",
	       *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++,
	       *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++,
	       *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++,
	       *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++);
	junk = junk - 32;
      }
      buginf("\n");
    }

    if (clv.rval & LZS_DEST_EXHAUSTED )  {	/* compressed larger.. */
      clv.anti_expansion_bit = COMP_ANTI_EXPANSION; /* send orig. pak w/ comp-header */
      datagram_done(clv.newpak);
      clv.newpak = pak;
    }
    if ( clv.rval == LZS_INVALID )
      errmsg(&msgsym(NOBUF, COMP), clv.hwidb->hw_namestring, "stac compression failed");
  }
  comp_set_comp_header(&clv);
  return clv.newpak;
}

paktype *rand_compress (hwidbtype *hwidb, paktype *pak)
{
paktype *newpak = NULL; /*keep compiler happy*/
comp_hdr_t *header_p;
uchar *src_p, *dest_p;
ulong src_len; 
ulong dest_len;
uchar anti_expansion_bit = COMP_NO_ANTI_EXPANSION;

    if (hwidb->comp_db == NULL || hwidb->comp_db->comp_configed == 
      COMP_NOT_ENABLED) {
	return(NULL);
    }

    if ((src_len = pak->datagramsize - LAPB_OVERHEAD(hwidb)) <= 0) {
	datagram_done(pak);
	return(NULL);
    }
    src_p = pak->datagramstart + LAPB_OVERHEAD(hwidb);

    switch (hwidb->comp_db->comp_configed) {
	case COMP_RAND:
/********************************************************************
 * CHANGE REQUIRED by CODE REVIEW of 11/24/93:
 *    ACTUAL: newpak size is: "pak->datagramsize + 9 + sizeof(comp_hdr_t)"
 *
 *    ADJUST: newpak->datagramstart up into "encapc" area.
 *	      but on an even word boundary per CSCdi17129 - so
 *            use 10 instead of 9.
 ********************************************************************
 */
            if ((newpak = getbuffer(pak->datagramsize)) == NULL) {
                hwidb->comp_db->comp_xmt_no_bufs++;
/********************************************************************
 * CHANGE REQUIRED by CODE REVIEW of 11/24/93:
 * use ANTI EXPANSION to send original frame anyway,
 * eliminating upper layer re-sends in the case of this
 * getbuffer() failure.  Still count the failure so as to
 * get the user to config more buffers.
 ********************************************************************
 */
		compress_rand_fixup(hwidb->comp_db->comp_xmt_guess_table, 
		    &hwidb->comp_db->comp_xmt_hash, src_p, src_len);
		anti_expansion_bit = COMP_ANTI_EXPANSION;
		newpak = pak;
		pak = NULL;
		newpak->datagramstart -= COMPHEADERBYTES;
		newpak->datagramsize += COMPHEADERBYTES;
		goto SEND_ANYWAY;
            }
	    pak_copy(pak, newpak, 0);
	    newpak->datagramstart -= (COMP_PRED_ENCAP_OFFSET + COMPHEADERBYTES);
	    /*safeguard see CSCdi17129*/
	    newpak->datagramstart = (uchar *) (((ulong)newpak->datagramstart) & 
					BUFFER_WORD_ALIGN);
	    break;
	default:
	    errmsg(&msgsym(CONFIGBAD, COMP), hwidb->hw_namestring);
	    datagram_done(pak);
	    return(NULL);
	    /*break;*/
    }

/* WARNING:
 * lapb encaps is already in datagramstart & datagramsize. 
 * in addition, this routine can not be interrupted (in compress_rand())
 * & packets processed here must be sent in order & processed in
 * received order by the other router else the compression/decompression
 * will not work.  interrupts are dis
 abled.  stack trace is:
 *  (A)	lapb_output(), lapb_dooutput(), compress().
 *  (B) lapb_input(), lapb_start(), lapb_dooutput().
 * case (A), the call to lapb_dooutput() is protected by 
 *   raise_interrupt_level()/reset_interrupt_level().
 * case (B), lapb_input() is on the serial receive interrupt thread.
 */

    dest_p = newpak->datagramstart + COMPHEADERBYTES + LAPB_OVERHEAD(hwidb);

    switch (hwidb->comp_db->comp_configed) {
	case COMP_RAND:
	    anti_expansion_bit = compress_rand(hwidb->comp_db->comp_xmt_guess_table, 
		&hwidb->comp_db->comp_xmt_hash, src_p, src_len, dest_p, &dest_len);
	    break;
	default:
	    break;	
    }
    if (anti_expansion_bit == COMP_ANTI_EXPANSION) {
	datagram_done(newpak);
	newpak = pak;
	pak = NULL;
	newpak->datagramstart -= COMPHEADERBYTES;
	newpak->datagramsize += COMPHEADERBYTES;
    } else {
	datagram_done(pak);
	newpak->datagramsize = (ushort) dest_len + COMPHEADERBYTES + 
					LAPB_OVERHEAD(hwidb);
    }

SEND_ANYWAY:

    header_p = (comp_hdr_t *) (newpak->datagramstart + LAPB_OVERHEAD(hwidb));
    PUTSHORT(&header_p->org_size, (ushort) src_len);
    hwidb->comp_db->comp_xmt_in_bytes +=  src_len + LAPB_OVERHEAD(hwidb);
    header_p->version = hwidb->comp_db->comp_configed | anti_expansion_bit; 

    header_p->xmt_paks =  (uchar) (hwidb->comp_db->comp_xmt_sync & COMP_SYNC_MASK);
    if (compress_debug) {
	buginf("\n \t COMP xmt_paks %d version %x", 
		header_p->xmt_paks, header_p->version);
    }
    if ((++hwidb->comp_db->comp_xmt_sync & COMP_SYNC_MASK) == 0)
	hwidb->comp_db->comp_xmt_sync = 1;

    return(newpak);
}/*end compress()*/

paktype *default_decompress (hwidbtype *hwidb, paktype *pak)
{
  errmsg(&msgsym(CONFIGBAD, COMP), hwidb->hw_namestring);
  lapb_reset(hwidb, "COMP config error");
  NEW_MIB_STATE(hwidb, LAPB_MIBMODE_OTHER, LAPB_MIBREASON_OTHER)
  datagram_done(pak);
  return NULL;
}


typedef struct {
  hwidbtype *hwidb;
  paktype *newpak, *oldpak;
  comp_hdr_t *header_p;
  uchar *src_p, *dest_p;
  ulong src_len;
  ulong dest_len;
  boolean anti_expansion;
  boolean uncompressed;
  int lapb_overhead;
} decomp_local_vars;


static boolean decomp_local_setup (decomp_local_vars *dlv, hwidbtype *hwidb, paktype *pak)
{
  dlv->hwidb = hwidb;  dlv->oldpak = pak;
  dlv->lapb_overhead = LAPB_OVERHEAD(dlv->hwidb);
  dlv->header_p = (comp_hdr_t *)dlv->oldpak->datagramstart;
  dlv->anti_expansion = dlv->header_p->version & COMP_ANTI_EXPANSION ? TRUE : FALSE;
  dlv->uncompressed   = dlv->header_p->version & COMP_UNCOMPRESSED   ? TRUE : FALSE;
  if (dlv->hwidb->comp_db == NULL || dlv->hwidb->comp_db->comp_configed == COMP_NOT_ENABLED) {
    if (compress_debug) {
      buginf("\n DECOMP local setup fails (COMP DISBLD) xmt_paks %d rcv_sync %d",
	   dlv->header_p->xmt_paks, dlv->hwidb->comp_db->comp_rcv_sync);
    }
    datagram_done(dlv->oldpak);
    return FALSE;
  }

  /*->comp_rcv_sync & ->comp_xmt_sync are zero initially but never again */
  /* unless config t; no comp; comp; transition occurs                   */ 
  if (dlv->header_p->xmt_paks != (uchar) (dlv->hwidb->comp_db->comp_rcv_sync & COMP_SYNC_MASK)) {
    errmsg(&msgsym(SYNCBAD, COMP), dlv->hwidb->hw_namestring);
    lapb_reset(dlv->hwidb, "COMP sync error");
    NEW_MIB_STATE(dlv->hwidb, LAPB_MIBMODE_OTHER, LAPB_MIBREASON_OTHER)
    if (compress_debug) {
      buginf("\n DECOMP local setup fails (BAD SYNC) xmt_paks %d rcv_sync %d",
	   dlv->header_p->xmt_paks, dlv->hwidb->comp_db->comp_rcv_sync);
    }
    datagram_done(dlv->oldpak);
    return FALSE;
  }
  if ((dlv->header_p->version & COMP_VERSION_MASK) != dlv->hwidb->comp_db->comp_configed) {
    errmsg(&msgsym(VERSIONBAD, COMP), dlv->hwidb->hw_namestring,
		dlv->hwidb->comp_db->comp_configed, dlv->header_p->version);
    lapb_reset(dlv->hwidb, "COMP version error");
    NEW_MIB_STATE(dlv->hwidb, LAPB_MIBMODE_OTHER, LAPB_MIBREASON_OTHER)
    if (compress_debug) {
      buginf("\n DECOMP local setup fails (BAD VERSION) xmt_paks %d rcv_sync %d",
	   dlv->header_p->xmt_paks, dlv->hwidb->comp_db->comp_rcv_sync);
    }
    datagram_done(dlv->oldpak);
    return FALSE;
  }
  return TRUE;
}


static void update_rcv_sync (decomp_local_vars *dlv)
{
  if ((++dlv->hwidb->comp_db->comp_rcv_sync & COMP_SYNC_MASK) == 0) 
    dlv->hwidb->comp_db->comp_rcv_sync = 1;
}


/* need to remove compression header from packet.  can't just wack datagramstart
   as that would leave this little hole between lapb header and datagram
   this is essentially decompress_rand_fixup() without the fixup... */
static void decomp_wack_comp_hdr(decomp_local_vars *dlv)
{
  bcopy(dlv->oldpak->datagramstart - dlv->lapb_overhead, 
    dlv->oldpak->datagramstart + COMPHEADERBYTES - dlv->lapb_overhead, dlv->lapb_overhead);
  dlv->oldpak->datagramstart += COMPHEADERBYTES;
}



static boolean get_decomp_buffer (decomp_local_vars *dlv)
{
  if ( ( dlv->newpak = getbuffer(dlv->header_p->org_size + 1) ) == NULL ) {
    dlv->hwidb->comp_db->comp_rcv_no_bufs++;
    datagram_done(dlv->oldpak);
    return FALSE;
  }
  return TRUE;
}


static void decomp_copy_pak (decomp_local_vars *dlv)
{
  pak_copy(dlv->oldpak, dlv->newpak, 0);
  dlv->src_p = dlv->oldpak->datagramstart + COMPHEADERBYTES;
  dlv->src_len = dlv->oldpak->datagramsize - COMPHEADERBYTES;
  dlv->dest_p = dlv->newpak->datagramstart;
  dlv->dest_len = dlv->header_p->org_size + 1;
}


static boolean sane_after_decomp (decomp_local_vars *dlv, ulong rval)
{
  boolean retVal = TRUE;
  
  dlv->hwidb->comp_db->compressed_rcvd += dlv->oldpak->datagramsize;
  dlv->hwidb->comp_db->uncompressed_rcvd += dlv->header_p->org_size;
  
  /*  if ( ( (rval & LZS_END_MARKER) != LZS_END_MARKER ) || dlv->dest_len != 1 ) { */
  if ( !dlv->anti_expansion 
      && ( ( (rval & LZS_END_MARKER) != LZS_END_MARKER )
          || (rval & LZS_DEST_EXHAUSTED) ) ) {
    errmsg(&msgsym(SIZEBAD, COMP), dlv->hwidb->hw_namestring);
    lapb_reset(dlv->hwidb, "COMP size error");
    NEW_MIB_STATE(dlv->hwidb, LAPB_MIBMODE_OTHER, LAPB_MIBREASON_OTHER)
    datagram_done(dlv->newpak);
    retVal = FALSE;
  } else {
    dlv->newpak->datagramsize = dlv->header_p->org_size;
    dlv->hwidb->comp_db->comp_rcv_out_bytes +=
      (ulong) dlv->newpak->datagramsize + dlv->lapb_overhead;
  }
  datagram_done(dlv->oldpak);
  return retVal;
}

paktype *stac_decompress (hwidbtype *hwidb, paktype *pak)
{
  decomp_local_vars dlv;
  ulong rval;
  ushort flags = LZS_SAVE_HISTORY;
  long junk, junk2=0;			/* following used only for compress debugging */
  long junk3=0;
  uchar *j2=NULL;		
  
  if (!decomp_local_setup(&dlv, hwidb, pak))
    return NULL;			/* packet fails sanity ... bail */
  
  if (dlv.uncompressed) {
    update_rcv_sync(&dlv);
    decomp_wack_comp_hdr(&dlv);
    return dlv.oldpak;			/* pak is naked...no need to process */
  }
  
  if (!get_decomp_buffer(&dlv))
    return NULL;			/* horrifying...can't decompress */
  
  update_rcv_sync(&dlv);		/* set up to decompress... */
  decomp_copy_pak(&dlv);

  if ( (long)dlv.src_len <= 0 )		/* incoming zero-length pak? */
    return NULL;

  if (dlv.anti_expansion) {		/* for stac-beta, *all* packets go through dictionary */
    flags |= LZS_UNCOMPRESSED;
    /* for stac beta code, need workaround to set srcLen=destLen */
    dlv.dest_len = dlv.src_len;
  }

  if (compress_debug) {
    j2   = dlv.src_p;
    buginf("\ndecomp! src_len= %X, dest_len=%X, destp=%X history @ %X, flags=%x",
	   dlv.src_len, dlv.dest_len, dlv.dest_p, dlv.hwidb->comp_db->stac_decomp_history, flags);
    junk = dlv.src_len;
    while ( junk > 0 ) {
      buginf("\ndecomp(RAW)%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x",
	     *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++,
	     *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++,
	     *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++,
	     *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++);
      junk = junk - 32;
    }
    j2 = dlv.dest_p;
    junk3 = dlv.dest_len;
    junk2 = checksum_history(dlv.hwidb->comp_db->stac_decomp_history, FALSE);
  }
  
  rval = LZS_Decompress(&dlv.src_p, &dlv.dest_p, &dlv.src_len, &dlv.dest_len,
			dlv.hwidb->comp_db->stac_decomp_history, flags);

  if (compress_debug) {
    junk = checksum_history(dlv.hwidb->comp_db->stac_decomp_history, FALSE);
    buginf("\ndecomp! stat=%4x len=%4x history(pre)=%X (post)=%X",
	   rval, dlv.dest_len, junk2, junk);
    junk = junk3 - dlv.dest_len;
    while ( junk > 0 ) {
      buginf("\ndecomp!   %2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x",
	     *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++,
	     *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++,
	     *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++,
	     *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++, *j2++);
      junk = junk - 32;
    }
    buginf("\n");
  }
  
  if ( !sane_after_decomp(&dlv, rval) )
    return NULL;
  else
    return dlv.newpak;
}

paktype *rand_decompress (hwidbtype *hwidb, paktype *pak)
{
paktype *newpak;
comp_hdr_t *header_p = (comp_hdr_t *) pak->datagramstart;
uchar *src_p, *dest_p;
ulong src_len;
ulong dest_len;
boolean anti_expansion = header_p->version & COMP_ANTI_EXPANSION ? TRUE : FALSE;

    if (hwidb->comp_db == NULL || hwidb->comp_db->comp_configed == 
      COMP_NOT_ENABLED) {
	return(NULL);
    }

    if (compress_debug) {
	buginf("\n DECOMP xmt_paks %d rcv_sync %d", header_p->xmt_paks, 
		hwidb->comp_db->comp_rcv_sync); 
    }
    /*->comp_rcv_sync & ->comp_xmt_sync are zero initially but never again */
    /* unless config t; no comp; comp; transition occurs                   */ 
    if (header_p->xmt_paks != (uchar) (hwidb->comp_db->comp_rcv_sync & COMP_SYNC_MASK)) {
	errmsg(&msgsym(SYNCBAD, COMP), hwidb->hw_namestring);
	lapb_reset(hwidb, "COMP sync error");
	NEW_MIB_STATE(hwidb, LAPB_MIBMODE_OTHER, LAPB_MIBREASON_OTHER)
	datagram_done(pak);
	return(NULL);
    }

    if ((header_p->version & COMP_VERSION_MASK) != hwidb->comp_db->comp_configed) {
	errmsg(&msgsym(VERSIONBAD, COMP), hwidb->hw_namestring,
		hwidb->comp_db->comp_configed, header_p->version);
	lapb_reset(hwidb, "COMP version error");
	NEW_MIB_STATE(hwidb, LAPB_MIBMODE_OTHER, LAPB_MIBREASON_OTHER)
	datagram_done(pak);
	return(NULL);
    }

    switch (hwidb->comp_db->comp_configed) {
	case COMP_RAND: 
	    if (anti_expansion) {
	    /*if anti expansion frame just strip off the comp header & adjust
	     *decompressor dictionary via decompress_rand_fixup() & return
	     *
	     *WARNING
	     * IP does not use pak->datagramstart.  
	     *       see ../ip/ip.h iptype_ & ip_sanity_check_inline(pak)
	     * decompress_rand_fixup() trims off comp header via a move.
	     */
		pak->datagramsize -= COMPHEADERBYTES;
		hwidb->comp_db->comp_rcv_out_bytes +=
			(ulong) pak->datagramsize + LAPB_OVERHEAD(hwidb);
		decompress_rand_fixup(hwidb->comp_db->comp_rcv_guess_table, 
		    &hwidb->comp_db->comp_rcv_hash, pak->datagramstart +
		    COMPHEADERBYTES, pak->datagramsize, pak->datagramstart);

		if ((++hwidb->comp_db->comp_rcv_sync & COMP_SYNC_MASK) == 0) 
		    hwidb->comp_db->comp_rcv_sync = 1;
		return (pak);
	    }

	    if ((newpak = getbuffer(header_p->org_size)) == NULL) {
		hwidb->comp_db->comp_rcv_no_bufs++;
		datagram_done(pak);
		return(NULL);
	    }
	    break;
	default:
	    errmsg(&msgsym(CONFIGBAD, COMP), hwidb->hw_namestring);
	    lapb_reset(hwidb, "COMP config error");
	    NEW_MIB_STATE(hwidb, LAPB_MIBMODE_OTHER, LAPB_MIBREASON_OTHER)
	    datagram_done(pak);
	    return(NULL);
	    /*break;*/
    }

    if ((++hwidb->comp_db->comp_rcv_sync & COMP_SYNC_MASK) == 0) 
	hwidb->comp_db->comp_rcv_sync = 1;

    pak_copy(pak, newpak, 0);

    src_p = pak->datagramstart +  COMPHEADERBYTES;
    src_len = pak->datagramsize - COMPHEADERBYTES;
    dest_p = newpak->datagramstart;
    /*decompress here*/
    switch (hwidb->comp_db->comp_configed) {
	case COMP_RAND:
	    decompress_rand(hwidb->comp_db->comp_rcv_guess_table, &hwidb->comp_db->comp_rcv_hash,
				src_p, src_len, dest_p, &dest_len);
	    newpak->datagramsize = (ushort) dest_len;
	    break;
	default:
	    break;	
    }

    hwidb->comp_db->comp_rcv_out_bytes +=
	(ulong) newpak->datagramsize + LAPB_OVERHEAD(hwidb);
    if (GETSHORT(&header_p->org_size) != newpak->datagramsize) {
	errmsg(&msgsym(SIZEBAD, COMP), hwidb->hw_namestring);
	lapb_reset(hwidb, "COMP size error");
	NEW_MIB_STATE(hwidb, LAPB_MIBMODE_OTHER, LAPB_MIBREASON_OTHER)
	datagram_done(newpak);
	datagram_done(pak);
	return(NULL);
    }

    datagram_done(pak);
    return(newpak);
}/* end decompress() */

/************************************/
/* End File: compress.c             */
/************************************/
