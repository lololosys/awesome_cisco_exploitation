/* $Id: lane_aip_fast.h,v 3.4.4.6 1996/08/27 02:07:35 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/lane/lane_aip_fast.h,v $
 *------------------------------------------------------------------
 * lane_aip_fast.h: AIP Fast level code for LAN Emulation
 *
 * May 1995, Steve Jay
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lane_aip_fast.h,v $
 * Revision 3.4.4.6  1996/08/27  02:07:35  rbadri
 * CSCdi67231:  Topology change messages hinder LANE client debugging
 * Branch: California_branch
 * Dispose Topology change messages at fast path, if there is no
 * change or if it is from ourself.
 *
 * Revision 3.4.4.5  1996/08/26  18:20:11  rbadri
 * CSCdi67142:  Fastswitching routine should bridge Non LANE Control
 * packets only
 * Branch: California_branch
 * Look for bridgeable packets only if it is not a LANE Control packet.
 *
 * Revision 3.4.4.4  1996/08/08  20:08:47  rbadri
 * CSCdi65507:  Lines in lane_aip_fast.h mangled in previous commit
 * Branch: California_branch
 * Move the 2 lines to the if statement.
 *
 * Revision 3.4.4.3  1996/08/02  00:27:33  rbadri
 * CSCdi63540:  Bridging NetBEUI over LANE from 4500 doesnt work
 * Branch: California_branch
 * lane_aip_fast() should also have swidb in its parameter list.
 * Return LANE_DO_NOT_ROUTE for multicast packets of non-routed
 * protocols so that they can bridged at the fast level.
 *
 * Revision 3.4.4.2  1996/05/09  14:32:13  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.4.4.1.18.1  1996/04/27  07:06:08  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.4.4.1.8.4  1996/04/16  02:44:08  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.4.4.1.8.3  1996/04/12  01:52:42  ddecapit
 * Branch: LE_Cal_V112_0_2_branch
 *
 * - Rip C7000 support of Token Ring LANE until ucode is ready
 * - Modify indentation futher to placate the masses
 *
 * Revision 3.4.4.1.8.2  1996/04/08  14:51:32  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.4.4.1.8.1  1996/04/08  01:56:34  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.4.4.1  1996/03/18  20:32:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.8.1  1996/03/22  22:53:52  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.4  1996/03/08  23:31:40  shj
 * CSCdi50945:  LANE client does not handle data arriving on multicast-send
 * Treat packets coming in on mcast-send same as mcast-forward.
 *
 * Revision 3.3.42.1  1996/03/05  06:42:36  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.3.32.1  1996/02/27  21:09:58  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3.12.2  1996/01/23  23:54:00  cakyol
 * HSRP over LANE Code review changes:
 * - "interface_up" now works properly with HSRP after
 *   Badri's network.c fix.
 * - additional HSRP mac addresses implemented much
 *   faster on the LANE client.
 * - starting an already running or stopping an already
 *   stopped LANE client now behaves properly.
 * - white space cleanup
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.12.1  1996/01/11  01:46:04  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.8.1  1995/12/29  01:06:28  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.3  1995/11/20  22:41:58  shj
 * CSCdi44213:  ping to self via LANE fails
 * Do lecid check only for packets arriving via BUS, not via Data Directs.
 *
 * Revision 3.2  1995/11/17  17:50:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:12:19  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  21:18:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#define LANE_REFLECT(reg, hwidb, vcid)                  \
{                                                       \
  reg->argreg = hwidb->mci_index;                       \
  reg->cmdreg = MCI_CMD_RX_MOVE;                        \
  if (reg->cmdreg != MCI_RSP_OKAY) {                    \
    hwidb->counters.output_total_drops++;               \
    FLUSH_AND_RETURN_DONE;                              \
  }                                                     \
                                                        \
  reg->argreg = hwidb->rxoffset;                        \
  reg->cmdreg = MCI_CMD_TX1_SELECT;                     \
  reg->write1short = vcid;                              \
  reg->write1short = AIP_DM_EOM_EN | AIP_DM_PKT_AAL5;   \
                                                        \
  reg->argreg = hwidb->rxlength;                        \
  reg->cmdreg = MCI_CMD_TX1_START;                      \
  hwidb->counters.outputs_fast[ACCT_PROTO_OTHER]++;	\
  hwidb->counters.tx_cumbytes_fast[ACCT_PROTO_OTHER] 	\
	       += hwidb->rxlength; 			\
}


#define FLUSH_AND_RETURN_DONE                   \
{                                               \
  regptr->cmdreg = MCI_CMD_RX_FLUSH;            \
  return(LANE_ALREADY_DONE);                    \
}


/*
 * aip_lane_fast
 */

static LANE_FAST_ACTION
lane_aip_fast (hwidbtype *hwidb, idbtype *swidb) 
{
  union {
    ulong                l[2];
    lane_data_hdr_t      hdr;
    lane_dest_t          le_arp_dst;
  } sniff_le_dat;

  lane_info_t           *lane;
  lsv_info_t            *lsv;
  lec_info_t            *client;
  vc_info_t             *vc;
  ushort                 op;
  ushort                 summary;
  mciregtype            *regptr;
 
  vc = hwidb->atm_db->fast_vc_info;
  
  /* Check for NULL vc done in CBUS_AIP_RECEIVE */

  if (vc->swidb->lane_info) {

    lane = vc->swidb->lane_info;
    regptr = (mciregtype *)hwidb->mci_regptr;

    switch(vc->user_vc_type) {
      
    case LANE_BUS_SEND:
      
      /* For all packets arriving at BUS on a multicast send vcc, reflect
       * out on multicast forward vcc.  This implements a very dumb BUS,
       * with all packets handled at fast level.  If a smart BUS is
       * implemented, this will have to be changed.
       *
       * We do count data packets sent to unicast MAC, because too many
       * of these could mean some client is using the BUS too much. 
       */

      lsv  = lane->lsv_info;
      if (lsv == NULL)
          return(LANE_NORMAL);
      regptr->argreg = hwidb->rxoffset +
        btow(LANE_DATA_ID_ETHER_OFFSET + sizeof(atm_encap_t));
      regptr->cmdreg = FCI_CMD_RX_SELECT;
      sniff_le_dat.l[0] = regptr->readlong;

      if (!IS_LANE_CTL(sniff_le_dat.hdr.lec_id)) {
          if (IS_ETHER_UNICAST(sniff_le_dat.hdr.lan_type.ether.daddr))
            lsv->buss.busUnicastForwards++;
      }


      lsv->buss.busBytes += hwidb->rxlength;
      lsv->buss.busInFrames++;
      lsv->buss.busOutFrames++;

      LANE_REFLECT(regptr, hwidb, lsv->forward_vcc.vcd);
      return(LANE_ALREADY_DONE);
      
    case LANE_LEC_FORWARD:
    case LANE_LEC_SEND:
    case LANE_LEC_DATA:
      /*
       * For packets arriving at LEC on multicast-forward, or  multicast-
       * send, prevent loops by tossing packets with (lecid == me).  We
       * allow (lecid == me) on data-direct so pings to self work.
       *
       * For all data packets, regardless of which vc they arrived on,
       * if unicast to someone else (not my mac), return indication
       * to NOT route packet.  This prevents data packets flooded to
       * everyone from being handled by the wrong router.  If not
       * unicast for someone else, do normal processing.
       */
      client = lane->lec_info;
      if (client == NULL)
          return(LANE_NORMAL);
      regptr->argreg = hwidb->rxoffset +
        btow(LANE_DATA_ID_ETHER_OFFSET + sizeof(atm_encap_t));
      regptr->cmdreg = FCI_CMD_RX_SELECT;
      sniff_le_dat.l[0] = regptr->readlong;

      if ((sniff_le_dat.hdr.lec_id == client->lecid) &&
          (vc->user_vc_type == LANE_LEC_FORWARD))
        FLUSH_AND_RETURN_DONE;

      if (!IS_LANE_CTL(sniff_le_dat.hdr.lec_id)) {
	  if (IS_ETHER_UNICAST(sniff_le_dat.hdr.lan_type.ether.daddr)) {
	      sniff_le_dat.l[1] = regptr->readlong;     /* get rest of mac */
	      if (! lec_mac_valid(client, 
				  sniff_le_dat.hdr.lan_type.ether.daddr))
		  return(LANE_DO_NOT_ROUTE);
	  } else {
	      if ((!ROUTING_RXTYPE(swidb, regptr->rxtype)) &&
		  (swidb->tbridge_on_swidb))
		  return(LANE_DO_NOT_ROUTE);
	  }
      }

      return(LANE_NORMAL);

  case LANE_LEC_DISTRIBUTE:
      client = lane->lec_info;
      if (client == NULL)
          return(LANE_NORMAL);
      summary = regptr->rxtype;
      if (summary == RXTYPE_LANE_CONTROL) {
	  regptr->argreg = (hwidb->rxoffset +
			    btow(LANE_CTL_OPCODE_OFFSET + 
				 sizeof(atm_encap_t)));
	  regptr->cmdreg = FCI_CMD_RX_SELECT;
	  op = regptr->readshort;
	  if (op == LANE_TOPO_CHANGE) {
	      ushort lecid;	/* requestor lec-id */
	      ushort flags;     
	      /*
	       * Throw away packets with our own lec-id
	       */
	      regptr->argreg = (hwidb->rxoffset +
				btow(LANE_CTL_LECID_OFFSET +
				     sizeof(atm_encap_t)));
	      regptr->cmdreg = FCI_CMD_RX_SELECT;
	      lecid = regptr->readshort;

	      if (lecid == client->lecid) {
		  FLUSH_AND_RETURN_DONE;
	      }
	      regptr->argreg = (hwidb->rxoffset + 
				btow(LANE_CTL_FLAGS_OFFSET +
				     sizeof(atm_encap_t)));
	      regptr->cmdreg = FCI_CMD_RX_SELECT;

	      /*
	       * Give the topology change to process level, if the
	       * flags in the packet and the client differ.
	       */
	      if ((regptr->readshort & LANE_PROT_FLAG_TOPO_CHANGE)) {
		  flags = TRUE;
	      } else {
		  flags = FALSE;
	      }
	      if (client->C19_topology_change != flags) {
		  return(LANE_NORMAL);
	      } else {
		  FLUSH_AND_RETURN_DONE;
	      }
	  }
      }
      return(LANE_NORMAL);
    case LANE_LES_DIRECT:
      
      /* For packets arriving at LES on control direct vc:
       * a.  If le-arp resp, flush-resp, or le-arp req for other than mcast
       *      addr, count & reflect out on control distribute vc.
       * b.  Other control packets to process level.
       * c.  Toss data packets.
       */

      lsv = lane->lsv_info;
      if (lsv == NULL)
          return(LANE_NORMAL);
      summary = regptr->rxtype;
      if (summary == RXTYPE_LANE_CONTROL) {

        regptr->argreg = hwidb->rxoffset +
          btow(LANE_CTL_OPCODE_OFFSET +  sizeof(atm_encap_t));
        regptr->cmdreg = FCI_CMD_RX_SELECT;
        op = regptr->readshort;
        switch(op) {

        case LANE_ARP_REQ:

          /* On le-arp's, sniff target MAC.  le-arp's for multicast addresses
           * must be pushed to process level, so they will be answered by
           * the LES.  All others, fall through to reflect code.
           */

          regptr->argreg = hwidb->rxoffset +
            btow(LANE_ARP_TARGET_OFFSET + sizeof(atm_encap_t));
          regptr->cmdreg = FCI_CMD_RX_SELECT;
          sniff_le_dat.l[0] = regptr->readlong;
          sniff_le_dat.l[1] = regptr->readlong;
          if (!IS_ETHER_UNICAST(sniff_le_dat.le_arp_dst.addr.mac))
            return(LANE_TO_PROCESS);
          lsv->lsvs.lsvFastFwdArpReqs++;
          goto reflect_distribute;

        case LANE_ARP_RSP:
          lsv->lsvs.lsvFastFwdArpResps++;
          goto reflect_distribute;

        case LANE_FLUSH_RSP:
          lsv->lsvs.lsvFastFwdFlushResps++;
          goto reflect_distribute;

      case LANE_TOPO_CHANGE:
	  /* Just fall through */
	   lsv->lsvs.lsvOutTopReqs++;
        reflect_distribute:
          LANE_REFLECT(regptr, hwidb, lsv->distribute_vcc.vcd);
          return(LANE_ALREADY_DONE);

        default:
          /*not one of the opcodes we care about */
          return(LANE_TO_PROCESS);

        } /* end switch(op) */

      } else {
      
        /* toss non-control pkt sent to LES */
        lsv->lsvs.lsvFastUnknownFrames++;
        FLUSH_AND_RETURN_DONE;

      }

    default:
      /* not one of the 3 vc types we care about */
      return(LANE_NORMAL);

    }   /* end switch */
    
  }   /* end if */
  
  return(LANE_NORMAL);          /* if no vc or no lane_info */
  
}







