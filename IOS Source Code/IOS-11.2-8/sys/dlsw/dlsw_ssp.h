/* $Id: dlsw_ssp.h,v 3.4.10.1 1996/08/21 22:19:42 akhanna Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_ssp.h,v $
 *------------------------------------------------------------------
 * All the Switch to Switch proitocol definitions
 * DLSw subsystem 
 *
 * Fri July 29, 1994 G. Pandian
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Includes all the switch to switch protocols and some macros to 
 * classify SSP frames.
 *------------------------------------------------------------------
 * $Log: dlsw_ssp.h,v $
 * Revision 3.4.10.1  1996/08/21  22:19:42  akhanna
 * CSCdi48010:  dlsw flow control allow max/min flow control window sizes
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/26  19:54:55  fbordona
 * CSCdi49900:  DLSw should increment circuit flow control window faster
 *
 * Revision 3.3  1995/11/21  22:33:13  kpatel
 * CSCdi42929:  DLSw : Second call fails
 *
 * Revision 3.2  1995/11/17  09:04:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:25:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:56:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/07  04:37:57  kpatel
 * CSCdi39871:  DLSw+ does not interoperate with remote window size
 *              of one
 *
 * Revision 2.2  1995/08/24  03:27:35  kpatel
 * CSCdi39082:  DLSw flow control out of sync causing session lock-up
 *
 * Revision 2.1  1995/06/07  20:27:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SSP_H__
#define __SSP_H__



typedef struct _ssp_ckt_id {
    uint		dlc_corr;
    uint 		dlc_port_id;
} ssp_ckt_id_t;

typedef struct _ssp_ckt_transport_id {
    uint 		dlc_port_id;
    uint		dlc_corr;
    uint	 	transport_id;
} ssp_transport_id_t;

typedef struct _ssp_i_frame {
    uchar 		version;
    uchar 		header_len;
    ushort  		message_len;
    ssp_ckt_id_t 	rem_ckt_id;
    ushort  		res1;
    uchar 		msg_type1;
    uchar    		flow_control;
} ssp_i_frame_t;

typedef struct _ssp_dlc_id {
    uchar 		target_mac_address[IEEEBYTES];
    uchar 		origin_mac_address[IEEEBYTES];
    uchar 		origin_link_sap;
    uchar 		target_link_sap;
} ssp_dlc_id_t;

typedef	struct { 
    uchar 		cmd_rsp;
    uchar 		poll_final;
}ssp_llc_flags_t;

typedef struct _ssp_control_frame {
    ssp_i_frame_t  	info_hdr;
    uchar 		protocol_id;
    uchar 		header_number;
    /* ushort  		res2; */

    /* Extention */
    ssp_llc_flags_t   	ssp_llc_flags_u;
    uchar       	largest_frame_bits;
    uchar		csex_flags;
    uchar 		ckt_priority;
    uchar 		msg_type2;
    ssp_dlc_id_t 	dlc_id;
    uchar 		frame_direction;
    uchar 		res5;
    ushort  		res6;
    ushort  		dlc_header_len;
    ssp_transport_id_t	origin_id;
    ssp_transport_id_t	target_id;
    uint		res7;
} ssp_control_frame_t;

STRUCTURE_REF(ssp_hdr_t);

struct ssp_hdr_t_ {
    union {
	ssp_control_frame_t ssp_ctl;
	ssp_i_frame_t       ssp_info;
    }ssp_u;
} ;



typedef struct _ssp_data_msg_t {
    ssp_i_frame_t	ssp;
    uchar	data[0];
} ssp_info_t;

typedef struct _ssp_ctl_msg_t {
    ssp_control_frame_t	ssp;
    uchar	data[0];
} ssp_ctl_t;

/*
 * Used to peak ahead into a SSP packet received from TCP.
 */

typedef struct _ssp_hdr_start_t {
    uchar               version;
    uchar               header_len;
    ushort              message_len;
} ssp_hdr_start_t;


#define ssp_vers		ssp_u.ssp_info.version
#define ssp_hdr_len		ssp_u.ssp_info.header_len
#define ssp_msg_len		ssp_u.ssp_info.message_len

#define rem_dlc_corr		ssp_u.ssp_info.rem_ckt_id.dlc_corr
#define rem_dlc_port_id		ssp_u.ssp_info.rem_ckt_id.dlc_port_id

#define ssp_msg_type		ssp_u.ssp_info.msg_type1

#define ssp_fc			ssp_u.ssp_info.flow_control

#define ssp_prot_id		ssp_u.ssp_ctl.protocol_id

#define ssp_hdr_no		ssp_u.ssp_ctl.header_number

#define ssp_flags		ssp_u.ssp_ctl.csex_flags

#define ssp_dlc_id		ssp_u.ssp_ctl.dlc_id

#define target_mac		ssp_u.ssp_ctl.dlc_id.target_mac_address
#define origin_mac		ssp_u.ssp_ctl.dlc_id.origin_mac_address
#define target_sap		ssp_u.ssp_ctl.dlc_id.target_link_sap
#define origin_sap		ssp_u.ssp_ctl.dlc_id.origin_link_sap



#define ssp_dirn		ssp_u.ssp_ctl.frame_direction

#define ssp_dlc_hdr_len		ssp_u.ssp_ctl.dlc_header_len

#define ssp_lf			ssp_u.ssp_ctl.largest_frame_bits
#define ssp_priority		ssp_u.ssp_ctl.ckt_priority

#define origin_dlc_port_id	ssp_u.ssp_ctl.origin_id.dlc_port_id
#define origin_dlc_corr		ssp_u.ssp_ctl.origin_id.dlc_corr
#define origin_transport_id	ssp_u.ssp_ctl.origin_id.transport_id

#define target_dlc_port_id	ssp_u.ssp_ctl.target_id.dlc_port_id
#define target_dlc_corr		ssp_u.ssp_ctl.target_id.dlc_corr
#define target_transport_id	ssp_u.ssp_ctl.target_id.transport_id


#define ssp_llc_cr		ssp_u.ssp_ctl.ssp_llc_flags_u.cmd_rsp
#define ssp_llc_pf		ssp_u.ssp_ctl.ssp_llc_flags_u.poll_final

#define ssp_llc_flags		ssp_u.ssp_ctl.ssp_llc_flags_u

#define SSP_CNTL_HDR_LEN 	0x48
#define SSP_INFO_HDR_LEN 	0x10
#define SSP_VERS  		0x31  /* ASCII "1" */
#define SSP_OLD_VERS  		0x4B  /* 6611 */
#define SSP_HDR_NUM		0x01

#define SSP_DIRN_FORWARD 	0x01
#define SSP_DIRN_BACKWARD 	0x02

#define SSP_PROTOCOL_ID 	0x42
#define SSP_NB_DLC_HDR_LEN	0x23
#define SSP_SNA_DLC_HDR_LEN	0x00
#define SSP_PT_DLC_HDR_LEN	0x23  /* + 8 ??? max for FRMR */  

#define SSP_FLAGS_EX		0x80

/*
 * 7  6  5  4  3  2  1  0
 * c  				Origin DLSw don't care flag 0 - fail ckt
 *    r							    1 - don't fail; 
 *       b			LF base
 *          b
 *             b
 *                e		LF extented
 *                   e
 *                      e
 */
#define SSP_LF_IGNORE_MASK      0x80
#define SSP_LF_BASE_MASK        0x38
#define SSP_LF_EXT_MASK         0x07
#define SSP_LF_IGNORE_BIT       0x80

#define SSP_LF516               0x00
#define SSP_LF1500              0x08
#define SSP_LF2052              0x10
#define SSP_LF4472              0x18
#define SSP_LF8144              0x20
#define SSP_LF11407             0x28
#define SSP_LF17800             0x30
#define SSP_LFMAX               0x38 /* used in all-routes bcast frames */


/* from the flow control rig let

             bit   7   6   5   4   3   2   1   0
                 +---+---+---+---+---+---+---+---+
                 |FCI FCA| reserved  |    FCO    |
                 +---+---+---+---+---+---+---+---+
          
             FCI : Flow Control Indicator
             FCA : Flow Control Ack
             FCO : Flow Control Operator Bits
          
                   000 - Repeat Window Operator
                   001 - Increment Window Operator
                   010 - Decrement Window Operator
                   011 - Reset Window Operator
                   100 - Halve Window Operator
                   101 - Reserved
                   110 - Reserved
                   111 - Reserved

*/




#define SSP_FLOW_CONTROL_FCI    0x80  /* Flow control indicator */
#define SSP_FLOW_CONTROL_FCA    0x40  /* Flow control ack  */

#define SSP_FLOW_CONTROL_MASK   0x07

#define SSP_FLOW_CONTROL_RWO    0x00  /* Repeat Window Operator */
#define SSP_FLOW_CONTROL_IWO    0x01  /* Increment Window Operator */
#define SSP_FLOW_CONTROL_DWO    0x02  /* Decrement Window Operator */
#define SSP_FLOW_CONTROL_ZWO    0x03  /* Reset/Zero Window Operator */
#define SSP_FLOW_CONTROL_HWO    0x04  /* Half Window Operator */

/* 
 * Circuit Priority field (offset 22) support in SSP Control Frames
 * First 5 bits reserved; last 3:
 * 0 - Unsupported
 * 1 - Low
 * 2 - Medium
 * 3 - High
 * 4 - Highest
 * 5 thru 7 - reserved for future use
 */

typedef enum {
 SSP_PRIORITY_UNSUPPORTED = 0,
} ssp_priority_t;

#define DLSW_CKT_WINDOW_LOW_THRESHOLD(x) ((x->fc_recv_curr_window*4)/5)

#define is_ssp_fc_ind(fc)       ((fc) & SSP_FLOW_CONTROL_FCI)
#define is_ssp_fc_ack(fc)       ((fc) & SSP_FLOW_CONTROL_FCA)
#define is_ssp_fc_none(fc)	( !(is_ssp_fc_ind(fc) || is_ssp_fc_ack(fc)) )

#define get_ssp_fc_oper(fc)     ((fc) & SSP_FLOW_CONTROL_MASK ) 

/*
 * 7  6  5  4  3  2  1  0
 * r  				
 *    r				
 *       r			
 *          r
 *             r
 *                r		
 *                   p            ckt priority bits
 *                      p
 */
#define SSP_PRIORITY_MASK         0x03

#define SSP_PRIORITY_UNSPECIFIED  0x00
#define SSP_PRIORITY_LOW          0x01
#define SSP_PRIORITY_MEDIUM       0x02
#define SSP_PRIORITY_HIGH         0x03
#define SSP_PRIORITY_HIGHEST      0x04
/* 
 * matches SNA FID4 TH TPF & NTWK_PRTY fields 
 */

typedef enum {
 SSP_OP_NONE		= 0x00,  /* NONE - Nothing 			   */
 SSP_OP_canureach	= 0x03,  /* CUR  - CAN U Reach Station ex/cs       */
 SSP_OP_icanreach	= 0x04,  /* ICR  - I Can Reach Station ex/cs       */
 SSP_OP_reach_ack	= 0x05,  /* ACK  - Reach Acknowledgment            */
 SSP_OP_dgmframe	= 0x06,  /* DGM  - Datagram Frame (See note)       */
 SSP_OP_xidframe	= 0x07,  /* XID  - XID Frame                       */
 SSP_OP_contact		= 0x08,  /* CONQ - contact Remote Station          */
 SSP_OP_contacted	= 0x09,  /* CONR - Remote Station Contacted        */
 SSP_OP_restart_dl	= 0x10,  /* RSTQ - Restart Data Link               */
 SSP_OP_dl_restarted	= 0x11,  /* RSTR - Data Link Restarted             */
 SSP_OP_enter_busy	= 0x0C,  /* enter busy - retired;                  */
 SSP_OP_exit_busy	= 0x0D,  /* enter busy - retired;                  */
 SSP_OP_infoframe	= 0x0A,  /* INFO - Information (I) Frame           */
 SSP_OP_halt_dl		= 0x0E,  /* HLTQ - Halt Data Link                  */
 SSP_OP_dl_halted	= 0x0F,  /* HLTR - Data Link Halted                */
 SSP_OP_netbios_nq	= 0x12,  /* NBNQ - NetBIOS Name Query - ex/cs      */
 SSP_OP_netbios_nr	= 0x13,  /* NBNR - NetBIOS Name Recognized - ex/cs */
 SSP_OP_dataframe	= 0x14,  /* DATA - Data Frame           	   */
 SSP_OP_halt_dl_noack	= 0x19,  /* HLTN - Halt Data Link with no Ack      */
 SSP_OP_netbios_anq	= 0x1A,  /* NBAQ - NetBIOS Add Name Query          */
 SSP_OP_netbios_anr	= 0x1B,  /* NBAR - NetBIOS Add Name Response       */
 SSP_OP_keepalive       = 0x1D,  /* TKEP - Transport keepalive             */
 SSP_OP_capability_xchg = 0x20,  /* CAPX - capability exchange             */
 SSP_OP_ifcm		= 0x21,  /* IFCM - Independent pacing messae       */
 SSP_OP_test_ckt_req	= 0x7A,  /* TSTQ - Test Circuit Request            */
 SSP_OP_test_ckt_rsp 	= 0x7B,  /* TSTR - Test Circuit Response           */


} ssp_op_t;

#define SSP_FC_IND_MASK		0x01
#define SSP_FC_ACK_MASK		0x02
#define SSP_FC_OPER_MASK	0x0C

#define SSP_FC_OPER_SHIFT	2

#define SSP_FC_OPER(x)		(( (x) & SSP_FC_OPER_MASK ) >> SSP_FC_OPER_SHIFT )

#define SSP_FC_REP_WINDOW	0x00
#define SSP_FC_INC_WINDOW	0x01
#define SSP_FC_DEC_WINDOW	0x02
#define SSP_FC_RESET_WINDOW	0x03

#define SSP_CAP_EX_ID       	0x1520
#define SSP_CAP_POS_RSP_ID  	0x1521
#define SSP_CAP_NEG_RSP_ID  	0x1522

/*
 * Warning : CapXchg is not included here!
 */

#define is_ssp_ctl_hdr(ssp_op) 		          \
	((ssp_op) == SSP_OP_canureach		||\
	 (ssp_op) == SSP_OP_icanreach		||\
	 (ssp_op) == SSP_OP_reach_ack	 	||\
	 (ssp_op) == SSP_OP_dgmframe	 	||\
	 (ssp_op) == SSP_OP_halt_dl		||\
	 (ssp_op) == SSP_OP_dl_halted	 	||\
	 (ssp_op) == SSP_OP_contact		||\
	 (ssp_op) == SSP_OP_contacted	 	||\
	 (ssp_op) == SSP_OP_dl_restarted	||\
	 (ssp_op) == SSP_OP_restart_dl	 	||\
	 (ssp_op) == SSP_OP_halt_dl_noack	||\
	 (ssp_op) == SSP_OP_xidframe	 	||\
	 (ssp_op) == SSP_OP_test_ckt_req        ||\
	 (ssp_op) == SSP_OP_test_ckt_rsp        ||\
	 is_ssp_ctl_mac_hdr(ssp_op)          	  )

#define is_ssp_info_hdr(ssp_op)             	  \
	((ssp_op) == SSP_OP_infoframe	 	 ||\
	 (ssp_op) == SSP_OP_ifcm	 	 )

#define is_ssp_fc_ctr_op(ssp_op)             	  \
	((ssp_op) == SSP_OP_infoframe	 	 ||\
	 (ssp_op) == SSP_OP_dgmframe	 	 )

#define is_ssp_ifcm(ssp_op)             	  \
	((ssp_op) == SSP_OP_ifcm	 	 )

#define is_ssp_ctl_mac_hdr(ssp_op)          	  \
       ((ssp_op) == SSP_OP_netbios_nq	 	||\
	(ssp_op) == SSP_OP_netbios_nr	 	||\
	(ssp_op) == SSP_OP_dataframe	 	||\
	(ssp_op) == SSP_OP_netbios_anq	 	||\
	(ssp_op) == SSP_OP_netbios_anr	         )
								  
#define is_ssp_lf_valid(ssp_op)             	  \
       ((ssp_op) == SSP_OP_canureach	 	||\
	(ssp_op) == SSP_OP_icanreach	 	||\
        (ssp_op) == SSP_OP_netbios_nq	 	||\
	(ssp_op) == SSP_OP_netbios_nr	 	 )
								  
#define is_ssp_need_data(ssp_op)                  \
       ((ssp_op) == SSP_OP_infoframe  		||\
	(ssp_op) == SSP_OP_dgmframe	 	||\
	(ssp_op) == SSP_OP_xidframe	 	||\
	(ssp_op) == SSP_OP_canureach		||\
	(ssp_op) == SSP_OP_icanreach		||\
	(ssp_op) == SSP_OP_reach_ack	 	||\
	is_ssp_ctl_mac_hdr(ssp_op)		 )

#define is_ssp_filter_priority(ssp_op)            \
       ((ssp_op) == SSP_OP_infoframe  		||\
	(ssp_op) == SSP_OP_dgmframe	 	||\
	is_ssp_ctl_mac_hdr(ssp_op)		 )


#define is_ssp_csm_op(ssp_op)                     \
       ((ssp_op) == SSP_OP_canureach		||\
	(ssp_op) == SSP_OP_icanreach		||\
	(ssp_op) == SSP_OP_netbios_nq		||\
	(ssp_op) == SSP_OP_netbios_nr		||\
	(ssp_op) == SSP_OP_dataframe	 	||\
	(ssp_op) == SSP_OP_netbios_anq	 	||\
	(ssp_op) == SSP_OP_netbios_anr	         )

#define is_ssp_valid_op(ssp_op)    		  \
       (is_ssp_ctl_hdr(ssp_op) 		 	||\
        is_ssp_ctl_mac_hdr(ssp_op)          	||\
	is_ssp_info_hdr(ssp_op)             	  )

#define is_ssp_fc_op(ssp_op) 		          \
	((ssp_op) == SSP_OP_icanreach		||\
	 (ssp_op) == SSP_OP_reach_ack	 	||\
	 (ssp_op) == SSP_OP_xidframe	 	||\
	 (ssp_op) == SSP_OP_dgmframe	 	||\
	 (ssp_op) == SSP_OP_contact		||\
	 (ssp_op) == SSP_OP_contacted	 	||\
	 (ssp_op) == SSP_OP_contact		||\
	 (ssp_op) == SSP_OP_contacted		||\
	 (ssp_op) == SSP_OP_restart_dl		||\
	 (ssp_op) == SSP_OP_dl_restarted	||\
	 is_ssp_info_hdr(ssp_op)		)

#define is_ssp_priority_op(ssp_op) 		  \
	((ssp_op) == SSP_OP_canureach		||\
	 (ssp_op) == SSP_OP_icanreach		||\
	 (ssp_op) == SSP_OP_reach_ack	 	)

#define is_ssp_rem_id_op(ssp_op) 		  \
	((ssp_op) == SSP_OP_canureach		||\
	 (ssp_op) == SSP_OP_icanreach		||\
	 (ssp_op) == SSP_OP_reach_ack	 	)

extern boolean dlsw_is_cstm_ssp_op(ssp_hdr_t *ssp_hdr);

static inline boolean ssp_sanity_check_inline (ssp_hdr_t *ssp_hdr)
{
    if (is_ssp_info_hdr(ssp_hdr->ssp_msg_type)) {
        if (ssp_hdr->ssp_hdr_len == sizeof(ssp_i_frame_t))
            return(TRUE);
        else
            return(FALSE);
    } else if (is_ssp_ctl_hdr(ssp_hdr->ssp_msg_type) || 
               is_ssp_ctl_mac_hdr(ssp_hdr->ssp_msg_type)) {
        if (ssp_hdr->ssp_hdr_len == sizeof(ssp_control_frame_t))
            return(TRUE);
        else
            return(FALSE);
    } else {
        return(FALSE);
    }
}

#endif  __SSP_H__











