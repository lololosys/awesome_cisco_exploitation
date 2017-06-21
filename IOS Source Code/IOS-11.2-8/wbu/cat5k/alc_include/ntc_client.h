/* $Id: ntc_client.h,v 1.1.4.1 1996/05/09 14:55:43 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/ntc_client.h,v $
 *------------------------------------------------------------------
 * ntc_client.h -- Client/user interfaces for ACP NTC.
 *
 * May 31, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ntc_client.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:43  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:12  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:10  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.2  1996/03/27  21:55:48  sakumar
 * Got rid of some compiler warnings.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.1  1996/03/22  09:11:22  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:14  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:42  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.10  1995/10/06  17:01:49  sakumar
 * Added RX and TX octet count (based on cell count*48). However, currently
 * disabling TX count since PMC counts idle cells sent by TXSAR.
 *
 * Revision 1.9  1995/09/30  00:17:09  rbau
 * added prototyping for API functions to access PMD serial eeprom
 *
 * Revision 1.8  1995/09/15  22:30:25  rbau
 * changed function prottotype of syn_atm_pmc_oc3_init_loopback to return int
 *
 * Revision 1.7  1995/09/08  23:07:43  sakumar
 * New defines for PMD types.
 *
 * Revision 1.6  1995/09/07  16:25:00  sakumar
 * Added loopback function for pmc sierra PMD chip.
 *
 * Revision 1.5  1995/08/30  19:14:50  sakumar
 * Support for new PMC PMD.
 *
 * Revision 1.4  1995/08/02  22:50:35  sakumar
 * Added function to register NTC line state change upcall.
 *
 * Revision 1.3  1995/06/22  17:58:13  sakumar
 * added ntc_get_framer_status() ref.
 *
 * Revision 1.2  1995/06/16  00:33:13  sakumar
 * Added ntc_show_status_regs()
 *
 * Revision 1.1  1995/06/01  22:37:21  sakumar
 * File contains ntc external interfaces.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
#ifndef _NTC_CLIENT_H_
#define _NTC_CLIENT_H_


/*
**
**	INCLUDES
**
*/

#include	"acp_basic_types.h"

/*
**	DEFINES
*/
#define LOSS_OF_FRAMING	0
#define IN_FRAMING		1

/*
** Various flavors of PMD chips
*/
#define PMD_NO_EEPROM 0
#define PMD_NTCa 1	/* Fujitsu NTC Rev A Chip */
#define PMD_NTCb 2	/* Fujitsu NTC Rev B Chip */
#define PMD_PMC  3	/* PMC Sierra Chip  */

/*
** Various flavors of Physical Media
*/
#define PMD_MODE_MMF	32		/* Multi-Mode Fiber */
#define PMD_MODE_SMF	33		/* Single-Mode Fiber */
#define PMD_MODE_UTP	34		/* Unshielded Twisted Pair */
#define PMD_MODE_INVALID	0		/* Invalid */


/*
**	PROTOTYPES
*/

extern UINT32	ntc_init(void);
extern UINT32	ntc_control(UINT32  opcode);
extern void		ntc_isr (void) ;
extern void		ntc_dma_init(void);
extern void 	ntc_stats_init(void);
extern void		ntc_show_status_regs(void);
extern int		ntc_get_framer_status(void);


extern void		ntc_Reg_line_state_change_upcall(void *);
typedef			void (tNtc_line_state_upcall) (int);


extern void syn_atm_pmc_oc3_init (void);
void syn_atm_pmc_oc3_isr (void);
int syn_atm_pmc_oc3_init_loopback (void);
extern int		rd_pmd_type (void);
extern int		rd_pmd_mode (void);

extern int pmc_get_framer_status (void) ;
extern void		pmc_Reg_line_state_change_upcall(void *);
typedef			void (tPmc_line_state_upcall) (int);


extern UINT32	pmd_chip_type;


#endif /* _NTC_CLIENT_H_ */
