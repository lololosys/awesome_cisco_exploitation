/* $Id: nasi.c,v 3.7.8.4 1996/09/11 01:46:32 che Exp $
 * $Source: /release/112/cvs/Xsys/xns/nasi.c,v $
 *------------------------------------------------------------------
 * NASI.c Netware Asynchronous Services Interface (Server code)
 *
 * Sept 1995, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nasi.c,v $
 * Revision 3.7.8.4  1996/09/11  01:46:32  che
 * CSCdi66795:  Double authentication with NASI and TACACS+
 * Branch: California_branch
 * Bypass exec authentication if we have already done NASI authentication.
 *
 * Revision 3.7.8.3  1996/06/07  01:17:40  irfan
 * CSCdi55494:  NASI server does not work after reload
 * Branch: California_branch
 * Don't reopen NASI's SPX socket if already opened.
 *
 * Revision 3.7.8.2  1996/06/03  20:23:50  john
 * CSCdi58748:  ipxwan and nlsp command nvgen order wrong and subsys issues
 * Branch: California_branch
 *
 * Revision 3.7.8.1  1996/03/18  22:52:52  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.3  1996/03/16  07:59:19  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.2.2  1996/03/07  11:21:45  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.2.1  1996/02/20  21:59:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/28  09:34:25  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.6  1996/02/03  04:13:19  tkolar
 * CSCdi47986:  IOS modem integration isnt quite there yet
 * Add a way for the IOS to know if the modem switch has reserved the
 * modem.
 *
 * Revision 3.5  1996/02/02  07:04:01  irfan
 * CSCdi47829:  rconsole wr t caused a crash after visiting an asyn port.
 * redefine static buffer to accommodate max expected o/p data length.
 *
 * Revision 3.4  1996/01/26  07:45:18  irfan
 * CSCdi47560:  nasi malformed service name
 * fix the name.
 *
 * Revision 3.3  1996/01/18  15:58:40  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.2  1995/11/17  19:22:18  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:58:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:38:45  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/26  12:13:02  irfan
 * Netware Asynchronous Services Interface: holding area
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ctype.h>
#include "subsys.h"
#include "sys_registry.h"
#include "../h/cs_registry.regh"
#include "address.h"
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "logger.h"
#include "ieee.h"
#include "../os/free.h"
#include "xns.h"
#include "novell.h"
#include "novell_public.h"
#include "../if/network.h"
#include "../os/buffers.h"
#include "../parser/macros.h"
#include "../clns/clns.h"
#include "../clns/isis_externs.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_ipx.h"
#include "../xns/ipx_registry.h"
#include "../xns/spx.h"
#include "../xns/nasi.h"
#include "../ui/debug.h"
#include "../xns/nasi_debug.h"
#include "connect.h"
#include "../os/async.h"
#include "../h/auth_registry.regh"
#include "aaa.h"
#include "stacks.h"

/*
 * some constants
 */
static const int NASI_SPEED_TABLE[NASI_MAX_SPEED_INDEX] = { 0, 50, 75, 100,
 						   150, 300, 600, 1200,
				                  1800, 2000, 2400, 3600,
				                  4800, 7200, 9600, 19200,
				                 38400, 57600, 115200 };
static const int NASI_WORDLEN_TABLE[NASI_MAX_WORDLEN_INDEX] = { 5, 6, 7, 8};
static const int NASI_STOPBITS_TABLE[NASI_MAX_STOPBITS_INDEX] =
                          { STOPBITS1, STOPBITS15, STOPBITS2 };
static const int NASI_PARITY_TABLE[NASI_MAX_PARITY_INDEX] =
                        { PARITY_NO, PARITY_ODD, PARITY_EVEN,
			      PARITY_MARK, PARITY_SPACE };

#define NASI_PORT_IN_USE(port)  \
	((MODEMS[port]->statbits & (HASPROC | CONNECTED | SLIPMODE)) \
		|| reg_invoke_modem_hardware_in_use(MODEMS[port]))

/*
 * Local Variables
 */
static boolean nasi_enabled = FALSE;
boolean nasi_configured = FALSE;
static int nasi_service_socket = NASI_SERVICE_SOCKET;
spxsocktype *nasi_my_spx_sock = NULL;
static void nasi_vector_init(void);
static forktype nasi_main(spxsocktype *);
static int nasi_process;
static uchar nasi_service_name[NASI_SERVICE_NAME_LEN];
static ulong nasi_internal_net;
vectortype NASIvector;
connvector nasi_connvector;
static ulong nasi_status_flags = 0;
static char temp_nasi_port_name_string[NASI_PORT_NAME_MAXLEN];
static nasi_port ret_port;  /* will be used immediately by caller */
static char nasi_tty_op_data[sizeofnasi_hdr3 + NASI_MAX_SEND + 16];



/*
 * Forward declarations
 */
static void nasi_setup_ncs_service(void);
static boolean nasi_setup_spx_socket(void);
static void nasi_reset_stats(void);
static boolean nasi_newincoming_con(spxsocktype *, spxcontype *);
static boolean nasi_close_con_allowed(spxcontype *spxcon);
void nasi_idb_up(idbtype *);
static void nasi_ipx_internal_net_changed(void);
static void nasi_ipx_internal_name_changed(void);
static void nasi_start(void);
static spxcontype *tty2spxcon(tt_soc *, boolean);
static int nasi_getc(tt_soc *);
static void nasi_clearinput(tt_soc *);
static int nasi_inputpending(tt_soc *);
static void nasi_putc(tt_soc *, int);
static boolean nasi_outputblock(tt_soc *);
static boolean nasi_startoutput(tt_soc *);
static int nasi_outputpending(tt_soc *);
static void nasi_clearoutput(tt_soc *tty);
static void nasi_setsflow(tt_soc *tty);
static boolean nasi_set_new_tty(conntype *, tt_soc *);
static boolean nasi_alive(conntype *);
static void nasi_close_connection(conntype *);
static void nasi_putbyte (spxcontype *, int);
static boolean nasi_putblock(spxcontype *);
static int nasi_unread(conntype *);
static int nasi_getbyte(spxcontype *, int *);
static boolean start_nasivty(tt_soc *, spxcontype *);
static void nasi_send_server_preq_response(spxcontype *, uchar *,
					   tt_soc *, boolean);
static void nasi_rcvd_srvinfo_req(spxcontype *, uchar *, paktype *);
static void nasi_rcvd_qname_ffirst(spxcontype *, uchar *, paktype *);
static void nasi_rcvd_qname_fnext(spxcontype *, uchar *, paktype *);
static void nasi_send_close_pkt(spxcontype*);
static void nasi_rcvd_close(spxcontype *);
static void nasi_rcvd_flush_rxbuf(spxcontype *);
static void nasi_rcvd_flush_txbuf(spxcontype *);
static void nasi_rcvd_set_service_name(spxcontype *, uchar *);
static void nasi_rcvd_get_service_name(spxcontype *, uchar *);
static void nasi_rcvd_get_general_name(spxcontype *, uchar *);
static void nasi_rcvd_get_specific_name(spxcontype *, uchar *);
static void nasi_send_setget_service_response(spxcontype *, uchar *,
					      boolean, ushort,
					      uchar *, uchar *);
static void nasi_send_server_ack(spxcontype *, nasi_hdr3 *, int, boolean);
static void nasi_decrypt_pw(uchar *, uchar *);
/*
static void nasi_encrypt_pw(uchar *, uchar *);
*/
static void nasicon_setbuffers(spxcontype *, int, int);
static void nasi_process_input(spxcontype *, paktype *, int);
static void nasi_rcv_client_data(spxcontype *, uchar *);
static void nasicon_output(spxcontype *);
static boolean nasicon_puts(spxcontype *, int, uchar *);
static void nasicon_clearinput(spxcontype *);
static void nasicon_clearoutput(spxcontype *);
static void nasi_initport_rcvd(spxcontype *, uchar *, paktype *);
static void nasi_modify_port(spxcontype *, nasi_preq *);
static boolean nasiBLOCK(ulong);
static boolean nasi_check_auth(tt_soc *, nasi_preq *);
static void nasi_break_rcvd(spxcontype *, boolean);
static void nasi_sflow_rcvd(spxcontype *, boolean);
static void nasi_def_sflow_rcvd(spxcontype *, boolean);
static boolean nasi_sendbyte(spxcontype *, uchar);
static boolean nasi_send_data_pkt(spxcontype *, int, uchar *);

static int nasi_next_port(int);
static nasi_port *nasi_get_port_info(int);
static int nasi_port_name_to_num (uchar *);
static int nasi_read_portnum(uchar *);
static void nasi_get_port_params(nasi_port *, int);
static nasi_port *nasi_get_port_info(int);

static inline uchar tty2nasi_speed(int);
static inline uchar tty2nasi_databits(int);
static inline uchar tty2nasi_stopbits(int);
static inline uchar tty2nasi_parity(int);
static inline uchar nasi_table_search(const int [], int, int, int, int);
static inline uchar tty2nasi_flow(ulong);

/*
 * this code has some magic numbers, where we
 * attempt to reproduce appropriate NASI packets
 * as expected by the client. These values are
 * not fully understood, hence not documented
 * meaningfully.
 * Such magic numbers will be indicated by this comment:
 *   Magic numbers to fake NASI pkts. See top for comment.
 */


/*
 * nasi_server_init
 * Set up NASI data structures
 * Initialize the NASI server system. Includes setting up data
 * structures, sourcing a SAP entry for NASI, registering parse
 * chains and hooking into some IPX registeries.
 */
static void nasi_server_init (subsystype *subsys)
{
    
    nasi_setup_parser();
    nasi_vector_init();
    nasi_debug_init();
    reg_add_ipx_internal_net_changed(nasi_ipx_internal_net_changed,
				     "nasi_ipx_internal_net_changed");
    reg_add_set_hostname(nasi_ipx_internal_name_changed,
			 "nasi_ipx_internal_name_changed");
    reg_add_restarted(nasi_start, "nasi_start");
}

/*
 * nasi_server_enable
 *
 * Make server ready for incoming NCS services.
 */
void nasi_server_enable (void)
{

    if (nasi_enabled) {
	return;
    }

    nasi_reset_stats();
    if ((NULL != nasi_my_spx_sock) || nasi_setup_spx_socket()) {
	if (systeminit_flag) {
	    nasi_start();
	}
	return;
    }
    if (nasi_err_debug) {
	buginf("\nNASI: Failed to start. Bad SPX socket");
    }
}

/*
 * nasi_server_disable
 *
 * Make server unavailable for NCS.
 */
void nasi_server_disable (void)
{

    nasi_reset_stats();
    nasi_enabled = FALSE;
    nasi_status_flags |= NASI_MAIN_EXIT;

}

/*
 * nasi_start
 *
 * delayed setup of IPX SAP services and startup of process
 * doesn't work if called at system startup time.
 */
static void nasi_start (void)
{
    
    if (nasi_configured) {
	nasi_setup_ncs_service();
	nasi_process = cfork((forkproc *)nasi_main, (long) nasi_my_spx_sock,
			     0, "NASI Handler", 0);
	if (NO_PROCESS == nasi_process)
	    return;
	if (nasi_activity_debug) {
	    buginf("\nNASI-MAIN: Started. Socket ptr: %x",
		   nasi_my_spx_sock);
	}
	nasi_enabled = TRUE;
    }
}

/*
 * nasi_ipx_internal_name_changed
 *
 * NASI service name depends on hostname
 * we need to keep track of changes to system hostname
 */
static void nasi_ipx_internal_name_changed (void)
{

    /*
     * Delete the current SAP for NCS
     * use old name to delete.
     */
    if (nasi_configured) {
	nov_sap_command(FALSE, FALSE,
			nasi_service_name, NOVELL_SAP_NASI,
			nasi_internal_net, ipx_internal_hostid,
			nasi_service_socket, 0,
			IPX_STATIC_DEFINE_TYPE_NO_NVGEN);

	/*
	 * copy new hostname into our local copy
	 */
	sprintf(nasi_service_name,
		"%s-%s", hostname, "NCS");
	/* create a new SAP for NCS
	 * use new name to create.
	 */
	nov_sap_command(FALSE, TRUE,
			nasi_service_name, NOVELL_SAP_NASI,
			nasi_internal_net, ipx_internal_hostid,
			nasi_service_socket, 0,
			IPX_STATIC_DEFINE_TYPE_NO_NVGEN);
    }
}

/*
 * nasi_ipx_internal_net_changed
 *
 * nasi is uses system's internal network number as source
 * we need to keep track of changes to this network.
 */
static void nasi_ipx_internal_net_changed (void)
{
    if (nasi_configured) {
	/*
	 * delete the current SAP for NCS
	 */
	nov_sap_command(FALSE, FALSE,
			nasi_service_name, NOVELL_SAP_NASI,
			nasi_internal_net, ipx_internal_hostid,
			nasi_service_socket, 0,
			IPX_STATIC_DEFINE_TYPE_NO_NVGEN);

	/*
	 * create a new SAP for NCS
	 */
	nov_sap_command(FALSE, TRUE,
			nasi_service_name, NOVELL_SAP_NASI,
			ipx_internal_net,
			ipx_internal_hostid,
			nasi_service_socket, 0,
			IPX_STATIC_DEFINE_TYPE_NO_NVGEN);
    }
    nasi_internal_net = ipx_internal_net;
}

/*
 * nasi_setup_ncs_service
 *
 * Create the static NASI SAP on ipx_internal_net
 */
static void nasi_setup_ncs_service (void)
{

    if (nasi_configured) {
	if (nasi_activity_debug) {
	    buginf("\nNASI: Attempting to setup SAP services");
	}
	/*
	 * create a new SAP for NCS
	 */
	nasi_internal_net = ipx_internal_net;
	sprintf(nasi_service_name, "%s-%s",
		hostname, "NCS");
	nov_sap_command(FALSE, TRUE,
			nasi_service_name, NOVELL_SAP_NASI,
			ipx_internal_net,
			ipx_internal_hostid,
			nasi_service_socket, 0,
			IPX_STATIC_DEFINE_TYPE_NO_NVGEN);
    }
    return;
}

/*
 * nasi_setup_spx_socket
 *
 * Open the listening SPX socket on NASI socket #
 */
static boolean nasi_setup_spx_socket (void)
{
    nasi_my_spx_sock = spx_api_listen_socket(nasi_service_socket,
					     SPXCON_PACKET_IO,
					     nasi_newincoming_con,
					     nasi_close_con_allowed);
    if (NULL == nasi_my_spx_sock) {
	if (nasi_err_debug) {
	    buginf("\nNASI: failed to open SPX listen socket");
	}
	return (FALSE);
    }
    return (TRUE);
}

/*
 * nasi_reset_stats
 *
 * Reset various NASI counters.
 * currently we don't maintain any. Will add later
 */
static void nasi_reset_stats (void)
{
}

/*
 * nasi_vector_init
 *
 * Set up the nasi tty vector
 */
void nasi_vector_init (void)
{
    vectortype *vector;

    vector = &NASIvector;
    vector_init(vector);		/* initialize vector structure */

    vector->vgetc = nasi_getc;		/* character input */
    vector->vclearinput = nasi_clearinput;/* clear buffered input */
    vector->vinputpending = nasi_inputpending;

    vector->vputc = nasi_putc;			/* character output */
    vector->vputpkt = generic_putpkt;		/* Put packet routine */
    vector->voutputblock = nasi_outputblock;	/* Would output block? */

    vector->vstartoutput = nasi_startoutput;	/* start character output */
    vector->voutputpending = nasi_outputpending;
    vector->vclearoutput = nasi_clearoutput;	/* send AO DM */
    vector->vsetsflow = nasi_setsflow;		/* turn on flow control */

    vector->vservicehook = helper_servicehook;
    vector->vnoservicehook = helper_noservicehook;

    conn_vector_init(&nasi_connvector,      /* Init the connection vector */
		     "nasi", "NASI protocol");

    nasi_connvector.vsettty = nasi_set_new_tty; /* Set the new tty routine */
    nasi_connvector.valive = nasi_alive; /* Is connection alive? */
    nasi_connvector.vclose = nasi_close_connection; /* Close the session */
    nasi_connvector.vunread = nasi_unread; /* Get unread input count */
}

/*
 * start_nasivty
 *
 * Create a tiptop for a NASI reverse virtual terminal connection.
 * Returns FALSE if couldn't create tiptop, TRUE otherwise.
 */
static boolean start_nasivty (tt_soc *tty, spxcontype *spxcon)
{

    conntype *conn;		/* connection pointer */
    ulong stacksize = 0;

    conn = getconnection(tty, &nasi_connvector);
    if (NULL == conn) {
	if (nasi_err_debug) {
	    buginf("\nNASI%t: failed to get connection structure",
		   tty->ttynum);
	}
	return(FALSE);
    }
    conn->spxcon = spxcon;			/* point to SPXconnection */
    spxcon->ttynum = tty->ttynum;		/* set line number */
    unqueue(&tty->connhead, conn);	/* remove from general q */
    tty->conntop = conn;		/* remember the driver conn */
    tty->statbits &= ~(CARDROP+HANGUP+IDLE);	/* reincarnated again! */
    tty->statbits |= READY;
    tty->conncount++;				/* another use */
    tty->ivector = &NASIvector;			/* this is a NASI vty */
    spxcon->tty = tty;				/* point back to tty */
    bcopy(&spxcon->remote_net, &conn->address.nov_addr.net, XNS_NETBYTES);
    ieee_copy(spxcon->remote_host, conn->address.nov_addr.host);
    conn->address.type = ADDR_NOVELL;
    conn->address.length = ADDRLEN_NOVELL;
    nasicon_setbuffers(spxcon, NASI_IO_BUFSIZE, NASI_IO_BUFSIZE);
    if (tty->tty_passspeed) {
        setspeed (tty, tty->tty_passspeed, tty->tty_passspeed);
    }

    switch (tty->type) {

    case VTY_LINE:
	tty_create_vty_exec(tty);
	break;

    case AUX_LINE:
    case TTY_LINE:
	if (reg_invoke_kerberos_is_running())
	    stacksize = (ulong) EXEC_STACK_SIZE;
	tty->tiptop = cfork((forkproc *)tty_daemon, (long)tty, stacksize,
			    "TTY Daemon", tty->ttynum);
	break;

    default:
	tty->tiptop = -1;
	break;
    }

    if (tty->tiptop < 0) {
	freeconnection(conn);
	return(FALSE);
    }
    return(TRUE);
}

/*
 * tty2spxcon
 *
 * Obtain the SPX connection block ptr given a tty
 * Returns NULL if connection is dead or dying.
 */
static spxcontype *tty2spxcon (tt_soc *tty, boolean readflag)
{
    spxcontype *spxcon;

    if (tty->statbits & (CARDROP|IDLE)) {
	return(NULL);
    }
    if (VTY_LINE == tty->type) { /* on vtys, set cardrop if connection dies */
	if (NULL == tty->conntop) {
	    tty->statbits |= CARDROP;
	} else {
	    spxcon = tty->conntop->spxcon;
	    if (readflag && spx_api_con_closing(spxcon)) {
		tty->statbits |= CARDROP;
		return(NULL);
	    }
	}
    }
    if (tty->conntop) {
	return(tty->conntop->spxcon);
    } else {
	return (NULL);
    }
}

/*
 * nasi_outputblock
 *
 * Determine if output would block
 */
static boolean nasi_outputblock (tt_soc *tty)
{
    spxcontype *spxcon;

    spxcon = tty2spxcon(tty, FALSE);
    if (NULL == spxcon) {
	return(FALSE);
    }
    if (SPXCON_ST_OPEN != spxcon->state) {
	return(FALSE);
    }
    return (nasi_putblock(spxcon));
}

/*
 * nasi_startoutput
 *
 * Send off a buffer of bytes
 */
static boolean nasi_startoutput (tt_soc *tty)
{
    spxcontype *spxcon;

    spxcon = tty2spxcon(tty, FALSE);
    if (NULL == spxcon) {
	return(FALSE);
    }

    /*
     * Signal helper process.
     */
    if (tty_signal_helper(tty)) {
	return(TRUE);
    }

    spx_api_push(spxcon, TRUE, FALSE);
    return(TRUE);
}


/*
 * nasi_set_new_tty
 *
 * Attempt to set a new tty for the nasi connection.
 * used by async_daemon_attach kind of thingies
 */
static boolean nasi_set_new_tty (conntype *conn, tt_soc *tty)
{
    spxcontype *spxcon = conn->spxcon;

    spxcon->tty = tty;
    spxcon->ttynum = tty->ttynum;
    return(TRUE);

}

/*
 * nasi_alive
 *
 * Determine whether the nasi connection is alive or not
 */
static boolean nasi_alive (conntype *conn)
{
    if (conn->spxcon && spx_api_con_alive(conn->spxcon)) {
	return(TRUE);
    }
    return(FALSE);
}

/*
 * nasi_close_connection
 *
 * Generic close call for connection of type NASI.
 * Called via  conn_close()
 */
static void nasi_close_connection (conntype *conn)
{
    spxcontype *spxcon;

    if (!conn) {
	return;
    }
    spxcon = conn->spxcon;

    if (nasi_activity_debug) {
	buginf("\nNASI: %x need to close connection", spxcon);
    }
    /*
     * send a NASI close packet
     *      (if there is any such thing)
     */
    if (!spx_api_con_closing(spxcon)) {
	nasi_send_close_pkt(spxcon);
    }

    /*
     * release the connection block
     */
    freeconnection(conn);	/* release connection block */

    /*
     * don't kill the NASI connection yourself
     * send a signal to the NASI_main process
     */
    spxcon->nasi_flags |= NASI_FLAG_NEED_CLOSE;
    spxcon->tty = NULL;
}

/*
 * nasicon_close
 *
 * called from nasi_main process to close this connection.
 * nasi_close_connection sets up a flag that tells nasi_main
 * to call this routine; this in turn will cause the SPX
 * thread to purge the spxcon.
 */
static void nasicon_close (spxcontype *spxcon)
{

    if (nasi_activity_debug) {
	buginf("\nNASI%t: MAIN closing connection",
	       (spxcon->tty)?spxcon->tty->ttynum:0);
    }
    free(spxcon->nasi_instart);
    free(spxcon->nasi_outstart);
    spxcon->nasi_instart = NULL;
    spxcon->nasi_outstart = NULL;
    spxcon->nasi_flags &= ~NASI_FLAG_NEED_CLOSE;
    if (spxcon) {
	spx_api_closecon(spxcon);
    }
}

/*
 * nasi_newincoming_con
 *
 * for any new incoming SPX connection for NASI socket
 * ensure that we want to open it. This could also include
 * a check for MAX-connections...not available currently.
 */
static boolean nasi_newincoming_con (spxsocktype *spxsock,
				     spxcontype *spxcon)
{

    if (! ((nasi_configured && nasi_enabled) &&
	   (spxsock->sock_num == nasi_service_socket) )) {
	return (FALSE);
    }

    if (nasi_activity_debug) {
	buginf("\nNASI: new incoming connection %x", spxcon);
    }
    return (TRUE);
}


/*
 * nasi_close_con_allowed
 *
 * is it a simple spxcon, not tied to any tty yet ?
 * if yes, this can be purged and killed easily by
 * the SPX thread. Else it needs to be hanged onto and
 * some purge timer needs to expire.
 */
static boolean nasi_close_con_allowed (spxcontype *spxcon)
{

    if (spxcon->tty) {
	/*
	 * IF there are resources for this NASI session,
	 * then we don't want to kill this session here
	 * Other processes need to detect purging state
	 * and clean up.
	 */
	return (FALSE);
    }
    return (TRUE);
}

/*
 * nasi_putblock
 *
 * would a put block for this connection ?
 */
static boolean nasi_putblock (spxcontype *spxcon)
{
    return (spx_api_outputsize(spxcon) > 0);
}

/*
 * nasi_unread
 *
 * possibly put back the last byte read. Not possible with NASI
 */
static int nasi_unread (conntype *conn)
{
    return -1;
}

/*
 * Block function for nasi_main
 */
static boolean nasiBLOCK (ulong nasi_sock)
{
    spxcontype *spxcon;

    /*
     * for all NASI connections...wake up if any
     * input packets pending. Or any net output
     * is needed. Or any signal is waiting on NASI
     * connection.
     */
    FOR_ALL_SPXCON_ON_SOCK((spxsocktype *)nasi_sock, spxcon)
	if (spx_api_con_alive(spxcon)) {
	    if (spx_api_inputsize(spxcon) || spxcon->nasi_outcount
		|| spxcon->nasi_flags) {
		return (FALSE);
	    }
	} else {
	    return (FALSE);
	}

    return (TRUE);
}

/*
 * nasi_main
 *
 * the process that scans through all NASI connections.
 *    reading and responding to all network packets.
 *    if a tty is available, it will do tty I/O as well.
 *    if the connection needs to be closed, it will try to
 *    do so.
 */
static forktype nasi_main (spxsocktype *nasi_sock)
{

    if (nasi_sock <= 0) {
	if (nasi_err_debug || nasi_activity_debug) {
	    buginf("\nNASI-MAIN: startup failed. No SPX socket");
	}
	process_kill(THIS_PROCESS);
	return;
    }
    nasi_status_flags |= NASI_MAIN_RUNNING;
    process_wait_on_system_init();
    while (!(NASI_MAIN_EXIT & nasi_status_flags)) {
	spxcontype *spxcon;
	paktype *pak;
	int size, pak_count;
	int ret_code;

	edisms((blockproc *)nasiBLOCK, (ulong) nasi_sock);
	FOR_ALL_SPXCON_ON_SOCK(nasi_sock, spxcon) {
	    pak_count = NASI_PER_CON_PAK_COUNT;
	    while (--pak_count) {
		ret_code = spx_api_read_chunk(spxcon, &pak, &size);
		if ((SPX_RET_DATA == ret_code) && pak && (size > 0)) {
		    nasi_process_input(spxcon, pak, size);
		    datagram_done(pak);
		} else {
		    break;
		}
	    }
	    if (spxcon->nasi_outcount) {
		nasicon_output(spxcon);
	    }
	    if (spxcon->nasi_flags & NASI_FLAG_NEED_CLOSE) {
		nasicon_close(spxcon); /* Close the NASI connection */
		break;                 /* The connection queue has */
		                       /*  been manipulated. Get out */
	    }
	}
    }
    nasi_status_flags &= ~(NASI_MAIN_RUNNING | NASI_MAIN_EXIT);
    if (nasi_err_debug || nasi_activity_debug) {
	buginf("\nNASI-MAIN: exited");
    }
    /*
     * shutdown and clean up all NASI connections.
     * close spx socket.
     * clear all SAP entries.
     */
    process_kill(THIS_PROCESS);
}


/*
 * nasi_sendbyte
 *
 * write a single byte back to the client over the network
 */
static boolean nasi_sendbyte (spxcontype *spxcon, uchar ch)
{
    static nasi_hdr3 byte_buf;

    memset(&byte_buf, 0, sizeofnasi_hdr3);
    PUTSHORT(&byte_buf.code_subcode, h2n(NASI_OP_SRV_DATA));
    /*
     * Magic numbers to fake NASI pkts. See top for comment.
     */
    byte_buf.size = 1;
    byte_buf.field1 = 0x01;
    byte_buf.field3 = 0x01;
    byte_buf.field4 = 0x20;
    byte_buf.field5 = 0x01;
    byte_buf.field6 = 0x40;
    byte_buf.data = ch;
    if (nasi_activity_debug || nasi_pkt_debug) {
	buginf("\nNASI%t: %x sending NASI server TTY data: 1 byte"
	       " in %d size pkt", (spxcon->tty)?spxcon->tty->ttynum:0,
	       spxcon->remote_con, sizeofnasi_hdr3+1);
    }
    return (SPX_RET_OK == spx_api_send_data(spxcon, (uchar *) &byte_buf,
					    sizeofnasi_hdr3+1));
}

/*
 * nasi_send_server_ack
 *
 * send a server ack message, for every bunch of data received.
 */
static void nasi_send_server_ack (spxcontype *spxcon, nasi_hdr3 *nhdr,
				  int val, boolean good)
{
    /* good: 4481, size-0, 0100, 0120, 0000, 0140, 00 */
    /* bad:  44C1, size-0, 0100, 0920, 0000, 0140, 00 */

    memset(nhdr, 0, sizeofnasi_hdr3);
    PUTSHORT(&nhdr->code_subcode, h2n(
	  (good) ? NASI_OP_SRV_DATA_GOOD_ACK : NASI_OP_SRV_DATA_BAD_ACK));
    nhdr->size = val;
    /*
     * Magic numbers to fake NASI pkts. See top for comment.
     */
    nhdr->field1 = 0x01;
    nhdr->field3 = (good) ? 0x01 : 0x09;
    nhdr->field4 = 0x20;
    nhdr->field5 = 0x01;
    nhdr->field6 = 0x40;
    if (nasi_activity_debug || nasi_pkt_debug) {
	buginf("\nNASI%t: %x sending NCS %s server Data Ack in %d bytes pkt"
	       " in %d size pkt", (spxcon->tty)?spxcon->tty->ttynum:0,
	       spxcon->remote_con, (good) ? "Good":"Bad",
	       val, sizeofnasi_hdr3);
    }
    spx_api_send_data(spxcon, (uchar *) nhdr, sizeofnasi_hdr3);
}

/*
 * nasi_rcv_client_data
 *
 * we have received some tty data from the client. What do
 * we do with it ?   Probably write it on the tty native.
 */
static void nasi_rcv_client_data(spxcontype *spxcon, uchar *data)
{
    nasi_hdr3 *nhdr = (nasi_hdr3 *) data;

    if (spxcon->tty) {
	if (!nasicon_puts(spxcon, nhdr->size, &nhdr->data)) {
	    if (nasi_err_debug) {
		buginf("\nNASI%t: %x nasicon_puts failed",
		       spxcon->tty->ttynum, spxcon);
	    }
	}
	startoutput(spxcon->tty);           /* make sure output running */
	nasi_send_server_ack(spxcon, nhdr, nhdr->size, TRUE);
    } else {
	if (nasi_err_debug) {
	    buginf("\nNASI: %x incoming data for 'no' tty ??",
		   spxcon);
	}
	nasi_send_server_ack(spxcon, nhdr, nhdr->size, FALSE);
    }
}

/*
 * nasi_process_input
 *
 * some packet came in for this NASI connection.
 * let's see what we can do with it.
 */
static void nasi_process_input (spxcontype *spxcon, paktype *pak,
				int size)
{

    uchar *nasi_data = ((uchar *)(ipxheadstart(pak))) + sizeof(spxhdr);
    ushort code_subcode;
    int i;

    code_subcode = n2h(GETSHORT(nasi_data));

    switch (code_subcode) {

    case NASI_OP_CLIENT_DATA:
	if (nasi_pkt_debug) {
/*	    buginf("\nNASI%t: %x incoming tty data",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);*/
	}
	nasi_rcv_client_data(spxcon, nasi_data);
	break;

    case NASI_OP_SRV_DATA:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x server receiving NASI_OP_SRV_DATA ????",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	break;

    case NASI_OP_SRV_DATA_GOOD_ACK:
    case NASI_OP_SRV_DATA_BAD_ACK:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x server receiving NASI_OP_SRV_DATA_ACK ????",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	break;

    case NASI_OP_SRV_DISCONNECT:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x server receiving NASI_OP_SRV_DATA_ACK ????",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	break;

    case NASI_OP_INITIALIZE_PORT:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x port request. setting up port",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	nasi_initport_rcvd(spxcon, nasi_data, pak);
	break;

    case NASI_OP_RELEASE_PORT:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x port release request. killing daemon",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	nasi_rcvd_close(spxcon);
	break;

    case NASI_OP_QNAME_FFIRST:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x Query Port. Find first",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	nasi_rcvd_qname_ffirst(spxcon, nasi_data, pak);
	break;

    case NASI_OP_QNAME_FNEXT:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x Query Port. Find next",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	nasi_rcvd_qname_fnext(spxcon, nasi_data, pak);
	break;

    case NASI_OP_CHECK_SERVER:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x Check server info",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	nasi_rcvd_srvinfo_req(spxcon, nasi_data, pak);
	break;

    case NASI_OP_INFO_SERVER:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x Check Server",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	nasi_rcvd_srvinfo_req(spxcon, nasi_data, pak);
	break;

    case NASI_OP_FLUSH_RXBUF:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x Flush Rx Buffers",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	nasi_rcvd_flush_rxbuf(spxcon);
	break;

    case NASI_OP_FLUSH_TXBUF:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x Flush Tx Buffers",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	nasi_rcvd_flush_txbuf(spxcon);
	break;

    case NASI_OP_SET_SERVICE_NAME:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x Set Service name",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	nasi_rcvd_set_service_name(spxcon, nasi_data);
	break;

    case NASI_OP_GET_SERVICE_NAME:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x Get Service name",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	nasi_rcvd_get_service_name(spxcon, nasi_data);
	break;

    case NASI_OP_GET_GENERAL_NAME:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x Get General name",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	nasi_rcvd_get_general_name(spxcon, nasi_data);
	break;

    case NASI_OP_GET_SPECIFIC_NAME:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x Get Specific name",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	nasi_rcvd_get_specific_name(spxcon, nasi_data);
	break;

    case NASI_OP_DISCONNECT:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x Rcvd Disconnect VC",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	nasi_rcvd_close(spxcon);
	break;

    case NASI_OP_BREAK_OFF:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x Rcvd Break Off",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	nasi_break_rcvd(spxcon, FALSE);
	break;

    case NASI_OP_BREAK_ON:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x Rcvd Break On",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	nasi_break_rcvd(spxcon, TRUE);
	break;

    case NASI_OP_HOLD:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x Rcvd VC Hold",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	break;

    case NASI_OP_SET_SFLOW:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x Rcvd Set SFlow",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	nasi_sflow_rcvd(spxcon, TRUE);
	break;

    case NASI_OP_RESET_SFLOW:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x Rcvd Reset SFlow",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	nasi_sflow_rcvd(spxcon, FALSE);
	break;

    case NASI_OP_DEFINE_SFLOW:
	if (nasi_pkt_debug) {
	    buginf("\nNASI%t: %x Rcvd Define SFlow",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con);
	}
	nasi_def_sflow_rcvd(spxcon, TRUE);
	break;

    default:
	if (nasi_err_debug || nasi_pkt_debug) {
	    buginf("\nNASI%t: %x Unknown NASI code %X  Pkt Size: %d",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con, code_subcode, size);
	    buginf("\n");
	    for (i=0; i<size; i++) {
		buginf(" %x", nasi_data[i]);
	    }
	}
	break;
    }
}

static void nasi_rcvd_srvinfo_req (spxcontype *spxcon,
				   uchar *nasi_data, paktype *pak)
{
    nasi_sinfo_hdr *nasi_shdr = (nasi_sinfo_hdr *) nasi_data;
    int i;

    /*
     * Magic numbers to fake NASI pkts. See top for comment.
     */
    nasi_shdr->field1 = 0x10;
    nasi_shdr->field2 = 0x11;
    nasi_shdr->field4 = 0x04;
    nasi_shdr->field5 = 0x0;
    nasi_shdr->field6 = 0x41;
    nasi_shdr->field3 = 0x22;

    /*
     * The name response field should be NULL
     * and incoming name should either match our's or else
     * it should be all wildcards.
     */
    i = strlen(hostname);
    if ((!nasi_shdr->out_srv_name[0]) &&
	((!strncmp(nasi_shdr->in_srv_name, NASI_ANYNAME8,
		   NASI_SRVSTRING_LEN)) || 
	 (!strncmp(nasi_shdr->in_srv_name, hostname, i)))) {

	nasi_shdr->err = 0x0;

	strncpy(nasi_shdr->out_srv_name, hostname,
		NASI_SRVSTRING_LEN);
	for (; i < NASI_SRVSTRING_LEN; i++) {
	    nasi_shdr->out_srv_name[i] = NASI_NULL_CHAR;
	}
	nasi_shdr->out_srv_name[NASI_SRVSTRING_LEN] = 0x0;

	bcopy((uchar *)&spxcon->local_net, nasi_shdr->snet, XNS_NETBYTES);
	bcopy((uchar *)&spxcon->local_host, nasi_shdr->shost, XNS_HOSTBYTES);
	PUTSHORT(&nasi_shdr->s_sock, h2n(spxcon->local_sock));
	if (nasi_activity_debug || nasi_pkt_debug) {
	    buginf("\nNASI%t: %x sending server-info %x  "
		   " Good response: %d bytes",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con, GETSHORT(&nasi_shdr->code_subcode),
		   sizeofnasi_sinfo_hdr);
	}
    } else {
	nasi_shdr->err = 0xfc;
	if (nasi_activity_debug || nasi_pkt_debug) {
	    buginf("\nNASI%t: %x sending server-info %x  "
		   " Bad response: %d bytes",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con, GETSHORT(&nasi_shdr->code_subcode),
		   sizeofnasi_sinfo_hdr);
	}
    }
    spx_api_send_data(spxcon, nasi_data, sizeofnasi_sinfo_hdr);
}

/*
 * nasi_send_server_preq_response
 *
 * send the response packet for port request. Parameter 'good'
 * defines whether it is an ack or a nak.
 */
static void nasi_send_server_preq_response (spxcontype *spxcon,
					    uchar *nasi_data,
					    tt_soc *tty, boolean good)
{
    nasi_preq *nasi_hdr = (nasi_preq *) nasi_data;
    nasi_port *nasi_port_ptr = NULL;

    memset(nasi_hdr, 0, sizeofnasi_preq_info);
    PUTSHORT(&nasi_hdr->code_subcode, NASI_OP_INITIALIZE_PORT);
    /*
     * Magic numbers to fake NASI pkts. See top for comment.
     */
    PUTSHORT(&nasi_hdr->ret_cx, h2n(0x1000));
    PUTSHORT(&nasi_hdr->ret_word2, h2n(0x0141));
    if (good && tty) {
	PUTSHORT(&nasi_hdr->ret_ax, h2n(0x0004));
	PUTSHORT(&nasi_hdr->ret_flags, h2n(0x1120));
	nasi_port_ptr = nasi_get_port_info(tty->ttynum);
	nasi_hdr->port_status = nasi_port_ptr->port_status;
	bcopy(&nasi_port_ptr->nasi_rx_baud, nasi_hdr->port_params, 11);
    } else {
	PUTSHORT(&nasi_hdr->ret_ax, h2n(0xf800));
	PUTSHORT(&nasi_hdr->ret_flags, h2n(0x19a0));
    }

    spx_api_send_data(spxcon, nasi_data, sizeofnasi_preq_data);
    if (nasi_activity_debug || nasi_pkt_debug) {
	buginf("\nNASI%t: %x sending Preq response: %d bytes %s",
	       (spxcon->tty)?spxcon->tty->ttynum:0,
	       spxcon->remote_con, sizeofnasi_preq_data,
	       (good) ? "Good" : "Bad");
    }
}

/*
 * nasi_rcvd_qname_ffirst
 *
 * we have received a query name Find first packet. Let's
 * process it..possibly returning a response.
 */
static void nasi_rcvd_qname_ffirst (spxcontype *spxcon, uchar *nasi_data,
				    paktype *pak)
{

    nasi_qport_hdr *nasi_qport = (nasi_qport_hdr *) nasi_data;
    int i;
    nasi_port *nasi_port_ptr = NULL;

    if (nasi_pkt_debug) {
	buginf("\nNASI%t: FFirst: line %x %x, port: %s, group: %s",
	       (spxcon->tty)?spxcon->tty->ttynum:0,
	       nasi_qport->port_num, nasi_qport->port_status,
	       nasi_qport->p_name, nasi_qport->g_name);
    }
    /*
     * Magic numbers to fake NASI pkts. See top for comment.
     */
    PUTSHORT(&nasi_qport->ret_ax, h2n(0x0));
    PUTSHORT(&nasi_qport->ret_cx, h2n(0x01000));
    PUTSHORT(&nasi_qport->ret_flags, h2n(0x011A2));
    PUTSHORT(&nasi_qport->ret_word1, h2n(0x0));
    PUTSHORT(&nasi_qport->ret_word2, h2n(0x0141));
    nasi_qport->zero1 = NASI_NULL_CHAR;

    strncpy(nasi_qport->srv_name, hostname, NASI_SRVSTRING_LEN);
    for (i = strlen(hostname); i < NASI_SRVSTRING_LEN; i++) {
	nasi_qport->srv_name[i] = NASI_NULL_CHAR;
    }
    nasi_qport->srv_name[NASI_SRVSTRING_LEN] = 0x0;

    strncpy(nasi_qport->sess_srv_name, hostname, NASI_SRVSTRING_LEN);
    for (i = strlen(hostname); i < NASI_LONGSTRING_SIZE; i++) {
	nasi_qport->sess_srv_name[i] = NASI_NULL_CHAR;
    }

    PUTSHORT(&nasi_qport->word_field4, 0x0);
    nasi_qport->zero =NASI_NULL_CHAR;

    i = nasi_port_name_to_num(nasi_qport->p_name);
    if (i >= 0) {
	if (0 == i) {
	    i = nasi_next_port(0);
	}
	if (i > 0) {
	    nasi_port_ptr = nasi_get_port_info(i);
	}
    }
    if (!nasi_port_ptr || !nasi_port_ptr->name || !*nasi_port_ptr->name) {
	PUTSHORT(&nasi_qport->ret_ax, h2n(0xfc00));
	nasi_qport->port_status = NASI_PORT_STATUS_ONHOLD;
	if (nasi_activity_debug || nasi_pkt_debug) {
	    buginf("\nNASI%t: %x sending Qport find-first "
		   " Bad response: %d bytes",
		   (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con, sizeofnasi_qport_hdr);
	}
	spx_api_send_data(spxcon, nasi_data, sizeofnasi_qport_hdr);
	return;
    }

    strncpy(nasi_qport->p_name, nasi_port_ptr->name, NASI_PORTSTRING_SIZE);
    for (i = strlen(nasi_port_ptr->name); i < NASI_PORTSTRING_SIZE; i++) {
	nasi_qport->p_name[i] = NASI_FILLER_CHAR;
    }

    strncpy(nasi_qport->g_name, nasi_port_ptr->group, NASI_GRPSTRING_SIZE);
    for (i = strlen(nasi_port_ptr->group); i < NASI_GRPSTRING_SIZE; i++) {
	nasi_qport->g_name[i] = NASI_FILLER_CHAR;
    }

    nasi_qport->port_num = nasi_port_ptr->num;

    nasi_qport->port_status = nasi_port_ptr->port_status;
    nasi_qport->inc_byte = 0xdf;
    bcopy(&nasi_port_ptr->nasi_rx_baud, nasi_qport->port_params, 11);

    if (nasi_activity_debug || nasi_pkt_debug) {
	buginf("\nNASI%t: %x sending Qport find-first response: %d bytes",
	       (spxcon->tty)?spxcon->tty->ttynum:0,
	       spxcon->remote_con, sizeofnasi_qport_hdr);
    }
    spx_api_send_data(spxcon, nasi_data, sizeofnasi_qport_hdr);
}


/*
 * nasi_rcvd_qname_fnext
 *
 * we have received a query name Find next packet. Let's
 * process it..possibly returning a response.
 */
static void nasi_rcvd_qname_fnext (spxcontype *spxcon, uchar *nasi_data,
				   paktype *pak)
{

    nasi_qport_hdr *nasi_qport = (nasi_qport_hdr *) nasi_data;
    int i;
    nasi_port *nasi_port_ptr = NULL;

    if (nasi_pkt_debug) {
	buginf("\nNASI%t: FNext: line %x %x, port: %s, group: %s",
	       (spxcon->tty)?spxcon->tty->ttynum:0,
	       nasi_qport->port_num, nasi_qport->port_status,
	       nasi_qport->p_name, nasi_qport->g_name);
    }

    /*
     * Magic numbers to fake NASI pkts. See top for comment.
     */
    PUTSHORT(&nasi_qport->ret_ax, h2n(0x0));
    PUTSHORT(&nasi_qport->ret_cx, h2n(0x01000));
    PUTSHORT(&nasi_qport->ret_flags, h2n(0x011A2));
    PUTSHORT(&nasi_qport->ret_word1, h2n(0x0));
    PUTSHORT(&nasi_qport->ret_word2, h2n(0x0141));
    nasi_qport->zero1 = NASI_NULL_CHAR;

    PUTSHORT(&nasi_qport->word_field4, 0x0);
    memset(nasi_qport->sess_cl_pw_hash, 0, NASI_PWSTRING_LEN);
    nasi_qport->zero =NASI_NULL_CHAR;

    i = nasi_port_name_to_num(nasi_qport->p_name);
    if (i >= 0) {
	i = nasi_next_port(i);
	if (i > 0) {
	    nasi_port_ptr = nasi_get_port_info(i);
	}
    }

    if (!nasi_port_ptr || !nasi_port_ptr->name || !*nasi_port_ptr->name) {
	PUTSHORT(&nasi_qport->ret_ax, h2n(0xfc00));
	if (nasi_activity_debug || nasi_pkt_debug) {
	    buginf("\nNASI%t: %x sending Qport find-next Bad response:"
		   " %d bytes", (spxcon->tty)?spxcon->tty->ttynum:0,
		   spxcon->remote_con, sizeofnasi_qport_hdr);
	}
	spx_api_send_data(spxcon, nasi_data, sizeofnasi_qport_hdr);
	return;
    }

    strncpy(nasi_qport->srv_name, hostname, NASI_SRVSTRING_LEN);
    for (i = strlen(hostname); i < NASI_SRVSTRING_LEN; i++) {
	nasi_qport->srv_name[i] = NASI_NULL_CHAR;
    }
    nasi_qport->srv_name[NASI_SRVSTRING_LEN] = 0x0;

    strncpy(nasi_qport->sess_srv_name, hostname, NASI_SRVSTRING_LEN);
    for (i = strlen(hostname); i < NASI_LONGSTRING_SIZE; i++) {
	nasi_qport->sess_srv_name[i] = NASI_NULL_CHAR;
    }

    strncpy(nasi_qport->p_name, nasi_port_ptr->name, NASI_PORTSTRING_SIZE);
    for (i = strlen(nasi_port_ptr->name); i < NASI_PORTSTRING_SIZE ; i++) {
	nasi_qport->p_name[i] = NASI_FILLER_CHAR;
    }

    strncpy(nasi_qport->g_name, nasi_port_ptr->group, NASI_GRPSTRING_SIZE);
    for (i = strlen(nasi_port_ptr->group); i < NASI_GRPSTRING_SIZE ; i++) {
	nasi_qport->g_name[i] = NASI_FILLER_CHAR;
    }

    nasi_qport->port_num = nasi_port_ptr->num;
    nasi_qport->port_status = nasi_port_ptr->port_status;
    nasi_qport->inc_byte = 0xdf;
    bcopy(&nasi_port_ptr->nasi_rx_baud, nasi_qport->port_params, 11);

    if (nasi_activity_debug || nasi_pkt_debug) {
	buginf("\nNASI%t: %x sending Qport find-next response: %d bytes",
	       (spxcon->tty)?spxcon->tty->ttynum:0,
	       spxcon->remote_con, sizeofnasi_qport_hdr);
    }
    spx_api_send_data(spxcon, nasi_data, sizeofnasi_qport_hdr);

}

/*
 * nasi_get_port_info
 *
 * return a pointer to a nasi_port structure. This is a globally
 * shared structure. And we use it with the ascertained assumption
 * that b/w the time we fill it and use it, no one else gets a chance
 * to mess with it.
 * 
 * the services in the NASI find-service (first/next) packets
 * should contain all the rotaries as the general names. All
 * the "line-x" as specific names. All the busy lines should
 * be skipped.
 */
static nasi_port *nasi_get_port_info (int port)
{

    /*
     * make sure we don't print more than NASI_GRPSTRING_SIZE bytes
     * onto group, and more than NASI_PORTSTRING_SIZE bytes onto name.
     */

    /*
     * start with last_port upto last tty line.
     * also put out one VTY line.
     */
    if ((port <= 0) || (port > ALLlines)) {
	return (NULL);
    }

    if (NASI_PORT_IN_USE(port)) {
	/*
	 * port is in use
	 */
	return (NULL);
    }

    switch (MODEMS[port]->type) {

     case TTY_LINE:
	sprintf(ret_port.group, "ASYNC");
	sprintf(temp_nasi_port_name_string, "TTY%u-%s", port,
		MODEMS[port]->location);
	break;

     case AUX_LINE:
	sprintf(temp_nasi_port_name_string, "AUX%u-%s", port,
		MODEMS[port]->location);
	sprintf(ret_port.group, "ASYNC");
	break;

     case VTY_LINE:
	sprintf(ret_port.group, "RCONSOLE");
	sprintf(temp_nasi_port_name_string, "VTY%u", port,
		MODEMS[port]->location);
	break;

     default:
	if (nasi_err_debug) {
	    buginf("\nNASI: bad port number %d in nasi_get_port_info",
		   port);
	}
	return (NULL);
    }

    ret_port.num = port;
    ret_port.port_status = NASI_PORT_STATUS_IDLE;
    strncpy(ret_port.name, temp_nasi_port_name_string, NASI_PORTSTRING_SIZE);
    nasi_get_port_params(&ret_port, port);
    return (&ret_port);
}

/*
 * nasi_next_port
 *
 * given a port number, skip until we find a viable NASI next port
 * for it.
 */
static int nasi_next_port (int last_port)
{

    int i;

    /*
     * start with last_port upto last tty line.
     * also put out one VTY line.
     */
    if ((last_port < 0) || (last_port >= ALLlines)) {
	if (nasi_err_debug) {
	    buginf("\nNASI: bad port number %d in nasi_next_port",
		   last_port);
	}
	return (-1);
    }
    for (i = last_port + 1; i <= nttylines; i++) {
	if (!NASI_PORT_IN_USE(i)) {
	    return (i);
	}
    }
    if (nauxlines && (last_port < AuxBase)) {
	i = AuxBase;
	if (!NASI_PORT_IN_USE(i)) {
	    return (i);
	}
    }
    if (nvtylines && (last_port < VTYBase)) {
	i = VTYBase;
	for (i= VTYBase; i < (VTYBase + nvtylines); i++) {
	    if (!NASI_PORT_IN_USE(i)) {
		return (i);
	    }
	}
    }

    return (-1);
}


/*
 * nasi_read_portnum
 *
 * read out the port number from the 2 digit string passed.
 */
static int nasi_read_portnum (uchar *port_numbers)
{
    /*
     * read a 2 digit number
     */
    int port_num = 0;
    uchar ch;

    /*
     * read the first digit
     * if there is any, then there might be another
     * one. Read it too.
     */
    ch = port_numbers[0];
    if (isdigit(ch)) {
	port_num = ch - '0';
	ch = port_numbers[1];
	if (isdigit(ch)) {
	    port_num = (port_num * 10) + (ch - '0');
	}
    }
    return ((port_num) ? port_num : -1);
}

/*
 * nasi_port_name_to_num
 *
 * for incoming port name, arrive at a port number from the
 * extracted port-type/number.
 */
static int nasi_port_name_to_num (uchar *port_name)
{

    int port_num;

    /*
     * We have a string of the form TTYxy or AUXxy or VTYxy
     * Read out the x  or xy both.
     */
    port_num = nasi_read_portnum(&port_name[3]);
    if (!strncmp("TTY", port_name, 3)) {
	if ((port_num > nttylines) || (port_num < 1)) {
	    port_num = -1;
	}
    } else if (!strncmp("AUX", port_name, 3)) {
	if ((port_num > (nauxlines + AuxBase)) || (port_num < AuxBase)) {
	    port_num = -1;
	}
    } else if (!strncmp("VTY", port_name, 3)) {
	if ((port_num > (nvtylines + VTYBase)) || (port_num < VTYBase)) {
	    port_num = -1;
	}
    } else if (!strncmp(NASI_ANYNAME12, port_name, 12)) {
	port_num = 0;
    } else {
	if (nasi_err_debug) {
	    buginf("\nNASI: Port name to num..bad port name: %s",
		   port_name);
	}
	port_num = -1;
    }
    /*if (nasi_activity_debug) {
	  buginf("\nNASI: Port name to num = %d", port_num);
	  }*/
    return (port_num);
}


/*
 * nasicon_setbuffers
 * Add wrap around queues for tty data I/O to NASI.
 */
static void nasicon_setbuffers (spxcontype *spxcon, int insize, int outsize)
{

    leveltype status;

    status = raise_interrupt_level(TTY_DISABLE);

    /*
     * Release current input buffer, if any, and set a new one, if any.
     */
    free(spxcon->nasi_instart);
    spxcon->nasi_instart = NULL;
    spxcon->nasi_insize = insize;
    if (insize > 0) {
	spxcon->nasi_instart = malloc_named(insize, "NASI Input Buf");
    }
    nasicon_clearinput(spxcon);

    /*
     * Release current output buffer, if any, and set a new one, if any.
     */
    free(spxcon->nasi_outstart);
    spxcon->nasi_outstart = NULL;
    spxcon->nasi_outsize = outsize;
    if (outsize > 0) {
	spxcon->nasi_outstart = malloc_named(outsize, "NASI Output Buf");
    }
    nasicon_clearoutput(spxcon);
    reset_interrupt_level(status);
}

/*
 * nasicon_puts
 *
 * we have some tty data to be written to the tty native. Let's
 * spit it out.
 */
static boolean nasicon_puts (spxcontype *spxcon, int size, uchar *data)
{

    if (!size) {
	if (nasi_err_debug) {
	    buginf("\nNASI%t: bad nasicon_puts NO bytes",
		   (spxcon->tty)?spxcon->tty->ttynum:0);
	}
	return (FALSE);
    }

    /*
     * we may have more data than what we can write
     */
    while (size--) {
	if (spxcon->nasi_intail >= spxcon->nasi_inend) {
	    spxcon->nasi_intail = spxcon->nasi_instart; /* Handle wraps */
	}

	if ((spxcon->nasi_intail + 1) == spxcon->nasi_inhead) {
	    spxcon->nasi_inoverflow++;
	    if (nasi_err_debug) {
		buginf("\nNASI%t: overflow of incoming data %x",
		       (spxcon->tty)?spxcon->tty->ttynum:0,
		       spxcon->nasi_intail);
	    }
	    continue;
	}
	*spxcon->nasi_intail++ = *data++; /* Store character */
	++spxcon->nasi_incount;	          /* Count it */
    }
    return (TRUE);
}


static void nasi_get_port_params (nasi_port *port_info, int port_num)
{
    tt_soc *tty = MODEMS[port_num];
    modem_status modemsignals;

    port_info->nasi_rx_baud = tty2nasi_speed(tty->tty_rxspeed);
    port_info->nasi_rx_wlen = tty2nasi_databits(tty->tty_rxspeed);
    port_info->nasi_rx_stopbits = tty2nasi_stopbits(tty->tty_stopbits);
    port_info->nasi_rx_parity = tty2nasi_parity(tty->tty_parity);

    port_info->nasi_tx_baud = tty2nasi_speed(tty->tty_txspeed);
    port_info->nasi_tx_wlen = tty2nasi_databits(tty->tty_txspeed);
    port_info->nasi_tx_stopbits = tty2nasi_stopbits(tty->tty_stopbits);
    port_info->nasi_tx_parity = tty2nasi_parity(tty->tty_parity);

    modemsignals = tty_modemsignal(tty);
    port_info->nasi_dtr_state = modemsignals.dtr;
    port_info->nasi_rts_state = modemsignals.rts;
    port_info->nasi_flow_type = tty2nasi_flow(tty->capabilities);

}

static inline uchar nasi_table_search (const int table_name[],
				       int table_max_index, int min_value,
				       int max_value, int search_value)
{
    int i;

    if (search_value < min_value) {
	search_value = min_value;
    } else if (search_value > max_value) {
	search_value = max_value;
    }
    for (i=0; i < table_max_index; i++) {
	if (table_name[i] == search_value) {
	    return (i);
	}
    }
    return (0);
}

static inline uchar tty2nasi_speed (int speed)
{

    return (nasi_table_search(NASI_SPEED_TABLE, NASI_MAX_SPEED_INDEX,
			      NASI_MIN_TTY_SPEED, NASI_MAX_TTY_SPEED,
			      speed));
}

static inline uchar tty2nasi_databits (int databits)
{

    return (nasi_table_search(NASI_WORDLEN_TABLE, NASI_MAX_WORDLEN_INDEX,
			      NASI_MIN_TTY_WORDLEN, NASI_MAX_TTY_WORDLEN,
			      databits));
}

static inline uchar tty2nasi_stopbits (int stopbits)
{

    return (nasi_table_search(NASI_STOPBITS_TABLE, NASI_MAX_STOPBITS_INDEX,
			      NASI_MIN_TTY_STOPBITS, NASI_MAX_TTY_STOPBITS,
			      stopbits));
}

static inline uchar tty2nasi_parity (int parity)
{

    return (nasi_table_search(NASI_PARITY_TABLE, NASI_MAX_PARITY_INDEX,
			      NASI_MIN_TTY_PARITY, NASI_MAX_TTY_PARITY,
			      parity));
}

static inline uchar tty2nasi_flow (ulong cap_word)
{
    if (cap_word & (SFLOWIN | SFLOWOUT)) {
	return (NASI_FLOW_SOFT);
    } else if (cap_word & (HFLOWIN | HFLOWOUT)) {
	return (NASI_FLOW_HARD);
    } else {
	return (NASI_FLOW_NONE);
    }
}


/*
 * nasi_getc
 * Get a character from NASI.  Returns -1 if none available.
 */

static int nasi_getc (tt_soc *tty)
{
    spxcontype *spxcon;
    int ch;
    int status;

    /*
     * First check if connection is still alive.
     */
    spxcon = tty2spxcon(tty, TRUE);
    if (NULL == spxcon) {
	return(-1);
    }

    /*
     * Check for saved character.
     */
    if (spxcon->nasi_isavedchar != -1) {
	ch = spxcon->nasi_isavedchar;
	spxcon->nasi_isavedchar = -1;
	return(ch);
    }

    /*
     * Nothing saved, try protocol driver.
     */
    while (TRUE) {			/* Loop fetching characters */
	ch = nasi_getbyte(spxcon, &status); /* get next raw input byte */
        if (-1 == ch) {			/* nothing there....  */
	    return(-1);
	}

	/*
	 * Preserve any status bits that may have meaning.  This code is
	 * slightly faster than checking for the status bits, but it does
	 * require that the MSB of the low byte never have meaning, since
	 * that bit can get masked out even when there are status bits.
	 */
	ch &= (0xFFFFFF00 | (int)tty->tty_datamask); /* Trim any parity bits */

	if (VTY_LINE == tty->type) {
	    GET_TIMESTAMP(tty->conntop->idletime); /* 0 idle time */
	    if (special_chars(tty,ch) < 0) { /* Check for specials */
		continue;
	    }
	}
	if (tty->statbits & LASTWASBREAK) {
	    tty->statbits &= ~LASTWASBREAK;
	    return(BREAK_CHAR);
	}
	if (tty->statbits & LASTWASSPEC) {
	    tty->statbits &= ~LASTWASSPEC;
	    return(ch | ESCAPED_FLAG);
	}
	return(ch);
    }
    return (-1);
}

/*
 * nasi_getbyte
 *
 * read one byte from the network.
 */
static int nasi_getbyte(spxcontype *spxcon, int *size)
{

    uchar i;

    if (0 == spxcon->nasi_incount) {
	return (-1);
    }

    i = *spxcon->nasi_inhead++;
    if (spxcon->nasi_inhead >= spxcon->nasi_inend) {
	spxcon->nasi_inhead = spxcon->nasi_instart;
    }
    atomic_decrement(&spxcon->nasi_incount);

    return((uint) i);
}

/*
 * nasiputBLOCK
 *
 * Block until nasi output buffer is empty.
 * Note that some of our callers *require* the output buffer be empty!
 */
static boolean nasiputBLOCK (spxcontype *spxcon)
{
    if (spx_api_con_closing(spxcon)) {
	return FALSE;
    }
    if (spxcon->tty && (spxcon->tty->statbits & (CARDROP|IDLE))) {
	return(FALSE);
    }
    if (spxcon->tty && TIMER_RUNNING_AND_AWAKE(spxcon->tty->dallytimer)) {
	spxcon->tty->statbits |= (IDLETIMEOUT+CARDROP);
	return(FALSE);
    }
    return (spxcon->nasi_outcount ? TRUE : FALSE);
}

/*
 * nasi_putc
 * Write a byte to a NASI connection
 */
static void nasi_putc (tt_soc *tty, int ch)
{
    spxcontype *spxcon;

    /*
     * Experimental code to improve milking machine performance
     */
    if (tty->tty_dispatchtimeout && (tty->type != VTY_LINE) &&
	(tty->tty_dispatchtimeout > 10)) {
	conntype *conn = tty->conntop;
	if (!TIMER_RUNNING(conn->dispatchtimer)) {
	    TIMER_START(conn->dispatchtimer, tty->tty_dispatchtimeout);
	}
    }
    spxcon = tty2spxcon(tty, FALSE);
    if (NULL == spxcon) {
	return;
    }
    ch &= (0xFFFFFF00 | (int)tty->tty_datamask); /* Trim any parity bits */
    (void) nasi_putbyte(spxcon, ch);
}

/*
 * nasi_putbyte
 *
 * write a byte to the network
 */
static void nasi_putbyte (spxcontype *spxcon, int ch)
{
    uchar *t;
    leveltype status;

    status = raise_interrupt_level(TTY_DISABLE); /* Block TTY interrupts */

    t = spxcon->nasi_outtail + 1;
    if (t == spxcon->nasi_outend) {
	t = spxcon->nasi_outstart;
    }

    if (t == spxcon->nasi_outhead) {

	while ((t == spxcon->nasi_outhead)) {
	    reset_interrupt_level(status);
	    if (spxcon->tty) {
		tty_dallytimer(spxcon->tty);
	    }
            edisms((blockproc *)nasiputBLOCK, (ulong)spxcon);
	    if (spx_api_con_closing(spxcon) ||
		(spxcon->tty && 
		 spxcon->tty->statbits & (CARDROP|IDLE|IDLETIMEOUT))) {
	        return;
	    }
	    raise_interrupt_level(TTY_DISABLE);
	    t = spxcon->nasi_outtail + 1;
	    if (t == spxcon->nasi_outend) {
		t = spxcon->nasi_outstart;
	    }
	}
    }

    *t = (uchar) ch;		        /* store character */
    spxcon->nasi_outtail = t;		/* Set new tail pointer */
    spxcon->nasi_outcount++;		/* Count another character */

    reset_interrupt_level(status);	/* Drop priority */
}


/*
 * nasi_inputpending
 *
 * Return the number of unread bytes
 */
static int nasi_inputpending (tt_soc *tty)
{
    spxcontype *spxcon;
    int count;

    spxcon = tty2spxcon(tty, TRUE);
    if (NULL == spxcon) {
	return (0);
    }

    count = spxcon->nasi_incount;
    return(count);
}


/*
 * nasi_clearinput
 *
 * Flush the NASI input queues.
 */
static void nasi_clearinput (tt_soc *tty)
{
    spxcontype *spxcon;

    spxcon = tty2spxcon(tty, FALSE);
    if (NULL == spxcon) {
	return;
    }
    nasicon_clearinput(spxcon);
}

/*
 * nasi_outputpending
 *
 * Return the number of bytes to be written to the net.
 */
static int nasi_outputpending (tt_soc *tty)
{
    spxcontype *spxcon;

    spxcon = tty2spxcon(tty, FALSE);
    if (NULL == spxcon) {
	return(0);
    }
    return (spxcon->nasi_outcount);
}

/*
 * nasi_clearoutput
 *
 * clear any pending NASI output  (data to be written to network.
 */
static void nasi_clearoutput (tt_soc *tty)
{
    spxcontype *spxcon;

    spxcon = tty2spxcon(tty, FALSE);
    if (NULL == spxcon) {
	return;
    }
    nasicon_clearoutput(spxcon);
}

/*
 * nasi_setsflow
 *
 * set software flow for the network link to the client
 * this is not applicable to NASI.
 */
static void nasi_setsflow (tt_soc *tty)
{
}

/*
 * nasicon_output
 *
 * read data from tty native and write to the network
 * called from nasi_main
 */
static void nasicon_output (spxcontype *spxcon)
{
    int count = spxcon->nasi_outcount;
    uchar *cp = spxcon->nasi_outhead + 1;
    int bytes_till_end;


    bytes_till_end = spxcon->nasi_outend - cp;
    
    if (count > bytes_till_end) {
	count = bytes_till_end;	              /* Avoid buffer wraps */
	if (!count) {	                      /* At the buffer end? */
	    cp = spxcon->nasi_outstart;       /* Yes, go to start */
	    spxcon->nasi_outhead = cp - 1;    /* And fix the head pointer */
	    count = spxcon->nasi_outcount;    /* Get back the count */
	}
    }
    if (count > NASI_MAX_SEND) {
	count = NASI_MAX_SEND;	              /* NASI max packet size */
    }
    spxcon->nasi_outcount -= count;	      /* Update count */
    spxcon->nasi_outhead += count;            /* And the header pointer */
    if (1 == count) {
	nasi_sendbyte(spxcon, *cp);
    } else {
	nasi_send_data_pkt(spxcon, count, cp);
    }
}

/*
 * nasicon_clearinput
 *
 * clear any NASI data to be written to the tty native
 */
static void nasicon_clearinput (spxcontype *spxcon)
{
    leveltype status;

    if (NULL == spxcon->nasi_instart) {
	return;
    }
    status = raise_interrupt_level(TTY_DISABLE);
    spxcon->nasi_inend = spxcon->nasi_instart + spxcon->nasi_insize;
    spxcon->nasi_inhead = spxcon->nasi_instart;
    spxcon->nasi_intail = spxcon->nasi_instart;
    spxcon->nasi_incount = 0; 
    spxcon->nasi_isavedchar = -1;
    reset_interrupt_level(status);
}

/*
 * nasicon_clearoutput
 *
 * clear any NASI data to be written to the network
 */
static void nasicon_clearoutput (spxcontype *spxcon)
{
    leveltype status;

    if (NULL == spxcon->nasi_outstart){
	return;
    }
    status = raise_interrupt_level(TTY_DISABLE);
    spxcon->nasi_outend = spxcon->nasi_outstart + spxcon->nasi_outsize;
    spxcon->nasi_outhead = spxcon->nasi_outstart;
    spxcon->nasi_outtail = spxcon->nasi_outstart;
    spxcon->nasi_outcount = 0;
    reset_interrupt_level(status);
}

/*
 * nasi_initport_rcvd
 *
 * client wants to grab this port with the supplied parameters or
 * wants to modify allocated port, with the supplied parameter.
 * We don't do the later yet.
 */
static void nasi_initport_rcvd (spxcontype *spxcon, uchar *nasi_data,
				paktype *pak)
{

    tt_soc *tty = spxcon->tty;
    int line;
    nasi_preq *nasi_hdr = (nasi_preq *) nasi_data;
    boolean proceed = TRUE;

    if (tty) {
	if (nasi_activity_debug) {
	    buginf("\nNASI%t: REPEAT request to initialize port",
		   (spxcon->tty)?spxcon->tty->ttynum:0);
	}
	nasi_modify_port(spxcon, nasi_hdr);
	nasi_send_server_ack(spxcon, (nasi_hdr3 *)nasi_data, 0, TRUE);
	process_sleep_for(NASI_INITPORT_DELAY);
	nasi_send_server_preq_response(spxcon, nasi_data, tty,
				       TRUE);
	process_sleep_for(NASI_INITPORT_DELAY);
	return;
    }

    if (nasi_activity_debug) {
	buginf("\nNASI%t: port INIT: name: %s",
	       (spxcon->tty)?spxcon->tty->ttynum:0, nasi_hdr->p_name);
    }
    line = nasi_port_name_to_num(nasi_hdr->p_name);
    if (nasi_activity_debug) {
	buginf("\n\nNASI%t: port INIT request for line %d\n",
	       (spxcon->tty)?spxcon->tty->ttynum:0, line);
    }
    if (line != nasi_hdr->port_num) {
	if (nasi_err_debug) {
	    buginf("\nNASI%t: Init port has conflicting port identifiers",
		   (spxcon->tty)?spxcon->tty->ttynum:0);
	}
    }
    if (nasi_activity_debug) {
	buginf("\nNASI%t: Port Num in request is: %x %d",
	       (spxcon->tty)?spxcon->tty->ttynum:0, line, line);
    }
    if ((line <=0) || (line > ALLlines)) {
	proceed = FALSE;
	if (nasi_err_debug) {
	    buginf("\nNASI%t: initport  bad line number",
		   (spxcon->tty)?spxcon->tty->ttynum:0);
	}
    }

    if (proceed && (NULL == (tty = tty_allocate(line, TRUE)))) {
	proceed = FALSE;
	if (nasi_err_debug) {
	    buginf("\nNASI%t: failed to allocate port",
		   (spxcon->tty)?spxcon->tty->ttynum:0);
	}
    }

    if (proceed && !nasi_check_auth(tty, nasi_hdr)) {
	proceed = FALSE;
	if (nasi_err_debug) {
	    buginf("\nNASI%t: failed authentication",
		   (spxcon->tty)?spxcon->tty->ttynum:0);
	}
    } else {
	if (nasi_err_debug) {
	    buginf("\nNASI%t: authentication succeeded",
		   (spxcon->tty)?spxcon->tty->ttynum:0);
	}
    }

    if (proceed && !start_nasivty(tty, spxcon)) {
	proceed = FALSE;
	if (tty->ustruct) {
	    aaa_free_user(&tty->ustruct);
	}
    }

    if ((!proceed) || (!tty)) {
	/*
	 * send back a negative port req ack
	 */
	if (nasi_err_debug) {
	    buginf("......FAILED");
	}
	if (tty) {
	    tty_deallocate(tty);
	}
	nasi_send_server_ack(spxcon, (nasi_hdr3 *)nasi_data, 0, FALSE);
	process_sleep_for(NASI_INITPORT_DELAY);
	nasi_send_server_preq_response(spxcon, nasi_data, NULL,
				       FALSE);
	nasi_send_close_pkt(spxcon);
	return;
    }

    if (nasi_err_debug) {
	buginf(".....OK");
    }
    spxcon->tty = tty;				/* point back to tty */
    nasi_send_server_ack(spxcon, (nasi_hdr3 *)nasi_data, 0, TRUE);
    process_sleep_for(NASI_INITPORT_DELAY);
    nasi_send_server_preq_response(spxcon, nasi_data, tty, TRUE);
    process_sleep_for(NASI_INITPORT_DELAY);
    if (nasi_activity_debug) {
	buginf("\nNASI daemon running on line %t", spxcon->tty->ttynum);
    }
}

/*
 * nasi_send_data_pkt
 *
 * NASI interface to spx_api_send_data  for tty data output
 * back to the client.
 */
static boolean nasi_send_data_pkt (spxcontype *spxcon, int size,
				   uchar *data)
{
    nasi_hdr3 *nasi_spkt = (nasi_hdr3 *) nasi_tty_op_data;
    boolean ret_code;

    /*
     * the buffer we use is immediately copied
     * out of SPX, so it is OK to re-use the statically
     * allocated buffer.
     */
    PUTSHORT(&nasi_spkt->code_subcode, h2n(NASI_OP_SRV_DATA));
    nasi_spkt->size = size;
    /*
     * Magic numbers to fake NASI pkts. See top for comment.
     */
    nasi_spkt->field1 = 0x01;
    nasi_spkt->field3 = 0x01;
    nasi_spkt->field4 = 0x20;
    nasi_spkt->field5 = 0x01;
    nasi_spkt->field6 = 0x40;
    bcopy(data, &nasi_spkt->data, size);
    /*	buginf("\nNASI%t: %x sending NCS server TTY data: %d byte"
	" in %d size pkt", (spxcon->tty)?spxcon->tty->ttynum:0,
	spxcon->remote_con, size, sizeofnasi_hdr3+size);*/
    ret_code = spx_api_send_data(spxcon, (uchar *) nasi_spkt,
				 sizeofnasi_hdr3+size);
    return (SPX_RET_OK == ret_code);
}

/*
 * nasi_send_close_pkt
 *
 * we want to close the connection. Send the Terminate pkt.
 */
static void nasi_send_close_pkt (spxcontype *spxcon)
{

  /* 4200, 0000, 1000, 0924, 0000, 0140, 00 */

    static nasi_hdr3 nasi_cpkt;

    memset(&nasi_cpkt, 0, sizeofnasi_hdr3);
    PUTSHORT(&nasi_cpkt.code_subcode, h2n(NASI_OP_SRV_DISCONNECT));
    /*
     * Magic numbers to fake NASI pkts. See top for comment.
     */
    nasi_cpkt.field1 = 0x10;
    nasi_cpkt.field3 = 0x09;
    nasi_cpkt.field4 = 0x24;
    nasi_cpkt.field5 = 0x01;
    nasi_cpkt.field6 = 0x40;

    if (nasi_activity_debug || nasi_pkt_debug) {
	buginf("\nNASI%t: %x sending NCS server Disconnect in %d"
	       " bytes pkt", (spxcon->tty)?spxcon->tty->ttynum:0,
	       spxcon->remote_con, sizeofnasi_hdr3);
    }
    spx_api_send_data(spxcon, (uchar *) &nasi_cpkt, sizeofnasi_hdr3);

}

/*
 * nasi_rcvd_close
 *
 * we have received a close pkt.
 * we leave it for SPX to notify us through con_closing
 */
static void nasi_rcvd_close (spxcontype *spxcon)
{
    /* should change NASI state to closing */
}

/*
 * nasi_rcvd_flush_rxbuf
 *
 * client thinks we need to flush receive buffers
 */
static void nasi_rcvd_flush_rxbuf (spxcontype *spxcon)
{
    /*
     * There are no replies to this packet.
     *
     * If we have a tty connection
     *   then flush all data destined to be sent out
     *   to the net (client).
     *   Means clear the output buffer.
     * Does not clear data queued for sending on this
     *   connection.
     */
    nasicon_clearoutput(spxcon);
}

/*
 * nasi_rcvd_flush_txbuf
 *
 * client thinks we need to flush transmit buffers
 */
static void nasi_rcvd_flush_txbuf (spxcontype *spxcon)
{
    /*
     * There are no replies to this packet.
     *
     * If we have a tty connection
     *   then flush all data destined to be sent out
     *   to the tty (native).
     * Does not clear the data in the native (hw) driver.
     * Does not flush unprocessed pkts on this connection.
     */
    nasicon_clearinput(spxcon);
}

/*
 * nasi_rcvd_set_service_name
 *
 * client wants to set a new service name
 * ideally a last service name should be kept, but the
 * client sends this again anyway in the init_port, so we
 * get away without doing it.
 */
static void nasi_rcvd_set_service_name (spxcontype *spxcon,
					uchar *nasi_data)
{

    /*
     * We don't maintain the last service name
     * for sessions. We will just pretend that we
     * did.
     */
    nasi_send_setget_service_response(spxcon, nasi_data, FALSE,
				      NASI_OP_SET_SERVICE_NAME,
				      NULL, NULL);
}

/*
 * nasi_rcvd_get_service_name
 *
 * the client wants to get the last service name specified.
 */
static void nasi_rcvd_get_service_name (spxcontype *spxcon,
					uchar *nasi_data)
{

    /*
     * We don't maintain the last service name
     * for sessions. We will just pretend that we
     * did.
     */
    nasi_send_setget_service_response(spxcon, nasi_data, TRUE,
				      NASI_OP_SET_SERVICE_NAME,
				      NULL, NULL);
}

/*
 * nasi_rcvd_get_general_name
 *
 * the client wants to get the general name of the port
 * last specified.
 */
static void nasi_rcvd_get_general_name (spxcontype *spxcon,
					uchar *nasi_data)
{

    /*
     * We don't maintain the last service name
     * for sessions. We will just pretend that we
     * did.
     */
    nasi_send_setget_service_response(spxcon, nasi_data, TRUE,
				      NASI_OP_SET_SERVICE_NAME,
				      NULL, NULL);
}

/*
 * nasi_rcvd_get_specific_name
 *
 * the client wants to get the specific name of the port
 * last specified.
 */
static void nasi_rcvd_get_specific_name (spxcontype *spxcon,
					 uchar *nasi_data)
{

    /*
     * We don't maintain the last service name
     * for sessions. We will just pretend that we
     * did.
     */
    nasi_send_setget_service_response(spxcon, nasi_data, TRUE,
				      NASI_OP_SET_SERVICE_NAME,
				      NULL, NULL);
}

/*
 * nasi_send_setget_service_response
 *
 * our generic response to the above 4 packet types.
 */
static void nasi_send_setget_service_response (spxcontype *spxcon,
					       uchar *nasi_data,
					       boolean good,
					       ushort code,
					       uchar *server_name,
					       uchar *service_name)
{
    nasi_preq *nasi_hdr = (nasi_preq *) nasi_data;

    memset(nasi_hdr, 0, sizeofnasi_preq_info);
    PUTSHORT(&nasi_hdr->code_subcode, code);
    /*
     * Magic numbers to fake NASI pkts. See top for comment.
     */
    PUTSHORT(&nasi_hdr->ret_ax, h2n((good) ? (0x0004) : (0xfc00)));
    PUTSHORT(&nasi_hdr->ret_cx, h2n(0x1000));
    PUTSHORT(&nasi_hdr->ret_flags, h2n(0x1120));
    PUTSHORT(&nasi_hdr->ret_word2, h2n(0x0141));

    spx_api_send_data(spxcon, nasi_data, sizeofnasi_preq_data);
    if (nasi_activity_debug || nasi_pkt_debug) {
	buginf("\nNASI%t: %x sending setget Service response: %d"
	       " bytes %s", (spxcon->tty)?spxcon->tty->ttynum:0,
	       spxcon->remote_con, sizeofnasi_preq_data,
	       (good) ? "Good" : "Bad");
    }
}

/*
 * nasi_break_rcvd
 *
 * the client wants us to send a break on the tty line.
 * we don't do this yet.
 */
static void nasi_break_rcvd (spxcontype *spxcon, boolean sense)
{
    if (spxcon->tty) {
	if (sense) {
	    if (nasi_activity_debug) {
		buginf("\nNASI%t: %x sending break on line %t",
		       (spxcon->tty)?spxcon->tty->ttynum:0, spxcon,
		       spxcon->tty->ttynum);
	    }
	} else {
	    if (nasi_activity_debug) {
		buginf("\nNASI%t: %x stop sending break on line %t",
		       (spxcon->tty)?spxcon->tty->ttynum:0, spxcon,
		       spxcon->tty->ttynum);
	    }
	}
    }
}


/*
 * nasi_sflow_rcvd
 *
 * the client wants us to set SW flow control on the line
 * we don't do this yet, either.
 */
static void nasi_sflow_rcvd (spxcontype *spxcon, boolean sense)
{
    if (spxcon->tty) {
	if (sense) {
	    if (nasi_activity_debug) {
		buginf("\nNASI%t: %x setting SW flow control on line %t",
		       (spxcon->tty)?spxcon->tty->ttynum:0,
		       spxcon, spxcon->tty->ttynum);
	    }
	} else {
	    if (nasi_activity_debug) {
		buginf("\nNASI%t: %x resetting SW flow control on line %t",
		       (spxcon->tty)?spxcon->tty->ttynum:0,
		       spxcon, spxcon->tty->ttynum);
	    }
	}
    }
}

/*
 * nasi_def_sflow_rcvd
 *
 * the client wants to define XON/XOFF chars of its own choice
 * not implemented yet
 */
static void nasi_def_sflow_rcvd (spxcontype *spxcon, boolean sense)
{
    if (spxcon->tty) {
	if (sense) {
	    if (nasi_activity_debug) {
		buginf("\nNASI%t: %x defining SFLOW characters on"
		       " line %t",
		       (spxcon->tty)?spxcon->tty->ttynum:0,
		       spxcon, spxcon->tty->ttynum);
	    }
	}
    }
}

/*
 * nasi_modify_port
 *
 * the client wants to modify port parameters
 * not implementedy yet
 */
static void nasi_modify_port (spxcontype *spxcon, nasi_preq *nasi_hdr)
{
}

/*
 * nasi_check_auth
 *
 * check the username/password pair received from client
 */
static boolean nasi_check_auth (tt_soc *tty, nasi_preq *nasi_hdr)
{

    int i;
    uchar pw_array[NASI_PWSTRING_LEN + 1];
    char *reason;

    if (nasi_pkt_debug) {
	buginf("\nNASI: Check-login User: ");
	for (i = 0; i < NASI_USTRING_LEN; i++) {
	    buginf("%c ", nasi_hdr->sess_uname2[i]);
	}
	buginf("\nNASI: Check-login PW hash: ");
	for (i = 0; i < NASI_PWSTRING_LEN; i++) {
	    buginf("%x ", nasi_hdr->sess_pw_hash[i]);
	}
    }
    nasi_decrypt_pw(nasi_hdr->sess_pw_hash, pw_array);
    if (nasi_pkt_debug) {
	buginf("\nNASI: Check-login PW: ");
	for (i = 0;i < NASI_PWSTRING_LEN; i++) {
	    buginf("%c ", pw_array[i]);
	}
    }

    if (old_access_control) {
	return (reg_invoke_login_login(tty, nasi_hdr->sess_uname2,
				       pw_array, &reason));
    } else {
	return (aaa_simple_login_check(tty, nasi_hdr->sess_uname2,
				       pw_array, NULL, TRUE, AAA_SVC_NASI));
    }
}

/*
 * nasi_decrypt_pw
 *
 * decrypt the hashed password to get plain ascii
 */
static void nasi_decrypt_pw (uchar *in_char, uchar *out_char)
{
    int i;

    out_char[0] = in_char[0] ^ NASI_PW_HASH_BYTE;
    for (i = 1; i < NASI_PWSTRING_LEN; i++) {
	out_char[i] = in_char[i] ^ in_char[i-1] ^ (i-1);
    }
}

/*
static void nasi_encrypt_pw (uchar *in_char, uchar *out_char)
{
    int i;

    out_char[0] = in_char[0] ^ NASI_PW_HASH_BYTE;
    for (i = 1; i < NASI_PWSTRING_LEN; i++) {
	out_char[i] = in_char[i] ^ out_char[i-1] ^ (i-1);
    }
}
*/


/*
 * NASI Server subsystem header
 */
#define NASI_SERVER_MAJVERSION 1
#define NASI_SERVER_MINVERSION 0
#define NASI_SERVER_EDITVERSION  1

SUBSYS_HEADER(nasi, NASI_SERVER_MAJVERSION, NASI_SERVER_MINVERSION,
	      NASI_SERVER_EDITVERSION,
	      nasi_server_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: ipx, spx", "req: ipx, spx");
