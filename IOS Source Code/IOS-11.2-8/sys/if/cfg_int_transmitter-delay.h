/* $Id: cfg_int_transmitter-delay.h,v 3.2.60.4 1996/08/12 16:02:18 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_int_transmitter-delay.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ T R A N S M I T T E R D E L A Y . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_transmitter-delay.h,v $
 * Revision 3.2.60.4  1996/08/12  16:02:18  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.60.3  1996/07/09  21:07:13  etrehus
 * CSCdi45163:  transmitter-delay help message is confusing
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/06/17  08:31:52  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.2.60.1  1996/06/09  08:59:17  rramacha
 * CSCdi58196:  Transmitter-delay broken on FSIP DCE interfaces
 * o FSIP takes HDLC flags for transmitter delay
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:26:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:02:45  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/03  06:34:29  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Automation missed on a dual-homed file.
 *
 * Revision 2.1  1995/07/02  10:51:50  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:07:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 *	transmitter-delay { <microseconds> | <hdlc-flags> }
 *	no transmitter-delay [{ <microseconds> | <hdlc-flags> }]
 */
EOLNS	(ci_transdelay_eol, transmitter_command);

static void ci_transdelay_number_help_func (parseinfo *csb,
					    uint *lower, uint *upper,
					    const char **help)
{
    *lower = 0;
    
    *help = "Number of HDLC flags";
    *upper = 0x1FFFF;

    switch(csb->interface->hwptr->type) {
      case IDBTYPE_QUICC_SERIAL:
      case IDBTYPE_CD2430:
      case IDBTYPE_S4T68360:
	*upper = 15;
	break;
      case IDBTYPE_MK5025:
	*upper = 62;
	break;
      case IDBTYPE_CBUSSERIAL:
	*upper = reg_invoke_media_max_transmit_delay(csb->interface->hwptr);
/*FALLTHROUGH*/
      default:
	*help = "Number of HDLC flags for 4T, HSSI or FSIP - "
	        "microseconds otherwise";
	break;
    }
}

NUMBER_HELP_FUNC(ci_transdelay_number_help, ci_transdelay_eol, no_alt,
	    OBJ(int,1), ci_transdelay_number_help_func);
NOPREFIX(ci_transdelay_no, ci_transdelay_number_help, ci_transdelay_eol);
NVGENNS	(ci_transdelay_nvgen, ci_transdelay_no, transmitter_command);
KEYWORD	(ci_transdelay, ci_transdelay_nvgen, NONE,
	 "transmitter-delay", "Set dead-time after transmitting a datagram",
	 PRIV_CONF);
ASSERT	(ci_transdelay_test, ci_transdelay, ALTERNATE,
	 reg_used_media_transmit_delay(csb->interface->hwptr->type));

#undef	ALTERNATE
#define	ALTERNATE	ci_transdelay_test
