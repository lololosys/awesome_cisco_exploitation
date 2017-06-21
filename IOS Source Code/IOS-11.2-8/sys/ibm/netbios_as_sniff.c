/* $Id: netbios_as_sniff.c,v 3.3.20.1 1996/03/18 20:11:49 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/ibm/netbios_as_sniff.c,v $
 *------------------------------------------------------------------
 * NBF (NetBIOS Frames or NetBEUI) Access Server support
 *
 * September 1995, Shoou Yiu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: netbios_as_sniff.c,v $
 * Revision 3.3.20.1  1996/03/18  20:11:49  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:42:14  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  00:54:41  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/01  06:03:14  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1995/11/17  09:23:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:02:12  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  00:32:23  syiu
 * Initial placeholder for NBF.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "fastswitch_registry.h"
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
#include "../ibm/netbios.h"
#include "../llc2/llc2.h"
#include "../if/tring.h"
#include "../if/rif.h"
#include "../ibm/parser_defs_netbios.h"
#include "../if/network.h"
#include "../srt/srb.h"
#include "../srt/rsrb.h"
#include "../ui/debug.h"
#include "../ibm/netbios_as_debug.h"
#include "../ibm/netbios_as.h"

/* 
 * Function prototypes in this module 
 */
int nbf_llc_sniff(uchar *, paktype *);
boolean nbf_netbios_sniff(uchar *);
static void nbf_display_netbios(int, uchar *, uchar *, int, int, int,int, int, int,int, int, int);

/*
 * External references
 */
extern boolean netbios_packet_debug;
extern boolean netbios_error_debug;


/*
 * nbf_llc_sniff
 * Returns LLC1 or LLC2 information
 * Input:
 *  - llc_start - pointer to beginning of LLC data (DSAP SSAP CONTROL..)
 *  - pak - pointer to packet
 * Output:
 *  - Via function return - Size to advance from llc_start past
 *    the LLC Control Field Bits. Value of 0 indicates illegal LLC 
 *    header. 
 *  - command - LLC command
 *  - c_r - Command(0) or Response(1) command 
 *  - format - Unnumbered, Supervisory or Information LLC format
 *  - ns - Xmitter send sequence
 *  - nr - Xmitter receive sequence
 *  - poll - P/F bit (0/1) 
 */
int nbf_llc_sniff (uchar *llc_start, paktype *pak)
{
    uchar command, c_r, format, ns, nr, poll; 
    uchar control, second_byte, third_byte, fourth_byte;
    int advance = SAP_HDRBYTES;

    /*
     * Is this a command or response LLC ?
     * SSAP with lsb clear indicates Response
     *  "    "     " set indicates Command
     */
    c_r = *(llc_start+1) & 1;
  
    /*
     * Check out LLC control byte
     * Offset is 2 to get past the DSAP & SSAP bytes
     */
    control = *(llc_start+2);
    second_byte = *(llc_start+3);
    third_byte = *(llc_start+4);
    fourth_byte = *(llc_start+5);


    /* 
     * Take out the P/F bit to get the command
     */
    command = control & ~LLC1_P;
    
    switch (command) {

      /*
       * U-format UI, XID, SABME, TEST, DISC, UA, FRMR
       */
      case UI:
           format = LLC_UNNUMBERED;
           poll = control & LLC1_P;

           if (netbios_packet_debug)
              buginf ("\n%s (i) U-format UI C_R=0x%x", 
                      pak->if_input->hwptr->hw_namestring, c_r);
           break;
      case XID:
           format = LLC_UNNUMBERED;
           poll = control & LLC1_P;

           if (netbios_packet_debug)
              buginf("\n%s (i) U-format XID C_R=0x%x xid=%x Type1=%d Type2=%d RcvWinSize=%d",
                 pak->if_input->hwptr->hw_namestring,
                 c_r, second_byte, (third_byte & XID_CLASS_TYPE1),
                 (third_byte & XID_CLASS_TYPE2),
                 (fourth_byte & XID_WIN_TYPE2)>>1);
           break;
      case LLC_TEST:
           format = LLC_UNNUMBERED;
           poll = control & LLC1_P;

           if (netbios_packet_debug)
              buginf("\n%s (i) U-format TEST C_R=0x%x",  
                      pak->if_input->hwptr->hw_namestring, c_r);
           break;
      case SABME:
           format = LLC_UNNUMBERED;
           poll = control & LLC1_P;

           if (netbios_packet_debug)
               buginf("\n%s (i) U-format SABME C_R=0x%x", 
                      pak->if_input->hwptr->hw_namestring, c_r);
           break;
      case DISC:
           format = LLC_UNNUMBERED;
           poll = control & LLC1_P;

           if (netbios_packet_debug)
              buginf("\n%s (i) U-format DISC C_R=0x%x",
                      pak->if_input->hwptr->hw_namestring, c_r);
           break;
      case UA:
           format = LLC_UNNUMBERED;
           poll = control & LLC1_P;

           if (netbios_packet_debug)
              buginf ("\n%s (i) U-format UA C_R=0x%x",
                      pak->if_input->hwptr->hw_namestring, c_r);
           return(SAP_HDRBYTES);
           break;
      case FRMR:
           format = LLC_UNNUMBERED;
           poll = control & LLC1_P;

           if (netbios_packet_debug)
              buginf ("\n%s (i) U-format FRMR C_R=0x%x",
                      pak->if_input->hwptr->hw_namestring, c_r);
           break;

      case DM:
           format = LLC_UNNUMBERED;
           poll = control & LLC1_P;

           if (netbios_packet_debug)
              buginf ("\n%s (i) U-format DM C_R=0x%x",
                      pak->if_input->hwptr->hw_namestring, c_r);
           break;

      /*
       * S-format RR, RNR, REJ
       */    
      case RR:
           format = LLC_SUPERVISORY;
           advance = SAP_LONGHDRBYTES;
           nr = (second_byte & N_MASK)>>1;
           poll = second_byte & PF_BIT;
             
           if (netbios_packet_debug) 
               buginf("\n%s (i) S-format RR C_R=0x%x P/F=%d NR=%d", 
                      pak->if_input->hwptr->hw_namestring,
                      c_r, poll, nr); 
              break;

      case RNR:
           format = LLC_SUPERVISORY;
           advance = SAP_LONGHDRBYTES;
           nr = (second_byte & N_MASK)>>1;
           poll = second_byte & PF_BIT;

           if (netbios_packet_debug) 
              buginf("\n%s (i) S-format RNR C_R=0x%x P/F=%d NR=%d",
                     pak->if_input->hwptr->hw_namestring, 
                     c_r, poll, nr); 
              break;

      case REJ:
           format = LLC_SUPERVISORY;
           advance = SAP_LONGHDRBYTES;
           nr = (second_byte & N_MASK)>>1;
           poll = second_byte & PF_BIT;

           if (netbios_packet_debug) 
              buginf("\n%s (i) S-format REJ C_R=0x%x P/F=%d NR=%d",
                    pak->if_input->hwptr->hw_namestring, 
                    c_r, poll, nr); 
              break;

      default:
       /*
        * If I-format, break down to N(S), N(R)
        * Double check to make sure it is an information transfer LLC 
        */
       if (!(command & 1)) {
         ns = (control & N_MASK)>>1;
         nr = (second_byte & N_MASK)>>1;
         poll = (second_byte & PF_BIT);
         format = LLC_IFRAME;
         advance = SAP_LONGHDRBYTES;              
         if (netbios_packet_debug)
         buginf("\n%s (i) I-format C_R=0x%x P/F=%d NS=%d NR=%d", 
                pak->if_input->hwptr->hw_namestring, 
                c_r, poll, ns, nr);  
         break;
       }

       /*
        * Fell thru to here - must be illegal LLC
        */ 
       if (netbios_packet_debug)
         buginf("\n%s (i) Unknown LLC format",
             pak->if_input->hwptr->hw_namestring); 
       return(0);
    }
    return(advance);
}


/*
 * nbf_netbios_sniff - Sniffer a NetBIOS packet
 */
boolean nbf_netbios_sniff (uchar *netbios_hdr)
{
    uchar command;

    command = *(netbios_hdr+4);
    switch(command) {
      case NETBIOS_ADD_GROUP_QUERY:
        nbf_display_netbios(LLC_UNNUMBERED,
                        "NETBIOS_ADD_GROUP_QUERY", netbios_hdr,
                        0,0,0,0,0,0,1,0,1);
        break;
      case NETBIOS_ADD_NAME_QUERY:
        nbf_display_netbios(LLC_UNNUMBERED,
                        "NETBIOS_ADD_NAME_QUERY", netbios_hdr,
                        0,0,0,0,0,0,1,0,1);
        break;
      case NETBIOS_NAME_IN_CONFLICT:
        nbf_display_netbios(LLC_UNNUMBERED,
                        "NETBIOS_NAME_IN_CONFLICT", netbios_hdr,
                        0,0,0,0,0,0,0,0,1);
        break;
      case NETBIOS_STATUS_QUERY:
        nbf_display_netbios(LLC_UNNUMBERED,
                        "NETBIOS_STATUS_QUERY", netbios_hdr,
                        0,0,0,1,1,0,1,1,1);
        break;
      case NETBIOS_TERMINATE_TRACE_REM:
        nbf_display_netbios(LLC_UNNUMBERED,
                        "NETBIOS_TERMINATE_TRACE_REM", netbios_hdr,
                        0,0,0,0,0,0,0,0,0);
        break;
      case NETBIOS_DATAGRAM:
        nbf_display_netbios(LLC_UNNUMBERED,
                        "NETBIOS_DATAGRAM", netbios_hdr,
                        1,0,0,0,0,0,0,1,1);
        break;
      case NETBIOS_DATAGRAM_BROADCAST:
        nbf_display_netbios(LLC_UNNUMBERED,
                        "NETBIOS_DATAGRAM_BROADCAST", netbios_hdr,
                        1,0,0,0,0,0,0,0,1);
        break;
      case NETBIOS_NAME_QUERY:
        nbf_display_netbios(LLC_UNNUMBERED,
                        "NETBIOS_NAME_QUERY", netbios_hdr,
                        0,0,0,0,1,0,1,1,1);
        break;
      case NETBIOS_ADD_NAME_RESPONSE:
        nbf_display_netbios(LLC_UNNUMBERED,
                        "NETBIOS_ADD_NAME_RESPONSE", netbios_hdr,
                        0,0,0,1,1,1,0,1,1);
        break;
      case NETBIOS_NAME_RECOGNIZED:
        nbf_display_netbios(LLC_UNNUMBERED,
                        "NETBIOS_NAME_RECOGNIZED", netbios_hdr,
                        0,0,0,0,1,1,1,1,1);
        break;
      case NETBIOS_STATUS_RESPONSE:
        nbf_display_netbios(LLC_UNNUMBERED,
                        "NETBIOS_STATUS_RESPONSE", netbios_hdr,
                        0,0,0,1,1,1,0,1,1);
        break;
      case NETBIOS_TERMINATE_TRACE_REM_LOC:
        nbf_display_netbios(LLC_UNNUMBERED,
                        "NETBIOS_TERMINATE_TRACE_REM_LOC", netbios_hdr,
                        0,0,0,0,0,0,0,0,0);
        break;
      case NETBIOS_DATA_ACK:
        nbf_display_netbios(LLC_IFRAME,
                        "NETBIOS_DATA_ACK", netbios_hdr,
                        0,0,0,0,0,1,0,1,1);
        break;
   case NETBIOS_DATA_FIRST_MIDDLE:
        nbf_display_netbios(LLC_IFRAME,
                        "NETBIOS_DATA_FIRST_MIDDLE", netbios_hdr,
                        1,0,0,1,1,1,1,1,1);
        break;
   case NETBIOS_ONLY_LAST:
        nbf_display_netbios(LLC_IFRAME,
                        "NETBIOS_ONLY_LAST", netbios_hdr,
                        1,0,0,1,1,1,1,1,1);
        break;
   case NETBIOS_SESSION_CONFIRM:
        nbf_display_netbios(LLC_IFRAME,
                        "NETBIOS_SESSION_CONFIRM", netbios_hdr,
                        0,0,0,1,1,1,1,1,1);
        break;
   case NETBIOS_SESSION_END:
        nbf_display_netbios(LLC_IFRAME,
                        "NETBIOS_SESSION_END", netbios_hdr,
                        0,0,0,0,1,0,0,1,1);
        break;
   case NETBIOS_SESSION_INITIALIZE:
        nbf_display_netbios(LLC_IFRAME,
                        "NETBIOS_SESSION_INITIALIZE", netbios_hdr,
                        0,0,0,1,1,1,1,1,1);
        break;
   case NETBIOS_NO_RECEIVE:
        nbf_display_netbios(LLC_IFRAME,
                        "NETBIOS_NO_RECEIVE", netbios_hdr,
                        0,0,0,1,1,0,0,1,1);
        break;
   case NETBIOS_RECEIVE_OUTSTANDING:
        nbf_display_netbios(LLC_IFRAME,
                        "NETBIOS_RECEIVE_OUTSTANDING", netbios_hdr,
                        0,0,0,0,1,0,0,1,1);
        break;
   case NETBIOS_RECEIVE_CONTINUE:
        nbf_display_netbios(LLC_IFRAME,
                        "NETBIOS_RECEIVE_CONTINUE", netbios_hdr,
                        0,0,0,0,0,1,0,1,1);
        break;
   case NETBIOS_SESSION_ALIVE:
        nbf_display_netbios(LLC_IFRAME,
                        "NETBIOS_SESSION_ALIVE", netbios_hdr,
                        0,0,0,0,0,0,0,0,0);
        break;
   default:
     buginf("\n(i) Bad NetBIOS packet command=0x%x", command);
     return(FALSE);
 }
 return(TRUE);  
}


/*
 * nbf_display_netbios - Display NetBIOS packet
 */ 
static void nbf_display_netbios (int format, uchar *cmd_string, uchar *packet, 
                        int len, int delimiter, int command,
                        int data1, int data2, int xmit_ctor,
                        int rsp_ctor, int dest, int src)
{

  netbios_header *p;
  int i;

  p = (netbios_header *) packet;
  buginf("\n(i) %s", cmd_string);
  if (len)
   buginf("\n  Length= 0x%x 0x%x", p->header_length[0],  p->header_length[1]);
  if (delimiter)
   buginf("\n  Delimiter= 0x%x 0x%x", p->delimiter[0], p->delimiter[1]);
  if (command)
   buginf("\n  Command= 0x%x", p->command);
  if (data1)
   buginf("\n  Data1= 0x%x", p->data1);
  if (data2)
   buginf("\n  Data2= 0x%x 0x%x", p->data2[0], p->data2[1]);
  if (xmit_ctor)
   buginf("\n  Xmit_correlator= 0x%x 0x%x", p->xmit_correlator[0], 
           p->xmit_correlator[1]);
  if (rsp_ctor)
   buginf("\n  Resp_correlator= 0x%x 0x%x", p->rsp_correlator[0], 
           p->rsp_correlator[1]);

  if (format == LLC_UNNUMBERED) {
     if (dest) {
       buginf("\n  Dest name=");
       for (i=0;i<NETBIOS_MAX_NAME_LEN;i++) {
         buginf("%c", p->destname[i]);
       }
     }

     if (src) {
       buginf("\n  Src  name=");
       for (i=0;i<NETBIOS_MAX_NAME_LEN;i++) {
        buginf("%c", p->srcname[i]);
       }
     }
  } else {
     if (dest) 
       buginf("\n  Dest session #= 0x%x", ((netbios_header_sess *)p)->destnum);

     if (src)
       buginf("\n  Src session #= 0x%x", ((netbios_header_sess *)p)->srcnum);
  }
  buginf("\n");
}
