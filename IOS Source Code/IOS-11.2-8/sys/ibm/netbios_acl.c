/* $Id: netbios_acl.c,v 3.7.4.2 1996/05/17 11:20:48 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ibm/netbios_acl.c,v $
 *------------------------------------------------------------------
 * NetBIOS (over LLC2 and Novell IPX) access list support routinges
 *
 * January 21, 1991 - Joel P. Bion
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: netbios_acl.c,v $
 * Revision 3.7.4.2  1996/05/17  11:20:48  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.4.7  1996/05/05  23:08:21  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.5.4.6  1996/05/02  20:06:50  gcox
 * Branch: IbuMod_Calif_branch
 * IBU Modularity - add ibu_int subsystem
 *
 * Revision 3.5.4.5  1996/04/26  07:54:19  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.5.4.4  1996/04/03  14:34:09  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.4.3  1996/03/19  02:04:37  ppearce
 * IBU modularity - NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Incorporate NetBios session access lists into NetBios subblock
 *
 * Revision 3.5.4.2  1996/03/17  17:50:27  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.5.4.1  1996/03/16  12:09:17  ppearce
 * CSCdi50498:  IBU modularity - Implement NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Move NetBios fields from hwidbtype/idbtype structs to NetBios subblock
 *
 * Revision 3.7.4.1  1996/03/18  20:11:39  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.4  1996/03/16  06:53:06  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.20.3  1996/03/13  01:23:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.20.2  1996/03/07  09:42:04  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.20.1  1996/02/20  00:54:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/03/07  06:34:08  kchiu
 * CSCdi40165:  Add netbios session level byte filtering support
 *
 * Revision 3.6  1996/03/04  07:14:07  kchiu
 * CSCdi49101:  make netbios name recognized filtering optional
 *
 * Revision 3.5  1996/02/07  16:11:52  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/02/01  06:03:06  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/01  06:23:12  mschaef
 * CSCdi37234:  Allow fastswitching of certain directed broadcast packets
 *
 * Revision 3.2  1995/11/17  09:23:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/10/03  15:07:47  fbordona
 * CSCdi40884:  crashdump netbios_acfcheck(0x301faf0+0x2c9844)+0x160)
 *              Add DLSw callback when NetBios access list removed
 *
 * Revision 2.3  1995/06/28  09:25:37  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/18  23:36:44  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:47:30  hampton
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
#include "interface_private.h"
#include "netbios.h"
#include "netbios_sb.h"
#include "../srt/netbios_srb.h"
#include "access_expr.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_netbios.h"
#include "packet.h"
#include "../xns/novell.h"
#include "sys_registry.h"
#include "ieee.h"
#include "subsys.h"
#include "../if/tring.h"
#include "../srt/srt_registry.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/isis_ipx.h"
#include "../xns/ipx_registry.h"


/*
 * Global Storage and forward references
 */
queuetype netbios_host_accQ;
queuetype netbios_bytes_accQ;
static void netbios_acl_init(subsystype *);
int netbios_name_length;
boolean netb_acc_filter_name_recognized;

/*
 * Netbios access list subsystem header
 */
#define	NETBIOSACL_MAJVERSION 1
#define	NETBIOSACL_MINVERSION 0
#define	NETBIOSACL_EDITVERSION 1

SUBSYS_HEADER(netbios_acl, NETBIOSACL_MAJVERSION,
	      NETBIOSACL_MINVERSION, NETBIOSACL_EDITVERSION,
	      netbios_acl_init, SUBSYS_CLASS_PROTOCOL,
	      "req: ibu_int",
	      NULL);

/*
 * netbios_acl
 *
 * Parse a NETBIOS access list entry (global)
 * Form:
 *	no netbios access-list host <name>
 *	[no] netbios access-list host <name> permit|deny <wildstring>
 *
 * First version deletes an entire netbios name list, the second version
 * adds/deletes entry in such a list.  <name> is a name for the list,
 * and <wildstring> is a string, where "*" (end only) indicates a prefix
 * where any characters may follow in the match, and "?" indicates a wildcard
 * match on a single character.
 *
 *      no netbios access-list bytes <name>
 *      [no] netbios access-list bytes <name> permit|deny <n> hex-string
 *
 * First version deletes an entire netbios name list, the second version
 * adds/deletes entry in such a list.  <name> is a name for the list,
 * and <n> is an offset into the beginning of the NETBIOS frame (you must
 * know the frame format to do this), and hex-string is a string to compare
 * in the frame. The length of the comparison is the length of the string.
 * Wildcards for single bytes are represented by the TWO-CHARACTER-STRING "**".
 * Such access lists are by their variablue nature quite slow and should only
 * be used where a special security is required and the "host" access list
 * does not meet the needs.
 *
 */
void netbios_acl (parseinfo *csb)
{
    int netbstrlen, i, netboffset;
    netbios_acclisttype *netblist;
    netbios_acctype *netb;
    uchar *chp, *chm;
    long netbaddr;
    idbtype *idb;
    boolean netbresult, netbprefix;
    queuetype *netbios_accQ = NULL;
    boolean isbytes = FALSE;
    ulong netbwildpattern = 0;

    if (csb->nvgen) {
	char hold[NETBIOS_MAX_NAME_LEN * 2 + 1];
	uchar *patternp;

	switch (csb->which) {
	  case NETBIOS_ACCESS_BYTES:
	    for (netblist = netbios_bytes_accQ.qhead;
		 netblist;
		 netblist = netblist->next) {
		for (netb = (netbios_acctype *) netblist->q.qhead; netb;
		     netb = netb->next) {
		    for (i = 0, patternp = (uchar *) &(netb->pattern[0]);
			 i < netb->length; i++, patternp++) {
			if (((uchar *) netb->mask)[i]) {
			    hold[i * 2] = '*'; hold[i * 2 + 1] = '*';
			} else {
			    sprintf(&(hold[i * 2]), "%02x", *patternp);
			}
		    }
		    hold[netb->length * 2] = '\0';
		    nv_write(TRUE, "%s %s %s %d %s", csb->nv_command,
			     netblist->name,
			     (netb->result ? "permit" : "deny"),
			     netb->offset, hold);
		}
	    }
	    break;

	  case NETBIOS_ACCESS_HOST:
	    for (netblist = (netbios_acclisttype *) netbios_host_accQ.qhead;
		 netblist;
		 netblist = netblist->next) {
		for (netb = (netbios_acctype *) netblist->q.qhead; netb;
		     netb = netb->next) {
		    for (patternp = (uchar *) netb->pattern, i = 0;
			 i < netb->length; i++, patternp++) {
			hold[i] = (*patternp == '\0') ? '?' : *patternp;
		    }
		    if (netb->prefix) {
			hold[i++] = '*';
		    }
		    hold[i] = '\0';
		    nv_write(TRUE, "%s %s %s %s", csb->nv_command,
			     netblist->name,
			     (netb->result ? "permit" : "deny"),
			     hold);
		}
	    }
	    break;
          case NETBIOS_ACCESS_FILTER_NAME_RECOGNIZED:
            nv_write(netb_acc_filter_name_recognized, "%s", csb->nv_command);
            break;

	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }


    switch (csb->which) {
      case NETBIOS_ACCESS_BYTES:
	isbytes = TRUE;
	netbios_accQ = &netbios_bytes_accQ;
	break;
      case NETBIOS_ACCESS_HOST:
	isbytes = FALSE;
	netbios_accQ = &netbios_host_accQ;
	break;
      case NETBIOS_ACCESS_FILTER_NAME_RECOGNIZED:
        netb_acc_filter_name_recognized = csb->sense;
        return;
      default:
	bad_parser_subcommand(csb, csb->which);
	return;
    }

    netblist = find_namedthing(netbios_accQ, GETOBJ(string,1));

    /* Define list if it is not there... (and sense is positive) */
    if ((csb->sense) && (netblist == NULL)) {
	if ((netblist = malloc(sizeof(netbios_acclisttype))) == NULL) {
	    printf(nomemory);
	    return;
	}
	sstrncpy(netblist->name, GETOBJ(string,1), NAMEDTHINGNAMELEN);
	enqueue(netbios_accQ, netblist);
        /*
        **  Update all access-expressions on all interfaces that might
        **  reference this NetBIOS access list.
        **
        **  N.B. This is done to cover the case where a NetBIOS access-list
        **       has been deleted and is being added back here with the
        **       same name.
        */
        expr_update_netbios_access_list(netblist, EXPR_ADD_NETBIOS_LIST);
    }

    /* If no other keywords delete the entire list (if sense == FALSE) */
    if (GETOBJ(int,1) == 0) {
	if ((!csb->sense) && (netblist != NULL)) {
	    unqueue(netbios_accQ, netblist);
            /*
            **  Purge all access-expressions on all interfaces of this
            **  NetBIOS access list.
            */
            expr_update_netbios_access_list(netblist,
					    EXPR_DELETE_NETBIOS_LIST);
            reg_invoke_srt_remove_netbios_wan_filter(netblist);
            reg_invoke_dlsw_remove_netbios_wan_filter(netblist);
	    FOR_ALL_SWIDBS(idb) {
                netbios_sb_t *netb_sb = netbios_get_or_create_sb(idb);
		if (netb_sb->ibm_netbios_b_iacf == netblist)
  		    netb_sb->ibm_netbios_b_iacf = NULL;
		if (netb_sb->ibm_netbios_bs_iacf == netblist)
  		    netb_sb->ibm_netbios_bs_iacf = NULL;
		if (netb_sb->ibm_netbios_b_oacf == netblist)
		    netb_sb->ibm_netbios_b_oacf = NULL;
		if (netb_sb->ibm_netbios_bs_oacf == netblist)
  		    netb_sb->ibm_netbios_bs_oacf = NULL;
		if (netb_sb->ibm_netbios_h_iacf == netblist)
		    netb_sb->ibm_netbios_h_iacf = NULL;
		if (netb_sb->ibm_netbios_h_oacf == netblist)
		    netb_sb->ibm_netbios_h_oacf = NULL;
		if (netb_sb->nov_netbios_b_iacf == netblist)
  		    netb_sb->nov_netbios_b_iacf = NULL;
		if (netb_sb->nov_netbios_b_oacf == netblist)
		    netb_sb->nov_netbios_b_oacf = NULL;
		if (netb_sb->nov_netbios_h_iacf == netblist)
		    netb_sb->nov_netbios_h_iacf = NULL;
		if (netb_sb->nov_netbios_h_oacf == netblist)
		    netb_sb->nov_netbios_h_oacf = NULL;
	    }
	    for (netb = (netbios_acctype *) netblist->q.qhead; netb;) {
		netbaddr = (ulong) netb->next;
		free(netb);
		netb = (netbios_acctype *) netbaddr;
	    }
	    free(netblist);
	}
	return;
    }

    /*
     * Oh, goody. Some actual keywords. Parse them and add an entry
     * (intelligently) to the list.
     */
    /* "permit" = 1; "deny" = 2 */
    netbresult = GETOBJ(int,1) == 1 ? TRUE : FALSE;
    netboffset = GETOBJ(int,2);
    netbstrlen = strlen(GETOBJ(string,2));

    if (!isbytes) {
	/* Parse the prefix indicator */
	if (GETOBJ(string,2)[netbstrlen - 1] == '*') {
	    netbprefix = TRUE;
	    GETOBJ(string,2)[netbstrlen - 1] = '\0';
	    netbstrlen -= 1;
	} else {
	  netbprefix = FALSE;
	}

	/* Clean up ul from garbage left over from GETOBJ(string,2) */
	for (chp = (uchar *) GETOBJ(string,2), i = 0; i < NETBIOS_MAX_NAME_LEN; i++)
	  chp[i] = ((i < netbstrlen) ?
		    (((GETOBJ(string,2))[i] == '?') ? '\0' : (GETOBJ(string,2))[i]) :
		    ((netbprefix) ? '\0' : ' '));
    } else {
	uchar *buffs, *buffe, c;

	/* Parse bytes info */ 
	netbprefix = FALSE;
	buffs = (uchar *) &(GETOBJ(string,2)[0]);
	buffe = buffs + netbstrlen - 1;

	if (!((buffe - buffs) & 0x1)) {
	    printf("\nByte patterns must be an even number of hex digits");
	    return;
	}
	
	/* Skip silly 0x or 0X */
	if (*buffs == '0')
	    if ((*(buffs + 1) == 'x') || (*(buffs + 1) == 'X'))
	        buffs += 2;
	
	for (i = 0; (buffe >= buffs); i++) {
	    if (*buffs == '*') {
		if (*(++buffs) != '*') {
		    printf("\nIllegal netbios byte pattern");
		    return;
		} else {
		    buffs++;
		    netbwildpattern |= (0x1 << i);
		    GETOBJ(string,2)[i] = '\0';
		}
	    } else {
		c = toupper(*buffs++);
		if (!(((c >= '0') && (c <= '9')) ||
		      ((c >= 'A') && (c <= 'F')))) {
		    printf("\nIllegal netbios byte pattern");
		    return;
		}
		GETOBJ(string,2)[i] = (uchar)
		  ((uchar) c - (((c >= '0') && (c <= '9')) ? '0' : '7')) << 4;
		c = toupper(*buffs++);
		if (!(((c >= '0') && (c <= '9')) ||
		      ((c >= 'A') && (c <= 'F')))) {
		    printf("\nIllegal netbios byte pattern");
		    return;
		}
		GETOBJ(string,2)[i] |= (uchar)
		  ((uchar) c - (((c >= '0') && (c <= '9')) ? '0' : '7'));
	    }
	}
	netbstrlen = i;
	for (i = netbstrlen; i < NETBIOS_MAX_NAME_LEN; i++) {
	    GETOBJ(string,2)[i] = '\0';
	}
    }

    if (netbstrlen > NETBIOS_MAX_NAME_LEN) {
	printf("\nNETBIOS pattern length too long -- maximum is %d",
	       NETBIOS_MAX_NAME_LEN);
	return;
    }


    /* See if we already know about this one... */
    for (netb = (netbios_acctype *) netblist->q.qhead; netb;
	 netb = netb->next) {
	if ((netbprefix != netb->prefix) || (netbstrlen != netb->length) ||
	    (netboffset != netb->offset))
	    continue;
	if (!bcmp((uchar *) netb->pattern, (uchar *) (GETOBJ(string,2)),
		  netbios_name_length)) {
	    if (csb->sense) {
		netb->result = netbresult;
	    } else {
		unqueue(&(netblist->q), netb);
		free(netb);
	    }
	    return;
	}
    }

    /* A new one. Build mask, pattern, etc. */

    /* ...but only if there is no "no" in front of it... */
    if (!csb->sense) {
      return;
    }
    
    if ((netb =  malloc(sizeof(netbios_acctype))) == NULL) {
	printf(nomemory);
	return;
    }

    netb->result = netbresult;
    netb->prefix = netbprefix;
    netb->length = netbstrlen;
    netb->offset = netboffset;

    /*
     * This builds an "ignore" mask for the name, for fast comparisons
     * at interrupt level (complex code here makes for simple code there)
     */
    for (i = 0, chp = (uchar *) netb->pattern, chm = (uchar *) netb->mask;
	 (i < NETBIOS_MAX_NAME_LEN); i++, chp++, chm++) {
	*chp = GETOBJ(string,2)[i];
	if (!isbytes) {
	    *chm = (*chp == '\0' && (netbprefix || (i < netbstrlen))) ?
	      '\377' : '\0';
	} else {
	    *chm = ((netbwildpattern >> i) & 0x1) ? '\377' : '\0';
	}
    }
    enqueue(&(netblist->q), netb);	
}

/*
 * netbios_acfcheck
 *
 * Given a frame whose data starts at "dgstart" and size is "dgsize", coming
 * into or out of "idb" (as specified by "incoming"). The type of frame is
 * specified in 'kind'.
 */
boolean netbios_acfcheck (uchar *dgstart, ulong dgsize, idbtype *idb, 
			  boolean incoming, netbios_acclisttype *listtouse,
			  boolean ishost, int kind)
{
    netbios_header *nbh = NULL;
    uchar *field = NULL;
    uchar *bytes_offset = NULL;
    netbios_acctype *netb;
    ulong pattern[NETBIOS_MAX_NAME_ULONG];
    int i, length;
    boolean result;
    netbios_sb_t *netb_sb = netbios_get_sb(idb);
    netbios_acclisttype *thelist = NULL;
    uchar netbios_dest_mask;
    
    if (kind == NETBIOS_TOKEN_LLC2_CHECK) {
	nbh = (netbios_header *) dgstart;

	/* If delimiter not NETBIOS, let it through */
	if ((nbh->delimiter[0] != NETBIOS_DELIMITER_0) ||
	    (nbh->delimiter[1] != NETBIOS_DELIMITER_1))
	  return(TRUE);

        netbios_dest_mask = (netb_acc_filter_name_recognized ?
                             NETBIOS_DEST_MASK_2 : NETBIOS_DEST_MASK_1);
	if ((nbh->command & ~(NETBIOS_SRC_MASK)) == NETBIOS_SRC_VALUE)
	  field = nbh->srcname;
	else if ((nbh->command & ~(netbios_dest_mask)) == NETBIOS_DEST_VALUE)
	  field = nbh->destname;
	if (listtouse)
	  thelist = (ishost) ? listtouse : NULL;
	else if (incoming)
	  thelist = (netb_sb ? netb_sb->ibm_netbios_h_iacf : NULL);
	else
	  thelist = (netb_sb ? netb_sb->ibm_netbios_h_oacf : NULL);
    } else {
	field = (uchar *)
	  &(((novellnetbiosheader *) dgstart)->nov_netbios_server[0]);
	if (listtouse)
	  thelist = (ishost) ? listtouse : NULL;
	else if (incoming)
	  thelist = (netb_sb ? netb_sb->nov_netbios_h_iacf : NULL);
	else
	  thelist = (netb_sb ? netb_sb->nov_netbios_h_oacf : NULL);
    }

    
    if (field && thelist) {

        netb = (netbios_acctype *) (thelist->q.qhead);
	result = FALSE;

	memset(pattern, 0, NETBIOS_MAX_NAME_LEN);
	/* Allows for faster compares without worrying about word allignment */
	bcopy(field, (uchar *) pattern, netbios_name_length);

	/* Ugh... space fill as some impl. use null-pad others, space pad */
	for (length = -1, field = (uchar *) pattern, i = 0;
	     i < NETBIOS_MAX_NAME_LEN;
	     i++, field++) {
	    if ((*field == '\0') || (*field == ' ')) {
		if (length < 0)
		  length = i;
		if (*field == '\0')
		  *field = ' ';
	    }
	}

	if (length < 0)
	    length = netbios_name_length;
        else if (length > 0) {
	/* always use NETBIOS_MAX_NAME_LEN for access-list filter */
	    field = (uchar *) pattern;
	    if (netbios_name_length < NETBIOS_MAX_NAME_LEN)
		for (i = netbios_name_length;
		    i < NETBIOS_MAX_NAME_LEN; i++)
		    field[i] = ' ';
        }

	/*
	 * Novell NetBIOS is only 14 characters long. Fill in the rest with
	 * spaces
	 */
	if (kind == NETBIOS_IPX_CHECK) {
	    field = (uchar *) pattern;
	    field[14] = field[15] = ' ';
	}
	 
	for (; netb; netb = netb->next) {
	    if ((length == netb->length) ||
		(netb->prefix && (length > netb->length))) {
		/* (Relatively) fast compare of NETBIOS access lists... */
		if ((netb->pattern[0] == (pattern[0] & ~netb->mask[0])) &&
		    (netb->pattern[1] == (pattern[1] & ~netb->mask[1])) &&
		    (netb->pattern[2] == (pattern[2] & ~netb->mask[2])) &&
		    (netb->pattern[3] == (pattern[3] & ~netb->mask[3]))) {
		    result = netb->result;
		    break;
		}
	    }
	}
	if (result == FALSE)
	    return(FALSE);
    }

    /* Either there were no host access lists, or the host list gave true */
    if (kind == NETBIOS_TOKEN_LLC2_CHECK) {
	bytes_offset = dgstart;
	if (listtouse)
	  thelist = (!ishost) ? listtouse : NULL;
	else if (incoming)
	  thelist = (netb_sb ? netb_sb->ibm_netbios_b_iacf : NULL);
	else
	  thelist = (netb_sb ? netb_sb->ibm_netbios_b_oacf : NULL);
    } else {
	bytes_offset = (uchar *)
	  &(((novellnetbiosheader *) dgstart)->nov_netbios_nametypeflag);
	if (listtouse)
	  thelist = (!ishost) ? listtouse : NULL;
	else if (incoming)
	  thelist = (netb_sb ? netb_sb->nov_netbios_b_iacf : NULL);
	else
	  thelist = (netb_sb ? netb_sb->nov_netbios_b_oacf : NULL);
    }

    if (thelist) {
	netb = (netbios_acctype *) (thelist->q.qhead);

	for (; netb; netb = netb->next) {
	    if ((netb->offset + netb->length) > dgsize) {
		continue;
	    }
	    field = bytes_offset + netb->offset;
	    for (i = 0; i < netb->length; i++) {
		if ((((uchar *) netb->pattern)[i]) !=
		    (field[i] & ~(((uchar *) netb->mask)[i]))) {
		    break;
		}
	    }
	    if (i == netb->length) {
		return(netb->result);
	    }
	}
	return(FALSE);
    }
    return(TRUE);
}    

/* 
 * Session level byte filtering
 */
boolean netbios_session_acfcheck(uchar *bytes_offset, ulong dgsize,
                            netbios_acclisttype *thelist)
{
    uchar *field = NULL;
    netbios_acctype *netb;
    int i;

    netb = (netbios_acctype *) (thelist->q.qhead);
    for (; netb; netb = netb->next) {
        if ((netb->offset + netb->length) > dgsize) {
            continue;
        }
        field = bytes_offset + netb->offset;
        for (i = 0; i < netb->length; i++) {
            if ((((uchar *) netb->pattern)[i]) !=
                (field[i] & ~(((uchar *) netb->mask)[i]))) {
                break;
            }
        }
        if (i == netb->length) {
            return(netb->result);
        }
    }
    return(FALSE);
}


boolean netbios_expr_host_check (
    uchar *dgstart,
    ulong dgsize,
    netbios_acclisttype *netbios_list)
{
    return(netbios_acfcheck(dgstart, dgsize, NULL, FALSE, netbios_list, 
			    TRUE, NETBIOS_TOKEN_LLC2_CHECK));
}

boolean netbios_expr_bytes_check (
    uchar *dgstart,
    ulong dgsize,
    netbios_acclisttype *netbios_list)
{
    return(netbios_acfcheck(dgstart, dgsize, NULL, FALSE, netbios_list, 
			    FALSE, NETBIOS_TOKEN_LLC2_CHECK));
}

/*
 * netbios_acl_if_command
 *
 * Parse interface specific netbios access list commands
 */
void netbios_acl_if_command (parseinfo *csb)
{
    idbtype *idb;
    hwidbtype *hwidb;
    boolean isbytes = FALSE;
    queuetype *netbios_accQ = NULL;
    netbios_sb_t *netb_sb;
    netbios_acclisttype *netblist;

    idb = csb->interface;
    hwidb = hwidb_or_null(idb);

    netb_sb = netbios_get_or_create_sb(idb);
    if (!netb_sb) {
      printf(nomemory);
      return;
    }
    if (csb->nvgen) {
	netbios_acclisttype *p;

	switch (csb->which) {
	  case NETBIOS_ACCESS_FILTER:
	    if (GETOBJ(int,1) == NETB_IAFILTER) {
		if (GETOBJ(int,2) == NETBIOS_BYTES_FILTER) {
	            p = netb_sb->ibm_netbios_b_iacf;
                } else if (GETOBJ(int,2) == NETBIOS_SESSION_BYTES_FILTER) {
                    p = netb_sb->ibm_netbios_bs_iacf;
		} else {
	            p = netb_sb->ibm_netbios_h_iacf;
		}
	    } else if (GETOBJ(int,1) == NETB_NOV_IAFILTER) {
		if (GETOBJ(int,2)) {
		    p = netb_sb->nov_netbios_b_iacf;
		} else {
		    p = netb_sb->nov_netbios_h_iacf;
		}
	    } else if (GETOBJ(int,1) == NETB_OAFILTER) {
		if (GETOBJ(int,2) == NETBIOS_BYTES_FILTER) {
	            p = netb_sb->ibm_netbios_b_oacf;
                } else if (GETOBJ(int,2) == NETBIOS_SESSION_BYTES_FILTER) {
                    p = netb_sb->ibm_netbios_bs_oacf;
		} else {
	            p = netb_sb->ibm_netbios_h_oacf;
		}
	    } else /* if (GETOBJ(int,1) == NETB_NOV_OAFILTER) */ {
		if (GETOBJ(int,2)) {
		    p = netb_sb->nov_netbios_b_oacf;
		} else {
		    p = netb_sb->nov_netbios_h_oacf;
		}
	    }
	    nv_write((int) p, "%s %s", csb->nv_command, p->name);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }


    switch (csb->which) {
      case NETBIOS_ACCESS_FILTER:
	/* Sorry - LLC2 commands only apply to TOKEN RING interfaces so far */
	if ((GETOBJ(int, 1) == NETB_IAFILTER) || (GETOBJ(int, 1) == NETB_OAFILTER)) {
	    if (!is_srb_hw(hwidb)) {
		printf("\nnetbios interface commands only allowed on Token Rings");
		return;
	    }
	} else /* Must be Novell... */ {
	    if (!RUNNING_IF(LINK_NOVELL, idb)) {
		printf("\nNovell routing must be enabled on the interface");
		return;
	    }
 	    if (reg_invoke_ipx_novell_fastswitch_bcast()) {
 		printf("\n%%Disable IPX broadcast fastswitching first");
  		return;
  	    }
	}
        if ((GETOBJ(int,1) == NETB_IAFILTER) ||
             ((GETOBJ(int,1) == NETB_OAFILTER))) {
            switch(GETOBJ(int,2)) {
                case NETBIOS_SESSION_BYTES_FILTER:
                case NETBIOS_BYTES_FILTER:
                    netbios_accQ = &netbios_bytes_accQ;
                    break;
                case NETBIOS_HOST_FILTER:
                    netbios_accQ = &netbios_host_accQ;
                    break;
                default:
                    printf("\nInvalid netbios filter");
            }
        }   
        else {
            isbytes = GETOBJ(int,2);	/* "bytes" = TRUE; "host" = FALSE */
            if (isbytes) {
                netbios_accQ = &netbios_bytes_accQ;
            } else {
                netbios_accQ = &netbios_host_accQ;
            }
        }
	if ((netblist = find_namedthing(netbios_accQ, GETOBJ(string,1))) == NULL) {
	    printf("\nUnknown netbios filter - %s", GETOBJ(string,1));
	    return;
	}

	if (GETOBJ(int,1) == NETB_IAFILTER) {
            if (GETOBJ(int,2) == NETBIOS_SESSION_BYTES_FILTER) {
                netb_sb->ibm_netbios_bs_iacf = ((csb->sense) ? netblist : NULL);
            } else if (GETOBJ(int,2) == NETBIOS_BYTES_FILTER) {
		netb_sb->ibm_netbios_b_iacf = ((csb->sense) ? netblist : NULL);
	    } else {
		netb_sb->ibm_netbios_h_iacf = ((csb->sense) ? netblist : NULL);
	    }
	} else if (GETOBJ(int,1) == NETB_NOV_IAFILTER) {
	    if (isbytes) {
		netb_sb->nov_netbios_b_iacf = ((csb->sense) ? netblist : NULL);
	    } else {
		netb_sb->nov_netbios_h_iacf = ((csb->sense) ? netblist : NULL);
	    }
	} else if (GETOBJ(int,1) == NETB_OAFILTER) {
            if (GETOBJ(int,2) == NETBIOS_SESSION_BYTES_FILTER) {
                netb_sb->ibm_netbios_bs_oacf = ((csb->sense) ? netblist : NULL);
            } else if (GETOBJ(int,2) == NETBIOS_BYTES_FILTER) {
		netb_sb->ibm_netbios_b_oacf = ((csb->sense) ? netblist : NULL);
	    } else {
		netb_sb->ibm_netbios_h_oacf = ((csb->sense) ? netblist : NULL);
	    }
	} else /* if (GETOBJ(int,1) == NETB_NOV_OAFILTER) */ {
	    if (isbytes) {
		netb_sb->nov_netbios_b_oacf = ((csb->sense) ? netblist : NULL);
	    } else {
		netb_sb->nov_netbios_h_oacf = ((csb->sense) ? netblist : NULL);
	    }
	}
	(*tr_set_access_flags_p)(hwidb);
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/******************************************************************************
*
* Function:	expr_update_netbios_access_list()
*
* Description:
*	This function takes a NetBIOS access-list (either "host" or "bytes")
*	and updates all ExpressionTrees that references this access-list.
*	There are two modes of operation for this function:
*	    1. Deletion Mode - remove all references to this 
*	       'netbios_acclisttype' on all interfaces.
*	    2. Update Mode - update all references to this 'netbios_acclisttype'
*	on all interfaces.
*
*	Deletion Mode is invoked for the specific case where the person
*	configuring the cisco router entered:
*
*		no netbios access-list {host|bytes} <name>
*
*	for a NetBIOS access list that is also referenced in an Expression
*	on one or more interfaces.
*
*	Update Mode is invoked for the case where the person configuring
*	the router deleted an access-list (per the above command) and then
*	reentered a new access-list with the same name using the following:
*
*		netbios access-list {host|bytes} <name> <parameters>
*
*
* Call:		expr_update_netbios_access_list (netbios_list)
*
* Input:
*   Arguments -
*	netbios_list	pointer to the NetBIOS access list that is being
*			deleted or added.
*	operation	indicator of the operation being performed on this
*			NetBIOS access list
*			  EXPR_ADD_NETBIOS_LIST    .. adding a list
*			  EXPR_DELETE_NETBIOS_LIST .. deleting a list
*			
* Output:	
*   Return Code -
*	(None)
*
* Notes:
*   (1)	Handling null NetBIOS Access Lists
*	There was code added to expr_determine_symbol_value() to handle
*	the case where a NetBIOS access list in the ExpressionTree is
*	null.
*
*****************************************************************************/

void expr_update_netbios_access_list (netbios_acclisttype *netbios_list,
				 int operation)
{
    hwidbtype *idb;

    /*
    **  Examine each IDB, looking for access expressions on either
    **  the input or output side.  If either are found, walk down the
    **  ExpressionTree and update any pointers to netbios_acclisttype
    **  matching the specified name.
    */
    FOR_ALL_HWIDBS(idb) {
	if (idb->access_expression_in)
	    expr_update_expression_tree(idb->access_expression_in,
					netbios_list, operation);

	if (idb->access_expression_out)
	    expr_update_expression_tree(idb->access_expression_out,
					netbios_list, operation);
    }
}

/*
 * netbios_acl_host
 * Execute a netbios access list for a host
 */

static boolean netbios_acl_host (uchar *dgstart, ulong dgsize, int riflen,
				 expr_tree_entry_t *symbol_node,
				 boolean *ignored)
{
    int	netbios_offset;
    sap_hdr *llc1;
    netbios_header *netbios_hdr;
    uchar netbios_dest_mask;

    /*
     *  First, check to see if the NetBIOS Access List has been
     *  deleted from beneath the access expression.  If it has,
     *  let these packets through unconditionally.
     */
    if (symbol_node->access.netbios.access_list == 0) {
	*ignored = TRUE;
	return(TRUE);
    }
    netbios_offset = (TRING_ENCAPBYTES + riflen + ET_SAP_LEN);

    /*
     *  If this is not a NetBIOS packet with a name in it, then let it thru.
     *  N.B. This logic must match the same NetBIOS packet examination
     *	     logic that exists in netbios_acfcheck().
     */
     llc1 = (sap_hdr *)((uint) dgstart + TRING_ENCAPBYTES + riflen);
     netbios_hdr = (netbios_header *) (dgstart + netbios_offset);
     if ((((llc1->control & ~LLC1_P)  != LLC1_UI) 		||
	  (llc1->dsap                != SAP_NETBIOS))		||
	 (netbios_hdr->delimiter[0] != NETBIOS_DELIMITER_0) 	||
	 (netbios_hdr->delimiter[1] != NETBIOS_DELIMITER_1)) {
	 *ignored = TRUE;
	 return(TRUE);
     }
    netbios_dest_mask = (netb_acc_filter_name_recognized ?
                         NETBIOS_DEST_MASK_2 : NETBIOS_DEST_MASK_1);
    if (((netbios_hdr->command & ~(NETBIOS_SRC_MASK)) !=  NETBIOS_SRC_VALUE) &&
	((netbios_hdr->command & ~(netbios_dest_mask)) != NETBIOS_DEST_VALUE)){
	*ignored = TRUE;
	return(TRUE);
    }
    return(netbios_acfcheck((dgstart + netbios_offset),
			    (dgsize  - netbios_offset),
			    NULL,
			    FALSE,
			    symbol_node->access.netbios.access_list,
			    TRUE,
			    NETBIOS_TOKEN_LLC2_CHECK));
}

/*
 * netbios_acl_bytes
 * Execute a netbios access list for bytes.
 */

static boolean netbios_acl_bytes (uchar *dgstart, ulong dgsize, int riflen,
				  expr_tree_entry_t *symbol_node,
				  boolean *ignored)
{
    int	netbios_offset;
    sap_hdr *llc1;
    netbios_header *netbios_hdr;

    /*
     *  First, check to see if the NetBIOS Access List has been
     *  deleted from beneath the access expression.  If it has,
     *  let these packets through unconditionally.
     */
    if (symbol_node->access.netbios.access_list == 0) {
	*ignored = TRUE;
	return(TRUE);
    }

    netbios_offset = (TRING_ENCAPBYTES + riflen + ET_SAP_LEN);

    /*
     *  If this is not a NetBIOS packet, then let it through.
     *
     *  N.B. This logic must match the same NetBIOS packet examination
     *	     logic that exists in netbios_acfcheck().
     */
    llc1 = (sap_hdr *)((uint) dgstart + TRING_ENCAPBYTES + riflen);
    netbios_hdr = (netbios_header *) (dgstart + netbios_offset);
    if ((((llc1->control & ~LLC1_P)  != LLC1_UI) 		||
	 (llc1->dsap                != SAP_NETBIOS))		||
	(netbios_hdr->delimiter[0] != NETBIOS_DELIMITER_0) 	||
	(netbios_hdr->delimiter[1] != NETBIOS_DELIMITER_1)) {
	*ignored = TRUE;
	return(TRUE);
    }
    return(netbios_acfcheck((dgstart + netbios_offset),
			    (dgsize  - netbios_offset),
			    NULL,
			    FALSE,
			    symbol_node->access.netbios.access_list,
			    FALSE,
			    NETBIOS_TOKEN_LLC2_CHECK));
}

/*
 * netbios_acl_get
 * Given an access expression code and an identifier, return a pointer to
 * a named NetBios access list.  Used by SERVICE_NETBIOS_ACL_GET
 */

static netbios_acclisttype *netbios_acl_get (int typecode, char *identifier)
{
    queuetype *queue;

    switch (typecode) {
      case EXPR_TREE_NETBIOS_HOST:
	queue = &netbios_host_accQ;
	break;
      case EXPR_TREE_NETBIOS_BYTES:
	queue = &netbios_bytes_accQ;
	break;
	break;
      default:
	queue = NULL;
	break;
    }
    if (queue)
	return(find_namedthing(queue, identifier));
    return(NULL);
}

/*
 * netbios_acl_check
 * Given an access expression code and a slew of arguments, see if we
 * should permit/deny this Netbios packet.  Used by SERVICE_NETBIOS_ACL_CHECK
 */

static boolean netbios_acl_check (int typecode,
				  uchar *dgstart, ulong dgsize, int riflen,
				  expr_tree_entry_t *symbol_node,
				  boolean *ignored)
{
    switch (typecode) {
      case EXPR_TREE_NETBIOS_HOST:
	return(netbios_acl_host(dgstart, dgsize, riflen,
				symbol_node, ignored));
	break;
      case EXPR_TREE_NETBIOS_BYTES:
	return(netbios_acl_bytes(dgstart, dgsize, riflen,
				 symbol_node, ignored));
	break;
      default:
	return(TRUE);
	break;
    }
    return(TRUE);
}

/*
 * netbios_sb_ibm_iacf_is_enabled() 
 *   Returns TRUE if _any_ IBM NetBios input access filter is enabled
 *
 */

static 
boolean netbios_sb_ibm_iacf_is_enabled (idbtype *swidb)
{
    netbios_sb_t  *sb;

    sb = netbios_get_sb(swidb);
    if (sb) {
        return(sb->ibm_netbios_h_iacf || 
               sb->ibm_netbios_b_iacf || 
               sb->ibm_netbios_bs_iacf);
    } else {
        return(FALSE);
    }
}


/*
 * netbios_sb_ibm_oacf_is_enabled() 
 *   Returns TRUE if _any_ NetBios output access filter is enabled
 *
 */
static 
boolean netbios_sb_ibm_oacf_is_enabled (idbtype *swidb)
{
    netbios_sb_t  *sb;

    sb = netbios_get_sb(swidb);
    if (sb) {
        return(sb->ibm_netbios_h_oacf || 
               sb->ibm_netbios_b_oacf || 
               sb->ibm_netbios_bs_oacf);
    } else {
        return(FALSE);
    }
}



/*
 * netbios_acl_init
 * Initialize netbios access list data structures
 */

static void netbios_acl_init (subsystype *subsys)
{
    queue_init(&netbios_host_accQ, 0);
    queue_init(&netbios_bytes_accQ, 0);
    netb_acc_filter_name_recognized = FALSE;

    /*
     * Register some functions
     */
    reg_add_netbios_acl_get(netbios_acl_get, "netbios_acl_get");
    reg_add_netbios_acl_check(netbios_acl_check, "netbios_acl_check");

    netbios_sb_ibm_iacf_is_enabled_p = & netbios_sb_ibm_iacf_is_enabled;
    netbios_sb_ibm_oacf_is_enabled_p = & netbios_sb_ibm_oacf_is_enabled;

}
