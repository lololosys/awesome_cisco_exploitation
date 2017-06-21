/* $Id: ncia_debug.c,v 3.1.2.8 1996/07/26 06:23:58 schiang Exp $
 * $Source: /release/112/cvs/Xsys/cls/ncia_debug.c,v $
 *------------------------------------------------------------------
 * ncia_debug.c - NCIA Debugging routines
 *
 * March 1996, Steve Chiang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ncia_debug.c,v $
 * Revision 3.1.2.8  1996/07/26  06:23:58  schiang
 * CSCdi64000:  remove unused variable ncia_cisco_debug
 * Branch: California_branch
 *
 * Revision 3.1.2.7  1996/07/25  09:42:32  snyder
 * CSCdi63981:  eliminate unused code, fix bugs, make a common routine
 *              1. create a routine call generic_debug_all and
 *                 make most debugs use it
 *              2. eliminate if (*_debug_inited) return, most never set
 *              the var
 *                 to TRUE making the code useless.
 *              3. declare some constant arrays const
 *              4. fix bugs found along the way
 *              5. save 2768 bytes from image, 660 out of data section
 * Branch: California_branch
 *
 * Revision 3.1.2.6  1996/07/03  06:34:34  jolee
 * CSCdi61974:  Fix NCIA crash if unconfig a client in active open
 * processing
 * Branch: California_branch
 *
 * Revision 3.1.2.5  1996/06/18  19:01:37  schiang
 * CSCdi60706:  move extern in .c to .h
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/06/12  06:51:48  schiang
 * CSCdi60121:  ncia keepalive cant be turned through configuration
 * clean up code so that no line exceeds 80 characters.
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/06/01  21:49:23  jolee
 * CSCdi59245:  Added clear ncia client registered & show ncia client
 * saplist
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/28  22:33:48  schiang
 * CSCdi58852:  router crashes when ncia server is un-configured
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/17  10:43:34  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.5  1996/05/14  04:31:54  jolee
 * CSCdi57570:  Added clear client support, removed backup mac suppport
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.4  1996/05/03  22:06:25  schiang
 * CSCdi56629:  ncia server code needs to clean up
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.1  1996/04/05  06:05:44  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.1  1996/03/29  07:17:02  schiang
 * CSCdi53012:  add placeholder for NCIA files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
#include "master.h"
#include "sys_registry.h"
#include "../cls/dlc_registry.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "ieee.h"
#include "address.h" /* Defines hwaddrtype for clsi_msg.h    */
#include "../cls/clsi_msg.h"
#include "../cls/dlc.h"

#include "../cls/ncia.h"
#include "../cls/ncia_client.h"
#include "../cls/ncia_server_fsm.h"
#include "../cls/ncia_debug.h"
#include "../cls/ncia_debug_flags.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "ncia_debug_flags.h"

static char ncia_type_string[NCIA_TYPE_STRING_SIZE];
/*
 * File scope function prototypes 
 */
static char *ncia_get_ncia_type_string(ulong type);
static char *ncia_get_client_ncia_type_string(ushort type);
static void ncia_debug_all(boolean sense);
static void ncia_parse_debug_client(parseinfo *csb);
static void ncia_client_debug_cmd(boolean msg, nciapeertype *ncia_client, 
                                  ushort type, boolean sense);
static void ncia_parse_debug_circuit(parseinfo *csb);
static void ncia_circuit_debug_cmd(boolean msg, ulong type, boolean sense);
static void ncia_server_debug_cmd(boolean msg, boolean sense);
static void ncia_show_circuit_debug(void);
static void ncia_show_client_debug(void);
static void ncia_show_server_debug(void);
static void ncia_show_debug(void);
static void ncia_show_server(ncia_servertype *ncia_server);
static void ncia_show_client(nciapeertype *ncia_client);
static void ncia_show_client_saplist (nciapeertype *ncia_client);
static void ncia_show_circuit(NciaCepT *ncia_circuit);
static void ncia_parse_show_circuit(parseinfo *csb);
static void ncia_parse_show_server(parseinfo *csb);
static void ncia_parse_show_client(parseinfo *csb);
static void ncia_parse_show_client_saplist (parseinfo *csb);
static void ncia_clear_circuits(parseinfo *csb);
static void ncia_clear_clients(parseinfo *csb);
static void ncia_clear_the_client(nciapeertype *ncia_client, boolean relflag);
static void ncia_clear_client_registered (parseinfo *csb);



/*
 * ---------------------------------------------------------------
 * Debug Functions
 * ---------------------------------------------------------------
 */

void ncia_debug_init (void)
{

    ncia_client_debug  = FALSE;
    ncia_server_debug  = FALSE;
    ncia_circuit_debug = FALSE;

    /* Register for "debug all" and "show debug" events. */
    reg_add_debug_all(ncia_debug_all, "ncia_debug_all");
    reg_add_debug_show(ncia_show_debug, "ncia_show_debug");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when NCIA is initialized, we want to turn on
     * all the NCIA debugging right now.
     */
    ncia_debug_all(debug_all_p());
}



/*
 * ncia_debug_all()
 */
static
void ncia_debug_all (boolean sense)
{
    ncia_server_debug_cmd(FALSE, sense);
    ncia_client_debug_cmd(FALSE, NULL, NciaClientDebugALL, sense);
    ncia_circuit_debug_cmd(FALSE, NciaCircuitDebugALL, sense);
}



void debug_ncia (parseinfo *csb) 
{
    switch (GETOBJ(int,1)) {
      case NCIA_DEBUG_SERVER:
        ncia_server_debug_cmd(TRUE, csb->sense);
        break;

      case NCIA_DEBUG_CLIENT:
        ncia_parse_debug_client(csb);
        break;

      case NCIA_DEBUG_CIRCUIT:
        ncia_parse_debug_circuit(csb);
        break;

      case NCIA_DEBUG_ALL:
        ncia_server_debug_cmd(TRUE, csb->sense);
        ncia_client_debug_cmd(TRUE, NULL, NciaClientDebugALL, csb->sense);
        ncia_circuit_debug_cmd(TRUE, NciaCircuitDebugALL, csb->sense);
        break;

      default:
        bad_parser_subcommand(csb, csb->which);
        break;
    }
}



/**********************************************************************
 *
 * Client debug commands.
 *
 *********************************************************************/

/*
 * ncia_parse_debug_client()
 */
static
void ncia_parse_debug_client (parseinfo *csb)
{
    ipaddrtype     client_ipaddr = (ipaddrtype) GETOBJ(paddr,1)->ip_addr;
    nciapeertype   *ncia_client = NULL;
 
    if (((ulong) client_ipaddr != NCIA_DEBUG_ALL_CLIENTS) &&
        !(ncia_client = ncia_verify_client((ipaddrtype) client_ipaddr))) {
        printf("\nNCIA: Error, no client is known by IP addr %i", 
               client_ipaddr);
        return;
    }
 
    if ((ushort) client_ipaddr == NCIA_DEBUG_ALL_CLIENTS) {
        printf("\nNCIA: Debug client for all clients");
        ncia_client = NULL;
      }
 
    switch (GETOBJ(int,2)) {
      case NCIA_DEBUG_CLIENT_ALL:
        ncia_client_debug_cmd(TRUE, ncia_client, NciaClientDebugALL, 
                              csb->sense);
        break;
 
      case NCIA_DEBUG_CLIENT_MSGS:
        ncia_client_debug_cmd(TRUE, ncia_client, NciaClientDebugMSGS, 
                              csb->sense);
        break;
 
      case NCIA_DEBUG_CLIENT_EVENT:
        ncia_client_debug_cmd(TRUE, ncia_client, NciaClientDebugEVENT, 
                              csb->sense);
        break;
 
      case NCIA_DEBUG_CLIENT_ERROR:
        ncia_client_debug_cmd(TRUE, ncia_client, NciaClientDebugERROR, 
                              csb->sense);
        break;
 
      default:
        printf("\nNCIA: Illegal debug client type");
        break;
    }
}


 
/*
 * ncia_client_debug_cmd()
 *
 * If client is NULL, indicates debug scope is "all".
 */
static
void ncia_client_debug_cmd (boolean msg, nciapeertype *ncia_client, ushort type,
                            boolean sense)
{
    if (ncia_client) {
        if (sense)
            NciaSetClientDebug(ncia_client, type);
        else
            NciaUnsetClientDebug(ncia_client, type);

        printf ("\nNCIA: Client debugging for %s (ip_addr: %i) is %s",
                ncia_get_client_ncia_type_string(type), ncia_client->ipaddr,
                ncia_client->NciaFlags & type ? "on" : "off");
    } else {
        ncia_servertype *ncia_server;

        if (sense)
            ncia_client_debug |= type;
        else
            ncia_client_debug &= ~type;

        ForEachNciaServer(ncia_group, ncia_server)
            ForEachNciaClient(ncia_server, ncia_client)
                if (sense)
                    NciaSetClientDebug(ncia_client, type);
                else
                    NciaUnsetClientDebug(ncia_client, type);
        if (msg)
            printf("\nNCIA: Client debugging for %s is %s",
                   ncia_get_client_ncia_type_string(type), 
                   (ncia_client_debug & type) ? "on" : "off");
    }
}




/**********************************************************************
 *
 * Circuit debug commands.
 *
 *********************************************************************/
 
/*
 * ncia_parse_debug_circuit()
 */
static
void ncia_parse_debug_circuit (parseinfo *csb)
{
    switch (GETOBJ(int,2)) {
      case NCIA_DEBUG_CIRCUIT_ALL:
        ncia_circuit_debug_cmd(TRUE, NciaCircuitDebugALL, csb->sense);
        break;

      case NCIA_DEBUG_CIRCUIT_FLOW:
        ncia_circuit_debug_cmd(TRUE, NciaCircuitDebugFLOW, csb->sense);
        break;

      case NCIA_DEBUG_CIRCUIT_EVENT:
        ncia_circuit_debug_cmd(TRUE, NciaCircuitDebugEVENT, csb->sense);
        break;

      case NCIA_DEBUG_CIRCUIT_STATE:
        ncia_circuit_debug_cmd(TRUE, NciaCircuitDebugSTATE, csb->sense);
        break;

      case NCIA_DEBUG_CIRCUIT_ERROR:
        ncia_circuit_debug_cmd(TRUE, NciaCircuitDebugERROR, csb->sense);
        break;

      default:
        printf("\nNCIA: Illegal debug circuit type");
        break;
    }
}


static
char *ncia_get_client_ncia_type_string (ushort type)
{
    memset(&ncia_type_string, 0, NCIA_TYPE_STRING_SIZE);
 
    if (type & NciaClientDebugMSGS)
         sprintf(ncia_type_string, "Msgs, ");
    if (type & NciaClientDebugEVENT)
         sprintf(ncia_type_string, "%sEvent, ", ncia_type_string);
    if (type & NciaClientDebugERROR)
         sprintf(ncia_type_string, "%sError ", ncia_type_string);
    return(&ncia_type_string[0]);
}


static
char *ncia_get_ncia_type_string (ulong type)
{
    memset(&ncia_type_string, 0, NCIA_TYPE_STRING_SIZE);

    if (type & NciaCircuitDebugFLOW)
         sprintf(ncia_type_string, "Flow ");
    if (type & NciaCircuitDebugEVENT)
         sprintf(ncia_type_string, "%sEvent ", ncia_type_string);
    if (type & NciaCircuitDebugSTATE)
         sprintf(ncia_type_string, "%sState ", ncia_type_string);
    if (type & NciaCircuitDebugERROR)
         sprintf(ncia_type_string, "%sError ", ncia_type_string);
    return(&ncia_type_string[0]);
}



/*
 * ncia_circuit_debug_cmd()
 *
 * If circuit is NULL, indicates debug scope is "all".
 */
static
void ncia_circuit_debug_cmd (boolean msg, ulong type, boolean sense)
{
    if (sense)
        ncia_circuit_debug |= type;
    else
        ncia_circuit_debug &= ~type;

    if (msg)
        printf("\nNCIA: Circuit debugging for %sis %s", 
               ncia_get_ncia_type_string(type), 
               ncia_circuit_debug ? "on" : "off");
}



/**********************************************************************
 *
 * Server debug commands.
 *
 *********************************************************************/
 
/*
 * ncia_server_debug_cmd()
 *
 * If server is NULL, indicates debug scope is "all".
 */
static
void ncia_server_debug_cmd (boolean msg, boolean sense)
{
    ncia_server_debug = sense;

    if (msg)
        printf("\nNCIA: Server debugging is %s", sense ? "on" : "off");
}


 
static
void ncia_show_circuit_debug (void)
{
    if ((ncia_circuit_debug & NciaCircuitDebugALL) == NciaCircuitDebugALL) {
        printf("\nNCIA: Debug circuit all is on");
        return;
    }

    if (ncia_circuit_debug)
        printf("\nNCIA: Circuit debugging for %sis %s", 
            ncia_get_ncia_type_string(ncia_circuit_debug), 
            ncia_circuit_debug ? "on" : "off");
}

 

static
void ncia_show_client_debug (void)
{
    ncia_servertype *ncia_server;
    nciapeertype    *ncia_client;
 
    if (ncia_client_debug) {
        printf("\nNCIA: Debug client all is on");
        return;
    }
 
    ForEachNciaServer(ncia_group, ncia_server)
        ForEachNciaClient(ncia_server, ncia_client)  
            if (NciaClientDebugOn(ncia_client))
                printf("\nNCIA: Debug client all with ip address %i is on",
                        ncia_client->ipaddr);
}

 

static
void ncia_show_server_debug (void)
{
    if (ncia_server_debug)
        printf("\nNCIA: Debug server is on");
}
 


/*
 * ---------------------------------------------------------------
 * Show Functions
 * ---------------------------------------------------------------
 */
static
void ncia_show_debug (void)
{
    /* First show the flag that follow the standard scheme 
     * however, it can't provide enough info.  Therefore, we
     * skip it.
     * debug_show_flags(&(ncia_debug_arr[0]), "NCIA");
     */

    /* show each NCIA component's debug */
    ncia_show_client_debug();
    ncia_show_server_debug();
    ncia_show_circuit_debug();
}



static
void ncia_show_server (ncia_servertype *ncia_server)
{
    int          ii;
    int          mac_count = 0;
    nciapeertype *ncia_client;

    if (!(ncia_server->flags & NCIA_SERVER_CONFIGURED))
        return;

    printf("\nNCIA Server [%d]:", ncia_server->index);
    printf("\n    ip address: %i", ncia_server->ipaddr);
    printf("\n    Server Virtual MAC address: %e", ncia_server->server_mac);
    printf("\n    Starting Mac Address: %e", ncia_server->vmac_start);
    printf("\n    Mac Address Range: %d", ncia_server->vmac_range);
    printf("\n    Flags: 0x%x", ncia_server->flags);
    for (ii=0; ii<ncia_server->vmac_range; ii++) {
        if (ncia_server->vmac_list[ii] == NCIA_MAC_INDEX_USED)
            mac_count++;
    }
    printf("\n    number of mac address being used: %d", mac_count);
    ForEachNciaClient(ncia_server, ncia_client)  
        printf("\n    Client: %i", ncia_client->ipaddr);
}




static
void ncia_show_client (nciapeertype *ncia_client)
{
    NciaCepT *ncia_circuit;
    int      index = 1;

    /*
     * Don't display inactive client.
     */
    if (!(ncia_client->NciaFlags & NCIA_CLIENT_CONFIGED)   && 
        !(ncia_client->NciaFlags & NCIA_CLIENT_REGISTERED) &&
        !(ncia_client->NciaFlags & NCIA_CLIENT_ACTIVE))
        return;
    printf("\n%i\t%-5d %e 0x%4x\t",
                     ncia_client->ipaddr,
                     ncia_client->client_state,
                     ncia_client->Vmac,
                     ncia_client->NciaFlags);
    printf("%-2d    %-8d %-8d %-4d",
                     ncia_client->cap_xchg_ctl_vec.ctl_vec_len ? 
                     ncia_client->cap_xchg_ctl_vec.ctl_vec_len - 2: 0,
                     ncia_client->pkts_rx,
                     ncia_client->pkts_tx,
                     ncia_client->drops);

    ForEachNciaCircuit(ncia_client, ncia_circuit) {
        if (ncia_circuit->fState != nciast_closed)
            printf("\n    Circuit[%x]: %x", index, ncia_circuit);
        index++;
    }
}

static
void ncia_show_client_saplist (nciapeertype *ncia_client)
{
    short    i, num_sap;

    /*
     * Don't display inactive client.
     */
    if (!(ncia_client->NciaFlags & NCIA_CLIENT_CONFIGED)   && 
        !(ncia_client->NciaFlags & NCIA_CLIENT_REGISTERED) &&
        !(ncia_client->NciaFlags & NCIA_CLIENT_ACTIVE))
        return;
    num_sap = (ncia_client->cap_xchg_ctl_vec.ctl_vec_len) ? 
               ncia_client->cap_xchg_ctl_vec.ctl_vec_len - 2: 0;
    if (num_sap == 0)
        return;
    printf("\n%i\t  %-2d    ", ncia_client->ipaddr, num_sap);
    for (i = 0; i < num_sap; i++)
        printf("%2x", ncia_client->cap_xchg_ctl_vec.sap_list[i]); 
}




static
void ncia_show_circuit (NciaCepT *ncia_circuit)
{
    /* only display the circuit is in use */
    if (ncia_circuit->fState == nciast_closed)
        return;

    printf("\n%i\t%15s (Client)%8x %e\t%4x%4d%4d",
            ncia_circuit->fNciaClient->ipaddr, 
            ncia_print_circuit_state(ncia_circuit->fState), 
            ncia_circuit->fClientCktId, 
            ncia_circuit->fNciaClient->Vmac, 
            ncia_circuit->fClientSap, 
            ncia_circuit->fClientCurrentWindow, 
            ncia_circuit->fClientGrantPacket);

    printf("\n\t\t\t\t(Server)%8x %e\t%4x%4d%4d",
            ncia_circuit,
            ncia_circuit->fTargetAddr, 
            ncia_circuit->fTargetSap, 
            ncia_circuit->fCurrentWindow, 
            ncia_circuit->fGrantPacket);
}



void show_ncia (parseinfo *csb) 
{
    switch (GETOBJ(int,1)) {
      case NCIA_SHOW_SERVER:
        ncia_parse_show_server(csb);
        break;

      case NCIA_SHOW_CLIENT:
        ncia_parse_show_client(csb);
        break;

      case NCIA_SHOW_CIRCUIT:
        ncia_parse_show_circuit(csb);
        break;

      default:
        bad_parser_subcommand(csb, csb->which);
        break;
    }
}




/*
 * ncia_parse_show_circuit()
 */
static
void ncia_parse_show_circuit (parseinfo *csb)
{
    NciaCepT        *ncia_circuit;
    ncia_servertype *ncia_server;
    nciapeertype    *ncia_client;

    ncia_circuit = (NciaCepT *) GETOBJ(int,2);
    if ((word) ncia_circuit != NCIA_SHOW_ALL_CIRCUITS) {
        if (ncia_verify_circuit((ulong) ncia_circuit)) {
            printf("\nIP\t\t\tState   \tID\t Mac\t\tSAP CW  GP");
            ncia_show_circuit(ncia_circuit);
        } else
            printf("\nNCIA: Error, no circuit is known by ID 0x%x", 
                   (word) ncia_circuit);
        return;
    }
 
    printf("\nIP\t\tState\t\t\tID\t Mac\t\tSAP CW  GP");
    ForEachNciaServer(ncia_group, ncia_server)
        ForEachNciaClient(ncia_server, ncia_client)  
            ForEachNciaCircuit(ncia_client, ncia_circuit)
                ncia_show_circuit(ncia_circuit);
}


/*
 * ncia_parse_show_server()
 */
static
void ncia_parse_show_server (parseinfo *csb)
{
    ncia_servertype *ncia_server = NULL;
    int             server_index;

    server_index = GETOBJ(int,2);
    if (server_index != NCIA_DEBUG_ALL_SERVERS) {
        ncia_server = ncia_get_server_by_index(server_index);
        if (ncia_server)
            ncia_show_server(ncia_server);
        else
            printf("\nNCIA: Error, no server is known by index %d", 
                   server_index);

        return;
    }
 
    ForEachNciaServer(ncia_group, ncia_server)
        ncia_show_server(ncia_server);
}


/*
 * ncia_parse_show_client()
 */
void ncia_parse_show_client (parseinfo *csb)
{
    ipaddrtype      client_ipaddr;
    ncia_servertype *ncia_server;
    nciapeertype    *ncia_client;
    
    
    if (!GETOBJ(int, 2)) {     /* not show client saplist */
        client_ipaddr = (ipaddrtype) GETOBJ(paddr,1)->ip_addr;
        if ((ulong) client_ipaddr != NCIA_SHOW_ALL_CLIENTS) {
            if ((ncia_client = ncia_verify_client((ipaddrtype)client_ipaddr))) {
                printf(
         "\nIP\t\tState MacAddr\t     Flags    NumSap    PktRxd   PktTxd Drop");
                ncia_show_client(ncia_client);
	    }
            else
                printf("\nNCIA: Error, no client is known by IP addr  %i", 
                       client_ipaddr);
        }
        else {
            printf(
         "\nIP\t\tState MacAddr\t     Flags    NumSap    PktRxd   PktTxd Drop");
            ForEachNciaServer(ncia_group, ncia_server) 
                ForEachNciaClient(ncia_server, ncia_client)
                    ncia_show_client(ncia_client);
	}
    }
    else 
        ncia_parse_show_client_saplist(csb);
}

/*
 * ncia_parse_show_client_saplist()
 */
static void ncia_parse_show_client_saplist (parseinfo *csb)
{
    ipaddrtype      client_ipaddr;
    ncia_servertype *ncia_server;
    nciapeertype    *ncia_client;
 
    client_ipaddr = (ipaddrtype) GETOBJ(paddr,1)->ip_addr;
    if ((ulong) client_ipaddr != NCIA_SHOW_ALL_CLIENTS) {
        if ((ncia_client = ncia_verify_client((ipaddrtype) client_ipaddr))) {
            printf("\nIP\t\tNumSap  Saplist"); 
            ncia_show_client_saplist(ncia_client);
	}
        else
            printf("\nNCIA: Error, no client is known by IP addr  %i",
                   client_ipaddr);
    }
    else {
        printf("\nIP\t\tNumSap  Saplist"); 

        ForEachNciaServer(ncia_group, ncia_server) 
            ForEachNciaClient(ncia_server, ncia_client)
                ncia_show_client_saplist(ncia_client);
    }
}

/* 
 * ncia_clear_the_client
 */
static void ncia_clear_the_client(nciapeertype *ncia_client, boolean regflag)
{ 
    NciaCepT        *ncia_circuit;

    if (ncia_client->NciaFlags & NCIA_CLIENT_ACTIVE) {
        ForEachNciaCircuit(ncia_client, ncia_circuit)
            ncia_server_fsm(ncia_circuit, nciama_admin, nciami_admin_stop);
        ncia_client_fsm(ncia_client, CLOSE_CONN);
        /* let CLOSE_CON take care of client gracefully shutdown */
    }
    if (regflag)
        ncia_client->NciaFlags &= ~NCIA_CLIENT_REGISTERED;
}

/*
 * ncia_clear_client_registered()
 */
static void ncia_clear_client_registered (parseinfo *csb)
{
    ipaddrtype      client_ipaddr;
    ncia_servertype *ncia_server;
    nciapeertype    *ncia_client;
 
    client_ipaddr = (ipaddrtype) GETOBJ(paddr,1)->ip_addr;
    if ((ulong) client_ipaddr != NCIA_CLEAR_ALL_CLIENTS) {
        if ((ncia_client = ncia_verify_client((ipaddrtype) client_ipaddr))) {

            ncia_clear_the_client(ncia_client, TRUE);
	}
        else
            printf("\nNCIA: Error, no client is known by IP addr  %i", 
                   client_ipaddr);
    }
    else {
        ForEachNciaServer(ncia_group, ncia_server) 
            ForEachNciaClient(ncia_server, ncia_client)
                ncia_clear_the_client(ncia_client, TRUE);
    }
}

/*
 * clear client command
 */
static void ncia_clear_clients(parseinfo *csb)
{
    ncia_servertype *ncia_server;
    nciapeertype    *ncia_client;
    ipaddrtype      client_ipaddr;

    if (!GETOBJ(int, 2)) {     /* not clear client registered */
        client_ipaddr = (ipaddrtype) GETOBJ(paddr, 1)->ip_addr;
        if ((ulong) client_ipaddr != NCIA_CLEAR_ALL_CLIENTS) {
            ncia_client = NULL;
            if ((ncia_client = ncia_verify_client((ipaddrtype) client_ipaddr)))
                ncia_clear_the_client(ncia_client, FALSE);
            else
                printf("\nNCIA: Error, no client is known by IP addr  %i", 
                       client_ipaddr);
        }
        else {
            ForEachNciaServer(ncia_group, ncia_server) 
                ForEachNciaClient(ncia_server, ncia_client) 
                    ncia_clear_the_client(ncia_client, FALSE);
        }
    }
    else 
        ncia_clear_client_registered(csb);

}


/*
 * clear circuit command
 */
static
void ncia_clear_circuits (parseinfo *csb)
{
    NciaCepT        *ncia_circuit;
    ncia_servertype *ncia_server;
    nciapeertype    *ncia_client;
 
    if (GETOBJ(int, 2) == NCIA_CLEAR_ALL_CIRCUITS)
        /* clear all circuits */
        ForEachNciaServer(ncia_group, ncia_server) {
            ForEachNciaClient(ncia_server, ncia_client) { 
                ForEachNciaCircuit(ncia_client, ncia_circuit)
                    ncia_server_fsm(ncia_circuit, nciama_admin, 
                                    nciami_admin_stop);
        }
    }
    else {
        ncia_circuit = (NciaCepT *) GETOBJ(int, 2);
        if (ncia_verify_circuit((ulong) ncia_circuit))
            ncia_server_fsm(ncia_circuit, nciama_admin, nciami_admin_stop);
        else
            buginf("\nInvalid circuit ID: 0x%x", ncia_circuit);
    }
}
 


void clear_ncia (parseinfo *csb) 
{
    switch (GETOBJ(int,1)) {
      case NCIA_CLEAR_CIRCUIT:
        ncia_clear_circuits(csb);
        break;

      case NCIA_CLEAR_CLIENT:
        ncia_clear_clients(csb);
        break;

      default:
        bad_parser_subcommand(csb, csb->which);
        break;
    }
}





static void ncia_stop_server (ncia_servertype *ncia_server)
{
    nciapeertype *ncia_client;

    ncia_server->flags |= NCIA_SERVER_STOP;

    ForEachNciaClient(ncia_server, ncia_client)
        ncia_clear_the_client(ncia_client, FALSE);
}



static void ncia_start_server (ncia_servertype *ncia_server)
{
    if (ncia_server->flags & NCIA_SERVER_STOP)
        ncia_server->flags &= ~NCIA_SERVER_STOP;
    else
        printf("\nNCIA: Server %d is running already!", ncia_server->index);
}



void ncia_exec_command (parseinfo *csb)
{
    ncia_servertype *ncia_server;
    int              server_num;

    if (!ncia_running) {
        printf("\nNCIA: Server is not configured.");
        return;
    }

    server_num = GETOBJ(int,1);

    ncia_server = ncia_get_server_by_index(server_num);

    if (!ncia_server) {
        printf("\nNCIA: Server is not configured.");
        return;
    }

    if (!(ncia_server->flags & NCIA_SERVER_CONFIGURED)) {
        printf("\nNCIA: Server is not configured.");
        return;
    }

    switch (csb->which) {
    case EXEC_NCIA_STOP:
         ncia_stop_server(ncia_server);
         break;
    case EXEC_NCIA_START:
         ncia_start_server(ncia_server);
         break;
    default:
         bad_parser_subcommand(csb, csb->which);
         break;
    }
}
