/* $Id: cam_if.c,v 1.1.4.1 1996/05/09 14:54:51 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/cam_if.c,v $
 *------------------------------------------------------------------
 * cam_if.c -- ATM card CAM driver code and structures.
 *
 * March 1995, sakumar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cam_if.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:51  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:19  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:04  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.2  1996/03/30  02:17:42  sakumar
 * Changes to support sending of FLUSH message on Cat5K ATM LANE module.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.1  1996/03/22  09:10:21  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:18  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:47  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.8.8.1  1996/02/26  19:50:44  sakumar
 * During the deletion of the CAM entry, the TSAR sets the state to FREE,
 * rather than the ACP.
 *
 * Revision 1.8.6.1  1996/02/26  19:43:18  sakumar
 * During the deletion of the CAM entry, the TSAR sets the state to FREE,
 * rather than the ACP.
 *
 * Revision 1.8  1995/10/04  01:30:48  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.7  1995/09/27  23:27:26  sakumar
 * Removed references to TSAR flush holding queues since we will be
 * dropping packets during the flush state. Added support for LEC
 * to set the VPC along with the state in the TSAR CAM.
 *
 * Revision 1.6  1995/09/22  20:14:44  rbau
 * Fix for diagnostic looopback test to allow a clear_cam_entry()
 * while providing a NULL for the cam entry pointer. In this case
 * clear_cam_entry() will do a cam lookup to find the cam entry.
 *
 * Revision 1.5  1995/09/15  20:36:33  sakumar
 * Pulled out a bunch of debugging printfs.
 *
 * Revision 1.4  1995/06/15  21:01:04  rahuja
 * 	Fixed cam_getstate call to ignore errors
 *
 * Revision 1.3  1995/06/01  22:34:18  sakumar
 * Bunch of changes made in the last few days.
 *
 * Revision 1.2  1995/05/03  04:21:42  rlowe
 * Replace "acp_memcpy" with "memcpy", which is available in
 * services/libc/libc.a and libc_ios.a.
 *
 * Revision 1.1  1995/03/18  01:45:12  sakumar
 * Merged atmdriver with IOS environment
 *
*
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
**
**  INCLUDES
**
*/

#ifndef LANGUAGE_C
#define     LANGUAGE_C
#endif
 
#include    "basic_types.h"
#include    "cam_if.h"
#include    "sar_if.h"
#include    "spram.h"
#include    "atmdvr_gdata.h"

 
/*
**
**	GLOBALS
**
*/


/*
**
**	STATICS
**
*/
 


/*
**
**  Prototypes
**
*/


/*
*******************************************************************************
*
*   FUNCTION:
*           cam_init
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           ACP_OK
*			ACP_ERR
*
*   ALGORITHMS
*       Initialize cams
*
*******************************************************************************
*/

UINT32
cam_init()
{
tSARCMD    Cmd; 

	if( txsar_cmd(TXSAR_INIT_CAM_C, &Cmd) != ACP_OK ) {

		printf("\n\r cam_init: txsar_cmd failed");
		return ACP_ERR;

	} else {

		return ACP_OK;
	}

}/*end cam_init*/



/*
*******************************************************************************
*
*   FUNCTION:
*           cam_create_entry
*
*   PARAMETERS:
*           tpMACADDR	pDstMac
*			tCOLOR		color
*			UINT32		cam_state
*
*   RETURN:
*			tpCAM_ENTRY	pCamEntry           
*
*   ALGORITHMS
*       Send a create entry message to the tx sar.  The response has a 
*	10 bit value to be used as an index into the CAM Entry Table.  Set the
*	state of the cam entry. Init the holding queue.
*
*	Return pointer to entry.
*
*******************************************************************************
*/

tpCAM_ENTRY
cam_create_entry(tpMACADDR  pDstMac, tCOLOR  color, UINT32	cam_state)
{
tCAMPARM	cam_parm;
tpCAM_ENTRY	pCamEntry = NULLP;
INT16	indx;

	
	memcpy(&cam_parm.camp_macaddr, pDstMac, sizeof(*pDstMac));
	cam_parm.camp_vlanid = color;


	if( txsar_cmd(TXSAR_CREATE_CAM_C, &cam_parm) != ACP_OK ) {

		printf("\n\r cam_create_entry: txsar_cmd failed");
		return NULLP;

	} else {
	
		/*
		**	 The cam entry table index to be used is returned by the
		**	tx sar.  Use this index to fill in the fields of the cam
		**	entry
		*/

		indx = cam_parm.camp_index;
		if( (indx < 0) || 
			(indx > CAM_TABLE_SIZE) ) {

			printf("\n\r cam_create_entry: invalid cam index %d:", indx);
			return NULLP;	

		}
		
		/*
		**
		** Get pointer to cam entry in acp space
		**
		*/
		pCamEntry = &((tpCAM_ENTRY)(acp_Txspram_info.txspram_pCamTable))[indx];
#ifdef DEBUG
		printf("\n\r cam_create_entry: got INDEX %d:", indx);
		printf("\n\r cam_create_entry: got pCamEntry %x:", pCamEntry);
#endif
		if(pCamEntry->came_macstate != CAME_FREE_C) {
			printf("\n\rcam_create_entry: invalid came_macstate for index %d:",
				indx);
			printf("\n\rcam_create_entry: found came_macstate %d:",
					pCamEntry->came_macstate);
			/* cam_clear_entry */
			return NULLP;
		}
		pCamEntry->came_macstate = (UINT8)(cam_state);
		pCamEntry->came_pacing_counter = PACE_COUNT_C;
/*		pCamEntry->came_index = indx; */

	}

	return pCamEntry;

}/*end cam_create_entry*/




/*
*******************************************************************************
*
*   FUNCTION:
*           cam_get_state
*
*   PARAMETERS:
*           tpMACADDR   pDstMac
*           tCOLOR      color
*           UINT32      *pCamState
*
*   RETURN:
*           tpCAM_ENTRY pCamEntry
*
*   ALGORITHMS
*       Send a get entry message to the tx sar.  The response has a
*   10 bit value to be used as an index into the CAM Entry Table.  
*
*******************************************************************************
*/
 
tpCAM_ENTRY  
cam_get_state( tpMACADDR  pDstMac, tCOLOR  color, UINT32  *pCamState )
{
tCAMPARM    cam_parm;
tpCAM_ENTRY pCamEntry = NULLP;
INT16   indx;
 
 
    memcpy(&cam_parm.camp_macaddr, pDstMac, sizeof(*pDstMac));
    cam_parm.camp_vlanid = color;
	*pCamState = 0;
 

    if( txsar_cmd(TXSAR_GET_CAM_C, &cam_parm) != ACP_OK ) {
 
#ifdef DEBUG_PRINT
        printf("\n\r cam_get_state: txsar_cmd failed");
#endif /* DEBUG_PRINT */
        return NULLP;
 
    }

    /*
    **   The cam entry table index to be used is returned by the
    **  tx sar.  
    */
 
    indx = (INT16)(cam_parm.camp_index);

    if( (indx < 0) ||
        (indx > CAM_TABLE_SIZE) ) {

        printf("\n\r cam_get_state: invalid cam index %d:", indx);
        return NULLP;

    }
 
	pCamEntry = &((tpCAM_ENTRY)(acp_Txspram_info.txspram_pCamTable))[indx];
 
#ifdef DEBUG_PRINT
    if(pCamEntry->came_macstate == CAME_FREE_C) {
        printf("\n\r cam_get_state: invalid came_macstate for index %d:",
            indx);
        /* cam_clear_entry */
        return NULLP;

    } else {
		printf("\n\r cam_get_state: FOUND entry INDEX: %d", indx);
	}

#endif /* DEBUG_PRINT */
 
	*pCamState = cam_parm.camp_state;
    return pCamEntry;

}/*end cam_get_state */
 



/*
*******************************************************************************
*
*   FUNCTION:
*           cam_clear_entry
*
*   PARAMETERS:
*			tpCAM_ENTRY  pCamEntry
*           tpMACADDR   pDstMac
*           tCOLOR      color
*
*   RETURN:
*           ACP_OK
*			ACP_ERR
*
*   ALGORITHMS
*       Send a get clear cam message to the tx sar.  The response has a
*   10 bit value to be used as an index into the CAM Entry Table.  Compare
*	the cam entry pointer to that derived from the index.  Clear state.
*
*******************************************************************************
*/
 
UINT32
cam_clear_entry( tpCAM_ENTRY  pCamEntry, tpMACADDR  pDstMac, tCOLOR  color )
{
tCAMPARM    cam_parm;
tpCAM_ENTRY pCame = NULLP;
UINT32	CamState;
int i;
 
 
    memcpy(&cam_parm.camp_macaddr, pDstMac, sizeof(*pDstMac));
    cam_parm.camp_vlanid = color;
 
	/*
	**	If pointer to cam entry not provided by caller, look it up
	*/

	if (pCamEntry == NULLP) {
		if( (pCame = cam_get_state( pDstMac, color, &CamState )) == NULLP ) {

     	  	printf("\n\r cam_clear_entry: cam_get_state failed: %x", pCame);
			return ACP_ERR;

		}
		pCamEntry = pCame;
	}

	/*
	**	Send clear command down to tsar
	*/

    if( txsar_cmd(TXSAR_CLEAR_CAM_C, &cam_parm) != ACP_OK ) {
 
#ifdef DEBUG_PRINT
        printf("\n\r cam_clear_entry: txsar_cmd failed");
        printf("MAC Addr: %x %x %x %x %x %x  Color: %d\n",
                pDstMac->maddr[0],
                pDstMac->maddr[1],
                pDstMac->maddr[2],
                pDstMac->maddr[3],
                pDstMac->maddr[4],
                pDstMac->maddr[5],
                color);
        return ACP_ERR;
#endif /* DEBUG_PRINT */
 
    } 
 
    return ACP_OK;

}/*end cam_clear_entry */


/*
*******************************************************************************
*
*   FUNCTION:
*           cam_set_state
*
*   PARAMETERS:
*           tpCAM_ENTRY pCamEntry
*			tpMACADDR pDstMac
*			tCOLOR	color
*			UINT32 cam_state
*			UINT16 vcd
*
*   RETURN:
*           ACP_OK
*           ACP_ERR
*
*   ALGORITHMS
*			Send a Clear CAM State command to TSAR which would do a lookup
*			and if the lookup succeeded then it would change the CAM state
*
*******************************************************************************
*/

UINT32
cam_set_state(	tpMACADDR  pDstMac, 
				tCOLOR  color,
				UINT32 cam_state,
				UINT16 vcd)
{
tCAMPARM    cam_parm;
tpCAM_ENTRY pCame = NULLP;
UINT32	CamState;
tVPC vpc;
int i;

    if (cam_state == VC_ACTIVE_C) {
	    if ( (vpc = get_vpc_from_vcd(0, vcd)) == 0xffff) {
		    buginf("\n\rcam_set_state: Unknown VCD %d", vcd);
		    return ACP_ERR;
	    }
        cam_parm.camp_dd_vpc = vpc;
    }
 
 
    memcpy(&cam_parm.camp_macaddr, pDstMac, sizeof(*pDstMac));
    cam_parm.camp_vlanid = color;
    cam_parm.camp_state = cam_state;

	/*
	**	Send Change state command to tsar
	*/

    if( txsar_cmd(TXSAR_CHANGE_CAM_STATE_C, &cam_parm) != ACP_OK ) {
 
#ifdef DEBUG_PRINT 
        printf("\n\r change_cam_state: txsar_cmd failed");
        printf("MAC Addr: %x %x %x %x %x %x  State: %d Color: %x\n",
                pDstMac->maddr[0],
                pDstMac->maddr[1],
                pDstMac->maddr[2],
                pDstMac->maddr[3],
                pDstMac->maddr[4],
                pDstMac->maddr[5],
				cam_state,
                color);
        return ACP_ERR;
#endif /* DEBUG_PRINT */
 
    } 
 
    return ACP_OK;

}/*end cam_clear_entry */


/*
*******************************************************************************
*
*   FUNCTION:
*           cam_set_counter
*
*   PARAMETERS:
*           tpCAM_ENTRY  pCamEntry
*           UINT32 cam_state
*
*   RETURN:
*           ACP_OK
*           ACP_ERR
*
*   ALGORITHMS
*       Chage the pacing count to the requested value
*
*******************************************************************************
*/

UINT32
cam_set_counter(void  *pCamEntry, UINT32 count)
{

	((tpCAM_ENTRY)pCamEntry)->came_pacing_counter = (UINT8)count;
	return ACP_OK;

}/*end cam_set_counter*/



/*
*******************************************************************************
*
*   FUNCTION:
*           cam_getnext_entry
*
*   PARAMETERS:
*           tpCAM_ENTRY  pCamEntry
*
*   RETURN:
*           ACP_OK
*           ACP_ERR
*
*   ALGORITHMS
*       return next valid entry
*
*******************************************************************************
*/
 
tpCAM_ENTRY  
cam_getnext_entry(tpCAM_ENTRY   pCamEntry)
{

	return NULLP;

}/*end cam_getnext_entry*/
