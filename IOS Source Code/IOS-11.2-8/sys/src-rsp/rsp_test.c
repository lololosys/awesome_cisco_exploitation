/* $Id: rsp_test.c,v 3.9.4.6 1996/09/04 18:13:09 kmitchel Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_test.c,v $
 *------------------------------------------------------------------
 * rsp_test.c -- rsp diagnostic routines
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: rsp_test.c,v $
 * Revision 3.9.4.6  1996/09/04  18:13:09  kmitchel
 * CSCdi66418:  IOS overreacts to DBUS io port-whine
 * Branch: California_branch
 *
 * Revision 3.9.4.5  1996/08/07  18:02:51  kmitchel
 * CSCdi49854:  DBUSINTERR after RSP dbus write; ineffective recvry of
 * disabled card
 * Branch: California_branch
 *
 * Revision 3.9.4.4  1996/07/30  21:10:44  rramacha
 * CSCdi57573:  Serial interface stays line down when remote side toggles
 * Branch: California_branch
 * o Add debug support
 *
 * Revision 3.9.4.3  1996/07/10  02:37:59  getchell
 * Added crash context mechanism which is shown via diag command and test
 * cbus (vc command)
 * CSCdi60415:  VIP difficult to debug in the field
 * Branch: California_branch
 *
 * Revision 3.9.4.2  1996/03/30  08:38:22  ogrady
 * CSCdi53013:  RSP test commands shouldnt access cached memd addrs
 * Branch: California_branch
 * Use uncached accesses in test cbus.  Misc. cleanup.
 *
 * Revision 3.9.4.1  1996/03/18  22:02:42  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.3  1996/03/16  07:36:49  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.12.2  1996/03/07  10:45:16  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.12.1  1996/02/20  18:01:47  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/03/11  11:09:59  ogrady
 * CSCdi51224:  RSP emulation code is obsolete
 *              Expunge code.
 *
 * Revision 3.8  1996/03/11  09:56:00  ogrady
 * CSCdi50668:  TRIP and FIP address filter should disable protocol bits
 *              for SRB.  Fix some AF bit masks shift values which are not
 *              the same for TRIP and FIP.
 *
 * Revision 3.7  1996/02/24  02:12:52  dkerr
 * CSCdi49787:  IPC rolls over and dies under load
 * Make IPC more robust in an RSP->VIP distributed switching environment
 * (and increase performance from the miserly 100 msgs per minute to
 * something respectable, like 300K msgs per second).
 *
 * Revision 3.6  1996/02/04  02:29:42  shaker
 * CSCdi47971:  RSP-3-NORESTART error message
 * Prevent this error from happening. Also enhance error logging
 * for some hard to reproduce error cases.
 *
 * Revision 3.5  1996/02/01  06:10:01  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1995/12/28  08:25:03  foster
 * CSCdi46157:  Dbus down-load needs to work on rsp2 slave down-load
 *
 * Revision 3.3  1995/11/17  18:49:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:33:20  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:28:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:28:48  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/08/10  20:34:06  schaefer
 * CSCdi38496: output hang timestamp used as future timer
 *
 * Revision 2.5  1995/07/28  19:42:08  mcieslak
 * Added AIP specific commands to "test cbus" as part of:
 * CSCdi35575:  test cbus needs IP-specific commands
 *
 * Revision 2.4  1995/07/16  22:31:09  getchell
 * CSCdi35501:  src-rsp/ccb_cmd.h should be moved to hes
 * src-rsp/[ccb.h,ccb_cmd.h] combined and moved to hes/ccb_cmd.h.
 * rsp_command prototypes removed from ccb_cmd.h and moved to
 * src-rsp/rsp_commands.h.  Files including altered files modifed to
 * reflect new file structure.
 *
 * Revision 2.3  1995/07/14  01:03:35  schaefer
 * CSCdi36179: send multiple cmds with <test cbus>
 *
 * Revision 2.2  1995/06/28  09:31:10  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  22:58:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ctype.h>
#undef   tolower			/* use library function call */
#include <ciscolib.h>
#include "interface_private.h"
#include "../hes/cbus_registry.h"
#include "ttysrv.h"
#include "ieee.h"
#include "../hes/if_fci.h"
#include "config.h"
#include "subsys.h"
#include "rsp_bufhdr.h"
#include "rsp_pcmap.h"
#include "rsp_if.h"
#include "rsp_qasic.h"
#include "qa_hw.h"
#include "rsp_fast.h"
#include "rsp_pcmap.h"
#include "rsp_cache.h"
#include "rsp_commands.h"
#include "../hes/ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "../hes/dbus.h"
#include "../hes/dgbus.h"
#include "../hes/dbus_io.h"
#include "../hes/ucode.h"
#include "../hes/if_cmd.h"
#include "../hes/ccb_cmd.h"
#include "../hes/if_mci.h"
#include "../hes/if_vip.h"
#include "../hes/cbus_test.h"
#include "rsp_fast.h"
#include "../util/random.h"
#include "fastswitch_registry.h"
#include "../os/gdb.h"

qa_diag_t	qasic_diag_copy;
qa_diag_t     *qasic_diag_copy_ptr;


static ulong read_memd_long(ulong*);
static void  write_memd_long(ulong*, ulong);
static ulong read_qa_copy(ulong*);
static void  write_qa_copy(ulong*, ulong);


#define RSPTST_BUFLEN 			80


static ulong default_qnum = QA_FIRST_QNUM;
static ulong default_qval = MEMD_BUFHDR_OFFSET;
static ulong default_slot = 0;
static ulong default_if = 0;
static ulong default_accnum = QA_FIRST_ACCNUM;
static ulong default_accval = 0;
static ulong default_search_pattern = 0;

/*
 * characteristic of various arrays
 */
struct rsp_test_array_defaults
{
	char	*name;
	char	*el_name;
	char	*addr_fmt;			/* how to display addr */
	char	*value_fmt;			/* how to display data */
	ulong	value_size;			/* bytes per read */
	ulong	line_size;			/* display elements/line */
	ulong	(*read_fun)(ulong *);		/* how to read it */
	void	(*write_fun)(ulong *, ulong);	/* how to write it */
	uchar	**base;				/* base addr */
	ulong	amin;				/* min address/offset */
	ulong	amax;				/* max address/offset + 1 */
	ulong	aincr;				/* address incr */
	ulong	alow;				/* current low addr */
	ulong	ahi;				/* current hi addr */
	ulong	fill;				/* current fill value */
};
typedef struct rsp_test_array_defaults rsp_test_array;

/*
 * Memd array
 */
static rsp_test_array memd_array = {
	"Memd", "offset", "%08x", "%08x", 4, 1,
	read_memd_long, write_memd_long, &memd_base, MEMD_BUFHDR_OFFSET,
	0 /* see note */, 4, MEMD_BUFHDR_OFFSET, 0 /* see note */, 0
};
/* note - MEMD size is filled in at subsystem initialization time */




/*
 * QA Diag array - including error log.
 */
static rsp_test_array diag_array = {
	"QA Diag", "offset", "%04x", "%08x", 4, 1,
	(ulong(*)(ulong*)) read_qa_diag,
	(void(*)(ulong*,ulong)) write_qa_diag, (uchar**) &qasic_diag_base,
	0, DIAG_ERROR_LOG_BASE, 8, 0, DIAG_ERROR_LOG_BASE, 0
};


/*
 * QA Diag copy array - including error log.
 */
static rsp_test_array diag_copy_array = {
	"QA Copy", "offset", "%04x", "%08x", 4, 1,
	read_qa_copy, write_qa_copy, (uchar**) &qasic_diag_copy_ptr,
	0, DIAG_ERROR_LOG_BASE, 8, 0, DIAG_ERROR_LOG_BASE, 0
};



static uchar *zero_base_ptr = 0;



/*
 * IP mema array
 */
static rsp_test_array *ip_mem_array;

static ulong read_ip_mem(ulong *);
static void write_ip_mem(ulong *, ulong);
static ulong read_fsip_mem(ulong *);
static void write_fsip_mem(ulong *, ulong);

/*
 * output formats here have been set to the preference of the s/w developers
 */

static rsp_test_array eip_mem_array = {
	"EIP Mema", "address", "%04x", "%04x", 2, 8,
	read_ip_mem, write_ip_mem, &zero_base_ptr,
	0, 0x800, 1, 0, 0x800, 0
};

static rsp_test_array fip_mem_array = {
	"FIP Mema", "address", "%04x", "%04x", 2, 1,
	read_ip_mem, write_ip_mem, &zero_base_ptr,
	0, 0x100, 1, 0, 0x100, 0
};

static rsp_test_array fsip_mem_array = {
	"FSIP Mem", "address", "%05x", "%04x", 2, 2,
	read_fsip_mem, write_fsip_mem, &zero_base_ptr,
	0, 0x20000, 2, 0, 0x20000, 0
};

static rsp_test_array trip_mem_array = {
	"TRIP Mema", "address", "%04x", "%04x", 2, 8,
	read_ip_mem, write_ip_mem, &zero_base_ptr,
	0, 0x800, 1, 0, 0x800, 0
};

static rsp_test_array generic_ip_mem_array = {
	"IP Mema", "address", "%04x", "%04x", 2, 8,
	read_ip_mem, write_ip_mem, &zero_base_ptr,
	0, 0x10000, 1, 0, 0x10000, 0
};


/*
 * address filter array
 */

static ulong read_addr_filter(ulong *);
static void write_addr_filter(ulong *, ulong);

static rsp_test_array addr_filter_array = {
	"AF", "offset", "%04x", "%02x", 1, 8,
	read_addr_filter, write_addr_filter, &zero_base_ptr,
	0, 0x2000, 1, 0, 0x2000, 0
};



/*======================================================================
 *
 *			rsp general routines
 *
 *======================================================================*/


/*
 * DON'T r/w memd cached in a running system - this could possibly cause 
 *   problems.
 */
static ulong read_memd_long (ulong *memd_addr)
{
    return *(ulong *)ADDR_MEMD_TO_UNCACHED(memd_addr);
}

static void write_memd_long (ulong *memd_addr, ulong value)
{
    *(ulong *)ADDR_MEMD_TO_UNCACHED(memd_addr) = value;
    return;
}




/*
 * test for user abort, return non-zero if true
 */
static int rsp_test_abort (void)
{
    while (getc(stdio) != -1) /* flush pending input */
	/* null */;
    return (stdio->statbits & (IDLETIMEOUT | HANGUP | CARDROP | IDLE)) ||
	   (stdio->statbits&(CONTROLC|CTRLC_ENBL)) == (CONTROLC|CTRLC_ENBL);
}


/*
 * load user entered unsigned value into *default_val
 * return 1 if successful
 *       -1 if user just hit return
 *        0 if bad input or user abort
 */
static int rsp_get_usr_val (char *prompt, ulong *default_val,
                                ulong min, ulong max )
{
    ulong value;
    char buff[RSPTST_BUFLEN];
    boolean parsed;
    boolean okflag;
    int stat;

    value = 0;
    okflag = 0;
    stat = 0;

    while (TRUE) {

	automore_enable(NULL);	/* reset automore */
	memset(buff, 0, RSPTST_BUFLEN);
        printf("\n%s [%#x]: ", prompt, *default_val);
        parsed = rdtty(buff, RSPTST_BUFLEN-1);
        if (!parsed || rsp_test_abort())
            break;
        
        if (null(buff)) {
            stat = -1;
	    break;
	}

        value = parse_unsigned(buff, &okflag);

	if (buff[0] == 'q'  ||  buff[0] == 'Q')
		break;

        if (!okflag) {
	    printf("\nInvalid value: %s", buff);
            continue;
	}
    
        if (value < min || value > max) {
	    printf("\nValue out of range: %#x - %#x", min, max);
            continue;
        }

	*default_val = value;
        stat = 1;
	break;
    }

    return stat;
}

/*
 * get/set default slot number
 */
static int rsp_test_get_slot(void)
{
    return(rsp_get_usr_val("Enter slot number:", &default_slot,0,MAX_SLOTS-1)
	   && rsp_get_usr_val("Enter interface number:", &default_if, 0,
                            MAX_CTRLR_INTERFACES));
}


/*
 * display an array of values
 */
static void rsp_test_dump (rsp_test_array *array)
{
    ulong alow;
    int i;

    alow = array->alow & ~((array->aincr*array->line_size)-1);

    while (!automore_quit()  &&  alow < array->ahi) {

        printf("\n%s ", array->name);
        printf(array->addr_fmt, alow);
        printf(": ");

	for( i = 0; alow < array->ahi && i < array->line_size; i++) {
	    if(i == 4)
                printf("  ");
	    else 
                printf(" ");

            printf( array->value_fmt,
		(*array->read_fun)((ulong *)(*array->base+alow)));

            alow += array->aincr;
	}
    }
}

/*
 * copy an array of values
 */
static void rsp_test_copy (rsp_test_array *array,
			   uchar *src, uchar *dst, uint bytes)
{
    leveltype level;

    level = raise_interrupt_level(ALL_DISABLE);

    while (bytes >= sizeof(long)) {
	*(ulong*) dst = array->read_fun((ulong*) src);
	src += sizeof(long);
	dst += sizeof(long);
	bytes -= sizeof(long);
    }
    if (bytes > 0) {
	ulong val;

	val = array->read_fun((ulong*) src);
	src = (uchar*) &val;
	switch (bytes) {
	  case 3: *dst++ = *src++;
	  case 2: *dst++ = *src++;
	  case 1: *dst   = *src;
	}
    }
    reset_interrupt_level(level);
    return;
}

/*
 * fill an array with fill_value 
 */
static void rsp_test_fill (rsp_test_array *array)
			    
{
    ulong alow;

    alow = array->alow & ~(array->aincr-1);

    while (alow < array->ahi) {
        (*array->write_fun)((ulong *)(*array->base+alow), array->fill);
        alow += array->aincr;
    }
}

/*
 * write user specified values to array
 */
static void rsp_test_write (rsp_test_array *array)
{
    ulong default_write_value;
    ulong alow, stat;
    char buff[RSPTST_BUFLEN];

    sprintf(buff, "Enter starting %s %s", array->name, array->el_name); 

    if (rsp_get_usr_val(buff, &array->alow, array->amin, array->amax))
    {
	alow = array->alow & ~(array->aincr-1);

        for( ; alow < array->amax; alow += array->aincr)
	{
	    default_write_value =
			(*array->read_fun)((ulong*)(*array->base+alow));

	    sprintf(buff, "Enter value for %#x (or `q' when done)", alow);

            stat = rsp_get_usr_val(buff, &default_write_value,0,~0);

	    if (!stat)
		break;

            else if (stat > 0)
	    {
            	(*array->write_fun)((ulong*)(*array->base+alow),
				default_write_value);
	    }
        }
    }
}


/*
 * load array alow and ahi with user values between amin and amax
 */
static int rsp_test_get_addrs (rsp_test_array *array)
{
    char buff[RSPTST_BUFLEN];
    int  stat;
    ulong addr_mask;

    stat = FALSE;
    for (;;)
    {
	sprintf(buff, "Enter %s starting %s", array->name, array->el_name);
        stat = rsp_get_usr_val(buff, &array->alow, array->amin, array->amax);
	if (!stat)
		break;

	sprintf(buff, "Enter %s ending %s", array->name, array->el_name);
        stat = rsp_get_usr_val(buff, &array->ahi, array->amin, array->amax);
	if (!stat)
		break;

        if (array->alow > array->ahi)
		printf ("\nHigh value < low value?");
        else
	{
	    addr_mask = ~(array->aincr-1);
            array->alow &= addr_mask;
            array->ahi &= addr_mask;
	    break;
	}
    }
    return (stat);
}

/*
 * get user addresses and value and fill an array
 */
static void rsp_test_usr_fill (rsp_test_array *array)
{
    if (rsp_test_get_addrs(array)) {
        if (rsp_get_usr_val("Enter fill value", &array->fill, 0,~0L)) {
            rsp_test_fill(array);
	}
    }
}

/*
 * get user addresses and dump an array
 */
static void rsp_test_usr_read (rsp_test_array *array)
{
    if (rsp_test_get_addrs(array)){
        rsp_test_dump(array);
    }
}


/*
 * r/w an IP addr filter value
 */
static ulong addr_filter_op (ulong *af_addr, boolean write, ulong af_wr_val)

{
    hwidbtype	*hwidb;
    ushort	status;
    ushort	af_raw_data;
    int         af_shift;
    ulong	rtn_val;
    leveltype   level;


    hwidb = slots[default_slot].interface_list[default_if];
    rtn_val = 0;
                                        /* trip handles args unshifted */
    af_shift = (hwidb->type == IDBTYPE_CTR) ? 0 : 8;

    level = raise_interrupt_level(NETS_DISABLE);

    /* 
     * select if and af/address
     */
    status = select_if(hwidb);
    if (status == FCI_RSP_OKAY) {
        status = send_af_select_cmd(hwidb, (ushort)((ulong)af_addr));

        if (status == FCI_RSP_OKAY) {

            /* 
             * r/w address filter
             */
            if (write) {
                af_raw_data = (ushort)(af_wr_val << af_shift);
                status = send_af_write_cmd(hwidb, af_raw_data);
            } else {
                status = send_af_read_cmd(hwidb, &af_raw_data);
                rtn_val = (af_raw_data >> af_shift) & 0xff;
            }
        }
    } 

    reset_interrupt_level(level);
    
    if (status != FCI_RSP_OKAY) {
        printf ("\nError accessing address filter %s", hwidb->hw_namestring);
	/* halt any looping operations */
        addr_filter_array.ahi = addr_filter_array.alow;
    }

    return rtn_val;
}


/*
 * read IP addr filter value
 */
static ulong read_addr_filter (ulong *af_addr)
{
    return(addr_filter_op(af_addr, FALSE, 0L));
}

/*
 * write IP addr filter value
 */
static void write_addr_filter (ulong *af_addr, ulong af_wr_val)
{
    addr_filter_op(af_addr, TRUE, af_wr_val);
}



/*
 * disable IP and set up for AF ops
 */
static boolean addr_filter_op_setup (
    hwidbtype **hwidb,
    int *if_enabled)
{
    ushort	status;
    leveltype   level;

    if(!rsp_test_get_slot())
	return FALSE;

    *hwidb = slots[default_slot].interface_list[default_if];

    if(!*hwidb)
	return FALSE;

    status = FCI_RSP_OKAY;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Note whether or not the interface was enabled when
     * we're called, so that we can preserve this state.
     * if the interface is enabled, disable it before setting the
     * address filter
     */
    *if_enabled = (*hwidb)->hw_enabled;

    if (*if_enabled) {

        status = select_if(*hwidb);

        if (status == FCI_RSP_OKAY)
	    status = send_if_disable_cmd_delay(*hwidb, IF_DISABLE_TIME);
    }

    reset_interrupt_level(level);

    if (status != FCI_RSP_OKAY) {
        printf ("\nError disabling interface %s", (*hwidb)->hw_namestring);
        return FALSE;
    }

    return TRUE;
}

/*
 * reenable IP after AF op
 */
static void addr_filter_op_end (
    hwidbtype **hwidb,
    int *if_enabled)
{
    ushort	status;
    leveltype   level;

    /*
     * if the interface was enabled when we started, we need to
     * re-enable it now
     */
    status = FCI_RSP_OKAY;
    if (*if_enabled) {

        level = raise_interrupt_level(NETS_DISABLE);

        status = select_if(*hwidb);
	if (status == FCI_RSP_OKAY) {
	    status = send_if_enable_cmd_delay(*hwidb, 10);
	} 

        reset_interrupt_level(level);
    }

    if (status != FCI_RSP_OKAY)
        printf ("\nError reenabling interface %s", (*hwidb)->hw_namestring);
}


/*======================================================================
 *
 *			QA specific routines
 *
 *======================================================================*/

/*
 * return a user entered q number
 */
static int qa_get_q_num (void)
{
	ulong value;


	for (;;) {

		value = default_qnum;

		if (!rsp_get_usr_val("Enter queue number (or addr)",
			&value, 0,~0))
			break;

		if (is_qptr( (qa_hwq_t *)value)) {
			default_qnum = get_queue_number((qa_hwq_t *)value);
			return 1;
		}

		if (is_qnum(value)) {
			default_qnum = value;
			return 1;
		}

		printf("\nInvalid queue number (or addr) %#x", value);
	}
	return 0;
}

/*
 * return a user entered q value
 */
static int qa_get_q_val (char *prompt)

{
    ulong    v, stat;

    v = default_qval;
    stat = FALSE;
    if (!prompt)
      prompt = "Enter buffer header queue value";

    for (;;) {
        stat = rsp_get_usr_val(prompt, &v, MEMD_BUFHDR_OFFSET,
			       (MEMD_BUFDATA_OFFSET&~0xf)-0x10);
        if (!stat)
		break;
        if ((v & 0xf) == 0) {
	    default_qval = v;
            break;
	}
        printf("\nInvalid queue data value 0x%0x", v);
    }

    return (stat);
}


/*
 * return a user entered acc number
 */
static int qa_get_acc_num (void)
{
	ulong value;

	for(;;) {

		value = default_accnum;

		if (!rsp_get_usr_val("Enter accumulator number (or addr)",
			&value, 0,~0))
				break;

		if (is_accptr((qa_acc_t *)value)) {
			default_accnum = get_acc_number((qa_acc_t *)value);
			return 1;
		}

		if (is_accnum(value)) {
			default_accnum = value;
			return 1;
		}

		printf("\nInvalid accumulator number (or addr) %#x", value);
	}
	return 0;
}


/*
 * return a user entered acc value
 */
static int qa_get_acc_val (void)
{
    return rsp_get_usr_val("Enter accumulator value",
                            &default_accval, 0, 0xffff);
}


/*
 * list contents of a queue
 */
static void qa_dump_q (ulong qnum, rsp_test_array *array)
{
    int count;
    ushort    n;

    n = QA_READ_HEAD(qnum, array->read_fun, (qa_diag_t *)(*array->base));
    printf("\n\n%04x ", n);
    count = 1;
        
    while(!automore_quit() && n) {
        n = QA_READ_BODY(n, array->read_fun, (qa_diag_t *)(*array->base));
        if ((count++ % 8) == 0)
            printf("\n");
          printf("%04x ", n);
    }
    printf("\n(%d total)", count-1);
}




/*
 * send command to IP - return 1 on success else 0
 * load rsp with response
 */

static int rsp_test_send_ipcmd (ushort cmd, ushort arg0, ushort arg1,
				ulong *rsp)
{
	slottype *slotp;
	ulong cmd_qnum;
	int status;
	leveltype level;


	slotp = &slots[default_slot];

	cmd_qnum = SLOT2CMDQ(default_slot);

	level = raise_interrupt_level(ALL_DISABLE);

	/*
	 * assume IP attention has been set up by system
	 */

	*rsp = ~0;

	send_ip_cmd_2arg(slotp, cmd, arg0, arg1);
	status = (get_ip_status(slotp) == CMD_RSP_OKAY) ? 1 : 0;
	if (status) {
		*rsp = get_ip_lval(slotp);
	}

	reset_interrupt_level(level);

	return status;
}

/*
 * support functions for r/w ip memory;
 */
static ulong read_ip_mem (ulong *addr)
{
	ulong rsp;

	(void)rsp_test_send_ipcmd(CCB_CMD_MEMA_SELECT,(ushort)(ulong)addr,
		0, &rsp);
	(void)rsp_test_send_ipcmd(CCB_CMD_MEMA_READ, 0, 0, &rsp);
	return(rsp & 0xFFFF);
}
static void write_ip_mem (ulong *addr, ulong value)
{
	ulong rsp;

	(void)rsp_test_send_ipcmd(CCB_CMD_MEMA_SELECT,(ushort)(ulong)addr,
		0, &rsp);
	(void)rsp_test_send_ipcmd(CCB_CMD_MEMA_WRITE, value, 0, &rsp);
}

static ulong read_fsip_mem (ulong *addr)
{
	return(read_ip_mem((ulong *)((ulong)addr/2)));
}

static void write_fsip_mem (ulong *addr, ulong value)
{
	write_ip_mem((ulong *)((ulong)addr/2), value);
}


/*======================================================================
 *
 *			rsp test menu command funtions
 *
 *======================================================================*/

/*
 * dequeue element from q and print
 */
static void rsp_test_dequeue (void)
{
	if (qa_get_q_num()) {
                printf("0x%08x", (ulong)hwq_dequeue(queue_ptr(default_qnum)));
	}
}

/*
 * enqueue value to q
 */
static void rsp_test_enqueue (void)
{
            if (qa_get_q_num() && qa_get_q_val(NULL)) {
                hwq_enqueue(queue_ptr(default_qnum),
			(bufhdr_t*)BUFHDR_OFFSET_TO_ADDR(default_qval));
            }
}

/*
 * peek at q
 */
static void rsp_test_peek_q (void)
{
       if (qa_get_q_num()) {
            printf("0x%08x", (ulong) hwq_peek(queue_ptr(default_qnum)));
       }
}

/*
 * read and dec accum
 */
static void rsp_test_dec_acc (void)
{
    if (qa_get_acc_num()) {
	printf("0x%04x", (ushort)
		hwacc_read_dec(acc_ptr(default_accnum)));
    }
}

/*
 * write (subtract from) acc
 */
static void rsp_test_write_acc (void)
{
        if (qa_get_acc_num()  &&  qa_get_acc_val()) {
                hwacc_sub(acc_ptr(default_accnum), default_accval);
        }
}

/*
 * peek at acc value
 */
static void rsp_test_peek_acc (void)
{
            if (qa_get_acc_num()) {
                printf("0x%04x",
			(ushort) hwacc_peek(acc_ptr(default_accnum)));
            }
}

/*
 * set acc to value
 */
static void rsp_test_init_acc (void)
{
        if (qa_get_acc_num()  &&  qa_get_acc_val()) {
                hwacc_set(acc_ptr(default_accnum), default_accval);
        }
}


/*
 * dump all accumulators
 */
static void rsp_test_dump_acc (void)
{
    int accn;

    for (accn = QA_FIRST_ACCNUM; accn <= QA_LAST_ACCNUM; ++accn) {
	short val;

	val = hwacc_peek(acc_ptr(accn));
	if (val)
	    printf("\nAcc %#x = %#x", accn, val);
    }
    return;
}


/*
 * read diag space
 */
static void rsp_test_read_diag (void)
{
    rsp_test_usr_read(&diag_array);
}

/*
 * write diag space
 */
static void rsp_test_write_diag (void)
{
       rsp_test_write(&diag_array);
}

/*
 * fill diag space with pattern
 */
static void rsp_test_fill_diag (void)
{
    rsp_test_usr_fill(&diag_array);
}

/*
 * read diag copy space
 */
static void rsp_test_read_qa_copy (void)
{
    rsp_test_usr_read(&diag_copy_array);
}

/*
 * write diag copy space
 */
static void rsp_test_write_qa_copy (void)
{
       rsp_test_write(&diag_copy_array);
}

/*
 * fill diag copy space with pattern
 */
static void rsp_test_fill_qa_copy (void)
{
    rsp_test_usr_fill(&diag_copy_array);
}

/*
 * find a bufhdr on a queue
 */
static void rsp_test_find_buffer (rsp_test_array *array)
{
    uint qnum, target, lsabit;
    uint buf, el, lastbuf, tail;

    if (!qa_get_q_val(NULL))
	return;
    target = default_qval;
    for (qnum = QA_FIRST_QNUM; qnum <= QA_LAST_QNUM; ++qnum) {

	buf = QA_READ_HEAD(qnum, array->read_fun, (qa_diag_t *)(*array->base));
	el = 1;
	lastbuf = 0;
	while (buf) {
	    lsabit = QA_READ_LSA( buf,
			array->read_fun, (qa_diag_t *)(*array->base));

	    if (lsabit == 0) {
		printf("\nBuffer 0x%04x, element %u on queue %#x, "
		       "has clear LSA", buf, el, qnum);
		break;
	    }
	    if (!is_bufhdr_off(buf)) {
		printf("\nIllegal buffer 0x%04x found on queue %#x, "
		       "element %u", buf, qnum, el);
		break;
	    }
	    if (buf == target) {
		printf("\nBuffer 0x%04x is element %u on queue %#x",
		       buf, el, qnum);
		return;
	    }

            if (el > QA_NQ_ELEMENTS) {
		printf("\nNo NULL terminator for queue %#x?", qnum);
		break;
	    }

	    ++el;
	    lastbuf = buf;
	    buf = QA_READ_BODY(buf, array->read_fun,
				    (qa_diag_t *)(*array->base));
	}
	if (lastbuf && !buf) {

	    tail =
	      QA_READ_TAIL(qnum, array->read_fun,(qa_diag_t *)(*array->base));

	    if (lastbuf != tail) {
	        lsabit = QA_READ_LSA( tail,
				array->read_fun, (qa_diag_t *)(*array->base));
		printf("\nQueue %#x: "
		       "last element 0x%04x != tail 0x%04x (LSA %u)",
		       qnum, lastbuf, tail, lsabit);
	    }
	}
    }
    printf("\nBuffer 0x%04x is not on any queue", target);
}
static void rsp_test_find_buf (void)
{
	rsp_test_find_buffer(&diag_array);
}
static void rsp_test_find_copy_buf (void)
{
	rsp_test_find_buffer(&diag_copy_array);
}


/*
 * find bufhdrs not on any queue
 */
static void rsp_test_missing_buffers (rsp_test_array *array)
{
    uint qnum, bnum, lsabit;
    ushort* bp;

    bp = malloc(bufhdr_count * sizeof(ushort));
    if (bp == NULL) {
	printf(nomemory);
	return;
    }
    for (qnum = QA_FIRST_QNUM; qnum <= QA_LAST_QNUM; ++qnum) {
	uint buf, el, lastbuf;

	buf = QA_READ_HEAD(qnum, array->read_fun, (qa_diag_t *)(*array->base));
	el = 0;
        lastbuf = 0;
	while (buf) {

	    ++el;
            if (el > QA_NQ_ELEMENTS) {
		printf("\nNo NULL terminator for queue %#x?", qnum);
		break;
	    }

	    if (!is_bufhdr_off(buf)) {
		printf("\nQueue %#3x, element %u: Illegal Buffer 0x%04x",
		       qnum, el, buf);
		break;
	    }

	    lsabit = QA_READ_LSA( buf,
				array->read_fun, (qa_diag_t *)(*array->base));

	    if (lsabit == 0) {
		printf("\nQueue %#3x, element %u: buffer %#04x "
			"has clear LSA", qnum, el, buf);
		break;
	    }

	    bnum = (buf - MEMD_BUFHDR_OFFSET) / sizeof(bufhdr_t);

	    if (bp[bnum]) {
		printf("\nQueue %#3x, element %u: buffer %#04x "
		       "also on queue %#x",
			qnum, el, buf, bp[bnum]);
		break;
	    } else {
		bp[bnum] = qnum;
	    }
	    lastbuf = buf;
	    buf = QA_READ_BODY(buf, array->read_fun,
			      (qa_diag_t *)(*array->base));
	}
	if (lastbuf && !buf) {
	    uint tail;

	    tail = QA_READ_TAIL(qnum, array->read_fun,
				(qa_diag_t *)(*array->base));

	    if (lastbuf != tail) {

	        lsabit = QA_READ_LSA( tail,
				array->read_fun, (qa_diag_t *)(*array->base));

		printf("\nQueue %#3x: "
		       "last element 0x%04x != tail 0x%04x (LSA %u)",
		       qnum, lastbuf, tail, lsabit);
	    }
	    printf("\nQueue %#3x: %u elements", qnum, el);
	}
	if (rsp_test_abort())
	    return;
    }
    for (bnum = 0; bnum < bufhdr_count; ++bnum) {
	uint buf;

	if (bp[bnum] == 0) {
	    buf = MEMD_BUFHDR_OFFSET + bnum * sizeof(bufhdr_t);

	    lsabit = QA_READ_LSA( buf,
			    array->read_fun, (qa_diag_t *)(*array->base));

	    printf("\nBuffer %#04x not found, LSA %u", buf, lsabit);
	}
    }
    free(bp);
}

static void rsp_test_missing_bufs (void)
{
    rsp_test_missing_buffers(&diag_array);
}
static void rsp_test_missing_copy_bufs (void)
{
    rsp_test_missing_buffers(&diag_copy_array);
}

static ulong read_qa_copy (ulong *qa_copy_addr)
{
    return(*qa_copy_addr);
}
static void write_qa_copy (ulong *qa_copy_addr, ulong value)
{
    *qa_copy_addr = value;
    return;
}
void rsp_snap_qadiag (void)
{
    if (!confirm("Causes long CyBus stall.  Continue?"))
	return;
    printf("\nQA copy at %#08x", &qasic_diag_copy);
    rsp_save_qadiag(&qasic_diag_copy);
}


/*
 * dump buffer header contents
 */
static void rsp_test_dump_bufhdr (void)
{
    bufhdr_t bh;

    if (!qa_get_q_val(NULL))
	return;

    rsp_test_copy(&memd_array, BUFHDR_OFFSET_TO_ADDR(default_qval),
		  (uchar*) &bh, sizeof(bh));

    printf("\n" "rxstat     0x%04x"	"      txptr   0x%06x (flags 0x%02x)"
	   "\n" "rxbytes     %-5u"	"      txbytes    %-5x"
	   "\n" "rxstat2    0x%04x"	"      unused    0x%04x"
	   "\n" "fipstat    0x%04x"	"      bcastid   0x%04x"
	   "\n" "bufptr   0x%06x"	"      qptr      0x%04x",
	   bh.bf_rx_status,		bh.bf_tx_ptr & TX_PTR_MASK,
	   						bh.bf_tx_ptr >> 24,
	   bh.bf_rx_bytes,		bh.bf_tx_bytes,
 	   bh.bf_rx_status2,		bh.bf_unused,
	   bh.bf_fip_status,		bh.bf_bcast_id,
	   bh.bf_buffer_ptr,		bh.bf_queue_ptr);

    if ((bh.bf_queue_ptr & ~LOCAL_QUEUE_MASK) == 0) {
	hwidbtype* hwidb;

	hwidb = LOCAL_QUEUE_TO_HW_IDB(bh.bf_queue_ptr);
	if (hwidb)
	    printf(" (%s)", hwidb->hw_namestring);
    }


    if ( bh.bf_buffer_ptr >= memd_array.amin  &&  
	 bh.bf_buffer_ptr < memd_array.amax  &&
 	 yes_or_no("\nDump buffer data",
	          FALSE /* default */, TRUE /* use default */)) {
        automore_enable(NULL);
	memd_array.alow = bh.bf_buffer_ptr & ~memd_array.aincr;
	memd_array.ahi = memd_array.alow + 8192;
        if (memd_array.ahi > memd_array.amax)
	    memd_array.ahi = memd_array.amax;
        rsp_test_dump(&memd_array);
    }

    return;
}    

/*
 * convert to/from addr to q or a number
 */
static void rsp_test_cnv (void)
{
	ulong value = default_slot;
	int	found = 0;

	if (rsp_get_usr_val( "Enter queue or acc number or address",
		&value, 0,~0)) {

		if (is_qptr( (qa_hwq_t *)value)) {
			printf( "\nQ ptr %#x == Q number %#x", value,
			get_queue_number((qa_hwq_t *)value));
			found = 1;

		} else if (is_qnum(value)) {
			printf( "\nQ number %#x == Q ptr %#x", value,
			queue_ptr(value));
			found = 1;
		}

		if (is_accptr( (qa_acc_t *)value)) {
			printf( "\nACC ptr %#x == ACC number %#x", value,
			get_acc_number((qa_acc_t *)value));
			found = 1;

		} else if (is_accnum(value)) {
			printf( "\nACC number %#x == ACC ptr %#x", value,
			acc_ptr(value));
			found = 1;
		}

		if (!found)
			printf( "\nNo Q or ACC number/ptr for %#x", value );
	}
}



/*
 * read Memd addr range
 */
static void rsp_test_read_memd (void)
{
     rsp_test_usr_read(&memd_array);
}

/*
 * repeat read Memd addr range - for hw testing
 */
static void rsp_test_rread_memd (void)
{
    ulong *p, *pend, n;

    n = 0;
    if (rsp_test_get_addrs(&memd_array)){

	psion(stdio);
        while ( !psipending()) {

	    p = (ulong *)MEMD_OFFSET_TO_ADDR(memd_array.alow);
	    pend = (ulong *)MEMD_OFFSET_TO_ADDR(memd_array.ahi);

            while (p < pend)
               n += *p++;
        }
        printf("\ntotal: %08x", n);
        psioff(stdio);
    }
}

/*
 * write to Memd
 */
static void rsp_test_write_memd (void)
{
    rsp_test_write(&memd_array);
}

/*
 * fill Memd with pattern
 */
static void rsp_test_fill_memd (void)
{
    rsp_test_usr_fill(&memd_array);
}


/*
 * read IP mema addr range
 */
static void rsp_test_read_ip_mem (void)
{
    rsp_test_usr_read(ip_mem_array);
}

/*
 * write to IP mema
 */
static void rsp_test_write_ip_mem (void)
{
    rsp_test_write(ip_mem_array);
}

/*
 * fill IP mema with pattern
 */
static void rsp_test_fill_ip_mem (void)
{
    rsp_test_usr_fill(ip_mem_array);
}


/*
 * read addr filter range
 */
static void rsp_test_read_addr_filter (void)
{
    hwidbtype *hwidb;
    int if_enabled;

    if (addr_filter_op_setup(&hwidb, &if_enabled)) {
        rsp_test_usr_read(&addr_filter_array);
        addr_filter_op_end(&hwidb, &if_enabled);
    }
}

/*
 * write to addr filter
 */
static void rsp_test_write_addr_filter (void)
{
    hwidbtype *hwidb;
    int if_enabled;

    if (addr_filter_op_setup(&hwidb, &if_enabled)) {
        rsp_test_write(&addr_filter_array);
        addr_filter_op_end(&hwidb, &if_enabled);
    }
}

/*
 * fill addr filter with pattern
 */
static void rsp_test_fill_addr_filter (void)
{
    hwidbtype *hwidb;
    int if_enabled;

    if (addr_filter_op_setup(&hwidb, &if_enabled)) {
        rsp_test_usr_fill(&addr_filter_array);
        addr_filter_op_end(&hwidb, &if_enabled);
    }
}



/*
 * show contents of queue
 */
static void rsp_test_show_q (void)
{
    if(qa_get_q_num()) {
        qa_dump_q(default_qnum, &diag_array);
    }
}
static void rsp_test_show_qa_copy_q (void)
{
    if(qa_get_q_num()) {
        qa_dump_q(default_qnum, &diag_copy_array);
    }
}


/*
 * parameters for array searches
 */
struct search_parms
{
	ulong	pattern;
	ulong	pattern_hi;
	boolean short_pattern;
};

/*
 * get array search params from user
 */
static int rsp_test_get_search_parms (struct search_parms *parms)
{
    int stat;

    parms->pattern = 0;
    parms->pattern_hi = 0;
    parms->short_pattern = 0;

    parms->pattern = default_search_pattern;

    stat = rsp_get_usr_val("Enter search pattern:", &parms->pattern, 0, ~0L);
    if (stat) {

        default_search_pattern = parms->pattern;

        if (parms->pattern <= 0xffff) {
	    parms->short_pattern =
		yes_or_no("Short word pattern", FALSE, TRUE);
            if (parms->short_pattern) {
                parms->pattern_hi = parms->pattern << 16;
                parms->pattern &= 0xffff;
            }
	}
    }
    return stat;
}



/*
 * Search array for a pattern, print matching locations
 */
static void rsp_test_search (rsp_test_array *array, struct search_parms *parms)
{
    ulong alow, val;
    int	match;

    psion(stdio);

    printf("\n\nSearching %s\n", array->name);

    for (alow = array->amin;
	 !psipending() && !automore_quit() && alow < array->amax;
	 alow += array->aincr) {
	
	val = (*array->read_fun)((ulong *)(*array->base+alow));
	match = 0;

	if (parms->short_pattern && array->value_size >= 3) {
		if ((val&~0xffff) == parms->pattern_hi || 
	             (val&0xffff) == parms->pattern)
			match = 1;
	}
	else if (val == parms->pattern)
		match = 1;



	if (match) {
	        printf("\n%s ", array->name);
	        printf( array->addr_fmt, alow);
	        printf(": ");
	        printf( array->value_fmt, val);
        }
    }
    psioff(stdio);
}



/*
 * Search qa and memd for a pattern
 */
static void rsp_test_search_memd (void)
{
    struct search_parms parms;

    if (rsp_test_get_search_parms(&parms))
    {
	rsp_test_search(&diag_array, &parms);
	rsp_test_search(&memd_array, &parms);
    }
}

/*
 * Search ip mema for a pattern
 */
static void rsp_test_search_ip_mem (void)
{
    struct search_parms parms;

    if (rsp_test_get_search_parms(&parms))
    {
	rsp_test_search(ip_mem_array, &parms);
    }
}


/*
 * Search address filter for a pattern
 */
static void rsp_test_search_addr_filter (void)
{
    struct search_parms parms;
    hwidbtype *hwidb;
    int if_enabled;

    if (rsp_test_get_search_parms(&parms))
    {
        if (addr_filter_op_setup(&hwidb, &if_enabled)) {
	    rsp_test_search(&addr_filter_array, &parms);
            addr_filter_op_end(&hwidb, &if_enabled);
        }
    }
}

/*
 * Search qa diag copy a pattern
 */
static void rsp_test_search_qa_copy (void)
{
    struct search_parms parms;

    if (rsp_test_get_search_parms(&parms))
    {
	rsp_test_search(&diag_copy_array, &parms);
    }
}




/*
 * init IP command parameters
 */
static void rsp_test_ipcmd_init (void)
{
	(void)rsp_test_get_slot();

	printf( "\nCommand queue for slot %d is %#x.  CCB is %#x",
		default_slot, SLOT2CMDQ(default_slot),
		ADDR_TO_MEMD_OFFSET_24(slots[default_slot].ccb_ptr));

    switch( slots[default_slot].ctrlr_type) {
    case FCI_ETHER_CONTROLLER:	ip_mem_array = &eip_mem_array; break;
    case FCI_CTR_CONTROLLER:	ip_mem_array = &trip_mem_array; break;
    case FCI_FDDIT_CONTROLLER:	ip_mem_array = &fip_mem_array; break;
    case FCI_FSIP_CONTROLLER:	ip_mem_array = &fsip_mem_array; break;
    default:			ip_mem_array = &generic_ip_mem_array; break;
    }
}


#define MAXCMDS	10

/*
 * send IP command - print result
 */
static void rsp_test_ipcmd (void)
{
    ulong val;
    ushort status;
    int ix, count;
    leveltype level;
    ccbtype replies[MAXCMDS];

    static int numcmds = 1, repeat = 1;
    static ccbtype cmds[MAXCMDS];

    val = numcmds;
    if (!rsp_get_usr_val("Number of cmds:", &val, 1, MAXCMDS))
	return;
    numcmds = val;

    for (ix = 0; ix < numcmds; ++ix) {
	printf("\nCommand %u", 1 + ix);

	val = cmds[ix].ccb_cmd;
	if (!rsp_get_usr_val("  Cmd:", &val, 0, 0xffff))
	    return;
	cmds[ix].ccb_cmd = val;

	for (;;) {
	    printf("\n  -- args (end with ^C)");

	    val = cmds[ix].ccb_arg0;
	    if (!rsp_get_usr_val("  Arg0:", &val, 0, 0xffff))
		break;
	    cmds[ix].ccb_arg0 = val;

	    val = cmds[ix].ccb_arg1;
	    if (!rsp_get_usr_val("  Arg1:", &val, 0, 0xffff))
		break;
	    cmds[ix].ccb_arg1 = val;

	    val = cmds[ix].ccb_res1;
	    if (!rsp_get_usr_val("  Res1:", &val, 0, 0xffff))
		break;
	    cmds[ix].ccb_res1 = val;

	    val = cmds[ix].ccb_res0;
	    if (!rsp_get_usr_val("  Res0:", &val, 0, 0xffff))
		break;
	    cmds[ix].ccb_res0 = val;

	    val = cmds[ix].ccb_diag0;
	    if (!rsp_get_usr_val("  Diag0:", &val, 0, 0xffff))
		break;
	    cmds[ix].ccb_diag0 = val;

	    val = cmds[ix].ccb_diag1;
	    if (!rsp_get_usr_val("  Diag1:", &val, 0, 0xffff))
		break;
	    cmds[ix].ccb_diag1 = val;

	    break;
	}

	cmds[ix].ccb_done = TRUE;
    }
    val = repeat;
    if (!rsp_get_usr_val("Repeat count:", &val, 1, MAXINT))
	return;
    repeat = val;

    printf("\n");

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * assume IP attention has been set up by system
     */

    status = CMD_RSP_OKAY;
    for (count = 0; count < repeat; ++count) {
	for (ix = 0; ix < numcmds; ++ix) {
	    *slots[default_slot].ccb_ptr = cmds[ix];

	    send_ip_cmd(&slots[default_slot], cmds[ix].ccb_cmd);
	    status = get_ip_status(&slots[default_slot]);

	    replies[ix] = *slots[default_slot].ccb_ptr;

	    if (status != CMD_RSP_OKAY) {
		reset_interrupt_level(level);
		printf("\nCmd %u failed, status %04x", 1 + ix, status);
		break;
	    }
	}
	if (status != CMD_RSP_OKAY) {
	    if (repeat > 1)
		printf(", during iteration %u", 1 + count);
	    break;
	}
    }
    if (status == CMD_RSP_OKAY)
	reset_interrupt_level(level);

    for (ix = 0; ix < numcmds; ++ix) {
	printf("\n%-2u: cmd %x, done %x, "
	       "res1:0 %04x-%04x, arg1:0 %04x-%04x, diag1:0 %04x-%04x",
	       1 + ix,
	       replies[ix].ccb_cmd,
	       replies[ix].ccb_done,
	       replies[ix].ccb_res1,
	       replies[ix].ccb_res0,
	       replies[ix].ccb_arg1,
	       replies[ix].ccb_arg0,
	       replies[ix].ccb_diag1,
	       replies[ix].ccb_diag0);
    }

    return;
}




#ifdef DFS_TEST

static uchar bridgepacket[60] = {
    0x50,
    0x00, 0x00, 0x22, 0x22, 0x22, 0x22,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0xf0, 0xf0, 0x03,
    
    0x12, 0x34, 0x56, 0x78, 0x9A, 0xbc, 0xcd, 0xef,
    0x12, 0x34, 0x56, 0x78, 0x9A, 0xbc, 0xcd, 0xef,
    0x12, 0x34, 0x56, 0x78, 0x9A, 0xbc, 0xcd, 0xef,
    0x12, 0x34, 0x56, 0x78, 0x9A, 0xbc, 0xcd, 0xef,
};

static uchar *txptr = &bridgepacket[0];
qa_acc_t *gen_tql = (qa_acc_t *)0x4800008A;
qa_hwq_t *gen_txq = (qa_hwq_t *)0x48000150;

#endif /* DFS_TEST */

ulong genlen = 60;
ulong da_min = 1;
ulong da_max = 5;
ulong da_off = 2;
ulong sa_min = 0;
ulong sa_max = 0x28;
ulong sa_off = 8;
ulong hash_max = 20;
ulong packet_count = 100000;
ulong bridge_packet_debug = 1;

#ifdef DFS_TEST
/*
 * bridging traffic generator
 * This is just some bogus code to generate lots of random
 * bridge cache entries.
 */
static boolean rsp_generate_packet (uchar *start, ulong len,
				    qa_acc_t *tqlptr, qa_hwq_t *txqptr)
{
    bufhdr_t	*bufptr;
    ulong	tx_memd_offset;
    
    /*
     * Check tql.
     */
    if (!hwacc_read_dec_inline(tqlptr))
	return(FALSE);

    /*
     * Get a MEMD data buffer, with buffer header,
     * large enough to hold the output packet and some prefix pad.
     */
    bufptr = get_memd_buffer(len + RSP_ALIGNED_BUFSTART_PAD);
    if (!bufptr) {
	hwacc_inc_inline(tqlptr); /* Restore TX limit */
	return(FALSE);
    }

    /*
     * Set up the output buffer header, and copy the output packet
     * from DRAM into MEMD:
     */
    tx_memd_offset = bufptr->bf_buffer_ptr + RSP_ALIGNED_BUFSTART_PAD;
    txptr = MEMD_OFFSET_TO_UNCACHED_ADDR(tx_memd_offset);
    bufptr->bf_tx_ptr = ADDR_TO_MEMD_OFFSET_24(txptr);
    bufptr->bf_tx_bytes = len;
    memdcopy(start, txptr, len);
    
    /*
     * Enqueue the packet on the IP's transmit queue:
     */
    hwq_enqueue_inline(txqptr, bufptr);
    return(TRUE);
}

static void rsp_bridge_packet_generator (void)
{
    leveltype	SR;
    ulong cnt, sent_cnt = 0, len = genlen, i;
    boolean sent;
    ulong sa_range, da_range;
    
    sa_range = sa_max - sa_min;
    da_range = da_max - da_min;
    
    bridgepacket[da_off] = da_min;
    bridgepacket[sa_off] = sa_min;

    for (cnt=0; cnt<packet_count; cnt++) {
	if (da_range)
	  bridgepacket[da_off] = (random_gen() % da_range) + da_min;
	if (sa_range)
	  bridgepacket[sa_off] = (random_gen() % sa_range) + sa_min;
	
	SR = raise_interrupt_level(NETS_DISABLE);
	sent = rsp_generate_packet((uchar *)bridgepacket,len,gen_tql,gen_txq);
	reset_interrupt_level(SR);
	if (sent) {
	    sent_cnt++;
	} else {
	    process_suspend();
	}
    }
    printf(" Sent %d packets out of %d tries\n", sent_cnt, cnt);
    printf("Last memd packet:\n");
    for (i=0; i<32; i++)
      printf("%02x", txptr[i]);
    printf("\n");
}

static void rsp_bridge_packet_setup (void)
{
    int i;
    ulong val;
    char prompt[64];
    
    if (!rsp_get_usr_val("Packets to send:", &packet_count, 1, 1000000000))
	return;
    if (!rsp_get_usr_val("Packet length:", &genlen, 16, 1500))
	return;
    if (!rsp_get_usr_val("First random byte offset:", &da_off, 0, 255))
	return;
    if (!rsp_get_usr_val("First random byte min val:", &da_min, 0, 255))
	return;
    if (!rsp_get_usr_val("First random byte max val:", &da_max, da_min, 255))
	return;
    if (!rsp_get_usr_val("Second random byte offset:", &sa_off, 0, 255))
	return;
    if (!rsp_get_usr_val("Second random byte min val:", &sa_min, 0, 255))
	return;
    if (!rsp_get_usr_val("Second random byte max val:", &sa_max, sa_min, 255))
	return;
    if (!rsp_get_usr_val("TXQ address:", (ulong *)&gen_txq,
			 0x48000000, 0x4800FFFF))
	return;
    if (!rsp_get_usr_val("TQL address:", (ulong *)&gen_tql,
			 0x48000000, 0x4800FFFF))
	return;

    for (i=0; i<32; i++) {
	sprintf(prompt, "packet byte %d:", i);
	val = bridgepacket[i];
	if (!rsp_get_usr_val(prompt, &val, 0, 255))
	  break;
	bridgepacket[i] = val;
    }
    printf("First 32 bytes of packet:\n");
    for (i=0; i<32; i++)
      printf("%02x", bridgepacket[i]);
    printf("\n");
}

static uchar ip_pkt_data[60] = {
    0x00, 0x00, 0x22, 0x22, 0x22, 0x22,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x08, 0x00,

    0x45, 0x00, 0x00, 0x2E, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x01,
    0x6C, 0xEB, 0x0F, 0x0F, 0x17, 0x63, 0x0F, 0x0F, 0x19, 0x63,
    
    0x12, 0x34, 0x56, 0x78, 0x9A, 0xbc, 0xcd, 0xef,
    0x12, 0x34, 0x56, 0x78, 0x9A, 0xbc, 0xcd, 0xef,
    0x12, 0x34, 0x56, 0x78, 0x9A, 0xbc, 0xcd, 0xef,
};

static void rsp_ip_packet_setup (void)
{
    int i;
    ulong val;
    char prompt[64];
    
    if (!rsp_get_usr_val("Packets to send:", &packet_count, 1, 1000000000))
	return;
    if (!rsp_get_usr_val("Packet length:", &genlen, 16, 1500))
	return;
    if (!rsp_get_usr_val("TQL address:", (ulong *)&gen_tql,
			 0x48000000, 0x4800FFFF))
      return;
    if (!rsp_get_usr_val("TXQ address:", (ulong *)&gen_txq,
			 0x48000000, 0x4800FFFF))
      return;

    for (i=0; i<40; i++) {
	sprintf(prompt, "packet byte %d:", i);
	val = ip_pkt_data[i];
	if (!rsp_get_usr_val(prompt, &val, 0, 255))
	  break;
	ip_pkt_data[i] = val;
    }
    printf("First 40 bytes of packet:\n");
    for (i=0; i<40; i++)
      printf("%02x", ip_pkt_data[i]);
    printf("\n");
}

static void rsp_ip_packet_generator (void)
{
    leveltype	SR;
    ulong cnt, sent_cnt = 0, len = genlen, i;
    boolean sent;

    for (cnt=0; cnt<packet_count; cnt++) {
	
	SR = raise_interrupt_level(NETS_DISABLE);
	sent = rsp_generate_packet((uchar *)ip_pkt_data,len,gen_tql,gen_txq);
	reset_interrupt_level(SR);
	if (sent) {
	    sent_cnt++;
	} else {
	    process_suspend();
	}
    }
    printf(" Sent %d ip packets out of %d tries\n", sent_cnt, cnt);
    printf("Last memd packet:\n");
    for (i=0; i<40; i++)
      printf("%02x", txptr[i]);
    printf("\n");
}

static boolean get_prefix_and_length (ipaddrtype *prefix, ulong *bits)
{
    ulong address = 0;	         /* build address here */
    ulong length = 0;
    char buff[RSPTST_BUFLEN], *ptr = buff;
    boolean parsed;

    printf("IP prefix/bits [%i/%d]: ", *prefix, *bits);
    parsed = rdtty(buff, RSPTST_BUFLEN-1);
    if (!parsed || rsp_test_abort())
      return(FALSE);
    if (null(ptr))
      return(TRUE);
    ptr = deblank(ptr);	/* flush leading blanks */
    if ((ptr = octet(ptr,&address,'.')) &&
	(ptr = octet(ptr,&address,'.')) &&
	(ptr = octet(ptr,&address,'.')) &&
	(ptr = octet(ptr,&address,'/')) &&
	(octet(ptr,&length,'\0') || octet(ptr,&length,' '))) {
	
	*prefix = (ipaddrtype) address;
	*bits = length;
	return(TRUE);		/* good IP prefix/bits */
    }
    printf("bad address\n");
    return(FALSE);
}
#endif /*DFS_TEST*/


static ulong rsp_dfs_out_index = 32;
static ulong rsp_dfs_slot = 0;
static uchar rsp_dfs_mac[24] = { 0x00, 0x00, 0x0c, 0x02, 0xEC, 0x30,
				   0x00, 0x03, 0x16, 0x1D, 0x26, 0x20,
				    0x08, 0x00,
				};
static ulong rsp_dfs_mac_len = 14; /* ethernet arpa */
static ipaddrtype rsp_dfs_prefix = 0x0F0F1563; /* 15.15.21.99 */
static ipaddrtype rsp_dfs_mask = 0xFFFFFF00;
static ulong rsp_dfs_bits = 24;
static ulong rsp_dfs_ip_mtu = 1500;

static void rsp_vip_dfs_setup (void)
{
    int i;
    ulong val;
    char prompt[64];
    
    if (!rsp_get_usr_val("Output mci_index:", &rsp_dfs_out_index, 0, 159))
      return;

    if (!rsp_get_usr_val("DFS slot:", &rsp_dfs_slot, 0, 10))
      return;

    if (!rsp_get_usr_val("Output mac_len:", &rsp_dfs_mac_len, 4, 24))
      return;

    if (!rsp_get_usr_val("IP prefix (sorry, hex):", &rsp_dfs_prefix, 1,
			 0xFFFFFFFF))
      return;

    if (!rsp_get_usr_val("IP mask:", &rsp_dfs_mask, 0xF0000000, 0xFFFFFFFF))
      return;

    if (!rsp_get_usr_val("IP prefix bits:", &rsp_dfs_bits, 1, 32))
      return;

    if (!rsp_get_usr_val("IP mtu:", &rsp_dfs_ip_mtu, 512, 4470))
      return;

    for (i=0; i<32; i++) {
	sprintf(prompt, "mac byte %d:", i);
	val = rsp_dfs_mac[i];
	if (!rsp_get_usr_val(prompt, &val, 0, 255))
	  break;
	rsp_dfs_mac[i] = val;
    }
    printf("Mac encaps:\n");
    for (i=0; i<rsp_dfs_mac_len; i++)
      printf("%02x", rsp_dfs_mac[i]);
    printf("\n");
}


static ulong dfs_gen_count = 10;
static void rsp_vip_dfs_generate (void)
{
    int i;
    ulong elapsed;
    sys_timestamp start;
    
    if (!rsp_get_usr_val("Number of dfs messages to send",
			 &dfs_gen_count, 0, 1000000))
      return;

    GET_TIMESTAMP(start);
    for (i=0; i<dfs_gen_count; i++) {
	reg_invoke_auto_sw_ip_cache_update(NULL, NULL, 0);
    }
    elapsed = ELAPSED_TIME(start);
    if (elapsed) {
	printf("%d attempts in %d.%03d seconds, or %d per second\n",
	       dfs_gen_count, elapsed / ONESEC, elapsed % ONESEC,
	       (dfs_gen_count * ONESEC)/ elapsed);
    } else {
	printf("%d attempts.  Elapsed time too small to measure (>4ms)\n",
	       dfs_gen_count);
    }
}

static void rsp_get_viper_context (void)
{
    uchar *ptr;
    int i;
    dgtype *dbus;
    sys_timestamp timer, poll_timer;

    if ((!rsp_get_usr_val("Slot number to access",
			 &default_slot, 0, MAX_SLOTS-1)))
	return;

    dbus = pslot_to_dbus(default_slot); 

    dbus_io_reg_write(default_slot, DBUS_IO_DEBUG, 0xff);
	
    GET_TIMESTAMP(timer);

    while (TRUE) {

	GET_TIMESTAMP(poll_timer);

	/* wait for polling time */
	while (ELAPSED_TIME(poll_timer) < (DBUS_SW_POLL_TIME * 8))
	    ;

	/* check if IP is done with setup */
	if (dbus_prot_rd(&dbus->diag_status) & DBUS_SW_READY) {
	    break;
	}

	/* check if we have timed out */
	if (ELAPSED_TIME(timer) > DBUS_SW_MAXDELAY) {
	    return;
	}
    }

    ptr = malloc(DBUS_CRASH_LEN);
    if (ptr == NULL) {
	printf(nomemory);
	return;
    }

    for (i = 0; i< DBUS_CRASH_LEN; i++) {

	ptr[i] = dbus_io_reg_read(default_slot, DBUS_IO_DEBUG);

	if (dbus_prot_rd(&dbus->diag_status) & DBUS_INTERNAL_ERROR) {
	    free(ptr);
	    return;
	}
    }
    vip_print_context("", ptr);

    free(ptr);
}

static void rsp_test_ucode_reload_slot (ulong slot_num)
{
    slottype* sp;

    sp = &slots[slot_num];
    if (sp->flags & BOARD_NONEXIST) {
	printf("\n%% slot %u empty", slot_num);
	return;
    }

    disable_slot(slot_num);
    dbus_hard_ctrlr_reset(pslot_to_dbus(slot_num));
    dbus_load_ucode(slot_num);
    dbus_slot_enable(pslot_to_dbus(slot_num));

    return;
}
static void rsp_test_ucode_reload (void)
{
    if (!confirm("Reload microcode on selected slot?"))
	return;

    rsp_test_ucode_reload_slot(default_slot);
}


#ifdef RSP_QATEST
/*
 * QA/Memd test: wait for buffer from IPs in a given queue
 */
static bufhdr_t *
rsp_qa_test_dequeue (ulong qa_num, bufhdr_t *bstart, bufhdr_t *bout,
                     bufhdr_t *bend, short lock_mode, ulong first_q, ulong nqs)
{
    bufhdr_t *bin, *btmp;
    qa_hwq_t *qptr;
    long      t;
    static    int test_peek = 0;

    qptr = queue_ptr(qa_num);

    test_peek++;

    bin = NULL;
    for (t = 500000; ; --t) {

	if (!t) {
	    /*
	     * time expired - try a dequeue for the hell of it
	     */
            bin = hwq_dequeue_inline(qptr);
	    if (!bin) {
                printf( "\nQ test: NULL buffer dequeued from qnum %#x at "
                    "%#x-%#x-%#x   lock_mode %#x", qa_num, bstart, bout,
		    bend, lock_mode);
                return (NULL);
	    } else {
                printf( "\nQ test peek failure from qnum %#x at "
                    "%#x-%#x-%#x: peek: NULL  read: 0x%08x   lock_mode %#x",
                    qa_num, bstart, bout, bend, bin, lock_mode);
		return (NULL);
	    }
        }

	if (test_peek & 2) {
            bin = hwq_peek_inline(qptr);
            if (bin) {
                btmp = hwq_dequeue_inline(qptr);
                if (bin != btmp) {
                    printf( "\nQ test peek mismatch from qnum %#x at "
                    "%#x-%#x-%#x: peek: 0x%08x read: 0x%08x   lock_mode %#x",
                    qa_num, bstart, bout, bend, bin, btmp, lock_mode);
                    return (NULL);
                }
                break;
            }
	} else {
            bin = hwq_dequeue_inline(qptr);
	    if (bin)
                break;
	}
        usecdelay(1);
    }
    if ( lock_mode == 1) {
        if (bin != bout) {
           printf( "\nQ test mismatch from qnum %#x at "
		"%#x-%#x-%#x: tx: 0x%08x rx: 0x%08x   lock_mode %#x",
		qa_num, bstart, bout, bend, bout, bin, lock_mode);
                return (NULL);
        }
    } else {
    	if (bin < bstart || bin >= bend || 
    	      (lock_mode != 0x7000 && 
	      ((((bin-bstart)*2)%nqs)+first_q+1) != qa_num)) {

            printf( "\nQ test non-exclusive error from qnum %#x on "
	    "%#x-%#x-%#x  expected Q %#x  lock_mode %#x",
	    qa_num, bstart, bin, bend,
	    ((((bin-bstart)*2)%nqs)+first_q+1), lock_mode);
            return (NULL);
        }
    }
    return (bin);
}

/*
 * test r/w of qa diag space
 */
static int test_diags (bufhdr_t *bmin, bufhdr_t *bmax,
		    ulong first_q, ulong last_q)
{
	bufhdr_t *buf, *bufout;
	uint bcurr, bnext, bhead, btail;
	ulong   qnum;
	int	nelements;

	for (qnum = first_q; qnum < last_q; qnum++) {

            for (buf = bmin; buf < bmax; buf++) {
                hwq_enqueue_inline(queue_ptr(qnum), buf);
	    }

	    btail = ADDR_TO_MEMD_OFFSET_16((bmax-1));
	    bcurr = qa_read_tail((ushort)qnum);
	    if (bcurr != btail) {
		printf( "\nQ diag tail mismatch  qnum %#x "
			   "bufs %#x-%#x: expect: 0x%04x actual: 0x%04x",
			   qnum, bmin, bmax, btail, bcurr);
		return(1);
	    }

	    bhead = ADDR_TO_MEMD_OFFSET_16(bmin);
	    bcurr = qa_read_head((ushort)qnum);

	    if (bcurr != bhead) {
		printf( "\nQ diag head mismatch  qnum %#x "
			   "bufs %#x-%#x: expect: 0x%04x actual: 0x%04x",
			   qnum, bmin, bmax, bhead, bcurr);
			   return(1);
	    }

	    
            nelements = QA_NQ_ELEMENTS;
            for (bcurr = bhead; ; bcurr += 0x10) {

		if (!qa_read_lsa(bcurr)) {
		    printf( "\nQ diag for qnum %#x  "
			   "bufs %#x-%#x: element %#04x lsa not in use",
			   qnum, bmin, bmax, bcurr);
			   return(1);
		}

		bnext = qa_read_body(bcurr);

		if (bnext != (bcurr+0x10)) {

		    if (bcurr == btail) {

		        if (!bnext)
			    break;		/* end of list */

		        printf( "\nQ diag for qnum %#x  "
			   "bufs %#x-%#x: tail %#04x points to %#04x",
			   qnum, bmin, bmax, bcurr, bnext);
		    } else {
		        printf( "\nQ diag for qnum %#x  "
			   "bufs %#x-%#x: buf %#04x points to %#04x",
			   qnum, bmin, bmax, bcurr, bnext);
		    }
		    return(1);
		}

		if (!nelements--) {
		    printf( "\nQ diag  q too long: qnum %#x  "
			   "bufs %#x-%#x: last: 0x%04x",
			   qnum, bmin, bmax, bcurr);
			   return(1);
		}
	    }

            for (bufout = buf = bmin; ; buf++) {

	        if (buf >= bmax) {
		    buf = 0;
                    bufout = hwq_dequeue_inline(queue_ptr(qnum));
		    break;
	        }

                bufout = hwq_dequeue_inline(queue_ptr(qnum));
		if (bufout != buf)
		    break;
	    }

	    if (bufout != buf) {

		    printf( "\nQ diag readback mismatch  qnum %#x "
			   "bufs %#x-%#x: expect: %#x actual: %#x",
			   qnum, bmin, bmax, buf, bufout);
			   return(1);
	    }
	}

	printf( "\nQA diags test OK for queues %#x-%#x bufs %#x-%#x\n",
		   first_q, last_q, bmin, bmax);
	return(0);
}

/* Qs for bufs w/data - must agree with ucode */
#define RSP_TEST_DATAQ(q)	((q) >= 0x100 && (q) <= 0x11f)

/*
 * QA/Memd diagnostics test - requires custom IP microcode
 */

void rsp_test_qa_test (void)
{
        static ulong qval_min = MEMD_BUFHDR_OFFSET;
        static ulong qval_max = MEMD_BUFDATA_OFFSET-0x10;
        static ulong first_q = QA_FIRST_QNUM;
        static ulong last_q = QA_LAST_QNUM;
	static ulong rep_count = 1;

        ulong    n_ops, p_ops, first_qa_num, hold_q, qa_num, data;
	bufhdr_t *bmin, *bmax, *bstart, *bend, *bin, *bout;
        qa_hwq_t *qptr, *tmp_qptr;
        qa_acc_t *aptr;
        uchar *p, *bufdata_ptr;
        short lock_mode;
        int buf_size;
        int page_bytes_left;
        int i;
        long t;
        ulong nqs;
        ulong tmp, rep;
        struct slottype_ *slot;
        extern int watchdog_time;
        int orig_watchdog_time = watchdog_time;

        int eip_slots[MAX_SLOTS];
        int neips;

        printf( "\n\nQA/Memd Diagnostic Test"
                "\n*** Requires Test EIP microcode ***"
                "\n*** WARNING: Overwrites QA and Memd ***\n\n");
        flush();


                /*
                 * figure out which slots have eips
                 */
        neips = 0;

        FOR_ALL_NONEMPTY_SLOTS(slot) {
                if (slot->ctrlr_type == FCI_ETHER_CONTROLLER)
                        eip_slots[neips++] = slot->slot_num;
        }

        if (!neips) {
                printf( "\nError: No EIPs found in system\n");
                return;
        }
                                         
        /*
         * get test queues
         * must be at least 2 qs per IP for attentions
         */
        if (!rsp_get_usr_val("Enter starting Q:", &first_q, QA_FIRST_QNUM,
                        QA_LAST_QNUM + 1 - (neips*2) ))
                return;

        if (!rsp_get_usr_val("Enter last Q:", &last_q,
                              first_q + (neips*2) - 1, QA_LAST_QNUM))
                return;

        if( !((last_q - first_q)&1))
                last_q--;                        /* nqs must be even */
        nqs = last_q + 1 - first_q;

        /*
         * get test bufhdrs
         */
        tmp = default_qval;
        default_qval = qval_min;

        if (!qa_get_q_val("Enter starting bufhdr"))
                return;

        qval_min = default_qval;
        bmin = (bufhdr_t *)BUFHDR_OFFSET_TO_ADDR(default_qval);

        default_qval = qval_max;
        if (!qa_get_q_val("Enter last bufhdr"))
                return;

        qval_max = default_qval;
        bmax = (bufhdr_t *)BUFHDR_OFFSET_TO_ADDR(default_qval+0x10);

        default_qval = tmp;


        if (!rsp_get_usr_val("Enter rep count:", &rep_count, 1, (~0)>>1))
                return;

        rsp_qa_init();

        memd_array.alow = memd_array.amin;
        memd_array.ahi = memd_array.amax;
        memd_array.fill = 0;
        rsp_test_fill(&memd_array);

	/*
	 * start ucode w cleared memd
	 */
	for (i = 0; i < neips; i++ ) {
            rsp_test_ucode_reload_slot(eip_slots[i]);
	}

        rsp_qa_init();	/* for some reason this is needed after ucode relo */

	if (test_diags(bmin, bmax, first_q, last_q)) {

            hwacc_peek(acc_ptr(0x3fe));	/* just to trigger logic analyzer */

            if (!confirm("\nContinue?"))
	        return;
        }

        rsp_qa_init();

        /*
         * pass first_q addr and nqs/2 to ucode
         */
        bufdata_ptr = (uchar *)MEMD_OFFSET_TO_ADDR(MEMD_BUFDATA_OFFSET-4);

        *((ushort *)bufdata_ptr)++ = (first_q * QA_QSIZE)/2;
        *((ushort *)bufdata_ptr) = nqs/2;
        
	for (i = 0; i < neips; i++ ) {
            rsp_test_ucode_reload_slot(eip_slots[i]);        /* restart ucode */
	}

        rsp_qa_init();	/* for some reason this is needed after ucode relo */

        printf( "\n\nTesting buffer header %#x through %#x"
                "\nTesting queues %#x through %#x\n",
                bmin, bmax-1, first_q, last_q );
        flush();


        psion(stdio);
        automore_disable();

        bstart = bend = bmin;
        qa_num = first_q;
        p_ops = n_ops = 0;
        watchdog_time = 0;

        for ( rep = rep_count; rep > 0; rep--) {

                /*
                 * start at each buf hdr
                 */
          for (bstart = bend = bmin; bstart < bmax; bstart++ ) {

                /*
                 * end at each buf hdr
                 */
            for (bend = bstart+1; bend <= bmax; bend++) {

                if (psipending())
                   goto done;

                if ( n_ops >= p_ops) {
                        p_ops += 500000;
                        printf("%8d  %8d  %8d  %8d\n", rep,
                                    ((bmax-bstart)*100)/(bmax-bmin),
                                    bstart == bmax ? 0 :
                                    ((bmax-bend)*100)/(bmax-bstart), n_ops);
                        flush();
                }
               
                        /*
                         * alternate lock contention mode
                         */
                 lock_mode = ((ulong)bend & 0x10) ? 1 : 0x8000;

                        /*
                         * IPs read from even qs write back to next odd q
                         * even/odd is relative to first_q
                         * odd accs are incr by IPs when they enqueue a bufhdr
                         * even accs used by IPs for lock contention
                         *   if init to 1 data will come back in order
                         *   if init to > 1 multiple IPs will service
                         *     same queue - queue data may be out of order
                         */
                for(qa_num = first_q, i = 0; qa_num <= last_q; qa_num += 2, i++)
                {

                                /*
                                 * set attention to IPs in the system
                                 * for the queues
                                 */
                        set_qa_ip_attention( qa_num, eip_slots[i%neips]);

                                /*
                                 * set locking mode and start count
                                 */
                        hwacc_set(acc_ptr(qa_num), lock_mode);
                        hwacc_set(acc_ptr(qa_num+1), 0x8000);
                }

                        /*
                         * set flags that indicate IPs are done
                         * 0x3ff can't be used for input q
                         */
                hwacc_set(acc_ptr(0x3ff), 0x8000);

                
                /*
                 * hammer on one queue test -
                 * need to init multiple qs for attns
                 */

                first_qa_num = qa_num = last_q+1-(neips*2);
                if (last_q < QA_LAST_QNUM)
                        hold_q = QA_LAST_QNUM;
                else if (first_q == QA_FIRST_QNUM)
                        hold_q = QA_FIRST_QNUM + 1;  /* hold_q should be "odd"*/
                else
                        hold_q = QA_FIRST_QNUM;

                if ((bend - bstart) >= (neips + 16) &&
                    !RSP_TEST_DATAQ(hold_q) &&
                    !RSP_TEST_DATAQ(first_qa_num) &&
                    !RSP_TEST_DATAQ(last_q) ) {

                    qptr = queue_ptr(last_q-1);
                    bout = bstart;
                    qa_num = first_qa_num;

                    for (i = 0; i < neips-1; i++, qa_num+=2) {
                        hwq_enqueue_inline(queue_ptr(qa_num), bout++);
                        hwacc_sub_inline(acc_ptr(qa_num+1), 1);
                    }

                    while ((bend - bout) >= 8) {
                       hwq_enqueue_inline(qptr, bout++);
                       hwq_enqueue_inline(qptr, bout++);
                       hwq_enqueue_inline(qptr, bout++);
                       hwq_enqueue_inline(qptr, bout++);
                       hwq_enqueue_inline(qptr, bout++);
                       hwq_enqueue_inline(qptr, bout++);
                       hwq_enqueue_inline(qptr, bout++);
                       hwq_enqueue_inline(qptr, bout++);
                    }
                    while (bout < bend)
                       hwq_enqueue_inline(qptr, bout++);

                    if (lock_mode != 1 && !((ulong)bend & 0x60)) {

                        /*
                         * (occasionally) test RP and IPs both
                         * reading or both writing same q
                         * do what IPs do - only in non-exclusive mode
                         */
                        aptr = acc_ptr(last_q);
                        tmp_qptr = queue_ptr(last_q);
                        for (;;) {
                            bin = hwq_dequeue_inline(qptr);
                            if (!bin)
                                break;
                            hwq_enqueue_inline(tmp_qptr, bin);
                            hwacc_sub_inline(aptr, 0xffff);
                        }
                    }

                    if (lock_mode == (short)0x8000)
                        lock_mode = (short)0x7000; /* skip non-ex chk */

                    /*
                     * verify
                     */
                    qa_num = first_qa_num+1;
                    qptr = queue_ptr(hold_q);
                    bout = bstart;

                    for (i = 0; i < neips-1; i++, qa_num+=2) {

                        bin = rsp_qa_test_dequeue(qa_num, bstart, bout++, bend,
                                                  lock_mode, first_q, nqs);
                        if (!bin)
                                goto error;
                        hwq_enqueue_inline(qptr, bin);
                    }

                    while (bout < bend) {
                        bin = rsp_qa_test_dequeue(last_q, bstart, bout++,
                                              bend, lock_mode, first_q, nqs);
                        if (!bin)
                                goto error;
                        hwq_enqueue_inline(qptr, bin);
                    }

                    /*
                     * everything requeued to hold q should guarantee
                     * all bufhdrs are unique else hw will complain
                     * and fall within correct range
                     */
                    for (bout = bstart; bout < bend; bout++) {
                        bin = rsp_qa_test_dequeue(hold_q,
                                 bstart, bout, bend, lock_mode, first_q, nqs);
                        if (!bin)
                                goto error;
                    }
                    
                    hwacc_sub_inline(acc_ptr(last_q), (bend-bstart)-neips+1);

                    if (lock_mode == (short)0x7000)
                        lock_mode = (short)0x8000;
                }

                        /*
                         * enqueue hdrs from bstart to bend
                         * on "even" queues
                         * buffers to RSP_TEST_DATAQ also get actual data
                         */

                bufdata_ptr =
                        (uchar *)MEMD_OFFSET_TO_ADDR(MEMD_BUFDATA_OFFSET);

                qa_num = first_q;
                for (bout = bstart; bout < bend; bout++, qa_num+=2) {

                   if (qa_num >= last_q)
                       qa_num = first_q;

                   if (RSP_TEST_DATAQ(qa_num)) {

                        /*
                         * init buffer data - vary data size (should be longs)
                         * don't span page boundaries
                         * size should be lword multiple
                         */
                        buf_size = ((bend-bstart)+(bend-bout)) & 0xfc;
                        page_bytes_left = 0x1000 - ((ulong)bufdata_ptr%0x1000);
                        if (buf_size+4  > page_bytes_left)
                                bufdata_ptr += page_bytes_left;
                        bout->bf_tx_bytes = buf_size;
                        bout->bf_rx_bytes = buf_size;
                        bout->bf_buffer_ptr=ADDR_TO_MEMD_OFFSET_24(bufdata_ptr);
                                /*
                                 * data is hdr offset
                                 */
                        data = ((uint)bout&0xffff) | (((uint)bout&0xffff)<<16);
                        for( i = 0; (i+32) < buf_size; i += 32) {
                            *((ulong *)bufdata_ptr)++ = data;
                            *((ulong *)bufdata_ptr)++ = data;
                            *((ulong *)bufdata_ptr)++ = data;
                            *((ulong *)bufdata_ptr)++ = data;
                            *((ulong *)bufdata_ptr)++ = data;
                            *((ulong *)bufdata_ptr)++ = data;
                            *((ushort *)bufdata_ptr)++ = data;
                            *((ushort *)bufdata_ptr)++ = data;
                            *((ulong *)bufdata_ptr)++ = data;
                        }

                        for( ; i < buf_size; i += sizeof(ushort))
                            *((ushort *)bufdata_ptr)++ = data;

                        *((ushort *)bufdata_ptr)++ = 0xeeee; /* overrun mark */
                        *((ushort *)bufdata_ptr)++ = 0xffff; /* overrun mark */
                        buf_size += 4;
                                /*
                                 * flush data out of cache
                                 * shouldn't need to flush hdr
                                 * it is write-thru
                                 */
                        writeback_inval_data_inline( bufdata_ptr-buf_size,
                                  buf_size);
                        bufdata_ptr += R4K_DLINE_SHIFT;
                   }


                   qptr = queue_ptr(qa_num);
                   aptr = acc_ptr(qa_num+1);

                           /*
                         * decr corresponding acc (both ways)
                         */
                        /*
                         * want QA ops to follow each other with min delay
                         */
                   if ((ulong)bout & 0x10) {
                       hwq_enqueue_inline(qptr, bout);
                       hwacc_read_dec_inline(aptr);
                   } else {
                       hwq_enqueue_inline(qptr, bout);
                       hwacc_sub_inline(aptr, 1);
                   }
                }

                if (psipending())
                   goto done;

                   /*
                    * check results
                    */
                bufdata_ptr =
                        (uchar *)MEMD_OFFSET_TO_ADDR(MEMD_BUFDATA_OFFSET);

                qa_num = first_q+1;
                for (bout = bstart; bout < bend; bout++, qa_num+=2 ) {

                        /*
                         * expect IP(s) put bufhdrs back on next higher queue
                         */
                    if (qa_num > last_q)
                        qa_num = first_q+1;

                    bin = rsp_qa_test_dequeue(qa_num, bstart, bout, bend,
                                                  lock_mode, first_q, nqs);
                    if (!bin)
                        goto error;

                    if (RSP_TEST_DATAQ(qa_num-1)) {

                        data = ((uint)bout&0xffff) | (((uint)bout&0xffff)<<16);

                            if ( lock_mode != 1) {
                                bufdata_ptr = (uchar *)
                                        MEMD_OFFSET_TO_ADDR(bin->bf_buffer_ptr);
                                data = ((uint)bin&0xffff) |
                                       (((uint)bin&0xffff)<<16);
                        }

                        data += 0x10001;        /* IP's mod data +1 */

                                /*
                                 * check data should've been incremented
                                 * by IPs
                                 */
                        buf_size = ((bend-bstart)+(bend-bin)) & 0xfc;
                        page_bytes_left = 0x1000 - ((ulong)bufdata_ptr%0x1000);
                        if (buf_size+4  > page_bytes_left)
                                bufdata_ptr += page_bytes_left;

                                /*
                                 * inval buffer here - could have sucked
                                 * in the start of buffer with previous buffer
                                 * read before it was ready
                                 */
                        for (p = (uchar *)
                                 ((ulong)bufdata_ptr & ~(R4K_DLINE_SHIFT-1));
                             p < bufdata_ptr+buf_size+4;
                             p += R4K_DLINE_SHIFT)
                             inval_buf_hdr_inline((bufhdr_t *)p);


                        for( i = 0; i < buf_size; i += sizeof(ulong)) {
                                if( *(ulong *)bufdata_ptr != data) {
                                   
                                        printf( "\nQ Data mismatch at "
                                          "%#x-%#x-%#x: bufadr: 0x%08x "
                                          "size %#x  error at %#x "
                                          " tx: 0x%08x  rx: 0x%08x",
                                          bstart, bin, bend,
                                          bin->bf_buffer_ptr, buf_size,
                                          bufdata_ptr, data,
                                          *(ulong *)bufdata_ptr);
                                                goto error;
                                 }
                                 ((ulong *)bufdata_ptr)++;
                         }
                         if (*(ushort *)bufdata_ptr != 0xeeee ||
                              *++(ushort *)bufdata_ptr != 0xffff) {

                                printf( "\nQ Data overrun at "
                                          "%#x-%#x-%#x: bufadr: 0x%08x "
                                          "size %#x value %#x",
                                          bstart, bin, bend,
                                          bin->bf_buffer_ptr,
                                          buf_size, *(ushort *)bufdata_ptr);
                                       goto error;
                         }
                         ((ushort *)bufdata_ptr)++;
                         bufdata_ptr += R4K_DLINE_SHIFT;
                   }
                   n_ops++;
               }

                        /*
                         * ips do psuedo random delays between ops - wait
                         */
                for (t = 50000; t; t--) {

                    if (hwacc_peek(acc_ptr(0x3ff))==(short)0x8000)
                            break;        /* IPs all done */

                    usecdelay(1);
                }
                  
                for(qa_num = first_q; qa_num < last_q; qa_num += 2) {

                        if (hwacc_peek(acc_ptr(qa_num)) != lock_mode) {
                            printf( "\nACC lock error on "
                                "%#x for %#x-%#x: tx: 0x%08x  rx: 0x%08x",
                                qa_num, bstart, bend, lock_mode,
                                hwacc_peek(acc_ptr(qa_num)) );
                            goto error;
                        }
                        if (hwacc_peek(acc_ptr(qa_num+1))!=(short)0x8000) {
                             printf( "\nACC count error on "
                                "%#x for %#x-%#x: rx: 0x%08x",
                                qa_num+1, bstart, bend,
                                hwacc_peek(acc_ptr(qa_num+1)));
                             goto error;
                        }
                }
            }
          }
	}
        goto done;

error:

        hwacc_peek(acc_ptr(0x3fe));


done:
        watchdog_time = orig_watchdog_time;

        printf("\nEnd of test"
                "\nbmin %#x   bstart %#x   bend %#x   bmax %#x"
                "\nfirst q %#x   qa num   %#x   last q %#x\n",
                bmin, bstart, bend, bmax,
                first_q, qa_num, last_q );

        printf("\n%8d  %8d  %8d\n", ((bmax-bstart)*100)/(bmax-bmin),
                                    bstart == bmax ? 0 :
                                    ((bmax-bend)*100)/(bmax-bstart), n_ops);
        psioff(stdio);
        return;

}
#endif /* RSP_QATEST */


/*======================================================================
 *
 *			AIP specific routines
 *
 *======================================================================*/

static ulong aip_tx_proc;

/*
 * Select the processor which we are interested
 */
static void aip_select_processor (void)
{
    ulong rsp;

    aip_tx_proc = 1;
    if (rsp_get_usr_val("Processor to select (tx:1,rx:0)", &aip_tx_proc,
	0, 1)) {
	if (rsp_test_send_ipcmd(CCB_CMD_AIP_PROC_SELECT, aip_tx_proc, 0, &rsp))
	    printf("\ncommand successful\n");
	else
	    printf("\ncommand failed\n");
    }
}

/*
 * Read the selected processor's memory 
 */
static void aip_read_processor_memory (void)
{
    leveltype level;
    ulong addr, mem_val;
    ulong i, j;
    ulong rsp;
    ulong fail;
 
    static ulong rd_addr;
    static ulong dump_len;

    if (rsp_get_usr_val("Memory read address", &rd_addr, 0, 0xFFFFFFFF) &&
	rsp_get_usr_val("number of words", &dump_len, 1, 0x400)) {

	if (!rsp_test_send_ipcmd(CCB_CMD_AIP_ADDR_SELECT, rd_addr,
	    rd_addr >> 16, &rsp)) {
	    printf("\ncommand failed\n");
	    return;
	}

	automore_enable(NULL);
	for (i=j=0,addr=rd_addr; i < dump_len; i++,addr += 4) {
	    level = raise_interrupt_level(NETS_DISABLE);
	    fail = FALSE;	
	    if (!aip_tx_proc) {
		/* In the tx processor, the read address will automatically
		 * be increased by 4 after each read. In the rx processor, 
		 * the address increment is not done to speed up the response
		 * time. So here we only increase the reading address to rx
		 * processor.
		 */
		fail = fail || !rsp_test_send_ipcmd(CCB_CMD_AIP_ADDR_SELECT,
		    addr, addr >> 16, &rsp);
	    }

	    /* Perform the read. */
	    fail = fail || !rsp_test_send_ipcmd(CCB_CMD_AIP_MEM_READ, 0, 0,
		&mem_val);

	    if (!aip_tx_proc) {
		/* To read stuff out from rx processor, the AIP receive
		 * processor will not return the result to the RP directly.
		 * Instead, it puts the result in the dual port memory on
		 * the AIP board. Therefore we need an additional command
		 * to inform the AIP tx processor to read it out from the
		 * dual port memory and return the result.
		 * 75 is the command arg to do that.
		 */
		fail = fail || !rsp_test_send_ipcmd(CCB_CMD_AIP_DEBUG_GETRX,
		    0, 0, &mem_val);
	    }

	    reset_interrupt_level(level);

	    if (fail) {
		printf("\ncommand failed\n");
		automore_disable();
		return;
	    }

	    if (j == 0)
		printf("%08x:", addr);
	    printf(" %08x",mem_val);
	    if (++j == 4) {
		printf("\n");
		j = 0;
	    }
	}
	printf("\n");
	automore_disable();
    }
}

/*
 * Write the selected processor's memory 
 */
static void aip_write_processor_memory (void)
{
    leveltype level;
    ulong rsp;
    ulong fail;
 
    static ulong wr_addr;
    static ulong wr_data;

    if (rsp_get_usr_val("Memory write address", &wr_addr, 0, 0xFFFFFFFF) &&
	rsp_get_usr_val("Memory write data", &wr_data, 0, 0xFFFFFFFF)) {

	level = raise_interrupt_level(NETS_DISABLE);

	fail = (!rsp_test_send_ipcmd(CCB_CMD_AIP_ADDR_SELECT, wr_addr,
	    wr_addr >> 16, &rsp) || !rsp_test_send_ipcmd(CCB_CMD_AIP_DEBUG_WR,
	    wr_data, wr_data >> 16, &rsp));

	reset_interrupt_level(level);

	if (fail)
	    printf("\ncommand failed\n");
	else
	    printf("\ncommand successful\n");
    }
}

/*======================================================================
 *
 *			FSIP specific routines
 *
 *======================================================================*/

#define SIP_MAX_ADDRESS   0xd0

/*
 * Read the selected interface's sip memory 
 */
static void fsip_dump_sip_memory (void)
{
    
    leveltype level;
    ushort i, sdata;
    ulong rsp, ldata, fail;
    static ulong interface_num = 0;

    if (rsp_get_usr_val("relative interface of fsip: " ,
			&interface_num, 0, 7)) {
	
	level = raise_interrupt_level(NETS_DISABLE);

	fail = (!rsp_test_send_ipcmd(CCB_CMD_SELECT, interface_num, 0, &rsp) 
		|| !rsp_test_send_ipcmd(CCB_CMD_FSIP_SIP_ADDR, 0, 0, &rsp));
	    
	reset_interrupt_level(level);

	if (fail) {
	    printf("\ncommand failed\n");	
	    return;
	}

	for (i=0; i < SIP_MAX_ADDRESS; i+=2) {    
	
	    level = raise_interrupt_level(NETS_DISABLE);

	    fail = (!rsp_test_send_ipcmd(CCB_CMD_FSIP_SIP_READ, 0, 0, &ldata));
	    
	    reset_interrupt_level(level);
	
	    if (fail) {
		printf("\ncommand failed\n");	
		return;
	    }
	       
	    sdata = (short)ldata;
	    if ((i % 16) == 0)
		printf("sip%d+%04x:  %04x", interface_num, i, sdata);
	    else if ((i % 8) == 0)
		printf("  %04x", sdata);
	    else if ((i % 16) == 14)
		printf(" %04x\n", sdata);
	    else
		printf(" %04x", sdata);
	}
	printf("\n");    
    } 
}


/*======================================================================
 *
 *			diag menu stuff
 *
 *======================================================================*/

static char rsp_test_helpcmd[] = "?";

struct rsp_test_cmds {
        char *name;		/* NULL name == end of menu */
        char *help;
        void (*fun)(void);	/* NULL command == exit menu */
};

/*
 * show command help
 */
static void rsp_test_help (struct rsp_test_cmds menu[])
{
        int	i;

        printf( "\n");
        for (i = 0; menu[i].name; i++)
        	printf( "%-5s - %s\n", menu[i].name, menu[i].help);
}


/*
 * get usr command from menu and execute
 */
static void rsp_test_do_menu (struct rsp_test_cmds menu[], char *prompt)
{
    char buff[RSPTST_BUFLEN], *command;
    boolean parsed;
    int i;
    
    command = rsp_test_helpcmd;

    for(;;) {
        int match_count, nmatches, match_index;

        memset(buff, 0, RSPTST_BUFLEN);
        printf("\n%s (%s for help) [%s]: ", prompt, rsp_test_helpcmd, command);
        parsed = rdtty(buff, RSPTST_BUFLEN-1);

        if (rsp_test_abort())
            return;

        if (!null(buff) && parsed) {
    		command = buff;
        	while (isspace(*command))
        	    ++command;
        }

        for (i = 0; buff[i]; i++)
        	buff[i] = tolower(buff[i]);

        if( !strncmp(rsp_test_helpcmd, command, strlen(rsp_test_helpcmd))) {
        	automore_enable(NULL);
        	rsp_test_help(menu);
        	automore_disable();
    		command = rsp_test_helpcmd;
        	continue;
        }
        							 
        match_count = 0;
        nmatches = 0;
        match_index = -1;
        for (i = 0; ; i++) {
        	int count;
        	char *cmdp, *namp;

        	if (!menu[i].name) {
        		if (nmatches > 1) {
        		    printf("\nAmbiguous command '%s'.", command);
        		    command = rsp_test_helpcmd;
        		    break;
        		}
        		if (nmatches == 1) {
        		    i = match_index;

exact_match:		    if (menu[i].fun) {
        			command = menu[i].name;
        			automore_enable(NULL);
        			(*menu[i].fun)();
        			automore_disable();
        			break;
        		    }
        		    else
        			return;
        		}
        		printf("\nUnknown command '%s'.  "
        		       "Type '%s' for help",command,rsp_test_helpcmd);
        		command = rsp_test_helpcmd;
        		break;
        	}

        	cmdp = command;
        	namp = menu[i].name;
        	/*
        	 * match input string against command name in table
        	 */
        	while (*cmdp) {
        	    if (*cmdp != *namp)
        		break;
        	    ++cmdp;
        	    ++namp;
        	}
        	/*
        	 * trailing whitespace in input is the same as end-of-string
        	 * while remaining non-whitespace in input means no match
        	 */
        	if (isspace(*cmdp))
        	    *cmdp = '\0';
        	else
        	    if (*cmdp)
        		continue;

        	/*
        	 * end of both strings means exact match
		 */
		if (*cmdp == *namp)
		    goto exact_match;

		/*
		 * count matched characters
		 */
		count = cmdp - command;
		if (count == 0)
		    continue;

		/*
		 * bump match counter if same length as previous match,
		 * or remember this entry if this is the longest match
		 */
		if (count == match_count)
		    ++nmatches;
		else if (count > match_count) {
		    match_count = count;
		    nmatches = 1;
		    match_index = i;
		}
	}
    }
}


struct rsp_test_cmds rsp_test_fsip_menu[] =
{
  { "q", "exit", NULL },
  { "m", "dump formatted sip memory", fsip_dump_sip_memory },
  { NULL, NULL, NULL }
};

struct rsp_test_cmds rsp_test_fip_menu[] =
{
  { "q", "exit", NULL },
  { NULL, NULL, NULL }
};

struct rsp_test_cmds rsp_test_trip_menu[] =
{
  { "q", "exit", NULL },
  { NULL, NULL, NULL }
};

struct rsp_test_cmds rsp_test_aip_menu[] =
{
  { "q", "exit", NULL },
  { "r", "Read memory", aip_read_processor_memory },
  { "p", "Processor select (tx:1,rx:0)", aip_select_processor },
  { "w", "Write memory", aip_write_processor_memory },
  { NULL, NULL, NULL }
};

/*
 * IP specific sub menus
 */
static void rsp_test_ip_test (void)
{
    switch( slots[default_slot].ctrlr_type) {
    case FCI_CTR_CONTROLLER:
    	rsp_test_do_menu(rsp_test_trip_menu, slots[default_slot].card_name);
	break;
    case FCI_FDDIT_CONTROLLER:
    	rsp_test_do_menu(rsp_test_fip_menu, slots[default_slot].card_name);
	break;
    case FCI_FSIP_CONTROLLER:
    	rsp_test_do_menu(rsp_test_fsip_menu, slots[default_slot].card_name);
	break;
    case FCI_AIP_CONTROLLER:
    	rsp_test_do_menu(rsp_test_aip_menu, slots[default_slot].card_name);
	break;
    default:
	printf( "\nNo IP specific commands for %s",
		slots[default_slot].card_name);
	break;
    }
}


#ifdef TEST
extern void rsp_test_cause_xbufhdr(void);
extern void rsp_test_cause_invrtn_type(void);
extern void rsp_test_cause_invrtn_hdr(void);
extern void rsp_test_cause_invrtn_bcastid(void);
#endif /* TEST */


#ifdef TEST 
/*
 * cause a QA_DIAG_REUSE0_LINK_ERROR (reuse link, not zero link).
 */
static void rsp_test_cause_reuse_error (void)
{
    bufhdr_t *buf = NULL;
    leveltype level;
    ulong qnum_one;
    ulong qnum_two;

    printf("\nThis function will dequeue a buffer from a specified queue"
	   "\nand then enqueue it onto two other queues.");
    printf("\nFirst enter the number of the queue to dequeue from.");

    if (qa_get_q_num()) {
	buf = hwq_dequeue(queue_ptr(default_qnum));
	printf("0x%08x", buf);
    }

    if (buf != NULL) {
	printf("\nNow enter the number of the first queue to enqueue onto.");
	if (qa_get_q_num()) {
	    qnum_one = default_qnum;
	}
	else
	    return;

	printf("\nNow enter the number of the second queue to enqueue onto.");
	if (qa_get_q_num()) {
	    qnum_two = default_qnum;
	}
	else
	    return;
	
	/* lock out interrupts while doing the two enqueues */

	level = raise_interrupt_level(NETS_DISABLE);
	hwq_enqueue(queue_ptr(qnum_one), buf);
	hwq_enqueue(queue_ptr(qnum_two), buf);
	reset_interrupt_level(level);
    }

/* These instructions help to ensure that interrupt is handled while the
 * PC is still in this function, and so helps with testing the stack
 * backtrace portion of the diagnostic code.
 */

    asm volatile(" nop");
    asm volatile(" nop");
    
    return;
}
#endif /* TEST */

#ifdef TEST 
/*
 * cause a QA_DIAG_REUSE0_LINK_ERROR (zero link, not reuse link).
 */
static void rsp_test_cause_zero_error (void)
{
    printf("\nThis function will enqueue a zero onto a specified queue.");
    printf("\nFirst enter the number of the queue to enqueue onto.");

    if (qa_get_q_num()) {
	hwq_enqueue(queue_ptr(default_qnum), NULL);
    }
}
#endif /* TEST */

struct rsp_test_cmds rsp_test_main_menu[] =
{
  { "rq", "read/dequeue value from a queue", rsp_test_dequeue },
  { "wq", "write/enqueue value to a queue", rsp_test_enqueue },
  { "pq", "peek at head of queue", rsp_test_peek_q },
  { "lq", "list contents of queue\n", rsp_test_show_q },
 
  { "ra", "read and decrement accumulator", rsp_test_dec_acc },
  { "wa", "subtract value from accumulator", rsp_test_write_acc },
  { "pa", "peek at accumulator value", rsp_test_peek_acc },
  { "ia", "initialize accumulator value", rsp_test_init_acc },
  { "da", "dump non-zero accumulators\n", rsp_test_dump_acc },

  { "rd", "read QA Diag contents", rsp_test_read_diag },
  { "wd", "write values to QA Diag space", rsp_test_write_diag },
  { "fd", "fill QA Diag space with pattern\n", rsp_test_fill_diag },

  { "csd", "copy QA diag space into CPU RAM", rsp_snap_qadiag },
  { "rsd", "read diag copy contents", rsp_test_read_qa_copy },
  { "wsd", "write values to diag copy", rsp_test_write_qa_copy },
  { "fsd", "fill diag copy with pattern", rsp_test_fill_qa_copy },
  { "ssd", "search diag copy for pattern", rsp_test_search_qa_copy },
  { "lsd", "list contents of diag copy queue", rsp_test_show_qa_copy_q },
  { "fbsd", "find what diag copy queue contains a buffer header",
					rsp_test_find_copy_buf },
  { "fmsd", "find missing buffer headers inQA copy\n",
					rsp_test_missing_copy_bufs },

  { "rm", "read Memd contents", rsp_test_read_memd },
  { "wm", "write values to Memd", rsp_test_write_memd },
  { "sm", "search QA Diag and Memd for pattern", rsp_test_search_memd },
  { "fm", "fill Memd with pattern\n", rsp_test_fill_memd },

  { "ri", "read IP mema contents", rsp_test_read_ip_mem },
  { "wi", "write values to IP mema", rsp_test_write_ip_mem },
  { "si", "search IP mema for pattern", rsp_test_search_ip_mem },
  { "fi", "fill IP mema with pattern\n", rsp_test_fill_ip_mem },

  { "rf", "read IP address filter contents", rsp_test_read_addr_filter },
  { "wf", "write values to IP address filter", rsp_test_write_addr_filter },
  { "sf", "search IP address filter for pattern", rsp_test_search_addr_filter },
  { "ff", "fill IP address filter with pattern\n", rsp_test_fill_addr_filter },

  { "db", "dump bufhdr (and buffer) contents", rsp_test_dump_bufhdr, },
  { "fb", "find what queue contains a buffer header", rsp_test_find_buf },
  { "fmb", "find missing buffer headers", rsp_test_missing_bufs },
  { "cv", "Q or ACC address to/from number conversions\n", rsp_test_cnv },

  { "mr", "microcode reload\n", rsp_test_ucode_reload },

#ifdef DFS_TEST
  { "bs", "bridge generator setup", rsp_bridge_packet_setup },
  { "bg", "bridge packet generator", rsp_bridge_packet_generator },
  { "is", "ip generator setup", rsp_ip_packet_setup },
  { "ig", "ip packet generator", rsp_ip_packet_generator },
#endif
  
  { "dfsg", "DFS Generator", rsp_vip_dfs_generate },
  { "vs", "IP DFS test setup\n", rsp_vip_dfs_setup },

#ifdef TEST
  { "ce", "Cause an XBUFHDR error", rsp_test_cause_xbufhdr },
  { "cf", "Cause an INVRTN error (type)", rsp_test_cause_invrtn_type },
  { "cg", "Cause an INVRTN error (buffer hdr)", rsp_test_cause_invrtn_hdr },
  { "ch", "Cause an INVRTNBCASTID error\n", rsp_test_cause_invrtn_bcastid },
  { "cr", "Cause a reuse link error", rsp_test_cause_reuse_error },
  { "cz", "Cause a zero link error\n", rsp_test_cause_zero_error },
#endif /* TEST */

  { "vc", "Get Viper context\n", rsp_get_viper_context },

  { "i", "set default IP/slot", rsp_test_ipcmd_init },
  { "c", "send IP command", rsp_test_ipcmd },
  { "z", "clear QA error, QA Diag space and Memd", rsp_qa_init },
  { "+", "interface specific commands for default slot\n", rsp_test_ip_test},

#ifdef RSP_QATEST
  { "xq", "QA test", rsp_test_qa_test },
#endif /* RSP_QATEST */

  { "rrm", "repeat read Memd test\n", rsp_test_rread_memd },

  { "q", "exit", NULL },
  { NULL, NULL, NULL }
};



/*
 * QA, MEMD, IP, rsp diag commands main menu
 *
 * the name is obviously wrong, but this allows us to use
 * the existing (7000) parse chain
 */
void cbus_mci_command (parseinfo *csb)
{
    printf("\nRSP diagnostic console program");
    rsp_test_ipcmd_init();
    rsp_test_do_menu(rsp_test_main_menu, "RSP");
}


static void rsp_test_init (subsystype *subsys)
{
    memd_array.amax = memd_size;
    memd_array.ahi  = memd_size;
    qasic_diag_copy_ptr = &qasic_diag_copy;
    cbus_parser_test_init();	/* to match existing parser code */
}

/*
 * cBus test subsystem header
 */
#define	CBUSTST_MAJVERSION	1
#define	CBUSTST_MINVERSION	0
#define	CBUSTST_EDITVERSION	1

SUBSYS_HEADER(cbus_test,		/* name */
	      CBUSTST_MAJVERSION,	/* major version */
	      CBUSTST_MINVERSION,	/* minor version */
	      CBUSTST_EDITVERSION,	/* edit version */
	      rsp_test_init,		/* init function */
	      SUBSYS_CLASS_DRIVER,	/* subsystem class */
	      "seq: rsp",		/* sequence list */
	      "req: rsp"		/* prerequisite list */
	      );
