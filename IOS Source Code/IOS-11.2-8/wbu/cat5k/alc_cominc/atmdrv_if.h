/* $Id: atmdrv_if.h,v 1.1.4.3 1996/07/02 23:44:49 sakumar Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cominc/atmdrv_if.h,v $
 *-----------------------------------------------------------------
 * atmdrv_if.h -- ATM driver interface file.  
 *
 * Ratinder P. S. Ahuja
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmdrv_if.h,v $
 * Revision 1.1.4.3  1996/07/02  23:44:49  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Incorporated CLI change requests for PVC config and throttle config.
 *
 * Revision 1.1.4.2  1996/06/18  16:44:27  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/09  14:54:28  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:53  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:34  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.2  1996/03/27  21:55:38  sakumar
 * Got rid of some compiler warnings.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.1  1996/03/22  09:09:55  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:53  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:23  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *------------------------------------------------------------------
 *
 *  FILE:  atmdrv_if.h
 *
 *	CONTENTS:
 *
 *  This file describes the interface to the services of the ATM driver.
 *  Service users create a service access point with the ATM driver by 
 *  opening a VCI.  
 *	The services of the ATM driver are:
 *	
 *		aal_open_vcisap		
 *		aal_close_vcisap
 *		aal_getnext_vcisap		(to traverse active VCI's)
 *		aal_getvci_stat			statistics on VCI
 *		aal_udata_req
 *		aal_udata_ind
 *		
 *	OWNER:
 *
 *		Ratinder P. S. Ahuja
 *
*/

#ifndef _ATMDRVIF_H_
#define _ATMDRVIF_H_

#include	"basic_types.h"
#include	"ivcimap.h"
#include	"ovcimap.h"

/* The following two addresses should be 4 byte aligned */
#define VCR_IVCI_MAP_AREA_START_ADDR 0xfff00460
#define VCR_IVCI_MAP_AREA_END_ADDR 0xfff01000

#define MAX_VCR_IVCI_MAP_ENTRIES 64

#define ALC_CLK_FREQ 50000000.0
#define MAX_OC3_MBITS_PER_SEC 155


/*
**	ATM driver service access point
*/

typedef struct atmdrv_sap {						/* one per vc open			  */
	UINT16		vcstate;						/* state of VC 				  */
	UINT16		vcd;							/* IOS VCD Number for this VC */
	void *		pVCR_ivci_map_entry;			/* addr (in VCR) of Map Entry */
	void		(*sap_uep)( UINT16	vcd,		/* service user callback	  */
							UINT16  event, 		/* for event indications	  */
							UINT32	length,		/* length of packet           */
							void	*pData);	/* Pointer to start of data   */
} tAALSAP, *tpAALSAP;

/*
**  ATM driver global control block
*/
 
typedef struct atmdrv_gcb {
 
    UINT16      numsap;                         /* number of active SAPs    */
    tAALSAP     sap_entry[MAX_PORT][MAX_DEFAULT_VPC_C];
												/* array of vc sap entries  */
                                                /* per port                 */
	UINT32		rxcellcount; UINT32		txcellcount;
    UINT32		throttle_parm;
    UINT32      B1_err;
    UINT32      B2_err;
    UINT32      B3_err;
    UINT32      Z2_err;
    UINT32      G1_err;
    UINT32      CHCS_err;
    UINT32      UHCS_err;
    UINT32      OOF_count;
    UINT32      LOF_count;
} tATMDRVGCB, *tpATMDRVGCB;
 
extern tpATMDRVGCB  pAtmdrvGcb;


/*
** vcd_vpc_map[] translates FROM vcd TO vpc
*/
extern UINT16  vcd_vpc_map[MAX_PORT][MAX_DEFAULT_VPC_C];



	/*
	**			P R O T O T Y P E S
	**
	**	Operations defined on AAL saps.  All operations are synchronous
	**	(blocking).
	**
	*/



/*
**
** aalsap_open: Instantiate a SAP for the service user.  
**				This call is made for every VCI that is created.
*/ 
extern int	aalsap_open_vcc( UINT16	portindex,
							 UINT16 vcd,
							 tVPI  	vpi, 
							 tVCI  	vci,
							 tQOS  	qos, 
							 UINT16 txsar_state,
							 UINT16 rxsar_state,
                 			 UINT16 vlan,
							 UINT16 lecid,
							 void	(*aal_user_uep)(UINT16	vcd,
													UINT16  event, 
													UINT32	length,
													void	*pData) );

/*
**
**	get_vpc_from_vcd:  Return a vpc given a vcd
**
*/
extern tVPC get_vpc_from_vcd(UINT16  portindex, UINT16  vcd);


/*
**
**	get_vcd_from_vpc:  Return a vcd given a vpc
**
*/
extern UINT16 get_vcd_from_vpc(UINT16  portindex, tVPC  vpc);


/*
** 		aalsap_lec_map_bus_vcds: Map INCOMING BUS VC (on Client) to 
**					 OUTGOING BUS VC. Also bind VLAN AND LECID.
**		NOTE: Both VCs must ALREADY be open at this time.
**		Return: 1 means OK
**			    0 means call failed.
*/ 
extern	int	aalsap_lec_map_bus_vcds(UINT16 portindex,
				    UINT16 from_vcd,	/* The VC to be modified */
				    UINT16 to_vcd, 	/* VCD Encoded in Syn Hdr*/
				    UINT16 vlan,
				    UINT16 lecid);
/*
**      aalsap_bus_map_bus_vcds: Map INCOMING TO BUS VC to 
**                   OUTGOING FROM BUS VC.
**      NOTE: Both VCs must ALREADY be open at this time.
**      Return: 1 means OK
**              0 means call failed.
*/
extern  int aalsap_bus_map_bus_vcds(UINT16 portindex,
                    UINT16 in_vcd,    /* INCOMING TO BUS FROM NETWORK */
                    UINT16 out_vcd);  /* OUTGOING FROM BUS TO NETWORK */

/*
**      aalsap_change_vcd_clbit: For BUS Multicast VC from a Client
**                               perspective, changes the Conditional
**                               Learn bit from on to off or vice versa.
**		NOTE1: Both VCs must ALREADY be open at this time.
**   	RETURN:
**           1 means OK
**           0 means call failed.
*/
extern  int aalsap_change_vcd_clbit(UINT16 portindex,
                    UINT16 from_vcd,    /* The VC to be modified */
                    UINT16 to_vcd,  	/* VCD Encoded in Syn Hdr*/
                    UINT16 vlan,
                    UINT16 c_l);		/* Conditional Learn Bit */


/*
**      aalsap_bind_vlan_to_vc: Associate VLAN with NON-BUS data VC.
**      NOTE: VC must ALREADY be open at this time.
**      *** Important *** DO NOT use this function for BUS VCs.
**                        use aalsap_lec_map_bus_vcds() instead.
**      Return: 1 means OK
**              0 means call failed.
*/
extern int aalsap_bind_vlan_to_vc(UINT16 portindex,
				  UINT16 vcd,
				  UINT16 vlan,
                                  UINT32 vcstate);


/*
**
** aalsap_send:  Send data on indicated SAP.
** Return code: OK                                    :  0
**              Couldn't allocate command/data buffer : -1
**              Bad port                              : -2
**              VC not active                         : -3
**              Packet too long                       : -4
**
*/
extern int	aalsap_send(tVPC vpc, 
						UINT16 portindex, 
						UINT32 length,
						void *pData);

/*
**
** aalsap_nmp_send: Send data packet to NMP.
** Return code: OK                                      :  0
**              Resource unavailable                    : -1
**              Bad port                                : -2
**              Packet too long                         : -3
**
*/
extern int aalsap_nmp_send(	UINT16 portindex, 
							UINT32 datalen, 
							void *pData);


/*
**  aalsap_is_vpc_active:  return FALSE if state is inactive, true otherwise
*/
extern UINT32 aalsap_is_vpc_active(tVPC  vpc);


/*
**
** aalsap_getnext_vcisap:  Used to traverse active sap/vcis.  A call with
**	VPC value = 0 returns the next active VPC and the corresponding  vci, vpi 
**	and qos
**
*/
extern tVPC aalsap_getnext_vcisap(	UINT16 portindex, 
									tVPC vpc, 
									tVCI vci, 
									tVPI vpi, 
									tQOS qos);


/*
**
**	aalsap_getvci_stat:  Get usage information on a VC
**
*/
extern UINT16 aalsap_getvci_stat( tVPC vpc, UINT16);
		


/*
**
** aalsap_close:  Free the sap, map entries
**
*/
extern void	aalsap_close_vcc(tVPC  vpc, UINT16 portindex);

/*
**
** aalsap_vc_per_vp:  Modify number of VCI per VPI
**			 RETURN:  1 -- OK
** 					  0 -- Command failed, invalid num_vcs value
*/
int aalsap_vc_per_vp(UINT32 num_vcs);

/*
**
** aalsap_set_tsar_throttle: Pace global output of TSAR
**           RETURN:  1 -- OK
**                    0 -- Command failed
*/
int aalsap_set_tsar_throttle(UINT32 txsar_throttle_parm);



/*
**
**	atmdrv_BLOCK:  Waits for interrupt from the sars
**
*/
extern tBOOLEAN    atmdrv_BLOCK(tpSARCMD pCmd);

/*
** atm_drv_init:  To initialize the driver (used by IOS).
*/
extern void	atm_drv_init (void) ;

/*
** txsar_isr:  TxSAR Interrupt Service Routine (used by IOS).
*/
extern void	txsar_isr (void) ;

/*
** rxsar_isr:  RxSAR Interrupt Service Routine (used by IOS).
*/
extern void	rxsar_isr (void) ;

/*
** ADVR_RegInPktUpCall:  Register upcall routine for inbound (to ACP) packets.
*/
extern void	ADVR_RegInPktUpCall (void * pFunc) ;

/*
** ADVR_RxSarPollSvc:  Provide polling service for RxSar.
*/
extern UINT8	ADVR_RxSarPollSvc (UINT8 fOnce) ;

/*
** ADVR_RxSarPollSvc:  Provide polling service for TxSar.
*/
extern UINT8	ADVR_TxSarPollSvc (UINT8 fOnce) ;

/*
** ADVR_ChgEvPollFlag:  Force driver into/out of device polling mode.
*/
extern UINT8	ADVR_ChgEvPollFlag (UINT8) ;



extern tVPC atm_get_vpc(tVCI vci, tVPI vpi);
unsigned char ALC_InitAtmDriver (void) ;



#endif /* _ATMDRVIF_H_ */
