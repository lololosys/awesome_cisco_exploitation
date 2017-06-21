/* $Id: encrypt.c,v 3.2.62.6 1996/09/01 00:10:05 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/crypto_lib/encrypt.c,v $
 *------------------------------------------------------------------
 * Encryption/Decryption wrapper functions to various different 
 * algorithms and implementations. 
 * This file is the renamed file of des.c, revision 2.1.6.3.
 *
 * 10, 1995, xliu 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: encrypt.c,v $
 * Revision 3.2.62.6  1996/09/01  00:10:05  xliu
 * CSCdi67790:  The trailing bytes are not encrypted correctly.
 * Branch: California_branch
 * The trailing bytes should be XORed with the full queue length.
 *
 * Revision 3.2.62.5  1996/08/21  06:54:02  xliu
 * CSCdi66207:  Hardware EPA does not interoperate with software crypto
 * router
 * Branch: California_branch
 * Correct CFB8 implementation error.
 *
 * Revision 3.2.62.4  1996/07/15  10:06:30  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 3.2.62.3  1996/06/09  20:16:03  xliu
 * CSCdi59763:  Crypto cfb-8 mode is not yet functional
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/05/12  23:13:00  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/04/19  14:56:50  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.10  1996/03/25  18:43:59  bew
 * Branch: IosSec_branch
 * Added 40-bit DES and DES 8-bit CFB support. Implemented partial
 * challenge processing support, not yet compatible with SDU.
 * Updated the crypto methods vector per Cylink's new definitions.
 *
 * Revision 1.1.2.9  1996/02/28  18:55:31  xliu
 * Branch: IosSec_branch
 * Moved out the ECB mode.
 *
 * Revision 1.1.2.8  1996/02/05  03:22:29  xliu
 * Branch: IosSec_branch
 * Add DH code; Add crypto_engine_encrypt/decrypt vectors.
 *
 * Revision 1.1.2.7  1996/01/04  18:54:23  xliu
 * Branch: IosSec_branch
 * After initial clean up.
 *
 * Revision 1.1.2.6  1995/12/10  09:31:44  xliu
 * Branch: IosSec_branch
 *
 * Use the hardcoded DH number for testing encryption code.
 *
 * Revision 1.1.2.5  1995/11/30  01:32:46  xliu
 * Branch: IosSec_branch
 * Integated with the new Cylink crypto lib source doe.
 *
 * Revision 1.1.2.4  1995/11/28  01:48:15  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. Removal of
 * references to kappa.
 *
 * Revision 1.1.2.3  1995/10/06  17:28:01  xliu
 * Branch: IosSec_branch
 * Fixed naming problem.
 *
 * Revision 1.1.2.2  1995/10/04  22:21:31  xliu
 * Branch: IosSec_branch
 * Check in the code to handle the packet whhich is not multiple of 8
 * bytes.
 *
 * Revision 1.1.2.1  1995/10/03  18:49:09  xliu
 * Branch: IosSec_branch
 *
 * Moved the old epa dir under crypto. Also renamed the files.
 *
 * Revision 1.1  1995/10/03  00:48:46  xliu
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
#include "ttysrv.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ip/ip.h"
#include "../crypto/crypto_registry.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_engine_api.h"
#include "../crypto/crypto_engine/core/crypto_engine.h"
#include "../crypto/crypto_engine/core/crypto_swepa.h"
#include "cylink_c_des.h"

void epa_des_encrypt(int type, uchar *key, uchar *block, 
                     uchar *queue, ushort block_len)
{
    ushort i,j;
    uchar temp_queue[8];

    /* For CFB 64 mode, the queue has the ciphertext of the previous block.
     * For ECB mode, the queue is NULL.
     */

    switch (type) {
   
	case DES_CFB64_IV1:
        case DES_40_CFB64_IV1:
        default:
 
            bcopy(queue, temp_queue, CRYPTO_ENCRYPTION_QUEUE_LEN);
            cylink_des_encrypt(key, temp_queue);
            for (i=0; i<block_len; i++) {
                block[i] ^= temp_queue[i];
                queue[i] = block[i];
            }

	    break;

        case DES_CFB8_IV1:
        case DES_40_CFB8_IV1:
            for (i=0; i<block_len; i++) {
                bcopy(queue, temp_queue, CRYPTO_ENCRYPTION_QUEUE_LEN);
                cylink_des_encrypt(key, temp_queue);
                block[i] ^= temp_queue[0];
                for (j = 0; j<CRYPTO_CFB8_SHIFT_COUNT; j++) 
                    queue[j] = queue[j+1];
                queue[CRYPTO_CFB8_SHIFT_COUNT] = block[i];
            }
            break;

    }
}

void epa_des_decrypt(int type, uchar *key, uchar *block, 
                     uchar* queue, ushort block_len)
{
    char temp[8];
    ushort i,j;
    uchar temp_queue[8];

    /* 
     * For CFB 64 mode, the queue needs to be updated with the new cipher 
     * block for the decipher of the next block. For ECB mode, the queue is 
     * NULL.
     */

    switch (type) {
   
	case DES_CFB64_IV1:
        case DES_40_CFB64_IV1:
        default:
            bcopy(queue, temp_queue, CRYPTO_ENCRYPTION_QUEUE_LEN);
            cylink_des_encrypt(key, temp_queue);
            for (i=0; i<block_len; i++) {
                /* Save the current ciphertext.*/
                temp[0] = block[i];
                block[i] ^= temp_queue[i];
                queue[i] = temp[0];
            }
            break;

        case DES_CFB8_IV1:
        case DES_40_CFB8_IV1:
            for (i=0; i<block_len; i++) {
                bcopy(queue, temp_queue, CRYPTO_ENCRYPTION_QUEUE_LEN);
                cylink_des_encrypt(key, temp_queue);
                temp[0] = block[i];
                block[i] ^= temp_queue[0];
                for (j = 0; j<CRYPTO_CFB8_SHIFT_COUNT; j++) 
                    queue[j] = queue[j+1];
                queue[CRYPTO_CFB8_SHIFT_COUNT] = temp[0];
            }
            break;
    }
}

inline int epa_des_initkey (int type, int id, int size,char * key_value)
{
    switch (type) {
    case DES_CFB64_IV1:
    case DES_40_CFB64_IV1:
    case DES_40_CFB8_IV1:
    case DES_CFB8_IV1:
    default:
        cylink_des_initkey(id, type, size, key_value);
        break;
    }

    /* Update the connection state */
    epa_swcrypto_conn_tbl[id].state = EPA_SWCRYPTO_CONN_SET;
    epa_swcrypto_conn_tbl[id].connection_id = id; 
    return 1;
}



