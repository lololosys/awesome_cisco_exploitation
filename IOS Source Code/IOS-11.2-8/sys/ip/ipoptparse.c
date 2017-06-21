/* $Id: ipoptparse.c,v 3.4.20.8 1996/08/07 09:00:17 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipoptparse.c,v $
 *------------------------------------------------------------------
 * ipoptparse.c -- IP options support parsing for ping command
 *
 * 02-May-1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipoptparse.c,v $
 * Revision 3.4.20.8  1996/08/07  09:00:17  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.4.20.7  1996/07/12  02:01:14  widmer
 * CSCdi52582:  parser should remove the trailing spaces in the filename
 * Branch: California_branch
 *
 * Revision 3.4.20.6  1996/06/29  00:11:28  snyder
 * CSCdi61694:  change some char * to char [] in ip
 * Branch: California_branch
 *     save 116 bytes
 *
 * Revision 3.4.20.5  1996/06/28  23:17:48  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.20.4  1996/06/06  04:25:26  widmer
 * CSCdi59599:  Add printf style formatting to parse_line
 * Branch: California_branch
 *
 * Revision 3.4.20.3  1996/05/21  09:51:32  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4.20.2  1996/05/11  02:30:15  tylin
 * CSCdi56413:  Multicast ping should have a ttl option
 *              Added for extended command only.
 * Branch: California_branch
 *
 * Revision 3.4.20.1  1996/03/18  20:17:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.2  1996/03/07  09:46:47  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.14.1  1996/02/20  01:00:15  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/01  06:04:32  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:29:50  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:35:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/25  08:32:23  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.2  1995/06/15  15:18:38  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  20:59:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "../if/network.h"
#include "ip.h"
#include "ttysrv.h"
#include "config.h"
#include "parser.h"
#include "ipoptions.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

/*
 * Definitions and command tables
 */

#define MAXLEN 80

#define GIPOPT_LOOSE 0
#define GIPOPT_STRICT (GIPOPT_LOOSE+1)
#define GIPOPT_RECORD (GIPOPT_STRICT+1)
#define GIPOPT_TIMESTAMP (GIPOPT_RECORD+1)
#define GIPOPT_VERBOSE (GIPOPT_TIMESTAMP+1)
#define GIPOPT_HELP  (GIPOPT_VERBOSE+1)
#define NGIPOPTCMDS  (GIPOPT_HELP+1)

static char *const gipopt_cmds[NGIPOPTCMDS] = {
"loose", "strict", "record", "timestamp", "verbose", "?"
};

static const ushort gipopt_flags[NGIPOPTCMDS] = {
CMD_PRV, CMD_PRV, CMD_PRV, CMD_PRV, CMD_PRV, CMD_INV+CMD_PRV
};

static char *const gipopt_help[NGIPOPTCMDS] = {
"Loose <hop1> <hop2> ...   Loose source route",
"Strict <hop1> <hop2> ...  Strict source route",
"Record <nhops>            Record route",
"Timestamp <nhops>         Record Timestamps",
"Verbose                   Toggle verbose reporting",
NULL
};

const char noroom[] = "%% No room for that option\n";
static const char onesource[] = "%% Only one source route option allowed\n";


/* The below parse chain is used by ping and trace for
 * interactive parsing of source routes. They aren't linked
 * anywhere, they just exist.
 */

#undef  ALTERNATE
#define ALTERNATE       no_alt
#include        "interact_source_route.h"


void get_ping_options (
    char *buff,
    ushort *pattern,
    ipoptiontype **options,
    ipoptiontype *ipoption,
    int *tos,
    boolean *verbose,
    boolean *nofrag,
    ipaddrtype *source,
    ipaddrtype destination,
    int *addrinc,
    idbtype **broad_idb,
    boolean *validate,
    boolean *rotate_pattern,
    ushort *ttl)
{
    boolean parsed;
    idbtype *idb;

    /*
     * Special ping options?
     */
    printf("\nExtended commands [n]: ");
    if (rdtty(buff, MAXLEN) && toupper(*buff) == 'Y') {

	/*
	 * Get interface for mutlicast destination. Packet will be sent out
	 * single interface.
	 */
	if (IPMULTICAST(destination)) {
	    while (TRUE) {
		printf("\nInterface [All]: ");
		if (rdtty(buff, MAXLEN)) {
		    if (!*buff) break;
		    parse_interface(buff, broad_idb);
		    if (*broad_idb) break;
		    printf("\n%% Invalid interface\n");
		} else {
		    break;
		}
	    }

	    /*
	     *  Get ttl.
	     */
	    *ttl = get_num_in_range("Time to live", 10, TTL_1, TTL_255, *ttl);
	}

	while (TRUE) {

	    if (!*broad_idb && IPMULTICAST(destination)) break;

	    if (IPMULTICAST(destination)) {
		printf("Source address: ");
	    } else {
		printf("Source address or interface: ");
	    }
	    parsed = rdtty(buff, MAXLEN);
	    if (!parsed)
		break;   /* could have lost connection */
	    if (parsed){
		if (!null(buff)){
		    /*
		     * An address or interface could be specified. First check
		     * if interface is specified or else
		     * check that the specified address is one of ours 
		     */
		    parse_interface(buff, &idb);
		    if (idb) {
			if (idb->ip_enabled || idb->ip_address)
			    *source = idb->ip_address;
			else
			    parsed = FALSE;
		    }
		    else if (parse_hostname(buff, source)) {
			FOR_ALL_SWIDBS(idb){
			    parsed = ip_ifaddress(idb, *source);
			    if (parsed)
				break;
			}	
		    } else
			parsed = FALSE;
		}
		if (!parsed) {
                    *source = 0;
		    printf("\n%% Invalid source\n");
		    continue;
		}
	    }
	    break;
	}

	printf("\nType of service [0]: ");
	if (rdtty(buff, MAXLEN) && !null(buff)) {
	    *tos = parse_unsigned(buff, &parsed);
	    if (!parsed)
	      *tos = TOS_PREC_ROUTINE;
	}

	*nofrag = yes_or_no("Set DF bit in IP header", FALSE, TRUE);
	*validate = yes_or_no("Validate reply data", FALSE, TRUE);

	parsed = TRUE;
	do {
	    printf("\nData pattern [0x%-04x]: ", *pattern);
	    if (rdtty(buff, MAXLEN) && !null(buff)) {
		if (buff[0] == 'r' || buff[0] == 'R') {
		    *rotate_pattern = TRUE;
		    parsed = TRUE;
		} else {
		    *pattern = parse_unsigned(buff, &parsed);
		    if (!parsed) {
			printf("\nInvalid pattern, try again.");
		    }
		}
	    }
	} while ((parsed==FALSE)&&((stdio->statbits&CARDROP)==0));
	
	*options = malloc(sizeof(ipoptiontype));
	if (*options == NULL)
	  printf(nomemory);
	else {
	    (void) get_options(ipoption, *options, buff, verbose,
			       destination);  
	    
	    /*
	     * The user may have said let's do options but then did not
	     * actually select any. Return the allocated memory.
	     */
	    if (ipoption->paddedlength == 0 && *options) {
		free(*options);
		*options = NULL;
	    }
	}

#ifdef	PING_INCREMENT
	/*
	 * This is a dangerous command, don't ship images to customers with
	 * this option enabled, but it is a GREAT way to stress-test the
	 * route cache code. :-)
	 */
	printf("\nDestination address increment [0]: ");
	if (rdtty(buff, MAXLEN) && !null(buff)) {
	    *addrinc = parse_unsigned(buff, &parsed);
	    if (!parsed)
	       *addrinc = 0;
	}
#endif

    }
    printc('\n');
}

/*
 * parse_ipoptions
 * Parse the IP option keywords of the ping options subcommand
 */

int parse_ipoptions (char *buff)
{
    int index, matches;

    if (!null(buff)) {
	matches = nmatch(buff,gipopt_cmds,gipopt_flags,NGIPOPTCMDS,&index);
	if (matches > 1) {
	    printf("%% Ambiguous - \"%s\"",buff);
	    return(-1);
	}
	if (matches == 0) {
	    printf("%% No such option - \"%s\"",buff);
	    return(-1);
	}
    } else
	return(-1);
    if (index == GIPOPT_HELP) {
	help_command(gipopt_help, gipopt_flags, NGIPOPTCMDS);
	printc('\n');
    	return(-1);
    }
    return(index);
}

/*
 * gipopt_prompt
 * Print out letters corresponding to selected options
 */

void gipopt_prompt (boolean verbose, ulong flags)
{
    printc('[');
    if ((flags == 0) && !verbose)
	printf("none");
    if (flags & PAKOPT_HASSSR)
	printc('S');
    if (flags & PAKOPT_HASLSR)
	printc('L');
    if (flags & PAKOPT_HASRRT)
	printc('R');
    if (flags & PAKOPT_HASTSTMP)
	printc('T');
    if (verbose)
	printc('V');
    printf("]: ");
}

/*
 *   get_options
 *      This routine will collect the ip options (if any)
 *      Note the obscure use of 'one_option'. This routine builds
 *      each option in 'one_option' before accumulating them in
 *      all_options.
 */

ulong get_options (
    ipoptiontype *all_options,	/* accumulate options here */
    ipoptiontype *one_option,	/* get each option in here */
    char *buff,			/* input buffer */
    boolean *verbose,
    ipaddrtype destination)
{
    ulong          flags = 0L;
    int            result;
#ifdef ICMPDESIGNEROPT
    boolean 	   parsed;
#endif

    all_options->length = 0;
    all_options->paddedlength = 0;
    /* Room for options. Reserve room for security if necessary */
    if (ip_secopt_bytes) {
	ip_add_bso_opt(all_options);
    }

    while (TRUE) {
        printf("Loose, Strict, Record, Timestamp, Verbose");
	gipopt_prompt(*verbose, flags);
	if (!rdtty(buff, MAXLEN) || null(buff))
	    break;  /* done with parsing options */

	result = parse_ipoptions(buff);
	switch (result) {
	    case GIPOPT_VERBOSE:
	        *verbose = (*verbose ? FALSE : TRUE);
		break;

	    case GIPOPT_RECORD:
	        if (optionbytesleft(MAXIPHEADERBYTES, all_options) < 7) {
		    printf(noroom);
		    break;
		}
		printf("Number of hops [ %d ]: ",
			    btol(optionbytesleft(MAXIPHEADERBYTES,
						 all_options) -
				 IPOPT_ROUTEHEADERSIZE));

		(void) rdtty(buff, MAXLEN);
		one_option->ipopts[0] = IPOPT_RRT;
		if (!parse_ipoptrecord(one_option, buff,
				       optionbytesleft(MAXIPHEADERBYTES,
						       all_options))) {
		    printf("Invalid Number of Hops\n");
		    break;
		}
		if (!add_ipoption(all_options, one_option)) {
		  printf(noroom);
		  break;
		}
		flags |= PAKOPT_HASRRT;
		*verbose = TRUE;
		break;

	    case GIPOPT_LOOSE:
		if (optionbytesleft(MAXIPHEADERBYTES, all_options) < 7) {
		    printf(noroom);
		    break;
		}
 		if (flags & (PAKOPT_HASLSR|PAKOPT_HASSSR)) {
 		    printf(onesource);
 		    break;
 		}
		one_option->ipopts[0] = IPOPT_LSR;
		if (!parse_line(one_option, &pname(interact_source_route),
				PARSEBUF, "Source route: ")) {
		    printf("Can't parse source route\n");
		    break;
		}
		if (!add_ipoption(all_options, one_option)) {
		    printf(noroom);
		    break;
		}
		flags |= PAKOPT_HASLSR;
		*verbose = TRUE;
		break;

	    case GIPOPT_STRICT:
		if (optionbytesleft(MAXIPHEADERBYTES, all_options) < 7) {
		    printf(noroom);
		    break;
		}
 		if (flags & (PAKOPT_HASLSR|PAKOPT_HASSSR)) {
 		    printf(onesource);
 		    break;
 		}
		one_option->ipopts[0] = IPOPT_SSR;
		if (!parse_line(one_option, &pname(interact_source_route),
				PARSEBUF, "Source route: ")) {
		    printf("Can't parse source route\n");
		    break;
		}
		if (!add_ipoption(all_options, one_option)) {
		    printf(noroom);
		    break;
		}
		flags |= PAKOPT_HASSSR;
		*verbose = TRUE;
		break;

	    case GIPOPT_TIMESTAMP:
		if (optionbytesleft(MAXIPHEADERBYTES, all_options) < 8) {
		    printf(noroom);
		    break;
		}
	        printf("\nNumber of timestamps [ %d ]: ",
		      btol(optionbytesleft(MAXIPHEADERBYTES,all_options)-4));

		if (!rdtty(buff,MAXLEN))
		    break;
		one_option->ipopts[0] = IPOPT_TSTMP;
		if (!parse_ipopttstmp(one_option, buff,
			   optionbytesleft(MAXIPHEADERBYTES, all_options))) {
		    printf("Invalid number of hops\n");
		    break;
		}
		if (!add_ipoption(all_options,one_option)) {
		    printf (noroom);
		    break;
		}
		flags |= PAKOPT_HASTSTMP;

		*verbose = TRUE;
		break;
	  } /* end of switch */
      } /* end of while */

#ifdef ICMPDESIGNEROPT
      printf("\nArbitrary option string: ");
      if (rdtty(buff, MAXLEN) && !null(buff)) {
	  char *p = buff;
	  int j;

	  for (i=0;
	       i<optionbytesleft(size-sizeof(icmptype),all_options);
	       i++) {
		j = parse_unsigned(p, &parsed);
		if (parsed)
		    all_options->ipopts[all_options->length++] = j;
		else
		    break;
		p = deblank(p);
		while (!term_char(*p))
		    p++;
	  }
	  for (i = all_options->length; i & 3; i++)
	      all_options->ipopts[i] = PADDING;
	  all_options->paddedlength = i;
      }
#endif  /* ICMPDESINEROPT */
    return(flags);
} /* end of get_options */
