/* $Id: struct.h,v 1.1.4.1 1996/05/09 15:01:15 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/struct.h,v $
 *------------------------------------------------------------------
 * struct.h -- 
 *
 * ?
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: struct.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:15  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:14  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:38  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:24  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:54:10  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:42  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************/
/*                                                                            */
/* DIAGNOSTICS DRIVER                                                         */
/*                                                                            */
/* This file contains "Structure Definitions" that are included as part of    */
/* Diagnostics Driver.                                                        */
/*                                                                            */
/* NOTE TO THE REVIEWER:                                                      */
/* --------------------                                                       */
/* You may find that Comments & Remarks are kept to the minimum extent, which */
/* eventually would be updated in due course of time.  The author believes in */
/* "Self-documentation" by way of meaningful names for procedures, variables  */
/* etc. and "Hierarchical programming" style which would work out better in   */
/* terms of time & productivity.  In the meantime, if you encounter any       */
/* problems in going thru the code or have any comments/suggestions, do let   */
/* the author know about it.                                                  */
/*                                                                            */
/******************************************************************************/

	struct COMMAND_TABLE {
	   char		*command;
	   char		*description;
	   char		**syntax;
	   int		abbrev;
	   int		(*procedure)();
	   int		display_control;
	};

#define	PARAMETER_SET		0x0001
#define	PARAMETER_RESET		0x0002
#define	PARAMETER_MVALUE	0x8000
	struct PARAMETER_TABLE {
	   char 	*parameter;
	   char		*name;
	   char		*format;
	   union {
	      uInt32	value;
	      char	*ptr;
	   }		default_value;
	   union {
	      uInt32	value;
	      uInt32	*bitmap_p;
	   }		min_value;
	   union {
	      uInt32	value;
	      uInt32	length;
	   }		max_value;
	   char		*discrete_value;
	   int		parameter_control;
	};

	struct RESULTS_TABLE {
	   char		*result;
	   char		*name;
	   char		*format;
	   int		display_control;
	};

#define	RUN		0x0001
#define	SKIP		0x0002
/*#define	INTERACTIVE	0x0004
#define	EXTLB_TEST	0x0008*/
#define	INVALID		0x0010
#define	INIT		0x8000
	struct SECTION_TOC {
	   char				*description;
	   char				*name;
	   char				*id;
	   struct PARAMETER_TABLE	*parameter_table;
	   struct TEST_TOC		*test_toc;
	   int				execution_control;
	};
	struct TEST_TOC {
	   char				*description;
	   char				*name;
	   char				*id;
	   int				(*procedure)();
	   struct PARAMETER_TABLE	*parameter_table;
	   struct RESULTS_TABLE		*results_table;
	   int				rc_index;
	   int				execution_control;
	};

#ifdef MASERATI
/* MAC Register Structures */
struct MAC_REGS {
  union {
    struct {
      union {
	uInt16 cmdreg1;
	uInt16 st1u;
      } addr0;
      union {
	uInt16 cmdreg2;
	uInt16 st1l;
      } addr1;
    } lw;
    uInt32 reg0[2*1024];
  }r0;

  union {
    struct {
      uInt16 st2u;
      uInt16 st2l;
    } lw;
    uInt32 reg1[2*1024];
  }r1;
  union {
    struct {
      uInt16 imsk1u;
      uInt16 imsk1l;
    } lw;
    uInt32 reg2[2*1024];
  }r2;
  union {
    struct {
      uInt16 imsk2u;
      uInt16 imsk2l;
    } lw;
    uInt32 reg3[2*1024];
  }r3;
  union {
    struct {
      uInt16 said;
      uInt16 laim;
    } lw;
    uInt32 reg4[2*1024];
  }r4;
  union {
    struct {
      uInt16 laic;
      uInt16 lail;
    } lw;
    uInt32 reg5[2*1024];
  }r5;
  union {
    struct {
      uInt16 sagp;
      uInt16 lagm;
    } lw;
    uInt32 reg6[2*1024];
  }r6;
  union {
    struct {
      uInt16 lagc;
      uInt16 lagl;
    } lw;
    uInt32 reg7[2*1024];
  }r7;
  union {
    struct {
      uInt16 mdreg1;
      uInt16 stmchn;
    } lw;
    uInt32 reg8[2*1024];
  }r8;
  union {
    struct {
      uInt16 mir1;
      uInt16 mir0;
    } lw;
    uInt32 reg9[2*1024];
  }r9;
  union {
    struct {
      uInt16 tmax;
      uInt16 tvx;
    } lw;
    uInt32 reg10[2*1024];
  }r10;
  union {
    struct {
      uInt16 trt;
      uInt16 tht;
    } lw;
    uInt32 reg11[2*1024];
  }r11;
  union {
    struct {
      uInt16 tneg;
      uInt16 tmrs;
    } lw;
    uInt32 reg12[2*1024];
  }r12;
  union {
    struct {
      uInt16 treq0;
      uInt16 treq1;
    } lw;
    uInt32 reg13[2*1024];
  }r13;
  union {
    struct {
      uInt16 pri0;
      uInt16 pri1;
    } lw;
    uInt32 reg14[2*1024];
  }r14;
  union {
    struct {
      uInt16 pri2;
      uInt16 tsync;
    } lw;
    uInt32 reg15[2*1024];
  }r15;
  union {
    struct {
      uInt16 mdreg2;
      uInt16 frmthr;
    } lw;
    uInt32 reg16[2*1024];
  }r16;
  union {
    struct {
      uInt16 eacb;
      uInt16 earv;
    } lw;
    uInt32 reg17[2*1024];
  }r17;
  union {
    struct {
      uInt16 eas;
      uInt16 eaa0;
    } lw;
    uInt32 reg18[2*1024];
  }r18;
  union {
    struct {
      uInt16 eaa1;
      uInt16 eaa2;
    } lw;
    uInt32 reg19[2*1024];
  }r19;
  union {
    struct {
      uInt16 sacl;
      uInt16 sabc;
    } lw;
    uInt32 reg20[2*1024];
  }r20;
  union {
    struct {
      uInt16 wpxsf;
      uInt16 rpxsf;
    } lw;
    uInt32 reg21[2*1024];
  }r21;
  union {
    struct {
      uInt16 unused1;
      uInt16 rpr;
    } lw;
    uInt32 reg22[2*1024];
  }r22;
  union {
    struct {
      uInt16 wpr;
      uInt16 swpr;
    } lw;
    uInt32 reg23[2*1024];
  }r23;
  union {
    struct {
      uInt16 wpxs;
      uInt16 wpxa0;
    } lw;
    uInt32 reg24[2*1024];
  }r24;
  union {
    struct {
      uInt16 wpxa1;
      uInt16 wpxa2;
    } lw;
    uInt32 reg25[2*1024];
  }r25;
  union {
    struct {
      uInt16 swpxs;
      uInt16 swpxa0;
    } lw;
    uInt32 reg26[2*1024];
  }r26;
  union {
    struct {
      uInt16 swpxa1;
      uInt16 swpxa2;
    } lw;
    uInt32 reg27[2*1024];
  }r27;
  union {
    struct {
      uInt16 rpxs;
      uInt16 rpxa0;
    } lw;
    uInt32 reg28[2*1024];
  }r28;
  union {
    struct {
      uInt16 rpxa1;
      uInt16 rpxa2;
    } lw;
    uInt32 reg29[2*1024];
  }r29;
  union {
    struct {
      uInt16 marr;
      uInt16 marw;
    } lw;
    uInt32 reg30[2*1024];
  }r30;
  union {
    uInt32 mdr;
    struct w {
      uInt16 mdru;
      uInt16 mdrl;
    } wd;
    uInt32 reg31[2*1024];
  } data_reg;  

  union {
    struct {
      uInt16 tmsync;
      uInt16 fcntr;
    } lw;
    uInt32 reg32[2*1024];
  }r32;
  union {
    struct {
      uInt16 lcntr;
      uInt16 ecntr;
    } lw;
    uInt32 reg33[2*1024];
  }r33;
};
#endif /* MASERATI */

#ifdef CALYPSO
/* MAC Register Structures */
struct MAC_REGS {
  union {
    struct {
      union {
	uInt16 cmdreg1;
	uInt16 st1u;
      } addr0;
	uInt16 unused1;
      union {
	uInt16 cmdreg2;
	uInt16 st1l;
      } addr1;
	uInt16 unused2;
    } lw;
    uInt32 reg0[2];
  }r0;

  union {
    struct {
      uInt16 st2u;
      uInt16 unused1;
      uInt16 st2l;
      uInt16 unused2;
    } lw;
    uInt32 reg1[2];
  }r1;
  union {
    struct {
      uInt16 imsk1u;
      uInt16 unused1;
      uInt16 imsk1l;
      uInt16 unused2;
    } lw;
    uInt32 reg2[2];
  }r2;
  union {
    struct {
      uInt16 imsk2u;
      uInt16 unused1;
      uInt16 imsk2l;
      uInt16 unused2;
    } lw;
    uInt32 reg3[2];
  }r3;
  union {
    struct {
      uInt16 said;
      uInt16 unused1;
      uInt16 laim;
      uInt16 unused2;
    } lw;
    uInt32 reg4[2];
  }r4;
  union {
    struct {
      uInt16 laic;
      uInt16 unused1;
      uInt16 lail;
      uInt16 unused2;
    } lw;
    uInt32 reg5[2];
  }r5;
  union {
    struct {
      uInt16 sagp;
      uInt16 unused1;
      uInt16 lagm;
      uInt16 unused2;
    } lw;
    uInt32 reg6[2];
  }r6;
  union {
    struct {
      uInt16 lagc;
      uInt16 unused1;
      uInt16 lagl;
      uInt16 unused2;
    } lw;
    uInt32 reg7[2];
  }r7;
  union {
    struct {
      uInt16 mdreg1;
      uInt16 unused1;
      uInt16 stmchn;
      uInt16 unused2;
    } lw;
    uInt32 reg8[2];
  }r8;
  union {
    struct {
      uInt16 mir1;
      uInt16 unused1;
      uInt16 mir0;
      uInt16 unused2;
    } lw;
    uInt32 reg9[2];
  }r9;
  union {
    struct {
      uInt16 tmax;
      uInt16 unused1;
      uInt16 tvx;
      uInt16 unused2;
    } lw;
    uInt32 reg10[2];
  }r10;
  union {
    struct {
      uInt16 trt;
      uInt16 unused1;
      uInt16 tht;
      uInt16 unused2;
    } lw;
    uInt32 reg11[2];
  }r11;
  union {
    struct {
      uInt16 tneg;
      uInt16 unused1;
      uInt16 tmrs;
      uInt16 unused2;
    } lw;
    uInt32 reg12[2];
  }r12;
  union {
    struct {
      uInt16 treq0;
      uInt16 unused1;
      uInt16 treq1;
      uInt16 unused2;
    } lw;
    uInt32 reg13[2];
  }r13;
  union {
    struct {
      uInt16 pri0;
      uInt16 unused1;
      uInt16 pri1;
      uInt16 unused2;
    } lw;
    uInt32 reg14[2];
  }r14;
  union {
    struct {
      uInt16 pri2;
      uInt16 unused1;
      uInt16 tsync;
      uInt16 unused2;
    } lw;
    uInt32 reg15[2];
  }r15;
  union {
    struct {
      uInt16 mdreg2;
      uInt16 unused1;
      uInt16 frmthr;
      uInt16 unused2;
    } lw;
    uInt32 reg16[2];
  }r16;
  union {
    struct {
      uInt16 eacb;
      uInt16 unused1;
      uInt16 earv;
      uInt16 unused2;
    } lw;
    uInt32 reg17[2];
  }r17;
  union {
    struct {
      uInt16 eas;
      uInt16 unused1;
      uInt16 eaa0;
      uInt16 unused2;
    } lw;
    uInt32 reg18[2];
  }r18;
  union {
    struct {
      uInt16 eaa1;
      uInt16 unused1;
      uInt16 eaa2;
      uInt16 unused2;
    } lw;
    uInt32 reg19[2];
  }r19;
  union {
    struct {
      uInt16 sacl;
      uInt16 unused1;
      uInt16 sabc;
      uInt16 unused2;
    } lw;
    uInt32 reg20[2];
  }r20;
  union {
    struct {
      uInt16 wpxsf;
      uInt16 unused1;
      uInt16 rpxsf;
      uInt16 unused2;
    } lw;
    uInt32 reg21[2];
  }r21;
  union {
    struct {
      uInt16 unused;
      uInt16 unused1;
      uInt16 rpr;
      uInt16 unused2;
    } lw;
    uInt32 reg22[2];
  }r22;
  union {
    struct {
      uInt16 wpr;
      uInt16 unused1;
      uInt16 swpr;
      uInt16 unused2;
    } lw;
    uInt32 reg23[2];
  }r23;
  union {
    struct {
      uInt16 wpxs;
      uInt16 unused1;
      uInt16 wpxa0;
      uInt16 unused2;
    } lw;
    uInt32 reg24[2];
  }r24;
  union {
    struct {
      uInt16 wpxa1;
      uInt16 unused1;
      uInt16 wpxa2;
      uInt16 unused2;
    } lw;
    uInt32 reg25[2];
  }r25;
  union {
    struct {
      uInt16 swpxs;
      uInt16 unused1;
      uInt16 swpxa0;
      uInt16 unused2;
    } lw;
    uInt32 reg26[2];
  }r26;
  union {
    struct {
      uInt16 swpxa1;
      uInt16 unused1;
      uInt16 swpxa2;
      uInt16 unused2;
    } lw;
    uInt32 reg27[2];
  }r27;
  union {
    struct {
      uInt16 rpxs;
      uInt16 unused1;
      uInt16 rpxa0;
      uInt16 unused2;
    } lw;
    uInt32 reg28[2];
  }r28;
  union {
    struct {
      uInt16 rpxa1;
      uInt16 unused1;
      uInt16 rpxa2;
      uInt16 unused2;
    } lw;
    uInt32 reg29[2];
  }r29;
  union {
    struct {
      uInt16 marr;
      uInt16 unused1;
      uInt16 marw;
      uInt16 unused2;
    } lw;
    uInt32 reg30[2];
  }r30;
  union {
    struct {
      uInt16 mdru;
      uInt16 unused1;
      uInt16 mdrl;
      uInt16 unused2;
    } wd;
    uInt32 reg31[2];
  } data_reg;  

  union {
    struct {
      uInt16 tmsync;
      uInt16 unused1;
      uInt16 fcntr;
      uInt16 unused2;
    } lw;
    uInt32 reg32[2];
  }r32;
  union {
    struct {
      uInt16 lcntr;
      uInt16 unused1;
      uInt16 ecntr;
      uInt16 unused2;
    } lw;
    uInt32 reg33[2];
  }r33;
};

/*----------------------------------------------------------------------------*
 * BIGA Register Structures 
 *----------------------------------------------------------------------------*/
	struct BIGA_REGS {
#define	   NPCTL_RESET	0x01
#define	   NPCTL_BYPN	0x02
#define	   NPCTL_BACKEN	0x04
#define	   NPCTL_SBCI	0x08
#define	   NPCTL_IMOD	0x10
#define	   NPCTL_NPCI	0x20
#define	   NPCTL_THHLC	0x40
#define	   NPCTL_RHHLC	0x80
	   struct {
	      uInt16	npctl16;
	      uChar	f[14];
	   }		r0;
	   struct {
	      uInt16	com;
	      uChar	f[14];
	   }		r1;
	   struct {
	      uChar	int0;
	      uChar	f[15];
	   }		r2;
	   struct {
	      uChar	int1;
	      uChar	f[15];
	   }		r3;
	   struct {
	      uInt16	npim;
	      uChar	f[14];
	   }		r4;
	   struct {
	      uInt16	nist;
	      uChar	f[14];
	   }		r5;
	   struct {
	      uChar	i0v;
	      uChar	f[15];
	   }		r6;
	   struct {
	      uChar	i1v;
	      uChar	f[15];
	   }		r7;
	   struct {
	      uChar	rsrcv;
	      uChar	f[15];
	   }		r8;
	   struct {
	      uChar	rcvev;
	      uChar	f[15];
	   }		r9;
	   struct {
	      uChar	xmtev;
	      uChar	f[15];
	   }		r10;
	   struct {
	      uChar	rdnpv;
	      uChar	f[15];
	   }		r11;
	   struct {
	      uChar	rdsbv;
	      uChar	f[15];
	   }		r12;
	   struct {
	      uChar	xmtdv;
	      uChar	f[15];
	   }		r13;
	   struct {
	      uChar	mfnpv;
	      uChar	f[15];
	   }		r14;
	   struct {
	      uChar	lev;
	      uChar	f[15];
	   }		r15;
	   struct {
	      uChar	aev;
	      uChar	f[15];
	   }		r16;
	   struct {
	      uChar	cstat;
	      uChar	f[15];
	   }		r17;
	   struct {
	      uChar	parv;
	      uChar	f[15];
	   }		r18;
	   struct {
	      uInt16	sist;
	      uChar	f[14];
	   }		r19;
	   struct {
	      uInt16	hica;
	      uChar	f[14];
	   }		r20;
	   struct {
	      uInt16	hicb;
	      uChar	f[14];
	   }		r21;
	   struct {
	      uChar	hicc;
	      uChar	f[15];
	   }		r22;
	   struct {
	      uInt16	upad;
	      uChar	f[14];
	   }		r23;
	   struct {
	      uInt16	dctrl;
	      uChar	f[14];
	   }		r24;
	   struct {
	      uChar	dstat;
	      uChar	f[15];
	   }		r25;
	   struct {
	      uChar	dctrl2;
	      uChar	f[15];
	   }		r26;
	   struct {
	      uInt16	pctrl;
	      uChar	f[14];
	   }		r27;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	unused;
	      }		thead;
	      uChar	f1[4];
	      struct {
	         uInt16	high;
	         uInt16	unused;
	      }		thead1;
	      uChar	f2[4];
	   }		r28;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	unused;
	      }		ttail;
	      uChar	f1[4];
	      struct {
	         uInt16	high;
	         uInt16	unused;
	      }		ttail1;
	      uChar	f2[4];
	   }		r29;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	unused;
	      }		ttmph;
	      uChar	f1[4];
	      struct {
	         uInt16	high;
	         uInt16	unused;
	      }		ttmph1;
	      uChar	f2[4];
	   }		r30;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	unused;
	      }		tptr;
	      uChar	f1[4];
	      struct {
	         uInt16	high;
	         uInt16	unused;
	      }		tptr1;
	      uChar	f2[4];
	   }		r31;
	   struct {
	      uChar	tqsel;
	      uChar	f[15];
	   }		r32;
	   struct {
	      uInt16	tlen;
	      uChar	f[14];
	   }		r33;
	   struct {
	      uInt16	tdsc;
	      uChar	f[14];
	   }		r34;
	   struct {
	      uInt16	pstat;
	      uChar	f[14];
	   }		r35;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	unused;
	      }		rdesc;
	      uChar	f1[4];
	      struct {
	         uInt16	high;
	         uInt16	unused;
	      }		rdesc1;
	      uChar	f2[4];
	   }		r36;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	unused;
	      }		rhead;
	      uChar	f1[4];
	      struct {
	         uInt16	high;
	         uInt16	unused;
	      }		rhead1;
	      uChar	f2[4];
	   }		r37;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	unused;
	      }		rtail;
	      uChar	f1[4];
	      struct {
	         uInt16	high;
	         uInt16	unused;
	      }		rtail1;
	      uChar	f2[4];
	   }		r38;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	unused;
	      }		rtmph;
	      uChar	f1[4];
	      struct {
	         uInt16	high;
	         uInt16	unused;
	      }		rtmph1;
	      uChar	f2[4];
	   }		r39;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	unused;
	      }		rplen;
	      uChar	f1[4];
	      struct {
	         uInt16	high;
	         uInt16	unused;
	      }		rplen1;
	      uChar	f2[4];
	   }		r40;
	   struct {
	      uInt16	rtlen;
	      uChar	f[14];
	   }		r41;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	unused;
	      }		rptr;
	      uChar	f1[4];
	      struct {
	         uInt16	high;
	         uInt16	unused;
	      }		rptr1;
	      uChar	f2[4];
	   }		r42;
	   struct {
	      uInt16	rlen;
	      uChar	f[14];
	   }		r43;
	   struct {
	      uChar	fltr;
	      uChar	f[15];
	   }		r44;
	   struct {
	      uChar	fc;
	      uChar	f[15];
	   }		r45;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		sfq;
	      uChar	f[12];
	   }		r46;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		sfqt;
	      uChar	f[12];
	   }		r47;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		rq;
	      uChar	f[12];
	   }		r48;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		stq;
	      uChar	f[12];
	   }		r49;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		stqt;
	      uChar	f[12];
	   }		r50;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		atq0;
	      uChar	f[12];
	   }		r51;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		atq0t;
	      uChar	f[12];
	   }		r52;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		atq1;
	      uChar	f[12];
	   }		r53;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		atq1t;
	      uChar	f[12];
	   }		r54;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		atq2;
	      uChar	f[12];
	   }		r55;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		atq2t;
	      uChar	f[12];
	   }		r56;
	   struct {
	      uChar	clrdma0;
	      uChar	f[15];
	   }		r57;
	   struct {
	      uChar	clrdma1;
	      uChar	f[15];
	   }		r58;
	   struct {
	      uChar	rev;
	      uChar	f[15];
	   }		r59;
	   struct {
	      uInt32	cfg;
	      uChar	f[12];
	   }		r60;
	};
#endif /* CALYPSO */
