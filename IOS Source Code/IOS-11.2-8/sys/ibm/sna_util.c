/* $Id: sna_util.c,v 3.3.42.2 1996/05/17 11:21:32 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ibm/sna_util.c,v $
 *------------------------------------------------------------------
 * sna_util.c - utilities for SNA PIU manipulation
 *
 * 12/6/94, Anna O. Lam
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sna_util.c,v $
 * Revision 3.3.42.2  1996/05/17  11:21:32  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.30.3  1996/04/25  23:58:18  bchan
 * CSCdi55817:  gs7-s-mz broken
 * Branch: IbuMod_Calif_branch
 * move ConvertAsciiToEbcdic from dspu/snautil.c to ibm/sna_util.c
 *
 * Revision 3.3.30.2  1996/04/11  14:37:58  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.30.1  1996/04/03  14:34:48  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.42.1  1996/03/18  20:12:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.12.2  1996/03/07  09:42:36  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.12.1  1996/02/20  14:21:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/23  00:54:27  kpatel
 * CSCdi46079:  DLSw+ does not support non-activation XID
 *
 * Revision 3.2  1995/11/17  09:24:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:46:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/19  08:09:35  bchan
 * CSCdi27730:  SNA Fragmentation break if no buffers available
 *
 * Revision 2.2  1995/08/07  05:35:25  richl
 * CSCdi38315:  makefile rework
 *              move utility routines to libibm_util.a
 *
 * BUG: THERE'S NO PROVISION FOR WHAT TO DO IF YOU CAN'T GET A BUFFER
 * Revision 2.1  1995/06/07  20:48:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <string.h>
#include <ciscolib.h>
#include "packet.h"
#include "sna_util.h"
#include "ieee.h"

/*
 * Fragment a SNA frame into lots of little pieces, using the chaining bits
 * in the TH.  Use the send routine provided to get the  little blighters
 * on their way asap
 * 
 */
boolean
sna_th_frag (paktype *pak, ushort max_frag, queuetype* q)
{

    uchar *offset;		/* Offset from original into fragmented
				   packet */
    uchar origbiu;
    int bytesleft;		/* Bytes left */
    int cursize;
    uchar thheader[TH_HEADER_SIZE]; /* Common data for all frames */
    paktype *newpak;
    boolean firstframe, lastframe;


     queue_init(q, 0);
    /* Save the original segmentation information */
    origbiu = pak->datagramstart[0];

    /* Save original TH area */
    bcopy(pak->datagramstart, thheader, TH_HEADER_SIZE);

    /* Unless special cases, (see below) all fragments are "middle" */
    SNA_SET_MIDDLE_BIU((thheader[0]));

    /* Loop through the fragments, building them... */
    for ( firstframe = TRUE, lastframe = FALSE,
	     offset = pak->datagramstart + TH_HEADER_SIZE,
	     bytesleft = pak->datagramsize - TH_HEADER_SIZE;
	     bytesleft;
	     bytesleft -= (cursize - TH_HEADER_SIZE),
	     offset += (cursize - TH_HEADER_SIZE) ) {

       /* Get the current packet size */
       if ( (bytesleft + TH_HEADER_SIZE) <= max_frag ) {
          lastframe = TRUE;
          cursize = bytesleft + TH_HEADER_SIZE;
       } else 
	   cursize = max_frag;

       /* Get/initialize the packet */
       if ( !(newpak = getbuffer(cursize) ) ) {
          while ((newpak = dequeue(q)) != NULL)
	       datagram_done(newpak);
          return FALSE;
       }

       newpak->if_output     = pak->if_output;
       newpak->datagramsize  = cursize;
       newpak->datagramstart = newpak->network_start;

       /* Copy the TH header */
       bcopy( thheader, newpak->datagramstart, TH_HEADER_SIZE );

       /* Copy the appropriate portion of the frame */
       bcopy( offset, (newpak->datagramstart + TH_HEADER_SIZE),
		              (cursize - TH_HEADER_SIZE) );

       /* Set the headers appropriately */
       if (firstframe) {
          firstframe = FALSE;

          switch ( SNA_BIU(origbiu) ) {

             case SNA_TH_FIRST_BIU:
             case SNA_TH_WHOLE_BIU:
                /* Make first */
                SNA_SET_FIRST_BIU( newpak->datagramstart[0] );
                break;

             case SNA_TH_LAST_BIU:
             case SNA_TH_MIDDLE_BIU:
                /* In these cases, do nothing. Leave as "middle" */
                break;

             default:
                break;
          }
       } else if (lastframe) {

                 switch ( SNA_BIU(origbiu) ) {

                    case SNA_TH_MIDDLE_BIU:
                    case SNA_TH_FIRST_BIU:
                       /* In these cases, do nothing. Leave as "middle" */
                       break;

                    case SNA_TH_WHOLE_BIU:
                    case SNA_TH_LAST_BIU:
                       /* In these cases, make "last" biu */
                       SNA_SET_LAST_BIU(newpak->datagramstart[0]);
                       break;
                 }
       }
       enqueue(q, newpak);
   }
   datagram_done(pak);
   return TRUE;

}

void sendqueue (queuetype* q, void (* sendfrag)(void *controlblock, paktype *pak), 
                              void* controlblock)
{
    paktype* pak;
    while ((pak = dequeue(q)) != NULL)
       (*sendfrag)(controlblock, pak);
}

boolean valid_XID(char *buffs, uchar *xid, int xid_size)
{
    char *buffe;
    char c;
    int i;
    boolean parse_error = FALSE;

    buffe 	= buffs + strlen(buffs) -1;

    /* Skip silly 0x or 0X */
    if (*buffs == '0')
       if ((*(buffs + 1) == 'x') || (*(buffs + 1) == 'X'))
           buffs += 2;

    if ( (buffe - buffs) != ( (xid_size * 2) - 1) ) {
	parse_error = TRUE;
    } else {
	for (i = 0; (buffe >= buffs); i++) {

	    c = toupper(*buffs++);
	    if (!(((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'F')))) {
		parse_error = TRUE;
		break;
	    }
        xid[i] = (uchar)
                  ((uchar) c - (((c >= '0') && (c <= '9')) ? '0' : '7')) << 4;

        c = toupper(*buffs++);
        if (!(((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'F')))) {
            parse_error = TRUE;
	    break;
	}
        xid[i] |= (uchar)
            ((uchar) c - (((c >= '0') && (c <= '9')) ? '0' : '7'));
	}
    }
    if (parse_error) {
	printf("\nAn XID string must consist of %u hex digits.",
	       (xid_size * 2));
	return( FALSE );
    }
    return ( TRUE );
}

/*
 * i s _ x i d 3
 *
 * quick test to make sure that we're interpreting a packet correctly
 * before reading it
 */
 
boolean is_xid3(paktype *pak)
{
    XID3type *xid;
 
    if (pak->datagramsize < sizeof(XID3type))
        return(FALSE);
 
    xid = (XID3type *)(pak->datagramstart);
 
    if ( (xid->xid3_fmt & XID_FMT_MASK) != XID_XID3)
        return(FALSE);
 
    return (TRUE);
}
 
#ifdef NOBODY_CALLS_ME
/*
 * x i d _ s p o o f _ a b m
 *
 * note the abm bit we received, and drop in the ABM bit the partner
 * wants
 */
void xid3_spoof_abm(paktype *pak, uchar *from_abm, uchar *to_abm)
{
    XID3type *xid = (XID3type *)pak->datagramstart;
 
    /* pick up the input value */
 
    *from_abm = xid->xid3_lsflags & XID3_ABM;
 
    /* plop in the new one */
    if (*to_abm)
        xid->xid3_lsflags |= XID3_ABM;
    else
        xid->xid3_lsflags &= ~XID3_ABM;
}
#endif /* NOBODY_CALLS_ME */
 
/*
 * x i d _ g l e a n _ m a x b t u
 *
 * extract the max btu from the XID-3, without having to worry about
* odd/even byte boundaries
*/
 
void xid3_glean_maxbtu( XID3type *xid, uchar *stash)
{
    PUTSHORT (stash, GETSHORT(&xid->xid3_maxbtu[0]));
}
 
/* 
 * Given a pointer to the start of the XID data, get and set an
 * assortment of interesting fields. All the hard coded numbers you
 * see in here come straight out of the IBM Formats GA27-3136-13.
 * These offsets should be easier to maintain than a structure because
 * you don't need to worry about how a compiler might add padding to a
 * structure. 
 */

/*
 * Return the format. It should be one of the following:
 *
 * XID_FMT_0, XID_FMT_1, XID_FMT_2, XID_FMT_3, XID_FMT_8
 */
uchar xid_get_format(const uchar* ptr)
{
    return (*ptr & XID_FMT_MASK);
}

 /*
  * i s _ v a l i d _ x i d
  * returns true if xid type is a known type of 0, 1, 2, 3, or 8.
  */
boolean is_valid_xid(paktype *pak)
 {
   uchar* xidPtr;
 
   xidPtr = NULL;
      
      if (pak) {
 
          xidPtr = pak->datagramstart;
 	 if (xidPtr) {
 	     switch (xid_get_format(xidPtr)) {
 	       case XID_FMT_0:
 	       case XID_FMT_1:
 	       case XID_FMT_2: 
 	       case XID_FMT_3:
 	       case XID_FMT_8:
 		 return TRUE;
 	       default:
 		 /* fall through */
 	    }
 	 }
      }
   return FALSE;
}
 





/* 
 *  Return the node type of the XID sender. Should be one of the
 * following:
 * 
 * XID_TYPE_1, XID_TYPE_2, XID_TYPE_4
 */
uchar xid_get_node_type(const uchar* ptr)
{
    return (*ptr & XID_TYPE_MASK);
}

char xid_get_length(const uchar* ptr)
{
    switch (xid_get_format(ptr)) {
    case XID_FMT_0:
	/* 
	 * Fixed format XID0 has 4 byte IDBLK/IDNUM
	 */
	return (XID0_LENGTH);

    case XID_FMT_8:
	return(XID_FMT_STD_LEN);

    case XID_FMT_1:
    case XID_FMT_2:  /* not sure about this one*/
    case XID_FMT_3:
    default:
	return (*(ptr + XID_LEN_OFFSET));
    }
}

#ifdef NOBODY_CALLS_ME
boolean xid3_get_abm(const uchar* ptr)
{
    if (*(ptr + XID3_ABM_OFFSET) & XID3_ABM)
	return TRUE;
    else
	return FALSE;
}
#endif /* NOBODY_CALLS_ME */

void xid3_set_abm(uchar* ptr, boolean abm)
{
    if (abm)
	*(ptr+XID3_ABM_OFFSET) |= XID3_ABM;
    else
	*(ptr+XID3_ABM_OFFSET) &= ~XID3_ABM;
}

uchar xid3_get_maxiframe(const uchar* ptr)
{
    return ((*(ptr+XID3_MAXIFRAME_OFFSET)) & 0x7F);	/* bit 0 is reserved */
}

#ifdef NOBODY_CALLS_ME
void xid3_set_maxiframe(uchar* ptr, uchar maxiframe)
{
    *(ptr+XID3_MAXIFRAME_OFFSET) = maxiframe & 0x7F;	/* bit 0 is reserved */
}

/*
 * Get and set the maxbtu size field in an XID_FMT_3. 
 * 
 * IMPORTANT: Assumes machine byte ordering
 */

ushort xid3_get_maxbtu(const uchar* ptr)
{
    return (GETSHORT(ptr+XID3_MAXBTU_OFFSET));
}

void xid3_set_maxbtu(uchar* ptr, ushort maxbtu)
{
    PUTSHORT((ptr+XID3_MAXBTU_OFFSET), maxbtu);
}
#endif /* NOBODY_CALLS_ME */


/* 
 * xid3_is_nonactivation
 *
 * Test XID3 to see if it is a non-activation XID
 */
boolean xid3_is_nonactivation(const uchar *ptr)
{
    if ((*(ptr + XID3_NONACTIVATION_OFFSET) & XID3_NONACTIVATION)         
                                                    == XID3_NONACTIVATION)
	return TRUE;
    else
	return FALSE;
}

/* 
 * xid_is_nonactivation
 *
 * Test an XID to see if it is a non-activation XID
 */
boolean xid_is_nonactivation(const uchar *ptr)
{
    if (xid_get_format(ptr) != XID_FMT_3)
        return FALSE;

    if (xid3_is_nonactivation(ptr))
        return TRUE;
    else
        return FALSE;
}

const uchar asciiToEbcdic[]  = {
                                 0x40      /* SP */
                                ,0x5A      /* ! */
                                ,0x7F      /* " */
                                ,0x7B      /* # */
                                ,0x5B      /* $ */
                                ,0x6C      /* % */
                                ,0x50      /* & */
                                ,0x7d      /* ' */
                                ,0x4D      /* ( */
                                ,0x5D      /* ) */
                                ,0x5C      /* * */
                                ,0x4E      /* + */
                                ,0x6B      /* ' */
                                ,0x60      /* - */
                                ,0x4B      /* . */
                                ,0x61      /* / */
                                ,0xF0           /* 0	*/
                                ,0xF1           /* 1	*/
                                ,0xF2           /* 2	*/
                                ,0xF3           /* 3	*/
                                ,0xF4           /* 4	*/
                                ,0xF5           /* 5	*/
                                ,0xF6           /* 6	*/
                                ,0xF7           /* 7	*/
                                ,0xF8           /* 8	*/
                                ,0xF9           /* 9	*/
                                ,0x7A           /* :    */
                                ,0x5E           /* ;    */
                                ,0x4C            /* <    */
                                ,0x7E            /* =    */
                                ,0x6E            /* >    */
                                ,0x6F            /* ?    */
                                ,0x7C            /* @    */
                                ,0xC1           /* A    */
                                ,0xC2           /* B    */
                                ,0xC3           /* C    */
                                ,0xC4           /* D    */
                                ,0xC5           /* E    */  
                                ,0xC6           /* F    */
                                ,0xC7           /* G    */
                                ,0xC8           /* H    */
                                ,0xC9           /* I    */
                                ,0xD1           /* J    */
                                ,0xD2           /* K    */
                                ,0xD3           /* L    */
                                ,0xD4           /* M    */
                                ,0xD5           /* N    */
                                ,0xD6           /* O    */
                                ,0xD7           /* P    */
                                ,0xD8           /* Q    */
                                ,0xD9           /* R    */
                                ,0xE2           /* S    */
                                ,0xE3           /* T    */
                                ,0xE4           /* U    */
                                ,0xE5           /* V    */
                                ,0xE6           /* W    */
                                ,0xE7           /* X    */ 
                                ,0xE8           /* Y    */
                                ,0xE9           /* Z    */
                                ,0x4D           /* [ -> (   */  
                                ,0xE9           /* \    */
                                ,0x5D           /* ] -> )   */                          
                                ,0x5F           /* >    */
                                ,0x6D           /* -    */
                                ,0x79           /*      */
                                ,0x81           /* a    */
                                ,0x82           /* b    */
                                ,0x83           /* c    */
                                ,0x84           /* d    */
                                ,0x85           /* e    */
                                ,0x86           /* f    */
                                ,0x87           /* g    */ 
                                ,0x88           /* h    */
                                ,0x89           /* i    */
                                ,0x91           /* j    */
                                ,0x92           /* k    */
                                ,0x93           /* l    */
                                ,0x94           /* m    */
                                ,0x95           /* n	*/
                                ,0x96           /* o	*/
                                ,0x97           /* p	*/
                                ,0x98           /* q	*/
                                ,0x99           /* r	*/
                                ,0xA2           /* s	*/
                                ,0xA3           /* t	*/
                                ,0xA4           /* u	*/
                                ,0xA5           /* v	*/
                                ,0xA6           /* w	*/
                                ,0xA7           /* x	*/
                                ,0xA8           /* y	*/
                                ,0xA9           /* z	*/
                                };

/*=====================================================================*
* ConvertAsciiToEbcdic
*======================================================================*
*
* Description:
*   This routine converts a given ascii string to ebcdic.    
*   
**/
void ConvertAsciiToEbcdic
       (const uchar * ascii
       ,uchar       * ebcdic
       ,int         length)
{
   uchar c;
   while (length-- > 0){
      if ((c = *ascii++) < ' ') {
           c = 0x15;          /* convert unprintable character to nl */
       }
      else if (c > 'z') {
          c = 0x6F;            /* convert > z to ? */
      }
      else {
          c = asciiToEbcdic[c - ' '];;
      }
      *ebcdic++ = c;
  }
}
