/* $Id: md5_crypt.h,v 3.2.62.2 1996/06/05 18:19:41 carrel Exp $
 * $Source: /release/112/cvs/Xsys/util/md5_crypt.h,v $
 *------------------------------------------------------------------
 * Prototype definitions for md5_crypt.c
 *
 * February 1995, David Carrel
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: md5_crypt.h,v $
 * Revision 3.2.62.2  1996/06/05  18:19:41  carrel
 * CSCdi44545:  enable secret ? help is confusing to customers
 * Branch: California_branch
 * Cleaned up usage of passwords (enable pass, enable secret and username)
 * throughout.
 *
 * Revision 3.2.62.1  1996/04/15  15:00:56  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.2  1995/11/17  17:56:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:12:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define CRYPT_SECRET_LEN	120

/* This provides the version number.  It is the begining of ALL secrets. */
#define MD5_CRYPT_MAGIC		"$1$"
#define MD5_CRYPT_MAGIC_LEN	3

#define MD5_CRYPT_SALT_MAX	8	/* Maximum length for salt. */
#define MD5_CRYPT_SALT_DEF	4	/* Default length for salt. */

void md5_crypt(char *pw, char *salt, char *secret);
boolean md5_crypt_compare(char *secret, char *pass, uint enc_type);
boolean ask_md5secret(tt_soc *tty, char *secret);
boolean md5_crypt_valid_secret(char *secret);
