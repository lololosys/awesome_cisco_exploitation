/* $Id: netbios_as.c,v 3.10.8.4 1996/08/28 12:46:57 thille Exp $
 * $Source: /release/112/cvs/Xsys/ibm/netbios_as.c,v $
 *------------------------------------------------------------------
 * netbios_as.c - NBF (NetBIOS Frames or NETBEUI) Access Server Support
 *
 * September 1995, Shoou Yiu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: netbios_as.c,v $
 * Revision 3.10.8.4  1996/08/28  12:46:57  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.8.3  1996/07/12  16:02:42  swenson
 * CSCdi62535:  netbios_as buginf doesnt check if debug on
 * Branch: California_branch
 *
 * Revision 3.10.8.2  1996/05/17  11:20:52  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.9.6.5  1996/05/05  23:08:23  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.9.6.4  1996/04/03  14:34:11  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.9.6.3  1996/03/20  17:54:13  ppearce
 * IBU modularity - NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Code review comment - name changes only
 *
 * Revision 3.9.6.2  1996/03/17  17:50:30  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.9.6.1  1996/03/16  12:09:19  ppearce
 * CSCdi50498:  IBU modularity - Implement NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Move NetBios fields from hwidbtype/idbtype structs to NetBios subblock
 *
 * Revision 3.10.8.1  1996/03/18  20:11:42  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.14.4  1996/03/16  06:53:13  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.8.14.3  1996/03/13  01:23:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.8.14.2  1996/03/07  09:42:08  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8.14.1  1996/02/20  00:54:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/03/01  00:48:51  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up correctly
 * Restore CSCdi47275 w/fix to netbios_as.c
 *
 * Revision 3.9  1996/02/01  06:03:10  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.8  1995/12/12  06:09:26  gschleic
 * CSCdi44259:  Netbios name-cache timeout parses incorrectly
 *
 * Revision 3.7  1995/12/07  03:09:47  syiu
 * CSCdi45173:  NetBIOS over async table in use
 *
 * Revision 3.6  1995/12/06  19:09:04  syiu
 * CSCdi45122:  NetBIOS over async sometimes not going to LLC2
 *
 * Revision 3.5  1995/11/30  02:59:48  syiu
 * CSCdi44150:  Remote Node NetBEUI on Token Ring platform not working
 *
 * Revision 3.4  1995/11/18  20:49:35  syiu
 * CSCdi43793:  NetBIOS async to async using NBFCP not working
 *
 * Revision 3.3  1995/11/17  09:23:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:35:39  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:45:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:02:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  00:31:21  syiu
 * Initial placeholder for NBF.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "fastswitch_registry.h"
#include "../h/cs_registry.regh"
#include "interface_private.h"
#include "ieee.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "address.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "ttysrv.h"
#include "../wan/nbfcp.h"
#include "netbios.h"
#include "netbios_sb.h"
#include "../llc2/llc2.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/rif.h"
#include "parser_defs_netbios.h"
#include "../if/network.h"
#include "../srt/srb.h"
#include "../srt/rsrb.h"
#include "../ui/debug.h"
#include "netbios_as_debug.h"
#include "netbios_as.h"
#include "netbios_as_inline.h"


/*
 * Global Storage and forward references
 */
static queuetype netbios_name_queryQ;
static queuetype netbios_name_recognizedQ;
static queuetype netbios_session_initializeQ;
static queuetype netbios_pending_session_initializeQ;
static queuetype netbios_add_nameQ;
static queuetype netbios_status_queryQ;
static queuetype netbios_status_responseQ;
static watched_queue *NBFpacketQ;
queuetype *netbios_lan_hash_table_ptr;

/*
 * Netbios caching subsystem header
 */

#define NBF_MAJVERSION 1
#define NBF_MINVERSION 0
#define NBF_EDITVERSION  1

/* 
 * Function prototypes in this module 
 */
static void nbf_enqueue(paktype *);
static process nbf_input(void);
static void nbf_process_input(void);
static void nbf_housekeeping(void); 
static void nbf_pakout(paktype *,idbtype *,uchar *);
boolean nbf_netbios_sniff(uchar *);
static boolean nbf_prep_interface(idbtype *);
static boolean nbf_netbios_processing(paktype *, netbios_header *, int, uchar *);
static boolean nbf_nbfcp_processing(paktype *, netbios_header *, int);
static void nbf_forward_pakout(paktype *, uchar *);
static boolean nbf_check_and_enqueue(paktype *, int, boolean);
static paktype *nbf_check_and_dequeue(paktype *, netbios_header *);
static void netbios_as_if_state_change(hwidbtype *hwidb);
static void nbf_llc2_link_state_change(llctype *, int);
static void nbf_register (llctype *);
void nbf_xmit_netbios_session_end(uchar, uchar, uchar *, idbtype *);
static void nbf_cleanup_llc(llctype *);
static void nbf_llc2_open(paktype *, int, hwaddrtype *);
static void nbf_flush_queue(queuetype *);
static void nbf_action_pending_session_init(llctype *, boolean);
void nbf_xmit_netbios_session_end(uchar, uchar, uchar *, idbtype *);
static netbios_nbfcp_name_proj_entry *nbf_name_proj_get_next(netbios_nbfcp_name_proj_entry *, int);
static netbios_nbfcp_name_proj_entry *nbf_name_proj_search(netbios_nbfcp_name_proj_entry *, int, uchar *);
static paktype *nbf_create_netbios_add_name_query(idbtype *, uchar *, uchar);

/*
 * External references
 */
extern void nbf_media_registry(void);
extern void netbios_as_parser_init(void);
extern int nbf_llc_sniff(uchar *, paktype *);
extern boolean nbf_netbios_sniff(uchar *);
extern boolean nbf_insert_sess_entry(hwidbtype *, hwidbtype *, uchar *, uchar *, llctype *, llctype *, netbios_header_sess *); 
extern boolean nbf_delete_sess_entry(hwidbtype *, hwidbtype *, uchar *, uchar *, netbios_header_sess *);
extern void nbf_reset_session_table(hwidbtype *, boolean, boolean, boolean);
extern void nbf_reset_lan_hash_table(hwidbtype *, boolean, boolean, boolean);
extern void nbf_delete_all_buckets(int, queuetype *, idbtype *idb, boolean, boolean);
extern boolean nbf_free_lan_sess_entry(int, uchar, uchar, hwidbtype *, hwidbtype *, uchar *, boolean);
extern int nbf_hash_table_entries(idbtype *);
extern boolean nbf_insert_access_sess_entry(hwidbtype *, hwidbtype *, uchar, uchar, uchar *);
extern netbios_lan_sess_entry *nbf_insert_lan_entry(hwidbtype *, hwidbtype *,uchar, uchar, uchar *, uchar *);
extern void display_netbios_interfaces(void);
extern void display_netbios_sess_entry(hwidbtype *, uchar,netbios_sess_entry *);
extern void display_netbios_lan_sess_entry(netbios_lan_sess_entry *, uchar);
extern void nbf_xmit_netbios_session_end(uchar, uchar, uchar *, idbtype *);


extern uchar netbios_multicast[];
extern boolean netbios_packet_debug;
extern boolean netbios_error_debug;


/*
 * nbf_on_idb
 * Return TRUE if NBF supported and enabled on the given interface
 */
static boolean nbf_on_idb (idbtype *idb)
{
    if ((IS_NETBIOS_ACCESS(idb->hwptr) || IS_NETBIOS_LAN(idb->hwptr)) &&
         netbios_sb_access_server_is_enabled(idb))
       return(TRUE);
    return(FALSE);
}


/*
 * netbios_as_init
 * Initialize netbios name data structures and other functions
 * for the Access Server
 */
static void netbios_as_init (subsystype *subsys)
{
    queue_init(&netbios_name_queryQ, 0); 
    queue_init(&netbios_name_recognizedQ, 0);
    queue_init(&netbios_session_initializeQ, 0);
    queue_init(&netbios_pending_session_initializeQ, 0);
    queue_init(&netbios_add_nameQ, 0);
    queue_init(&netbios_status_queryQ, 0);
    queue_init(&netbios_status_responseQ, 0);

    NBFpacketQ = create_watched_queue("NBF Packets", 0, 0);  

    /*
     * Fork the NBF process
     */
    process_create(nbf_input, "NBF Input", NORMAL_STACK, PRIO_NORMAL);

    /* 
     * Initialize parser support for nbf
     */
    netbios_as_parser_init();

    /*
     * Debug initialization chores
     */
    netbios_as_debug_init();

    /*
     * Register media dependent functions
     */
    nbf_media_registry();
    
    /* 
     * Register NBF functions
     */
    reg_add_proto_on_swidb(LINK_NBF, nbf_on_idb, "nbf_on_idb");
    reg_add_raw_enqueue(LINK_NBF, nbf_enqueue, "nbf_enqueue");
    reg_add_hwif_state_change(netbios_as_if_state_change, 
                              "netbios_as_if_state_change");
}

/*
 * nbf_register - LLC2 SAP registration callback.
 * Nop - it's here to keep LLC2 callbacks happy. 
 */
static void nbf_register (llctype *cookie)
{
}


/*
 * nbf_enqueue
 * Interrupt level enqueue routine to queue packets to NBF process
 */
static void nbf_enqueue (paktype *pak)
{
  /*
   * NetBIOS Broadcast or Session packets are passed to NBF process level 
   * to be switched to NBF enabled interfaces. 
   * LLC2 (e.g. I-frame containing NetBIOS data or Supervisory LLC2s etc) 
   * arriving on NetBIOS Access interfaces come directly without going 
   * thru the LLC2 engine yet (fresh off PPP decap). We will pass it 
   * on to the LLC2 engine, which, in turn, rentering this routine, will 
   * only pass on NetBIOS LLC2 I-Frames 
   */
   if (IS_NETBIOS_ACCESS(pak->if_input->hwptr)) {
     /*
      * pak->datagramstart points to one byte before beginning
      * of LLC1/2 data (TYPE_PPP_NBF) 
      * pak->network_start points to beginning of LLC1/2 data.
      * If this is an LLC Unnumbered Information frame, it is a 
      * NetBIOS UI packet.
      * Otherwise it may either be a LLC2 packet arriving from the PPP 
      * engine or had gone thru the the LLC2 engine. 
      * In the former case, send it to the LLC2 engine. In the latter case, 
      * it must be a LLC IFrame with NetBIOS Session data. Process it 
      * accordingly.  
      */
     if ((((snap_hdr *)pak->network_start)->control & ~LLC1_P) == 
         UI) {
       /*
        * Make sure network_start-> NetBIOS data
        * and datagramsize is the size of the NetBIOS data
        */
       pak->network_start += SAP_HDRBYTES;
       pak->info_start = pak->network_start;
       pak->datagramsize -= (SAP_HDRBYTES+1) ;
       pak->enctype = pak->llc2_enctype = ET_SAP; 
       process_enqueue_pak(NBFpacketQ, pak);

     } else if ((pak->llc2_sap == SAP_NETBIOS) &&
                pak->llc2_cb) {
       /*
        * Already went thru the LLC engine.
        * pak->info_start ->LLC2 header
        * pak->datagramstart ->NETBIOS data
        * pak->datagramsize = size of NetBIOS packet 
        * pak->llc2_cb = LLC of interface receiving this pak
        * Pass it to nbf process to be switched
        */
       pak->enctype = ET_LLC2;
       pak->llc2_sap = 0;
       process_enqueue_pak(NBFpacketQ, pak);

     } else {
       /*
        * This is an LLC2 packet arriving from the PPP link.
        * 
        * Make pak->info_start point to beginning of LLC.
        * Mark sap offset as one byte after pak->datagramstart
        * Pass on to the LLC2 engine. If this an async interface and the 
        * pak is a LLC2 Supervisory and other non-informational LLC2 packet, 
        * trigger the async helper process to startoutput on the TTY. 
        * This is required since the LLC2 engine will send LLC2 replies
        * in interrupt level back to the same interface. 
        */
       pak->llc2_sapoffset = 1;  
       pak->info_start = pak->network_start;
       pak->llc2_enctype = ET_LLC2; 

       reg_invoke_llc2_input(pak);
 
       /*
        * If debug is desired, display debug messages only if the nbf process
        * never gets the message (i.e. not a LLC2 IFRAME message)
        */  
       if (netbios_packet_debug && 
           (((((snap_hdr *)pak->network_start)->control & LLC_IMASK) != LLC_IFRAME)))
         nbf_llc_sniff(pak->network_start, pak);

       /*
        * Trigger startoutput if this is an async interface and the packet
        * is not an LLC2 IFRAME
        */ 
       if (((((snap_hdr *)pak->network_start)->control & LLC_IMASK) != LLC_IFRAME) && 
           is_async(pak->if_input->hwptr)) 
         reg_invoke_async_trigger_startoutput(pak->if_input->hwptr);
     }

   } else if (IS_NETBIOS_LAN(pak->if_input->hwptr)) {
     /*
      * A LAN interface
      * If pak->enctype is ET_SAP
      *   pak->network_start points to beginning of NetBIOS header
      * Else if pak->enctype is ET_LLC2
      *   pak has already been passed on to the LLC2 engine, and from there
      *   is passed on here. 
      *   pak->info_start ->LLC2 header
      *   pak->datagramstart ->NETBIOS data
      *   pak->datagramsize = size of NetBIOS packet 
      *   pak->llc2_cb = LLC of interface receiving this pak
      *   Pass it to nbf process to be switched
      */
     if ((pak->enctype == ET_SAP) || (pak->enctype == ET_LLC2)) {
         pak->llc2_enctype = pak->enctype;
         process_enqueue_pak(NBFpacketQ, pak);
     } else {
        if (netbios_error_debug)
           buginf("\n%s nbf Illegal enctype=%x", 
                  pak->if_input->hwptr->hw_namestring, pak->enctype);
        datagram_done(pak);
      }

   } else {
     if (netbios_error_debug)
        buginf("\n%s nbf Illegal interface", pak->if_input->hwptr->hw_namestring);
     datagram_done(pak); 
   }
}


/*
 * nbf_input - NBF process to redistribute NBF packets
 */
static process nbf_input (void)
{
    sys_timestamp nbf_periodic_timer;
    ulong major, minor;

    TIMER_START(nbf_periodic_timer, ONEMIN);
    process_watch_timer(&nbf_periodic_timer, ENABLE);
    process_watch_queue(NBFpacketQ, ENABLE, RECURRING);

    while (TRUE) {
	process_safe_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case QUEUE_EVENT:
		nbf_process_input();
		break;

	      case TIMER_EVENT:
		TIMER_UPDATE(nbf_periodic_timer, ONEMIN);
		nbf_housekeeping();
		break;

	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}


/*
 * nbf_process_input - Process any packets that are on the NBF input queue
 */
static void nbf_process_input (void)
{
    paktype *pak;
    int count;
    uchar destmac[IEEEBYTES];

    /* 
     * Process only a limit number of packets per pass. 
     */
    count = PAK_SWITCHCOUNT;
    while (count-- > 0) {
	/*
	 * Dequeue a NBF datagram
	 */
	pak = process_dequeue(NBFpacketQ);
	if (!pak)
	    break;

        /*
         * Check where it came from
         */
        if (IS_NETBIOS_LAN(pak->if_input->hwptr)) {

           if (pak->enctype == ET_SAP) {
             /* 
              * pak->network_start points to beginning of NetBIOS header
              */ 

             if (netbios_packet_debug) {
               nbf_llc_sniff((pak->network_start-SAP_HDRBYTES), pak);
               nbf_netbios_sniff(pak->network_start);
             }

             /*
              * Process NetBIOS pertinent info
              */
             if (!nbf_netbios_processing(pak, 
                (netbios_header *)pak->network_start, 0, destmac)) {
		if (netbios_error_debug)
                    buginf("\n%s nbf Bad packet", pak->if_input->hwptr->hw_namestring);
                datagram_done(pak);
                continue;
	     }

             /*
              * If the pak matches an outstanding NBFCP control packet, 
              * reassemble the relevant outstanding NBFCP control packet
              */
             if (pak->flags & PAK_CONTROL) {
                nbf_nbfcp_processing(pak, 
                              (netbios_header *)pak->network_start,
                              NETBIOS_NAME_PROJ_REASSEMBLE);
                datagram_done(pak);
                continue;
             } 
  
             /*
              * Readjust so payload size and data to be xmitted is relevant 
              * from the LLC header (instead of the NetBIOS header)
              */
             if (is_ethernet(pak->if_input->hwptr))
                 pak->datagramsize -= ETHER_ARPA_ENCAPBYTES;
             else
                 pak->datagramsize -= TRING_ENCAPBYTES;

             nbf_forward_pakout(pak, destmac); 

           } else if (pak->enctype == ET_LLC2) {
             /* 
              * Else must be LLC2 packet passed up by the LLC2 engine
              * pak->info_start = LLC header.
              * pak->network_start = NetBIOS header
              * pak->datagramstart = NetBIOS header
              * pak->datagramsize = NetBIOS packet size 
              */
             
             if (netbios_packet_debug) {
                nbf_llc_sniff(pak->info_start, pak);
                nbf_netbios_sniff(pak->info_start+SAP_LONGHDRBYTES);
	     }

             /*
              * Process NetBIOS pertinent data only
              */
             if (!nbf_netbios_processing(pak, 
                  (netbios_header *)(pak->info_start + SAP_LONGHDRBYTES),
                  0, destmac)) {
                if (netbios_error_debug)
                   buginf("\n%s nbf Bad packet", 
                           pak->if_input->hwptr->hw_namestring);
                datagram_done(pak);
                continue;
	     }
             
             /*
              * Send the IFRAME out
              */
             llc2_send(pak->llc2_cb, pak, pak->datagramsize);

           } else {
             buginf("\nnbf Unknown enctype=%x", pak->enctype); 
             datagram_done(pak);
           }

	} else {
           /*
            * Came from an async or ISDN interface
            */
           if (pak->enctype == ET_SAP) {
             /*
              * Pak->network_start points to beginning of NetBIOS data
              * If this is a special control packet from NBFCP for 
              * NetBIOS Name projection, process it separately
              */
             if (pak->flags & PAK_CONTROL) {
                 nbf_nbfcp_processing(pak,
                                   (netbios_header *)pak->network_start, 
                                   NETBIOS_NAME_PROJ_DISPERSE);
                 continue;
             }
             
             if (netbios_packet_debug) {
                nbf_llc_sniff((pak->network_start-SAP_HDRBYTES), pak);
                nbf_netbios_sniff(pak->network_start);
	    }

             /*
              * Process NetBIOS pertinent data only
              */
             if (!nbf_netbios_processing(pak, 
                  (netbios_header *)pak->network_start, 0, destmac)) {
                if (netbios_error_debug)
                   buginf("\n%s nbf Bad packet", 
                           pak->if_input->hwptr->hw_namestring);
                datagram_done(pak);
                continue;
	      }

              /*
               * If the pak matches an outstanding NBFCP control packet, 
               * reassmble the relevant outstanding NBFCP control packet
               */
              if  (pak->flags & PAK_CONTROL) {
                 nbf_nbfcp_processing(pak,
                          (netbios_header *)pak->network_start,
                          NETBIOS_NAME_PROJ_REASSEMBLE);
                 datagram_done(pak);
                 continue;
              } 

              /*
               * pak->network_start = NetBIOS header
               * pak->datagramsize = size of NetBIOS data
               * Note: The Ethernet vencap will backfill the dsap/ssap/control
               */
              nbf_forward_pakout(pak, destmac);

	   } else {
              /* 
               * Else must be LLC2 packet passed up by the LLC2 engine
               * pak->info_start = LLC header.
               * pak->network_start = NetBIOS header
               * pak->datagramstart = NetBIOS header
               * pak->datagramsize = NetBIOS packet size 
               */
              if (netbios_packet_debug) {
                nbf_llc_sniff(pak->info_start, pak);
                nbf_netbios_sniff(pak->info_start+SAP_LONGHDRBYTES);
              }
            
              /*
               * Process NetBIOS pertinent data only
               */
              if (!nbf_netbios_processing(pak, 
                  (netbios_header *)(pak->info_start + SAP_LONGHDRBYTES),
                  SAP_LONGHDRBYTES, destmac)) {
                if (netbios_error_debug)
                   buginf("\n%s nbf Bad packet", 
                           pak->if_input->hwptr->hw_namestring);
                datagram_done(pak);
                continue;
	      }

              /*
               * Send the IFRAME out
               */
              llc2_send(pak->llc2_cb, pak, pak->datagramsize);

           }
	 } 
      } /* end while */
}


/*
 * nbf_name_proj_get_next 
 * Return the NBFCP Name Projection entry at specified index
 */ 
static netbios_nbfcp_name_proj_entry *nbf_name_proj_get_next (
     netbios_nbfcp_name_proj_entry *name_proj, int index)
{
   return(&name_proj[index]);
}


/*
 * nbf_name_proj_search
 * Search the NBFCP Name Projection list for a specified NetBIOS
 * Name
 */
static netbios_nbfcp_name_proj_entry *nbf_name_proj_search (
     netbios_nbfcp_name_proj_entry *input, int size, uchar *name)
{
    int i;
    netbios_nbfcp_name_proj_entry *ptr;
   
    for (i=0; i < size; i++) {
       ptr = nbf_name_proj_get_next(input, i);
       if (strncmp(ptr->netbios_name, name, NETBIOS_MAX_NAME_LEN) == 0)
         return(ptr); 
       input++;
    }   
    return(NULL);
}


/*
 * nbf_housekeeping - Do NBF periodic housekeeping 
 */
static void nbf_housekeeping (void) 
{
   nbf_flush_queue(&netbios_name_queryQ);
   nbf_flush_queue(&netbios_name_recognizedQ);
   nbf_flush_queue(&netbios_session_initializeQ);
   nbf_flush_queue(&netbios_pending_session_initializeQ);
   nbf_flush_queue(&netbios_add_nameQ);
   nbf_flush_queue(&netbios_status_queryQ);
   nbf_flush_queue(&netbios_status_responseQ);
}


/*
 * nbf_flush_queue
 * Scan a queue and remove any paks that have timeout out
 */
static void nbf_flush_queue (queuetype *queue)
{
   paktype *prev_pak, *temp_pak, *qpak;
  
   prev_pak = NULL;
   qpak = queue->qhead;

   while (qpak) {
      if (TIMER_RUNNING_AND_AWAKE(qpak->gp_timer)) {
        temp_pak = qpak->next;
        remqueue(queue, qpak, prev_pak);
        retbuffer(qpak);
        qpak = temp_pak;
        continue;
      }
      prev_pak = qpak;
      qpak = qpak->next;
   }
}


/*
 * nbf_forward_pakout
 * Forward this pak to all eligible NetBIOS interfaces
 * The NetBIOS multicast address is used as the destination MAC address.
 * This is only meaningful for Ethernet and Token Ring interfaces
 */
static void nbf_forward_pakout (paktype *pak, uchar *destmac)
{
    hwidbtype *tempidb;
    paktype *newpak;

    /*
     * If pak is marked as a directed traffic, send it only to the 
     * the target interface
     */
    if (!(pak->flags & PAK_BROADCAST)) {
       nbf_pakout(pak, pak->if_output, destmac);
       return;
    } 

    /*
     * Forward the pak to all NetBIOS ready interfaces
     * (except the interface where the pak arrived)
     */
    FOR_ALL_HWIDBS(tempidb) {

      if (interface_up_simple(tempidb->firstsw) && 
          (pak->if_input->hwptr != tempidb) && 
          netbios_sb_access_server_is_enabled(tempidb->firstsw)) {
       
           if (IS_NETBIOS_LAN(tempidb)) {

              /*
               * Duplicate the packet
               */
              newpak = pak_duplicate(pak);

              if (!newpak)
               break;

              nbf_pakout(newpak, tempidb->firstsw, destmac); 
 
           } else if (IS_NETBIOS_ACCESS(tempidb)) {

              /*
               * Duplicate the packet
               */
              newpak = pak_duplicate(pak);

              if (!newpak)
                 break;
              nbf_pakout(newpak, tempidb->firstsw, NULL);
	   }
        }
    }

    /*
     * Discard the original pak
     */
    datagram_done(pak);
}


/*
 * nbf_pakout - Encapsulate and output a NBF packet
 */
static void nbf_pakout (
     paktype *pak,
     idbtype *idb,
     uchar *addr)
{
    pak->if_output = idb;
    pak->linktype = LINK_NBF;

    pak->enctype = ET_ARPA;

    /*
     * If the output interface is an access interface, 
     * we need to correct the network_start (pointing to NetBIOS 
     * header) to point to the LLC2 header (NetBIOS-SAP NetBIOS-SAP UI).
     * The datagramsize is also corrected.
     * NB: This correction should really be done in ppp_vencap but it would
     *     have to know that the packet was a UI packet. 
     */
    if (IS_NETBIOS_ACCESS(pak->if_output->hwptr)) {
       pak->network_start -= SAP_HDRBYTES;
       pak->datagramsize +=  SAP_HDRBYTES;
    }

    pak->datagramstart = pak->network_start;
    if ((*idb->hwptr->vencap)(pak, (long)addr)) {
	datagram_out(pak);
    } else {
	datagram_done(pak);
    }
}


/*
 * nbf_netbios_processing
 * Process the NetBIOS packet according to NetBIOS command type. 
 * In general, for a LLC1 NetBIOS packet received on a source interface, 
 * before being sent out to a destination MAC address on a target interface, 
 * a duplicate is kept on queue so that when the expected NetBIOS packet 
 * is received back (sent by station at the destination MAC address), 
 * we will know how to switch it back on the original sender.
 * For certain LLC1 and LLC2 NetBIOS handshaking packets between the the 
 * NetBIOS peer that culminate in a NetBIOS session being formed between 
 * the two peers, we will snoop these packets and enter the session 
 * and relevant switching info in the relevant session tables.  
 * A NetBIOS SESSION_END snooped will delete the entry on the arriving 
 * interface and the corresponding entry in the destination interface.
 *
 * Outputs:
 *   Return FALSE - unexpected happens (e.g. low memory and other system
 *                  related etc). Caller should discard pak.
 *   Return TRUE  - caller may send the pak to the appropriate destination
 *                  - LLC2 NetBIOS packets (NetBIOS SESSION packets):
 *                      pak->if_output-> destination interface
 *                      pak->llc2_cb-> target LLC connection 
 *                      destmac-> destination IEEE mac address
 *                  - LLC1 UI NetBIOS packets (NetBIOS datagram packets)
 *                      Broadcast NetBIOS packets
 *                        - pak->flags's PAK_BROADCAST set
 *                        - destmac = NetBIOS multicast MAC address
 *                      Directed NetBIOS packets 
 *                        - pak->flags's PAK_BROADCAST clear
 *                        - pak->if_output-> destination interface
 *                        - destmac = NetBIOS detination MAC address 
 */  
static boolean nbf_netbios_processing (paktype *pak, netbios_header *netbios_hdr, int advance, uchar *destmac)
{
    paktype *newpak, *qpak;
    hwaddrtype macaddr;
    uchar srcmac[IEEEBYTES];
    netbios_sess_entry *netbios_sess_entry_ptr;
    netbios_lan_sess_entry *netbios_lan_sess_entry_ptr;
    llctype *target_llc;

    switch (netbios_hdr->command) {
       case NETBIOS_NAME_IN_CONFLICT:
       case NETBIOS_DATAGRAM:
       case NETBIOS_DATAGRAM_BROADCAST:
       case NETBIOS_TERMINATE_TRACE_REM:
       case NETBIOS_TERMINATE_TRACE_REM_LOC:
          /*
           * These NetBIOS packets (and expected returns) are  all 
           * broadcast packets. No need to queue, just forward them.
           * Mark the pak as a broadcast pak 
           */
          pak->flags |= PAK_BROADCAST;
          ieee_copy(netbios_multicast, destmac);           
          break;

       case NETBIOS_ADD_GROUP_QUERY:
       case NETBIOS_ADD_NAME_QUERY:
          /*
           * Duplicate the pak and queue it. Queuing the duplicate pak sets
           * the expected matching return from destination with a 
           * NETBIOS_ADD_NAME_RESPONSE packet. 
           * Multiple entries are allowed to be queued since one or 
           * more NETBIOS_ADD_NAME_RESPONSE arriving should find its way 
           * to the sender of this pak (NETBIOS_NAME_CONFLICT is thus sent
           * by the peer).
           */
          newpak = pak_duplicate(pak);
          if (!newpak) 
             return(FALSE);

          if (!nbf_check_and_enqueue(newpak, advance, TRUE)) {
             retbuffer(newpak);
             return(FALSE);
	  }

          /*
           * Mark the pak as a broadcast pak 
           */
          pak->flags |= PAK_BROADCAST;
          ieee_copy(netbios_multicast, destmac);
          break;

       case NETBIOS_ADD_NAME_RESPONSE:
          /*
           * Search queue for match with previous NETBIOS_ADD_GROUP_QUERY
           * or NETBIOS_ADD_NAME_QUERY
           */  
          qpak = nbf_check_and_dequeue(pak, netbios_hdr);

          if (!qpak)
             return(FALSE);

          /*
           * We have a pak in the queue that matches the the current pak.
           * Load the current pak with the target output interface and
           * destination MAC address (if necessary). Inherit the flags
           * of the queued pak.
           */
          pak->flags |= qpak->flags;
          pak->if_output = qpak->if_input;
          pak->flags &= ~PAK_BROADCAST;
          if (IS_NETBIOS_LAN(pak->if_output->hwptr)) {
             (*pak->if_output->hwptr->extract_snpa)(qpak, &macaddr, SOURCE);
               ieee_copy((uchar *)macaddr.addr, destmac);
	  }

          /*
           * Discard the queued pak
           */
          retbuffer(qpak);
          break;
     
       case NETBIOS_STATUS_QUERY:
          /*
           * Duplicate the pak and queue it. Queuing the duplicate pak sets
           * the expected matching return from destination with a 
           * NETBIOS_STATUS_RESPONSE packet.
           */
          newpak = pak_duplicate(pak);
          if (!newpak) 
             return(FALSE);

          if (!nbf_check_and_enqueue(newpak, advance, FALSE)) {
             retbuffer(newpak);
             return(FALSE);
	  }

          /*
           * Normally, this packet is a broadcast packet. However, it 
           * can be a directed packet to the original sender of a previous 
           * NETBIOS_STATUS_RESPONSE packet which had indicated it had more 
           * info to send. Thus, we must check if there is such a match
           */
          if (QUEUESIZE(&netbios_status_queryQ)) {
             qpak = nbf_check_and_dequeue(pak, netbios_hdr);

             if (!qpak) {
                /*
                 * Not found. Mark the pak as a broadcast pak 
                 */
                pak->flags |= PAK_BROADCAST;
                ieee_copy(netbios_multicast, destmac);
                break;
             }

             /*
              * We have a pak in the queue that matches the the current pak.
              * Load the current pak with the target output interface and
              * destination MAC address (if necessary)
              */
             pak->if_output = qpak->if_input;
             pak->flags &= ~PAK_BROADCAST;
             if (IS_NETBIOS_LAN(pak->if_output->hwptr)) {
                (*pak->if_output->hwptr->extract_snpa)(qpak, &macaddr, SOURCE);
                ieee_copy((uchar *)macaddr.addr, destmac);
	     }

             /*
              * Discard the queued pak
              */
             retbuffer(qpak);

          } else {
             /*
              * Mark the pak as a broadcast pak 
              */
             pak->flags |= PAK_BROADCAST;
             ieee_copy(netbios_multicast, destmac);
	  }
          break;

       case NETBIOS_STATUS_RESPONSE:
          /*
           * Search queue for match with previous NETBIOS_STATUS_QUERY
           */  
          qpak = nbf_check_and_dequeue(pak, netbios_hdr);

          if (!qpak)
             return(FALSE);

          /*
           * We have a pak in the queue that matches the the current pak.
           * Load the current pak with the target output interface and
           * destination MAC address (if necessary)
           */
          pak->if_output = qpak->if_input;
          pak->flags &= ~PAK_BROADCAST;
          if (IS_NETBIOS_LAN(pak->if_output->hwptr)) {
             (*pak->if_output->hwptr->extract_snpa)(qpak, &macaddr, SOURCE);
               ieee_copy((uchar *)macaddr.addr, destmac);
	  }

          /*
           * Discard the queued pak
           */
          retbuffer(qpak);

          /*
           * A NETBIOS_STATUS_RESPONSE message that is marked as unable to 
           * deliver all its info from the sender will solicit the 
           * receiver of this message to send another NETBIOS_STATUS_QUERY.
           * Only this time, it will be a directed message instead of a 
           * broadcast. For this reason, the  response message must also
           * be queued
           */
          if (netbios_hdr->data1 > 0) {
             /*
              * Duplicate the pak and queue it. Queuing the duplicate pak sets
              * the expected matching return from destination with a 
              * NETBIOS_STATUS_QUERY packet.
              */
             newpak = pak_duplicate(pak);
             if (!newpak) 
                return(FALSE);

             if (!nbf_check_and_enqueue(newpak, advance, FALSE)) {
                retbuffer(newpak);
                return(FALSE);
	     }
          }             
          break;

       case NETBIOS_NAME_QUERY:
          /*
           * Duplicate the pak and queue it. Queuing the duplicate pak sets
           * the expected matching return from destination with a 
           * NETBIOS_NAME_RECOGNIZED packet.
           */
          newpak = pak_duplicate(pak);
          if (!newpak) 
             return(FALSE);

          /*
           * Try queuing it if not already queued
           */
          if (!nbf_check_and_enqueue(newpak, advance, FALSE))
             retbuffer(newpak);

          /*
           * Mark the pak as a broadcast pak 
           */
          pak->flags |= PAK_BROADCAST;
          ieee_copy(netbios_multicast, destmac);
          break;

       case NETBIOS_NAME_RECOGNIZED:
          /*
           * Search queue for match with previous NETBIOS_NAME_QUERY
           */  
          qpak = nbf_check_and_dequeue(pak, netbios_hdr);

          if (!qpak)
             return(FALSE);

          /*
           * We have a pak in the queue that matches the the current pak.
           * Load the current pak with the target output interface and
           * destination MAC address (if necessary)
           */
          pak->if_output = qpak->if_input;
          pak->flags &= ~PAK_BROADCAST;
          if (IS_NETBIOS_LAN(pak->if_output->hwptr)) {
             (*pak->if_output->hwptr->extract_snpa)(qpak, &macaddr, SOURCE);
               ieee_copy((uchar *)macaddr.addr, destmac);
	  }

          /*
           * Discard the queued pak
           */
          retbuffer(qpak);

          /*
           * Don't queue this NETBIOS_NAME_QUERY if the data2 field 
           * indicates it has no listen pending or has insufficient 
           * resources. A NETBIOS_SESSION_INITIALIZE is not expected.
           */
          if ((netbios_hdr->data2[0] == NETBIOS_NAME_QUERY_NO_LISTEN) ||
              (netbios_hdr->data2[0] == NETBIOS_NAME_QUERY_NO_RESOURCE))
             break;

          /*
           * Duplicate the pak, queue it. Queuing the duplicate pak sets
           * the expected matching return from destination with a 
           * NETBIOS_SESSION_INITIALIZE packet.
           */
          newpak = pak_duplicate(pak);

          if (!newpak)
             return(FALSE);

          /*
           * Try queueing it in expectation of a NETBIOS_SESSION_INITIALIZE.
           * Regardless of the outcome, always forward this pak out
           */
          if (!nbf_check_and_enqueue(newpak, advance, FALSE))
             retbuffer(newpak);

          break;

       case NETBIOS_SESSION_INITIALIZE:
          /*
           * Search the queue for match with previous NETBIOS_NAME_RECOGNIZED
           */
          qpak = nbf_check_and_dequeue(pak, netbios_hdr);

          if (!qpak)
             return(FALSE);

          /*
           * We have a pak in the queue that matches the the current pak.
           * Load the current pak with the target output interface and
           * destination MAC address (if any)
           */
          pak->if_output = qpak->if_input;

          (*pak->if_output->hwptr->extract_snpa)(qpak, &macaddr, SOURCE);
          ieee_copy((uchar *)macaddr.addr, destmac);

          /*
           * Check if there is a LLC2 connection to destination 
           */
          target_llc = llc2_find_cb(destmac, SAP_NETBIOS, SAP_NETBIOS, 
                             pak->if_output);

          /*
           * No LLC2 connection yet. Open an LLC connection (non-blocking).
           * When it is open asynchronously, then send the SESSION_INITIALIZE 
           * across. Meanwhile, return FALSE here so SESSION_INITIALIZE is 
           * not sent
           */           
          if (!target_llc) {
          
              nbf_llc2_open(pak, advance, &macaddr);
               
              if (netbios_packet_debug)
                  buginf("\n%s nbf target LLC2 Attempt Open dstidb=%s destmac=%e", 
                         pak->if_input->hwptr->hw_namestring, 
                         pak->if_output->hwptr->hw_namestring, destmac); 
              retbuffer(qpak);
              return(FALSE);

          } else if (target_llc->state != LLS_NORMAL) {
               /*
                * If the connection is not LLS_NORMAL, it is still pending.
                * Requeue the previous NAME_RECOGNIZED queue back onto 
                * relevant queue. The SESSION_INITIALIZE pak is discarded
                * without any action by the caller.
                */
               if (netbios_error_debug) 
                  buginf("\n%s LLC connection not LLS_NORMAL llc idb= %s", 
                         pak->if_output->hwptr->hw_namestring,
                         target_llc->idb->hwptr->hw_namestring);
              
               TIMER_START(qpak->gp_timer, NETBIOS_NBF_QUEUE_TIMEOUT); 
               requeue(&netbios_name_recognizedQ, qpak);
               return(FALSE); 
          } 

          /*
           * Discard the original queued pak.
           */
          retbuffer(qpak); 

          /*
           * There is already an existing LLC2 connection. We can proceed 
           * with queuing the duplicated  NETBIOS_SESSION_INITIALIZE packet 
           * Queuing the duplicate pak sets the expected matching return 
           * from destination with a NETBIOS_SESSION_CONFIRM packet.
           */
          newpak = pak_duplicate(pak);

          if (!newpak)
             return(FALSE);

          if (!nbf_check_and_enqueue(newpak, advance, FALSE)) {
             retbuffer(newpak);
             return(FALSE);
	  }

          /*
           * Mark the pak to that LLC2 connection.
           */
          pak->llc2_cb = target_llc;
          break;

	case NETBIOS_SESSION_CONFIRM:
          /*
           * Search queue for match with previous NETBIOS_SESSION_INITIALIZE
           */
          qpak = nbf_check_and_dequeue(pak, netbios_hdr);

          if (!qpak) {
             if (netbios_error_debug) 
               buginf("\nnbf LLC2 Session Confirm no longer valid srcidb=%s "
                      "destmac=%e", pak->if_input->hwptr->hw_namestring, 
                      destmac); 
             return(FALSE);
	   }
 
          /*
           * We have a pak in the queue that matches the the current pak.
           * Load the current pak with the target output interface and
           * destination MAC address (if necessary)
           */
          pak->if_output = qpak->if_input;

          /*
           * If SESSION_CONFIRM came in on a LAN interface, get the source 
           * MAC address of the sender. 
           * The address is moved to destmac, since by definition, destmac 
           * is the destination MAC address of a NETBIOS session packet 
           * that we have received (regardless of which interface) must be 
           * be sent to.
           * If the previous matching SESSION_INITIALIZE also came in on a LAN 
           * interface, get the source address of the sender. 
           * It's OK, NetBIOS Access interfaces return empty addresses here.
           */
          (*pak->if_input->hwptr->extract_snpa)(pak, &macaddr, SOURCE);
          ieee_copy((uchar *)macaddr.addr, destmac);
          (*qpak->if_input->hwptr->extract_snpa)(qpak, &macaddr, SOURCE);
          ieee_copy((uchar *)macaddr.addr, srcmac);

          /*
           * Discard the queued pak
           */
          retbuffer(qpak);

          /*
           * Check if there is a LLC2 connection to destination 
           * The destination MAC is the source MAC of the sender of 
           * SESSION_INITIALIZE.
           */
          target_llc = llc2_find_cb(srcmac, SAP_NETBIOS, SAP_NETBIOS, 
                             pak->if_output);
          if (!target_llc) {
             if (netbios_error_debug) 
               buginf("\nnbf LLC2 Session Confirm no longer valid dstidb=%s "
                      "destmac=%e", pak->if_output->hwptr->hw_namestring, 
                      destmac); 
             return(FALSE);
          }

          /*
           * After matching the protocol sequence thru the various queues 
           * we now will update/insert the session table with information 
           * concerning this session.  
           * Note that:
           *  pak->if_output - interface which SESSION_INITIALIZE arrived 
           *  pak->if_input  -   "         "   SESSION_CONFIRM     "
           *  srcmac         - MAC address of sender of SESSION_INITIALIZE
           *  dstmac         -   "    "    "    "    "  SESSION_CONFIRM
           *  netbios_hdr    - pointer to NETBIOS SESSION<_CONFIRM packet
           */
          if (!nbf_insert_sess_entry(pak->if_output->hwptr, 
                                     pak->if_input->hwptr, 
                                     srcmac, destmac,
                                     target_llc, pak->llc2_cb, 
                                     (netbios_header_sess *)netbios_hdr))
             return(FALSE);

          /*
           * Mark the pak to that LLC2 connection.
           */
          pak->llc2_cb = target_llc;
          break;

       case NETBIOS_DATA_ACK:
       case NETBIOS_DATA_FIRST_MIDDLE:
       case NETBIOS_ONLY_LAST:
       case NETBIOS_SESSION_END:
       case NETBIOS_NO_RECEIVE:
       case NETBIOS_RECEIVE_OUTSTANDING:
       case NETBIOS_RECEIVE_CONTINUE:
       case NETBIOS_SESSION_ALIVE:
          /*
           * These NetBIOS Session packets will be switched to the 
           * the intended target interface and destination MAC address
           * (if any).
           * If incoming interface is an access interface, fill in 
           * the pak with the info from the per-interface 
           * session table. Otherwise, it must be a LAN interface. 
           * Fill in tha pak with info from the global hash table
           */
          if (IS_NETBIOS_ACCESS(pak->if_input->hwptr)) {
 
             netbios_sess_entry_ptr = 
               nbf_sess_occupied(pak->if_input, 
                                 ((netbios_header_sess *)netbios_hdr)->srcnum);

             if (!netbios_sess_entry_ptr)
                return(FALSE);

             pak->if_output = netbios_sess_entry_ptr->dstidb->firstsw;
             pak->llc2_cb = netbios_sess_entry_ptr->dst_llc;
             ieee_copy(netbios_sess_entry_ptr->dstmacaddr, destmac);

          } else {

             /*
              * Sender must have come in from the LAN
              * Get the source mac address of sender  
              */
             (*pak->if_input->hwptr->extract_snpa)(pak, &macaddr, SOURCE);
             ieee_copy((uchar *)macaddr.addr, srcmac);
             
             netbios_lan_sess_entry_ptr = 
              nbf_lan_sess_occupied(pak->if_input->hwptr, NULL, 
                        ((netbios_header_sess *)netbios_hdr)->srcnum,
                        ((netbios_header_sess *)netbios_hdr)->destnum, srcmac, 
                        NULL);

             if (!netbios_lan_sess_entry_ptr)
                return(FALSE);

             pak->if_output = netbios_lan_sess_entry_ptr->dstidb->firstsw;
             pak->llc2_cb = netbios_lan_sess_entry_ptr->dst_llc;
             ieee_copy(netbios_lan_sess_entry_ptr->dstmacaddr, destmac);
	  }

          /*
           * If this is a SESSION_END, tear down the relevant entries
           */ 
          if (netbios_hdr->command == NETBIOS_SESSION_END) {
             if (netbios_packet_debug)
               buginf("\n%s (i) SESSION_END detected, quietly deleting "
                      "session entries", pak->if_input->hwptr->hw_namestring);
             nbf_delete_sess_entry(pak->if_input->hwptr,
                                   pak->if_output->hwptr,
                                   srcmac, 
                                   destmac,
                                   (netbios_header_sess *)netbios_hdr);
	  }

          return(TRUE);
          break;
       
       default:
          return(FALSE); 
    }
    return(TRUE);
}


/*
 * nbf_nbfcp_processing
 * If method is NETBIOS_NAME_PROJ_DISPERSE 
 *   Handle a special NBFCP control packet containing a list of
 *   NetBIOS Names to be broadcasted as individual NETBIOS_ADD_NAME_QUERY or
 *   NETBIOS_ADD_GROUP_QUERY packets. 
 * Else if method is NETBIOS_NAME_PROJ_REASSEMBLE
 *   Reassemble the pending NBFCP control packet from the 
 *   NETBIOS_ADD_NAME_RESPONSE received from the network
 */  
static boolean nbf_nbfcp_processing (paktype *pak, netbios_header *netbios_hdr,
int method)
{
    int i, total_entries;
    netbios_nbfcp_name_proj_entry *nbf_name_proj_entry;
    netbios_sb_t *netb_sb;
    paktype *temppak;
    uchar destmac[IEEEBYTES];

    switch (method) {
      case NETBIOS_NAME_PROJ_DISPERSE:

          /*
           * Create the per-session table if not there already
           * This allows creation only when necessary, saving memory
           */
          if (!nbf_prep_interface(pak->if_input))
             return(FALSE); 

          /*
           * Calculate the total number of Name Projection entries
           * in the control pak
           */
          total_entries = pak->datagramsize / sizeof(netbios_nbfcp_name_proj_entry);
          /*
           * For each entry, generate a NETBIOS_ADD_NAME_QUERY or 
           * NETBIOS_ADD_GROUP_QUERY and broadcast it out the network
           */ 
          for (i=0; i < total_entries; i++) {
              nbf_name_proj_entry = nbf_name_proj_get_next(
                      (netbios_nbfcp_name_proj_entry *)pak->network_start, i);

              if (!nbf_name_proj_entry)
                 return(FALSE);

              /*
               * Manufacture a NETBIOS_ADD_NAME_QUERY/NETBIOS_ADD_GROUP_QUERY
               * packet
               */
              temppak = nbf_create_netbios_add_name_query(pak->if_input,
                         nbf_name_proj_entry->netbios_name,
                         nbf_name_proj_entry->reason);

              if (!temppak)
                 return(FALSE);

              temppak->flags |= PAK_CONTROL;

              /*
               * Mark the entry with an initial return code
               */
              nbf_name_proj_entry->reason = NETBIOS_NCB_GOOD_RETURN;

              if (netbios_packet_debug)
                buginf("\nnbf nbfcp %s %d", 
                       nbf_name_proj_entry->netbios_name, 
                       nbf_name_proj_entry->reason);

              /*
               * Pass the generated pak thru the standard processing 
               * as if it was actually received on an interface
               */
              if (!nbf_netbios_processing(temppak, 
                 (netbios_header *)temppak->network_start, 0, destmac)) {
                 if (netbios_error_debug)
                    buginf("\n%s nbf Bad packet", 
                           pak->if_input->hwptr->hw_namestring);
                 retbuffer(temppak);
                 continue;
	      }
               
              if (netbios_packet_debug) {
                 nbf_llc_sniff((pak->network_start-SAP_HDRBYTES), pak);
                 nbf_netbios_sniff(temppak->network_start);
	      }
              
              nbf_forward_pakout(temppak, destmac);
          }
          break;

      case NETBIOS_NAME_PROJ_REASSEMBLE:
          /*
           * Look for the special NBFCP control packet hanging off the 
           * the output interface. If there isn't one, NBFCP probably
           * timed out. 
           */
          netb_sb = netbios_get_sb(pak->if_output);
          temppak = (netb_sb ? netb_sb->netbios_nbfcp_pak : NULL);
          if (!temppak) {
             if (netbios_error_debug)
                buginf("\n%s nbf nbfcp Name Projection reassembly discarded", 
                       pak->if_output->hwptr->hw_namestring);
             return(FALSE);
          }
           
          /*
           * Calculate the total number of Name Projection entries
           * in the control pak
           */
          total_entries = temppak->datagramsize 
                          / sizeof(netbios_nbfcp_name_proj_entry);

          if (!total_entries)
              return(FALSE);

          /*
           * Look for a match by NetBIOS name with the NBFCP control packet.
           * If found, update the entry with the appropriate reason.
           */
          switch (netbios_hdr->command) {
	    case NETBIOS_ADD_NAME_RESPONSE:
               nbf_name_proj_entry = nbf_name_proj_search(
                 (netbios_nbfcp_name_proj_entry *)(temppak->network_start),
                 total_entries, netbios_hdr->destname);

               if (!nbf_name_proj_entry)
                  return(FALSE);

               if (netbios_packet_debug)
                 buginf("\n%s nbf nbfcp update %s %d", 
                        pak->if_output->hwptr->hw_namestring,
                        nbf_name_proj_entry->netbios_name,
                        NETBIOS_NCB_NAME_INUSE);

               nbf_name_proj_entry->reason = NETBIOS_NCB_NAME_INUSE;
               netb_sb->nbfcp_pak_modified = TRUE;
               return(TRUE);
               break;

            default:
              return(FALSE);
	  }
          break;

      default:
        return(FALSE);
    }
    return(TRUE);
}


/*
 * nbf_prep_interface
 * Allocate/Deallocate a table when the interface is NetBIOS enabled/disabled 
 * For NetBIOS Access interfaces, there is a netbios session table
 * per interface. For NetBIOS LAN interfaces, there is one global session 
 * hash table. 
 */
static boolean nbf_prep_interface (idbtype *idb)
{
  int i;
  netbios_sb_t *netb_sb;

  netb_sb = netbios_get_sb(idb);
  if (!netb_sb) {
      return(FALSE);
  }

  /*
   * Create a NBF session table only when necessary
   */
  if (!netb_sb->access_server_enabled) {
    return(FALSE);
  }
     
   /*
    * Turning on NetBIOS access server functionality.
    * If there is not a NetBIOS context already, initialize one.
    * For ASYNC and ISDN interfaces, there is a netbios session table
    * per interface. 
    * For ETHER or TR interfaces, there is one global session hash 
    * table. 
    */
   if (IS_NETBIOS_ACCESS(idb->hwptr) && !netb_sb->sess_table_ptr) {
       netb_sb->sess_table_ptr = 
             malloc_named((NETBIOS_MAX_SESSIONS+1)*sizeof(netbios_sess_entry),
                          "NetBIOS session table");
       if (!netb_sb->sess_table_ptr)
          return(FALSE);
       if (netbios_packet_debug)
          buginf("\n%s NetBIOS session table created", 
                 idb->hwptr->hw_namestring);

   } else if (IS_NETBIOS_LAN(idb->hwptr) &&
              !netbios_lan_hash_table_ptr) {

       netbios_lan_hash_table_ptr = 
             malloc_named(NETBIOS_HASH_SESS_LEN*sizeof(queuetype),
                          "NetBIOS LAN session hash table");
       if (!netbios_lan_hash_table_ptr)
          return(FALSE);
       if (netbios_packet_debug)
          buginf("\n%s NetBIOS LAN session hash table created",
                   idb->hwptr->hw_namestring);    
       for (i=0; i<NETBIOS_HASH_SESS_LEN; i++)
            queue_init(&netbios_lan_hash_table_ptr[i], 0);
   } 
   return(TRUE);
}


/*
 * nbf_check_and_enqueue
 * If queue_anyway is FALSE
 *   Will only queue a pak if the netbios portion of the pak
 *   is not identical to one already in the queue. This prevents
 *   unnecessary queuing a retransmitted packet onto the same queue.
 * else if queue_anyway is TRUE
 *   queue it unconditionally. Duplicate entries of the same NetBIOS 
 *   packet are allowed then.
 * Return TRUE unless pak enqueuing (if necessary) fails. 
 * Need need to interrupt lock since all queuing/dequeuing is done
 * at (this) process level only 
 */
static boolean nbf_check_and_enqueue (paktype *pak, int advance, boolean queue_anyway)
{
    queuetype *queue;
    paktype *qpak, *temp_pak, *qpak_prev;
    netbios_header_sess *qnetbios_hdr_sess, *netbios_hdr_sess; 
    netbios_header *qnetbios_hdr = NULL;
    netbios_header *netbios_hdr;
    int count = PAK_SWITCHCOUNT;

    if (!pak)
       return(FALSE);

    netbios_hdr = (netbios_header *)(pak->network_start+advance);

    switch (netbios_hdr->command) {
       case NETBIOS_NAME_QUERY:
            queue = &netbios_name_queryQ;
            break;
       case NETBIOS_NAME_RECOGNIZED:
            queue = &netbios_name_recognizedQ;
            break;
       case NETBIOS_SESSION_INITIALIZE:
            queue = &netbios_session_initializeQ;
            break;
       case NETBIOS_ADD_NAME_QUERY:
       case NETBIOS_ADD_GROUP_QUERY:
            queue = &netbios_add_nameQ;
            break;
       case NETBIOS_STATUS_QUERY:
            queue = &netbios_status_queryQ;
            break;
       case NETBIOS_STATUS_RESPONSE:
            queue = &netbios_status_responseQ;
            break;
       default:
          return(FALSE);
    }

    /*
     * Always normalize the netbios header to be pointed to by
     * pak->networkstart
     */
    pak->network_start += advance;

    /*
     * If requested, Queue it unconditionally. Certain NetBIOS packets 
     * require this. 
     */
    if (queue_anyway) {
      TIMER_START(pak->gp_timer, NETBIOS_NBF_QUEUE_TIMEOUT); 
      enqueue(queue, pak);
      return(TRUE);
    } 

    /*
     * Search the specified queue for a match. It there is one, 
     * don't add but reset the timer of the pak in the queue.
     * The match conditions depend on the NetBIOS command type.
     * Also, do some cleaning up if timer of the queued pak has expired.
     * Use a safeguard count to avoid being a CPUHOG.
     */    
    qpak_prev = NULL;
    qpak = queue->qhead;
    while (qpak && (count-- >0)) {    
      /*
       * If the pak has expired, discard it 
       */
      if (TIMER_RUNNING_AND_AWAKE(qpak->gp_timer)) {
         temp_pak = qpak->next;
         remqueue(queue, qpak, qpak_prev);
         retbuffer(qpak);
         qpak = temp_pak;
         continue;
      } else {
         switch (netbios_hdr->command) {

	  case NETBIOS_ADD_NAME_QUERY:
          case NETBIOS_ADD_GROUP_QUERY:
            qnetbios_hdr = (netbios_header *)qpak->network_start;

	    if ((pak->if_input->hwptr == qpak->if_input->hwptr) &&
                (GETSHORT(&(netbios_hdr->rsp_correlator)) ==
                 GETSHORT(&(qnetbios_hdr->rsp_correlator))) &&
                (strncmp(netbios_hdr->srcname, qnetbios_hdr->srcname, 
                         NETBIOS_MAX_NAME_LEN) == 0)) {
             /*
              * Pak already queued. Update timer
              */
             TIMER_START(qpak->gp_timer, NETBIOS_NBF_QUEUE_TIMEOUT); 
             return(FALSE);
	    }
            break;    

          case NETBIOS_NAME_QUERY:
            qnetbios_hdr = (netbios_header *)qpak->network_start;

	    if ((pak->if_input->hwptr == qpak->if_input->hwptr) &&
                (GETSHORT(&(netbios_hdr->rsp_correlator)) ==
                 GETSHORT(&(qnetbios_hdr->rsp_correlator))) &&
                (GETSHORT(&(netbios_hdr->data2)) ==
                 GETSHORT(&(qnetbios_hdr->data2))) &&
                (strncmp(netbios_hdr->srcname, qnetbios_hdr->srcname, 
                         NETBIOS_MAX_NAME_LEN) == 0) &&
                (strncmp(netbios_hdr->destname, qnetbios_hdr->destname, 
                         NETBIOS_MAX_NAME_LEN) == 0)) {
             /*
              * Pak already queued. Update timer
              */
             TIMER_START(qpak->gp_timer, NETBIOS_NBF_QUEUE_TIMEOUT); 
             return(FALSE);
	    }
            break;

          case NETBIOS_NAME_RECOGNIZED:
            qnetbios_hdr = (netbios_header *)qpak->network_start;
	    if ((pak->if_input->hwptr == qpak->if_input->hwptr) &&
                (GETSHORT(&(netbios_hdr->xmit_correlator)) ==
                 GETSHORT(&(qnetbios_hdr->xmit_correlator))) &&
                (GETSHORT(&(netbios_hdr->rsp_correlator)) ==
                 GETSHORT(&(qnetbios_hdr->rsp_correlator))) &&
                (GETSHORT(&(netbios_hdr->data2)) ==
                 GETSHORT(&(qnetbios_hdr->data2))) &&
                (strncmp(netbios_hdr->srcname, qnetbios_hdr->srcname, 
                         NETBIOS_MAX_NAME_LEN) == 0) &&
                (strncmp(netbios_hdr->destname, qnetbios_hdr->destname, 
                         NETBIOS_MAX_NAME_LEN) == 0)) {
             /*
              * Pak already queued. Update timer and data2 field
              * (session number)
              */
             TIMER_START(qpak->gp_timer, NETBIOS_NBF_QUEUE_TIMEOUT);
             return(FALSE); 
	    }
            break; 

          case NETBIOS_STATUS_QUERY:
            qnetbios_hdr = (netbios_header *)qpak->network_start;
	    if ((pak->if_input->hwptr == qpak->if_input->hwptr) &&
                (GETSHORT(&(netbios_hdr->rsp_correlator)) ==
                 GETSHORT(&(qnetbios_hdr->rsp_correlator))) &&
                (strncmp(netbios_hdr->srcname, qnetbios_hdr->srcname, 
                         NETBIOS_MAX_NAME_LEN) == 0) &&
                (strncmp(netbios_hdr->destname, qnetbios_hdr->destname, 
                         NETBIOS_MAX_NAME_LEN) == 0)) {
             /*
              * Pak already queued. Update timer and data2 field
              * (session number)
              */
             TIMER_START(qpak->gp_timer, NETBIOS_NBF_QUEUE_TIMEOUT);
             return(FALSE); 
	    }
            break; 

          case NETBIOS_STATUS_RESPONSE:
            qnetbios_hdr = (netbios_header *)qpak->network_start;
	    if ((pak->if_input->hwptr == qpak->if_input->hwptr) &&
                (GETSHORT(&(netbios_hdr->xmit_correlator)) ==
                 GETSHORT(&(qnetbios_hdr->xmit_correlator))) &&
                (strncmp(netbios_hdr->srcname, qnetbios_hdr->srcname, 
                         NETBIOS_MAX_NAME_LEN) == 0) &&
                (strncmp(netbios_hdr->destname, qnetbios_hdr->destname, 
                         NETBIOS_MAX_NAME_LEN) == 0)) {
             /*
              * Pak already queued. Update timer and data2 field
              * (session number)
              */
             TIMER_START(qpak->gp_timer, NETBIOS_NBF_QUEUE_TIMEOUT);
             return(FALSE); 
	    }
            break; 

          case NETBIOS_SESSION_INITIALIZE:
            qnetbios_hdr_sess = (netbios_header_sess *)qpak->network_start;
            netbios_hdr_sess = (netbios_header_sess *)netbios_hdr;
	    if ((pak->if_input->hwptr == qpak->if_input->hwptr) &&
                (GETSHORT(&(netbios_hdr->xmit_correlator)) ==
                 GETSHORT(&(qnetbios_hdr->xmit_correlator))) &&
                (GETSHORT(&(netbios_hdr->rsp_correlator)) ==
                 GETSHORT(&(qnetbios_hdr->rsp_correlator))) &&
                (netbios_hdr_sess->destnum == qnetbios_hdr_sess->destnum) &&
                (netbios_hdr_sess->srcnum == qnetbios_hdr_sess->srcnum)) {
             /*
              * Pak already queued. Update timer. 
              */
             TIMER_START(qpak->gp_timer, NETBIOS_NBF_QUEUE_TIMEOUT); 
             return(FALSE);
	    }
            break;           
           
          default:
            buginf("\nnbf error in nbf_check_and_enqueue");
            return(FALSE); 
	  } /* end switch */
       }
       qpak_prev = qpak;
       qpak = qpak->next;
    } /* end while */
   
    /*
     * Queue has reached allowed length for single sweep and match 
     * was still not found. Return FALSE so that caller will 
     * discard the original pak. This should be OK since the 
     * remote end will retransmit.
     */
    if (count == 0)
      return(FALSE);

    /*
     * No match on queue (could be an empty queue).
     * Stick the specified pak in the queue
     */
    TIMER_START(pak->gp_timer, NETBIOS_NBF_QUEUE_TIMEOUT); 
    enqueue(queue, pak);
    
    return(TRUE);    
}

 
/*
 * nbf_check_and_dequeue
 * If a pak in the specified queue matches the the specified pak,
 * the pak in the queue is removed and returned to caller. 
 */
static paktype *nbf_check_and_dequeue (paktype *pak, netbios_header *netbios_hdr)
{
    queuetype *queue;
    paktype *qpak, *temp_pak, *qpak_prev;
    netbios_header_sess *qnetbios_hdr_sess, *netbios_hdr_sess; 
    netbios_header *qnetbios_hdr;
    int count = PAK_SWITCHCOUNT;

    switch (netbios_hdr->command) {
       case NETBIOS_NAME_RECOGNIZED:
            queue = &netbios_name_queryQ;
            break;
	  case NETBIOS_SESSION_INITIALIZE:
            queue = &netbios_name_recognizedQ;
            break;
          case NETBIOS_SESSION_CONFIRM:
            queue = &netbios_session_initializeQ;
            break;
          case NETBIOS_ADD_NAME_RESPONSE:
            queue = &netbios_add_nameQ;
            break;
          case NETBIOS_STATUS_RESPONSE:
            queue = &netbios_status_queryQ;
            break;
          case NETBIOS_STATUS_QUERY:
            queue = &netbios_status_responseQ;
            break;
          default:
            return(NULL);
    }
       
    /*
     * Search the specified queue for a match. It there is one, 
     * remove it and return to caller.
     * The match conditions depend on the NetBIOS command type.
     * Also, do some cleaning up if timer of the queued pak has expired.
     * Use a safeguard count to avoid being a CPUHOG  
     */    
    qpak_prev = NULL;
    qpak = queue->qhead;
    while (qpak && (count-- >0)) {
      /*
       * If the pak has expired, discard it 
       */
      if (TIMER_RUNNING_AND_AWAKE(qpak->gp_timer)) {
         temp_pak = qpak->next;
         remqueue(queue, qpak, qpak_prev);
         retbuffer(qpak);
         qpak = temp_pak;
         continue;
      } else {
         switch (netbios_hdr->command) {
 	  case NETBIOS_ADD_NAME_RESPONSE:
	  case NETBIOS_STATUS_RESPONSE:
	   qnetbios_hdr = (netbios_header *)qpak->network_start;
	    if ((GETSHORT(&(netbios_hdr->xmit_correlator)) ==
                 GETSHORT(&(qnetbios_hdr->rsp_correlator))) &&
                (strncmp(netbios_hdr->destname, qnetbios_hdr->srcname, 
                         NETBIOS_MAX_NAME_LEN) == 0)) {

             
             /*
              * Match found. Remove and return it.
              */
             remqueue(queue, qpak, qpak_prev);
             return(qpak);
	    }
            break; 

          case NETBIOS_STATUS_QUERY:
            qnetbios_hdr = (netbios_header *)qpak->network_start;
	    if ((pak->if_input->hwptr == qpak->if_output->hwptr) &&
                (strncmp(netbios_hdr->destname, qnetbios_hdr->srcname, 
                         NETBIOS_MAX_NAME_LEN) == 0)) {
             /*
              * Match found. Remove and return it.
              */
             remqueue(queue, qpak, qpak_prev);
             return(qpak);
	    }
            break; 

          case NETBIOS_NAME_RECOGNIZED:
            qnetbios_hdr = (netbios_header *)qpak->network_start;
	    if ((GETSHORT(&(netbios_hdr->xmit_correlator)) ==
                 GETSHORT(&(qnetbios_hdr->rsp_correlator))) &&
                (strncmp(netbios_hdr->srcname, qnetbios_hdr->destname, 
                         NETBIOS_MAX_NAME_LEN) == 0) &&
                (strncmp(netbios_hdr->destname, qnetbios_hdr->srcname, 
                         NETBIOS_MAX_NAME_LEN) == 0)) {
             /*
              * Match found. Remove and return it.
              */
             remqueue(queue, qpak, qpak_prev);
             return(qpak);
	    }
            break; 

          case NETBIOS_SESSION_INITIALIZE:
            qnetbios_hdr_sess = (netbios_header_sess *)qpak->network_start;
            netbios_hdr_sess = (netbios_header_sess *)netbios_hdr;
	    if ((pak->if_input->hwptr == qpak->if_output->hwptr) &&
                (netbios_hdr_sess->destnum == qnetbios_hdr_sess->data2[0])) {
             /*
              * Match found. Remove and return it.
              */
             remqueue(queue, qpak, qpak_prev);
             return(qpak);
	    }
            break;

          case NETBIOS_SESSION_CONFIRM:
            qnetbios_hdr_sess = (netbios_header_sess *)qpak->network_start;
            netbios_hdr_sess = (netbios_header_sess *)netbios_hdr;
	    if ((pak->if_input->hwptr == qpak->if_output->hwptr) &&
                (netbios_hdr_sess->destnum == qnetbios_hdr_sess->srcnum) &&
                (netbios_hdr_sess->destnum == qnetbios_hdr_sess->srcnum)) {
             /*
              * Match found. Remove and return it.
              */
             remqueue(queue, qpak, qpak_prev);
             return(qpak);
	    }
            break;     
           
          default:
            buginf("\nnbf error in nbf_check_and_dequeue");
            return(NULL); 
	  } /* end switch */
       }
       qpak_prev = qpak;
       qpak = qpak->next;
    } /* end while */
   
    /*
     * Queue has reached allowed length for single sweep and match 
     * was still not found. Return NULL so that caller will 
     * discard the original pak. This should be OK since the 
     * remote end will retransmit.
     */
    return(NULL);  
}


/*
 * nbf_llc2_open - Open a LLC2 connection asynchronously
 */
static void nbf_llc2_open(paktype *pak, int advance, hwaddrtype *destaddr)
{
   paktype *newpak;
   hwaddrtype srcmacaddr;

   /*
    * Duplicate the SESSION_INITIALIZE pak and queue it to the 
    * pending queue. 
    */
   newpak = pak_duplicate(pak);

   if (!newpak)
      return;

   /*
    * Use a NULL source address. 
    * Non-CLS SAPs with non-NULL address causes mismatching LLCs
    */
   ieee_zero((uchar *)&srcmacaddr.addr);

   /*
    * Open the LLC2 connection, storing the LLC handle in the queued pak
    */
   newpak->llc2_cb = llc2_open(SAP_NETBIOS, SAP_NETBIOS, destaddr, &srcmacaddr,
             pak->if_output, LINK_NBF, 127, nbf_llc2_link_state_change, 
             FALSE, FALSE);

   /*
    * Set the llc cleanup callback vector to nbf_cleanup_llc
    */ 
   newpak->llc2_cb->media_llc_cleanup = nbf_cleanup_llc;

   /*
    * Always normalize the netbios header to be pointed to by 
    * pak->network_start
    */
   newpak->network_start += advance;

   /*
    * Queue the pak 
    */
   TIMER_START(newpak->gp_timer, NETBIOS_NBF_QUEUE_TIMEOUT); 
   enqueue(&netbios_pending_session_initializeQ, newpak);
  
}


/*
 * nbf_llc2_link_state_change
 * LLC2 Link Change notification for new connections, changes in LLC2 
 * link state, etc. 
 */
static void nbf_llc2_link_state_change (llctype *llc, int reason)
{
    uchar *what;

    what = llc2_prreason(reason);

    if (netbios_packet_debug || netbios_error_debug)
       buginf("\n%s nbf LLC2 link state change reason = %s", 
              llc->idb->hwptr->hw_namestring, what);

    switch (reason) {
      case LC_CONNECT:
          nbf_action_pending_session_init(llc, TRUE);
          break;
      case LC_DM:
      case LC_DISC_LAN:
      case LC_DISC_CNF:
      case LC_TIMEOUT:
      case LC_INTDOWN:
      case LC_RESET:
      case LC_ABORTED:
        /*
         * Discard any paks in pending SESSION_INITIALIZE queue that 
         * matches this LLC
         */
        nbf_action_pending_session_init(llc, FALSE);

        /* 
         * Turn off nbf (but preserve the table allocation).
         * LLC2 is dying/dead so don't bother notifying NetBIOS
         * sessions on this interface. However notify the 
         * corresponding remote NetBIOS of disconnect
         */
       if (IS_NETBIOS_ACCESS(llc->idb->hwptr)) 
          nbf_reset_session_table(llc->idb->hwptr, FALSE, FALSE, TRUE);
       else 
          nbf_reset_lan_hash_table(llc->idb->hwptr, FALSE, FALSE, TRUE);
	break;

      default:
	return;
   }
}



/*
 * nbf_cleanup_llc
 * LLC2 callback to close NBF sessions on this LLC2 connection
 */
static void nbf_cleanup_llc (llctype *llc)
{
  if (!llc) {
     if (netbios_error_debug)
        buginf("\nnbf nbf_cleanup_llc null llc pointer");
     return;
  }

  if (netbios_packet_debug || netbios_error_debug)
     buginf("\nnbf nbf_cleanup_llc %s on %s",
            llc2_prreason(llc->adm_reason), llc->idb->hwptr->hw_namestring);
       
  /*
   * Discard any paks in pending SESSION_INITIALIZE queue that 
   * matches this LLC
   */
  nbf_action_pending_session_init(llc, FALSE);

  /* 
   * Turn off nbf (but preserve the table allocation).
   * LLC2 is dying/dead so don't bother notifying NetBIOS
   * sessions on this interface. However notify the 
   * corresponding remote NetBIOS of disconnect
   */
   if (IS_NETBIOS_ACCESS(llc->idb->hwptr)) 
      nbf_reset_session_table(llc->idb->hwptr, FALSE, FALSE, TRUE);
   else 
      nbf_reset_lan_hash_table(llc->idb->hwptr, FALSE, FALSE, TRUE);
}


/*
 * netbios_as_if_state_change
 *
 * This routine will be called any time an interface has a state change.
 * Check to see if it is a change might should netbios, and if so, effect
 * the necessary changes.
 */
static void netbios_as_if_state_change (hwidbtype *hwidb)
{
    /*
     * If this is an Access Interface configured for NBF 
     * and the state is not UP, delete the session table (this session 
     * table is later created on demand)
     */
    if (IS_NETBIOS_ACCESS(hwidb) && 
        netbios_sb_access_server_is_enabled(hwidb->firstsw) &&
        (hwidb->state != IDBS_UP)) {
       nbf_reset_session_table(hwidb, TRUE, FALSE, TRUE); 
    }
}

/*
 * nbf_action_pending_session_init
 * Search and delete a pak in the pending SESSION_INITIALIZE queue that matches
 * the specified llc connection.
 * Also send a pak out if so requested. 
 */
static void nbf_action_pending_session_init (llctype *llc, boolean send)
{
    paktype *qpak, *prev_pak, *newpak;

    /*
     * Search the pending SESSION_INITIALIZE queue for the correct
     * match. On a match, send the queued pak out on the newly opened
     * LLC2 connection
     */
    prev_pak = NULL;
    qpak = netbios_pending_session_initializeQ.qhead;  
    while (qpak) {
       if ((qpak->if_output == llc->idb) && (qpak->llc2_cb == llc)) {
          remqueue(&netbios_pending_session_initializeQ, qpak, prev_pak);
             
          if (netbios_packet_debug)
                  buginf("\n%s nbf target LLC2 Open dstidb=%s", 
                         qpak->if_input->hwptr->hw_namestring, 
                         qpak->if_output->hwptr->hw_namestring);
           /*
            * If requested, duplicate and queue it to the SESSION_INITIALIZE 
            * queue and send the SESSION_INITIALIZE IFRAME out.
            * LLC2 will discard the packet.
            */
           if (send) {
              newpak = pak_duplicate(qpak);

              if (!newpak)
                 return;
              
              if (!nbf_check_and_enqueue(newpak, 0, FALSE)) {
                 retbuffer(newpak);
                 retbuffer(qpak);
                 return;
	      }
              llc2_send(llc, qpak, qpak->datagramsize);
              return;
	   }

           retbuffer(qpak);
           return;
	}
        prev_pak = qpak;
        qpak = qpak->next;
     }
}


/*
 * netbios_as_if_command - Enable/Disable NBF per interface
 */
void netbios_as_if_command (parseinfo *csb) 
{
   idbtype *idb;
   netbios_sb_t *netb_sb;

   idb = csb->interface;

   if (system_loading)
      return;

   netb_sb = netbios_get_or_create_sb(idb);
   if (!netb_sb) {
       printf(nomemory);
       return;
   }

   /*
    * Take care of NV generation chores
    */
   if (csb->nvgen) {
      nv_write(netb_sb->access_server_enabled, "%s", csb->nv_command);
      return;
   }

   /*
    * Turn nbf on if not already on.
    * Otherwise turn off nbf, unless already off
    */
   if (csb->sense & !netb_sb->access_server_enabled) {
      /*
       * Start LLC2 and register 
       */
      llc2_start();
      llc2_register(SAP_NETBIOS, LINK_NBF, nbf_register);

      netb_sb->access_server_enabled = TRUE;
      /*
       * Create a session table only if it is a LAN interface.
       * Otherwise, for Access interfaces, it is created on-the-fly
       * The reason why LAN interfaces don't use this is because doing 
       * on-the-fly is much harder to determine. Plus the memory savings 
       * are on the much more numerous access interfaces anyway.
       */
      if (IS_NETBIOS_LAN(idb->hwptr)) 
         nbf_prep_interface(idb);

      if (idb->hwptr->set_extra_addrs)
         (*idb->hwptr->set_extra_addrs)(idb->hwptr);
  
   } else if (!csb->sense & netb_sb->access_server_enabled) {
     /* 
      * Turn off nbf and notify NetBIOS session on both ends of 
      * disconnect. Also deallocate tables. 
      */
     if (IS_NETBIOS_ACCESS(idb->hwptr)) 
       nbf_reset_session_table(idb->hwptr, TRUE, TRUE, TRUE);
     else 
       nbf_reset_lan_hash_table(idb->hwptr, TRUE, TRUE, TRUE);
     netb_sb->access_server_enabled = FALSE;

   }
}


/*
 * show_netbios_nbf - Display NetBIOS NBF information
 */
void show_netbios_nbf (parseinfo *csb) 
{
   /* 
    * Display per interface and global nbf info
    */ 
   display_netbios_interfaces();

   /*
    * Display the queue sizes
    */
   printf("\nADD_[GROUP]NAME_QUERY queuesize=%d",
          QUEUESIZE(&netbios_add_nameQ));
   printf("\nSTATUS_QUERY queuesize=%d", 
          QUEUESIZE(&netbios_status_queryQ));
   printf("\nSTATUS_RESPONSE queuesize=%d", 
          QUEUESIZE(&netbios_status_responseQ));
   printf("\n\nNAME_QUERY queuesize=%d",
          QUEUESIZE(&netbios_name_queryQ));
   printf("\nNAME_RECOGNIZED queuesize=%d",
          QUEUESIZE(&netbios_name_recognizedQ));
   printf("\nSESSION_INITIALIZE queuesize=%d",
          QUEUESIZE(&netbios_session_initializeQ));
   printf("\nSESSION_INITIALIZE (pending) queuesize=%d",
          QUEUESIZE(&netbios_pending_session_initializeQ));

}


/*
 * nbf_create_netbios_add_name_query
 * Manufacture a NetBIOS ADD_NAME_QUERY or ADD_GROUP_QUERY packet
 */
static paktype *nbf_create_netbios_add_name_query (idbtype *idb, uchar *name, 
uchar nbfcp_type)
{
   paktype *pak;
   int i;

   pak = getbuffer(SMALLDATA);

   if (!pak)
     return(NULL);
   
   memset(pak->datagramstart, 0, sizeof(netbios_header) + SAP_HDRBYTES);
   
   /*
    * LLC1 Header 
    */
   *pak->datagramstart = SAP_NETBIOS;
   *(pak->datagramstart+1) = SAP_NETBIOS;
   *(pak->datagramstart+2) = UI;

   /*
    * Move up to NetBIOS header 
    */
   pak->datagramstart += SAP_HDRBYTES;
   pak->network_start = pak->datagramstart;
   ((netbios_header *)pak->datagramstart)->header_length[0] = sizeof(netbios_header);
   ((netbios_header *)pak->datagramstart)->delimiter[0] = NETBIOS_DELIMITER_0;
   ((netbios_header *)pak->datagramstart)->delimiter[1] = NETBIOS_DELIMITER_1;
   ((netbios_header *)pak->datagramstart)->rsp_correlator[0] = 111;

   for (i=0; i < NETBIOS_MAX_NAME_LEN; i++)
     ((netbios_header *)pak->datagramstart)->srcname[i] = name[i];

   if (nbfcp_type == NBFCP_NAME_PROJ_UNIQUE_NAME)
     ((netbios_header_sess *)pak->datagramstart)->command = NETBIOS_ADD_NAME_QUERY;
   else
     ((netbios_header_sess *)pak->datagramstart)->command = NETBIOS_ADD_GROUP_QUERY;

   pak->if_input = idb;
   pak->flags |= PAK_BROADCAST;
   pak->enctype = pak->llc2_enctype = ET_SAP;
   pak->datagramsize = sizeof(netbios_header);
   return(pak);
}


/*
 * nbf_xmit_netbios_session_end
 * Manufacture and send a NetBIOS SESSION_END packet with HANGUP 
 * termination code. 
 */
void nbf_xmit_netbios_session_end (uchar srcnum, uchar dstnum, uchar *destmac, 
idbtype *dstidb)
{
   paktype *pak;
   llctype *llc;
   uchar macaddr[IEEEBYTES];

   /*
    * A NULL destination MAC address requires we supply a real zeroed 
    * address
    */ 
   if (!destmac) {
      ieee_zero(macaddr);
      destmac = macaddr;
   }

   /*
    * Locate the LLC for this interface and destination MAC address
    */
   llc = llc2_find_cb(destmac, SAP_NETBIOS, SAP_NETBIOS, dstidb);

   /*
    * We either did not find it or found the LLC in a state that we cannot 
    * send a packet
    */
   if (!llc || (llc->state != LLS_NORMAL))
     return;

   pak = getbuffer(SMALLDATA);

   if (!pak)
     return;
   
   memset(pak->datagramstart, 0, sizeof(netbios_header_sess));

   ((netbios_header_sess *)pak->datagramstart)->header_length[0] = sizeof(netbios_header_sess);
   ((netbios_header_sess *)pak->datagramstart)->delimiter[0] = NETBIOS_DELIMITER_0;
   ((netbios_header_sess *)pak->datagramstart)->delimiter[1] = NETBIOS_DELIMITER_1;
   ((netbios_header_sess *)pak->datagramstart)->srcnum = srcnum;
   ((netbios_header_sess *)pak->datagramstart)->destnum = dstnum;
   ((netbios_header_sess *)pak->datagramstart)->data2[1] = NETBIOS_HANGUP;
   ((netbios_header_sess *)pak->datagramstart)->command = NETBIOS_SESSION_END;

   pak->datagramsize = sizeof(netbios_header_sess);

   llc2_send(llc, pak, pak->datagramsize);
}

SUBSYS_HEADER(netbios_as_name, NBF_MAJVERSION, NBF_MINVERSION, NBF_EDITVERSION,
	      netbios_as_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: netbios_acl",
	      "req: netbios_acl");
