/* $Id: lcpdiag.c,v 1.1.4.1 1996/05/09 15:01:08 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/lcpdiag.c,v $
 *------------------------------------------------------------------
 * lcpdiag.c -- support functions to do system loopback tests.
 *
 * 1995, rlb (adapted to Synergy ATM line card)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lcpdiag.c,v $
 * Revision 1.1.4.1  1996/05/09  15:01:08  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:07  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:30  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:17  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  10:52:46  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:54:02  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:36  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.8.2.1  1995/11/16  00:09:33  rbau
 * merge mainline code changes in 1.4 branch
 *
 * Revision 1.9  1995/11/08  00:38:49  rbau
 * added EnDisSage SCP support function for off-line diag
 *
 * Revision 1.8  1995/10/13  18:15:56  rbau
 * added cvs keywords in the header
 *
 * Revision 1.7  1995/08/27  23:51:19  rbau
 * returning error codes for failed SCP messages, more code clean up
 *
 * Revision 1.6  1995/07/26  23:15:16  rlowe
 * Try once again to fix the file access modes.  Also do a few cosmetic
 * touches to log comments.
 *
 * Revision 1.5  1995/07/21  00:28:04  rbau
 * file permission was screwed up on the previous check-in
 *
 * Revision 1.4  1995/07/20  23:58:15  rbau
 * added support to enable/disable sage.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include  "acp_basic_types.h"
#include  "scp_defs.h"
#include  "scp_msgs.h"
#include  "st_regs.h"
#include  "../alc_drivers/lcp_defs.h"
#include  "lcp_if.h"
#include  "acp_c_defs.h"
#include  "mfp_acp_defs.h"
#include  "lo_wdog.h"
#include  "atmdiag.h"
#include  "errcode.h"


tLCP_DvrBufU   LCPT_TxBuf ;
tLCP_DvrBufU   LCPT_RxBuf ;

u_int8  MySlotNum = 4;     /* one based */
u_int8  NmpSlotNum = 1;    /* one based */

/* special MAC address used by TxSAR CAM search for loopback test */ 
char diag_dmac[] = {0x00, 0x02, 0x03, 0x04, 0x05, 0x06 };

/*---< Prototypes >---*/
int   memcmp1(u_int8 *, u_int8 *, int);
static int wait_for_rsp_pkt(int);
int EnDisSage(int);

/*-------------------------------------------------------------------*
 * LCP_EchoTest()
 *    Echo test on  LCP/ACP communication 
 *
 * Return Value:
 *   0 if successfull sending message
 *  -1 if SCP error 
 *-------------------------------------------------------------------*
 */
int
LCP_EchoTest()
{
	uChar   msglen;
	int     num = 1;
	int     rc, i;
	char	*data_ptr;
	int	  err_code = LPBKDIAG_OK;

	MySlotNum = LCP_PhysSlotNbr();

/* -------------------------------------------------------------------- */
/* This routine formats, sends, and compares Echo messages with			*/
/* the local LCP.  Provides a convenient way of checking on LCP			*/
/* performance too.                 */
/* Note: excerpted from lcp_test2.c for temp test                       */
/* -------------------------------------------------------------------- */

	memset (LCPT_TxBuf.BF, 0, sizeof (LCPT_TxBuf.BF)) ;
	memset (LCPT_RxBuf.BF, 0, sizeof (LCPT_RxBuf.BF)) ;

	/* Allocate room in front of string for a 4-byte counter. */
	/* Align counter, so need extra pad byte in front of it.  */

	for (i=0; i< (MAX_SCP_PKT_DATA_SIZE-5); i++)
		LCPT_TxBuf.BU.msg.mdata [5+ i]  = i;

	msglen = MAX_SCP_PKT_DATA_SIZE ;

#ifdef  LCPDBG 
	printf ("   echo msg body len %d\n",  msglen) ;
#endif  /* LCPDBG */

	LCPP_FmtHeader (& LCPT_TxBuf.BU, TRUE, msglen, LCP_LCL_8051_SAP,
#ifdef   ACP_REV0_LCP
         (LCP_ACP_SAP_FLAG | LCP_ACP_OWN_RESET_SAP),
#else
         LCP_LCL_8051_SAP,
#endif
         LCP_MSG_SCP_PING) ;

	while (num > 0)
	{
		-- num ;

		++ * (u_int32 *)(& LCPT_TxBuf.BU.msg.mdata [1]) ;
		rc = LCP_TransmitPoll (TRUE, TRUE, FALSE, & LCPT_TxBuf.BU) ;
		if (rc != LCP_RC_Normal)
		{
#ifdef  DEBUG
			printf ("   LCP_TransmitPoll rtn %d\n", rc) ;
#endif  /* DEBUG */
			return(LPBKDIAG_SCP_SEND_ERROR);
		}

		LCP_ReceivePoll  (FALSE, & LCPT_RxBuf.BU) ;

		/*
		 * wait on the SCP response  message  
		 */	
		if ( wait_for_rsp_pkt(2))
			return(LPBKDIAG_SCP_RCV_ERROR);

#ifdef LCPDIAG
		if (LCPT_TxBuf.BU.msg.length != LCPT_RxBuf.BU.msg.length)
			printf (" msg hdr len mismatch %02x %02x\n",
						LCPT_TxBuf.BU.msg.length, LCPT_RxBuf.BU.msg.length) ;

		if (LCPT_TxBuf.BU.msg.opcode!= LCPT_RxBuf.BU.msg.opcode)
			printf (" msg hdr opcode mismatch %02x %02x\n",
						LCPT_TxBuf.BU.msg.opcode, LCPT_RxBuf.BU.msg.opcode) ;
#endif  /* LCPDIAG */

		rc |= memcmp1(LCPT_TxBuf.BU.msg.mdata, LCPT_RxBuf.BU.msg.mdata,
				msglen) ;

		if (rc)
		{
			err_code = LPBKDIAG_PKTDATA_ERROR;
#ifdef  DEBUG
			printf (" body memcmp did not match = %d\n", rc) ;
#endif  /* DEBUG */
		}
   }

	return(err_code);

}  /* LCP_EchoTest() */


/*-------------------------------------------------------------------*
 * memcmp1()
 * 
 * return:
 *    0  -  match
 *    -1 -  failed 
 * renamed to memcmp1 so not to conflict with memcmp() in lcp_test2.c
 *-------------------------------------------------------------------*/
int
memcmp1(u_int8 *str1, u_int8 *str2, int len)
{

	while (len > 0)
	{
		if (*str1 != *str2)
			return (-1) ;     

		--len ;
		++str1 ;
		++str2 ;
	}

	return (0) ;            /* Match !! */
}

/*-------------------------------------------------------------------*
 * writeSaintReg - writes one register to a saint (or sage)
 *
 * Note: ATM line card only supports 1 Sage port 
 *-------------------------------------------------------------------*/
int
writeSaintReg(
	int slot,     			/* 1 based */
	int port,    			/* 1 based which saint */
	int subOpcode,       /* SAINT_SETBITS, SAINT_CLRBITS,
                         * SAINT_SETREGS, SAINT_GETREGS */
   int regOffset,       /* ie: MAC_CTL2 */
   int value )   			/* EXTERNAL_LOOPBACK */
{
	int 	rc;
	int	i;
	char	*data_ptr;

   PSAINT_REGS_BY_ID SetSaintMesg =
			(PSAINT_REGS_BY_ID)&LCPT_TxBuf.BU.msg.addr;
	PSAINT_REGS_BY_ID SetSaintMesgReply =
			(PSAINT_REGS_BY_ID)&LCPT_RxBuf.BU.msg.addr;


	SetSaintMesg->scphdr.dest_addr = slot - 1;
	SetSaintMesg->scphdr.src_addr  = slot - 1;
	SetSaintMesg->scphdr.dsap      = 0;
	SetSaintMesg->scphdr.ssap      = 0;
	SetSaintMesg->scphdr.opcode    = SCP_SET_SAINT_REGS_BY_ID;
	SetSaintMesg->scphdr.length    = 6;
	SetSaintMesg->cmdStatus        = subOpcode;
	SetSaintMesg->WhichSaint.SaintId = port-1;   /* change to 0 based */
	SetSaintMesg->NoOfSaintsSpecified = 1;
	SetSaintMesg->WhichSaint.NoOfRegistersSpecified = 1;
	SetSaintMesg->WhichSaint.RegAndValue[0].RegOffset = regOffset;
	SetSaintMesg->WhichSaint.RegAndValue[0].RegValue  = value;


#ifdef  SAGEDBG 
   for (i = 0, data_ptr = (char *)SetSaintMesg; i < 11; i++)
	{
		printf("%.2X ", *data_ptr++ & 0xff);
   }
   printf("\n");
#endif /* SAGEDBG */

   /* Send the messge and wait for a reply */
	rc = LCP_TransmitPoll(TRUE, TRUE, FALSE, & LCPT_TxBuf.BU) ;
	if (rc != LCP_RC_Normal)
	{
#ifdef  DEBUG
		printf ("   LCP_TransmitPoll rtn %d\n", rc) ;
#endif  /* DEBUG */
      return(LPBKDIAG_SCP_SEND_ERROR);
	}

	LCP_ReceivePoll (FALSE, & LCPT_RxBuf.BU) ;

	/* 
	 * wait for SCP response message 
	 */
	if ( wait_for_rsp_pkt(2))
		return(LPBKDIAG_SCP_RCV_ERROR);  
			
	if (SetSaintMesgReply->cmdStatus != SCPSTAT_OK)
   {
#ifdef  SAGEDBG
printf("   writeSaintRegs: SCP Command failed, status = %.2X\n",
   	 SetSaintMesgReply->cmdStatus);
#endif  /* SAGEDBG */

		return(LPBKDIAG_SCP_RCV_ERROR);  
   }

   return(LPBKDIAG_OK);
}  /* writeSaintReg() */


/*-------------------------------------------------------------------*
 * getSaintReg()
 *    dump SAGE registers
 *-------------------------------------------------------------------*/
int 
getSaintReg()
{

	PSAINT_DUMP_REGS SaintRegsMesg =
		(PSAINT_DUMP_REGS)&LCPT_TxBuf.BU.msg.addr;

	uInt32  SaintId, i;
	uChar *data_ptr;
	Int32		rc;

	SaintId = 1;

	/* Fill in message */
	SaintRegsMesg->scphdr.src_addr = MySlotNum - 1;
	SaintRegsMesg->scphdr.dest_addr = MySlotNum - 1;

	SaintRegsMesg->scphdr.dsap = 0;
	SaintRegsMesg->scphdr.ssap = 0;
	SaintRegsMesg->scphdr.opcode = SCP_DUMP_SAINT_REGS;
	SaintRegsMesg->scphdr.length = 2;
	SaintRegsMesg->Status = 0xFF;

	SaintRegsMesg->SaintId = (uChar)(SaintId - 1);

#ifdef  SAGEDBG 
    printf("\nMesg: ");
    for (i = 0, data_ptr = (char *)SaintRegsMesg; i < 7; i++)
    {
        printf("%.2X ", *data_ptr++);
    }
    printf("\n");
#endif  /* SAGEDBG */

   /* Send the messge and wait for a reply */
   rc = LCP_TransmitPoll(TRUE, TRUE, FALSE, & LCPT_TxBuf.BU) ;
   if (rc != LCP_RC_Normal)
	{
#ifdef  DEBUG
      printf ("   LCP_TransmitPoll rtn %d\n", rc) ;
#endif  /* DEBUG */
		return(LPBKDIAG_SCP_SEND_ERROR);
	}

   LCP_ReceivePoll(FALSE, & LCPT_RxBuf.BU);

	/*
	 * wait on  SCP response message  
	 */	
	if ( wait_for_rsp_pkt(3))
		return(LPBKDIAG_SCP_RCV_ERROR);

#ifdef  SAGEDBG 
   printf("\nReturn Mesg: ");
   for (i = 0, data_ptr = &LCPT_RxBuf.BU.msg.addr; i < 7; i++)
   {
      printf("%.2X ", *data_ptr++);
   }
    printf("\n");

	SaintRegsMesg = (PSAINT_DUMP_REGS)&LCPT_RxBuf.BU.msg.addr;

   /* Check for good return status from LCP */
   if (SaintRegsMesg->Status == SCPSTAT_OK)
   {
       printf("\n0x40:  XX XX XX XX XX XX XX XX ");
       for (i = 0x48, data_ptr = &SaintRegsMesg->regData[0];
           i <= 0xFF; i++)
       {
         if ((i % 16) == 0) printf("\n0x%.2X:  ", i);
            printf("%.2X ", *data_ptr++);
       }
       printf("\n");

   }
   else
   {
		/* Command failed at MCP/LCP end */
      printf("Error: DumpSaintReg Command failed, Return Status = %.2X\n",
             SaintRegsMesg->Status);
	}
#endif  /* SAGEDBG */

    return(LPBKDIAG_OK);
}  /* getSaintReg() */


/*-------------------------------------------------------------------*
 * EnableSage()
 *		cmd : 0   disable Sage
 *		cmd : 1   enable Sage
 *-------------------------------------------------------------------*/
int
EnableSage(int cmd)
{
   PENABLE_DISABLE_MSG  EDMesg =
			(PENABLE_DISABLE_MSG)&LCPT_TxBuf.BU.msg.addr;
   uInt32  SaintId, i;
   uChar *data_ptr;
	int	rc;

   SaintId = 1;

	/* Fill in message */
	EDMesg->hdr.src_addr = MySlotNum - 1;
	EDMesg->hdr.dest_addr = MySlotNum - 1;

	EDMesg->hdr.dsap = 0;
	EDMesg->hdr.ssap = 0;
	EDMesg->hdr.length = 7;
	EDMesg->hdr.opcode = SCP_ENADIS_MODPORT;
	if (cmd)
		EDMesg->cmdStatus = COMMAND_PORT_ENABLE;
	else	
		EDMesg->cmdStatus = COMMAND_PORT_DISABLE;
	EDMesg->ModuleOrPortMask = 1;   
	EDMesg->SubCommandBits = SUBCOMMAND_SET_SAINT_DEFAULTS;

#ifdef  SAGEDBG
	printf("\nMesg: ");
	for (i = 0, data_ptr = &LCPT_TxBuf.BU.msg.addr; i < 12; i++)
	{
        printf("%.2X ", *data_ptr++);
    }
    printf("\n");
#endif  /* SAGEDBG */

   /* Send the messge and wait for a reply */
   rc = LCP_TransmitPoll(TRUE, TRUE, FALSE, & LCPT_TxBuf.BU) ;
	if (rc != LCP_RC_Normal)
	{
#ifdef  DEBUG
		printf ("   LCP_TransmitPoll rtn %d\n",  rc) ;
#endif  /* DEBUG */
		return(LPBKDIAG_SCP_SEND_ERROR);
	}

	LCP_ReceivePoll (FALSE, & LCPT_RxBuf.BU) ;

	/*
	 * filter out unsolicited SCP message  
	 */	
	if ( wait_for_rsp_pkt(2))
		return(LPBKDIAG_SCP_RCV_ERROR);
			

#ifdef  SAGEDBG
   printf("\nReturn Mesg: ");
   for (i = 0, data_ptr = &LCPT_RxBuf.BU.msg.addr; i < 7; i++)
   {
      printf("%.2X ", *data_ptr++);
   }
   printf("\n");
#endif  /* SAGEDBG */

	return(LPBKDIAG_OK);

}  /* EnableSage() */


/*-------------------------------------------------------------------*
 * SageTxPacket()
 *-------------------------------------------------------------------*/
int
SageTxPacket()
{
	PTRANSMIT_PACKET  SageTxPkt =
	(PTRANSMIT_PACKET)&LCPT_TxBuf.BU.msg.addr;

	int   SaintId;
	uChar *data_ptr;
	int   i, rc;

	SaintId = 1;

	/* Fill in message */
	SageTxPkt->scphdr.src_addr = MySlotNum - 1;
	SageTxPkt->scphdr.dest_addr = MySlotNum - 1;

	SageTxPkt->scphdr.dsap = 0;
	SageTxPkt->scphdr.ssap = 0;

	/* 2 scpmsg operands+synhdr + SA+DA + 180 */
	SageTxPkt->scphdr.length = 206;  

	SageTxPkt->scphdr.opcode = SEND_PACKET;
	SageTxPkt->PortId = SaintId -1;
	SageTxPkt->Direction = 0;    /* to network */

	for (i=0; i< 12; i++)
		SageTxPkt->PacketData[i] = 0;

#ifdef COLOR1
   SageTxPkt->PacketData[3] = 0x01 << 1;  /* set vlan color 1 */
#else  /* COLOR1 */	
   SageTxPkt->PacketData[3] = 0x0 ; 		/* set vlan color 0 */
#endif  /* COLOR1 */

	SageTxPkt->PacketData[6] = 0x10;   /* set Status S4 ignore CRC */
	SageTxPkt->PacketData[8] = 0;
	SageTxPkt->PacketData[9] = 0xC0;  /* length =192 bytes, w/o syn_hdr */
	SageTxPkt->PacketData[7] =        /* length parity */
		SageTxPkt->PacketData[8] ^ SageTxPkt->PacketData[9];

	data_ptr = &SageTxPkt->PacketData[12];

   /* fill in DA & SA */
	for (i=0; i < 6; i++)
		*data_ptr++ = diag_dmac[i];

	for (i=0; i < 6; i++)
		*data_ptr++ = diag_dmac[i] + 0x10 ; 

	data_ptr = &SageTxPkt->PacketData[24];

   /* put in  test data 180 byte, */
	for (i = 0; i < 180; i++)
		*data_ptr++ = i;

#ifdef  SAGEDBG 
   printf("\nMesg: ");
   for (i = 0, data_ptr = (char *)SageTxPkt; i < 19; i++)
	{
		printf("%.2X ", *data_ptr++);
	}
	printf("\n\n");

	/* print packet data */
	for (i= 19 ; i< 211 ; i++)
	{
		if ((i % 16) == 0) printf("\n0x%.2X:  ", i);
			printf("%2.2X ", *data_ptr++);
	}
   printf("\n");
#endif  /* SAGEDBG */

	/* Send the messge and wait for a reply */
	rc = LCP_TransmitPoll(TRUE, TRUE, FALSE, & LCPT_TxBuf.BU) ;
	if (rc != LCP_RC_Normal)
	{
#ifdef  DEBUG
		printf ("   LCP_TransmitPoll rtn %d\n",  rc) ;
#endif  /* DEBUG */
		return(LPBKDIAG_SCP_SEND_ERROR);
	}

   LCP_ReceivePoll(FALSE, & LCPT_RxBuf.BU);

	/*
	 * wait on SCP response message  
	 */	
	if ( wait_for_rsp_pkt(2))
		return(LPBKDIAG_SCP_RCV_ERROR);
			

#ifdef  SAGEDBG  
   getSaintReg();
#endif  /* SAGEDBG */

   return rc;
}  /* SageTxPacket */

/*-------------------------------------------------------------------*
 * SageRxPacket()
 *-------------------------------------------------------------------*/
int
SageRxPacket(char *ret_msg)
{
	PRECEIVE_PACKET  SageRxPkt = (PRECEIVE_PACKET)&LCPT_TxBuf.BU.msg.addr;
	int   SaintId;
	uChar *data_ptr;
	int   i, rc;

	SaintId = 1;

	/* Fill in message */
	SageRxPkt->scphdr.src_addr = MySlotNum - 1;
	SageRxPkt->scphdr.dest_addr = MySlotNum - 1;

	SageRxPkt->scphdr.dsap = 0;
	SageRxPkt->scphdr.ssap = 0;

	/* 2 scpmsg operands+synhdr + SA+DA + 180 */
	SageRxPkt->scphdr.length = 207;   

	SageRxPkt->scphdr.opcode = RCV_PACKET;
	SageRxPkt->PortId = SaintId -1;
	SageRxPkt->Direction = 0;    /* from network */

	/* Send the messge and wait for a reply */
	rc = LCP_TransmitPoll(TRUE, TRUE, FALSE, & LCPT_TxBuf.BU) ;
	if (rc != LCP_RC_Normal)
	{
#ifdef  DEBUG
		printf ("   LCP_TransmitPoll rtn %d\n",  rc) ;
#endif  /* DEBUG */
		return (LPBKDIAG_SCP_SEND_ERROR);
	}

	LCP_ReceivePoll(FALSE, & LCPT_RxBuf.BU);
	
	/*
	 * filter out unsolicited SCP message  
	 */	
	if ( wait_for_rsp_pkt(3))
		return (-1);     /* no response */
			

	SageRxPkt = (PRECEIVE_PACKET)&LCPT_RxBuf.BU.msg.addr;

	data_ptr = &SageRxPkt->PacketData[12];
	memcpy(ret_msg, data_ptr, TSTPACKET_LEN+12);  /* 192 bytes */

#ifdef  SAGEDBG
	printf("\nSageRxPkt: after lcp_rx_pkt message\n ");
	printf("\nMesg: ");
	for (i = 0, data_ptr = (char *)SageRxPkt; i < 20; i++)
	{
		printf("%.2X ", *data_ptr++);
	}
	printf("\n\n");

	/* print packet data */
	for (i= 20; i< 211 ; i++)
	{
		if ((i % 16) == 0) printf("\n0x%.2X:  ", i);
			printf("%2.2X ", *data_ptr++);
	}
	printf("\n");
#endif  /* SAGEDBG */

	return 0;     /* received a packet */
}  /* SageRxPacket() */

/*--------------------------------------------------------------------*
 * spin_loop()
 *--------------------------------------------------------------------*/
void
spin_loop(int i)
{
   int loop_cnt;

	while ( i-- > 0)
   {
		loop_cnt = 3000;

		while(loop_cnt--)
			PUNCH_WD;
	}

}  /* spin_loop() */

/*--------------------------------------------------------------------*
 * SageOff_synbus()
 *    Enable Sage with default setup
 *    Disable Sage DMA from/to Synergy bus
 *--------------------------------------------------------------------*/
int
SageOff_synbus()
{

#ifdef  LCPDBG 
	printf("   Enable Sage with Default Setup\n");
#endif  /* LCPDBG */
	EnableSage(0);
	EnableSage(1);

#ifdef  SAGEDBG
	getSaintReg();
#endif  /* SAGEDBG */

#ifdef  LCPDBG
	printf("   Disable DMA between SAGE and Synergy Bus\n");
#endif  /* LCPDBG */

	writeSaintReg(MySlotNum, 1, SAINT_SETREGS, SAINT_CTRL1, 0x01);
	writeSaintReg(MySlotNum, 1, SAINT_SETREGS, SAINT_CTRL2, 0xE4);
#ifdef  DEBUG
	getSaintReg();
#endif  /* DEBUG */

}  /* SageOff_synbus() */


/*-------------------------------------------------------------------*
 * wait_for_scprsp_pkt()
 * input:   laps of spin_loop
 *
 * ret: -1  no response
 *       0  response
 *-------------------------------------------------------------------*/
static int 
wait_for_rsp_pkt(int laps)
{
	int  i;
	
	/*
	 * filter out unsolicited SCP message
    */
	for (i= 0; i< SCPMSG_FILTER_CNT; i++)
   {
		if (LCPT_TxBuf.BU.msg.addr != LCPT_RxBuf.BU.msg.addr)
		{
#ifdef  LCPDBG 
            printf (" msg hdr addr mismatch %02x %02x\n",
                  LCPT_TxBuf.BU.msg.addr, LCPT_RxBuf.BU.msg.addr) ;
#endif  /* LCPDBG */
            spin_loop(laps);
            LCP_ReceivePoll(FALSE, & LCPT_RxBuf.BU);

		}

		else if (LCPT_TxBuf.BU.msg.opcode!= LCPT_RxBuf.BU.msg.opcode)
		{
#ifdef  LCPDBG 
            printf (" msg hdr opcode mismatch %02x %02x\n",
                  LCPT_TxBuf.BU.msg.opcode, LCPT_RxBuf.BU.msg.opcode) ;
#endif  /* LCPDBG */
            spin_loop(laps);
            LCP_ReceivePoll(FALSE, & LCPT_RxBuf.BU);
		}
		else
			break;
	}
	
	if (i == SCPMSG_FILTER_CNT)	
		return -1;

	return 0;
}


/*-------------------------------------------------------------------*
 * EnDisSage()
 *    SCP support function to Enable/Disable Sage
 *-------------------------------------------------------------------*/
int
EnDisSage(int cmd)
{
   PENABLE_DISABLE_MSG  EDMesg =
            (PENABLE_DISABLE_MSG)&LCPT_TxBuf.BU.msg.addr;
   uInt32  SaintId, i;
   uChar *data_ptr;
    int rc;

   SaintId = 1;

    /* Fill in message */
    EDMesg->hdr.src_addr = MySlotNum - 1;
    EDMesg->hdr.dest_addr = MySlotNum - 1;

    EDMesg->hdr.dsap = 0;
    EDMesg->hdr.ssap = 0;
    EDMesg->hdr.length = 7;
    EDMesg->hdr.opcode = SCP_ENADIS_MODPORT;
    EDMesg->cmdStatus = cmd;

    EDMesg->ModuleOrPortMask = 1;
    EDMesg->SubCommandBits = SUBCOMMAND_SET_SAINT_DEFAULTS;

#ifdef  SAGEDBG
    printf("\nMesg: ");
    for (i = 0, data_ptr = &LCPT_TxBuf.BU.msg.addr; i < 12; i++)
    {
        printf("%.2X ", *data_ptr++);
    }

    printf("\n");
#endif  /* SAGEDBG */

   /* Send the messge and wait for a reply */
   rc = LCP_TransmitPoll(TRUE, TRUE, FALSE, & LCPT_TxBuf.BU) ;
    if (rc != LCP_RC_Normal)
    {
#ifdef  DEBUG
        printf ("   LCP_TransmitPoll rtn %d\n",  rc) ;
#endif  /* DEBUG */
        return(LPBKDIAG_SCP_SEND_ERROR);
    }

    LCP_ReceivePoll (FALSE, & LCPT_RxBuf.BU) ;

    /*
     * filter out unsolicited SCP message
     */
    if ( wait_for_rsp_pkt(2))
        return(LPBKDIAG_SCP_RCV_ERROR);
   
/* #ifdef  SAGEDBG  */
#if  1
   printf("\nReturn Mesg: ");
   for (i = 0, data_ptr = &LCPT_RxBuf.BU.msg.addr; i < 7; i++)
   {
      printf("%.2X ", *data_ptr++);
   }
   printf("\n");
#endif  /* SAGEDBG */

    return(LPBKDIAG_OK);

}  /* EnDisSage */

/*---------------------------< END >----------------------------------*/
