/* $Id: latslt.h,v 3.2 1995/11/17 17:34:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/latslt.h,v $
 *------------------------------------------------------------------
 * $Log: latslt.h,v $
 * Revision 3.2  1995/11/17  17:34:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:27:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:26:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

 /*-----------------------------------------------------------------------
    latslt		lat protocol slot definitions
    
    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO
   
    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved. 
 -----------------------------------------------------------------------*/
#ifndef LATSLT_H
#define LATSLT_H    0			/* Indicate file has been read	*/

/*-----------------------------------------------------------------------
    Declare the LAT slot types and flags.
 -----------------------------------------------------------------------*/
#define SLOT_DST	0x00ff		/* Destination slot ID		*/
#define SLOT_SRC	0xff00		/* Source slot ID		*/

#define SLOT_CREDITS	(~0xf0)		/* Credit field mask            */
#define SLOT_TYPE	(~0x0f)		/* Type   field mask            */

#define SLOT_DATA_A	0x00		/* Data-A slot                  */
#define SLOT_START	0x90		/* Start  slot                  */
#define SLOT_DATA_B	0xa0		/* Data-B slot                  */
#define SLOT_ATTENTION	0xb0		/* Attention slot               */
#define SLOT_REJECT	0xc0		/* Reject slot                  */
#define SLOT_STOP	0xd0		/* Stop   slot                  */

/*-----------------------------------------------------------------------
    Define the LAT slot structure (fixed part).
 -----------------------------------------------------------------------*/
    typedef struct
    {
	byte	slot_dst;		/* Slot destination id		*/
	byte	slot_src;		/* Slot source id		*/
	byte	slot_len;		/* Slot length (variable)       */
	byte	slot_code;		/* Slot type, credits           */
    } slot_hdr, IF_VIEW(slot_iptr), OF_VIEW(slot_optr);

/*-----------------------------------------------------------------------
    Define the START SLOT structure, optional parameters.
 -----------------------------------------------------------------------*/
    typedef struct
    {
	byte	sv_class;		/* Service class                */
	byte	max_attn;		/* Maximum attention slot       */
	byte	max_data;		/* Maximum data-a/b  slot       */
    } starts_slot, IF_VIEW(starts_iptr), OF_VIEW(starts_optr);

#define SLOT_P_FLAGS	1		/* Optional flags		*/
#define SLOT_PF_REMOTE	0x01		/* Remote (modem) line		*/
#define SLOT_PF_AUTO	0x02		/* Disable autologin		*/

#define SLOT_P_HIC	2		/* HIC request number		*/
#define SLOT_P_DST_PORT	4		/* Destination port name	*/
#define SLOT_P_SRC_PORT	5		/* Terminal server port name	*/
#define SLOT_P_GROUPS	6		/* Group codes enabled		*/
#define SLOT_P_PASSWORD	7		/* Service password		*/
#define SLOT_P_XSERVER	8		/* X server number (SC 3)	*/

#define SLOT_START_LEN	(SV_STR + SV_STR + OB_STR + OB_STR + GROUPS + 20)

/*-----------------------------------------------------------------------
    Define the DATA-B slot structure, control masks, and optional parameters.
 -----------------------------------------------------------------------*/
    typedef struct
    {
	byte	control;		/* Control flags                */
	byte	out_off;		/* Output off flow control      */
	byte	out_on;			/* Output on  flow control      */
	byte	inp_off;		/* Input  off flow control      */
	byte	inp_on;			/* Input  on  flow control      */
    } datab_slot, IF_VIEW(datab_iptr), OF_VIEW(datab_optr);

#define SLOT_FLOW	0x1f		/* The slot flow control fields	*/

#define SLOT_INP_ON	0x01		/* Enable  input  flow control  */
#define SLOT_INP_OFF	0x02		/* Disable intput flow control  */
#define SLOT_OUT_ON	0x04		/* Enable  output flow control  */
#define SLOT_OUT_OFF	0x08		/* Disable output flow control  */
#define SLOT_BREAK	0x10		/* Break character seen         */

#define SLOT_SET	0x20		/* Parameters passed to set	*/
#define SLOT_INFO	0x40		/* Parameters passed as info	*/

#define	SLOT_P_MODE	5		/* Data transparency mode	*/

#define SLOT_P_CHAR	1		/* Character size and parity	*/
#define SLOT_PC_SIZE	0x0f		/* Character size   mask 	*/
#define SLOT_PC_PARITY	0xf0		/* Character parity mask	*/
#define SLOT_PC_NONE	0x00		/* Parity, none			*/
#define SLOT_PC_SPACE	0x20		/* Parity, space		*/
#define SLOT_PC_ODD	0x30		/* Parity, odd			*/
#define SLOT_PC_EVEN	0x50		/* Parity, even			*/
#define SLOT_PC_MARK	0x70		/* Parity, mark			*/

#define SLOT_P_XMIT	2		/* Transmit speed (TT->server)	*/
#define SLOT_P_RECV	3		/* Receive  speed (server->TT)	*/

#define SLOT_P_PREF	4		/* User preferences		*/
#define SLOT_PP_BELL	1		/* Bell if character discarded	*/

#define SLOT_P_STATUS	6		/* Character status		*/
#define SLOT_PS_UNKNOWN	0		/* Unknown error		*/
#define SLOT_PS_SHORT	1		/* Short break detected		*/
#define SLOT_PS_LONG	2		/* Long  break detected		*/
#define SLOT_PS_FRAME	3		/* Framing error		*/
#define SLOT_PS_OVERRUN	4		/* Data overrun			*/
#define SLOT_PS_PARITY	5		/* Parity error			*/

#define SLOT_DATAB_LEN	31		/* Maximum DATA-B slot length	*/

/*-----------------------------------------------------------------------
    Define the LAT ATTENTION control flags.
 -----------------------------------------------------------------------*/
    typedef struct
    {
	byte	control;		/* Control flags                */
    } attention_slot, IF_VIEW(attention_iptr), OF_VIEW(attention_optr);

#define SLOT_FLUSH	0x20		/* Flush terminal output        */
#define SLOT_FORCE_XON	0x40		/* Force XON			*/

/*-----------------------------------------------------------------------
    The REJECT and STOP slots have no other fields. The credit part
    of the 'slot_code' is used for the stop reason value. These codes
    are also used for HIC status messages.
 -----------------------------------------------------------------------*/
#define SLOT_STOP_NONE		0	/* No reason given		*/
#define SLOT_STOP_USER	        1       /* User requested disconnect    */
#define SLOT_STOP_SYSTEM    	2       /* System shutting down         */
#define SLOT_STOP_BAD_SLOT	3       /* Bad slot number received     */
#define SLOT_STOP_BAD_CLASS	4       /* Bad service class            */
#define SLOT_STOP_NO_RESOURCE	5       /* No resources available       */
#define SLOT_STOP_SRV_IN_USE	6	/* Service in use		*/
#define SLOT_STOP_SRV_UNKNOWN	7	/* Service is unknown		*/
#define SLOT_STOP_SRV_DISABLED	8	/* Service is disabled		*/
#define SLOT_STOP_SRV_NOT_OFFER	9	/* Service is not offered	*/
#define SLOT_STOP_PORT_UNKNOWN	10	/* Port is unknown		*/
#define SLOT_STOP_PASSWORD	11	/* Bad password supplied	*/
#define SLOT_STOP_NOT_QUEUED	12	/* Request not queued		*/
#define SLOT_STOP_IMMED_REJECT	13	/* Request cannot start now	*/
#define SLOT_STOP_ACCESS_DENIED	14	/* Accessed denied to service	*/
#define SLOT_STOP_BAD_SOLCIT	15	/* Bad solicit message		*/

#define SLOT_STOP_BAD_COMMAND	16	/* Bad command message		*/
#define SLOT_STOP_CANNOT_START	17	/* Canot start request		*/
#define SLOT_STOP_ENTRY_DELETED	18	/* Queued entry deleted		*/
#define SLOT_STOP_BAD_PARMS	19	/* Bad request parameters	*/
#endif
