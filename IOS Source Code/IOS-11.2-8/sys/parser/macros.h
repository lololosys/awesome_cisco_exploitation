/* $Id: macros.h,v 3.7.10.9 1996/09/09 01:34:14 xtang Exp $
 * $Source: /release/112/cvs/Xsys/parser/macros.h,v $
 *------------------------------------------------------------------
 * M A C R O S . H
 *
 * Token chain macro definitions.
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: macros.h,v $
 * Revision 3.7.10.9  1996/09/09  01:34:14  xtang
 * CSCdi56632:  unknown clock rates on 4T PA, FSIP
 * Branch: California_branch
 *         add ALT_HELPS to use csb->which in different
 *         driver procedures, and print the
 *         driver specific info. In this case, the clockrate
 *         option table, and csb->which = CFG_INT_CLOCKRATE
 *
 * Revision 3.7.10.8  1996/08/12  16:06:30  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.7.10.7  1996/07/29  21:32:42  rchandra
 * CSCdi64437:  BGP: community display is not user-friendly
 * Branch: California_branch
 * - add user friendly community display
 * - add update throttling code (CSCdi64451)
 * - allow 'set community <additive>' during redistribution into BGP
 *
 * Revision 3.7.10.6  1996/07/09  21:07:40  etrehus
 * CSCdi45163:  transmitter-delay help message is confusing
 * Branch: California_branch
 *
 * Revision 3.7.10.5  1996/06/17  23:35:52  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.7.10.4  1996/05/30  22:44:20  hampton
 * Add parser support for accepting per-protocol access list number
 * ranges.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.7.10.3  1996/05/17  11:39:29  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.2.3  1996/05/05  23:41:44  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.7.2.2  1996/04/01  04:41:49  bchan
 * CSCdi53188:  appn parser multiple definition
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.2.1  1996/03/28  11:29:28  bchan
 * CSCdi52724:  merge tn3280 server code
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.10.2  1996/04/25  23:22:11  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.7.10.1  1996/04/15  21:20:35  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.7  1996/02/21  17:28:21  widmer
 * CSCdi49001:  privilege level configuration doesnt take hub ether
 * Make hub macro priv 0
 *
 * Revision 3.6  1996/02/16  22:42:27  widmer
 * CSCdi49239:  Make time and timesec real parser macros
 *
 * Revision 3.5  1996/01/16  21:18:16  widmer
 * CSCdi33940:  show extended command doesnt work
 * Add keyword_trans macro that doesn't accept the keyword
 *  unless commands have been added to the link point.
 *
 * Revision 3.4  1995/12/14  13:07:54  widmer
 * CSCdi45510:  Add General_String macro to replace other string macros
 * Change Var_String, String, Txt_Line, and Text to use General_String
 * Add fail argument to Priv_Test
 * Change Neg1 to use Keyword_Id
 * Remove Sense macro
 *
 * Revision 3.3  1995/11/27  21:25:00  widmer
 * CSCdi43607:  Add keyword_option macro to parse multiple keywords
 * Also create permit_deny macro using keyword_option macro to parse
 * permit/deny keywords.
 * Change "ip security multilevel" command to use keyword_option macro.
 *
 * Revision 3.2  1995/11/17  18:48:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:20:29  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/26  04:05:14  sluong
 * CSCdi42574:  Remove ipx router isis command from the help menu
 *
 * Revision 2.4  1995/07/26  15:15:18  schrupp
 * CSCdi37690:  Support ATM ESI addressing with ILMI prefix
 *
 * Revision 2.3  1995/06/19  14:49:31  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/15  15:19:26  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  22:28:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef	BOOT
#define	PHELP(str)	NULL
#else	BOOT
#define	PHELP(str)	str
#endif	BOOT

#define OBJ(type,num) PTYPE_ ## type ## ( ## num ## )

#define pfdecl(x) extern void x (parseinfo *)

#define TRANS_PRIV(name, priv)\
    static trans_priv P ## name =\
	{\
            (priv),\
            (priv),\
            TRANS_PRIV_FLAGS_SET(priv),\
	    0\
	}


#define NOP(name, acc, alt)\
    static transition pname(name) = {&pname(acc), &pname(alt),\
			      (trans_func) NOP_action, NULL}

#define	LINK_POINT(name, acc)\
    static dynamic_transition pname(name) =\
    {\
	(dynamic_transition *)&pname(acc),\
	(dynamic_transition *)&pname(link_point),\
	(trans_func) NOP_action,\
	NULL\
    }

#define	LINK_TRANS(name, acc)\
    static dynamic_transition pname(L ## name) =\
    {\
	(dynamic_transition *)&pname(NONE),\
	(dynamic_transition *)&pname(link_trans),\
	(trans_func) NOP_action,\
	NULL\
    };\
    static transition pname(name) = {(transition *) &pname(L ## name),\
			      &pname(acc),\
			      (trans_func) NOP_action, NULL}

#define	LINK_EXIT(name, acc)\
    LINK_TRANS(name, acc)

#define WHITESPACE(name,acc,alt)\
  static transition pname(name) = {&pname(acc), &pname(alt),\
				(trans_func) whitespace_action, NULL}

#define NVGENS(name, acc, func, subfunc)\
  pfdecl(func);\
  static nvgens_struct L ## name = {func, subfunc};\
  static transition pname(name) = {&pname(acc), NULL,\
			    (trans_func) nvgens_action, & L ## name}

#define	NVGENNS(name, acc, func)\
    pfdecl(func);\
    static transition pname(name) =\
    {\
	&pname(acc),\
	NULL,\
	(trans_func) nvgenns_action,\
	func\
    }

#define PRINT(name,acc,str)\
  static print_struct L ## name = {str};\
  static transition pname(name) = {&pname(acc), NULL, (trans_func) print_action, & L ## name}

#define	EOLNS(name,func)\
    pfdecl(func);\
    static transition pname(name) =\
    {\
	NULL,\
	NULL,\
	(trans_func) eolns_action,\
	func\
    }

#define	EOLS(name,func,subfunc)\
  pfdecl(func);\
  static eols_struct L ## name = {func, subfunc};\
  static transition pname(name) = {NULL, NULL, (trans_func) eols_action, & L ## name}

#define ALT_HELPS(name,func,subfunc)\
  pfdecl(func);\
  static eols_struct L ## name = {func, subfunc};\
  static transition pname(name) = {NULL, NULL, (trans_func) alt_helps_action, & L ## name}

#define HELP(name,alt,str)\
  static help_struct L ## name = {PHELP(str), NULL};\
  static transition pname(name) = {NULL, &pname(alt), (trans_func) help_action, & L ## name}

#define DISPLAY(name,acc,str)\
  static help_struct L ## name = {PHELP(str), NULL};\
  static transition pname(name) = {NULL, &pname(acc),\
				(trans_func) display_action, & L ## name}

#define	GENERAL_STRING(name, acc, alt, str, help, flags)\
    static general_string_struct L ## name =\
    {\
	CSBOFF(str),\
	PHELP(help),\
	flags\
    };\
    static transition pname(name) =\
    {\
	&pname(acc),\
	&pname(alt),\
	(trans_func) general_string_action,\
	& L ## name\
    }

#define VAR_STRING(name, acc, alt, str)  \
    GENERAL_STRING(name, acc, alt, str, NULL, STRING_QUOTED)

/* A string delimited by whitespace. */
#define	STRING(name, acc, alt, str, help)\
    GENERAL_STRING(name, acc, alt, str, help,\
		   STRING_QUOTED | STRING_HELP_CHECK)

#define	TXT_LINE(name, acc, alt, str)\
    GENERAL_STRING(name, acc, alt, str, NULL, STRING_WS_OK)

/* A string delimited by EOL. */
#define	TEXT(name, acc, alt, str, help)\
    GENERAL_STRING(name, acc, alt, str, help,\
		   STRING_WS_OK | STRING_HELP_CHECK)

#define CHARACTER(name,acc,alt,character)\
  static char_struct L ## name = {character};\
  static transition pname(name) = {&pname(acc), &pname(alt),\
			    (trans_func) char_action, & L ## name}

#define	CHAR_NUMBER(name, acc, alt, var, help)\
  static number_struct L ## name = {CSBOFF(var), 0, 127, PHELP(help), 0};\
  static transition pname(name) = \
        {&pname(acc), &pname(alt), (trans_func) char_number_action, & L ## name}

#define	CHAR8_NUMBER(name, acc, alt, var, help)\
  static number_struct L ## name = {CSBOFF(var), 0, 255, PHELP(help), 0};\
  static transition pname(name) = \
        {&pname(acc), &pname(alt), (trans_func) char_number_action, & L ## name}

#define FUNC(name, acc, func)\
  pfdecl(func);\
  static transition pname(name) = {&pname(acc), NULL, (trans_func) func_action, func}

/*
 * Testing Macros
 */

/* ASSERT - assert that a C expression is true */

#define	TEST_EXPR(name, true, false, alt, expr)\
    static boolean F ## name (parseinfo *csb, transition *mine)\
    {\
	return((expr) != 0);\
    }\
    static testexpr_struct L ## name = {&pname(false), F ## name};\
    static transition pname(name) = {&pname(true), &pname(alt),\
			      (trans_func) testexpr_action, & L ## name}

#define	ASSERT(name, acc, alt, expr)\
    TEST_EXPR(name, acc, NONE, alt, expr)

#define IFELSE(name, if, else, expr)\
    TEST_EXPR(name, if, else, NONE, expr)

/* EVAL - always accept, and evaluate a C expression */

#define EVAL(name,acc,expr)\
  static void F ## name (parseinfo *csb, transition *mine)\
  { expr;push_node(csb, mine->accept); }\
  static transition pname(name) = {&pname(acc), NULL, (trans_func) F ## name, NULL}

/* TESTVAR - test a variable relative to a constant value */

#define TESTVAR(name, equal, notequal, lt, gt, alt, var, val)\
  static test_struct L ## name = {CSBOFF(var), (uint) val,\
			   &pname(equal), &pname(notequal),\
			   &pname(lt), &pname(gt)};\
  static transition pname(name) = {NULL, &pname(alt),\
			    (trans_func) test_action, & L ## name}

#define	TEST_BOOLEAN(name, nonzero, zero, alt, var)\
    static test_bool_struct L ## name = {&var, &pname(zero), &pname(nonzero)};\
    static transition pname(name) = {NULL, &pname(alt),\
			      (trans_func) test_boolean_action, & L ## name}

#define	TEST_INT(name, nonzero, zero, alt, var)\
    static test_int_struct L ## name = {&var, &pname(zero), &pname(nonzero)};\
    static transition pname(name) = {NULL, &pname(alt),\
			      (trans_func) test_int_action, & L ## name}

#define	TEST_FUNC(name, nonzero, zero, alt, func)\
    static test_func_struct L ## name = {func, &pname(zero), &pname(nonzero)};\
    static transition pname(name) = {NULL, &pname(alt),\
			      (trans_func) test_func_action, & L ## name}

#define	TEST_LINE(name, nonzero, zero, alt, type)\
    static test_line_struct L ## name = {type, &pname(zero), &pname(nonzero)};\
    static transition pname(name) = {NULL, &pname(alt),\
			      (trans_func) test_line_action, & L ## name}

#define	TEST_IDBSTATUS(name, true, false, alt, val)\
    static test_idbstatus_struct L ## name = {&pname(false), val};\
    static transition pname(name) = {&pname(true), &pname(alt),\
			      (trans_func) test_idbstatus_action, & L ## name}

/* Relations simpler than TESTVAR in its entirity */

#define EQUAL(name, acc, alt, var, val)\
  TESTVAR(name, acc, NONE, NONE, NONE, alt, var, val)
#define NEQ(name, acc, alt, var, val)\
  TESTVAR(name, NONE, acc, NONE, NONE, alt, var, val)
#define SPLIT(name, acc, alt, var, val)\
  TESTVAR(name, acc, alt, NONE, NONE, NONE, var, val)

/* SET - always accept, setting a variable to a constant value */

#define SET(name, acc, var, val)\
    static set_struct L ## name = {CSBOFF(var), (uint) val};\
    static transition pname(name) = {&pname(acc), NULL,\
			      (trans_func) set_action, & L ## name}

/*
 * KEYWORD is a complete token, with help, error handling and whitespace.o
 * KEYWORD_OPTWS has optional trailing white-space.
 * KEYWORD_NOWS has no trailing white-space.
 * KEYWORD_MM takes an extra argument for the minimum number of chars required
 *  to match.
 */

#define	GENERAL_KEYWORD(name,acc,alt,str,help,priv,var,val,match,flags)\
    TRANS_PRIV(name, priv);\
    static general_keyword_struct L ## name = {str, PHELP(help), & P ## name,\
					    var, val, match, flags};\
    static transition pname(name) = {&pname(acc),&pname(alt),\
			      (trans_func) general_keyword_action, & L ## name}

#define KEYWORD(name, acc, alt, str, help, priv)\
    TRANS_PRIV(name, priv);\
    static keyword_struct L ## name = {str, PHELP(help), & P ## name};\
    static transition pname(name) = {&pname(acc), &pname(alt),\
			      (trans_func) keyword_action, & L ## name}

#define KEYWORD_OPTWS(name,acc,alt,str,help, priv)\
    TRANS_PRIV(name, priv);\
    static keyword_struct L ## name = {str, PHELP(help), & P ## name};\
    static transition pname(name) = {&pname(acc), &pname(alt),\
			      (trans_func) keyword_optws_action, & L ## name}

#define KEYWORD_NOWS(name,acc,alt,str,help, priv)\
    TRANS_PRIV(name, priv);\
    static keyword_struct L ## name = {str, PHELP(help), & P ## name};\
    static transition pname(name) = {&pname(acc), &pname(alt),\
			      (trans_func) keyword_nows_action,  & L ## name}

#define KEYWORD_MM(name,acc,alt,str,help, priv, count)\
    TRANS_PRIV(name, priv);\
    static keyword_mm_struct L ## name = {str, PHELP(help), & P ## name, count};\
    static transition pname(name) = {&pname(acc), &pname(alt),\
			      (trans_func) keyword_mm_action, & L ## name}

#define	KEYWORD_ID(name, acc, alt, var, val, str, help, priv)\
    TRANS_PRIV(name, priv);\
    static keyword_id_struct L ## name = {str,PHELP(help),&P##name,CSBOFF(var),val};\
    static transition pname(name) = {&pname(acc), &pname(alt),\
			      (trans_func)  keyword_id_action, & L ## name}

#define	KEYWORD_DEBUG(name, acc, alt, var, val, str, help, priv)\
    KEYWORD_ID(name, acc, alt, var, (uint) val, str, help, priv)

#define	KEYWORD_ID_MM(name, acc, alt, var, val, str, help, priv, count)\
    TRANS_PRIV(name, priv);\
    static keyword_id_mm_struct L ## name = {str,PHELP(help),&P##name,CSBOFF(var),val,count};\
    static transition pname(name) = {&pname(acc), &pname(alt),\
			      (trans_func)  keyword_id_mm_action, & L ## name}

#define	KEYWORD_TRANS(name, link, acc, alt, str, help, priv)\
    LINK_TRANS(link, acc);\
    TRANS_PRIV(name, priv);\
    static keyword_struct L ## name =\
    {\
	str,\
	PHELP(help),\
	& P ## name\
    };\
    static transition pname(name) =\
    {\
	&pname(link),\
	&pname(alt),\
	(trans_func) keyword_trans_action,\
	& L ## name\
    }

/*
 * If parse keyword
 *    var |= val
 */
#define	KEYWORD_OR(name, acc, alt, var, val, str, help, priv)\
    TRANS_PRIV(name, priv);\
    static keyword_id_struct L ## name = {str,PHELP(help),&P##name,CSBOFF(var),val};\
    static transition pname(name) = {&pname(acc), &pname(alt),\
			      (trans_func)  keyword_orset_action, & L ## name}

/*
 * If !(var & val) and parse keyword
 *     var |= val
 */
#define	KEYWORD_ORTEST(name, acc, alt, var, val, str, help, priv)\
    TRANS_PRIV(name, priv);\
    static keyword_id_struct L ## name = {str,PHELP(help),&P##name,CSBOFF(var),val};\
    static transition pname(name) = {&pname(acc), &pname(alt),\
			      (trans_func)  keyword_ortest_action, & L ## name}

#define	KEYWORD_OPTIONS(name, acc, alt, options, var, priv, flags)\
    TRANS_PRIV(name, priv);\
    static keyword_option_struct L ## name =\
    {\
	options,\
	CSBOFF(var),\
	& P ## name,\
	flags\
    };\
    static transition pname(name) =\
    {\
	&pname(acc),\
	&pname(alt),\
	(trans_func) keyword_option_action,\
	& L ## name\
    }

#define	PERMIT_DENY(name, acc, alt, var, priv)\
    KEYWORD_OPTIONS(name, acc, alt, permitdeny_options, var, priv, 0);

/*
 * Number macros
 */

#define GENERAL_NUMBER(name,acc,alt,var,lower,upper,help,flags)\
    static number_struct L ## name = {CSBOFF(var), lower, upper, PHELP(help), flags};\
    static transition pname(name) =\
      {&pname(acc), &pname(alt), (trans_func) general_number_action, & L##name}

#define NUMBER(name, acc, alt, var, lower, upper, help)\
    GENERAL_NUMBER(name, acc, alt, var, lower, upper, help,\
		   (NUMBER_HEX | HEX_ZEROX_OK | NUMBER_OCT | OCT_ZERO_OK |\
		    NUMBER_DEC | NUMBER_WS_OK | NUMBER_HELP_CHECK))

#define	NUMBER_NV(name, acc, alt, var, lower, upper, help)\
    GENERAL_NUMBER(name, acc, alt, var, lower, upper, help,\
		   (NUMBER_HEX | HEX_ZEROX_OK | NUMBER_OCT | OCT_ZERO_OK |\
		    NUMBER_DEC | NUMBER_WS_OK | NUMBER_HELP_CHECK |\
		    NUMBER_NV_RANGE))

#define DECIMAL(name, acc, alt, var, lower, upper, help)\
    GENERAL_NUMBER(name, acc, alt, var, lower, upper, help,\
		   (NUMBER_DEC | NUMBER_WS_OK | NUMBER_HELP_CHECK))

/* Handle a decimal number, without help and whitespace. */
#define IDECIMAL(name,acc,alt,var,lower,upper)\
    GENERAL_NUMBER(name, acc, alt, var, lower, upper, NULL,\
    (NUMBER_DEC | NUMBER_NO_WS_OK))

/* Handle a decimal number, without help and whitespace. */
#define INUMBER(name,acc,alt,var,lower,upper)\
    GENERAL_NUMBER(name, acc, alt, var, lower, upper, NULL,\
    (NUMBER_HEX | HEX_ZEROX_OK | NUMBER_OCT | OCT_ZERO_OK |\
     NUMBER_DEC | NUMBER_NO_WS_OK))

/* HEXADECIMAL matches an unsigned hex number, minimum one
 * digit, and assigns it to a int.  This function works on numbers
 * which don't begin with '0x'. */

#define HEXADECIMAL(name,acc,alt,var)\
    GENERAL_NUMBER(name, acc, alt, var, 0, MAXUINT, NULL,\
    (NUMBER_HEX | HEX_NO_ZEROX_OK | NUMBER_NO_WS_OK))

#define	HEXNUM(name, acc, alt, var, help)\
    GENERAL_NUMBER(name, acc, alt, var, 0, MAXUINT, help,\
        (NUMBER_HEX | HEX_NO_ZEROX_OK | NUMBER_WS_OK | NUMBER_HELP_CHECK))

#define	HEXDIGIT(name, acc, alt, var, lower, upper, help)\
    GENERAL_NUMBER(name, acc, alt, var, lower, upper, help,\
        (NUMBER_HEX | HEX_NO_ZEROX_OK | NUMBER_WS_OK | NUMBER_HELP_CHECK))

#define OPT_HEXNUM(name, acc, alt, var, help)\
    GENERAL_NUMBER(name, acc, alt, var, 0, MAXUINT, help,\
       (NUMBER_HEX | HEX_NO_ZEROX_OK | HEX_ZEROX_OK |\
	NUMBER_WS_OK | NUMBER_HELP_CHECK))

#define OPT_HEXDIGIT(name, acc, alt, var, lower, upper, help)\
    GENERAL_NUMBER(name, acc, alt, var, lower, upper, help,\
       (NUMBER_HEX | HEX_NO_ZEROX_OK | HEX_ZEROX_OK |\
	NUMBER_WS_OK | NUMBER_HELP_CHECK))

#define	ZEROXHEXNUM(name, acc, alt, var, help)\
    GENERAL_NUMBER(name, acc, alt, var, 0, MAXUINT, help,\
       (NUMBER_HEX | HEX_ZEROX_OK | NUMBER_WS_OK | NUMBER_HELP_CHECK))

/* 
 * IOCTAL matches an unsigned octal number, minimum one
 * digit, and assigns it to a int
 */
#define IOCTAL(name,acc,alt,var)\
    GENERAL_NUMBER(name, acc, alt, var, 0, MAXUINT, NULL,\
    (NUMBER_OCT | OCT_ZERO_OK | OCT_NO_ZERO_OK |\
     NUMBER_NO_WS_OK))

/*
 * This is an OCTAL number, with help, error handling and
 * whitespace
 */
#define OCTAL(name,acc,alt,var,help)\
    GENERAL_NUMBER(name, acc, alt, var, 0, MAXUINT, help,\
   (NUMBER_OCT | OCT_ZERO_OK | OCT_NO_ZERO_OK |\
    NUMBER_WS_OK | NUMBER_HELP_CHECK))

#define	SIGNED_DEC(name,acc,alt,var,lower,upper,help)\
    static signed_number_struct L ## name = {CSBOFF(var), lower, upper, PHELP(help)};\
    static transition pname(name) =\
      {&pname(acc), &pname(alt), (trans_func) signed_number_action, & L##name}

#define	NUMBER_FUNC(name, acc, alt, var, func, help)\
    static number_func_struct L ## name =\
      {CSBOFF(var), func, PHELP(help)};\
    static transition pname(name) =\
      {&pname(acc), &pname(alt), (trans_func) number_func_action, & L ## name}

#define	NUMBER_HELP_FUNC(name, acc, alt, var, func)\
    static number_help_func_struct L ## name =\
      {CSBOFF(var), func};\
    static transition pname(name) =\
      {&pname(acc), &pname(alt), (trans_func) number_help_func_action, & L ## name} 

#define GENERAL_PARAMS(name, alt, keyword, var, lower, upper, func, subfunc, keyhelp, varhelp, priv, flags)\
    pfdecl(func);\
    TRANS_PRIV(name, priv);\
    static params_struct L ## name = {keyword, CSBOFF(var), lower, upper,\
			     func, subfunc, PHELP(keyhelp),\
			     PHELP(varhelp),\
			     & P ## name, flags};\
    static transition pname(name) = {NULL, & pname(alt),\
				(trans_func)  params_action, & L ## name}

#define	PARAMS(name, alt, keyword, var, lower, upper, func, subfunc, keyhelp,varhelp,priv)\
    GENERAL_PARAMS(name, alt, keyword, var, lower, upper, func,\
		   subfunc, keyhelp, varhelp, priv,\
		   (KEYWORD_WS_OK | KEYWORD_HELP_CHECK | NUMBER_DEC |\
		    NUMBER_WS_OK | NUMBER_HELP_CHECK))

#define	PARAMS_KEYONLY(name, alt, keyword, var, lower, upper, func, subfunc, keyhelp, varhelp, priv)\
    GENERAL_PARAMS(name, alt, keyword, var, lower, upper, func,\
		   subfunc, keyhelp, varhelp, priv,\
		   (KEYWORD_WS_OK | KEYWORD_HELP_CHECK | NUMBER_DEC |\
		    NUMBER_WS_OK | NUMBER_HELP_CHECK | NO_NUMBER))

#define	TTYLINE_GENERAL(name, acc, alt, first, last, help, flags)\
    static ttyline_struct L ## name = {CSBOFF(first),CSBOFF(last),flags,PHELP(help)};\
    static transition pname(name) =\
        {&pname(acc), &pname(alt), (trans_func) ttyline_action, & L ## name}

#define TTYLINE(name, acc, alt, var, help)\
    TTYLINE_GENERAL(name, acc, alt, var, var, help, CON_ABSOLUTE)

#define	TTYLINE_RANGE(name, acc, alt, first, last, help, flags)\
    TTYLINE_GENERAL(name, acc, alt, first, last, help, flags|CON_RANGE)

/* NOPREFIX
 * Go to 'accept' if there isn't a 'no' prefix.
 * Consume the remainder of the input and go to 'alt' if there
 * was a 'no' prefix.  This is somewhat backwards from other macros
 * in that the major action takes place before going to the alternate
 * state, but it keeps the major thread of the command token chains
 * similar to other tokens.
 */
#define	NOPREFIX(name, acc, alt)\
    static transition pname(name) = {&pname(acc), &pname(alt),\
			    (trans_func) noprefix_action, NULL}

/* IPADDR gets an IP address and stores it in addr.
 * IPADDR_NAME gets an IP address or hostname and 
 * converts to an address stored in addr.
 */
#define IPADDR_NAME(name, acc, alt, addr, help)\
    static addrparse_struct L ## name = {CSBOFF(addr), PHELP(help), ADDR_IP,\
				    ADDR_NAME_LOOKUP};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
				     (trans_func) generic_addr_action, & L ## name}

#define IPADDR(name,acc,alt,addr, help)\
    static addrparse_nf_struct L ## name = {CSBOFF(addr), PHELP(help), ADDR_IP};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
				   (trans_func) generic_addr_noflag_action, & L ## name}

#define IPMASK(name,acc,alt,mask, help)\
    static ipmask_struct L ## name = {CSBOFF(mask), PHELP(help)};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
				   (trans_func) ipmask_action, & L ## name}

#define IPADDR_MASK(name, acc, alt, addr, mask, help)\
    static ipaddrmask_struct L ## name = { CSBOFF(addr), CSBOFF(mask), help};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
			      (trans_func) ipaddrmask_action, & L ## name}

/*
 * An interface name and unit number.  A subset of all
 * interfaces may be searched by a logical 'or' of the desired
 * interfaces into the 'valid_ifs' argument.  The csb->idb1
 * variable will point to the selected interface if it is valid.
 */
#define INTERFACE_COMMAND(name,acc,alt,var,valid_ifs,flag,keyword,help)\
    static interface_struct IF ## name = {CSBOFF(var), valid_ifs, flag, keyword, help};\
    static transition pname(name) =\
        {&pname(acc), &pname(alt), (trans_func) interface_action, & IF ## name}

#define INTERFACE(name, acc, alt, var, valid_ifs)\
    INTERFACE_COMMAND(name, acc, alt, var, valid_ifs, 0, NULL, NULL);

/*
 * Same as an interface command, but can create virtual idb's if it's a
 * forward reference.  Should only be used in interface subcommands.
 */

#define FORWARD_INTERFACE(name, acc, alt, var, valid_ifs)\
    INTERFACE_COMMAND(name, acc, alt, var, valid_ifs, PARSE_CREATE_IDB,\
		      NULL, NULL);

#define UNIT_NUMBER(name, acc, alt, var, valid_if)\
    INTERFACE_COMMAND(name, acc, alt, var, valid_if,\
		      PARSE_UNIT_ONLY, NULL, NULL);

#define	INTERFACE_KEYWORD(name, acc, alt, var, valid_ifs, keyword, help)\
    INTERFACE_COMMAND(name, acc, alt, var, valid_ifs,\
		      PARSE_INTERFACE_KEYWORD, keyword, help);
#define	HUB(name, acc, alt, hub, num, help)\
    NUMBER(N ## name, acc, no_alt, num, 0, 0, "Hub number");\
    KEYWORD_ID(E ## name, N ## name, alt, hub, HUB_ETHER,\
	       "ether", "Ether " ## help, PRIV_MIN);\
    IFELSE(name, E ## name, N ## name, netherhubports != 0);

#define	HUB_RANGE(name, acc, alt, first, last, help)\
    static hub_struct L ## name = {CSBOFF(first),CSBOFF(last),PHELP(help)};\
    static transition pname(name) =\
{&pname(acc), &pname(alt), (trans_func) hub_action, & L ## name}

/*
 *************************************************************
 *
 *
 *
 *
 * The derived transitions start here.
 */

/* An IPX or Novell keyword */
#define NOVELL_IPX_KEYWORD(name, acc, alt, help, flags) \
   KEYWORD_MM (IPXKEY ## name, acc, alt, \
	       "ipx", help, flags, 3); \
   KEYWORD_MM (name, acc, IPXKEY ## name, \
	       "novell", help, flags | PRIV_HIDDEN | PRIV_NONVGEN, 3);

#define NOVELL_IPX_KEYWORD_ID(name, acc, alt, var, val, help, flags) \
   SET	(NOV_IPX_KEYWORD_ID ## name, acc, var, val); \
   NOVELL_IPX_KEYWORD(name, NOV_IPX_KEYWORD_ID ## name, alt, help, flags);

/* DAY - A day of the week */

#define DAY(name,acc,alt,var,help) \
    static day_struct L ## name = {CSBOFF(var), PHELP(help)};\
    static transition pname(name) =\
  {&pname(acc), &pname(alt), (trans_func) day_action, & L##name}

/* MONTH - A month of the year */

#define MONTH(name,acc,alt,var,help) \
    static month_struct L ## name = {CSBOFF(var), PHELP(help)};\
    static transition pname(name) =\
  {&pname(acc), &pname(alt), (trans_func) month_action, & L##name}

/* BGP_COMMUNITY - BGP community in aa:nn format */

#define BGP_COMMUNITY(name, acc, alt, var1, var2, help)\
    static bgp_community_struct L##name = {CSBOFF(var1), CSBOFF(var2), help};\
    static transition pname(name) =\
    {&pname(acc), &pname(alt), (trans_func) bgp_community_action, & L##name}

#define	GENERAL_TIME(name, acc, alt, var1, var2, var3, help)\
    static timesec_struct L ## name =\
    {\
	var1,\
	var2,\
	var3,\
	help\
    };\
    static transition pname(name) =\
    {\
	&pname(acc),\
	&pname(alt),\
	(trans_func) timesec_action,\
	& L ## name\
    }

/* TIME - A time (24 hours, without seconds) */
#define	TIME(name, acc, alt, var1, var2, help)\
    GENERAL_TIME(name, acc, alt, CSBOFF(var1),\
		 CSBOFF(var2), MAXUINT, help)

/* TIMESEC - A time (24 hours, with seconds) */
#define	TIMESEC(name, acc, alt, var1, var2, var3, help)\
    GENERAL_TIME(name, acc, alt, CSBOFF(var1),\
		 CSBOFF(var2), CSBOFF(var3), help)

/* GROUPLIST - A LAT service list for bridge group filtering of LAT services.
 */

#define	GROUPLIST(name,acc,alt,str,help) \
    static string_struct L ## name = {CSBOFF(str), PHELP(help)};\
    static transition pname(name) = {&pname(acc), &pname(alt),\
		(trans_func) grouplist_action, & L ## name}

/* A DecNET address */

#define DNADDR(name,acc,alt,addr,help)\
    static addrparse_nf_struct L ## name = {CSBOFF(addr), PHELP(help), ADDR_DECNET};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
				     (trans_func) generic_addr_noflag_action,& L ## name}

/* An NSAP address */
#define NSAPADDR(name, acc, alt, addr, help)\
    static addrparse_nf_struct L ## name = {CSBOFF(addr), PHELP(help), ADDR_CLNS};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
				   (trans_func) generic_addr_noflag_action, & L ## name}

/* An SNPA address */
#define	SNPAADDR(name, acc, alt, addr, help)\
    static addrparse_struct L ## name = {CSBOFF(addr), PHELP(help), 0, 0};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
			    (trans_func) snpaaddr_action, & L ## name}

/* An SMDS address */
#define	SMDSADDR(name, acc, alt, addr, help)\
    static addrparse_nf_struct L ## name = {CSBOFF(addr), PHELP(help),\
				   ADDR_PARSE(STATION_SMDS64)};\
    static transition pname(name) = {&pname(acc), & pname(alt),\
			    (trans_func) generic_addr_noflag_action, & L##name}

/* An ATM NSAP address */
#define ATMNSAPADDR(name, acc, alt, addr, help)\
    static addrparse_nf_struct L ## name = {CSBOFF(addr), PHELP(help),\
                                   ADDR_PARSE(STATION_ATMNSAP)};\
    static transition pname(name) = {&pname(acc), & pname(alt),\
                            (trans_func) generic_addr_noflag_action, & L##name}

/* A Masked ATM NSAP address */
#define MASKEDATMNSAPADDR(name, acc, alt, addr, help)\
    static addrparse_nf_struct L ## name = {CSBOFF(addr), PHELP(help),\
                                   ADDR_PARSE(STATION_MASKED_ATMNSAP)};\
    static transition pname(name) = {&pname(acc), & pname(alt),\
                            (trans_func) generic_addr_noflag_action, & L##name}

/* An ATM ESI address */
#define ATMESIADDR(name, acc, alt, addr, help)\
  addrparse_nf_struct L ## name = {CSBOFF(addr), PHELP(help),\
                                   ADDR_PARSE(STATION_ATMESI)};\
  transition pname(name) = {&pname(acc), & pname(alt),\
                            (trans_func) generic_addr_noflag_action, & L##name}

/* An XNS address */

#define XNSADDR(name, acc, alt, addr, help)\
    static addrparse_nf_struct L ## name = {CSBOFF(addr), PHELP(help), ADDR_XNS};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
			    (trans_func) generic_addr_noflag_action, & L##name}

/* An XNS host address (does not include the net address).  The
 * net portion of the address is set to zero.
 */

#define XNSHOST(name, acc, alt, addr, help)\
    static addrparse_struct L ## name = {CSBOFF(addr), PHELP(help),\
				  ADDR_XNS, ADDR_HOST_ADDR_ONLY};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
			      (trans_func) generic_addr_action,& L ## name}

/* A Novell address */

#define NOVADDR(name, acc, alt, addr, help)\
    static addrparse_nf_struct L ## name = {CSBOFF(addr), PHELP(help), ADDR_NOVELL};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
			      (trans_func) generic_addr_noflag_action,&L##name}

/* A Novell host (does not include net) */

#define NOVHOST(name, acc, alt, addr, help)\
    static addrparse_struct L ## name = {CSBOFF(addr), PHELP(help),\
				  ADDR_NOVELL, ADDR_HOST_ADDR_ONLY};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
			      (trans_func) generic_addr_action,& L ## name}

/* An Apollo address */
#define APOLLOADDR(name, acc, alt, addr, help)\
    static addrparse_nf_struct L ## name = {CSBOFF(addr), PHELP(help), ADDR_APOLLO};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
			      (trans_func) generic_addr_noflag_action,&L##name}

/* An E164 address, used by FR SVCs, Use the X121 address code for now */
#define E164ADDR(name, acc, alt, addr, help)\
    static addrparse_nf_struct L ## name = {CSBOFF(addr), PHELP(help),\
	ADDR_X121};\
    static transition pname(name) = {&pname(acc), & pname(alt),\
				(trans_func) generic_addr_noflag_action, & L##name}

/* An X.121 address */
#define X121ADDR(name, acc, alt, addr, help)\
    static addrparse_nf_struct L ## name = {CSBOFF(addr), PHELP(help), ADDR_X121};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
			      (trans_func) generic_addr_noflag_action,&L##name}

#define X121ADDR_NAME(name, acc, alt, addr, help)\
    static addrparse_struct L ## name = {CSBOFF(addr), PHELP(help),\
			          ADDR_X121, ADDR_NAME_LOOKUP};\
    static transition pname(name) = {&pname(acc), &pname(alt),\
			      (trans_func) generic_addr_action, & L ## name}

/* An AppleTalk address */

#define ATADDR(name, acc, alt, addr, help)\
    static addrparse_nf_struct L ## name = {CSBOFF(addr), PHELP(help), ADDR_APPLE};\
    static transition pname(name) = {&pname(acc),&pname(alt),\
			       (trans_func) generic_addr_noflag_action,&L ## name}

/* A Vines address */

#define VINADDR(name, acc, alt, addr, help)\
    static addrparse_struct L ## name = {CSBOFF(addr), PHELP(help),\
				  ADDR_VINES, ADDR_NAME_LOOKUP};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
			      (trans_func) generic_addr_action,&L##name}

#define VINESNET(name, acc, alt, addr, help)\
    static addrparse_struct L ## name = {CSBOFF(addr), PHELP(help),\
				    ADDR_VINES, ADDR_NAME_LOOKUP|ADDR_HOST_NET_ONLY};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
			      (trans_func) generic_addr_action,& L ## name}

#define VINESNETNUM(name, acc, alt, addr, help)\
    static addrparse_struct L ## name = {CSBOFF(addr), PHELP(help),\
				  ADDR_VINES, ADDR_HOST_NET_ONLY};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
			      (trans_func) generic_addr_action,& L ## name}

#define ENADDR(name, acc, alt, addr, help)\
    static addrparse_nf_struct L ## name = {CSBOFF(addr), PHELP(help),\
				     ADDR_PARSE(STATION_IEEE48)};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
			      (trans_func) generic_addr_noflag_action,&L##name}

/* An STUN address */
#define STUNADDR(name, acc, alt, addr, help)\
    static addrparse_nf_struct L ## name = {CSBOFF(addr), PHELP(help), ADDR_STUN};\
    static transition pname(name) = {&pname(acc),&pname(alt),\
			      (trans_func) generic_addr_noflag_action,&L##name}

/* A BSTUN address */
#define BSTUNADDR(name, acc, alt, addr, help)\
    static addrparse_nf_struct L ## name = {CSBOFF(addr), PHELP(help), ADDR_BSTUN};\
    static transition pname(name) = {&pname(acc),&pname(alt),\
			      (trans_func) generic_addr_noflag_action,&L##name}

/* A qllc address */

#define QLLCADDR(name, acc, alt, addr, help)\
    static addrparse_nf_struct L ## name = {CSBOFF(addr), PHELP(help), ADDR_QLLC};\
    static transition pname(name) = {&pname(acc),&pname(alt),\
			       (trans_func) generic_addr_noflag_action,&L ## name}

/* A llc2 address */

#define LLC2ADDR(name, acc, alt, addr, help)\
    static addrparse_nf_struct L ## name = {CSBOFF(addr), PHELP(help), ADDR_LLC2};\
    static transition pname(name) = {&pname(acc),&pname(alt),\
			       (trans_func) generic_addr_noflag_action,&L ## name}

/* APPN Network names */

#define	APPN_FQNAME(name, acc, alt, str, help)\
    static string_struct L ## name = {CSBOFF(str), PHELP(help)};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
			    (trans_func) appn_fqname_action, & L ## name}

#define	APPN_NAME_STAR(name, acc, alt, str, help)\
    static string_struct L ## name = {CSBOFF(str), PHELP(help)};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
			    (trans_func) appn_name_star_action, & L ## name}

#define	APPN_TYPE_A_NAME(name, acc, alt, str, help)\
    static string_struct L ## name = {CSBOFF(str), PHELP(help)};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
			    (trans_func) appn_type_a_name_action, & L ## name}

#define	APPN_PCID(name, acc, alt, str, help)\
    static string_struct L ## name = {CSBOFF(str), PHELP(help)};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
			    (trans_func) appn_pcid_action, & L ## name}

#define	APPN_TYPE_A_NAME_STAR(name, acc, alt, str, help)\
    static string_struct L ## name = {CSBOFF(str), PHELP(help)};\
    static transition pname(name) = {& pname(acc), & pname(alt),\
			    (trans_func) appn_type_a_name_star_action, & L ## name}

/* Negative 1 (-1), used as the wild-card in many protocols */
#define NEG1(name, acc, alt, net, help)\
    KEYWORD_ID(name, acc, alt, net, MAXUINT, "-1", help, PRIV_MIN)

/* ROUTER - parses a routing process name and <AS> or <tag> for those
 * protocols that use them.
 */
/* (rtrstruct so far is just router_name from PARSER.H). */

#define	GENERAL_ROUTER(name, acc, alt, rtrstruct, flags)\
    static routerparse_struct L ## name = {CSBOFF(rtrstruct), flags};\
    static transition pname(name) = {&pname(acc), &pname(alt),\
			      (trans_func) router_action, & L ## name}

#define	ROUTER(name, acc, alt)\
    GENERAL_ROUTER(name, acc, alt, router_name,\
		   (PROC_BGP  | PROC_EGP | PROC_IGRP | PROC_MOBILE |\
		    PROC_IGRP2 | PROC_RIP | PROC_ISIS | PROC_ISOIGRP |\
		    PROC_OSPF | PROC_STATIC | RMATCH_IP | RMATCH_OSI |\
		    PROC_ODR |\
		    RMATCH_ARGS))
		    

#define	NOVELL_ROUTER(name, acc, alt)\
    GENERAL_ROUTER(name, acc, alt, router_name,\
		   (RMATCH_NOVELL | PROC_IGRP2 | PROC_RIP | \
		    PROC_NLSP | RMATCH_ARGS))

/*
 * ROUTINGPROTO - parses the name of an IP routing protocol source name
 * and <AS> or <pid> or <tag> for those protocols that use them.
 * csb->pdb1 will point to the pdb of the matching source, or will be -1
 * to indicate no existing pdb currently exists.
 */

#define	ROUTINGPROTO(name, acc, alt)\
    GENERAL_ROUTER(name, acc, alt, router_name,\
		   (PROC_BGP | PROC_CONNECTED | PROC_EGP |\
		    PROC_IGRP | PROC_IGRP2 | PROC_RIP | PROC_ISIS |\
		    PROC_OSPF | PROC_ODR | PROC_STATIC | RMATCH_IP |\
		    RMATCH_ARGS | RMATCH_NO_ARGS))

#define	ENCAPS(name, acc, alt, var, val, keyword, help, status)\
    KEYWORD_ID(ENCAPS_ ## name, acc, NONE, var, val, \
	       keyword, help, PRIV_CONF);\
    TEST_IDBSTATUS(name, ENCAPS_ ## name, NONE, alt, status)

#define	ENCAPS_SUBIF(name, acc, alt, var, val, keyword, help, assert)\
   KEYWORD_ID(ENCAPS_ ## name, acc, NONE, var, val, \
              keyword, help, PRIV_CONF|PRIV_SUBIF);\
   ASSERT(name, ENCAPS_ ## name, alt, assert)

#define	ENCAPS_HIDDEN(name, acc, alt, var, val, keyword, help, status)\
    KEYWORD_ID(ENCAPS_ ## name, acc, NONE, var, val, keyword, help, \
	       PRIV_CONF|PRIV_HIDDEN);\
    TEST_IDBSTATUS(name, ENCAPS_ ## name, NONE, alt, status)

/*
 * Hexadecimal data stream.  Used in 'llc2 send' command.
 */

#define HEXDATA(name, acc, alt, buf, count, help, bufmax)\
    static hexdata_struct L ## name = {CSBOFF(buf), CSBOFF(count),\
				  PHELP(help), bufmax};\
    static transition pname(name) = {&pname(acc), &pname(alt),\
				       (trans_func) hexdata_action, & L ## name}

/*
 * Appletalk cable range
 */
#define CRANGE(name, acc, alt, start, end)\
  HELP (CRH ## name, NONE, "Start-End\t");\
  WHITESPACE(CR4 ## name, acc, CRH ## name);\
  IDECIMAL(CR3 ## name, CR4 ## name, CRH ## name, end, 0, 65279);\
  CHARACTER(CR1 ## name, CR3 ## name, CRH ## name, '-');\
  IDECIMAL(CR0 ## name, CR1 ## name, alt, start, 0, 65279);\
  HELP (name, CR0 ## name, "  Start-End\tApple cable range")

/*
 * Decnet UIC
 */
#define DNUIC(name, acc, alt, group, user)\
  HELP (UICH ## name, NONE, "[grp,usr]\t");\
  WHITESPACE (UIC5 ## name, acc, UICH ## name);\
  CHARACTER (UIC4 ## name, UIC5 ## name, UICH ## name, ']');\
  INUMBER (UIC3 ## name, UIC4 ## name, UICH ## name, user, 0, MAXUINT);\
  CHARACTER (UIC2 ## name, UIC3 ## name, UICH ## name, ',');\
  INUMBER (UIC1 ## name, UIC2 ## name, UICH ## name, group, 0, MAXUINT);\
  CHARACTER (UIC0 ## name, UIC1 ## name, alt, '[');\
  HELP (name, UIC0 ## name, "  [grp,usr]\tDEC User Id Code")

/*
 * X3 pair
 */
#define	X3PAIR(name, acc, alt, num1, num2, help)\
    HELP (X3E ## name, NONE, "  <0-22>:<0-255>\t");\
    WHITESPACE (X3D ## name, acc, X3E ## name);\
    IDECIMAL (X3C ## name, X3D ## name, X3E ## name, num2, 0, 255);\
    CHARACTER (X3B ## name, X3C ## name, X3E ## name, ':');\
    IDECIMAL (X3A ## name, X3B ## name, alt, num1, 0, 22);\
    HELP (name, X3A ## name, "  <0-22>:<0-255>\t" help "\n")

#define	PROTOADDR(name,acc,alt,num,addr,spec)\
    static protoaddr_struct L ## name = {CSBOFF(num), CSBOFF(addr), spec};\
    static transition pname(name) = {&pname(acc), &pname(alt),\
					 (trans_func) protoaddr_action, & L ## name}

/* New protocol name/address parsing structure */

#define PROTONAME(name, acc, alt, variable, allowed)\
    static protoname_struct L ## name = {CSBOFF(variable), allowed};\
    static transition pname(name) =\
      {&pname(acc), &pname(alt), (trans_func) protoname_action, & L ## name}

#define	TEST_MULTIPLE_FUNCS(name, equal, notequal, alt)\
    static test_struct L ## name = {0, 0, &pname(equal), &pname(notequal), NULL, NULL};\
    static transition pname(name) = {NULL, &pname(alt),\
                            (trans_func) multiple_funcs_action, & L ## name}

/*
 * Connection transport protocols
 */

#define TRANSPORT(name, acc, alt, return, oneonly)\
    static transport_struct L ## name = {CSBOFF(return), (oneonly)};\
    static transition pname(name) =\
      {&pname(acc), &pname(alt), (trans_func) transport_action, & L ## name}

#define	PRIV_TEST(name, acc, fail, alt, priv)\
    TRANS_PRIV(name, priv);\
    static priv_struct L ## name = {& P ## name, &pname(fail)};\
    static transition pname(name) = {&pname(acc), &pname(alt),\
			      (trans_func) priv_action, & L ## name}

#define	MODE(name, acc, alt, pmp, flag)\
    static mode_struct L ## name = {CSBOFF(pmp), flag};\
    static transition pname(name) = {&pname(acc), &pname(alt),\
			      (trans_func) mode_action, & L##name}

#define	NVCOMMENT(name, acc)\
    static transition pname(name) =\
    {\
	&pname(acc),\
	&pname(NONE),\
	(trans_func) comment_action,\
	NULL\
    }

/*
 *  Common help strings
 */
extern const char parser_help_default_keyword[];
extern const char parser_help_no_keyword[];
extern const char parser_default_keyword[];
extern const char parser_no_keyword[];

#define NO_OR_DEFAULT(name, acc, flags) \
   SET       (name ## setsense, acc, sense, FALSE); \
   KEYWORD_ID(name ## default, name ## setsense, acc, set_to_default, TRUE, \
	      parser_default_keyword, parser_help_default_keyword, flags); \
   KEYWORD   (name, name ## setsense, name ## default, \
	      parser_no_keyword, parser_help_no_keyword, flags);

/*
 * Temporary macros for the access-list transition period.  These can be
 * used instead of the macros NUMBER and PARAMS_KEYWORD (respectively) in
 * order to accept both new and old access number ranges.  One of the
 * number ranges, either new or old, will always be hidden.
 */

#define ACCESS_LIST(name,acc,alt,var,old_low,old_high,new_low,new_high,help)\
    EVAL(name ## _e, acc, GET ## var = GET ## var - new_low + old_low);\
    GENERAL_NUMBER(name ## _n, name ## _e, no_alt, var, new_low, new_high, NULL,\
		   (NUMBER_HEX | HEX_ZEROX_OK | NUMBER_OCT | OCT_ZERO_OK |\
		    NUMBER_DEC | NUMBER_WS_OK | NUMBER_HELP_CHECK)); \
    PRIV_TEST(name ## _t, name ## _n, NONE, alt, PRIV_HIDDEN); \
    GENERAL_NUMBER(name, acc, name ## _t, var, old_low, old_high, help,\
		   (NUMBER_HEX | HEX_ZEROX_OK | NUMBER_OCT | OCT_ZERO_OK |\
		    NUMBER_DEC | NUMBER_WS_OK | NUMBER_HELP_CHECK))

#define PARAMS_ALIST(name, alt, keyword, var, lower, upper, lower2, upper2, func, subfunc, keyhelp, varhelp, priv)\
    pfdecl(func);\
    TRANS_PRIV(name, priv);\
    static params_struct_alist L ## name = {keyword, CSBOFF(var), lower, upper,\
		   func, subfunc, PHELP(keyhelp), PHELP(varhelp), & P ## name,\
		   (KEYWORD_WS_OK | KEYWORD_HELP_CHECK | NUMBER_DEC |\
		    NUMBER_WS_OK | NUMBER_HELP_CHECK | NO_NUMBER), \
		   lower2, upper2};\
    static transition pname(name) = {NULL, & pname(alt),\
				(trans_func)  params_action_alist, & L ## name}
