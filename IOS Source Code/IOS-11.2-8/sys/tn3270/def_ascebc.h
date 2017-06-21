/* $Id: def_ascebc.h,v 3.2 1995/11/17 18:50:54 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tn3270/def_ascebc.h,v $
 *------------------------------------------------------------------
 * D E F _ A S C E B C . H
 *
 * July, 1994, Bill Palter
 * 
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: def_ascebc.h,v $
 * Revision 3.2  1995/11/17  18:50:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:07:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

struct default_key_type {
	char *name;
	char input;
} static const default_keys[] = {
	{"ebcdic_40"	,	' '	},
	{"ebcdic_4A"	,	'¢'	},
	{"ebcdic_4B"	,	'.'	},
	{"ebcdic_4C"	,	'<'	},
	{"ebcdic_4D"	,	'('	},
	{"ebcdic_4E"	,	'+'	},
	{"ebcdic_4F"	,	'|'	},
	{"ebcdic_50"	,	'&'	},
	{"ebcdic_5A"	,	'!'	},
	{"ebcdic_5B"	,	'$'	},
	{"ebcdic_5C"	,	'*'	},
	{"ebcdic_5D"	,	')'	},
	{"ebcdic_5E"	,	';'	},
	{"ebcdic_5F"	,	'^'	},
	{"ebcdic_60"	,	'-'	},
	{"ebcdic_61"	,	'/'	},
	{"ebcdic_6B"	,	','	},
	{"ebcdic_6C"	,	'%'	},
	{"ebcdic_6D"	,	'_'	},
	{"ebcdic_6E"	,	'>'	},
	{"ebcdic_6F"	,	'?'	},
	{"ebcdic_79"    ,       '`'     },
	{"ebcdic_7A"	,	':'	},
	{"ebcdic_7B"	,	'#'	},
	{"ebcdic_7C"	,	'@'	},
	{"ebcdic_7D"	,	'\''	},
	{"ebcdic_7E"	,	'='	},
	{"ebcdic_7F"	,	'"'	},
	{"ebcdic_81"	,	'a'	},
	{"ebcdic_82"	,	'b'	},
	{"ebcdic_83"	,	'c'	},
	{"ebcdic_84"	,	'd'	},
	{"ebcdic_85"	,	'e'	},
	{"ebcdic_86"	,	'f'	},
	{"ebcdic_87"	,	'g'	},
	{"ebcdic_88"	,	'h'	},
	{"ebcdic_89"	,	'i'	},
	{"ebcdic_91"	,	'j'	},
	{"ebcdic_92"	,	'k'	},
	{"ebcdic_93"	,	'l'	},
	{"ebcdic_94"	,	'm'	},
	{"ebcdic_95"	,	'n'	},
	{"ebcdic_96"	,	'o'	},
	{"ebcdic_97"	,	'p'	},
	{"ebcdic_98"	,	'q'	},
	{"ebcdic_99"	,	'r'	},
	{"ebcdic_a2"	,	's'	},
	{"ebcdic_a3"	,	't'	},
	{"ebcdic_a4"	,	'u'	},
	{"ebcdic_a5"	,	'v'	},
	{"ebcdic_a6"	,	'w'	},
	{"ebcdic_a7"	,	'x'	},
	{"ebcdic_a8"	,	'y'	},
	{"ebcdic_a9"	,	'z'	},
	{"ebcdic_c1"	,	'A'	},
	{"ebcdic_c2"	,	'B'	},
	{"ebcdic_c3"	,	'C'	},
	{"ebcdic_c4"	,	'D'	},
	{"ebcdic_c5"	,	'E'	},
	{"ebcdic_c6"	,	'F'	},
	{"ebcdic_c7"	,	'G'	},
	{"ebcdic_c8"	,	'H'	},
	{"ebcdic_c9"	,	'I'	},
	{"ebcdic_d1"	,	'J'	},
	{"ebcdic_d2"	,	'K'	},
	{"ebcdic_d3"	,	'L'	},
	{"ebcdic_d4"	,	'M'	},
	{"ebcdic_d5"	,	'N'	},
	{"ebcdic_d6"	,	'O'	},
	{"ebcdic_d7"	,	'P'	},
	{"ebcdic_d8"	,	'Q'	},
	{"ebcdic_d9"	,	'R'	},
	{"ebcdic_e2"	,	'S'	},
	{"ebcdic_e3"	,	'T'	},
	{"ebcdic_e4"	,	'U'	},
	{"ebcdic_e5"	,	'V'	},
	{"ebcdic_e6"	,	'W'	},
	{"ebcdic_e7"	,	'X'	},
	{"ebcdic_e8"	,	'Y'	},
	{"ebcdic_e9"	,	'Z'	},
	{"ebcdic_F0"	,	'0'	},
	{"ebcdic_F1"	,	'1'	},
	{"ebcdic_F2"	,	'2'	},
	{"ebcdic_F3"	,	'3'	},
	{"ebcdic_F4"	,	'4'	},
	{"ebcdic_F5"	,	'5'	},
	{"ebcdic_F6"	,	'6'	},
	{"ebcdic_F7"	,	'7'	},
	{"ebcdic_F8"	,	'8'	},
	{"ebcdic_F9"	,	'9'	},
	{"ebcdic_AD"	,	'['	},
	{"ebcdic_BD"	,	']'	},
	{"ebcdic_C0"	,	'{'	},
	{"ebcdic_D0"	,	'}'	},
	{"ebcdic_E0"	,	'\\'	},
	{"ebcdic_A1"    ,       '~'     },
	{0,0}};
