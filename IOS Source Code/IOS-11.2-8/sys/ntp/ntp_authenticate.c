/* $Id: ntp_authenticate.c,v 3.2.62.3 1996/08/19 18:54:44 dkatz Exp $ 
 * $Source: /release/112/cvs/Xsys/ntp/ntp_authenticate.c,v $
 *------------------------------------------------------------------
 * ntp_authenticate.c - authentication code for NTP
 *
 * November 1993, Dave Katz
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This is mostly unexpurgated code from xntpd.  Don't blame me...
 *------------------------------------------------------------------
 * $Log: ntp_authenticate.c,v $
 * Revision 3.2.62.3  1996/08/19  18:54:44  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.2.62.2  1996/04/16  19:03:03  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/18  21:12:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:06:40  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  14:49:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:50:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:40:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:44:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define CISCO

#include "master.h"
#include <ciscolib.h>
#include "packet.h"
#include "interface_private.h"
#include "config.h"
#include "../ip/ip.h"
#include "ntpd.h"
#include "ntp_stdlib.h"
#include "../util/md5.h"
#include "login_public.h"

#define MD5_KEY_LEN 32
#define DECRYPTED_PASSWORD_MAX (MD5_KEY_LEN + 1) /* Max length of password */
#define ENCRYPTED_PASSWORD_MAX (2*DECRYPTED_PASSWORD_MAX) /* Crypto version */
/*
 * Structure to store keys in in the hash table.
 */
struct savekey {
	struct savekey *next;
	union {
#ifdef	DES
	    u_long DES_key[2];
#endif
	    char MD5_key[MD5_KEY_LEN + 1];
	} k;
	u_long keyid;
	u_short flags;
	int keylen;
};

#define	KEY_TRUSTED	0x1	/* this key is trusted */
#define	KEY_KNOWN	0x2	/* this key is known */

#ifdef	DES
#define	KEY_DES		0x100	/* this is a DES type key */
#endif

#define	KEY_MD5		0x200	/* this is a MD5 type key */

/*
 * The hash table.  This is indexed by the low order bits of the
 * keyid.  We make this fairly big for potentially busy servers.
 */
#define	HASHSIZE	64
#define	HASHMASK	((HASHSIZE)-1)
#define	KEYHASH(keyid)	((keyid) & HASHMASK)

static struct savekey **key_hash;

u_long authkeynotfound;
u_long authkeylookups;
u_long authnumkeys;
u_long authkeyuncached;
u_long authencryptions;
u_long authdecryptions;

#ifdef	DES
/*
 * Size of the key schedule
 */
#define	KEY_SCHED_SIZE	128	/* number of octets to store key schedule */

/*
 * The zero key, which we always have.  Store the permutted key
 * zero in here.
 */
#define	ZEROKEY_L	0x01010101	/* odd parity zero key */
#define	ZEROKEY_R	0x01010101	/* right half of same */
u_char DESzeroekeys[KEY_SCHED_SIZE];
u_char DESzerodkeys[KEY_SCHED_SIZE];
u_char DEScache_ekeys[KEY_SCHED_SIZE];
u_char DEScache_dkeys[KEY_SCHED_SIZE];
#endif

/*
 * The key cache.  We cache the last key we looked at here.
 */
static u_long cache_keyid;
u_short cache_flags;

static int	cache_keylen;
static char	*cache_key;

#ifdef  DES
extern void DESauth_subkeys();
#endif

/*
 * For our purposes an NTP packet looks like:
 *
 *	a variable amount of encrypted data, multiple of 8 bytes, followed by:
 *	NOCRYPT_OCTETS worth of unencrypted data, followed by:
 *	BLOCK_OCTETS worth of ciphered checksum.
 */ 
#define	NOCRYPT_OCTETS	4
#define	BLOCK_OCTETS	16

#define	NOCRYPT_LONGS	((NOCRYPT_OCTETS)/sizeof(u_long))
#define	BLOCK_LONGS	((BLOCK_OCTETS)/sizeof(u_long))
#define FAKE_PACKET_SIZE (LEN_PKT_NOMAC+BLOCK_OCTETS+NOCRYPT_OCTETS)

static MD5_CTX ctx;

#define LOOPCOUNT_SHIFT 9	/* log2 of loop counter */
#define AUTHTIME_LOOPCOUNT (1<<LOOPCOUNT_SHIFT)	/* # times to average over */

/*
 * auth_flush_cache
 *
 * Flush the key cache if our key of interest is in it.
 */
static void auth_flush_cache (u_long keyno)
{
    if (cache_keyid == keyno)
	cache_flags = cache_keyid = 0;
}

/*
 * auth_findkey - find a key in the hash table
 */
static struct savekey *
auth_findkey(u_long keyno)
{
    register struct savekey *sk;
    
    if (!key_hash)
	return(NULL);
    sk = key_hash[KEYHASH(keyno)];
    while (sk) {
	if (keyno == sk->keyid)
	    break;
	sk = sk->next;
    }
    return sk;
}
/*
 * auth_delete_key - delete a key
 */
static void auth_delete_key(struct savekey *sk) {
    struct savekey *current;
    struct savekey **prev;
    
    if (sk == NULL || key_hash == NULL)
	return;

    auth_flush_cache(sk->keyid);

    current = key_hash[KEYHASH(sk->keyid)];
    prev = &key_hash[KEYHASH(sk->keyid)];
    while (current) {
	if (current == sk) {
	    *prev = sk->next;
	    sk->next = NULL;
	    free(sk);
	    return;
	}
	prev = &current->next;
	current = current->next;
    }
}

/*
 * ntp_delete_key - delete a key by key number
 *
 * Returns success/failure.
 */
boolean ntp_delete_key (u_long keyno)
{
    struct savekey *sk;

    sk = auth_findkey(keyno);
    if (sk == NULL)
	return(FALSE);
    auth_delete_key(sk);
    return(TRUE);
}

int
MD5authencrypt(u_long keyno, u_long *pkt, int length)
{
    MD5_CTX ctx;
    int len;		/* in 4 byte quantities */

    authencryptions++;

    if (keyno != cache_keyid) {
	authkeyuncached++;
	if (!authhavekey(keyno)) {
	    return 0;
	}
    }

    len = length / sizeof(u_long);

    /*
     *  Generate the authenticator.
     */
    MD5Init(&ctx);
    MD5Update(&ctx, cache_key, cache_keylen);
    MD5Update(&ctx, (char *)pkt, length);
    MD5Final((char *) &pkt[NOCRYPT_LONGS + len], &ctx);

    return 4 + BLOCK_OCTETS;	/* return size of key and MAC  */
}

int
MD5authdecrypt(u_long keyno, u_long *pkt, int length)
{
    MD5_CTX ctx;
    u_char  digest[BLOCK_OCTETS];

    authdecryptions++;

    if (keyno != cache_keyid) {
	authkeyuncached++;
	if (!authhavekey(keyno))
	    return 0;
    }

    MD5Init(&ctx);
    MD5Update(&ctx, cache_key, cache_keylen);
    MD5Update(&ctx, (char *)pkt, length);
    MD5Final(digest, &ctx);

    return (0 == bcmp(digest, (char *)pkt + length + 4, BLOCK_OCTETS));
}

/*
 *  Do first stage of a two stage authenticator generation.
 */

void MD5auth1crypt(u_long keyno, u_long *pkt, int length)
{

    authencryptions++;

    if (keyno != cache_keyid) {
	authkeyuncached++;
	if (!authhavekey(keyno)) {
	    return;
	}
    }

    MD5Init(&ctx);
    MD5Update(&ctx, cache_key, cache_keylen);
    MD5Update(&ctx, (char *)pkt, length - 8);
    /* just leave the partially computed value in the static MD5_CTX */
}

/*
 *  Do second state of a two stage authenticator generation.
 */
int
MD5auth2crypt(u_long keyno, u_long *pkt, int length)
{
    /*
     *  Don't bother checking the keys.  The first stage would have
     *  handled that.  Finish up the generation by also including the
     *  last 8 bytes of the data area.
     */

    MD5Update(&ctx, (char *)(pkt) + length - 8, 8);
    MD5Final((char *) &pkt[NOCRYPT_LONGS + length/sizeof(u_long)], &ctx);

    return 4 + BLOCK_OCTETS;
}
/*
 * authhavekey - return whether a key is known.  Permute and cache
 *		 the key as a side effect.
 */
int
authhavekey(keyno)
	u_long keyno;
{
	register struct savekey *sk;

	authkeylookups++;
	if (keyno == 0 || keyno == cache_keyid)
		return 1;

	sk = auth_findkey(keyno);

	if (sk == 0 || !(sk->flags & KEY_KNOWN)) {
		authkeynotfound++;
		return 0;
	}
	
	cache_keyid = sk->keyid;
	cache_flags = sk->flags;
	if (sk->flags & KEY_MD5) {
	    cache_keylen = sk->keylen;
	    cache_key = sk->k.MD5_key;
	    return 1;
	}

#ifdef	DES
	if (sk->flags & KEY_DES) {
	    DESauth_subkeys(sk->k.DES_key, DEScache_ekeys, DEScache_dkeys);
	    return 1;
	}
#endif
	return 0;
}


/*
 * auth_make_key - create a new key structure
 */
static struct savekey *auth_make_key (u_long keyno)
{
    struct savekey *sk;

    if (!key_hash)
	return(NULL);
    sk = malloc(sizeof(struct savekey));
    if (sk) {
	sk->keyid = keyno;
	sk->next = key_hash[KEYHASH(keyno)];
	key_hash[KEYHASH(keyno)] = sk;
	authnumkeys++;
    }
    return(sk);
}


/*
 * authtrust - declare a key to be trusted/untrusted
 */
void
authtrust(keyno, trust)
	u_long keyno;
	boolean trust;
{
    register struct savekey *sk;
    
    sk = auth_findkey(keyno);
    
    /* If it doesn't exist and we don't trust it, don't sweat it. */
    
    if (sk == NULL && !trust)
	return;

    /* Blow the cache if it contains our key, since the flags may be wrong. */

    auth_flush_cache(keyno);
    
    if (sk != NULL) {		/* Key structure exists */
	if (trust) {
	    sk->flags |= KEY_TRUSTED;
	} else {
	    sk->flags &= ~KEY_TRUSTED;
	    if (!(sk->flags & KEY_KNOWN)) {
		auth_delete_key(sk);
	    }
	}
	return;
    }
    
    /* Trusted and it doesn't exist.  Create it. */

    sk = auth_make_key(keyno);
    if (sk)
	sk->flags = KEY_TRUSTED;
    return;
}


/*
 * authistrusted - determine whether a key is trusted
 */
boolean
authistrusted(u_long keyno)
{
	register struct savekey *sk;

	if (keyno == cache_keyid)
		return ((cache_flags & KEY_TRUSTED) != 0);

	authkeyuncached++;

	sk = auth_findkey(keyno);

	if (sk == 0 || !(sk->flags & KEY_TRUSTED))
		return FALSE;
	return TRUE;
}



#ifdef	DES
/*
 * DESauth_setkey - set a key into the key array
 */
void
    DESauth_setkey(keyno, key)
u_long keyno;
u_long *key;
{
    register struct savekey *sk;
    
    /*
     * See if we already have the key.  If so just stick in the
     * new value.
     */
    sk = auth_findkey(keyno);
    if (!sk)
	sk = auth_make_key(keyno);
    if (sk) {
	sk->k.DES_key[0] = key[0];
	sk->k.DES_key[1] = key[1];
	sk->flags |= KEY_KNOWN | KEY_DES;
	auth_flush_cache(keyno);
    }
}
#endif

void MD5auth_setkey(keyno, key, crypto, cryptotype)
    u_long keyno;
    char *key;
    boolean crypto;
    int cryptotype;
{
    register struct savekey *sk;
    char *decrypted_password;

    decrypted_password = malloc(DECRYPTED_PASSWORD_MAX);
    if (!decrypted_password)
	return;

    if (crypto) {
	if (strlen(key) > 2 * MD5_KEY_LEN) {
	    printf("\n%%NTP: Key too long");
	    goto cleanup;
	}
	decrypt(key, decrypted_password, DECRYPTED_PASSWORD_MAX);
    } else {
	if (strlen(key) > MD5_KEY_LEN) {
	    printf("\n%%NTP: Key too long");
	    goto cleanup;
	}
	sstrncpy(decrypted_password, key, MD5_KEY_LEN);
    }
    sk = auth_findkey(keyno);
    if (sk == NULL)
	sk = auth_make_key(keyno);
    if (sk == NULL)
	goto cleanup;

    sstrncpy(sk->k.MD5_key, decrypted_password, MD5_KEY_LEN);
    sk->keylen = strlen(sk->k.MD5_key);
    
    sk->flags |= KEY_KNOWN | KEY_MD5;
    auth_flush_cache(keyno);
 cleanup:
    free(decrypted_password);
}

/*
 *  auth1crypt - support for two stage encryption, part 1.
 */
void
auth1crypt(u_long keyno, u_long *pkt, int length)
{
    if (keyno && keyno != cache_keyid) {
	authkeyuncached++;
	if (!authhavekey(keyno)) {
	    return;
	}
    }

#ifdef	DES
    if (!keyno || (cache_flags & KEY_DES)) {
	DESauth1crypt(keyno, pkt, length);
	return;
    }
#endif

    if (cache_flags & KEY_MD5) {
	MD5auth1crypt(keyno, pkt, length);
	return;
    }
}


/*
 *  auth2crypt - support for two stage encryption, part 2.
 */
int
auth2crypt(u_long keyno, u_long *pkt, int length)
{
    if (keyno && keyno != cache_keyid) {
	authkeyuncached++;
	if (!authhavekey(keyno)) {
	    return 0;
	}
    }

#ifdef	DES
    if (!keyno || (cache_flags & KEY_DES))
	return DESauth2crypt(keyno, pkt, length);
#endif

    if (cache_flags & KEY_MD5)
	return MD5auth2crypt(keyno, pkt, length);

    return 0;
}

int
authencrypt(u_long keyno, u_long *pkt, int length)
{
#ifdef DES
    int sendlength = 0;
#endif

    if (keyno && keyno != cache_keyid) {
	authkeyuncached++;
	if (!authhavekey(keyno)) {
	    return 0;
	}
    }

#ifdef	DES
    if (!keyno || (cache_flags & KEY_DES))
	return sendlength = DESauthencrypt(keyno, pkt, length);
#endif

    if (cache_flags & KEY_MD5)
	return MD5authencrypt(keyno, pkt, length);
    return 0;
}


int
authdecrypt(u_long keyno, u_long *pkt, int length)
{
    if (keyno && (keyno != cache_keyid)) {
	authkeyuncached++;
	if (!authhavekey(keyno)) {
	    return 0;
	}
    }

#ifdef	DES
    if (!keyno || (cache_flags & KEY_DES))
	return DESauthdecrypt(keyno, pkt, length);
#endif

    if (cache_flags & KEY_MD5)
	return MD5authdecrypt(keyno, pkt, length);

    return 0;
}

/*
 * ntp_nv_trust - create ntp trusted-key NV commands
 */
void ntp_nv_trust (parseinfo *csb)
{
    int i;
    struct savekey *key;

    if (!key_hash)
	return;
    for (i=0; i<HASHSIZE; i++) {
	key = key_hash[i];
	while (key) {
	    nv_write(key->flags & KEY_TRUSTED, "%s %d", csb->nv_command,
		     key->keyid);
	    key = key->next;
	}
    }
}

/*
 * ntp_nv_authkey - create authentication key NV commands
 */
void ntp_nv_authkey (parseinfo *csb)
{
    int i;
    struct savekey *key;
    char *encrypted_string;
    int encryption_type;

    if (!key_hash)
	return;

    encrypted_string = malloc(ENCRYPTED_PASSWORD_MAX);
    if (!encrypted_string)
	return;
    for (i=0; i<HASHSIZE; i++) {
	key = key_hash[i];
	while (key) {
	    if ((key->flags & (KEY_MD5 | KEY_KNOWN)) == 
		(KEY_MD5 | KEY_KNOWN)) {
		encryption_type = encrypt(key->k.MD5_key, encrypted_string, 
					  NULL, ENCRYPTED_PASSWORD_MAX);
		nv_write(TRUE, "%s %d md5 %s %d", 
			 csb->nv_command, key->keyid, encrypted_string,
			 encryption_type);
	    }
	    key = key->next;
	}
    }
}

/*
 * ntp_compute_auth_delay - compute delay for 2nd half of authentication
 */
static void ntp_compute_auth_delay(void)
{
    int i;
    ulong keyno = 0;
    struct savekey *key;
    boolean boguskey = FALSE;
    static l_fp dummy1;	/* Make sure they're in memory */
    static ulong dummy2;
    l_fp start, middle, end, wholething, firstpart, lastpart;
    ulong *fake_packet;

    if (!key_hash)
	return;

    fake_packet = malloc(FAKE_PACKET_SIZE);
    if (!fake_packet)
	return;

    /*
     * Find an authentication key.  If we can't find one, create one
     * temporarily.
     */
    for (i=0; i<HASHSIZE; i++) {
	key = key_hash[i];
	if (key) {
	    keyno = key->keyid;
	    break;
	}
    }
    if (!keyno) {		/* No keys found */
	keyno = 1;
	MD5auth_setkey(keyno, "BogusKey", FALSE, 0);
	boguskey = TRUE;
    }

    /* Initialize the fake packet.  Use the algorithm from authspeed. */

    for (i=0; i<FAKE_PACKET_SIZE/sizeof(ulong); i++) {
	fake_packet[i] = i * 0x22222;
    }

    dummy1.l_ui = 0x80808080;
    dummy1.l_uf = 0xffffff00;
    dummy2 = 0x0aaaaaaa;
    
    /* Run the encryption algorithm repeatedly. */

    get_systime(&start);

    for (i=0; i<AUTHTIME_LOOPCOUNT; i++) {
	auth1crypt(keyno, fake_packet, LEN_PKT_NOMAC);
	L_ADDUF(&dummy1, dummy2);
	auth2crypt(keyno, fake_packet, LEN_PKT_NOMAC);
    }

    get_systime(&middle);

    for (i=0; i<AUTHTIME_LOOPCOUNT; i++) {
	auth1crypt(keyno, fake_packet, LEN_PKT_NOMAC);
    }

    get_systime(&end);

    /*
     * We want the length of time to run the 2nd half of the encryption
     * algorithm.  Calculate the length of time to do the whole thing,
     * minus the length of time to do the first half.
     */

    wholething = middle;
    L_SUB(&wholething, &start);
    firstpart = end;
    L_SUB(&firstpart, &middle);
    lastpart = wholething;
    L_SUB(&lastpart, &firstpart);

    /*
     * OK, now we need to do the 64 bit division.  Cleverly we used a
     * power of two, so we can do it with shifts.  Yuck.
     */

    lastpart.l_uf /= AUTHTIME_LOOPCOUNT;
    lastpart.l_uf += (lastpart.l_ui % AUTHTIME_LOOPCOUNT) << 
	(32 - LOOPCOUNT_SHIFT);
    lastpart.l_ui /= AUTHTIME_LOOPCOUNT;
    if (lastpart.l_ui) {	/* Utterly bogus */
	buginf("\nNTP: authentication delay calculation problems");
    }
    sys_authdelay = lastpart.l_uf;

    /* If we created a bogus key, get rid of it. */

    if (boguskey) {
	ntp_delete_key(1);
    }
    free(fake_packet);
}

/*
 * init_auth - initialize internal data
 */
void
init_auth(void)
{
#ifdef  DES
	u_long zerokey[2];
#endif

	/*
	 * Initialize hash table and free list
	 */
	key_hash = malloc(HASHSIZE * sizeof(struct savekey *));
	if (!key_hash)
	    return;
	cache_flags = cache_keyid = 0;

	authkeynotfound = authkeylookups = 0;
	authnumkeys = authkeyuncached = 0;
	authencryptions = authdecryptions = 0;

#ifdef	DES
	/*
	 * Initialize the zero key
	 */
	zerokey[0] = ZEROKEY_L;
	zerokey[1] = ZEROKEY_R;
	/* could just zero all */
	DESauth_subkeys(zerokey, DESzeroekeys, DESzerodkeys);
#endif

	/* Compute authentication delay. */

	ntp_compute_auth_delay();
}
