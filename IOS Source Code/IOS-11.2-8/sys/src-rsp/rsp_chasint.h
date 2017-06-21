/* $Id: rsp_chasint.h,v 3.3 1996/01/15 18:05:09 schaefer Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/src-rsp/rsp_chasint.h,v $
 *------------------------------------------------------------------
 * External interface into Chassis Interface code
 *
 * October 1994, Carl Schaefer
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_chasint.h,v $
 * Revision 3.3  1996/01/15  18:05:09  schaefer
 * CSCdi45761: support RSP in new chassis
 *
 * Revision 3.2  1995/11/17  18:47:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:28:05  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/10  19:12:02  schaefer
 * CSCdi38464: need branch patch in 11.0
 * don't crash if two RSPs are installed
 *
 * Revision 2.1  1995/06/07  22:57:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern int chasint_interrupt(void);
extern uchar compute_rsp_slot(void);
extern uchar adjust_rsp_slot(uchar slot);

/*
 * base RSP slot for different chassis types
 */
#define RSPSLOT_7506		6
#define RSPSLOT_7505		4
#define RSPSLOT_7507_PROC1	2
#define RSPSLOT_7507_PROC2	3
#define RSPSLOT_7513_PROC1	6	/* test chassis shares these defs */
#define RSPSLOT_7513_PROC2	7
#define RSPSLOT_7000		5
#define RSPSLOT_7010		3

#define RSP_NO_SLOT	((uchar) -1)

typedef struct {
    tinybool data[MAX_SLOTS];
} slot_mask_t;

extern slot_mask_t slot_present_mask;

extern boolean	ci_present(void);
extern boolean	ci_volt_valid(uint index);
extern char*	ci_volt_descr(uint index);
extern uint	ci_volt_value(uint index);
extern uint	ci_volt_lowthresh(uint index);
extern uint	ci_volt_highthresh(uint index);
extern uint	ci_volt_lastvalue(uint index);
extern uint	ci_volt_state(uint index);
extern boolean	ci_temp_valid(uint index);
extern char*	ci_temp_descr(uint index);
extern uint	ci_temp_value(uint index);
extern uint	ci_temp_thresh(uint index);
extern uint	ci_temp_lastvalue(uint index);
extern uint	ci_temp_state(uint index);
extern boolean	ci_fan_valid(uint index);
extern char*	ci_fan_descr(uint index);
extern uint	ci_fan_state(uint index);
extern boolean	ci_ps_valid(uint index);
extern char*	ci_ps_descr(uint index);
extern uint	ci_ps_state(uint index);
extern uint	ci_ps_source(uint index);
