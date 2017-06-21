/* $Id: connstup.h,v 1.1.6.1 1996/04/19 14:57:11 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/vip/connstup.h,v $
 *------------------------------------------------------------------
 * connstup.h  connection setup constants needed on VIP.
 *
 * Mar, 1996 xliu 
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: connstup.h,v $
 * Revision 1.1.6.1  1996/04/19  14:57:11  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.4.1  1996/03/30  06:38:05  xliu
 * Branch: IosSec_branch
 * Commit before CA split.
 *
 * Revision 1.1  1996/03/30  06:03:51  xliu
 * Placeholder files.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*--------------------------------- constants --------------------------------- */

#define ECHO_REQUEST            0
#define ECHO_REPLY              1
#define ECHO_CONFIRM            2

#define ICMP_HEADER_SIZE        8
#define ECHO_HEADER_SIZE        2
#define AEP_HEADER_SIZE         1

#define CHALLENGE_SIZE          20
#define CHAL_ODD_MASK           0xAA
#define CHAL_EVEN_MASK          0x55

#define NULL_INDEX              -1
#define UNDEF_INDEX             -2
#define MAC_ADDR_LEN            6
#define MAC_CRC32_SIZE			4
#define MAC_TO_DSAP_LEN         14
#define MAC_TO_SSAP_LEN         15
#define IP_ADDR_LEN             4
#define RAND_GEN_SIZE			20

/*#define DEF_DSS_SIZE            64 */
#define MAX_DSS_SIZE            96
#define DSS_FIX_SIZE            20
/*#define DEF_DH_SIZE             64 */
/*#define MAX_DH_SIZE             96 */
#define DEF_SIG_SIZE            40
#define CY_KEY_SIZE             8
#define SKEY_OFFSET             0
#define IKEY_OFFSET             32
#define NNC_GRP_BLK_SIZE		12

/*NOTE: If SERIAL_NUM_SIZE changes, sanity check of far PE echoing back needs */
/*		to change accordingly. */
#define SERIAL_NUM_SIZE          4
#define PDU_NW_ADDR_SIZE         4
#define	DDP_NW_ADDR_SIZE		 2
#define PCONN_INDEX_SIZE         2
#define PDU_MAX_NW_ADDR_SIZE	 16

#define NNC_GRP_VEC_OFFSET      4
#define NNC_GRP_OC_OFFSET       2
#define NNC_CY_VEC_OFFSET       16
#define NNC_CUS_ID_OFFSET       2
#define NNC_FIX_GRP_OFFSET      20      /*except DSS public */
#define NNC_ENTITY_ID_OFFSET	8
#define NNC_REV_OFFSET          14
#define NNC_FIX_SIZE			64		/*except DSS public and (blocks * 12) */
#define PDU_FIX_NNC_OFFSET      56      /*except DH public */
#define PDU_CHAL_OFFSET         36
#define PDU_FIX_NNC_REV_OFFSET  70      /*except DH public */
#define PDU_PS_OFFSET           1
#define PDU_OP_OFFSET           2     
#define PDU_STAT_OFFSET         3
#define PDU_SERIAL_NUM_OFFSET   4
#define PDU_NW_ADDR_OFFSET		8
#define PDU_INIT_CONNID_OFFSET 	24
#define PDU_RESP_CONNID_OFFSET	28
#define PDU_INIT_PCONN_OFFSET   32
#define PDU_RESP_PCONN_OFFSET   34
#define PDU_CFM_RP_CID_OFFSET	4
#define PDU_CFM_RPC_OFFSET      10
#define PDU_CFM_CHAL_OFFSET     12
#define PDU_CFM_SIG_OFFSET		32

#define MAX_RETX_HEADER         208  /*L2=23 bytes; L3=32 bytes; */
									 /*L4=8 bytes; L5=144 bytes */
#define DEFAULT_RETRANSMISSIONS   2    /*only retransmit once */

/*TO DO: verify the following later */
#define MAX_PRTCLS              32
#define MAX_GRP_BLKS            3
#define NNC_GV_BLK_SIZE         12
#define MAX_NNC_SIZE            (160 + (NNC_GV_BLK_SIZE * MAX_GRP_BLKS))
#define MAX_GRPS                (MAX_GRP_BLKS << 5)
/*#define MAX_SDU_PE: max number of DEFINED protected entities. see ep_frm.h */
#define SPECIAL_NNCS			2   /*UPE NNC and AUTO NNC */
#define MAX_NNC_BUF_SIZE        ((MAX_SDU_PE + SPECIAL_NNCS) * MAX_NNC_SIZE)

#define SNMP_ROW_ACTIVE			1L
#define SNMP_ROW_CREATE_GO		4L
#define SNMP_ROW_DESTROYED		6L

#define SYNDROME_HMA_OFFSET		20	/*ask Jim about this magic number */

/* defines used to indicate row status. */
#define NOTUSED			0
#define ACTIVE				1
#define NOTINSERVICE		2
#define NOTREADY			3
#define CREATEANDGO		4
#define CREATEANDWAIT	5
#define DESTROY			6

#define GET_PDU_USHORT(val, ptr)    \
{                                   \
    val = GETSHORT(ptr); \
}






