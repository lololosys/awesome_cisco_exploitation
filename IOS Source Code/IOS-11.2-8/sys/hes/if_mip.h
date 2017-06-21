/* $Id: if_mip.h,v 3.4.18.2 1996/08/27 07:11:11 mwu Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_mip.h,v $
 *------------------------------------------------------------------
 * if_mip.h -- Support for the MIP card
 *
 * 29-June-1993, Ronnie B. Kon
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_mip.h,v $
 * Revision 3.4.18.2  1996/08/27  07:11:11  mwu
 * CSCdi67143:  MIP interfaces dont come up on 7000 platform
 * Branch: California_branch
 *
 * Revision 3.4.18.1  1996/07/08  22:25:39  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/08  22:22:01  wmay
 * CSCdi48458:  mip uses old scheduler - use watched queues for love
 * note handler
 *
 * Revision 3.3  1995/11/17  09:18:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:28:31  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:40:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:42:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_MIP_H__
#define __IF_MIP_H__

#define VISIBLE_T1_BANDWIDTH	1536
#define VISIBLE_E1_BANDWIDTH	1984

#define	MIP_MAX_DAUGHTERS	2	/* max number of daughter cards */

#define	MIP_ENCAPSIZE_WORDS	1    /* additional encaps space used by MIP */
#define MIP_ENCAPSIZE_BYTES	(MIP_ENCAPSIZE_WORDS * 2)
#define MIP_RXOFFSET_WORDS      (MCI_SERIAL_OFFSET - MIP_ENCAPSIZE_WORDS)
#define	MIP_MAX_VC	31	/* max number for a VC */
#define	MIP_NUM_TS	32	/* number of timeslots on the MIP */

#define MAX_MIP_TX_DELAY 0x800

/*
 * The below are format of the messages from the MIP
 */
typedef struct LoveLtrHdrType_ {
    ushort   Command;
    ushort   Dummy;		/* space for type */
    ushort   VCnumber;
    uchar    Data[0];
} LoveLtrHdrType;

#define SNMP_LOVE_CMD_CURR_ONLY 0
#define SNMP_LOVE_CMD_CURR_PREV 1
#define APPL_REMOTE_BAD_RESP    4
#define APPL_REMOTE_GOOD_RESP   5
#define VC_ERRORS_LOVE_NOTE     6
#define VC_ERRORS_LOVE_NOTE_W_EXTRA 7
#define PRINTF_LOVE_LETTER	8
#define BAD_VC_PACKET_LOVE_LETTER 9
#define VC_REMOTE_LOOPBACK_RESP 10
#define VC_REMOTE_LOVE_DUMP	11

typedef struct MIP_VCErrorsType_ {
    ulong   VCnumber;
    ulong   RxPackets;
    ulong   RxBytes;
    ulong   RxGiants;
    ulong   RxCrcErrs;
    ulong   RxAborts;
    ulong   RxNoCbusBuffers;
    ulong   RxOverrun;
    ulong   TxPackets;
    ulong   TxBytes;
    ulong   TxNoBuffers;
} MIP_VCErrorsType;

/*
 * Caution: The following structure is actually "passed over the cbus",
 * so it must be defined in such a way that it will be padded identically
 * on the RP1's 68K, the MIP's 68K, and the RSP's Orion.
 * (see also dsx1.h)
 */
typedef struct MIBDataFromMIPType_ {
    SmGauge	dsx1TimeElapsed;
    ushort	dsx1LineStatus;
    uchar	dsx1LineType;
    uchar	dsx1LoopbackConfig;
    ushort      dummy;               /* Ensure long boundary for the rest */
    dsx1TableType	CurrentData;
    dsx1TableType	PrevData;
} MIBDataFromMIPType;


/*
 * Define all the information we need for a MIP interface.  There
 * is one of these structures per hwidb.
 */
typedef struct mipinfo_ {
    hwidbtype		*hwidb[MIP_MAX_VC + 1];	/* the vc's hwidb */
    struct cdbtype_	*cdb;
    int			slot, appl_no;		/* derived from unit number */
    int			unit;
    int			interfacetype;
    enum appl_type 	applique_type;
    sys_timestamp	lastfault;

    void		(*carrier)(struct mipinfo_ *);

#if 0
    struct encaps_info_ {
	vencap_t	vencap;
	bridge_vencap_t	bridge_vencap;
    } encaps_info[MIP_MAX_VC + 1];
#endif

    /* below are the packet counts for each mip at the time of the
     * last love letter.  This helps us with autonomous counts
     */
    struct mip_total_counts_ {
	ulong in_paks;
	ulong in_bytes;
	ulong out_paks;
	ulong out_bytes;
    } packet_counts[MIP_MAX_VC + 1];

    /* no of timeslots configured for a channel group */
    uchar vc_ts_no[MIP_MAX_VC + 1];
    
} mipinfo;

extern mipinfo *mip_find_by_idb(hwidbtype*);
extern struct cdbtype_ *mip_create_context(platform_hware_handle_type *, 
					   int, int, boolean, int);
extern void mip_love_letter_handler(mcitype *, ushort, int);

extern void mip_preallocate(boolean);
extern boolean platform_mip_carve_in_progress(cdbtype *);
extern void mip_platform_init(subsystype *);
extern boolean platform_mip_dynamic_idb_init(cdbtype *, mipinfo *,
					     hwidbtype *);
extern void platform_chain_hwidb_in(cdbtype *, hwidbtype *);
extern void platform_unchain_hwidb(cdbtype *, hwidbtype *);
extern boolean mip_platform_idb_create(hwidbtype *);
extern void mip_platform_idb_delete(hwidbtype *);
extern void mip_platform_map_channel(mipinfo *, hwidbtype *,
				     ulong);
extern void mip_platform_if_reset(mipinfo *, hwidbtype *);
extern int mip_platform_get_txqlength(hwidbtype *);
extern boolean mip_platform_configured_check(void);
extern void free_mip(mipinfo *);
extern inline ushort mip_select_vc(mipinfo *, int);
extern void mip_its_dead_jim(mipinfo *, ushort);

extern void platform_mip_adjust_mtu_for_love(mipinfo *mip,
					     ushort *maxmtu,
					     ulong *buffersize,
					     ulong *maxdgram);
extern ushort mip_select_if(mipinfo *);
extern ushort mip_controller_status(mipinfo *, ushort);
extern ushort mip_simple_command(mipinfo *, int);
extern ushort mip_simple_command_result(mipinfo *, int, ushort *);
extern ushort mip_command_arg(mipinfo *, int, ushort);
extern ushort mip_command_larg(mipinfo *, int, ulong);
extern void mip_throttle_love_letters(boolean);
extern void platform_mip_restart_init(mipinfo *);
extern void platform_mip_adjust_encsize(hwidbtype *);
extern void platform_mip_set_offsets(hwidbtype *);
extern boolean platform_mip_reset_in_progress(cdbtype *);
extern void platform_mip_request_bus_reset(cdbtype *);
extern void platform_mip_update_carve_dbase(cdbtype *);

extern mipinfo *mip_list[MAX_CBUS_INTERFACES];
extern watched_queue *mip_loveletterQ;
extern queuetype mip_preQ;	/* preallocated mip entries for hot-swap */

#ifndef RSP                 /* XXX Can this be split out somewhere? */

/*
 * mip_icb_to_idb
 * We have to sniff the first two bytes of encapsulation information
 * to learn the VC that this came in on.
 *
 * Assumes that interrupts are disabled.
 *
 * Used in fast path, so inline it
 */
static inline struct hwidbtype_ *mip_icb_to_idb (mcitype *cbus, int unit)
{
    short vc;

    cbus->regptr->argreg = MCI_SERIAL_OFFSET - MIP_ENCAPSIZE_WORDS;
    cbus->regptr->cmdreg = MCI_CMD_RX_SELECT;
    vc = cbus->regptr->readshort;
    
    /* check for love letters */
    if(vc == -1)
	return((void *)-1);

    vc &= 0xff;	 /* add off the top byte - encaps info for corona */
    /* Hey--let's be careful out there */
    if(vc < 0 || vc > MIP_MAX_VC) {
	buginf("\n%%MIP unit %d reports illegal VC %d", unit, vc);
	return(NULL);
    }
    
    return(mip_list[unit]->hwidb[vc]);
}
#endif  /* RSP */

#endif /* __IF_MIP_H__ */
