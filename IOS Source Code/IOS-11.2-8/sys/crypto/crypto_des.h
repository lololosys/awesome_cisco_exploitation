/* $Id: crypto_des.h,v 3.1.60.2 1996/08/03 04:04:57 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_des.h,v $
 *------------------------------------------------------------------
 * DES prototypes
 *
 * June 1995, Tim Kolar
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: crypto_des.h,v $
 * Revision 3.1.60.2  1996/08/03  04:04:57  che
 * CSCdi65047:  Kerberos SRVTAB keys not encrypted
 * Branch: California_branch
 * Make reg_invoke_des_init return useful error codes.
 *
 * Revision 3.1.60.1  1996/04/19  14:55:38  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.2.2.1  1996/02/28  06:14:27  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 2.2  1995/06/28  10:04:00  smackie
 * Welcome some latecomers to the vast and sprawling collection of
 * loser subsystems. (CSCdi23568)
 *
 * Revision 2.1  1995/06/27  17:07:10  tkolar
 * CSCdi35462:  DES code needs to be its own subsystem
 * Make it easier to export the source base.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


struct des_state_type_ {
        /* Lookup tables initialized once only at startup by desinit() */
        long (*sp)[64];         /* Combined S and P boxes */

        char (*iperm)[16][8];   /* Initial and final permutations */
        char (*fperm)[16][8];

        /* 8 6-bit subkeys for each of 16 rounds, initialized by setkey() */
        unsigned char (*kn)[8];
        int desmode;

};

boolean des_init(int, des_state_type **);
void des_setkey(des_state_type *, char *);
void des_endes(des_state_type *, char *);
void des_dedes(des_state_type *, char *);
void des_done(des_state_type *);
void des_subsys_init(subsystype *);
void des_pw_bitshift(char *);

#define P(args) args
