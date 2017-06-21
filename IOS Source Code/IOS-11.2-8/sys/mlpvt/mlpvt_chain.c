/* $Id: mlpvt_chain.c,v 1.1.42.11 1996/09/15 18:09:57 perryl Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/mlpvt_chain.c,v $
 *------------------------------------------------------------------
 * mlpvt_chain.c -- MLPVT parser support
 *
 * January 1996, Ashwin Baindur
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mlpvt_chain.c,v $
 * Revision 1.1.42.11  1996/09/15  18:09:57  perryl
 * CSCdi68323:  SGBP member help text is misleading
 * Branch: California_branch
 *
 * Revision 1.1.42.10  1996/08/09  21:13:42  perryl
 * CSCdi65641:  SGBP offload server doesnt always win all the bundles
 * Branch: California_branch
 * For all platform, the default seed bid is now 50. The offload seed bid
 * is 50 plus the CPU calibrated value.
 * Remove helpout when bid requester send out a bid of zero.
 *
 * Revision 1.1.42.9  1996/07/25  22:33:28  perryl
 * CSCdi62937:  Multichassis Multilink PPP crash in TCP
 * Branch: California_branch
 * SGBP may get 2 or more message on a closing tcb. Keep
 * track of free status of closing tcb in mlpvt_process_tcp_message()
 * Do not free an already freed tcb.
 *
 * Revision 1.1.42.8  1996/05/21  09:57:07  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.1.42.7  1996/05/18  22:34:48  perryl
 * CSCdi57939:  MLPVT is a bad name for Stack Group Bidding Protocol
 * Branch: California_branch
 * Rename it to SGBP, Stack Group Bidding Protocol
 *
 * Revision 1.1.42.6  1996/05/07  23:34:37  perryl
 * CSCdi56945:  MLPVT seed-bid parameter can be simpler
 * Branch: California_branch
 *
 * Revision 1.1.42.5  1996/05/06  23:41:28  perryl
 * CSCdi56533:  Redundant MLPVT parameter, connect retry interval
 * Branch: California_branch
 *
 * Revision 1.1.42.4  1996/05/06  21:47:05  perryl
 * CSCdi56528:  MLPVT doesnt allow stack member to use more than 1 IP src
 * address.
 * Branch: California_branch
 * Add a secondary address list to the MLPVT member structure.
 * When a hello msg's source IP address different than the
 * configured member IP address, record it in the secondary
 * address list. When TCP pipe open request, it will than
 * able to recognize the address as the member's
 *
 * Revision 1.1.42.3  1996/05/01  00:27:04  syiu
 * CSCdi56248:  Multichassis MLP not starting L2F daemon automatically
 * Branch: California_branch
 *
 * Revision 1.1.42.2  1996/04/29  09:40:13  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 1.1.42.1  1996/04/27  06:34:23  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.13  1996/04/22  00:31:17  perryl
 * Branch: Dial1_branch
 * Added CHAP style authenication to MLPVT.
 * Used AAA to obtain password, eliminate "mlpvt password"
 * command.
 * Fixed misc bugs and enhanced debug messages.
 *
 * Revision 1.1.2.12  1996/04/17  03:32:29  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 1.1.2.11  1996/04/03  22:58:55  abaindur
 * Branch: Dial1_branch
 *  Enhance bid generation scheme for MLP bundle mastership. Provide
 * a means of controlling mastership based on either locality of a call
 * or load-sharing.
 *
 * Revision 1.1.2.10  1996/03/25  05:31:40  tkolar
 * Branch: Dial1_branch
 * Add an include that is now required by l2f.h
 *
 * Revision 1.1.2.9  1996/03/22  05:33:49  tkolar
 * Branch: Dial1_branch
 * Reshape the way MLPVT interacts with VPN.
 *
 * Revision 1.1.2.8  1996/03/21  23:49:20  tkolar
 * Branch: Dial1_branch
 * Add support for multiple incoming tunnels, fix a crash in the resend
 * code, and add the beginnings of true authentication.
 *
 * Revision 1.1.2.7  1996/03/16  16:43:59  syiu
 * Branch: Dial1_branch
 * Create/Delete L2F tunnels from MLPVT. Also move code into
 * L2F create/delete tunnel routines
 *
 * Revision 1.1.2.6  1996/03/09  07:18:06  syiu
 * Branch: Dial1_branch
 * Move virtual-template from mlpvt to mlp
 *
 * Revision 1.1.2.5  1996/03/05  02:46:55  agh
 * Branch: Dial1_branch
 * Much cleanup of unwanted code, also added comments.
 *
 * Revision 1.1.2.4  1996/03/02  02:57:20  abaindur
 * Branch: Dial1_branch
 *  Remove hooks that were added for multi-MLPVT groups per system.
 *
 * Revision 1.1.2.3  1996/03/01  22:58:04  abaindur
 * Branch: Dial1_branch
 *  Remove config knob for a tie-breaker metric used in case of equal
 *  MLPPP bundle mastership bids.
 *
 * Revision 1.1.2.2  1996/02/08  06:51:42  syiu
 * Branch: Dial1_branch
 * Tie together MLPVT/MLP/VPN registries, add vtemplate
 * support in MLPVT
 *
 * Revision 1.1.2.1  1996/01/26  20:28:38  agh
 * Branch: Dial1_branch
 * Initial Code Commit for MLPVT.
 *
 * Revision 1.1  1996/01/26  19:22:34  abaindur
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "globs.h"
#include "interface_private.h"
#include "interface.h"
#include "sys_registry.h"
#include "packet.h"
#include "address.h"
#include "name.h"
#include "ttysrv.h"
#include "connect.h"
#include "logger.h"
#include "login.h"
#include "config.h"
#include "file.h"
#include "subsys.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../os/list.h"
#include "../mlpvt/mlpvt.h"
#include "../util/md5.h"
#include "../vpn/l2f.h"
#include "../wan/ppp_registry.h"
#include "../ip/ip.h"

/*
 * Parse chains for MLPVT debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_mlpvt.h"
LINK_POINT(mlpvt_debug_commands, ALTERNATE);
#undef	ALTERNATE

#define ALTERNATE NONE
#include "cfg_mlpvtgroup.h"
LINK_POINT(mlpvtgroup_cfg_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for MLPVT show commands
 */
#define	ALTERNATE	NONE
#include "../mlpvt/exec_show_mlpvt.h"
LINK_POINT(mlpvt_show_commands, ALTERNATE);
#undef	ALTERNATE

static parser_extension_request mlpvt_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(mlpvt_debug_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(mlpvtgroup_cfg_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(mlpvt_show_commands) },
    { PARSE_LIST_END, NULL }
};

boolean sgbp_forward_straight_ppp;
ipaddrtype sgbp_source_ip;

extern void mlpvt_hello_timer_start(void);

/*
 * =================================================================
 * Name: mlpvt_get_groupref
 *
 * Description:
 *
 * Parameters:
 *
 * Returns:
 *
 * =================================================================
 */

static ulong
mlpvt_get_groupref (void)
{
    hwidbtype *idb;

    FOR_ALL_HWIDBS(idb) {
        if (!(idb->status & (IDB_ETHER|IDB_TR|IDB_FDDI)))
            continue;
        /*
         * Now grab the lowest 32 bits of the bia.
         */
        return(GETLONG(&idb->bia[2]));
    }
    return(0);
}

/*
 * =================================================================
 * Name: mlpvtgroup_nvgen
 *
 * Description:
 *
 * Parameters:
 *
 * Returns:
 *
 * =================================================================
 */

static void
mlpvtgroup_nvgen (void)
{
    mlpvtgroup_t *grp;
    mlpvtmember_t *memb;

    /*
     * Scan through all group configured and write all
     * valid groups to nvram.
     */
    grp = mlpvt_getgroup();
    
    if (grp != NULL) {
        /* mlpvt group <name> */
        nv_write(TRUE, "!\n%s %s",
                 "sgbp group",grp->name);

        /* mlpvt seed-bid <value> */
	switch (grp->seedbidtype) {
	case MLPVT_SEEDBID_OFFLOAD:
	    nv_write(TRUE,"sgbp seed-bid offload");
	    break;
	case MLPVT_SEEDBID_NUM:
	    nv_write(TRUE,"sgbp seed-bid  %d", grp->queryseedbid);
	    break;
	case MLPVT_SEEDBID_FORWARD_ONLY:
	    nv_write(TRUE, "sgbp seed-bid forward-only");
	    break;
	}
        /* Now add all the members */
        memb = mlpvt_getfirst_member(grp);
        while (memb != NULL) {
            /* mlpvt member <name> <IP_address> */
            nv_write((memb != NULL), "%s %s %s %i",
                     "sgbp",
                     "member", memb->name, memb->addr);
            memb = mlpvt_getnext_member(grp, memb);
        }	

        if (sgbp_forward_straight_ppp)
           nv_write(TRUE,"sgbp ppp-forward");

	if (sgbp_source_ip)
	    nv_write(TRUE,"sgbp source-ip %i", sgbp_source_ip);

    }
    return;
}

/*
 * =================================================================
 * Name: mlpvt_alloc_member
 *
 * Description: Create a member. If this is a dupicate (used in the Keepalive 
 *              Queue - !new - don't bother to put an id or secondary address
 *                      list
 *
 * Parameters:
 *
 * Returns:
 *
 * =================================================================
 */

mlpvtmember_t *
mlpvt_alloc_member (mlpvtgroup_t *grp, char *name, ipaddrtype addr, 
boolean new)
{
    mlpvtmember_t *memb;
    static int id = 0;
    
    memb = malloc(MLPVTMEMBER_SIZE);
    if (memb == NULL)
        return(NULL);

    memb->name = malloc(strlen(name) + 2);
    if (memb->name == NULL) {
        free(memb);
        return(NULL);
    }
    memb->status = MLPVTGROUP_MEMBER_IDLE;
    memb->mlpvtgrp = grp;
    strcpy(memb->name, name);
    memb->addr = addr;

    if (new ) {
        memb->ticks = 0;
        memb->retries = 0;
        memb->id = ++id;
        memb->secaddr = list_create(NULL, 0, "MLPVT sec addr list",
                                 LIST_FLAGS_AUTOMATIC);
     } else {
        memb->id = 0;
        memb->secaddr = 0;
     }
 
    return(memb);
}


/*
 * =================================================================
 * Name: mlpvt_alloc_group
 *
 * Description:
 *
 * Parameters:
 *
 * Returns:
 *
 * =================================================================
 */

static mlpvtgroup_t *
mlpvt_alloc_group (char *name)
{
    mlpvtgroup_t *grp;
    
    grp = malloc(MLPVTGROUP_SIZE);
    if (grp == NULL)
        return(NULL);

    grp->name = malloc(strlen(name) + 2);
    if (grp->name == NULL) {
        free(grp);
        return(NULL);
    }

    grp->members = list_create(NULL, 0, "MLPVT Members",
                               LIST_FLAGS_AUTOMATIC);
    if (grp->members == NULL) {
        free(grp->name);
        free(grp);
        return(NULL);
    }
    
    strcpy(grp->name, name);
    grp->active = TRUE;
    grp->status = MLPVTGROUP_STATUS_CREATED;
    grp->reference = mlpvt_get_groupref();
    grp->queryseedbid = MLPVTGROUP_SEED_BID;
    grp->seedbidtype = MLPVT_SEEDBID_DEFAULT;
    return(grp);
}

/*
 * =================================================================
 * Name: mlpvtgroup_add_commands
 *
 * Description:
 *
 * Parameters:
 *
 * Returns:
 *
 * =================================================================
 */

static void
mlpvtgroup_add_commands (parseinfo *csb)
{
    mlpvtgroup_t *grp;
    mlpvtmember_t *memb;
    char *name;
    ipaddrtype addr;
    nametype *nameptr;
    int dummy;

    /*
     * First search for any existing entry in mlpvtgroup_t.
     * If entry found, just return.
     */
    grp = mlpvt_getgroup();
    
    if (grp == NULL) {
        /*
         * Are the mlpvt processes running.
         */
        if ((mlpvt_getpackage_state()) == MLPVT_IDLE)
            mlpvt_process_init();
        
        /*
         * mlpvt group doesn't exist
         */
        switch (csb->which) {
        case MLPVTGROUP_NAME:

            /*
             * Start up L2F, if not already done so
             */
            reg_invoke_l2f_create_daemon();

            grp = mlpvt_alloc_group(GETOBJ(string,1));
            if (grp == NULL) {
                /* XXX - what to do here? */
                return;
            }
           
            if (strlen(grp->name)  > MLPVT_MAX_NAME) {
               printf("\nSGBP group name too large, max is %d",  
                       MLPVT_MAX_NAME);
               return;
            }

            if (mlpvt_addgroup(grp) == FALSE) {
                /* XXX - what to do here? */
                return;
            }

            break;
        default:
            printf("\nSGBP group not configured");
        }
        return;
    } else {
        /*
         * mlpvt group found.
         */
        switch (csb->which) {

        case MLPVTGROUP_PPP_FWD:
            sgbp_forward_straight_ppp = TRUE;
            return;

	case MLPVTGROUP_SOURCE_IP:
	    sgbp_source_ip = GETOBJ(paddr,1)->ip_addr;
	    return;

        case MLPVTGROUP_NAME:
	    printf("\nSGBP group '%s' already configured",
		   grp->name);
            return;
		
        case MLPVTGROUP_MEMBER:
            name = GETOBJ(string,1);
	    addr = 0;
 
	    /*
	     * If the user supplies an address use it, otherwise
	     * use DNS to find an address that match the member name
	     */
            if (GETOBJ(paddr,1)->length)
                addr = GETOBJ(paddr,1)->ip_addr;
            else {
                nameptr = (nametype *) name_lookup(name, &dummy, NAM_IP);
                if (nameptr && nameptr->v.ip.adr[0].length)
                    addr = nameptr->v.ip.adr[0].addr.ip_address;
                else {
                    printf("Unknown member name, %s\n",name);
                    return;
                }
            }

            memb = mlpvt_getmemberbyname(grp, name);
            if (memb == NULL)
                memb = mlpvt_getmemberbyaddr(grp, addr);
            if (memb != NULL) {
                printf("\nDuplicate SGBP group member entry. Ignoring...");
                return;
            }

	    /* Check if it is myself */
	    if (ip_ouraddress(addr)) {
		printf("\nCan't add itself to the stack group "); 
		return;
	    }

            /*
             * New member to be added.
             */
            memb = mlpvt_alloc_member(grp, name, addr, TRUE);
            if (memb == NULL)
                return;
            if (mlpvt_addmember(grp, memb) == FALSE) {
                /*
                 * Problem adding this member.  XXX - what to do?
                 */
                if (memb->name != NULL)
                    free(memb->name);
                free(memb);
            }
 
            /*
             * Announce the new member asap
             */
            mlpvt_hello_timer_start();
            return;

	case MLPVTGROUP_QUERYBID_DEFAULT:
            grp->queryseedbid = MLPVTGROUP_SEED_BID;
	    grp->seedbidtype = MLPVT_SEEDBID_DEFAULT;
            return;

	case MLPVTGROUP_QUERYBID_OFFLOAD:
            grp->queryseedbid = MLPVTGROUP_SEED_BID + MLPVTGROUP_CALIBRATE_BID;
	    grp->seedbidtype = MLPVT_SEEDBID_OFFLOAD;	
            return;	

	case MLPVTGROUP_QUERYBID_NUM:
            grp->queryseedbid = GETOBJ(int,1);
	    grp->seedbidtype = MLPVT_SEEDBID_NUM;	
            return;

        case MLPVTGROUP_QUERYBID_FORWARD_ONLY:
	    grp->queryseedbid = 0;
	    grp->seedbidtype = MLPVT_SEEDBID_FORWARD_ONLY;
	    return;

        default:
            printf("\nUnknown SGBP option (%d)", csb->which);
            return;
        }
    }
}

/*
 * =================================================================
 * Name: mlpvtgroup_remove_commands
 *
 * Description:
 *
 * Parameters:
 *
 * Returns:
 *
 * =================================================================
 */

static void
mlpvtgroup_remove_commands (parseinfo *csb)
{
    mlpvtgroup_t *grp;
    mlpvtmember_t *memb;
    
    /*
     * Deal with removing an entire mlpvt group later thru
     * a separate process.
     * Here, just mark mlpvtgroup as deleted. The mlpvtgroup
     * may be active and we cannot blow the data structures
     * away.
     */
    grp = mlpvt_getgroup();
    if (grp == NULL) {
        printf("\nSGBP group not found :-(");
        return;
    }

    switch (csb->which) {

    case MLPVTGROUP_PPP_FWD:
        sgbp_forward_straight_ppp = FALSE;
        return;

    case MLPVTGROUP_SOURCE_IP:
	sgbp_source_ip = 0;
	return;

    case MLPVTGROUP_NAME:
        /*
         * Should all members also be deleted here ? Or will
         * the mlpvt process take care of that ?
         */
        mlpvtgroup_delete(grp);
        return;
	    
    case MLPVTGROUP_MEMBER:
	memb = mlpvt_getmemberbyname(grp, GETOBJ(string, 1));
        if (memb != NULL) {
            mlpvtgroup_member_delete(grp, memb);
	}
        return;
					
	/*
	 * When you do a no sgbp seed-bid XXX, it goes back to 
	 * default setting.
	 */
    case MLPVTGROUP_QUERYBID_NUM:
    case MLPVTGROUP_QUERYBID_OFFLOAD:
    case MLPVTGROUP_QUERYBID_DEFAULT:
    case MLPVTGROUP_QUERYBID_FORWARD_ONLY:
        grp->queryseedbid = MLPVTGROUP_SEED_BID;
	grp->seedbidtype = MLPVT_SEEDBID_DEFAULT;
        return;
	
    default:
        printf("\nUnknown SGBP option (%d)", csb->which);
        return;
    }
}

/*
 * =================================================================
 * Name: mlpvtgroup_command
 *
 * Description:
 *
 * Parameters:
 *
 * Returns:
 *
 * =================================================================
 */

void
mlpvtgroup_command (parseinfo *csb)
{
    if (csb->nvgen) {
        mlpvtgroup_nvgen();
        return;
    }
    
    if (csb->sense) {
	/*
	 * Add mlpvtgroup command to list.
         */
        mlpvtgroup_add_commands(csb);
    } else {
	/*
	 * Remove mlpvtgroup from list.
         */
        mlpvtgroup_remove_commands(csb);
    }
}
/*
 * =================================================================
 * Name: mlpvt_show_group
 *
 * Description:
 *
 * Parameters:
 *
 * Returns:
 *
 * =================================================================
 */

static void
mlpvt_show_group (mlpvtgroup_t *grp)
{
    printf("\nGroup Name: %s Ref: %#x",
           grp->name, grp->reference);
}

static char *mlpvt_membstate_print (int state)
{
    switch (state) {
    case MLPVTGROUP_MEMBER_ILLEGAL:
	return ("illegal");
    case MLPVTGROUP_MEMBER_IDLE:
	return ("idle");
    case MLPVTGROUP_MEMBER_WAIT_INFO:
	return("wait_info");
    case MLPVTGROUP_MEMBER_ACTIVE:
	return("active");
    case MLPVTGROUP_MEMBER_AUTHOK:
	return("authOK");
    }
    return ("invalid");
}
/*
 * =================================================================
 * Name: mlpvt_show_member
 *
 * Description:
 *
 * Parameters:
 *
 * Returns:
 *
 * =================================================================
 */

static void
mlpvt_show_member (mlpvtgroup_t *grp, mlpvtmember_t *memb)
{ 
    ipaddrtype *ipaddr;
    list_element *el;

    printf("\n  Member Name: %s State: %s Id: %d",
           memb->name, mlpvt_membstate_print(memb->status), memb->id);
    printf("\n  Ref: %#x", memb->reference);
    printf("\n  Address: %i", memb->addr);

    FOR_ALL_DATA_IN_LIST(memb->secaddr, el, ipaddr) {
	printf("\n  Other Active Address: %i", *ipaddr);
    }
    printf("\n"); 
}

/*
 * =================================================================
 * Name: mlpvt_show
 *
 * Description:
 *
 * Parameters:
 *
 * Returns:
 *
 * =================================================================
 */

static void
mlpvt_show (void)
{
    mlpvtgroup_t *grp;
    mlpvtmember_t *memb;
    int cur_seed_bid;

    grp = mlpvt_getgroup();
    if (grp != NULL) {
        mlpvt_show_group(grp);
	cur_seed_bid = mlpvt_get_seed_bid(NULL);

	switch (mlpvt_get_bidtype(NULL)) {
	case MLPVT_SEEDBID_DEFAULT:
	    printf("\nSeed bid: default, %d, default seed bid setting\n",
		   cur_seed_bid);
	    break;
	case MLPVT_SEEDBID_OFFLOAD:
	    printf("\nSeed bid: offload, %d, offload server seed bid setting\n",
		   cur_seed_bid);
	    break;
	
	case MLPVT_SEEDBID_NUM:
	    printf("\nSeed bid: %d, configured seed bid value\nDefault is %d\n",
		   cur_seed_bid, MLPVTGROUP_SEED_BID);
	    break;

	case MLPVT_SEEDBID_FORWARD_ONLY:
	    printf("\nSeed bid: forward-only, %s\nDefault is %d\n",
		   "hangup if unable to forward", MLPVTGROUP_SEED_BID);
	    break;
	}

        memb = mlpvt_getfirst_member(grp);
        while (memb != NULL) {
            mlpvt_show_member(grp, memb);
            memb = mlpvt_getnext_member(grp, memb);
        }
    }
}

/*
 * =================================================================
 * Name: show_mlpvt
 *
 * Description:
 *
 * Parameters:
 *
 * Returns:
 *
 * =================================================================
 */

void
show_mlpvt (parseinfo *csb)
{
    boolean choice;

    choice = GETOBJ(int, 1);
    if (choice)
	mlpvt_show_query_states();
    else
        mlpvt_show();
}

/*
 * =================================================================
 * Name: mlpvt_parser_init
 *
 * Description:
 *
 * Parameters:
 *
 * Returns:
 *
 * =================================================================
 */

void
mlpvt_parser_init (void)
{
    sgbp_forward_straight_ppp = FALSE;
    parser_add_command_list(mlpvt_chain_init_table, "MLPVT");
}
