/* $Id: crypto_registry.c,v 3.1.16.1 1996/04/19 14:55:58 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_registry.c,v $
 *------------------------------------------------------------------
 * crypto_registry.c - create crypto subsys registry.
 *
 * Feb 1996, xliu 
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_registry.c,v $
 * Revision 3.1.16.1  1996/04/19  14:55:58  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.1  1996/02/27  06:40:28  xliu
 * Branch: IosSec_branch
 * Add crypto_registry subsys.
 *
 * Revision 3.1  1996/02/27  05:33:53  xliu
 * Placeholder file.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "subsys.h"
#include "interface_private.h"
#include "crypto_registry.h"
#include "../crypto/crypto_registry.regc"

/*
 * crypto_registry_init
 *
 * Initialize Crypto Registry
 */
 
void crypto_registry_init (subsystype *subsys)
{
    registry_create(REG_CRYPTO, SERVICE_CRYPTO_MAX, "CRYPTO");
    create_registry_crypto();
}
 
/*
 * CRYPTO Registry subsystem header
 */
 
#define CRYPTO_REGISTRY_MAJVERSION 1
#define CRYPTO_REGISTRY_MINVERSION 0
#define CRYPTO_REGISTRY_EDITVERSION  1
 
SUBSYS_HEADER(crypto_registry,
              CRYPTO_REGISTRY_MAJVERSION, CRYPTO_REGISTRY_MINVERSION,
              CRYPTO_REGISTRY_EDITVERSION,
              crypto_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);

