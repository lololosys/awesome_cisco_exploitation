/* $Id: cylink_68_des.c,v 3.2.62.3 1996/07/16 01:55:51 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/crypto_lib/cylink_68_des.c,v $
 *------------------------------------------------------------------
 * Souce file for wrapper functions for Cylink des 68k asm code.
 * This file is renamed from cylink_68_des.c, Revision 2.1.2.3.
 *
 * 10, 1995, xliu 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cylink_68_des.c,v $
 * Revision 3.2.62.3  1996/07/16  01:55:51  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/05/12  23:12:59  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/04/19  14:56:43  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.6  1996/02/27  06:42:12  xliu
 * Branch: IosSec_branch
 * Fixed compiler errors after sync.
 *
 * Revision 1.1.2.5  1996/02/05  03:22:28  xliu
 * Branch: IosSec_branch
 * Add DH code; Add crypto_engine_encrypt/decrypt vectors.
 *
 * Revision 1.1.2.4  1995/12/10  09:31:41  xliu
 * Branch: IosSec_branch
 *
 * Use the hardcoded DH number for testing encryption code.
 *
 * Revision 1.1.2.3  1995/11/30  01:32:43  xliu
 * Branch: IosSec_branch
 * Integated with the new Cylink crypto lib source doe.
 *
 * Revision 1.1.2.2  1995/10/06  17:28:00  xliu
 * Branch: IosSec_branch
 * Fixed naming problem.
 *
 * Revision 1.1.2.1  1995/10/03  18:49:05  xliu
 * Branch: IosSec_branch
 *
 * Moved the old epa dir under crypto. Also renamed the files.
 *
 * Revision 1.1  1995/10/03  00:48:37  xliu
 * Placeholder files for crypto_lib dir.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/****************************************************************************
*  FILENAME:  des.c   PRODUCT NAME: CRYPTOGRAPHIC TOOLKIT
*
*  FILE STATUS:
*
*  DESCRIPTION:     DES Algorithm Functions File
*
*  PUBLIC FUNCTIONS:
*
*      void DESKeyExpand( uchar *key, uchar *expanded_key )
*      void DES_encrypt( uchar *keybuf, uchar *block )
*      void DES_decrypt( uchar *keybuf, uchar *block )
*      void permute(uchar *inblock, uchar perm[16][16][8], uchar *outblock)
*      long f(unsigned long r, uchar *subkey)
*      void round(uchar *keybuf, int num, unsigned long *block)
*      int main (void) - test function
*
*   Copyright (c) Cylink Corporation 1994. All rights reserved.
*
*  REVISION  HISTORY:
*               
*      Replaced the sp box and f function.
*
****************************************************************************/

/****************************************************************************
*  INCLUDE FILES
****************************************************************************/
#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "lib_cylink.h"
#include "lib_toolkit.h"
#include "types.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ip/ip.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_engine_api.h"
#include "../crypto/crypto_engine/core/crypto_engine.h"
#include "../crypto/crypto_engine/core/crypto_swepa.h"
#include "lib_des_68.h"

static void deskey_expand( uchar *key, uchar *K1 )
{
    ExpandKey(K1, key);
}

inline void cylink_des_encrypt(uchar *keybuf, uchar *block)
{
   Encrypt(keybuf, block);
}


inline void cylink_des_decrypt(uchar *keybuf, uchar *block)
{
   Decrypt(keybuf, block);
}

inline int cylink_des_initkey (int id, int type, int size,char * key_value)
{
    unsigned char *kn;

    /* allocate memory  */
    kn =  malloc (128);
    if (kn == NULL) {
        /* error report */
        return FALSE;
    }

    /* Initialize the key with the given key value */
    deskey_expand(key_value, kn);

    /* Enter the key in the table */
    epa_swcrypto_conn_tbl[id].expanded_encrypt_key = kn;
    epa_swcrypto_conn_tbl[id].alg_type = type;

    return 1;
 }



