/* $Id: cylink_fast_c_des.c,v 1.1.14.5 1996/08/19 23:44:40 dharkins Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/crypto_lib/cylink_fast_c_des.c,v $
 *------------------------------------------------------------------
 * cylink_fast_c_des.c -- Wrappers for Cylink DES C on MIP architecture. 
 *
 * Feb, 1996, xliu 
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cylink_fast_c_des.c,v $
 * Revision 1.1.14.5  1996/08/19  23:44:40  dharkins
 * CSCdi66502:  remove crypto toolkit from repository.
 * Branch: California_branch
 * fixed crypto toolkit call for compatabillity with new version
 *
 * Revision 1.1.14.4  1996/07/15  10:06:27  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 1.1.14.3  1996/07/06  19:45:08  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 1.1.14.2  1996/05/12  23:13:00  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.14.1  1996/04/19  14:56:46  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.3  1996/03/01  01:39:45  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 1.1.2.2  1996/02/28  19:30:40  bew
 * Branch: IosSec_branch
 * Changes relating to the Sync.
 *
 * Revision 1.1.2.1  1996/02/28  08:40:07  xliu
 * Branch: IosSec_branch
 * Check in Cylink DES C implementation on MIP.
 *
 * Revision 1.1  1996/02/28  08:35:16  xliu
 * Placeholder.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/****************************************************************************
*  INCLUDE FILES
****************************************************************************/
#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "lib_cylink.h"
#include "types.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ip/ip.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_registry.h"
#include "../crypto/crypto_engine_api.h"
#include "../crypto/crypto_util.h"
#include "../crypto/crypto_pubkey.h"
#include "lib_toolkit.h"
#include "../crypto/crypto_engine/core/crypto_engine.h"
#include "../crypto/crypto_engine/core/crypto_swepa.h"
#include "des_ecb.h"

int des_set_key(long long *,des_key_schedule*);
int des_ecb_encrypt(long long *, des_key_schedule*);

inline void cylink_des_encrypt(uchar *keybuf, uchar *block)
{
    des_ecb_encrypt((long long *)block, (des_key_schedule *)keybuf);
}

/*
 * epa_swcrypto_init_deskey
 *
 * This function allocates memory for the key and initialize the
 * key with the given key value. It enters the memory location of
 * the key in the key table.
 *
 */

inline int cylink_des_initkey (int id, int type, int size,char * key_value)
{
    unsigned char *kn;

    /* allocate memory for expanded key  */
    kn =  malloc (128);
    if (kn == NULL) {
        /* error report */
        return 0;
    }

    /* Initialize the key with the given key value */
    des_set_key((long long *)key_value, (des_key_schedule *)kn);

    /* Enter the key in the table */
    epa_swcrypto_conn_tbl[id].expanded_encrypt_key = kn;
    epa_swcrypto_conn_tbl[id].alg_type = type;

    return 1;
 }




