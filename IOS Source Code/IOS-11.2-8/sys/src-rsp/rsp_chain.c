/* $Id: rsp_chain.c,v 3.5.10.10 1996/08/30 06:58:27 mcieslak Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_chain.c,v $
 *------------------------------------------------------------------
 * RSP specific commands
 *
 * October 1994, Carl Schaefer
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_chain.c,v $
 * Revision 3.5.10.10  1996/08/30  06:58:27  mcieslak
 * CSCdi67677:  Need to enhance RSP cache and watch test commands
 * Branch: California_branch
 *
 * Revision 3.5.10.9  1996/08/09  23:36:17  xliu
 * CSCdi63854:  Traffic is pass in clear text when distributed switching
 * is off
 * Branch: California_branch
 *
 * Revision 3.5.10.8  1996/08/07  18:01:46  kmitchel
 * CSCdi49854:  DBUSINTERR after RSP dbus write; ineffective recvry of
 * disabled card
 * Branch: California_branch
 *
 * Revision 3.5.10.7  1996/07/02  17:13:47  drowell
 * CSCdi61308:  Flow switching is a hidden command
 * Branch: California_branch
 * Since we are selling flow switching as a feature, we need to unhide the
 * command to turn it on.
 *
 * Revision 3.5.10.6  1996/06/19  19:24:11  drowell
 * CSCdi56600:  legacy IPs do distributed switching
 * Branch: California_branch
 * Parser change to disallow distributed switching configuration on non-VIP
 * interfaces.
 *
 * Revision 3.5.10.5  1996/06/17  23:37:04  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.5.10.4  1996/06/06  03:08:30  mbeesley
 * CSCdi59592:  Bootstrap should not emit warnings wrt no support for
 * flow/opt switching. Don't print warning is system_loading.
 * Branch: California_branch
 *
 * Revision 3.5.10.3  1996/05/21  10:04:30  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.10.2  1996/03/27  08:41:54  dbath
 * CSCdi51833:  test rsp stall bus errors on virtually removed slave.
 * Branch: California_branch
 * When "test stall" is physically disabling virtually
 * removed cards, skip slots that are empty, that contain RSPs,
 * or that contain already disabled cards.
 *
 * Revision 3.5.10.1  1996/03/18  22:01:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.2  1996/03/16  07:36:09  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.2.1  1996/03/09  05:12:55  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.5  1996/02/22  14:38:29  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/19  03:03:30  dlobete
 * CSCdi44954:  Local-switching for DFS required
 * But that's not all! Also part of this jumbo Distributed IP
 * Switching commit:
 * - CSCdi43302: Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * - DFS config which mirrors RSP route-cache setting
 *         (ie configurable distributed flow or optimal switching)
 * - Interrupt level IPC updates and invalidates for DFS cache operations
 * - Stay in closer synch with RSP cache for prefix invalidations
 *
 * Revision 3.3  1995/12/17  18:38:17  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:47:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:27:59  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/07/17  08:21:54  schaefer
 * CSCdi36181: more robust microcode management (on RSP)
 *
 * Revision 2.3  1995/07/04  01:57:42  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/16  22:56:54  shaker
 * CSCdi35912:  overlapping no ip route-cache in generated config
 * Don't emit 'no ip route-cache optimum' if 'no ip route-cache' is
 * configured.
 *
 * Revision 2.1  1995/06/07  22:57:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "logger.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../os/gdb.h"
#include "../os/free.h"
#include "rsp_pcmap.h"
#include "rsp_bufhdr.h"
#include "rsp_if.h"
#include "rsp_macaddr.h"
#include "rsp_chasint.h"
#include "../os/region.h"
#include "../ip/ip_registry.h"
#include "subsys.h"
#include "../hes/if_fci.h"
#include "../hes/ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../dev/flash_spec.h"
#include "../hes/slot.h"
#include "../crypto/crypto_registry.h"

enum {
    TEST_RSP_WB_CPU = 1,
    TEST_RSP_WB_MEMD,
    TEST_RSP_WB_IO,
};

enum {
    TEST_RSP_WBSTATE_OFF = 1,
    TEST_RSP_WBSTATE_ON,
    TEST_RSP_WBSTATE_RAW,		/* ON + read-around-write */
};


static void
rsp_test_wb_command (parseinfo *csb)
{
    ushort *reg;
    ushort val, raw_val;

    reg = (ushort*) ADRSPC_DCL;
    val = raw_val = 0;
    switch (GETOBJ(int,1)) {
      case TEST_RSP_WB_CPU:
	val = DCL_DRAM_WRBUF;
	raw_val = DCL_DRAM_RDBYPASS;
	break;

      case TEST_RSP_WB_MEMD:
	val = DCL_MEMD_WRBUF;
	raw_val = DCL_MEMD_RDBYPASS;
	break;

      case TEST_RSP_WB_IO:
	val = DCL_IO_WRBUF;
	break;
    }
    switch (GETOBJ(int,2)) {
      case TEST_RSP_WBSTATE_OFF: {
	  leveltype level;

	  level = raise_interrupt_level(ALL_DISABLE);
	  *reg &= ~(val | raw_val);
	  reset_interrupt_level(level);
	  break;
      }

      case TEST_RSP_WBSTATE_RAW:
	val |= raw_val;
	/* FALLTHROUGH */

      case TEST_RSP_WBSTATE_ON:
	*reg = val;
	break;
    }
    
    return;
}


enum {
    TEST_RSP_CACHE_CPU = 1,
    TEST_RSP_CACHE_MEMD_FASTSWITCH,
    TEST_RSP_CACHE_MEMD_OPTIMUM_AND_FLOW,
    TEST_RSP_CACHE_BUFHDR,
    TEST_RSP_CACHE_KSEG0,
};

static void
rsp_test_cache_command (parseinfo *csb)
{
    int newpolicy, ix;
    uchar *vstart, *vend;
    struct r4k_tlb_t tlb_e;
    regiontype *region;
    boolean kseg0;

    vstart = vend = NULL;
    kseg0 = FALSE;
    switch (GETOBJ(int,1)) {
      case TEST_RSP_CACHE_CPU:
	region = region_find_by_class(REGION_CLASS_LOCAL);
	vstart = region->start;
	vend   = region->end;
	break;

      case TEST_RSP_CACHE_MEMD_FASTSWITCH:
	vstart = (uchar*) ADRSPC_MEMD;
	vend = vstart + memd_size;
	break;

      case TEST_RSP_CACHE_MEMD_OPTIMUM_AND_FLOW:
	vstart = (uchar*) ADRSPC_MEMD_WRITETHRU;
	vend = vstart + memd_size;
	break;

      case TEST_RSP_CACHE_BUFHDR:
	vstart = (uchar*) ADRSPC_MEMD_BUFHDR;
	vend = vstart + MEMD_BUFHDR_SIZE;
	break;

      case TEST_RSP_CACHE_KSEG0:
	kseg0 = TRUE;
	break;
    }
    newpolicy = GETOBJ(int,2);
    if (kseg0) {
	ulong config, status;

	config = getcp0_config();
	config &= ~CONFIG_KSEG0_COHERENCY;
	config |= newpolicy;
	GLOBAL_INTERRUPT_BLOCK(status);
	setcp0_config(config);
	GLOBAL_INTERRUPT_UNBLOCK(status);
    } else {
	for (ix = 0; ix < MAX_TLB_ENTRIES; ++ix) {
	    uint vpagesize;
	    uchar *vpagebase;
	    boolean changed;

	    r4k_gettlb(&tlb_e, ix);
	    vpagebase = (uchar*) (tlb_e.hi.vpn2 << TLB_HI_VPN2SHIFT);
	    vpagesize = 0x1000 * (1 + tlb_e.pmask.mask);
	    changed = FALSE;
	    if ((vstart <= vpagebase) && (vpagebase < vend)) {
		tlb_e.lo0.c = newpolicy;
		changed = TRUE;
	    }
	    vpagebase += vpagesize;
	    if ((vstart <= vpagebase) && (vpagebase < vend)) {
		tlb_e.lo1.c = newpolicy;
		changed = TRUE;
	    }
	    if (changed)
		r4k_settlb(&tlb_e, ix, TRUE /* clear cache */);
	}
    }
    return;
}


/*
 * OBJ(int,1) == addr, OBJ(int,2) == mask
 */
static void
rsp_watchaddr_command (parseinfo *csb)
{
    uchar control;
    volatile uchar *ctl;
    volatile ushort *sreg;
    ulong addr, mask;

    addr = GETOBJ(int,1);
    mask = ~(ulong) GETOBJ(int,2);

    ctl = (uchar*) ADRSPC_WATCH_CONTROL;
    control = *ctl;
    *ctl = DCL_WATCH_INACTIVE;

    sreg = (ushort*) ADRSPC_WATCH_HI;
    *sreg = addr >> 16;
    sreg = (ushort*) ADRSPC_WATCH_LO;
    *sreg = addr & 0xffff;
    sreg = (ushort*) ADRSPC_WATCH_MASK_HI;
    *sreg = mask >> 16;
    sreg = (ushort*) ADRSPC_WATCH_MASK_LO;
    *sreg = mask & 0xffff;    

    *ctl = control;
    return;
}

static void
rsp_watchctl_command (parseinfo *csb)
{
    volatile uchar *reg;

    reg = (uchar*) ADRSPC_WATCH_CONTROL;
    *reg = GETOBJ(int,1);
    return;
}

enum {
    TEST_RSP_WATCH_ACT_CRASH = 1,
    TEST_RSP_WATCH_ACT_CONTINUE,
};

static void
rsp_watchact_command (parseinfo *csb)
{
    watch_action_crash = (GETOBJ(int,1) == TEST_RSP_WATCH_ACT_CRASH);
}

/*
 * test rsp debug {off|on}
 */

enum {
    TEST_RSP_DEBUG_STATE_OFF = 1,
    TEST_RSP_DEBUG_STATE_ON,
};

static void
rsp_test_debug_command (parseinfo *csb)
{
    extern boolean rsp_debug;

    printf("Setting RSP debug ");
    if (GETOBJ(int,1) == TEST_RSP_DEBUG_STATE_ON) {
	rsp_debug = TRUE;
	printf("on");
    } else {
	rsp_debug = FALSE;
	printf("off");
    }
}



/*
 * test rsp auto-interface-restart {off|on}
 */

enum {
    TEST_RSP_AIR_STATE_OFF = 1,
    TEST_RSP_AIR_STATE_ON,
};

static void
rsp_test_air_command (parseinfo *csb)
{
    extern disable_auto_interface_restart;

    printf("Setting auto interface restart to ");
    if (GETOBJ(int,1) == TEST_RSP_AIR_STATE_ON) {
	disable_auto_interface_restart = FALSE;
	printf("on");
    } else {
	disable_auto_interface_restart = TRUE;
	printf("off");
    }
}

#include "../hes/dbus.h"
#include "../hes/dgbus.h"

static slot_mask_t next_slot_mask;

static void
test_rsp_stall (parseinfo *csb)
{
    int ix;
    volatile uchar *cbus0, *cbus1;
    if(rsp_is_master()) {
	/*
	 * stop IPs that have been virtually removed
	 */
	for (ix = 0; ix < MAX_SLOTS; ++ix) {
	    if (next_slot_mask.data[ix] && !slot_present_mask.data[ix]) {
		uchar ctl;
		dgtype *dbus;
		slottype *sp;

		/*
		 * If it's an RSP, or it's disabled, or it's an empty slot,
		 * don't attempt to disable it.
		 */
		sp = get_slotp(ix);
		if (!sp || FCI_TYPE_RSP(sp->ctrlr_type) || 
		    (sp->flags & (BOARD_DISABLED | BOARD_NONEXIST)))
		    continue;

		dbus = pslot_to_dbus(ix);
		ctl = dbus_prot_rd(&dbus->diag_control);
		if (ctl & (MASTER_ENABLE | ENABLE_LED))
		    dbus_prot_wr(ix, &dbus->diag_control,
				 ctl & ~(MASTER_ENABLE | ENABLE_LED));
	    }
	}
	slot_present_mask = next_slot_mask;
	disable_cbus_grant();
	cbus0 = (uchar*) ADRSPC_CBUS0REG;
	*cbus0 |= (CBUSX_LATCHED_STALL | CBUSX_STALL_UPDATE);
	cbus1 = (uchar*) ADRSPC_CBUS1REG;
	*cbus1 |= (CBUSX_LATCHED_STALL | CBUSX_STALL_UPDATE);
    }
    return;
}

enum {
    TEST_SLOT_MASK = 1,
    TEST_SLOT_UNMASK,
    TEST_SLOT_PRINT,
};

static void
test_rsp_slot (parseinfo *csb)
{
    int cmd, slot;

    cmd = GETOBJ(int,1);
    slot = GETOBJ(int,2);

    switch (cmd) {
      case TEST_SLOT_MASK:	next_slot_mask.data[slot] = TRUE;	break;
      case TEST_SLOT_UNMASK:	next_slot_mask.data[slot] = FALSE;	break;
      case TEST_SLOT_PRINT: {
	  int ix;

	  for (ix = 0; ix < MAX_SLOTS; ++ix)
	      if (slot_present_mask.data[ix] || next_slot_mask.data[ix])
		  printf("slot %-2u: %smasked now, %smasked later\n", ix,
			 (slot_present_mask.data[ix]) ? "" : "un",
			 (next_slot_mask.data[ix])    ? "" : "un");
	  break;
      }
    }
    return;
}

extern ulong dcl_spurious_count;

static void
show_rsp_command (parseinfo *csb)
{
    netint_throttle_show();
    printf("\nDCL spurious %u", dcl_spurious_count);
    return;
}


static boolean ip_routec_optimum_cmd (parseinfo *csb)
{
    idbtype *idb = csb->interface;
    hwidbtype *hwidb = idb->hwptr;

    if (csb->nvgen) {
	/*
	 * Since 'optimum' switching defaults on, only emit
	 * 'no ip route-cache optimum' if the interface can support optimum
	 * switching (and the code to implement it is present), if normal
	 * fast switching is enabled, and if optimum switching is disabled:
	 */
	if (reg_invoke_ip_turbo_idb_capable(hwidb) &&
	    (hwidb->ip_routecache & IP_FAST_ROUTECACHE) &&
	    (! (hwidb->ip_routecache & IP_OPTIMUM_ROUTECACHE) ) ) {
	    nv_write(TRUE, "no %s", csb->nv_command);
	}
	return(TRUE);
    }

    /*
     * Only allow 'ip route-cache optimum' to be configured if the
     * interface can support the feature, and only if the code to implement
     * it is present
     */
    if (reg_invoke_ip_turbo_idb_capable(hwidb)) {
	if (csb->set_to_default || csb->sense) {
	    /* Flow and RSP switching are mutually exclusive */
	    hwidb->ip_routecache |=
		(IP_FAST_ROUTECACHE | IP_OPTIMUM_ROUTECACHE);
	    hwidb->ip_routecache &= ~IP_FLOW_ROUTECACHE;
	} else {
	    hwidb->ip_routecache &= ~IP_OPTIMUM_ROUTECACHE;
	}

        /*
         * Because DFS shares this config setting let the VIP
         * know of this change if distributed is on.
         */
        if (hwidb->ip_routecache & IP_DISTRIBUTED_ROUTECACHE)
            reg_invoke_ip_enable_disable_dfs_ipc(hwidb);
    } else {
	if (!system_loading && !csb->set_to_default && csb->sense) {
	    printf("Optimum switching not supported on %s", idb->namestring);
	} else {
	    hwidb->ip_routecache &= ~IP_OPTIMUM_ROUTECACHE;
	}
	return(FALSE);
    }
    return(TRUE);
}

static boolean ip_routec_flow_cmd (parseinfo *csb)
{
    idbtype *idb = csb->interface;
    hwidbtype *hwidb = idb->hwptr;

    if (csb->nvgen) {
	if (hwidb->ip_routecache & IP_FLOW_ROUTECACHE) {
	    nv_write(TRUE, "%s", csb->nv_command);
	}
	return(TRUE);
    }

    if (reg_invoke_ip_turbo_idb_capable(hwidb)) {
	if (csb->sense) {
	    /* Flow and RSP switching are mutually exclusive */
	    hwidb->ip_routecache |=
		(IP_FAST_ROUTECACHE | IP_FLOW_ROUTECACHE);
	    hwidb->ip_routecache &= ~IP_OPTIMUM_ROUTECACHE;
	} else {
	    hwidb->ip_routecache &= ~IP_FLOW_ROUTECACHE;
	    hwidb->ip_routecache |= IP_OPTIMUM_ROUTECACHE;
	}
	/*
	 * Because DFS shares this config setting let the VIP
	 * know of this change if distributed is on.
	 */
	if (hwidb->ip_routecache & IP_DISTRIBUTED_ROUTECACHE)
	    reg_invoke_ip_enable_disable_dfs_ipc(hwidb);
    } else {
	if (!system_loading && csb->sense) {
	    printf("Flow switching not supported on %s", idb->namestring);
	} else {
	    hwidb->ip_routecache &= ~IP_FLOW_ROUTECACHE;
	}
	return(FALSE);
    }
    return(TRUE);
}

static boolean ip_routec_dfs_cmd (parseinfo *csb)
{
    idbtype *idb = csb->interface;
    hwidbtype *hwidb = idb->hwptr;
 
    if (csb->nvgen) {
        if (hwidb->ip_routecache & IP_DISTRIBUTED_ROUTECACHE) {
            nv_write(TRUE, "%s", csb->nv_command);
        }
        return(TRUE);
    }
 
    if (IS_VIP(hwidb)) {
        if (csb->sense) {
            /* Flow and RSP switching are mutually exclusive */
            hwidb->ip_routecache |= IP_DISTRIBUTED_ROUTECACHE;
        } else {
            /*
             * Check if the interface has crypto map assigned. If it 
             * has, confirm from the user and warning about the 
             * disabling of encryption.
             */
            if (idb->crypto_map) {
                if (!reg_invoke_crypto_remove_dfs(idb)) {
                    return FALSE;
                }
            }
            hwidb->ip_routecache &= ~IP_DISTRIBUTED_ROUTECACHE;
        }
	reg_invoke_ip_enable_disable_dfs_ipc(hwidb);
    } else {
        if (!system_loading && csb->sense) {
            printf("Distributed switching not supported on %s",
                        idb->namestring);
        } else {
            hwidb->ip_routecache &= ~IP_DISTRIBUTED_ROUTECACHE;
        }
        return(FALSE);
    }
    return(TRUE);
}

#define ALTERNATE       NONE
#include "exec_test_rsp.h"
LINK_POINT(rsp_test_commands, ALTERNATE);
#undef  ALTERNATE

#define ALTERNATE       NONE
#include "exec_show_rsp.h"
LINK_POINT(rsp_show_commands, ALTERNATE);
#undef  ALTERNATE

#define ALTERNATE       NONE
#include "cfg_scheduler_alloc.h"
LINK_POINT(rsp_cfg_sched_cmd, ALTERNATE);
#undef  ALTERNATE

LINK_EXIT(ci_ip2_routecache_optimum_exit, no_alt);
KEYWORD_ID(ci_ip2_routecache_distributed, ci_ip2_routecache_optimum_exit,
           NONE,
           OBJ(int,1), IP_DISTRIBUTED_ROUTECACHE,
           "distributed", "Enable Distributed fast-switching cache",
           PRIV_CONF);
KEYWORD_ID(ci_ip2_routecache_flow, ci_ip2_routecache_optimum_exit,
           ci_ip2_routecache_distributed,
           OBJ(int,1), IP_FLOW_ROUTECACHE,
           "flow", "Enable Flow fast-switching cache", PRIV_CONF);
KEYWORD_ID(ci_ip2_routecache_optimum, ci_ip2_routecache_optimum_exit,
           ci_ip2_routecache_flow,
           OBJ(int,1), IP_OPTIMUM_ROUTECACHE,
           "optimum", "Enable Optimum fast-switching cache", PRIV_CONF);
LINK_POINT(ci_ip2_routecache_optimum_enter, ci_ip2_routecache_optimum);

static parser_extension_request rsp_chain_init_table[] = {
    { PARSE_ADD_TEST_CMD, &pname(rsp_test_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(rsp_show_commands) },
    { PARSE_ADD_CFG_SCHED_CMD, &pname(rsp_cfg_sched_cmd) },
    { PARSE_ADD_CFG_INT_IPROUTEC_CMD, &pname(ci_ip2_routecache_optimum_enter) },
    { PARSE_ADD_CFG_INT_IPROUTEC_EXIT,
	  (dynamic_transition *) &pname(ci_ip2_routecache_optimum_exit) },
    { PARSE_LIST_END, NULL }
};

void rsp_parser_init (subsystype *subsys)
{
    parser_add_command_list(rsp_chain_init_table, "rsp");

    /* The IP registry only exists on the Master RSP. */
    if (rsp_is_master()) {
      reg_add_ip_route_cache_cmd(IP_OPTIMUM_ROUTECACHE, 
                                 ip_routec_optimum_cmd,
			         "ip_routec_optimum_cmd");
      reg_add_ip_route_cache_cmd(IP_FLOW_ROUTECACHE, 
                                 ip_routec_flow_cmd,
			         "ip_routec_flow_cmd");
      reg_add_ip_route_cache_cmd(IP_DISTRIBUTED_ROUTECACHE, 
                                 ip_routec_dfs_cmd,
                                 "ip_routec_dfs_cmd");
    }
}


#define	RSP_UI_MAJVERSION	1
#define	RSP_UI_MINVERSION	0
#define	RSP_UI_EDITVERSION	1

SUBSYS_HEADER(rsp_ui, RSP_UI_MAJVERSION,
	      RSP_UI_MINVERSION, RSP_UI_EDITVERSION,
	      rsp_parser_init, SUBSYS_CLASS_MANAGEMENT,
	      NULL,
	      "req: rsp");
