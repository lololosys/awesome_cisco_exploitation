/* $Id: diagdata.c,v 1.1.4.2 1996/06/18 16:45:22 sakumar Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/diagdata.c,v $
 *------------------------------------------------------------------
 * diagdata.c -- system level data packet loopback
 *
 * ?
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: diagdata.c,v $
 * Revision 1.1.4.2  1996/06/18  16:45:22  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/09  15:00:58  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:55  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:18  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:07  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  10:52:45  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:50  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:25  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.10.2.1.6.1  1996/03/05  02:32:13  rlowe
 * Fold in fixes from mfg_repair_branch.
 *
 * Revision 1.10.2.1  1995/11/16  00:17:20  rbau
 * merge mainline code changes in 1.4 branch
 *
 * Revision 1.12  1995/11/16  00:13:44  rbau
 * enable the packet content dump for DVT loopback test through ACP
 *
 * Revision 1.11  1995/11/08  00:33:26  rbau
 * added support for DVT test
 *
 * Revision 1.10  1995/10/13  18:14:58  rbau
 * added cvs keywords in the header. clean up some ifdefs. changed loop5 to some
 * sensible name.
 *
 * Revision 1.9  1995/10/04  01:30:40  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.8  1995/10/04  00:38:25  rbau
 * clean up debug messages.
 *
 * Revision 1.7  1995/09/27  18:38:26  rbau
 * made changes to use NMP's mac address for NMP SYNDIAG loopback test.
 *
 * Revision 1.6  1995/09/08  00:34:14  rlowe
 * More fixes relating to doing LCP driver init and system loopbacks.
 *
 * Revision 1.5  1995/09/07  12:09:01  rlowe
 * LCP driver changes to 1) push driver data/state from Boot-2 into IOS
 * Kernel; 2) allow for NMP-driven system loopback diags during SCP
 * startup sequence.
 *
 * Revision 1.4  1995/08/28  00:00:15  rbau
 * added support for SYNDIAG on NMP. more code clean up
 *
 * Revision 1.3  1995/07/21  00:17:35  rbau
 * ifdef'd out debug messages
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*---< Includes >---*/

#include "basic_types.h"
#include "queue_if.h"
#include "buffer_if.h"
#include "sar_if.h"
#include "shram.h"
#include "spram.h"
#include "atmdrv_if.h"
#include "acp_hw_defs.h"
#include "acp_c_defs.h"
#include "lo_wdog.h"
#include "../alc_drivers/ntc_if.h"
#include "atmdiag.h"
#include "mfp_acp_defs.h"
#include "errcode.h"
#include "lcp_api.h"

#define  PRINT_AFTER_THIS_MANY   10

/*---< Globals >---*/
UINT32	do_print_recvdata = 1;
UINT32	do_break = 0;
UINT32	send_packet_count = 0;
UINT32	recv_packet_count = 0;
UINT32	seq_err = 0;
UINT32	data_err = 0;
UINT8		next_seq = 0;
char		syndiag_snap[] = { 0xaa,0xaa,0x03,0x00,0x00,0x0c,0x20,0x02};	
uChar		nmptest_srcmac[] = { 0x00, 0x40, 0x0b, 0xd0, 0x00, 0x05};	
uChar		nmptest_mac[] = { 0x00, 0x40, 0x0b, 0x58, 0x30, 0x00};	

extern	UINT32	crc_err;
extern 	UINT32	rto_err;
extern 	UINT32	len_err;
extern	UINT32	buf_ovfl_err;
extern 	tpRXSPRAM       acp_pRxSpram;
extern	tRXSPRAM_INFO   acp_Rxspram_info;
extern  dvt_param_t dvt_test_param;

/*---< Function Prototypes >---*/
int sysdiag_Ncell();
int sysdiagvpc_setup(int);
int loop3_core(void);
int loop4_core(void);
int nmploop_core(void);
void nmploop_uep(tVPC, UINT16 , UINT32 , void *);
int	 DVT_core(void);
void dvt_uep(tVPC, UINT16 , UINT32 , void *);
void test_callback(tVPC vpc, UINT16 event, UINT32 length, void *pData);
void show_stats(void);


/*-------------------------------------------------------------------*
 * sysdiag_uep()
 *    upcall handler for receive packet demux 
 *-------------------------------------------------------------------*/
void
sysdiag_uep(tVPC	  vpc,
            UINT16  event,
            UINT32  length,
            void    *pData)
{
	UINT32 i;
	UINT8	*pCh;
	UINT32	*pW;
	static	UINT8 next_seq = 0;

#ifdef DEBUG 
    printf("\n\rsysdiag_uep: vpc= %d\n", vpc);
    printf("sysdiag_uep: length %d\n", length);
#if 0
     printf("sysdiag_uep: data %x\n", *(UINT32 *)pData);  
#endif  /* ZERO */
#endif  /* DEBUG */
	
	if(do_print_recvdata)
	{

#ifdef LOG_DATA
		pW = (UINT32 *)pData;
		printf("\r sysdiag_uep: DATA:\n\r",0);
		length = length/4;

		for(i=0;i<length; i++)
		{
			printf("%x  ",*pW);
			pW++;
		}
#endif  /* LOG_DATA */

		if(!(recv_packet_count % PRINT_AFTER_THIS_MANY))
		{
			printf("sysdiag_uep: recv packet count: 0x%x\n", recv_packet_count);
		}

	} /* !do_print_recvdata */
   else
	{ 
		pCh = (UINT8 *)pData;
		if(*pCh++ != ++next_seq)
		{
			seq_err++;
			next_seq = *(UINT8 *)pData;
			printf("sysdiag_uep: SEQ NO MISMATCH err count: %x\n",seq_err);
		}

		for(i=1; i<length; i++)
		{
			if(*pCh != i)
			{
				data_err++;
				printf("sysdiag_uep: DATA MISMATCH for location: %x\n",i);
				printf("sysdiag_uep: DATA MISMATCH got: %x\n", *pCh);
				goto err_exit;
 			}
			else
				++pCh;
		}
	}  /* do_print_recvdata */

	++recv_packet_count;

err_exit:
   return;
}

/*-------------------------------------------------------------------*
 * loop4_uep()
 *    upcall handler for loop4  packet demux 
 *-------------------------------------------------------------------*/
void
loop4_uep(tVPC	  vpc,
          UINT16  event,
          UINT32  length,
          void    *pData)
{
	UINT32 i;
	UINT8	*pCh;
	UINT32	*pW;
	static	UINT8 next_seq = 0;

#ifdef DIAGDATA 
    printf("\n\rloop4_uep: vpc= %d\n", vpc);
    printf("\tloop4_uep: length %d\n", length);
    printf("\tloop4_uep: data %x\n", *(UINT32 *)pData);
#endif  /* DIAGDATA */
	
	if(do_print_recvdata)
	{

#ifdef LOG_DATA
		pW = (UINT32 *)pData;
		pCh = (UINT8 *)pData;
		printf("\r loop4_uep: DATA:\n\r",0);
		length = length;

		for(i=0;i<length; i++)
		{
         if ((i % 16) == 0) printf("\n0x%.2X:  ", i);
			printf("%.2x ",*pCh++);
		}
      printf("\n");

#endif  /* LOG_DATA */

		if(!(recv_packet_count % PRINT_AFTER_THIS_MANY))
		{
			printf("loop4_uep: recv packet count: 0x%x\n", recv_packet_count);
		}

	} /* !do_print_recvdata */
   else
	{ 
		pCh = (UINT8 *)pData;
		pCh += 14;     /* bypass 12 bytes mac address, and 2 bytes lec_id */
		for(i= 0; i < (length - 14); i++)
		{
			if(*pCh != i)
			{
				data_err++;
				printf("loop4_uep: DATA MISMATCH for location: %x\n", i);
				printf("loop4_uep: DATA MISMATCH got: %x\n", *pCh);
				goto err_exit;
 			}
			else
				++pCh;
		}
	}

	++recv_packet_count;

err_exit:
   return;

}  /* loop4_uep() */

/*-------------------------------------------------------------------*
 * sysdiag_Ncell()
 *
 *	Open VPC
 *	Get buffer from ACP to sar data pool
 *	1 cell tests:
 *		send 30 bytes  => 1 cell, 10 byte pad 
 *		send 35 bytes  => 1 cell, 5 byte pad	
 *		send 40 bytes  => 1 cell, 0 byte pad
 *		send 42 bytes  => 1 cell, 6 byte pad + dummy last cell with eom,len,crc
 *		send 45 bytes  => 1 cell, 3 byte pad + dummy last cell with eom,len,crc
 *		send 48 bytes  => 1 cell, 0 byte pad + dummy last cell with eom,len,crc
 *		Wait for and check response
 *
 *-------------------------------------------------------------------*/
int
sysdiag_Ncell()
{
	tpQDESC  pAcpDataPool = acp_Shram_info.shram_pAcpToSarPool;
	UINT8		*pdata;
	UINT32	datalen, i;
	tVPC		vpc = 17;
	static UINT8	seq=0;
	int		err_code;

	tpBUFFER pBuf = NULLP;
	tTXSEND_REQ	Sreq;

	testresult = SYSDIAG_PASSED;

	if( aalsap_open_vcc( 0, vpc, 0, vpc, 0,
   	VPC_TX_ACTIVE, VPC_DEST_ACP, 0, 0,	sysdiag_uep) == 0)
	{
#ifdef DEBUG
      printf("sysdiag_Ncell: aalsap_open_vcc failed \n");
#endif /* DEBUG */
		err_code = LPBKDIAG_VCCOPEN_ERROR;  
		testresult = SYSDIAG_FAILED;
		return(err_code);
	}

	datalen = 35;

redo_test:

#ifdef  DIAGDATA

#define PRINT_AFTER_THIS_MANY 1

	if(!(send_packet_count % PRINT_AFTER_THIS_MANY))
	{
		printf("\nsysdiag_Ncell: send packet count: 0x%x\n", send_packet_count);
		printf("sysdiag_Ncell: recv packet count: 0x%x\n", recv_packet_count);
		printf("sysdiag_Ncell: recv seq_err count: 0x%x\n", seq_err);
		printf("sysdiag_Ncell: recv data_err count: 0x%x\n", data_err);
		printf("sysdiag_Ncell: recv crc_err count: 0x%x\n", crc_err);
		printf("sysdiag_Ncell: recv rto_err count: 0x%x\n", rto_err);
		printf("sysdiag_Ncell: recv buf_ovfl_err count: 0x%x\n",buf_ovfl_err);
		printf("sysdiag_Ncell: recv len_err count: 0x%x\n", len_err);
#ifdef  DEBUG
		printf("RXSAR debug0: 0x%x\n", acp_pRxSpram->rxspram_info.debug0);
		printf("RXSAR debug1: 0x%x\n", acp_pRxSpram->rxspram_info.debug1);
		printf("RXSAR debug2: 0x%x\n", acp_pRxSpram->rxspram_info.debug2);
		printf("RXSAR debug3: 0x%x\n", acp_pRxSpram->rxspram_info.debug3);
		if (acp_pRxSpram->rxspram_info.debug2)
		{
			q_log(acp_Rxspram_info.rxspram_pAcpBufPool);
			q_log(acp_Rxspram_info.rxspram_pRxToAcpDataq);
		}
#endif  /* DEBUG */

	}

#endif  /* DIAGDATA */

   if( (pBuf = (tpBUFFER)bufmgr_getbuf(pAcpDataPool)) == NULLP )
	{
      printf("\rtxrtxtest_Ncell: bufmgr_getbuf failed\n");
      printf("\rtxrtxtest_Ncell: pAcpDataPool:%x\n", pAcpDataPool);
      q_log(pAcpDataPool);
      shmem_log();
      spram_log();
		err_code =  LPBKDIAG_GETBUF_ERROR;
		testresult = SYSDIAG_FAILED;
      return(err_code);
	}

   /*
    * The buffer returned is in the TX SHRAM address space,
    * convert to acp space
    */
   pBuf = (tpBUFFER) MAP_SH_SAR_ACP(pBuf);

   pdata = (UINT8 *)&pBuf->buf_data;
   *pdata++ = ++seq;

   for(i=1;i<datalen;i++)
      *pdata++ = i;

#ifdef DIAGDATA
	printf("\t sysdiag_Ncell: sending %d bytes\n", datalen);
	printf("\t sysdiag_Ncell: sending pdata: 0x%x\n", &pBuf->buf_data);
	printf("\t sysdiag_Ncell: sending on vpc: %d\n", vpc);
#endif  /* DIAGDATA */

	Sreq.txsend_vpc = vpc;
	Sreq.txsend_len = datalen;
	Sreq.txsend_pbuf = (void *) &pBuf->buf_data;

	/*
	 *	txsar_cmd is a blocking call
	 */
	if (txsar_cmd(TXSAR_SEND_REQUEST_C, (void *) &Sreq) == ACP_ERR)
	{
		err_code = LPBKDIAG_TXSAR_SENDREQ_ERROR;
		testresult = SYSDIAG_FAILED;
		bufmgr_freebuf(pAcpDataPool, (tpBUFFER) MAP_SH_ACP_SAR(pBuf));
	}

	bufmgr_freebuf(pAcpDataPool, (tpBUFFER) MAP_SH_ACP_SAR(pBuf));
	send_packet_count++;

	do_print_recvdata = 0;

/*****************************************************************/
{
	int  loop_cnt = 30;   
	int  rxstat= 0;

#ifdef  DIAGDATA
printf("\t sysdiag_Ncell: polling for loopback packet on vpc: %d\n", vpc);
#endif  /* DIAGDATA */

	rxstat = ADVR_RxSarPollSvc(FALSE);    /* non-blocking */

   while ((rxstat == 0) && loop_cnt--)
   {

#ifdef BLOCKING
     /* blocking poll */
     MFP_PollForIntr (MFP_IMSK_RSAR, FALSE, FALSE, TRUE);
#else
     /* non-blocking poll */
     MFP_PollForIntr (MFP_IMSK_RSAR, FALSE, TRUE, TRUE); 
#endif /* BLOCKING */

		rxstat = ADVR_RxSarPollSvc(FALSE);  /* non-blocking */
   }

  	if (!rxstat)
   {
      printf("sysdiag_Ncell: no loopback cell\n");
      err_code = LPBKDIAG_NORXCELL_ERROR;
      testresult = SYSDIAG_FAILED;
	   aalsap_close_vcc( vpc, 0);
		return(err_code);
   }

}  
/*****************************************************************/


	/*
	 * next cell size to transmit
	 */
	switch(datalen)
	{
	   case 30:
			datalen = 35;
         break;

		case 35:	
			datalen = 36;	
			break;
		case 36:	
			datalen = 37;	
			break;
		case 37:	
			datalen = 38;	
			break;
		case 38:	
			datalen = 39;	
			break;
		case 39:	
			datalen = 40;	
			break;
		case 40:	
			datalen = 42;	
			break;
		case 42:
			datalen = 45;
			break;
		case 45:
			datalen = 48;
			break;
		case 48:
			datalen = 67;
			break;
		case 67:
			datalen = 95;
			break;
		case 95:
			datalen = 120;
			break;
		case 120:
			datalen = 240;
			break;
		case 240:  
			datalen = 131;
			break;
		case 131:	
			datalen = 132;	
			break;
		case 132:	
			datalen = 133;	
			break;
		case 133:	
			datalen = 134;	
			break;
		case 134:	
			datalen = 135;	
			break;
		case 135:	
			datalen = 136;	
			break;
		case 136:	
	     	aalsap_close_vcc( vpc, 0);
			err_code = LPBKDIAG_OK;
			return(err_code);

		default:
			datalen = 30;	
			break;
	} /* switch */
	
	goto redo_test;

} /* sysdiag_Ncell() */


/*-------------------------------------------------------------------*
 * sysdiagvpc_setup()
 *-------------------------------------------------------------------*/
int
sysdiagvpc_setup(int num)
{
	tVPC 		atmvpc;
	int		i=0;

	for(i=0;i<num;i++)
	{

		/*  0,         		port index
		 *  6,          		vcd
		 *  0,          		tVPI
		 *  (5 + i),			tVCI
		 *  0,            	QOS
		 *  VPC_TX_ACTIVE,	TXSAR state
		 *  VPC_DEST_ACP,	RXSAR state
		 *  0,       			VLAN number
		 *  0,       			LEC ID
		 *  sysdiag_uep		callback function
	    */
      atmvpc = aalsap_open_vcc( 0, 6, 0, (5+i), 0,
               VPC_TX_ACTIVE, VPC_DEST_ACP, 0, 0, sysdiag_uep);

		if(atmvpc == 0)
		{
			printf("\rsysdiagvpc_setup: failed %d\n", i);
			return ACP_ERR;
		}
#ifdef  DEBUG
		else
			printf("\rsysdiagvpc_setup: lap=%d\n", i);
#endif  /* DEBUG */

	} /* for */
	
	return ACP_OK;
} /* sysdiagvpc_setup() */	

/*-------------------------------------------------------------------*
 * sysdiagvpc_close()
 *-------------------------------------------------------------------*/
UINT32
sysdiagvpc_close(int num)
{
	int  i=0;
 
   for(i=0; i<num; i++) 
		aalsap_close_vcc((5+i), 0);

	return ACP_OK;
}


/*-------------------------------------------------------------------*
 * sysdiag_1vpc()
 *-------------------------------------------------------------------*/
void
sysdiag_1vpc()
{
	int status = 0;

#ifdef  DEBUG
	printf("syndiag_1vpc: entered\n");
#endif  /* DEBUG */

	if(sysdiagvpc_setup(1) != ACP_OK)
	{
		status |= 1;
#ifdef  DEBUG
		printf("syndiag_1vpc: setup failed\n");
#endif  /* DEBUG */
	}
#ifdef  DEBUG
	else
		printf("syndiag_1vpc: setup passed\n");
#endif  /* DEBUG */

	if(sysdiagvpc_close(1) != ACP_OK) 
	{
		status |= 1;
#ifdef  DEBUG
		printf("syndiag_1vpc: close failed\n");
#endif  /* DEBUG */
	}
#ifdef  DEBUG
	else
		printf("syndiag_1vpc: close passed\n");
#endif  /* DEBUG */

	if (status)
		testresult = SYSDIAG_FAILED;
	else
		testresult = SYSDIAG_PASSED;

} /* sysdiag_1vpc() */


/*-------------------------------------------------------------------*
 * sysdiag_Nvpc()
 *-------------------------------------------------------------------*/
void
sysdiag_Nvpc()
{
	int status = 0;
 
   if(sysdiagvpc_setup(10) != ACP_OK)
      status |= 1;

   if(sysdiagvpc_close(10) != ACP_OK)
      status |= 1;

	if (status)
		testresult = SYSDIAG_FAILED;
	else
		testresult = SYSDIAG_PASSED;
} /* sysdiag_Nvpc() */


/*-------------------------------------------------------------------*
 * loop4_core()
 *
 *	Open VPC
 * put packet thru LCP in packet buffer
 *
 *-------------------------------------------------------------------*/
int
loop4_core()
{
	tpQDESC  pAcpDataPool = acp_Shram_info.shram_pAcpToSarPool;
	UINT8		*pdata;
	UINT32	datalen, i;
	tVPC		vpc = SYSTST_VPC;
	static UINT8	seq=0;
	int		rc, err_code;

	tpBUFFER pBuf = NULLP;
	tTXSEND_REQ	Sreq;

	if( aalsap_open_vcc( 0, vpc, 0, vpc, 0,
   	VPC_TX_ACTIVE, VPC_DEST_ACP, 0, 0,	loop4_uep) == 0)
	{
#ifdef  DEBUG
      printf("\nloop4_core: aalsap_open_vcc failed %d\n", vpc);
#endif  /* DEBUG */
		err_code = LPBKDIAG_VCCOPEN_ERROR;
      return(err_code);
	}

	datalen = 67;
	send_packet_count = 0;
	recv_packet_count = 0;

	err_code = SageTxPacket();
	if (err_code)
      return(err_code);

	send_packet_count++;

	do_print_recvdata = 0;

#ifdef  DEBUG
	printf("\nLoop4_loopback: waiting on vpc: %d\n", vpc);
#endif  /* DEBUG */

/*****************************************************************/
{
	int  loop_cnt = 500;
	int  rxstat= 0;

	rxstat = ADVR_RxSarPollSvc(FALSE);   /* non-blocking */

   while ((rxstat == 0) && loop_cnt--)
   {
     /* non-blocking poll */
		MFP_PollForIntr (MFP_IMSK_RSAR, FALSE, TRUE, TRUE); 
		rxstat = ADVR_RxSarPollSvc(FALSE);  /* non-blocking */
   }
	if (!rxstat)
	{
#ifdef  DEBUG
		printf("loop4_core: no loopback cell\n"); 
#endif  /* DEBUG */
		err_code = LPBKDIAG_NORXCELL_ERROR;
	}
}  
	
/*****************************************************************/

	SD_DumpSarBigaInfo () ;

	aalsap_close_vcc( vpc, 0);

	return(err_code);    

} /* loop4_core() */


/*-------------------------------------------------------------------*
 * loop3_core()
 *
 *	Get buffer from ACP to sar data pool
 *		Wait for and check response
 *
 *-------------------------------------------------------------------*/
int
loop3_core()
{
	tpQDESC  pAcpDataPool = acp_Shram_info.shram_pAcpToSarPool;
	UINT8		*pdata;
	UINT32	datalen;
	tVPC		vpc = 17;
	static UINT8	seq=0;
	int		rc, err_code;
	int 		i;
	UINT8		tstbuf[250];

	tpBUFFER pBuf = NULLP;
	tTXSEND_REQ	Sreq;

	testresult = SYSDIAG_PASSED;
	err_code = LPBKDIAG_OK;

	/* length of ethernet packet w/ mac addr : 180 +12 */ 
	datalen = TSTPACKET_LEN + 12;   
	send_packet_count = 0;
	recv_packet_count = 0;

redo_test:

#ifdef  DIAGDATA
	if(!(send_packet_count % PRINT_AFTER_THIS_MANY))
	{
		printf("\nloop3_core: send packet count: 0x%x\n", send_packet_count);
		printf("loop3_core: recv packet count: 0x%x\n", recv_packet_count);
		printf("loop3_core: recv seq_err count: 0x%x\n", seq_err);
		printf("loop3_core: recv data_err count: 0x%x\n", data_err);
		printf("loop3_core: recv crc_err count: 0x%x\n", crc_err);
		printf("loop3_core: recv rto_err count: 0x%x\n", rto_err);
		printf("loop3_core: recv buf_ovfl_err count: 0x%x\n",buf_ovfl_err);
		printf("loop3_core: recv len_err count: 0x%x\n", len_err);
#ifdef  DEBUG
		printf("RXSAR debug0: 0x%x\n", acp_pRxSpram->rxspram_info.debug0);
		printf("RXSAR debug1: 0x%x\n", acp_pRxSpram->rxspram_info.debug1);
		printf("RXSAR debug2: 0x%x\n", acp_pRxSpram->rxspram_info.debug2);
		printf("RXSAR debug3: 0x%x\n", acp_pRxSpram->rxspram_info.debug3);
		if (acp_pRxSpram->rxspram_info.debug2)
		{
			q_log(acp_Rxspram_info.rxspram_pAcpBufPool);
			q_log(acp_Rxspram_info.rxspram_pRxToAcpDataq);
		}
#endif  /* DEBUG */

	}
#endif  /* DIAGDATA */

   if( (pBuf = (tpBUFFER)bufmgr_getbuf(pAcpDataPool)) == NULLP )
	{
      printf("\rloop3_core: bufmgr_getbuf failed\n");
      printf("\rloop3_core: pAcpDataPool:%x\n", pAcpDataPool);
      q_log(pAcpDataPool);
      shmem_log();
      spram_log();
		err_code = LPBKDIAG_GETBUF_ERROR;
		testresult = SYSDIAG_FAILED;
      return(err_code);
	}

   /*
    * The buffer returned is in the TX SHRAM address space,
    * convert to acp space
    */
   pBuf = (tpBUFFER) MAP_SH_SAR_ACP(pBuf);

   pdata = (UINT8 *)&pBuf->buf_data;
   *pdata++ = ++seq;

   for(i=1;i<datalen;i++)
      *pdata++ = i;

#ifdef DIAGDATA
	printf("\t loop3_core: sending %d bytes\n", datalen);
	printf("\t loop3_core: sending pdata: 0x%x\n", &pBuf->buf_data);
#endif  /* DIAGDATA */

 	/* Send packet out */
   if(aalsap_nmp_send(0, datalen, (void *)&pBuf->buf_data) != 0)
	{
		testresult = SYSDIAG_FAILED;
#ifdef  DEBUG
		printf("\nloop3_core: aalsap_nmp_send() FAILED!\n");
#endif  /* DEBUG */
   }
	bufmgr_freebuf(pAcpDataPool, (tpBUFFER) MAP_SH_ACP_SAR(pBuf));

	send_packet_count++;

	/* 
	 * ask Sage to pick up test packet in Sage's packet buffer 
	 */
   err_code = SageRxPacket(tstbuf);
	if (err_code)
		return(err_code);


#ifdef  DIAGDATA 
   /* print packet data */
	for (i= 0; i< datalen ; i++)
	{
		if ((i % 16) == 0) printf("\n0x%.2X:  ", i);
			printf("%2.2X ", tstbuf[i]);
	}
   printf("\n");
#endif  /* DIAGDATA */

	rc = memcmp1(&tstbuf, (char *)&pBuf->buf_data, datalen) ;
	if (rc)
	{
#ifdef DEBUG
		printf("Loop3_core: data compared failed\n");
#endif /* DEBUG */
		err_code = LPBKDIAG_PKTDATA_ERROR;
	}

	return(err_code);    

} /* loop3_core() */


/*-------------------------------------------------------------------*
 * nmploop_core()
 *   core function to respond to NMP's loopback test
 *-------------------------------------------------------------------*/
int
nmploop_core()
{
	tpQDESC  pAcpDataPool = acp_Shram_info.shram_pAcpToSarPool;
	UINT8		*pdata;
	UINT32	datalen, i;
	int		rc, err_code;

	tpBUFFER pBuf = NULLP;
	tTXSEND_REQ	Sreq;

	err_code = LPBKDIAG_OK;
	recv_packet_count = 0;

#ifdef  DIAGDATA  
	printf("loop5_core: recv packet count: 0x%x\n", recv_packet_count);
	printf("loop5_core: recv seq_err count: 0x%x\n", seq_err);
	printf("loop5_core: recv data_err count: 0x%x\n", data_err);
	printf("loop5_core: recv crc_err count: 0x%x\n", crc_err);
	printf("loop5_core: recv rto_err count: 0x%x\n", rto_err);
	printf("loop5_core: recv buf_ovfl_err count: 0x%x\n",buf_ovfl_err);
	printf("loop5_core: recv len_err count: 0x%x\n", len_err);
	printf("RXSAR debug0: 0x%x\n", acp_pRxSpram->rxspram_info.debug0);
	printf("RXSAR debug1: 0x%x\n", acp_pRxSpram->rxspram_info.debug1);
	printf("RXSAR debug2: 0x%x\n", acp_pRxSpram->rxspram_info.debug2);
	printf("RXSAR debug3: 0x%x\n", acp_pRxSpram->rxspram_info.debug3);

	if (acp_pRxSpram->rxspram_info.debug2)
	{
		q_log(acp_Rxspram_info.rxspram_pAcpBufPool);
		q_log(acp_Rxspram_info.rxspram_pRxToAcpDataq);
	}

#endif  /* DIAGDATA */

	do_print_recvdata = 1;

#ifdef  DEBUG
	printf("\nNMP loopback test: waiting for loopbacked cell\n");
#endif  /* DEBUG */

/*****************************************************************/
{
	int  rxstat= 0;

	/* Watch for NMP commanding loop test over, or other SCP cmds. */

	do {
		rxstat |= ADVR_RxSarPollSvc(FALSE);  /* non-blocking */
	} while (! LCP_DriverInit_4 (TRUE)) ;

	if (!rxstat)
	{
#ifdef  DEBUG 
		printf("nmp loopback test: no loopback cell\n"); 
#endif  /* DEBUG */
		err_code = LPBKDIAG_NORXCELL_ERROR;
	}
}  
/*****************************************************************/
	
	return(err_code);    

} /* nmploop_core() */


/*-------------------------------------------------------------------*
 * nmploop_uep()
 *    upcall handler for NMP loopback test through Synergy backplane 
 *-------------------------------------------------------------------*/
void
nmploop_uep(tVPC	  vpc,
          UINT16  event,
          UINT32  length,
          void    *pData)
{
	UINT32 i;
	UINT8	*pCh;
	UINT32	*pW;

#ifdef DEBUG
    printf("\n\rnmploop_uep: vpc= %d\n", vpc);
    printf("\tnmploop_uep: length %d\n", length);
    printf("\tnmploop_uep: data %x\n", *(UINT32 *)pData);
#endif  /* DEBUG */
	
	/*
	 *	skip first 2 bytes extra LEC id added by TxSAR
	 * need to check input length???
	 */
	pCh = (UINT8 *)pData + 2;        
	loop5_sendPkt(pCh, length -2);     /* play back the message */

	if(do_print_recvdata)
	{

#ifdef LOG_DATA
		pW = (UINT32 *)pData;
		pCh = (UINT8 *)pData;
		printf("\r nmploop_uep: DATA:\n\r",0);

		for(i=0;i<length; i++)
		{
         if ((i % 16) == 0) printf("\n0x%.2X:  ", i);
			printf("%.2x ",*pCh++);
		}
      printf("\n");

		if(!(recv_packet_count % PRINT_AFTER_THIS_MANY))
		{
			printf("%s:nmploop_uep: recv packet count: 0x%x\n", 
						__FILE__, recv_packet_count);
		}
#endif  /* LOG_DATA */


	} /* !do_print_recvdata */

	++recv_packet_count;

   return;
}  /* nmploop_uep() */


/*-------------------------------------------------------------------*
 * loop5_sendPkt()
 *
 *-------------------------------------------------------------------*/
int
loop5_sendPkt(char *pkt, int length)
{
	tpQDESC  pAcpDataPool = acp_Shram_info.shram_pAcpToSarPool;
	UINT8		*pdata;
	UINT32	datalen;
	int		err_code;
	int 		i;

	tpBUFFER pBuf = NULLP;
	tTXSEND_REQ	Sreq;

	err_code = LPBKDIAG_OK;

	send_packet_count = 0;

	if (pkt == (char *)0 )
	{
#ifdef  DEBUG  
		printf("no data to send\n");
#endif  /* DEBUG */ 
		return(LPBKDIAG_PKTDATA_ERROR);
	}

   if( (pBuf = (tpBUFFER)bufmgr_getbuf(pAcpDataPool)) == NULLP )
	{
#ifdef  DEBUG 
      printf("\rnmploop_sendPkt: bufmgr_getbuf failed\n");
      printf("\rnmploop_sendPkt: pAcpDataPool:%x\n", pAcpDataPool);
#endif  /* DEBUG */
      q_log(pAcpDataPool);
      shmem_log();
      spram_log();
		err_code = LPBKDIAG_GETBUF_ERROR;
      return(err_code);
	}

   /*
    * The buffer returned is in the TX SHRAM address space,
    * convert to acp space
    */
   pBuf = (tpBUFFER) MAP_SH_SAR_ACP(pBuf);
   pdata = (UINT8 *)&pBuf->buf_data;
	datalen = length;
	memcpy(pdata, pkt, length);

#ifdef DIAGDATA 
	printf("\t nmploop_sendPkt(): sending %d bytes\n", datalen);
	printf("\t nmploop_sendPkt(): sending pdata: 0x%x\n", &pBuf->buf_data);
	   /* print packet data */
      for (i= 0; i< datalen ; i++)
      {
         if ((i % 16) == 0) printf("\n0x%.2X:  ", i);
	         printf("%2.2X ", *pdata++);
      }
   printf("\n");

#endif  /* DIAGDATA */

	send_packet_count++;

 	/* Send packet out */
   if(aalsap_nmp_send(0, datalen, (void *)&pBuf->buf_data) != 0)
	{
		err_code	 = LPBKDIAG_SENDNMP_ERROR;
#ifdef  DEBUG
		printf("\nnmploop_sendPkt: aalsap_nmp_send() FAILED!\n");
#endif  /* DEBUG */
   }
	bufmgr_freebuf(pAcpDataPool, (tpBUFFER) MAP_SH_ACP_SAR(pBuf));

	return(err_code);    

} /* nmploop_sendPkt() */

/*-------------------------------------------------------------------*
 * DVT_core()
 *   DVT loopback test. 
 *
 *-------------------------------------------------------------------*/
int
DVT_core()
{
    tpQDESC  pAcpDataPool = acp_Shram_info.shram_pAcpToSarPool;
    UINT8       *pdata;
    UINT32  datalen, i;
    tVPC        vpc = SYSTST_VPC;
    static UINT8    seq=0;
    int     rc, err_code;
    int     idle_count;
    char    tmpbuf[5];
	

    tpBUFFER pBuf = NULLP;
    tTXSEND_REQ Sreq;

    err_code = LPBKDIAG_OK;

	/* 
	 * inform RxSAR MAC swapping
	 */
	if (dvt_test_param.mac_swap)
	{
		acp_pRxSpram->rxspram_info.debug0 = 1;
		printf("RXSAR debug0: 0x%x\n", acp_pRxSpram->rxspram_info.debug0);
	}
	else
	{
		acp_pRxSpram->rxspram_info.debug0 = 0;
		printf("RXSAR debug0: 0x%x\n", acp_pRxSpram->rxspram_info.debug0);
	}

	/*
	 * DVT loopback through ACP
	 */
    if (dvt_test_param.dvt_thru_acp)
    {
        if( aalsap_open_vcc( 0, vpc, 0, vpc, 0,
        VPC_TX_ACTIVE, VPC_DEST_ACP, dvt_test_param.txpkt_vlan /* 1 */ , 0,  dvt_uep) == 0)
        {
#ifdef  DEBUG
          printf("DVT_core: aalsap_open_vcc for ACP failed %d\n", vpc);
#endif  /* DEBUG */
            err_code = LPBKDIAG_VCCOPEN_ERROR;
            return(err_code);
        }
    }
    else
    {
		/*
		 * DVT loopback through RxSAR
		 */
        if( aalsap_open_vcc( 0, vpc, 0, vpc, 0,
			VPC_TX_ACTIVE, VPC_DEST_BIGA, dvt_test_param.txpkt_vlan, 0, test_callback) == 0)
        {
#ifdef  DEBUG
            printf("DVT_core: aalsap_open_vcc for BIGA failed %d\n", vpc);
#endif  /* DEBUG */
            err_code = LPBKDIAG_VCCOPEN_ERROR;
            return(err_code);
        }
        if (aalsap_bind_vlan_to_vc(0, vpc, dvt_test_param.rxpkt_vlan, VPC_DEST_BIGA) == 0)
		{
#ifdef  DEBUG
            printf("DVT_core: aalsap_bind_vlan_to_vc for BIGA failed %d\n", vpc
);
#endif  /* DEBUG */
            err_code = LPBKDIAG_VCCOPEN_ERROR;
            return(err_code);
            printf("Get in RxSar Loopback mode \n");
        }

        return(err_code);

    }

    send_packet_count = 0;
    recv_packet_count = 0;
    idle_count = 0 ;

    do_print_recvdata = 1;

#ifdef  DEBUG
    printf("DVT Test: waiting for loopbacked cell on vpc: %d\n", vpc);
#endif  /* DEBUG */

    while (idle_count < 10)
    {

/*****************************************************************/
{
        int  loop_cnt = 2000;
        int  rxstat= 0;

    rxstat = ADVR_RxSarPollSvc(FALSE);   /* non-blocking */

   while ((rxstat == 0) && loop_cnt--)
   {
     /* non-blocking poll */
        MFP_PollForIntr (MFP_IMSK_RSAR, FALSE, TRUE, TRUE);
        rxstat = ADVR_RxSarPollSvc(FALSE);  /* non-blocking */
        PUNCH_WD;
   }
    if (!rxstat)
    {
#ifdef  DEBUG
        printf("DVT_core: no loopback cell\n");
        printf("DVT_core: recev_count = %x \n", recv_packet_count);
#endif  /* DEBUG */
        err_code = LPBKDIAG_NORXCELL_ERROR;
        idle_count++ ;
    }
    else
        idle_count = 0 ;
} 
   
/*****************************************************************/
        /*  getline(tmpbuf, 5);  */
    }

    aalsap_close_vcc( vpc, 0);

#ifdef  0
    getSaintReg();
#endif  /* DEBUG */

    return(err_code);

} /* DVT_core() */

/*-------------------------------------------------------------------*
 * dvt_uep()
 *    upcall handler for dvt  packet demux
 *-------------------------------------------------------------------*/
void
dvt_uep(tVPC      vpc,
          UINT16  event,
          UINT32  length,
          void    *pData)
{
    UINT32 i;
    UINT8   *pCh;
    UINT32  *pW;
    char  tmp;

#if 0
    printf("\n\rdvt_uep: vpc= %d\n", vpc);
    printf("\tdvt_uep: length %d\n", length);
    printf("\tdvt_uep: data %x\n", *(UINT32 *)pData);
    printf(".");
#endif /* ZERO */
   
    /*
     *  skip first 2 bytes extra LEC id added by TxSAR
     * need to check input length???
     */
    pCh = (UINT8 *)pData + 2;

	if (dvt_test_param.mac_swap)
	{
    /* swap da/sa mac */
		for(i=0;i< 6 ;i++)
		{
			tmp = *(pCh+i);
			*(pCh+i) =  *(pCh+i+6);
			*(pCh+i+6) = tmp;
		}
	}

    loop5_sendPkt(pCh, length -2);     /* play back the message */

    if(do_print_recvdata)
    {


#if  1
        pW = (UINT32 *)pData;
        pCh = (UINT8 *)pData;
        printf("\r dvt_uep: DATA:\n\r",0);

        for(i=0;i<length; i++)
        {
         if ((i % 16) == 0) printf("\n0x%.2X:  ", i);
            printf("%.2x ",*pCh++);
        }
      printf("\n");

#endif  /* LOG_DATA */

        if(!(recv_packet_count % PRINT_AFTER_THIS_MANY))
        {
            printf("%s:dvt_uep: recv packet count: 0x%x\n",
                        __FILE__, recv_packet_count);
        }

    } /* !do_print_recvdata */

    ++recv_packet_count;

   return;
}  /* dvt_uep() */


/*-------------------------------------------------------------------
 * test_callback()
 *-------------------------------------------------------------------
 */
void
test_callback(tVPC    vpc,
          UINT16  event,
          UINT32  length,
          void    *pData)
{
    ++recv_packet_count;
}  /* test_callback() */


/*--------------------------------------------------------------------*
 * show_stats()
 *    statistics dump from SARs
 *--------------------------------------------------------------------*/
void
show_stats (void)
{

#ifdef  HEX 
    printf("RxSAR Statistics :\n");
    printf("   Rx Packet Counts:                 %x\n",
            acp_pRxSpram->rxstat_gl.rxgl_packet_input);

    printf("   Rx Cell Unknown VC Errors:        %x\n",
	    acp_pRxSpram->rxstat_gl.rxgl_cell_unknownvc_err);

    printf("   Rx Cell Buffer Unavail Errors:    %x\n",
	    acp_pRxSpram->rxstat_gl.rxgl_cell_no_buf_err);

    printf("   Rx Packet Time-out Errors:        %x\n",
            acp_pRxSpram->rxstat_gl.rxgl_packet_rto_err);

    printf("   Rx Packet CRC Errors:             %x\n",
            acp_pRxSpram->rxstat_gl.rxgl_packet_crc_err);

    printf("   Rx Packet Buffer Overflow Errors: %x\n",
            acp_pRxSpram->rxstat_gl.rxgl_packet_bufovl_err);

    printf("   Rx Packet BIGA Overflow Errors:   %x\n",
            acp_pRxSpram->rxstat_gl.rxgl_packet_bigaovl_err);

    printf("TxSAR Statistics :\n");
    printf("   SAR Tx Packet Counts:             %x\n",
            acp_pTxSpram->txstat_gl.txgl_upacket_output);

#else  /* HEX */
    printf("RxSAR Statistics :\n");
    printf("   Rx Packet Counts:                 %lu\n",
            acp_pRxSpram->rxstat_gl.rxgl_packet_input);

    printf("   Rx Cell Unknown VC Errors:        %lu\n",
	    acp_pRxSpram->rxstat_gl.rxgl_cell_unknownvc_err);
    
    printf("   Rx Cell Buffer Unavail Errors:    %lu\n",
	    acp_pRxSpram->rxstat_gl.rxgl_cell_no_buf_err);

    printf("   Rx Packet Time-out Errors:        %lu\n",
            acp_pRxSpram->rxstat_gl.rxgl_packet_rto_err);

    printf("   Rx Packet CRC Errors:             %lu\n",
            acp_pRxSpram->rxstat_gl.rxgl_packet_crc_err);

    printf("   Rx Packet Buffer Overflow Errors: %lu\n",
            acp_pRxSpram->rxstat_gl.rxgl_packet_bufovl_err);

    printf("   Rx Packet BIGA Overflow Errors:   %lu\n",
            acp_pRxSpram->rxstat_gl.rxgl_packet_bigaovl_err);

    printf("TxSAR Statistics :\n");
    printf("   SAR Tx Packet Counts:             %lu\n",
            acp_pTxSpram->txstat_gl.txgl_upacket_output);
#endif  /* HEX */

#if  0
		printf("RXSAR debug0: 0x%x\n", acp_pRxSpram->rxspram_info.debug0);
#endif /* ZERO */

}  /* show_stats() */

/*---------------------------< END >-----------------------------------*/
