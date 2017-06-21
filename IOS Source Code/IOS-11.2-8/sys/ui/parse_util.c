/* $Id: parse_util.c,v 3.5.10.5 1996/08/07 09:03:25 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ui/parse_util.c,v $
 *------------------------------------------------------------------
 * parse_util.c -- useful little parse tables
 *
 * March 1990, John Bashinski
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parse_util.c,v $
 * Revision 3.5.10.5  1996/08/07  09:03:25  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.5.10.4  1996/07/15  01:41:25  nkwok
 * CSCdi60101:  dialer map uses negative XNS network number
 * Branch: California_branch
 * use %lu print format instead of %d for xns_addr.net(ulong)
 *
 * Revision 3.5.10.3  1996/06/12  19:23:19  pst
 * CSCdi60173:  Eliminate cisco definition of ntohl in favor of
 * swap_xxbit_word (final part)
 *
 * All good things must come to an end.  ntohl() and friends finally have
 * UNIX semantics in California releases and later.
 *
 * Branch: California_branch
 *
 * Revision 3.5.10.2  1996/05/21  10:07:49  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.10.1  1996/03/18  22:29:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.6  1996/03/16  07:45:57  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.14.5  1996/03/14  01:36:14  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.4.14.4  1996/03/13  02:04:09  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.14.3  1996/03/09  05:14:10  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.14.2  1996/03/07  10:58:45  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.14.1  1996/02/20  21:26:33  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/26  06:53:08  dmccowan
 * CSCdi45288:  need appn configuration changes to support multilink ppp
 * Add configuration option for APPN over a dialer interface
 *
 * Revision 3.4  1995/12/17  18:40:19  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/29  18:23:17  shankar
 * CSCdi41439:  ags crash during atm-dxi test. Part I of a two
 * stage commit
 *
 * Revision 3.2  1995/11/17  17:48:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:41:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:11:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ctype.h>
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "config.h"
#include "../parser/parser_actions.h"
#include "ttysrv.h"     /* needed for rdtty prototype */
#include "name.h"       /* needed for NAM_IP */
#include "../ip/ip.h"


/*
 * parse_proto_addr
 * Parse a protocol address type 
 */

boolean parse_proto_addr (parseinfo *csb, uint flag,
			  addrtype *outaddr,
			  addrtype *inaddr, uint link)
{
    hwidbtype *idb = hwidb_or_null(csb->interface);

    if (link == LINK_X25)
	return(FALSE);
    if ((link == LINK_CLNS) || (link == LINK_LLC2) || (link == LINK_RSRB) ||
		(link == LINK_STUN) || (link == LINK_DLSW)) {
	if (is_x25(idb)) {
	    printf("\nThis command is replaced by es-neighbor command");
	    return(FALSE);
	} else if (is_frame_relay(idb) || is_atm_dxi(idb)) {
	    outaddr->addr.fr_address = GETOBJ(int,1);
	    return(TRUE);
	}
	bcopy(inaddr, outaddr, sizeof(addrtype));
	return(TRUE);
    }
    if ((link == LINK_BRIDGE) && (is_frame_relay(idb) || is_atm_dxi(idb))) {
	outaddr->addr.fr_address = GETOBJ(int,1);
	return(TRUE);
    }
    if ((link == LINK_IP) || (link == LINK_COMPRESSED_TCP)) {
	if (outaddr->ip_addr)		/* compatibility hack */
	    return(TRUE);
    }
    bcopy(inaddr, outaddr, sizeof(addrtype));
    return(TRUE);
}


void print_proto_addr (idbtype *idb, addrtype *addr,
		       uint link, char *str)
{
    ulong apollo_host;

    str[0] = '\0';
    switch (link) {
      case LINK_AARP:
        sprintf(str, "aarp");
        break;
      case LINK_APOLLO:
	bcopy(&(addr->apollo_addr.host[2]), &apollo_host,
	      sizeof(apollo_host));
        sprintf(str, "apollo %x.%x", addr->apollo_addr.net, apollo_host);
	
        break;
      case LINK_APPLETALK:
        sprintf(str, "appletalk %d.%d", addr->apple_addr.net,
		addr->apple_addr.host);
        break;
      case LINK_ARP:
        sprintf(str, "arp");
        break;
      case LINK_BRIDGE:
	if ( is_x25(hwidb_or_null(idb)) )
        	sprintf(str, "bridge %s", addr->x121_addr);
	else
        	sprintf(str, "bridge");
        break;
      case LINK_CDP:
        sprintf(str, "cdp ");
        break;
      case LINK_CLNS:
	if ( is_frame_relay(hwidb_or_null(idb)) || 
				is_atm_dxi(hwidb_or_null(idb)))
	    sprintf(str, "clns");
	else
	    sprintf(str, "clns %n", addr->clns_addr);
        break;
      case LINK_CLNS_ALL_ES:
        sprintf(str, "clns_es %n", addr->clns_addr);
        break;
      case LINK_CLNS_ALL_IS:
        sprintf(str, "clns_is %n", addr->clns_addr);
        break;
      case LINK_COMPRESSED_TCP:
        sprintf(str, "compressedtcp %i", addr->ip_addr);
        break;
      case LINK_CMNS:
        sprintf(str, "cmns %n", addr->cmns_addr);
        break;
      case LINK_DECNET:
        sprintf(str, "decnet %v", swap_16bit_word(addr->decnet_addr));
        break;
      case LINK_DECNET_NODE:
        sprintf(str, "decnet_node");
        break;
      case LINK_DECNET_ROUTER_L1:
        sprintf(str, "decnet_router-L1");
        break;
      case LINK_DECNET_ROUTER_L2:
        sprintf(str, "decnet_router-L2");
        break;
      case LINK_DECNET_PRIME_ROUTER:
        sprintf(str, "decnet_prime_router");
        break;
      case LINK_IP:
        sprintf(str, "ip %i", addr->ip_addr);
        break;
      case LINK_NOVELL:
	sprintf(str, "ipx %x.%e", addr->nov_addr.net,
		addr->nov_addr.host);
        break;
      case LINK_PAD:
        sprintf(str, "pad %s", addr->x121_addr);
        break;
      case LINK_RSRB:
        sprintf(str, "rsrb");
        break;
      case LINK_DLSW:
        sprintf(str, "dlsw");
        break;
      case LINK_SNAPSHOT:
	sprintf(str, "snapshot %d", addr->snapshot_addr);
	break;
      case LINK_STUN:
        sprintf(str, "stun");
        break;
      case LINK_BSTUN:
        sprintf(str, "bstun");
        break;
      case LINK_VINES:
        sprintf(str, "vines %z", addr->vines_addr.net,
                addr->vines_addr.host);
        break;
      case LINK_X25:
        sprintf(str, "x25");
        break;
      case LINK_XNS:
        sprintf(str, "xns %lu.%e", addr->xns_addr.net,
		addr->xns_addr.host);
        break;
    case LINK_QLLC:
	sprintf(str, "qllc %e", addr->qllc_addr);
        break;
    case LINK_LLC2:
	if ( is_frame_relay(hwidb_or_null(idb)) ||
             is_ppp(hwidb_or_null(idb)))
	    sprintf(str, "llc2 ");
	break;
      default:
        sprintf(str, "UNKNOWN");
        break;
    }
}

const char *print_protocol (uint link_type)
{
    int i;

    if (link_names != NULL) {
	for (i=0; link_names[i].link_type != LINK_ILLEGAL; i++) {
	    if (link_type == link_names[i].link_type) {
		return(link_names[i].name);
	    }
	}
    }
    return("UNKNOWN");
}

boolean get_source_file (char *sfname, int len,
                         char *oper, boolean verbose)
{
    char *headptr,*tailptr;
    if (strlen(sfname) == 0) {  /* File name not specified */
        if (verbose) {
            printf("\nName of file to %s? ", oper);
            flush();
            rdtty(sfname, len);
        }
        else
            return (FALSE);
    }
    /* Take care of leading and trailing white space in the filename */
    tailptr = sfname;
    headptr = sfname;
    /* Skip any leading white space */
    while ((*tailptr == ' ') || (*tailptr == '\t'))
       tailptr++;
    /* Get rid of trailing white space */
    while ((*tailptr != 0) && (*tailptr != ' ') && (*tailptr != '\t'))
       *headptr++ = *tailptr++;
    *headptr = 0;
    /*
     * Now check, after stripping leading-trailing spaces, whether name has
     * been entered or not.
     */
    if (strlen(sfname) == 0) {
        printf("\n%% File name not specified - %s aborted.\n", oper);
        return (FALSE);
    }
    return (TRUE);
}

boolean get_host_address (addrtype *host, char *bp, int length)
{
    nametype *ptr;
    int i;

    printf("Address or name of remote host [%s]? ", name_addr2string(host));
    flush();
    rdtty(bp, length - 1);
    if (*bp) {				/* user entered something */
        if (!numeric_lookup(ADDR_IP, bp, host)) {
            /*
             * Entered value is not a numeric. See if it's
             * a host name by doing a DNS lookup.
             */
            ptr = name_lookup(bp, &i, NAM_IP);
            if (ptr)			/* name lookup succeeded. Get address */
                address_copy(host, &ptr->v.ip.adr[0]);
            else {
                printf("\n%% Invalid host address or name.\n");
                return (FALSE);
            }
        }
    }
    return (TRUE);
}

void parser_set_keyword (parseinfo *csb)
{
    if (csb->nvgen) {
	nv_write(*(boolean *)GETOBJ(int, 1), csb->nv_command);
	return;
    }
    *((boolean *)GETOBJ(int, 1)) = csb->sense;
}

/*
 * confirm
 * Print confirmation message
 * Return TRUE if confirmed with RETURN or 'y'.
 */

boolean confirm (char *s)
{
    char c;

    if (stdio->csb && (stdio->csb->flags & CONFIG_HTTP)) {
	return(TRUE);
    }

    LOG_SYNC_REPRINT_REG(stdio,"\n[confirm]");
    while (TRUE) {
	printf("%s[confirm]", s);
	c = ttyin(stdio);
	c &= 0177;
	switch (c) {
	  case 015:
	  case 012:
	  case 'Y':
	  case 'y':
	      LOG_SYNC_REPRINT_END(stdio);
	      return(TRUE);
	  case '?':
	      printf("\nPress RETURN to continue, anything else to abort\n");
	      break;
          default:
	      LOG_SYNC_REPRINT_END(stdio);
	      return(FALSE);
	}
    }
}

/*
 * doit
 * Returns TRUE if user typed Y, y, or RETURN, else provide
 * a message while skipping other characters.
 */
boolean doit (void)
{
    char c;

    c = ttyin(stdio);
    c &= 0x7F; 
    printc('\n');
    if ((c != 'Y') && (c != 'y') && (c != RETURN)) {
	printf("Skipping...");
	return(FALSE);
    }
    return(TRUE);
}

/*
 * yes_or_no
 * Print a prompt and wait for a yes or no answer.  The supplied default
 * answer is used if a single return character is typed.  If usedef is
 * false set then no default is used and the user must type yes or no.
 */

#define MAXBUF 100

boolean yes_or_no (const char *prompt, boolean def, boolean usedef)
{
    char *c, buffer[MAXBUF];

    while (!(stdio->statbits & CONTROLC)) {
	if (prompt)
	    printf("\n%s? [%s]: ", prompt, usedef ? def ? "yes" : "no" :
		   "yes/no");
	if (!rdtty(buffer, MAXBUF))
	    return(FALSE);
	c = deblank(buffer);
	if (usedef && null(c))
	    return(def);
	if (tolower(c[0]) == 'y')
	    return(TRUE);
	if (tolower(c[0]) == 'n')
	    return(FALSE);
	printf("\n%% Please answer 'yes' or 'no'.");
    }
    return(FALSE);
}

/*
 * yes_or_no2
 * Like yes_or_no, but forces an explicit answer.
 */

boolean yes_or_no2 (jmp_buf *catch, const char *prompt, boolean def,
		    boolean usedef)
{
    char *c, buffer[MAXBUF];

    while (TRUE) {
	if (prompt)
	    printf("\n%s? [%s]: ", prompt, usedef ? def ? "yes" : "no" :
		   "yes/no");
	if (!rdtty(buffer, MAXBUF))
	    longjmp(catch, 1);
	c = deblank(buffer);
	if (usedef && null(c))
	    return(def);
	if (tolower(c[0]) == 'y')
	    return(TRUE);
	if (tolower(c[0]) == 'n')
	    return(FALSE);
	printf("\n%% Please answer 'yes' or 'no'.");
    }
}

/*
 * get_num_in_range
 * Ask for a number, and do bounds checking on the answer.  There is a
 * default value that is used if the user simply types return.
 */

ulong get_num_in_range (const char *prompt, int base, ulong lower,
			ulong upper, ulong def)
{
    ulong result = def;
    int status;
    char buffer[MAXBUF], *c;
    boolean parsed;

    while (!(stdio->statbits & CONTROLC)) {
	if (prompt) {
	    if (base == 16)
		printf("\n%s [%x]: ", prompt, def);
	    else
		printf("\n%s [%u]: ", prompt, def);
	}
	status = rdtty(buffer, MAXBUF);
	if (!status) {
	    result = def;
	    break;
	}
	c = deblank(buffer);
	if (null(c)) {
	    result = def;
	    break;
	}
	if (base == 16)
	    result = parse_hex_long(buffer);
	else
	    result = parse_unsigned(buffer, &parsed); /* ??? */
	if (result >= lower && result <= upper)
	    break;
	if (base == 16)
	    printf("\n%% A hexadecimal number between %x and %x.",lower,upper);
	else 
	    printf("\n%% A decimal number between %u and %u.", lower, upper);
    }
    return(result);
}

/*
 * get_num_in_range2
 * Like get_num_in_range but forces and explicit answer
 */

ulong get_num_in_range2 (jmp_buf *catch, const char *prompt,int  base,
			 ulong lower, ulong upper, ulong def)
{
    ulong result;
    char buffer[MAXBUF], *c;
    boolean parsed;

    while (TRUE) {
	if (prompt) {
	    if (base == 16)
		printf("\n%s [%x]: ", prompt, def);
	    else
		printf("\n%s [%u]: ", prompt, def);
	}
	if (!rdtty(buffer, MAXBUF))
	    longjmp(catch, 1);
	c = deblank(buffer);
	if (null(c)) {
	    result = def;
	    break;
	}
	if (base == 16)
	    result = parse_hex_long(buffer);
	else
	    result = parse_unsigned(buffer, &parsed); /* ??? */
	if (result >= lower && result <= upper)
	    break;
	if (base == 16)
	    printf("\n%% A hexadecimal number between %x and %x.",lower,upper);
	else 
	    printf("\n%% A decimal number between %u and %u.", lower, upper);
    }
    return(result);
}

/*
 * read_string2
 * Read a string from the terminal,
 * Pressing RETURN would cause the default to be accepted
 */

char *read_string2 (jmp_buf *catch, char *buffer, const char *prompt,
		    char *defstring)
{
    char *cp;

    while (TRUE) {
	if (null(defstring))
	    printf("\n%s: ", prompt);
	else
	    printf("\n%s [%s]: ", prompt, defstring);
	if (!rdtty(buffer, MAXBUF))
	    longjmp(catch, 1);
	cp = deblank(buffer);
	if (null(cp)) {
	    if (!null(defstring))
		return(defstring);
	    printf("%% No defaulting allowed");
	} else {
	    if (*cp != '?')
		return(cp);
	    printf("Type a string, then press RETURN");
	}
    }
}
