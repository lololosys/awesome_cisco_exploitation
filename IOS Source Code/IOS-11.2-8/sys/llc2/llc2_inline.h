/* $Id: llc2_inline.h,v 3.2.62.2 1996/08/28 13:00:19 thille Exp $
 * $Source: /release/112/cvs/Xsys/llc2/llc2_inline.h,v $
 *------------------------------------------------------------------
 * Inline functions for llc2
 *
 * March, 1994		Phil Morton
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Placeholder
 *------------------------------------------------------------------
 * $Log: llc2_inline.h,v $
 * Revision 3.2.62.2  1996/08/28  13:00:19  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.62.1  1996/06/21  22:31:40  syiu
 * CSCdi60893:  NETBIOS NBF (NBFCP) does not work to WFW or Win 95 clients
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:44:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/09  19:28:30  sberl
 * CSCdi35593:  Clean up cls queuing option support
 * All dlus hard code the default behaviour until they get around to
 * adding configuration.
 *
 * Revision 2.1  1995/06/07  21:42:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../cls/dlc_registry.h"
#include "../cls/dlc_public.h"
#include "../ibm/netbios_sb.h"
#include "../h/sys_registry.h"
#include "logger.h"			/* Required for next include */
#include "../llc2/msg_llc.c"		/* Not a typo, see logger.h */


#ifdef NOT_YET
/*
 * l l c G e t U C e p I d
 * l l c S e t U C e p I d
 *
 * Wrappers to extract or store u_cep_id from the fCep in the llctype
 */

static inline void *llcGetUCepId (llctype *llc)
{    	return(CepGetUCepId(&llc->fCep));			}

static inline void llcSetUCepId(llc, void *uCepId)
{	CepSetUCepId(&llc->fCep, uCepId);			}
#endif

/*
 * l l c 2 C l s N o t i f y ( )
 * support for new CLI entry into old llc code
 * Return TRUE if LLC2 under CLSI control...
 */
static inline boolean llc2ClsNotify (llctype *llc, ClsOpcodeT opcode,
				     ClsErrorCodeT ClsDlcErrorCode)
{
    ClsDlcParamBlockT clsDlcParamBlock;
    
    if (llc->u_cep_id != NO_CLS) 
    {
        memset(&clsDlcParamBlock, 0, sizeof(ClsDlcParamBlockT));
	llcBuildParmBlock(llc, opcode, &clsDlcParamBlock, ClsDlcErrorCode);
	reg_invoke_cls_rput(llc->u_cep_id, opcode, &clsDlcParamBlock);
	return (TRUE);
    }

    return(FALSE);
}

/*
 * L L C 2 i s C l s ( )
 * return TRUE if LLC2 is under CLS control
 * FALSE implies LLC was created by legacy code RSRB/LA, SDLLC, etc...
 */
static inline boolean LLC2isCls(llctype *llc)
{
    if (llc->u_cep_id == NO_CLS)
      return FALSE;
    else 
      return TRUE;
}

/*
 * LLC2isNBFCP( )
 * return TRUE if LLC2 is under NBFCP control
 * FALSE otherwise
 */
static inline boolean LLC2isNBFCP(llctype *llc)
{
    netbios_sb_t *sb;
    sb = idb_get_swsb(llc->idb, SWIDB_SB_NETBIOS);
    if (sb && sb->access_server_enabled)    
      return TRUE;
    else 
      return FALSE;
}

/*
 * llc_hash_dte
 * Generate hash table index for a dtestring
 */
static inline int llc_hash_dte (uchar *srcdtestring)
{
    int i;
    int n = 0;

    for (i = 0; i < LLC_MAX_DTE_SIZE; i++)
	n += srcdtestring[i];

    n &= LLC_DTE_HASH_MASK;

    return n;
}

static inline void llc_output_inline (paktype *pak, int type)
{
    llctype *llc;

    switch (type) {
    case LLC_IFRAME:
	llc = pak->llc2_cb;
	if (llc == NULL) {
	    errmsg(&msgsym(UNEXPECT, LLC), "no llc2_cb for iframe");
	    datagram_done(pak);
	    break;
	}
	p_enqueue(&llc->txQ, pak);
	llc->txqnum++;
	llc->txaction |= TXDATA_R_BIT;
	llc2call(llc);
	break;

    case LLC_UNNUMBERED:
    case LLC_SUPERVISORY:
	llc_sendframe(pak);
	break;
    default:
	errmsg(&msgsym(UNEXPECT, LLC), "llc_out");
	datagram_done(pak);
    }
    return;
}

