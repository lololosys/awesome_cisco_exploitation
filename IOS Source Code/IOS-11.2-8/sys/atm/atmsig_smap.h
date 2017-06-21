/* $Id: atmsig_smap.h,v 3.2.56.4 1996/08/16 02:13:17 lmercili Exp $
 * $Source: /release/112/cvs/Xsys/atm/atmsig_smap.h,v $
 *------------------------------------------------------------------
 * A T M S I G _ S M A P . H 
 *
 * January 1995,  Percy P. Khabardar 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmsig_smap.h,v $
 * Revision 3.2.56.4  1996/08/16  02:13:17  lmercili
 * CSCdi65496:  rfc1577 failed ip routing
 * Branch: California_branch
 *
 * Revision 3.2.56.3  1996/06/03  23:09:22  schrupp
 * CSCdi59328:  ATMSMAP: Request prefix from ILMI upon prefix deletion
 * Branch: California_branch
 *
 * Revision 3.2.56.2  1996/05/24  21:08:46  schrupp
 * CSCdi58635:  ATM: ESI cleanup
 * Branch: California_branch
 *
 *  - Deregister addresses when a new prefix is added by ILMI
 *  - Deregister addresses when ESI/NSAP is changed from command line
 *  - Reregister addresses even if the new prefix is the same as the old
 *  - Deregister addresses when subinterface is shut
 *  - Reregister addresses when subinterface is no shutD
 *  - Don't allow qsaal and ilmi pvc's on subinterfaces
 *  - Init atm_db->peeruni_ifname
 *
 * Revision 3.2.56.1  1996/05/08  02:43:53  grobelch
 * - Create system list registry functions invoked by static-map software
 *   when:
 *     a static-map is deleted, a map-group is deleted, and when a
 *     static-map is updated. A provider of static-map service (such
 *     as ATM signalling) can register for this, check for one of it's
 *     map's being the subject of the call, and act appropriately.
 *   - Provide ATM signalling routines for the static-map and group
 *     registries. Remove ARP table entries for IP and free up
 *     ATM signalling-specific data structures.
 *   - Improve vc "glomming" for point-to-point VCs to take in
 *     AAL5MUX case.
 *   - Fix mistaken use of an enumeration as a bit-value.
 * CSCdi53027:  ATMSIG: SVC static maps not updated properly in all cases
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:46:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/28  20:49:24  ronnie
 * CSCdi40510:  Too many ATM SVCs set up to same destination
 *              Also added some debugging to the ATM SigAPI
 *
 * Revision 2.2  1995/07/26  15:13:19  schrupp
 * CSCdi37690:  Support ATM ESI addressing with ILMI prefix
 *
 * Revision 2.1  1995/06/07  20:12:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ATMSIG_SMAP_H__
#define __ATMSIG_SMAP_H__

extern boolean atmSigRunning;

extern boolean atmSmap_debugError;
extern boolean atmSmap_debugEvent;
extern boolean atmSmap_debugAll;

/* add boolean multipoint here */
typedef struct atmSig_smap_ {
    atmSvcType *call_id;
    boolean locallyInitiatedCall;
    hwaddrtype destAtmAddr;
    hwaddrtype srcAtmAddr;
    int linktype;
    int protocol;
} atmSig_smap;

/* map esi requests to ilmi requests */
typedef enum ilmiReqType_ {
  reqAddressValid = 1,
  reqAddressInValid = 2,
}ilmiReqType;

/* ESI requests to ILMI 
 * This is the "cookie" that is passed to ILMI for address
 * registrations/deregistrations.
 */
typedef struct esiReqType_ {
  idbtype *idb;
  ilmiReqType ilmiReq;
}esiReqType;

/* External declarations */
extern boolean atmSmap_openSvcConnection(idbtype *, staticmap_type *, int);
extern void atmSmap_closeSvcConnection(void *);
extern boolean atmSmap_registerAtmAddress(idbtype *);
extern boolean atmSmap_cancelAtmAddress(idbtype *);
extern boolean atmSmap_findSvcStatus(void *);
extern void atmSmap_callbackFunction(void *ptr);
extern boolean atmSmap_staticMapUpdate(void *, idbtype *);
extern void atmSmap_staticMapDelete(void *call_id, idbtype *);
extern void atmSmap_sendToSigDriver(sig_api_msg *);
extern void atmSmap_Init(void);
extern vc_info_t *atm_glom_onto_vc (staticmap_type *, idbtype *);

/* multipoint additions */
extern boolean atmSmap_openSvcMtpConnection(idbtype *, staticmap_type *);
extern boolean atmSmap_staticMapMtpUpdate(void *, idbtype *);
extern boolean atmSmap_staticMapMtpDelete(void *, idbtype *);

extern boolean atmSmap_openPartyConnection (idbtype *, staticmap_type *, void *);
extern boolean atmSmap_staticMapPartyUpdate(void *, void *, idbtype *);
extern void atmSmap_closePartyConnection (void *, void *);
extern boolean atmSmap_staticMapPartyDelete(void *, void *, idbtype *);

/* extern int atmSmap_parseBroadbandLowLayerInfo (void *);
extern boolean atmSmap_fillBroadbandLowLayerInfo (staticmap_type *, sig_api_setup *); */

/* ILMI Additions */
extern boolean atmSmap_registerIlmi (void);
extern void atmSmap_ilmiCallback (void *);
extern boolean atmSmap_fillEsiInNsapInfo(idbtype *, atm_db_t *);
extern boolean atmSmap_getIlmiPrefix(atm_db_t *, atmSig_nsap_info *);
extern boolean atmSmap_sendNsapToIlmi (atmSig_nsap_info *, ilmiReqType);
extern boolean atmSmap_ilmiPrefix(hwidbtype *, uchar *);

#endif __ATMSIG_SMAP_H__

