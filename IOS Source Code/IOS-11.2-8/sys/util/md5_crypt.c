/* $Id: md5_crypt.c,v 3.2.62.3 1996/06/05 18:19:38 carrel Exp $
 * $Source: /release/112/cvs/Xsys/util/md5_crypt.c,v $
 *------------------------------------------------------------------
 * A One way cipher function based on md5,
 *
 * February 1995, David Carrel
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: md5_crypt.c,v $
 * Revision 3.2.62.3  1996/06/05  18:19:38  carrel
 * CSCdi44545:  enable secret ? help is confusing to customers
 * Branch: California_branch
 * Cleaned up usage of passwords (enable pass, enable secret and username)
 * throughout.
 *
 * Revision 3.2.62.2  1996/04/15  15:00:54  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.2.62.1  1996/03/18  22:30:44  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:59:54  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:45:00  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:56:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/30  05:21:45  hampton
 * Move externs for the random number generator to their own file.  Remove
 * redundant initialization of the random number generator.  [CSCdi36588]
 *
 * Revision 2.2  1995/06/09 13:22:28  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:12:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <phk@login.dknet.dk> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Poul-Henning Kamp
 * ----------------------------------------------------------------------------
 *
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "login.h"
#include "../ui/command.h"
#include "../util/md5.h"
#include "../util/random.h"
#include "../util/md5_crypt.h"

static const unsigned char itoa64[] =		/* 0 ... 63 => ascii - 64 */
	"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static void
to64 (char *s, unsigned long v, int n)
{
    while (--n >= 0) {
	*s++ = itoa64[v&0x3f];
	v >>= 6;
    }
}

void
md5_crypt (char *pw, char *salt, char *secret)
{
    static char	*magic = MD5_CRYPT_MAGIC; /*
					   * This string is magic for
					   * this algorithm.  Having
					   * it this way, we can get
					   * get better later on
					   */
    char		*p, *sp, *ep, tmp_salt[5];
    unsigned char	final[16];
    int			sl, pl, i, j;
    MD5_CTX		ctx, ctx1;
    unsigned long	l, rand;

    /* Refine the Salt first */
    if (salt == NULL) {
	salt = tmp_salt;
	rand = random_gen();
	rand <<= 12;
	rand += random_gen();
	to64(salt, rand, MD5_CRYPT_SALT_DEF);
	salt[MD5_CRYPT_SALT_DEF] = '\0';
    }
    sp = salt;

    /* If it starts with the magic string, then skip that */
    if(!strncmp(sp, magic, MD5_CRYPT_MAGIC_LEN))
	sp += MD5_CRYPT_MAGIC_LEN;

    /* It stops at the first '$', max MD5_CRYPT_SALT_MAX chars */
    for(ep = sp; *ep && (*ep != '$') && (ep < (sp+MD5_CRYPT_SALT_MAX)); ep++)
	continue;

    /* get the length of the true salt */
    sl = ep - sp;

    MD5Init(&ctx);

    /* The password first, since that is what is most unknown */
    MD5Update(&ctx, pw, strlen(pw));

    /* Then our magic string */
    MD5Update(&ctx, magic, strlen(magic));

    /* Then the raw salt */
    MD5Update(&ctx, sp, sl);

    /* Then just as many characters of the MD5(pw, salt, pw) */
    MD5Init(&ctx1);
    MD5Update(&ctx1, pw, strlen(pw));
    MD5Update(&ctx1, sp, sl);
    MD5Update(&ctx1, pw, strlen(pw));
    MD5Final(final, &ctx1);
    for (pl = strlen(pw); pl > 0; pl -= 16) {
	MD5Update(&ctx, final, pl > 16 ? 16 : pl);
    }

    /* Don't leave anything around they could use. */
    memset(final, 0, sizeof(final));

    /* Then something really weird... */
    for (j = 0, i = strlen(pw); i; i >>= 1) {
	if (i&1) {
	    MD5Update(&ctx, final+j, 1);
	} else {
	    MD5Update(&ctx, pw+j, 1);
	}
    }

    /* Now make the output string */
    strcpy(secret, magic);
    sstrncat(secret, sp, sl+1);
    strcat(secret, "$");

    MD5Final(final, &ctx);

    /* 
     * and now, just to make sure things don't run too fast 
     * On a 60 Mhz Pentium this takes 34 msec, so you would
     * need 30 seconds to build a 1000 entry dictionary...
     */
    for(i=0; i < 1000; i++) {
	if (!(i & 0xf)) {
	    process_suspend();	/* routing packets is a good thing too! */
	}
	MD5Init(&ctx1);
	if (i & 1) {
	    MD5Update(&ctx1, pw, strlen(pw));
	} else {
	    MD5Update(&ctx1, final, 16);
	}
	if (i % 3) {
	    MD5Update(&ctx1, sp, sl);
	}
	if (i % 7) {
	    MD5Update(&ctx1, pw, strlen(pw));
	}
	if (i & 1) {
	    MD5Update(&ctx1, final, 16);
	} else {
	    MD5Update(&ctx1, pw, strlen(pw));
	}
	MD5Final(final, &ctx1);
    }

    p = secret + strlen(secret);

    l = (final[ 0]<<16) | (final[ 6]<<8) | final[12]; to64(p, l, 4); p += 4;
    l = (final[ 1]<<16) | (final[ 7]<<8) | final[13]; to64(p, l, 4); p += 4;
    l = (final[ 2]<<16) | (final[ 8]<<8) | final[14]; to64(p, l, 4); p += 4;
    l = (final[ 3]<<16) | (final[ 9]<<8) | final[15]; to64(p, l, 4); p += 4;
    l = (final[ 4]<<16) | (final[10]<<8) | final[ 5]; to64(p, l, 4); p += 4;
    l =                    final[11]                ; to64(p, l, 2); p += 2;
    *p = '\0';

    /* Don't leave anything around they could use. */
    memset(final, 0, sizeof(final));

    return;
}

boolean
md5_crypt_compare (char *secret, char *pass, uint enc_type)
{
    char tmp_secret[120], tmp_pass[PASSWORDLEN+1];
    boolean ret;

    switch (enc_type) {
    case ENCRYPT_NONE:
	md5_crypt(pass, secret, tmp_secret);
	break;
    case ENCRYPT_TEMP:
	decrypt(pass, tmp_pass, PASSWORDLEN);
	md5_crypt(tmp_pass, secret, tmp_secret);
	memset(tmp_pass, 0, sizeof(tmp_pass));
	break;
    default:
	return(FALSE);
	break;
    }

    ret = !strcmp(tmp_secret, secret);

    memset(tmp_secret, 0, sizeof(tmp_secret));

    return(ret);
}

boolean
ask_md5secret (tt_soc *tty, char *secret)
{
    int i;
    char buff[80];

    if (secret == NULL) {
        printf("%% No secret set");
        return(FALSE);
    }
    for (i = 0; i < 3; i++)
        if (askstring(tty, buff, sizeof buff, "Password: ", 30 * ONESEC, 1,
		      TRUE))
            if (md5_crypt_compare(secret, buff, ENCRYPT_NONE))
                return(TRUE);

    printf("\n%% Bad secrets\n");
    return(FALSE);
}

/*
 * Check if secret is in the correct format.
 */
boolean
md5_crypt_valid_secret (char *secret)
{
    int i;

    /* Check for magic string. */
    if (strncmp(secret, MD5_CRYPT_MAGIC, MD5_CRYPT_MAGIC_LEN)) {
	return(FALSE);
    }

    /* Salt must be larger than 0 length */
    if (secret[MD5_CRYPT_MAGIC_LEN] == '$') {
	return(FALSE);
    }

    /* Must find a valid separator character at end of salt. */
    for (i = MD5_CRYPT_MAGIC_LEN + 1;
	 i < (MD5_CRYPT_MAGIC_LEN + MD5_CRYPT_SALT_MAX); i++) {
	if (secret[i] == '$') {
	    break;
	}
    }

    if (i == (MD5_CRYPT_MAGIC_LEN + MD5_CRYPT_SALT_MAX)) {
	/* Didn't find separator at end of salt. */
	return(FALSE);
    }

    if (strlen(secret) <= (MD5_CRYPT_MAGIC_LEN + MD5_CRYPT_SALT_MAX + 1)) {
	return(FALSE);
    }

    /* Must be a valid secret as far as we can tell. */
    return(TRUE);
}
