/* $Id: dss.h,v 3.2.62.5 1996/08/22 19:19:05 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/crypto_lib/dss.h,v $
 *------------------------------------------------------------------
 * Type & Const defintion file for inline functions for DSS code.
 *
 * 10, 1995, xlliu 
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dss.h,v $
 * Revision 3.2.62.5  1996/08/22  19:19:05  xliu
 * CSCdi66207:  Hardware EPA does not interoperate with software crypto
 * router
 * Branch: California_branch
 * Removed unused variable.
 *
 * Revision 3.2.62.4  1996/08/22  18:51:42  xliu
 * CSCdi66207:  Hardware EPA does not interoperate with software crypto
 * router
 * Branch: California_branch
 * Before SetCipherKey is integrated, use same offset for both iv_key and
 * session key.
 *
 * Revision 3.2.62.3  1996/06/21  22:30:48  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/05/06  17:16:45  xliu
 * CSCdi56450:  remove function crypto_init_random which is no longer
 * needed
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/04/19  14:56:49  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.9  1996/03/25  18:43:58  bew
 * Branch: IosSec_branch
 * Added 40-bit DES and DES 8-bit CFB support. Implemented partial
 * challenge processing support, not yet compatible with SDU.
 * Updated the crypto methods vector per Cylink's new definitions.
 *
 * Revision 1.1.2.8  1996/03/16  01:45:44  xliu
 * Branch: IosSec_branch
 * Modified yield handle.
 *
 * Revision 1.1.2.7  1996/01/19  07:00:56  xliu
 * Branch: IosSec_branch
 * Commit before the second sync.
 *
 * Revision 1.1.2.6  1996/01/09  00:56:44  xliu
 * Branch: IosSec_branch
 * Changed crypto registry to be well know registry.
 *
 * Revision 1.1.2.5  1996/01/04  18:54:22  xliu
 * Branch: IosSec_branch
 * After initial clean up.
 *
 * Revision 1.1.2.4  1995/12/20  23:32:02  bew
 * Branch: IosSec_branch
 * Context is now freed after a sign operation. Before the callback is
 * called a pointer to the pak is taken so it can be used after the
 * callback returns.
 *
 * Revision 1.1.2.3  1995/12/10  09:31:43  xliu
 * Branch: IosSec_branch
 *
 * Use the hardcoded DH number for testing encryption code.
 *
 * Revision 1.1.2.2  1995/11/30  01:32:45  xliu
 * Branch: IosSec_branch
 * Integated with the new Cylink crypto lib source doe.
 *
 * Revision 1.1.2.1  1995/10/03  18:49:09  xliu
 * Branch: IosSec_branch
 *
 * Moved the old epa dir under crypto. Also renamed the files.
 *
 * Revision 1.1  1995/10/03  00:48:45  xliu
 * Placeholder files for crypto_lib dir.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/* 
 * crypto_dss_gen_key
 * Desp:
 *  The interface wrapper for dss gen key pair.
 * Input:
 *  public key buffer and private key buffer.
 * Output:
 *  public key and private key.
 */

extern uchar DH_base[];
extern uchar DH_modulus[];

static inline epa_status crypto_dss_gen_key(uchar *pub_key,
                                            uchar* pri_key,
                                            uchar* ran_num) 
{
    ushort   status;

    /* Initialize the random number generator */
    random_fill (ran_num, CRYPTO_RANDOM_NUM_SIZE, TRUE);
    status = GenRand(SHA_LENGTH, ran_num, ran_num);

    if (status != SUCCESS) {
        return EPA_OP_FAIL;
    };

    status = GenDSSKeyY(DSS_LENGTH_MIN, NULL, NULL, NULL, 
                        pri_key, pub_key, 
                        ran_num, &crypto_yield_handle);

    if (status != SUCCESS)
        return EPA_OP_FAIL;
    else
        return EPA_OP_OK;
};

/*
 * crypto_dss_sign
 * Desp:
 *  The interface wrapper for dss sign
 * Input:
 *    private key, random number, data to be singed, the size of data
 * Output:
 *    signature r and s.
 */

static inline epa_status crypto_dss_sign (uchar* sig_r, 
                                          uchar* sig_s, 
                                          uchar* pri_key,
                                          uchar* ran_num,
                                          uchar* data,
                                          ushort data_size)
{
    ushort    status;
    uchar     digest[SHA_LENGTH];
    uchar     dss_k[SHA_LENGTH];
    
    /* Compute the secure hash function */
    status = SHAY (data, data_size, digest, &crypto_yield_handle);
    if (status != SUCCESS)
        return EPA_OP_FAIL;

    /* Generate the per signature random number k */
    random_fill (ran_num, CRYPTO_RANDOM_NUM_SIZE, TRUE);
    status = GenDSSNumber (dss_k, NULL, ran_num);
    if (status != SUCCESS)
        return EPA_OP_FAIL;

    
    /* Compute the DSS signature */
    status = GenDSSSignatureY (DSS_LENGTH_MIN, NULL, NULL, NULL,
                              pri_key, dss_k, sig_r, sig_s, digest, 
                              &crypto_yield_handle);
    if (status != SUCCESS)
        return EPA_OP_FAIL;
    else
        return EPA_OP_OK;
}

static inline epa_status crypto_dss_verify (ushort *result,
                                            uchar* data,
                                            ushort data_size,
                                            uchar* sig_r,
                                            uchar* sig_s,
                                            uchar* pub_key)
{
    int status;
    uchar   hash_result[SHA_LENGTH];

    /* Compute the hash result of the input data */
    status = SHAY (data, data_size, hash_result, &crypto_yield_handle);
    if (status != SUCCESS) 
        return EPA_OP_FAIL;

    status = VerDSSSignatureY(DSS_LENGTH_MIN, NULL, NULL, NULL,
                              pub_key, sig_r, sig_s, hash_result, 
			&crypto_yield_handle);
    if (status == SUCCESS) {
        *result = CRYPTO_VALID_SIGNATURE;
        return EPA_OP_OK;
    } else {
        if (status == ERR_SIGNATURE ) {
            *result = CRYPTO_INVALID_SIGNATURE;
            return EPA_OP_OK;
        } else {
            return EPA_OP_FAIL;
        }
    }
}

static inline epa_status crypto_sha_digest (uchar* message,
                                            ushort message_bytes,
                                            uchar* hash_result)
{   ushort status;

    status = SHAY (message, message_bytes, hash_result, &crypto_yield_handle);

    if (status != SUCCESS)
        return EPA_OP_FAIL;
    else
        return EPA_OP_OK;
}; 

static inline epa_status crypto_gen_dh_pair (uchar *DH_secret,
                                             uchar *DH_public,
                                             uchar* ran_num)
{
    ushort status;

    /* Initialize the random number generator */
    random_fill (ran_num, CRYPTO_RANDOM_NUM_SIZE, TRUE);
    status = GenDHPairY(64, DH_secret, DH_public,
                        DH_base,
                        DH_modulus,
                        ran_num,
                        &crypto_yield_handle);
    if (status != SUCCESS)
        return EPA_OP_FAIL;
    else
        return EPA_OP_OK;
}

static inline epa_status crypto_get_dh_keys (uchar* DH_secret_self, 
                                             uchar* DH_public_peer, 
                                             ushort encrypt_key_bytes, 
                                             uchar* encrypt_key, 
                                             ushort iv_key_bytes, 
                                             uchar* iv_key, 
                                             YIELD_context *yield)
{
    ushort status;

    status = GetDHKeyY (DH_LENGTH_MIN, DH_secret_self,DH_public_peer,
                        encrypt_key_bytes, encrypt_key,
                        iv_key_bytes, iv_key, 
                        DH_modulus,
                        &crypto_yield_handle);
    if (status != SUCCESS)
        return EPA_OP_FAIL;
    else
        return EPA_OP_OK;
}

static inline epa_status crypto_get_dh_shared (uchar *DH_secret_self,
                                               uchar *DH_public_peer,
                                               uchar *DH_shared,
                                               YIELD_context *yield)
{

    ushort status;

    status = GetDHSharedNumberY(DH_LENGTH_MIN, DH_secret_self,
                                DH_public_peer,DH_shared,
                                DH_modulus,
                                &crypto_yield_handle);

    if (status != SUCCESS) {
        return EPA_OP_FAIL;
    } 

    return EPA_OP_OK;
}

static inline epa_status crypto_gen_session_iv_keys (ushort type,
				       uchar * DH_shared,
                                       uchar * session_key,
                                       uchar * iv_key)
{
    /* Need to verify with Cylink about the offset used by sdu
       code to generate session and iv key.
     */
    ushort status;
    ushort method;

    method = get_cylink_encrypt_method(type);

    status = SetCipherKey (DH_LENGTH_MIN, DH_shared + SKEY_OFFSET, 
                       	   session_key,method); 


    if (status != SUCCESS) {
        return EPA_OP_FAIL;
    }

    /* IV key is created from the 32 bytes offset of the shared
     * DH number.
     */ 
    status = SetCipherKey (DH_LENGTH_MIN, DH_shared + IKEY_OFFSET, 
                           iv_key, method);

    if (status != SUCCESS ) {
        return EPA_OP_FAIL;
    }

    return EPA_OP_OK;

};

static inline epa_status 
epa_swcrypto_gen_session_key(ushort type,
                             ushort conn_id,
                             ushort dh_shared_number_size,
                             uchar* dh_shared_number)
{
    ushort status;

    status = crypto_gen_session_iv_keys (type, 
				    dh_shared_number,
                                    epa_swcrypto_conn_tbl[conn_id].encrypt_key,
                                    epa_swcrypto_conn_tbl[conn_id].iv_key);
    if (status != EPA_OP_OK) {
        return status;
    }

    if (epa_des_initkey(type, 
                        conn_id, /* connection_id, */
                        CRYPTO_DES_KEY_VALUE_SIZE,
                        epa_swcrypto_conn_tbl[conn_id].encrypt_key)) {
        return EPA_OP_OK;
    } else {
        return EPA_OP_FAIL;
    }
};

static inline epa_status crypto_gen_syndrome (ushort conn_id,
                                             ushort challenge_len,
                                             ushort offset,
                                             uchar * challenge)
{
    ushort i;

    /* Check input sanity */
    if (crypto_dh_pairs[conn_id].status == CRYPTO_DH_PAIR_UNSET) {
        return EPA_INVALID_INPUT;
    }

    if (challenge_len >= DH_LENGTH_MIN || offset >= DH_LENGTH_MIN) {
        return EPA_INVALID_INPUT;
    }

    for (i=offset; i<DH_LENGTH_MIN; i++) 
        challenge[i%challenge_len] ^= crypto_dh_pairs[conn_id].shared_number[i];

    return EPA_OP_OK;
}

