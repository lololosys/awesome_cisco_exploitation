/* $Id: dnsix_nat.c,v 3.8.18.3 1996/08/28 13:08:48 thille Exp $
 * $Source: /release/112/cvs/Xsys/servers/dnsix_nat.c,v $
 *------------------------------------------------------------------
 * dnsix_nat.c - Dnsix-NAT sussport
 *
 * Feb 1993, Phanindra Jujjavarapu
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dnsix_nat.c,v $
 * Revision 3.8.18.3  1996/08/28  13:08:48  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.18.2  1996/07/23  13:28:00  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.8.18.1  1996/03/18  21:46:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.2  1996/03/07  10:41:59  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  17:09:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/07  16:14:59  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.7  1996/02/01  06:08:46  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/24  20:19:49  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.5  1996/01/22  06:59:08  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1996/01/05  10:18:08  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.3  1995/11/21  21:24:15  snyder
 * CSCdi44272:  more spellink errors
 *              sendig -> sending
 *
 * Revision 3.2  1995/11/17  18:51:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:44:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "../os/old_timer_callbacks.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "config.h"
#include "parser.h"
#include "dmdp.h"
#include "dnsix_nat.h"
#include "dnsix_debug.h"
#include "parser_defs_dnsix.h"
#include "../os/clock.h"
#include "../ip/ip_registry.h"


/*
 * Global variables
 */
dnsixblocktype dnsix_nat_block;
static char dnsix_name[] = "Dnsix Timer";
static char test_dnsix[] = "Dnsix Tester";
extern dmdpblocktype dmdpblock;

/*
 * forward references
 */
/*
 * external references
 */

/*
 * dnsix_atm_init
 * init atm
 */
void dnsix_atm_init (void)
{
    int i;

    /*
     * Set up some debugging flags
     */
    dnsix_debug_init();

    dnsix_nat_block.serving = FALSE;
    dnsix_nat_block.source = 0;
    dnsix_nat_block.primary = 0;
    dnsix_nat_block.secondary = 0;
    dnsix_nat_block.dest = 0;
    for ( i = 0; i < NAT_REDIRECT_COUNT; i++) 
	dnsix_nat_block.auth_redi[i] = 0;
    dnsix_nat_block.transmit_count = NAT_DEFAULT_TRANSMIT_COUNT;
    dnsix_nat_block.current_pak = NULL;
    dnsix_nat_block.buffer_pak = NULL;
    dnsix_nat_block.state = 0;
    dnsix_nat_block.socket = NULL;
    dnsix_nat_block.redi_socket = NULL;
    dnsix_nat_block.atm_record_count = 0;
    dnsix_nat_block.atm_pak_count = 0;
    dnsix_nat_block.atm_redi_count = 0;
    dnsix_nat_block.test_timer = NULL;
    dnsix_nat_block.output_timer = NULL;
    dnsix_nat_block.test_time = 0;
}/* end of dnsix_atm_init */

/*
 * dnsix_test
 * test dnsix protocol
 */
void dnsix_test (uchar *ptr1, uchar *ptr2, uchar *ptr3)
{

    dnsix_nat_buginf("\nDNSIX-NAT: Sending the record");
    write_audit(NULL, DNSIX_SECERROR_SENDR);
    set_timer(dnsix_nat_block.test_timer, dnsix_nat_block.test_time * ONESEC);
} /* end of dnsix_test  */

/*
 * dnsix_output
 * start output
 * if necessary
 */
void dnsix_output(uchar *ptr1, uchar *ptr2, uchar *ptr3)
{
    paktype *pak;
    natinfotype *info;

    if ((dnsix_nat_block.state & NAT_CONNECTED) == 0)
	return;
    if (dnsix_nat_block.current_pak) {
	pak = dnsix_nat_block.current_pak;
	info = INFO_PTR(pak);
	if ((info->flag & ACK_WAITING) == 0)
	    nat_rec_send();
	return;
    }
    pak = dnsix_nat_block.buffer_pak;
    if (pak == NULL)
	return;
    info = INFO_PTR(pak);
    if ((info->length >= NAT_AUDIT_DATA_SIZE)
	|| (info->count >= dnsix_nat_block.transmit_count)) {
	dnsix_nat_block.current_pak = dnsix_nat_block.buffer_pak;
	dnsix_nat_block.buffer_pak = NULL;
	nat_rec_send();
    }
} /* end of dnsix_output */

/*
 * check_redi_list
 * check if the redirection
 * message is from authorized
 * NSCs.
 */
boolean check_redi_list (paktype *pak, char *data)
{
    iphdrtype *ip;
    int i;

    ip = (iphdrtype *)ipheadstart(pak);
    
    for ( i = 0; i < NAT_REDIRECT_COUNT; i++) {
	if (ip->srcadr == dnsix_nat_block.auth_redi[i])
	    return(TRUE);
    }
    /*
     * The NSC is not authorized. Send
     * an audit trail record with record
     * idendifier 223).
     */
    dnsix_audit_write(0, DNSIX_REDI_IGNORED, data, 
		      DNSIX_HDR_SIZE, ip->srcadr);
    return(FALSE);
}

/*
 * dnsix_rcv_nat
 * receive DNSIX audit messages
 * from port 195.
 */
void dnsix_rcv_nat (paktype *pack, uchar *data)
{	
    return;
}	
/*
 * dnsix_dmdp_ack
 * receive DMDP ack
 */
boolean dnsix_ack_nat (ulong handle, int retries, ipaddrtype faddress,
		       boolean flag)
{
    paktype *pak;
    natinfotype *info;

    if (dnsix_nat_block.current_pak == NULL)
	return(FALSE);

    if (flag) {
	/*
	 * positive notification, just
	 * return
	 */
	dnsix_nat_block.atm_pak_count++;
	datagram_done(dnsix_nat_block.current_pak);
	dnsix_nat_block.current_pak = NULL;
	if (dnsix_nat_block.buffer_pak)
	    set_timer(dnsix_nat_block.output_timer,OUTPUT_DELAY_TIME * ONESEC);
    } else {
	pak = dnsix_nat_block.current_pak;
	info =INFO_PTR(pak);
	switch (dnsix_nat_block.state) {
	  case NAT_PRIMARY:
	    if (dnsix_nat_block.secondary) {
		dnsix_nat_block.state = NAT_SECONDARY;
		dnsix_nat_block.dest = dnsix_nat_block.secondary;
	    } else
		dnsix_nat_block.state = NAT_FAILURE;
	    info->flag &= ~ACK_WAITING;
	    break;
	  case NAT_SECONDARY:
	    dnsix_nat_block.state = NAT_FAILURE;
	    info->flag &= ~ACK_WAITING;
	    printf("\nDNSIX-NAT: Both primary and secondary failed");
	    break;
	}
	if (dnsix_nat_block.state & NAT_CONNECTED)
	    set_timer(dnsix_nat_block.output_timer, OUTPUT_DELAY_TIME *ONESEC);
    }
    return(FALSE);
} /* end of dnsix_dmdp_ack */
	

/*
 * dnsix_rcv_redi
 * receive DNSIX redirect
 * messages.
 */
void dnsix_rcv_redi (paktype *pak, dnsixtype *dnsix)
{
    natinfotype *info;

    if (dnsix_nat_block.serving == FALSE)
	return;
    if (dnsix->record_id > DNSIX_REDI_FAILURE)
	return;
    dnsix_nat_block.atm_redi_count++;
    if ((check_redi_list(pak, (char *)dnsix)) == FALSE)
	return;
    switch (dnsix->record_id) {
      case DNSIX_REDI_IMMEDIATE:
	dnsix_nat_block.dest = dnsix->source_address;
	dnsix_nat_block.primary = dnsix->source_address;
	if (dnsix_nat_block.state == NAT_FAILURE) {
	    dnsix_nat_block.state = NAT_PRIMARY;
	} else {
	    dmdp_dump_pak(dnsix_nat_block.socket, dnsix_nat_block.current_pak);
	    if (dnsix_nat_block.current_pak) {
		info = INFO_PTR(dnsix_nat_block.current_pak);
		info->flag &= ~ACK_WAITING;
	    }
	}
	set_timer(dnsix_nat_block.output_timer, OUTPUT_DELAY_TIME * ONESEC);
	break;
      case DNSIX_REDI_FAILURE:
	dnsix_nat_block.secondary = dnsix->source_address;
	if (dnsix_nat_block.state == NAT_FAILURE) {
	    dnsix_nat_block.state = NAT_SECONDARY;
	    dnsix_nat_block.dest = dnsix->source_address;
	    set_timer(dnsix_nat_block.output_timer,OUTPUT_DELAY_TIME * ONESEC);
	} 
	break;
      default:
	printf("\nDNSIX: Invalid opcode");
	return;
    }
    dnsix_audit_write (0, DNSIX_AUDIT_REDI_RCV, (uchar *)dnsix, 
		       DNSIX_HDR_SIZE, 0);
} /* end of dnsix_rcv_redi */


/*
 * start_dnsix_nat
 * start DNSIX network audit
 * trail service.
 */

boolean start_dnsix_nat_service (ipaddrtype source)
{
    ipsocktype	*sock;

    if (dnsix_nat_block.serving) {
	if (source)
	    dnsix_nat_block.source = source;
	return(TRUE);
    } else {
	dmdp_start();
	dnsix_nat_block.source = source;
	if (dnsix_nat_block.socket == NULL) {
	    sock = open_ipsocket(UDP_PROT, NULL, NULL, DNSIX_PORT, 
				 DNSIX_PORT, SOCK_ANYSRC);
	    if (sock == NULL) {
		printf("\nDNSIX-NAT: Failed to open UDP socket");
		return(FALSE);
	    }
	    dnsix_nat_block.socket = sock;
	    dmdp_register(sock, dnsix_rcv_nat, dnsix_ack_nat);
	}
	if (dnsix_nat_block.redi_socket == NULL) {
	    sock = open_ipsocket(UDP_PROT, NULL, NULL, DNSIX_REDI_PORT,
				 DNSIX_REDI_PORT, SOCK_ANYSRC);
	    if (sock == NULL) {
		printf("\nDNSIX-NAT: Failed to open UDP socket");
		return(FALSE);
	    }
	    dnsix_nat_block.redi_socket = sock;
	    /*
	     * we don't send any packets on this
	     * socket, we just receive redirect packets.
	     */
	    dmdp_register(sock, (receivetype)dnsix_rcv_redi, (notifytype)NULL);
	}
	
	dnsix_nat_block.output_timer = create_timer(dnsix_output,
						    NULL,
						    NULL,
						    NULL,
						    dnsix_name,
						    TMT_ONCE);
	dnsix_nat_block.serving = TRUE;
	if (dnsix_nat_block.dest)
	    dnsix_nat_block.state = NAT_FAILURE;
	return(TRUE);
    }
} /* end of start_dnsix_nat */


/*
 * stop_dnsix_nat
 * stop the DNSIX service
 */
void stop_dnsix_nat_service (void)
{
    int i;

    if (dnsix_nat_block.serving) {
	reg_invoke_ip_close_socket(dnsix_nat_block.socket);
	reg_invoke_ip_close_socket(dnsix_nat_block.redi_socket);
	dnsix_nat_block.serving = FALSE;
	dmdp_deregister(dnsix_nat_block.socket);
	dmdp_deregister(dnsix_nat_block.redi_socket);
	dnsix_nat_block.socket = NULL;
	dnsix_nat_block.redi_socket = NULL;
	dnsix_nat_block.primary = 0;
	dnsix_nat_block.secondary = 0;
	dnsix_nat_block.dest = 0;
	dnsix_nat_block.transmit_count = NAT_DEFAULT_TRANSMIT_COUNT;
	for (i = 0; i < NAT_REDIRECT_COUNT; i++) {
	    dnsix_nat_block.auth_redi[i] = 0;
	}
	destroy_timer(dnsix_nat_block.output_timer);
	destroy_timer(dnsix_nat_block.test_timer);
	dnsix_nat_block.output_timer = NULL;
	dnsix_nat_block.test_timer = NULL;
	dnsix_nat_block.test_time = 0;
	dmdp_service_stop();

    }
} /* end of stop_dnsix_nat */


/*
 * audit_trail_pak
 * check if it is audit trail
 * packet.
 */
boolean audit_trail_pak (paktype *pak)
{
    iphdrtype *ip;
    udptype *udp;
    
    ip = (iphdrtype *)ipheadstart(pak);

    if (pak->if_input == NULL) {
	if (ip->prot == UDP_PROT) {
	    udp = (udptype *)ipdatastart(pak);
	    if ((udp->dport == DNSIX_PORT) && 
		(udp->sport == DNSIX_PORT))
		return(TRUE);
	}
    }
    return(FALSE);
} /* end of audit_trail_pak */


/*
 * nat_state_string
 * returns an ascii string for
 * atm state
 */

char *nat_state_string (uchar type)
{
    switch (type) {
      case NAT_FAILURE:
	return("FAILURE");
      case NAT_PRIMARY:
	return("PRIMARY");
      case NAT_SECONDARY:
	return("SECONDARY");
      default:
	return("UNKNOWN");
    }
} /* end of nat_state_string */

/*
 * write_audit
 * routine that is called
 * from applications.
 */
void write_audit (paktype *pak, ulong record_id)
{
    iphdrtype *ip;
    ulong data[2];

    if (pak && audit_trail_pak(pak))
	return;

    ip = (iphdrtype *)ipheadstart(pak);

    if (dnsix_nat_block.serving) {
      if ((record_id == DNSIX_SECERROR_SENDR)
	  || (record_id == DNSIX_SECERROR_RECV)) {
	  data[0] = pak ? ip->srcadr : 0;
	  data[1] = pak ? ip->dstadr : 0;
	  dnsix_nat_buginf("\nNAT: sending Audit trail with code %d to %i",
			   record_id, data[1]);
	  dnsix_audit_write (0, record_id, (uchar *)data,
			     2 * sizeof(ulong), 0);
      }
  }
} /* end of write_audit */


/*
 * nat_rec_send
 * send audit trail
 * packet.
 */
void nat_rec_send (void)
{
    paktype *pak;
    natinfotype *info;

    pak = dnsix_nat_block.current_pak;
    info = INFO_PTR(pak);
    if (dnsix_nat_block.dest) {
	info->flag |= ACK_WAITING;
	(void)(dmdp_send (dnsix_nat_block.socket, 
			  pak,
			  dnsix_nat_block.dest,
			  0,
			  info->length));
    }
} /* end of nat_rec_send */


/*
 * dnsix_audit_write
 * write audit trails
 */
void dnsix_audit_write (ulong session_id, ulong nat_record_id,
			uchar *data, ushort length, ipaddrtype source)
{
    paktype *pak;
    dnsixtype *dnsix;
    natinfotype *info;
    secs_and_nsecs time;

    secs_and_nsecs_since_jan_1_1970(&time);
  back:
    if (dnsix_nat_block.buffer_pak) {
	pak = dnsix_nat_block.buffer_pak;
	info = INFO_PTR(pak);
    } else {
	pak = getbuffer(IPHEADERBYTES(NOPT) + UDPHEADERBYTES 
			+ DMDP_HDR_SIZE + NAT_AUDIT_DATA_SIZE);
	if (!pak) {
	    return;
	}
	dnsix_nat_block.buffer_pak = pak;
	info =INFO_PTR(pak);
	info->length = 0;
	info->ptr = (((uchar *) info) + UDPHEADERBYTES + DMDP_HDR_SIZE);
	info->count= 0;
    }
    if ((info->length + (DNSIX_HDR_SIZE + length)) >= NAT_AUDIT_DATA_SIZE) {
	if (dnsix_nat_block.current_pak)
	    return;
	else {
	    dnsix_nat_block.current_pak = dnsix_nat_block.buffer_pak;
	    dnsix_nat_block.buffer_pak = NULL;
	    nat_rec_send();
	    goto back;
	}
    }
    dnsix = (dnsixtype *)info->ptr;

    switch (nat_record_id) {
      case DNSIX_AUDIT_REDI_RCV:
	dnsix->length = DNSIX_HDR_SIZE + length;
	info->length += length + DNSIX_HDR_SIZE;
	if (length)
	    bcopy(data, dnsix->data_ptr, length);
	break;
      case DNSIX_SECERROR_SENDR:
      case DNSIX_SECERROR_RECV:
	dnsix->length = DNSIX_HDR_SIZE + length;
	info->length += length + DNSIX_HDR_SIZE;
	if (length)
	    bcopy(data, dnsix->data_ptr, length);
	break;
      case DNSIX_REDI_IGNORED:
	dnsix->length = DNSIX_HDR_SIZE + length + SOURCE_ADDR_SIZE;
	info->length += length + DNSIX_HDR_SIZE + SOURCE_ADDR_SIZE;
	if (length)
	    bcopy(data, dnsix->data_ptr, length);
	*(ulong *)(((uchar *) dnsix->data_ptr) + length) = source;
	break;
      default:
	printf("\nDNSIX-NAT: Record type %d not supported", nat_record_id);
	return;
    }
    dnsix->record_id = nat_record_id;
    info->count++;
    dnsix->source_address = dnsix_nat_block.source;
    dnsix->time_stamp = time.seconds;
    dnsix->time_offset = time.nanoseconds;
    dnsix_nat_block.atm_record_count++;

    if (info->count >= dnsix_nat_block.transmit_count)
	if (dnsix_nat_block.current_pak == NULL) {
	    dnsix_nat_block.current_pak = dnsix_nat_block.buffer_pak;
	    dnsix_nat_block.buffer_pak = NULL;
	    nat_rec_send();
	}
} /* end of dnsix_audit_write */


/*
 * dnsix_command
 */
void dnsix_command (parseinfo *csb)
{
    int keyword;
    natinfotype *info;
    int i;

    keyword = GETOBJ(int,1);
    if (csb->nvgen) {
	if (dnsix_nat_block.serving == FALSE)
	    return;
	switch (keyword) {
	case DNSIX_ATM_SOURCE:
	    nv_write(dnsix_nat_block.source, "%s %i", csb->nv_command,
		     dnsix_nat_block.source);
	    break;
	case DNSIX_ATM_PRIMARY:
	    nv_write(dnsix_nat_block.primary, "%s %i", csb->nv_command,
		     dnsix_nat_block.primary);
	    break;
	case DNSIX_ATM_SECONDARY:
	    nv_write(dnsix_nat_block.secondary, "%s %i", csb->nv_command,
		     dnsix_nat_block.secondary);
	    break;
	case DNSIX_ATM_AUTHO_REDIRECT:
	    for ( i = 0; i < NAT_REDIRECT_COUNT; i++) {
		nv_write(dnsix_nat_block.auth_redi[i], "%s %i",
			 csb->nv_command, dnsix_nat_block.auth_redi[i]);
	    }
	    break;
	case DNSIX_ATM_TRANSMIT_COUNT:
	    if (dnsix_nat_block.transmit_count != NAT_DEFAULT_TRANSMIT_COUNT)
		nv_write(TRUE, "%s %d ", csb->nv_command,
			 dnsix_nat_block.transmit_count);
	    break;
	}
	return;
    }
    /* get the keyword */
    keyword = GETOBJ(int,1);
    switch (keyword) {
      case DNSIX_ATM_SOURCE:
	if (csb->sense)
	    start_dnsix_nat_service(GETOBJ(paddr,1)->ip_addr);
	else
	    stop_dnsix_nat_service();
	break;
      case DNSIX_ATM_PRIMARY:
	if (dnsix_nat_block.serving == FALSE) {
	    printf("\nDNSIX-NAT: Please issue dnsix-nat source command first");
	    return;
	}
	if (csb->sense) {
	    dnsix_nat_block.primary = GETOBJ(paddr,1)->ip_addr;
	    dnsix_nat_block.dest = dnsix_nat_block.primary;
	    if (dnsix_nat_block.state != NAT_FAILURE) {
		dmdp_dump_pak(dnsix_nat_block.socket,dnsix_nat_block.
			      current_pak);
		if (dnsix_nat_block.current_pak) {
		    info = INFO_PTR(dnsix_nat_block.current_pak);
		    info->flag &= ~ACK_WAITING;
		}
	    }
	    dnsix_nat_block.state = NAT_PRIMARY;
	    set_timer(dnsix_nat_block.output_timer,OUTPUT_DELAY_TIME * ONESEC);
	} else {
	    dnsix_nat_block.primary = 0;
	    if (dnsix_nat_block.state == NAT_PRIMARY) {
		dnsix_nat_block.dest = dnsix_nat_block.secondary;
		if (dnsix_nat_block.dest) {
		    dnsix_nat_block.state = NAT_SECONDARY;
		    dmdp_dump_pak(dnsix_nat_block.socket,dnsix_nat_block.
				  current_pak);
		    if (dnsix_nat_block.current_pak) {
			info = INFO_PTR(dnsix_nat_block.current_pak);
			info->flag &= ~ACK_WAITING;
		    }
		    set_timer(dnsix_nat_block.output_timer, 
			      OUTPUT_DELAY_TIME * ONESEC);
		} else {
		    dnsix_nat_block.state = NAT_FAILURE;
		    dmdp_dump_pak(dnsix_nat_block.socket,dnsix_nat_block.
				  current_pak);
		    if (dnsix_nat_block.current_pak) {
			info = INFO_PTR(dnsix_nat_block.current_pak);
			info->flag &= ~ACK_WAITING;
		    }
		}
	    }
	}
	break;
      case DNSIX_ATM_SECONDARY:
	if (dnsix_nat_block.serving == FALSE) {
	    printf("\nDNSIX-NAT: Please issue dnsix-nat source command first");
	    return;
	}
	dnsix_nat_block.secondary = GETOBJ(paddr,1)->ip_addr;
	if (dnsix_nat_block.state == NAT_FAILURE) {
	    dnsix_nat_block.state = NAT_SECONDARY;
	    dnsix_nat_block.dest = dnsix_nat_block.secondary;
	    set_timer(dnsix_nat_block.output_timer,OUTPUT_DELAY_TIME * ONESEC);
	} else {
	    if (dnsix_nat_block.state == NAT_SECONDARY) {
		dnsix_nat_block.dest = dnsix_nat_block.secondary;
		dmdp_dump_pak(dnsix_nat_block.socket,dnsix_nat_block.
			      current_pak);
		if (dnsix_nat_block.current_pak) {
		    info = INFO_PTR(dnsix_nat_block.current_pak);
		    info->flag &= ~ACK_WAITING;
		}
		set_timer(dnsix_nat_block.output_timer, 
			  OUTPUT_DELAY_TIME * ONESEC);
	    }
	}
	break;
      case DNSIX_ATM_TRANSMIT_COUNT:
	if (dnsix_nat_block.serving == FALSE) {
	    printf("\nDNSIX-NAT: Please issue dnsix-nat source command first");
	    return;
	}
	dnsix_nat_block.transmit_count = GETOBJ(int, 2);
	break;
      case DNSIX_ATM_AUTHO_REDIRECT:
	if (dnsix_nat_block.serving == FALSE) {
	    printf("\nDNSIX-NAT: Please issue dnsix-nat source command first");
	    return;
	}
	for ( i = 0; i < NAT_REDIRECT_COUNT; i++) {
	    if (dnsix_nat_block.auth_redi[i] == 0) {
		dnsix_nat_block.auth_redi[i] = GETOBJ(paddr, 1)->ip_addr;
		break;
	    }
	}
	if (i == NAT_REDIRECT_COUNT) {
	    printf("\nDNSIX-NAT: Supports only %d redirect addresses",
		   NAT_REDIRECT_COUNT);
	    return;
	}
	break;
      case DNSIX_ATM_TEST:
	if (dnsix_nat_block.serving == FALSE) {
	    printf("\nDNSIX: Please issue dnsix-nat source command first");
	    return;
	}
	if (csb->sense) {
	    dnsix_nat_block.test_time = GETOBJ(int, 2);
	    dnsix_nat_block.test_timer = create_timer(dnsix_test,
						      NULL,
						      NULL,
						      NULL,
						      test_dnsix,
						      TMT_ONCE);

	    set_timer(dnsix_nat_block.test_timer,
		      dnsix_nat_block.test_time * ONESEC);
	} else {
	    destroy_timer(dnsix_nat_block.test_timer);
	    dnsix_nat_block.test_time = 0;
	    dnsix_nat_block.test_timer = NULL;
	}
	break;
      default:
	bad_parser_subcommand(csb, keyword);
	break;
    }
} /* end of dnsix_command */


/*
 * show_dnsix
 * Display dnsix information
 */

void show_dnsix (parseinfo *csb)
{
    int i;

    if (dnsix_nat_block.serving == FALSE) {
	printf("\nDNSIX: Dnsix is not enabled");
	return;
    }
    printf("\n   Audit Trail Enabled with  Source %i ",dnsix_nat_block.source);
    printf("\n          State: %s",nat_state_string(dnsix_nat_block.state));
    if (dnsix_nat_block.state != NAT_FAILURE)
	printf("\n          Connected to %i ",dnsix_nat_block.dest);
    if (dnsix_nat_block.primary)
	printf("\n          Primary %i ",dnsix_nat_block.primary);
    if (dnsix_nat_block.secondary)
	printf("\n          Secondary %i ",dnsix_nat_block.secondary);
    printf("\n          Transmit Count %d ",dnsix_nat_block.transmit_count);
    printf("\n          DMDP retries %d", dmdpblock.retries);
    if (dnsix_nat_block.auth_redi[0]) { 
	printf("\n          Authorization Redirection List:");
	for (i = 0; i <NAT_REDIRECT_COUNT; i++) {
	    if (dnsix_nat_block.auth_redi[i]) 
		printf("\n               %i",dnsix_nat_block.auth_redi[i]);
	}
    }
    printf("\n          Record count: %d ",dnsix_nat_block.atm_record_count);
    printf("\n          Packet Count: %d ",dnsix_nat_block.atm_pak_count);
    printf("\n          Redirect Rcv: %d ",dnsix_nat_block.atm_redi_count);

} /* end of show_dnsix */

/*
 * dnsix_init
 */
void dnsix_init (void) {
    dnsix_atm_init();
    dmdp_init();
    dnsix_parser_init();

    reg_add_ip_send_audit(write_audit, "write_audit");
}
/* end of dnsix_nat.c */
