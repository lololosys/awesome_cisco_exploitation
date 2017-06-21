/* $Id: camtest.c,v 1.1.4.1 1996/05/09 14:54:52 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/camtest.c,v $
 *------------------------------------------------------------------
 * camtest.c -- ATM driver test running on the host
 *
 * Ratinder P. S. Ahuja
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: camtest.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:52  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:20  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:05  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:23  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:18  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:48  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.2  1995/10/13  17:54:03  rbau
 * ifdef'd out some cam debug messages. Added cvs keywords in the header
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
 
#ifdef ATMIZER_EMULATOR
#include    <mips.h>
#endif
#include    "basic_types.h"
#include	"cam_if.h"
#include    "sar_if.h"
#include    "atmdrv_if.h"
#include    "atmtest.h"
 

/*
**	Prototypes
*/


UINT32 ncam = 10; 


UINT32
atmcam_setup(UINT32 num)
{
UINT32	i=0;
tpCAM_ENTRY	pCame, pCameGet;
tMACADDR	dmac = {0,0,1,2,3,4}; 			/* vary the first 2 bytes */
tCOLOR	lanid = 1;
UINT16	*pch = (UINT16 *)&dmac;
UINT32	cstate;

	/*
	**	Create num cam entries
	*/

	for(i=0;i<num;i++) {

		if( !(lanid % 3) ) {				/* every third entry has differen*/
			lanid++;						/* color					*/
		}

		*pch = (UINT16)i;					/* vary the first 2 bytes 	*/

#ifdef  DEBUG_CAM
		printf("\n\r atmcam_setup: setting macaddr:%x", *(UINT32 *)pch);
		printf("\n\r atmcam_setup: setting lanid:%x", lanid);
#endif /* DEBUG_CAM */
		if( (pCame = cam_create_entry(&dmac, lanid, LE_ARP_IN_PROGRESS_C)) 
				== NULLP ) {
			printf("\n\r atmcam_setup: failed %d", i);
			return ACP_ERR;
		} else {
			printf("\n\ratmcam_setup create ok: pCame: %x",pCame);
		}

		/*
		**	Verify the cam entry
		*/

		
		if ( (pCameGet = cam_get_state(&dmac, lanid, &cstate)) != pCame ) {

			printf("\n\r atmcam_setup: cam_get_state failed got %x", pCameGet);
			return ACP_ERR;

		} else if (cstate != LE_ARP_IN_PROGRESS_C) {

			printf("\n\r atmcam_setup: cam_get_state state mismatch: %d", 
					cstate);
			return ACP_ERR;

		} else {

			printf("\n\r atmcam_setup: cam entry verified : %d", i);

		}

	}/*for num cam entries*/

	return ACP_OK;

}/*end atmcam_setup*/



UINT32
atmcam_clear(UINT32 num)
{
UINT32  i=0;
tpCAM_ENTRY pCameGet;
tMACADDR    dmac = {0,0,1,2,3,4};           /* vary the first 2 bytes */
tCOLOR  lanid = 1;
UINT16  *pch = (UINT16 *)&dmac;
UINT32  cstate;
 

	/*
	**	Clear num cam entries
	*/
 
    for(i=0;i<num;i++) {
 
        if(!(lanid % 3) ) {                 /* every third entry has differen*/
            lanid++;                        /* color                    */
        }
 
        *pch = (UINT16)i;                   /* vary the first 2 bytes   */

#ifdef  DEBUG_CAM
		printf("\n\r atmcam_clear: clearing macaddr:%x", *(UINT32 *)pch);
		printf("\n\r atmcam_clear: clearing lanid:%x", lanid);
#endif  /* DEBUG_CAM */
 
		if(cam_clear_entry(NULLP, &dmac, lanid) != ACP_OK) {
			printf("\n\r atmcam_clear: failed for %d", i);
			return ACP_ERR;		
		}

		/*
		**	Verify num cam entries does not exit
		*/

        if ( (pCameGet = cam_get_state(&dmac, lanid, &cstate)) != NULLP ) {
 
            printf("\n\r atmcam_clear: cam_get_state failed got %x", pCameGet);
            return ACP_ERR;
 
        } else {
 
            printf("\n\r atmcam_clear: cam entry clear verified : %d", i);
 
        }

	}/*for num cam entries */

	return ACP_OK;

}/*end atmcam_clear*/


void
atmtst_1cam()
{

	/*
	**	Setup and verify
	*/
	if(atmcam_setup(1) != ACP_OK) {
		testresult = ATMTST_FAILED;
	} else {
		testresult = ATMTST_PASSED;
	}


	/*	
	**	Clear and verify
	*/

	if(atmcam_clear(1) != ACP_OK) {
		testresult = ATMTST_FAILED;
	} else {
		testresult = ATMTST_PASSED;
	}

}/* end atmtst_1cam */	


void
atmtst_Ncam()
{
	/*
	** Setup and verify
	*/
    if(atmcam_setup(ncam) != ACP_OK) {
        testresult = ATMTST_FAILED;
    } else {
        testresult = ATMTST_PASSED;
    }

	
	/*
	**	Clear and verify
	*/
    if(atmcam_clear(ncam) != ACP_OK) {
        testresult = ATMTST_FAILED;
    } else {
        testresult = ATMTST_PASSED;
    }

	return ;

}/*end atmtst_Ncam*/  


