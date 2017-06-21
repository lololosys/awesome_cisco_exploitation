/* $Id: atmdiag.h,v 1.1.4.1 1996/05/09 15:00:46 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/atmdiag.h,v $
 *-----------------------------------------------------------------
 * atmdiag.h -- header file for ATM system diagnostic
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmdiag.h,v $
 * Revision 1.1.4.1  1996/05/09  15:00:46  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:43  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:04  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:55  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:38  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:15  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.4.2.1  1995/11/15  23:29:53  rbau
 * merge mainline code changes in 1.4 branch
 *
 * Revision 1.5  1995/11/08  00:39:49  rbau
 * added support for DVT test
 *
 * Revision 1.4  1995/10/13  18:10:31  rbau
 * added keywords in the header for cvs. changed name loop5_uep to nmploop_uep
 *
 * Revision 1.3  1995/09/27  18:34:36  rbau
 * make external refernce to variable "ncam" in cam_test.c
 *
 * Revision 1.2  1995/08/27  23:43:14  rbau
 * put function prototyping declarations in this header file
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef  _ATMDIAG_H_
#define  _ATMDIAG_H_

extern UINT32	testcase;
extern UINT32	testresult;

extern uChar   MySlotNum;
extern uChar   nmptest_mac[];
extern UINT32  ncam;

#define	TSTPACKET_LEN				180   /* loopback test packet length */
#define	SYSTST_VPC					6      /* VPC used for loopback test */
#define	SYSTST_COLOR				0      /* VLAN number used for loopback test */

/*---< State Defines for Test state machine states >---*/
#define  SYSDIAG_IACTIVE         0
#define  SYSDIAG_RUNNING         1
#define  SYSDIAG_PASSED          2
#define  SYSDIAG_FAILED          3
#define  SYSDIAG_EXIT        		4

/* Defines for wait_for_interrupt() */
#define  WAIT_FOREVER 			   1
#define  WAIT_TILL_TIMEOUT       0
#define  DEFAULT_MAX             10


#define  SYSDIAG_INIT_TEST       1	/*send reset to ATMDRV, wait for resp 	*/
#define  SYSDIAG_1ENQ_DEQ_TEST   2	/*send no-op to sars, wait for resp 	*/
#define  SYSDIAG_NENQ_DEQ_TEST   3	/*send N noops to sars, wait for resp 	*/
#define  SYSDIAG_1VPC_TEST     	4	/*set, get clear 1 vpc					*/
#define  SYSDIAG_NVPC_TEST     	5	/*set, get clear N vpcs					*/
#define  SYSDIAG_1CAM_TEST     	6	/*set, get clear 1 cam entry			*/
#define  SYSDIAG_NCAM_TEST     	7	/*set, get clear N cam entries		*/

#define	SYSDIAG_LCP_PING     	8	/* echo test between ACP/LCP 			*/
#define	SYSDIAG_NCELL_TEST     	9	/*send a various cell size frames	*/
#define	SYSDIAG_LAST_TEST			SYSDIAG_NCELL_TEST

#define	SYSDIAG_TEST_OVER			SYSDIAG_LAST_TEST+1

#define	SCPMSG_FILTER_CNT  	  10	/* num of polling loops for SCP mesag */

/*
 *	Function Prototypes
 */
extern void sysdiag_interrupt_sars();
extern void sysdiag_enqdeq();
extern void sysdiag_Nenqdeq();
extern void sysdiag_vpc_1test();
extern void sysdiag_vpc_Ntest();
extern void sysdiag_test_Ncell();
extern void sysdiag_test_Ncell();
extern int  SageRxPacket(char *);
extern int  SageTxPacket();
extern int  getSaintReg();
extern int  writeSaintReg(int, int, int, int, int);
extern int  EnableSage(int);
extern int  SageOff_synbus(void);
extern int  LCP_EchoTest();
extern void spin_loop(int);
extern void nmploop_uep(tVPC, UINT16 , UINT32 , void *);
extern int  EnDisSage(int);


#define  SYSDIAG_OK       0x0	 		/* shared memory init error */

#define  SYSDIAG_ERR1     0x01		/* shared memory init error */
#define  SYSDIAG_ERR2     0x02      /* TX SAR SP mem init error */
#define  SYSDIAG_ERR3     0x03      /* RX SAR SP mem init error */
#define  SYSDIAG_ERR4     0x04      /* shared memory init error */
#define  SYSDIAG_ERR5     0x05      /* shared memory init error */
#define  SYSDIAG_ERR6     0x06      /* shared memory init error */
#define  SYSDIAG_ERR7     0x07      /* CAM init error */

typedef struct   dvt_param_
{
    int  mac_swap;         /* flag for loopback through ACP */
    int  dvt_thru_acp;     /* flag for loopback through ACP */
    int  rxpkt_vlan ;      /* vlan number for packet going back to synergy */
    int  txpkt_vlan ;      /* vlan number for packet coming from  synergy */
} dvt_param_t, *pdvt_param_t;

#endif   /* _ATMDIAG_H_ */

