/* $Id: generic.h,v 3.2 1995/11/17 18:51:08 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tn3270/generic.h,v $
 *------------------------------------------------------------------
 * G E N E R I C . H
 *
 * July, 1994, Bill Palter
 * 
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: generic.h,v $
 * Revision 3.2  1995/11/17  18:51:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:07:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef notdef
char *keysgeneric[] = {
	"generic { clear = '^z'; flinp = '^x'; enter = '^m'; delete = '^d' | '^?';",
	"	synch = '^r'; reshow = '^v'; eeof = '^e'; tab = '^i';",
	"	btab = '^b'; nl = '^n'; left = '^h'; right = '^l';",
	"	up = '^k'; down = '^j'; einp = '^w'; reset = '^t';",
	"	xoff = '^s'; xon = '^q'; escape = '^c'; ferase = '^u';",
	"	insrt = '\\E ';",
	"	pa1 = '^p1' | '^p6'; pa2 = '^p2' '^p7'; pa3 = '^p3';",
	"	pfk1 = '\\E1'; pfk2 = '\\E2'; pfk3 = '\\E3'; pfk4 = '\\E4';",
	"	pfk5 = '\\E5'; pfk6 = '\\E6'; pfk7 = '\\E7'; pfk8 = '\\E8';",
	"	pfk9 = '\\E9'; pfk10 = '\\E0'; pfk11 = '\\E-'; pfk12 = '\\E=';",
	"	pfk13 = '\\E!'; pfk14 = '\\E@'; pfk15 = '\\E#'; pfk16 = '\\E$';",
	"	pfk17 = '\\E%'; pfk18 = '\\E\\^'; pfk19 = '\\E&'; pfk20 = '\\E*';",
	"	pfk21 = '\\E('; pfk22 = '\\E)'; pfk23 = '\\E_'; pfk24 = '\\E+';",
	"	ebcdic_40 = ' ';",
	"}",
	"vt100 | vt200 | vt220 | vt240 | vt200-80 | vt300 | vt400 | vt100nam |pt100 | vt125 | vt102 | direct831 | tek4125 | pcplot {",
	"enter = '^m';",
	"clear = '^z' | '\\EOM' | '\\3M';",
	"help = 		'\\E[28~' | '\\EH' 	| '\\C28~';",
	"capture = 	'^t' 	 | '\\E[29~' 	| '\\C29~';",
	"nl = '^?';",
	"tab = '^i';",
	"btab = '^b';",
	"left  = '^h' | '\\E[D' | '\\EOD' | '\\3D' | '\\CD';",
	"right = '^l' | '\\E[C' | '\\EOC' | '\\3C' | '\\CC';",
	"up    = '^k' | '\\E[A' | '\\EOA' | '\\3A' | '\\CA';",
	"down  = '^j' | '\\E[B' | '\\EOB' | '\\3B' | '\\CB';",
	"home = '\\EOn' | '\\3n';",
	"fm = '^y';",
	"delete = '^d';",
	"eeof = '^e';",
	"einp = '^w';",
	"insrt = '^ ' | '\\E ';",
	"# pf keys\n ",
	"pfk1 = '\\EOq' | '\\E1' | '\\3q'; ",
	"pfk2 = '\\EOr' | '\\E2' | '\\3r'; ",
	"pfk3 = '\\EOs' | '\\E3' | '\\3s';",
	"pfk4 = '\\EOt' | '\\E4' | '\\3t'; ",
	"pfk5 = '\\EOu' | '\\E5' | '\\3u'; ",
	"pfk6 = '\\EOv' | '\\E6' | '\\3v';",
	"pfk7 = '\\EOw' | '\\E7' | '\\3w'; ",
	"pfk8 = '\\EOx' | '\\E8' | '\\3x'; ",
	"pfk9 = '\\EOy' | '\\E9' | '\\3y';",
	"pfk10 = '\\EOP\\EOp' | '\\E0'  | '\\3P\\3p';  ",
	"pfk11 = '\\EOP\\EOq' | '\\E-'  | '\\3P\\3q';",
	"pfk12 = '\\EOP\\EOr' | '\\E='  | '\\3P\\3r'; ",
	"pfk13 = '\\EOP\\EOs' | '^f13' | '\\3P\\3s';",
	"pfk14 = '\\EOP\\EOt' | '^f14' | '\\3P\\3t'; ",
	"pfk15 = '\\EOP\\EOu' | '^f15' | '\\3P\\3u';",
	"pfk16 = '\\EOP\\EOv' | '^f16' | '\\3P\\3v'; ",
	"pfk17 = '\\EOP\\EOw' | '^f17' | '\\3P\\3w';",
	"pfk18 = '\\EOP\\EOx' | '^f18' | '\\3P\\3x'; ",
	"pfk19 = '\\EOP\\EOy' | '^f19' | '\\3P\\3y';",
	"pfk20 = '\\EOQ\\EOp' | '^f20' | '\\3Q\\3p'; ",
	"pfk21 = '\\EOQ\\EOq' | '^f21' | '\\3Q\\3q';",
	"pfk22 = '\\EOQ\\EOr' | '^f22' | '\\3Q\\3r'; ",
	"pfk23 = '\\EOQ\\EOs' | '^f23' | '\\3Q\\3s';",
	"pfk24 = '\\EOQ\\EOt' | '^f24' | '\\3Q\\3t';",
	"# program attention keys\n",
	"pa1 = '\\E\\EOP' | '^p1' | '\\E\\3P';",
	"pa2 = '\\E\\EOQ' | '^p2' | '\\E\\3Q';",
	"# local control keys\n",
	"escape = '^c' | '^^';	# escape to telnet command mode\n",
	"master_reset = '^g';",
	"# local editing keys\n",
	"settab = '\\E;';",
	"deltab = '\\E\\'';",
	"clrtab = '\\E:';",
	"setmrg = '\\E,';",
	"sethom = '\\E.';",
	"coltab = '\\E\\E[B' | '\\E\\EOB' | '\\E\\3B' | '\\E\\CB';",
	"colbak = '\\E\\E[A' | '\\E\\EOA' | '\\E\\3A' | '\\E\\CA';",
	"indent = '\\E\\E[C' | '\\E\\EOC' | '\\E\\3C' | '\\E\\CC';",
	"undent = '\\E\\E[D' | '\\E\\EOD' | '\\E\\3D' | '\\E\\CD';",
	"ebcdic_40 = ' ';",
	"ebcdic_4A = '~';",
	" ebcdic_4B = '.'; ebcdic_4C = '<';",
        "ebcdic_4D = '('; ebcdic_4E = '+'; ebcdic_4F = '|';",
	"ebcdic_50 = '&';",
	"ebcdic_5A = '!'; ebcdic_5B = '$'; ebcdic_5C = '*';",
        "ebcdic_5D = ')'; ebcdic_5E = ';'; ebcdic_5F = '\\^';",
	"ebcdic_60 = '-'; ebcdic_61 = '/';",
	"ebcdic_6A = '['; ebcdic_6B = ','; ebcdic_6C = '%';",
        "ebcdic_6D = '_'; ebcdic_6E = '>'; ebcdic_6F = '?';",
	"ebcdic_7A = ':'; ebcdic_7B = '#'; ebcdic_7C = '@';",
        "ebcdic_7D = '\\\''; ebcdic_7E = '='; ebcdic_7F = '\"';",
	"ebcdic_F0 = '0';",
	"ebcdic_F1 = '1';",
	"ebcdic_F2 = '2';",
	"ebcdic_F3 = '3';",
	"ebcdic_F4 = '4';",
	"ebcdic_F5 = '5';",
	"ebcdic_F6 = '6';",
	"ebcdic_F7 = '7';",
	"ebcdic_F8 = '8';",
	"ebcdic_F9 = '9';",
	"}    # end of vt100, etc.\n",
	0,			/* Terminate list of entries */
};
			;
#endif
char *keysgeneric[] = {
    /* This is the system default for the key sequence */
    "generic { clear = '^z'; flinp = '^x'; enter = '^m'; delete = '^d' | '^?';",
    "      synch = '^r'; reshow = '^v'; eeof = '^e'; tab = '^i';",
    "      btab = '^b'; nl = '^n'; left = '^h'; right = '^l';",
    "      up = '^k'; down = '^j'; einp = '^w'; reset = '^t';",
    "      xoff = '^s'; xon = '^q'; escape = '^c'; ferase = '^u';",
    "      insrt = '\\E ';",
    "      pa1 = '^p1'; pa2 = '^p2'; pa3 = '^p3';",
    "      pfk1 = '\\E1'; pfk2 = '\\E2'; pfk3 = '\\E3'; pfk4 = '\\E4';",
    "      pfk5 = '\\E5'; pfk6 = '\\E6'; pfk7 = '\\E7'; pfk8 = '\\E8';",
    "      pfk9 = '\\E9'; pfk10 = '\\E0'; pfk11 = '\\E-'; pfk12 = '\\E=';",
    "      pfk13 = '\\E!'; pfk14 = '\\E@'; pfk15 = '\\E#'; pfk16 = '\\E$';",
    "      pfk17 = '\\E%'; pfk18 = '\\E\\^'; pfk19 = '\\E&'; pfk20 = '\\E*';",
    "      pfk21 = '\\E('; pfk22 = '\\E)'; pfk23 = '\\E_'; pfk24 = '\\E+';",
    "}",

    0,			/* Terminate list of entries */
};


