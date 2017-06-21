/* $Id: macip_parse.c,v 3.4.18.3 1996/07/10 22:19:30 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/macip_parse.c,v $
 *------------------------------------------------------------------
 * macip_parse.c    - parse statements beginning with "appletalk macip".
 *
 * September 1991, Russ Tremain, Tremain Laboratories, under contract
 *  for cisco Systems, Inc.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: macip_parse.c,v $
 * Revision 3.4.18.3  1996/07/10  22:19:30  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.4.18.2  1996/03/23  01:28:32  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.4.18.1  1996/03/18  18:53:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/07  08:29:54  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  00:18:11  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/07  16:10:03  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.3  1996/02/01  05:59:25  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1995/11/17  08:45:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:51:10  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:10:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include <interface_private.h>
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "../ip/ip_registry.h"
#include "atalk_private.h"
#include "../atalk/atip_debug.h"
#include "macip_util.h"
#include "macip_db.h"
#include "macip_fsm.h"
#include "macip_server.h"
#include "macip_parse.h"
#include "parser_defs_atalk.h"
#include "at_registry.h"

/* definitions for at_err(): */
#define DEBUGSWITCH (at_debug(appletalk_debug, ATALK_NULL_MASK))

#define MAXLINE 128


/*************************  PRIVATE FUNCTIONS  ************************/

/*
 * zone_clause -
 *	
 *	zone_clause  --> 'zone' '<TEXT>' EOL
 *	NOTE:  do special mapping of input for non-printable chars.
 *	
 */
static boolean
zone_clause (parseinfo *csb, symbol *zname)
{
    char *buff = string_getnext();
    *zname = NILSYM;    /* result */

    atalk_hex_filter(GETOBJ(string,1), buff, ZONELEN);
    buff[ZONELEN] = '\0';   /* positive termination */

    /* save as symbol: */
    *zname = SY_APPEND(buff);

    return (TRUE);
}

/*
 * macip_server -
 *	
 *	server      --> 'server'  iaddr zone_clause
 *	
 */
static boolean
macip_server (parseinfo *csb)
{
#define SERVER_USAGE\
    "\nUsage:  'macip' 'server'  <ipaddr> 'zone' <server_zone>"

    ipaddrtype addr;
    ushort sid;
    symbol zname;

    /* Get IP address and zone name */
    addr = GETOBJ(paddr,1)->ip_addr;

    if (!zone_clause(csb, &zname)) {
        printf("%s", SERVER_USAGE);
        return (FALSE);
    }
    
    if (mdb_app_server(!csb->sense, &sid, addr, zname)) {
	if (at_debug(atalkmacip_debug, ATALK_NULL_MASK))
            buginf("\nAT: macip_server, append succeeded, sid=%d, %i, "
		   "zone='%s'\n", sid, addr, SY_STRING(zname));
    } else {
	if (at_debug(atalkmacip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: server append failed, %i, zone='%s'\n",
		   addr, SY_STRING(zname));
        return (FALSE);
    }

    return (TRUE);
}

/*
 * macip_resource -
 *	
 *	resource    --> iaddr ipaddr? zone_clause?
 *	
 *	finish parsing dynamic/static statement, store result.
 *	
 */
static boolean
macip_resource (parseinfo *csb, boolean isstatic)
{
    ipaddrtype	ll=0, uu=0;
    symbol	zname;
    ushort	sid;
    server_rec	*ss;

    /* Get addresses */
    uu = ll = GETOBJ(paddr,1)->ip_addr;
    if (GETOBJ(paddr,2)->type == ADDR_IP) {
	uu = GETOBJ(paddr,2)->ip_addr;
    }

    /* Get zone */
    if (!zone_clause(csb, &zname)) {
        printf("\nMacIP %s %i:  must specify zone",
	       isstatic?"static":"dynamic", ll);
        return (FALSE);
    }

    /* if no server for zone... */
    if (!mdb_serverbyzone(&ss, zname)) {
	printf("\n%s: No MacIP server for zone, cannot add client range",
		SY_STRING(zname));
        return (FALSE);
    }
    sid = ss->serverid;
    
    /* add addresses to server_resources relation: */
    mdb_app_resource(!csb->sense,sid,ll,uu,isstatic);

    return (TRUE);
}

/*
 * macip_dynamic -
 *	
 *	dynamic     --> 'dynamic'  ipaddr ipaddr? zone_clause
 *	
 */
static boolean
macip_dynamic (parseinfo *csb)
{
#define DYNAMIC_USAGE\
    "\nUsage:  'macip' 'dynamic' <ip> [<ip>] 'zone' <server_zone>"

    if (!macip_resource(csb,FALSE)) {
        printf("%s", DYNAMIC_USAGE);
        return (FALSE);
    }

    return (TRUE);
}

/*
 * macip_static -
 *	
 *	static      --> 'static'  iaddr_range+ zone_clause?
 *	
 */
static boolean
macip_static (parseinfo *csb)
{
#define STATIC_USAGE\
    "\nUsage:  'macip' 'static' <ip>[<ip>] 'zone' <server_zone>"

    if (!macip_resource(csb,TRUE)) {
        printf("%s", STATIC_USAGE);
        return (FALSE);
    }

    return (TRUE);
}

static ataddrtype cip1, cip2;
/*
 * add_client -
 *	add a range of MACIP clients for testing
 */
static void
add_client (parseinfo *csb)
{
    static ataddress src;
    extern void mdb_cli_add_test(ipaddrtype, ipaddrtype, ataddress *);

    if (csb == NULL)
	return;

    cip1 = GETOBJ(paddr,1)->ip_addr;
    cip2 = GETOBJ(paddr,2)->ip_addr;
    src.net  = GETOBJ(paddr,3)->apple_addr.net;
    src.node = GETOBJ(paddr,3)->apple_addr.host;
    src.spec = 2;

    mdb_cli_add_test(cip1, cip2, &src);
}

/*
 * del_client -
 *	delete a range of MACIP clients
 */
static void
del_client (parseinfo *csb)
{
    extern void mdb_cli_del_test(ipaddrtype, ipaddrtype);
    
    cip1 = GETOBJ(paddr,1)->ip_addr;
    cip2 = GETOBJ(paddr,2)->ip_addr;

    mdb_cli_del_test(cip1, cip2);
}

/*************************  EXPORTED FUNCTIONS  ***********************/


/*
 * macip_init -
 *	initialize macip data structures
 */
void
macip_init (void)
{
    if (macip_initialized)
	return;			/* already initialized */

    /* Requested aliased packets */
    reg_add_ip_alias(macip_enqueue, "macip_enqueue");
    reg_add_atalk_macip_add_client(add_client, "add_client");
    reg_add_atalk_macip_del_client(del_client, "del_client");

    /* init database: */
    if (!mdb_init()) {
        at_err(MODID,"macip_init:  database initialization failed!");
	return;
    }

    macip_gccontext = atmaint_AddGCContext(macip_empty_trash, 0);
    
    macip_initialized = TRUE;
}

/*
 * macip_parse -
 *	
 *	statement    --> 'no'? 'appletalk' 'macip'
 *	(server | dynamic | static)? '<EOF>'
 *	^
 *	you are here in the parse.
 *	
 *	"no appletalk macip" ==> shutdown
 *	
 *	
 */
void macip_command (parseinfo *csb)
{
   
    if (csb->nvgen) {
	if (macip_configured)
	    mdb_nv_macip(csb);
	return;
    }

    /* if not yet initialized... */
    if (!macip_initialized) {
        at_err(MODID,"macip_parse:  MACIP not initialized!");
        return;
    }

    switch (GETOBJ(int,1)) {
      case MACIP_DISABLE:
	/* "no appletalk macip" ==> SHUTDOWN */
	if (!csb->sense) {
	    macip_shutdown();
	}
	break;

      case MACIP_SERVER:
        (void) macip_server(csb);
	break;

      case MACIP_DYNAMIC:
        (void) macip_dynamic(csb);
	break;

      case MACIP_STATIC:
        (void) macip_static(csb);
	break;

      default:
	bad_parser_subcommand(csb, GETOBJ(int,1));
        break;
    }
}
