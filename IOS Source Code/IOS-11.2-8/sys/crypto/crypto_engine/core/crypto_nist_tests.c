/* 
 *------------------------------------------------------------------
 * crypto_nist_tests.c - DES tests to satisfy NIST certification
 *
 * June 1997, Brian Weis
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 */

#include "master.h"
#include <ciscolib.h>
#include "config.h"
#include "packet.h"
#include "globs.h"
#include "parser.h"
#include "types.h"
#include "interface_private.h"
#include "../crypto/parser_defs_crypto.h"
#include "../crypto/crypto_engine/crypto_lib/lib_toolkit.h"
#include "../ip/ip.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_engine_api.h"
#include "crypto_engine.h"
#include "crypto_swepa.h"
#include "../crypto/crypto_engine/crypto_lib/encrypt.h"

/*
 * NOTE: The NIST tests are included here so that they may be conveniently
 *       executed in the lab. They should NEVER be enabled in a customer image,
 *       particularly a 40-bit DES image as that could result in an export
 *       violation.
 *
 *       Enable the NIST tests by uncommenting the following line:
 *       #define NIST_TESTS_ENABLED 
 */

#ifdef NIST_TESTS_ENABLED

/*
 * External definitions of crypto toolkit functions
 */
extern int DESEncrypt(uchar *, uchar *, ushort, uchar *, uchar *, ushort);
extern int DESDecrypt(uchar *, uchar *, ushort, uchar *, ushort);
extern void DESKeyExpand(uchar *, uchar *);
extern void SetKeyParity(uchar *);

#define CHECK_AUTOMORE_QUIT \
    if (automore_quit()) { \
	return; \
    }

#define EXPANDED_KEY epa_swcrypto_conn_tbl[1].expanded_encrypt_key

typedef enum {
    NO_CC,
    DO_CC
} cc_choices;

typedef unsigned long  ord;

typedef union {
    unsigned long long ll;
    uchar c[8];
} dually;

/*
 * Print a dually value.
 */
static void print_c (dually *var, cc_choices cc)
{
    int pos;

    for (pos=0; pos<8; pos++) {
        printf("%02x", var->c[pos]);
    }
    if (cc == DO_CC) {
        printf("\n");
    }
}

/*
 * Determine how many feedback bits based on the DES mode
 */
static int crypto_nist_feedback_bits (ushort mode)
{
    switch (mode) {
    case DES_CFB8_IV1:
	return 8;
    case DES_CFB64_IV1:
	return 64;
    default:
	return 0;
    }
}

/*
 * Expand the key but hiding the details.
 */
static void crypto_nist_expand_key (ushort mode, char *key)
{
    bcopy(key, epa_swcrypto_conn_tbl[1].encrypt_key, 8);
    epa_des_initkey(mode, 1, 8, epa_swcrypto_conn_tbl[1].encrypt_key);
}

static void crypto_nist_test21 (ushort mode)
{
    /*
     * Test variables
     */
    dually key, t, iv;
    dually ib, ot;
    dually saved_iv;
    int i;
    int k;
    
    k = crypto_nist_feedback_bits(mode);

    /*
     * Lab value initialization
     */
    key.ll = 0x0000000000000000ULL;
    t.ll   = 0x0000000000000000ULL;
    iv.ll  = 0x8000000000000000ULL;

    /*
     * The encrypt function changes the IV given it, so
     * we save the original for bit manipulation later.
     */
    saved_iv.ll = iv.ll;

    /*
     * Expand the key
     */
    crypto_nist_expand_key(mode, key.c);

    printf("	Variable Text Known Answer Test - Cipher Feedback "
          "Mode - Encrypt Mode\n");
    if (k == 8) {
    	printf("	(Throughout this test, KEY=0000000000000000 and "
	       "8-bit TEXT = 0\n\n");
   } else {
    	printf("	(Throughout this test, KEY=0000000000000000 and "
	       "64-bit TEXT = 0\n\n");
   }

    for (i=1; i<=64; i++) {
	/*
	 * Set IB to T
	 */
	ib.ll = t.ll;
	
	/* 
	 * Begin the output for this iteration.
	 */
	printf("TEXT = ");
    	if (k == 8) {
	     printf("%02x",t.c[0]);
    	} else {
	     print_c(&t, NO_CC);
    	}
	printf("     IV=", i);
	print_c(&iv, NO_CC);
	
	/*
	 * Do the encryption, and copy the result to the ot strucuture.
	 */
        epa_des_encrypt(mode, EXPANDED_KEY, ib.c, iv.c, k/8);
	ot.ll = ib.ll;

	/* 
	 * Complete the output for this iteration.
	 */
	printf("     RESULT = ");
	if (k == 8) {
	    printf("%02x\n", ot.c[0]);
	} else {
	    print_c(&ot, DO_CC);
	}

	/*
	 * Move the bit in the IV over one position. The compiler
	 * 1-fills, so have to zero the filled bit;
	 */
	saved_iv.ll >>= 1;
	saved_iv.ll &=0x7fffffffffffffffULL;
	iv.ll = saved_iv.ll;
    }
}

static void crypto_nist_test22 (ushort mode)
{
    /*
     * Test variables
     */
    dually key, t, iv;
    dually ib, ot;
    dually saved_key, saved_iv;
    int i;
    int k;
    int parity = 1;

    /*
     * Lab value initialization
     */
    key.ll = 0x8001010101010101ULL;
    iv.ll  = 0x0000000000000000ULL;
    t.ll   = 0x0000000000000000ULL;
    saved_iv.ll = iv.ll;

    k = crypto_nist_feedback_bits(mode);

    /*
     * Save the initial key as a pattern to generate further keys.
     */
    saved_key.ll = 0x8000000000000000ULL;

    printf("\n\n");
    if (k == 8) {
    	printf("	Variable Key Known Answer Test - Cipher Feedback "
               "Mode - Encrypt Mode\n");
    	printf("	(Throughout this test, IV=0000000000000000 and "
	       "8-bit TEXT = 0\n\n");
   } else {
    	printf("Variable Key Known Answer Test - Cipher Feedback "
               "Mode - Encrypt Mode\n");
    	printf("	(Throughout this test, IV=0000000000000000 and "
	       "64-bit TEXT = 0\n\n");
   }

    for (i=1; i<=56; i++) {
	/*
	 * Set IB to T
	 */
	ib.ll = t.ll;
	
	/* 
	 * Begin the output for this iteration.
	 */
	printf("key=");
	print_c(&key, NO_CC);
    
   	printf("     TEXT = ");
    	if (k == 8) {
	    printf("%02x",t.c[0]);
    	} else {
	    print_c(&t, NO_CC);
    	}
    
	/*
	 * Expand the key
	 */
	crypto_nist_expand_key(mode, key.c);

	/*
	 * Do the encryption, and copy the result to the ot strucuture.
	 */
        epa_des_encrypt(mode, EXPANDED_KEY, ib.c, iv.c, k/8);
	ot.ll = ib.ll;

	/* 
	 * Complete the output for this iteration.
	 */
	printf("     RESULT = ");
	if (k == 8) {
	    printf("%02x\n", ot.c[0]);
	} else {
	    print_c(&ot, DO_CC);
	}

	/*
	 * Move the bit in the KEY over one position. The compiler
	 * 1-fills, so have to zero the filled bit. Then make the
	 * parity on each KEY byte odd.
	 */
	if (++parity == 8) {
	    parity = 1;
	    saved_key.ll >>= 1;
	}
	saved_key.ll >>= 1;
	saved_key.ll &=0x7fffffffffffffffULL;
	key.ll = saved_key.ll;
	SetKeyParity(key.c);

	/*
	 * Restore the iv to the original value
	 */
	iv.ll = saved_iv.ll;
    }
}

static void crypto_nist_test23 (ushort mode)
{
    /*
     * Test variables
     */
    dually key[33], t, iv;
    dually ib, ot;
    dually saved_iv;
    int i;
    int k;
    
    /*
     * Lab value initialization
     */
    t.ll   = 0x0000000000000000;
    iv.ll  = 0x0000000000000000;
    saved_iv.ll = iv.ll;

    k = crypto_nist_feedback_bits(mode);

    /*
     * From Table 1 in Appendix 1
     * Ignore key[0]
     */
    key[1].ll = 0x1046913489980131ULL;
    key[2].ll = 0x1007103489988020ULL;
    key[3].ll = 0x10071034c8980120ULL;
    key[4].ll = 0x1046103489988020ULL;
    key[5].ll = 0x1086911519190101ULL;
    key[6].ll = 0x1086911519580101ULL;
    key[7].ll = 0x5107b01519580101ULL;
    key[8].ll = 0x1007b01519190101ULL;
    key[9].ll = 0x3107915498080101ULL;
    key[10].ll = 0x3107919498080101ULL;
    key[11].ll = 0x10079115b9080140ULL;
    key[12].ll = 0x3107911598080140ULL;
    key[13].ll = 0x1007d01589980101ULL;
    key[14].ll = 0x9107911589980101ULL;
    key[15].ll = 0x9107d01589190101ULL;
    key[16].ll = 0x1007d01598980120ULL;
    key[17].ll = 0x1007940498190101ULL;
    key[18].ll = 0x0107910491190401ULL;
    key[19].ll = 0x0107910491190101ULL;
    key[20].ll = 0x0107940491190401ULL;
    key[21].ll = 0x19079210981a0101ULL;
    key[22].ll = 0x1007911998190801ULL;
    key[23].ll = 0x10079119981a0801ULL;
    key[24].ll = 0x1007921098190101ULL;
    key[25].ll = 0x100791159819010bULL;
    key[26].ll = 0x1004801598190101ULL;
    key[27].ll = 0x1004801598190102ULL;
    key[28].ll = 0x1004801598190108ULL;
    key[29].ll = 0x1002911598100104ULL;
    key[30].ll = 0x1002911598190104ULL;
    key[31].ll = 0x1002911598100201ULL;
    key[32].ll = 0x1002911698100101ULL;

    printf("\n\n");
    printf("	Permutation Operation Test -- Cipher Feedback "
          "- Encrypt Mode\n");
    if (k == 8) {
    	printf("	Throughout this test, IV=0000000000000000 and "
	       "8-bit TEXT=0\n\n");
    } else {
    	printf("	Throughout this test, IV=0000000000000000 and "
	       "64-bit TEXT=0\n\n");
    }

    for (i=1; i<=32; i++) {
	/*
	 * Set IB to T
	 */
	ib.ll = t.ll;
    
	/*
     	 * The encrypt function changes the IV given it, so
     	 * we reset the IV to the original.
     	 */
    	iv.ll = saved_iv.ll;
	
	/*
	 * Expand the key
	 */
	crypto_nist_expand_key(mode, key[i].c);
	
	/*
	 * Do the encryption, and copy the result to the ot strucuture.
	 */
        epa_des_encrypt(mode, EXPANDED_KEY, ib.c, iv.c, k/8);
	ot.ll = ib.ll;

	/* 
	 * Output for this iteration. Subtract 1 from the output
	 * to match the sample output.
	 */
	printf("%02d  KEY=", (i-1));
	print_c(&key[i], NO_CC);
	printf("     TEXT=");
    	if (k == 8) {
	     printf("%02x",t.c[0]);
    	} else {
	     print_c(&t, NO_CC);
    	}
	printf("     RESULT = ");
	if (k == 8) {
	    printf("%02x\n", ot.c[0]);
	} else {
	    print_c(&ot, DO_CC);
	}
    }
}

static void crypto_nist_test24 (ushort mode)
{
    /*
     * Test variables
     */
    dually key[20], t, iv[20];
    dually ib, ot;
    int i;
    int k;

    k = crypto_nist_feedback_bits(mode);

    /*
     * Lab value initialization
     */
    t.ll   = 0x0000000000000000;
    /*
     * From Table 2 in Appendix 1
     * Ignore key[0], and iv[0]
     */
    key[1].ll = 0x7ca110454a1a6e57ULL;
    key[2].ll = 0x0131d9619dc1376eULL;
    key[3].ll = 0x07a1133e4a0b2686ULL;
    key[4].ll = 0x3849674c2602319eULL;
    key[5].ll = 0x04b915ba43feb5b6ULL;
    key[6].ll = 0x0113b970fd34f2ceULL;
    key[7].ll = 0x0170f175468fb5e6ULL;
    key[8].ll = 0x43297fad38e373feULL;
    key[9].ll = 0x07a7137045da2a16ULL;
    key[10].ll = 0x04689104c2fd3b2fULL;
    key[11].ll = 0x37d06bb516cb7546ULL;
    key[12].ll = 0x1f08260d1ac2465eULL;
    key[13].ll = 0x584023641aba6176ULL;
    key[14].ll = 0x025816164629b007ULL;
    key[15].ll = 0x49793ebc79b3258fULL;
    key[16].ll = 0x4fb05e1515ab73a7ULL;
    key[17].ll = 0x49e95d6d4ca229bfULL;
    key[18].ll = 0x018310dc409b26d6ULL;
    key[19].ll = 0x1c587f1c13924fefULL;

    iv[1].ll = 0x01a1d6d039776742ULL;
    iv[2].ll = 0x5cd54ca83def57daULL;
    iv[3].ll = 0x0248d43806f67172ULL;
    iv[4].ll = 0x51454b582ddf440aULL;
    iv[5].ll = 0x42fd443059577fa2ULL;
    iv[6].ll = 0x059b5e0851cf143aULL;
    iv[7].ll = 0x0756d8e0774761d2ULL;
    iv[8].ll = 0x762514b829bf486aULL;
    iv[9].ll = 0x3bdd119049372802ULL;
    iv[10].ll = 0x26955f6835af609aULL;
    iv[11].ll = 0x164d5e404f275232ULL;
    iv[12].ll = 0x6b056e18759f5ccaULL;
    iv[13].ll = 0x004bd6ef09176062ULL;
    iv[14].ll = 0x480d39006ee762f2ULL;
    iv[15].ll = 0x437540c8698f3cfaULL;
    iv[16].ll = 0x072d43a077075292ULL;
    iv[17].ll = 0x02fe55778117f12aULL;
    iv[18].ll = 0x1d9d5c5018f728c2ULL;
    iv[19].ll = 0x305532286d6f295aULL;
    
    printf("\n\n");
    if (k == 8) {
    	printf("	Substitution Table Known Answer Test -- Cipher Feedback"
           " Mode - Encrypt Mode\n");
    	printf("	Throughout this test, 8-bit TEXT = 0\n\n");
    } else {
    	printf("Substitution Table Known Answer Test -- Cipher Feedback "
          "Mode - Encrypt Mode\n");
    	printf("	Throughout this test, 64-bit TEXT = 0\n\n");
    
    }

    for (i=1; i<=19; i++) {
	/*
	 * Set IB to T
	 */
	ib.ll = t.ll;
	
	/* 
	 * Begin the output for this iteration. Print (i-1) to
	 * match lab sample output.
	 */
	printf("%02d  KEY=", (i-1));
	print_c(&key[i], NO_CC);
	printf("    IV=");
	print_c(&iv[i], NO_CC);
    
	/*
	 * Expand the key
	 */
	crypto_nist_expand_key(mode, key[i].c);

	/*
	 * Do the encryption, and copy the result to the ot strucuture.
	 */
        epa_des_encrypt(mode, EXPANDED_KEY, ib.c, iv[i].c, k/8);
	ot.ll = ib.ll;

	/* 
	 * Complete the output for this iteration.
	 */
	printf("    RESULT=");
	if (k == 8) {
	    printf("%02x\n", ot.c[0]);
	} else {
	    print_c(&ot, DO_CC);
	}
    }
}

static void crypto_nist_test25 (ushort mode)
{
    /*
     * Test variables
     */
    dually key, pt, iv;
    dually ib, xct, fbc;
    int i,j;
    int k;

    k = crypto_nist_feedback_bits(mode);

    /*
     * Lab value initialization
     */
    if (k == 8) {
    	key.ll = 0x7ad0572676f870c2ULL;
    	pt.ll  = 0xd000000000000000ULL;
    	iv.ll  = 0x98f753e72d35c43bULL;
    } else {
	key.ll = 0xbc9d5ddf046ec41cULL;
	pt.ll  = 0xc74d0af97297b5dfULL;
	iv.ll  = 0x260184d22633319fULL;
    }
    fbc.ll = iv.ll;
    
    printf("\n\n");
    if (k == 8) {
	printf("8-bit Cipher Feedback Mode Test - The Encryption Process\n");
   } else {
    	printf("64-bit Cipher Feedback Mode Test - The Encryption Process\n");
   }
   printf("\n");

    for (i=1; i<=400; i++) {
	
	/* 
	 * Output i, KEY(i), IV(i), PT(i)
	 * NOTE: The IV is really output, but under the IB label.
	 *	 According to Key Goss of Infogard, the original NIST
	 *	 label for the IV was IB. Sigh.
	 */
	printf("%d   key=", (i-1));
	print_c(&key, NO_CC);
	printf("  IB=");
	print_c(&iv, NO_CC);
	printf("  PT=");
	if (k == 8) {
	    printf("%02x", pt.c[0]);
	} else {
	    print_c(&pt, NO_CC);
	}
	
	/*
	 * Set IB to T
	 */
	ib.ll = pt.ll;
    
	/*
	 * Expand the key
	 */
	crypto_nist_expand_key(mode, key.c);

	for (j=1; j<=10000; j++) {
	
	    /*
	     * Allow the IOS to take a breath
	     */
	    process_may_suspend();

	    /*
	     * Do the encryption.
	     */
            epa_des_encrypt(mode, EXPANDED_KEY, ib.c, iv.c, k/8);
	    xct.ll = ib.ll;

	    /*
	     * Set the IB to the leftmost k bits of the FBC
	     */
	    if (k == 8) {
		ib.ll = fbc.ll & 0xff00000000000000ULL;
	    } else {
		ib.ll = fbc.ll;
	    }

	    /*
	     * Update the FBC
	     * Shift fbc k bits to the left.
	     * Concatenate k bits of xct to the right.
	     */
	    if (k == 8) {
	    	fbc.ll <<= k;
		fbc.c[7] = xct.c[0];
	    } else {
		fbc.ll = xct.ll;
	    }
	}

	/* 
	 * Print CT (XCT(10000))
	 */
	printf("  CT =");
	if (k == 8) {
	    printf("%02x\n", xct.c[0]);
	} else {
	    print_c(&xct, DO_CC);
	}
	printf("\n");
	
	/*
	 * Quit if the user says so.
	 */
	CHECK_AUTOMORE_QUIT;

	/*
	 * Set KEY(i+1) to [KEY(i) XOR FBC], and adjusting each byte for odd
	 * parity.
	 */
	key.ll ^= fbc.ll;
	SetKeyParity(key.c);

	/*
	 * Set IV(i+1) to FBC
	 */
	iv.ll = fbc.ll;

	/*
	 * Set PT(i+1) to IB
	 */
	pt.ll = ib.ll;
    }
}

static void crypto_nist_test26 (ushort mode)
{
    /*
     * Test variables
     */
    dually key, xpt, iv;
    dually ib, ct, hold;
    dually saved_ct, saved_pt;
    int i,j;
    int k;
    int index;
    
    k = crypto_nist_feedback_bits(mode);

    /*
     * Lab value initialization
     */
    if (k == 8) {
    	key.ll = 0xd03107689107e6d9ULL;
    	ct.ll  = 0xf200000000000000ULL;
    	iv.ll  = 0xe0292cfa56f88d1eULL;
    } else {
	key.ll = 0xf4d3c44579c86185ULL;
	ct.ll  = 0xe24ca91a7119c491ULL;
	iv.ll  = 0xc7ae6a3699d643cdULL;
    }

    printf("\n\n");
    if (k == 8) {
	printf("8-bit Cipher Feedback Mode Test - The Decryption Process\n");
    } else {
    	printf("64-bit Cipher Feedback Mode Test - The Decryption Process\n");
    }
    printf("\n");
    
    for (i=1; i<=400; i++) {
	
	/* 
	 * Output i, KEY(i), IV(i), CT(i)
	 * NOTE: The IV is really output, but under the IB label.
	 *	 According to Ken Goss of Infogard, the original NIST
	 *	 label for the IV was IB. Sigh.
	 */
	printf("%d   key=", (i-1));
	print_c(&key, NO_CC);
	printf("  IB=");
	print_c(&iv, NO_CC);
	printf("  CT =");
	if (k == 8) {
	    printf("%02x", ct.c[0]);
	} else {
	    print_c(&ct, NO_CC);
	}
	
	/*
	 * Set IB to T
	 */
	ib.ll = ct.ll;
    
	/*
	 * Expand the key
	 */
	crypto_nist_expand_key(mode, key.c);

	/*
	 * Zero the saved vectors.
	 */
	saved_ct.ll = 0x0;
	saved_pt.ll = 0x0;
	    
	for (j=1; j<=10000; j++) {
	   
	    /*
	     * Allow the IOS to take a breath
	     */
	    process_may_suspend();

	    /*
	     * Set HOLD equal to IB. Do this before the decrypt
	     * since the decrypt dumps the resulting plaintext in IB.
	     */
	    hold.ll = ib.ll;

	    /*
	     * Do the decryption, and copy the result to the xpt strucuture.
	     */
            epa_des_decrypt(mode, EXPANDED_KEY, ib.c, iv.c, k/8);
	    xpt.ll = ib.ll;

	    /*
	     * We need the "last 64 bits of cleartext and ciphertext" for
	     * the next iteration. For k=8 this means the values used during
	     * the last 8 iterations through the loop.
	     */
	    if ((k==8) && (j>=9993)) {
		index = 8 - (10000 - j + 1);
		saved_ct.c[index] = hold.c[0];
		saved_pt.c[index] = xpt.c[0];
	    } 

	    /*
	     * Set IB to xpt(j) XOR HOLD
	     * This should be the same as the left most k bits of the 
	     * DES output (if we called DES_encrypt() directly). The following
	     * XOR has the effect of cancelling the CFB XOR with the
	     * ciphertext within DESDecrypt().
	     */
	    ib.ll = xpt.ll ^ hold.ll;
	}

	/* 
	 * Print (XPT(10000))
	 */
	printf("  PT=");
	if (k == 8) {
	    printf("%02x\n", xpt.c[0]);
	} else {
	    print_c(&xpt, DO_CC);
	}
	printf("\n");

	/*
	 * Quit if the user says so.
	 */
	CHECK_AUTOMORE_QUIT;

	/*
	 * Set KEY(i+1) to [KEY(i) XOR last 64 bits of plaintext], and 
	 * adjusting each byte for odd parity.
	 */
	if (k == 8) {
	    key.ll ^= saved_pt.ll;
	} else {
	    key.ll ^= xpt.ll;
	}
	SetKeyParity(key.c);

	/*
	 * Set IV(i+1) to the last 64 bits of ciphertext.
	 */
	if (k == 8) {
	    iv.ll = saved_ct.ll;
	} else {
	    iv.ll = hold.ll;
	}

	/*
	 * Set CT(i+1) to IB
	 */
	ct.ll = ib.ll;
    }
}
#endif /* NIST_TESTS_ENABLED */

void crypto_nist_runtests (parseinfo *csb)
{
#ifdef NIST_TESTS_ENABLED
    ushort mode;
    int selected_test = GETOBJ(int,2);

    /*
     * Determine the DES mode
     */
    switch (GETOBJ(int,1)) {
    case PARSER_TEST_NIST_CFB8:
    	mode = DES_CFB8_IV1;
	break;
    case PARSER_TEST_NIST_CFB64:
	mode = DES_CFB64_IV1;
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	return;
    }

    /*
     * Determine which tests to run. If the test number if zero
     * run all tests.
     */
    automore_enable(NULL);

    if ((selected_test == 0) || (selected_test == 21)) {
    	crypto_nist_test21(mode);
    }
    CHECK_AUTOMORE_QUIT;
    if ((selected_test == 0) || (selected_test == 22)) {
    	crypto_nist_test22(mode);
    }
    CHECK_AUTOMORE_QUIT;
    if ((selected_test == 0) || (selected_test == 23)) {
    	crypto_nist_test23(mode);
    }
    CHECK_AUTOMORE_QUIT;
    if ((selected_test == 0) || (selected_test == 24)) {
    	crypto_nist_test24(mode);
    }
    CHECK_AUTOMORE_QUIT;
    if ((selected_test == 0) || (selected_test == 25)) {
    	crypto_nist_test25(mode);
    }
    CHECK_AUTOMORE_QUIT;
    if ((selected_test == 0) || (selected_test == 26)) {
    	crypto_nist_test26(mode);
    }

    automore_disable();
#else
    printf("NIST tests are not enabled in this image\n");
#endif /* NIST_TESTS_ENABLED */
}
