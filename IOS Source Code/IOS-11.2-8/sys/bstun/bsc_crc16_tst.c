/* $Id: bsc_crc16_tst.c,v 3.1.68.1 1996/03/18 19:01:10 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/bstun/bsc_crc16_tst.c,v $
 *------------------------------------------------------------------
 * CRC16 calculation routines for BSC test commands.
 *
 * Oct 95. Saritha Roach
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bsc_crc16_tst.c,v $
 * Revision 3.1.68.1  1996/03/18  19:01:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.32.2  1996/03/07  08:38:56  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1.32.1  1996/02/20  13:38:08  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.1  1995/11/09  11:03:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/10  07:55:42  sroach
 * CSCdi41879:  Add BSC test command support
 *
 * Revision 2.1  1995/10/10  05:43:31  sroach
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  Usage:
 *     crc16 < source_string_file > output_string_file
 *
 *  The source_string_file is free format - all blanks, newlines etc. 
 *  ignored. 
 *  The source consists of one or more frames in hexadecimal.  
 *  Each frame is delimited by a semicolon.
 *  Put a full stop after the last one.
 *
 *  Each frame has an optional prefix "-" indicating ASCII.
 *  All other characters are ignored without error.
 *
 *  The output is the same as the input but with extra lines.  E.g.:
 *  in:  
 *       02 41 42 43 03;
 *  out: 
 *       02 41 42 43 03;
 *       crc = A7 13
 *
 *  The code has the smarts not to count SOH/STX/DLE STX in the BCC when these
 *  occur at the start of the frame.  However, a leading non-control character 
 *  is counted.  This allows the leading SOH/STX to be omitted from the input 
 *  frame.
 *  SYNs (DLE SYN if xparent) aren't counted anywhere.
 *
 *  Also does LRC for nontransparent ASCII.  If the data is entered with 
 *  correct parity then the generated LRC also has correct parity.
 */

#include "master.h"
#include <string.h>

#define dbug 
/* #define dbug printf("{%d,%4x}", state, crc); */
#define uchar unsigned char
#define Boolean uchar
#define uint unsigned int

/*
 *  SOB does duty for SOH and STX
 *  EOB does duty for ETB and ETX
 */
typedef enum BSC_Input {
SYN , SOB , DLE , GPH , EOB , ITB, NumInputs } BSC_Input;

/*--------------------------------------------------------------------------*/
/* ASCII set of valid BSC control characters. */

static BSC_Input crc_ascii_to_bsc [] = {

/* 00 - 07 */ GPH, SOB, SOB, EOB, GPH, GPH, GPH, GPH, 
/* 08 - 0F */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 10 - 17 */ DLE, GPH, GPH, GPH, GPH, GPH, SYN, EOB, 
/* 18 - 1F */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, ITB, 
/* 20 - 27 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 28 - 2F */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 30 - 37 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 38 - 3F */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 40 - 47 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 48 - 4F */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 50 - 57 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 58 - 5F */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 60 - 67 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 68 - 6F */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 70 - 77 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 78 - 7F */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 80 - 87 */ GPH, GPH, GPH, EOB, GPH, GPH, GPH, GPH, 
/* 88 - 8F */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 90 - 97 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, EOB, 
/* 98 - 9F */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* A0 - A7 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* A8 - AF */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* B0 - B7 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* B8 - BF */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* C0 - C7 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* C8 - CF */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* D0 - D7 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* D8 - DF */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* E0 - E7 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* E8 - EF */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* F0 - F7 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* F8 - FF */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 

};  /* crc_ascii_to_bsc[]. */


/*---------------------------------------------------------------------------*/
/* EBCDIC set of valid BSC control characters. */

static BSC_Input crc_ebcdic_to_bsc[] = {

/* 00 - 07 */ GPH, SOB, SOB, EOB, GPH, GPH, GPH, GPH, 
/* 08 - 0F */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 10 - 17 */ DLE, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 18 - 1F */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, ITB, 
/* 20 - 27 */ GPH, GPH, GPH, GPH, GPH, GPH, EOB, GPH, 
/* 28 - 2F */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 30 - 37 */ GPH, GPH, SYN, GPH, GPH, GPH, GPH, GPH, 
/* 38 - 3F */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH,  
/* 40 - 47 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 48 - 4F */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH,  
/* 50 - 57 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 58 - 5F */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH,  
/* 60 - 67 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 68 - 6F */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH,  
/* 70 - 77 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 78 - 7F */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH,  
/* 80 - 87 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 88 - 8F */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH,  
/* 90 - 97 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* 98 - 9F */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH,  
/* A0 - A7 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* A8 - AF */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH,  
/* B0 - B7 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* B8 - BF */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH,  
/* C0 - C7 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* C8 - CF */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH,  
/* D0 - D7 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* D8 - DF */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH,  
/* E0 - E7 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* E8 - EF */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH,  
/* F0 - F7 */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 
/* F8 - FF */ GPH, GPH, GPH, GPH, GPH, GPH, GPH, GPH, 

};  /* crc_ebcdic_to_bsc[]. */

#define NumStates 5 

static uchar fsmNewState [NumInputs][NumStates] =
/*
 *    Start SOB DLE DLE DLE
 *                  STX STX
 *                  DLE
 */

/*       0   1   2   3   4 */{
/*SYN*/{ 0,  1,  1,  4,  4},
/*SOB*/{ 1,  1,  4,  4,  4},
/*DLE*/{ 2,  2,  1,  4,  3},
/*Gr */{ 1,  1,  1,  4,  4},
/*EOB*/{ 0,  0,  1,  0,  4},
/*ITB*/{ 1,  1,  1,  1,  4}};

typedef enum {a0, a1, a2, aX, aE} FsmActs;

static FsmActs fsmAct [NumInputs][NumStates] =
/*
 *    Start SOB DLE DLE DLE
 *                  STX STX
 *                  DLE
 */

/*       0   1   2   3   4 */{
/*SYN*/{a0, a0, aE, a0, a1},
/*SOB*/{a0, a1, aX, aE, a1},
/*DLE*/{a0, a0, aE, a1, a0},
/*Gr */{a1, a1, aE, a1, a1},
/*EOB*/{a2, a2, aE, a2, a1},
/*ITB*/{a2, a2, aE, a2, a1}};

static uchar state;
static int   frameLen;
static uchar bccBytes, baseBccBytes;
static BSC_Input * charTbl;

static uchar dig1got;
static uint  crc;

static uint  poly[2] = {0, 0xA001};  /* x^0 + x^2 + x^15 */ 


void ProcByteBCC (uint byt) {

  int bit;

  if (bccBytes == 2) {
    /* CRC */
    for (bit = 0; bit < 8; bit++) {
      crc = poly[crc&1] ^ (crc >> 1);
    }
    crc ^= byt << 8;
  } else {
    /* LRC */
    crc ^= byt ^ 0x80;  /* keep parity even in the bcc */
  }
}

void ProcByte (uchar byt) {
  BSC_Input encode = charTbl[byt];
  FsmActs act = fsmAct[encode][state];
  state = fsmNewState[encode][state];
  switch (act) {
    case a0: {
      break;
    };
    case a1: {
      ProcByteBCC(byt);
      break;
    };
    case a2: {
      ProcByteBCC(byt);
      if (bccBytes == 2) {
        dbug;
        ProcByteBCC(0);
        dbug;
        ProcByteBCC(0);
/*        printf ("\ncrc = %2x %2x\n", crc%256, crc/256); */
      } else {
        /* calculate BCC with odd parity */
/*        printf ("\nlrc = %2x\n", crc ^ 0x80); */
      }
      break;
    };
    case aX: {
      bccBytes = 2;
      if (frameLen>1) {
        ProcByteBCC(0x10);  /* special case - count both DLE and STX */
        ProcByteBCC(byt);
      }
      break;
    };
    case aE: {
      printf ("\nUnexpected sequence\n");
      break;
    };
  }
  frameLen++;
dbug;
}

int ProcFrame (uchar *data, uint data_len, uint bcc_length)
{
  uchar *chr = data;
  uint i;

  state = 0;
  frameLen = 0;
  dig1got = 0;
  crc = 0;

  if (bcc_length == 1) {
    bccBytes = baseBccBytes = 1;
    charTbl = crc_ascii_to_bsc;
  } else {
    bccBytes = baseBccBytes = 2;
    charTbl = crc_ebcdic_to_bsc;
  }

  for (i = 0; i < data_len; i++,chr++) {
    ProcByte( *chr);
  }
  return 1;
}

/*********************************************************************/
/*                                                                   */
/* get_bcc                                                         */
/*                                                                   */
/*********************************************************************/
uint get_bcc (uchar *data, uint data_len, uint bcc_length)
{
  ProcFrame(data, data_len, bcc_length);
  return crc;
}
