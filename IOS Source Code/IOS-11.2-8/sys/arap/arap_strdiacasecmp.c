/* $Id: arap_strdiacasecmp.c,v 3.2.60.1 1996/08/07 08:56:30 snyder Exp $
 * $Source: /release/112/cvs/Xsys/arap/arap_strdiacasecmp.c,v $
 *------------------------------------------------------------------
 * strdiacasecmp.c - Diacritically insensitive string comparison
 *                   routines
 *
 * October 1993, Tim Kolar
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: arap_strdiacasecmp.c,v $
 * Revision 3.2.60.1  1996/08/07  08:56:30  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:38:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:05:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "arap_strdiacasecmp.h"

static const unsigned char cmap[] = {
    /* map	value	name */
    0 /* 0	NUL */ ,
    1 /* 1	SOH */ ,
    2 /* 2	STX */ ,
    3 /* 3	ETX */ ,
    4 /* 4	EOT */ ,
    5 /* 5	ENQ */ ,
    6 /* 6	ACK */ ,
    7 /* 7	BEL */ ,
    8 /* 8	BS */ ,
    9 /* 9	HT */ ,
    10 /* 10	NL */ ,
    11 /* 11	VT */ ,
    12 /* 12	NP */ ,
    13 /* 13	CR */ ,
    14 /* 14	SO */ ,
    15 /* 15	SI */ ,
    16 /* 16	DLE */ ,
    17 /* 17	DC1 */ ,
    18 /* 18	DC2 */ ,
    19 /* 19	DC3 */ ,
    20 /* 20	DC4 */ ,
    21 /* 21	NAK */ ,
    22 /* 22	SYN */ ,
    23 /* 23	ETB */ ,
    24 /* 24	CAN */ ,
    25 /* 25	EM */ ,
    26 /* 26	SUB */ ,
    27 /* 27	ESC */ ,
    28 /* 28	FS */ ,
    29 /* 29	GS */ ,
    30 /* 30	RS */ ,
    31 /* 31	US */ ,
    32 /* 32	SP */ ,
    33 /* 33	 ! */ ,
    34 /* 34	 " */ ,
    35 /* 35	 # */ ,
    36 /* 36	 $ */ ,
    37 /* 37	 % */ ,
    38 /* 38	 & */ ,
    39 /* 39	 ' */ ,
    40 /* 40	 ( */ ,
    41 /* 41	 ) */ ,
    42 /* 42	 * */ ,
    43 /* 43	 + */ ,
    44 /* 44	 , */ ,
    45 /* 45	 - */ ,
    46 /* 46	 . */ ,
    47 /* 47	 / */ ,
    48 /* 48	 0 */ ,
    49 /* 49	 1 */ ,
    50 /* 50	 2 */ ,
    51 /* 51	 3 */ ,
    52 /* 52	 4 */ ,
    53 /* 53	 5 */ ,
    54 /* 54	 6 */ ,
    55 /* 55	 7 */ ,
    56 /* 56	 8 */ ,
    57 /* 57	 9 */ ,
    58 /* 58	 : */ ,
    59 /* 59	 ; */ ,
    60 /* 60	 < */ ,
    61 /* 61	 = */ ,
    62 /* 62	 > */ ,
    63 /* 63	 ? */ ,
    64 /* 64	 @ */ ,
    65 /* 65	 A */ ,
    66 /* 66	 B */ ,
    67 /* 67	 C */ ,
    68 /* 68	 D */ ,
    69 /* 69	 E */ ,
    70 /* 70	 F */ ,
    71 /* 71	 G */ ,
    72 /* 72	 H */ ,
    73 /* 73	 I */ ,
    74 /* 74	 J */ ,
    75 /* 75	 K */ ,
    76 /* 76	 L */ ,
    77 /* 77	 M */ ,
    78 /* 78	 N */ ,
    79 /* 79	 O */ ,
    80 /* 80	 P */ ,
    81 /* 81	 Q */ ,
    82 /* 82	 R */ ,
    83 /* 83	 S */ ,
    84 /* 84	 T */ ,
    85 /* 85	 U */ ,
    86 /* 86	 V */ ,
    87 /* 87	 W */ ,
    88 /* 88	 X */ ,
    89 /* 89	 Y */ ,
    90 /* 90	 Z */ ,
    91 /* 91	 [ */ ,
    92 /* 92	 \ */ ,
    93 /* 93	 ] */ ,
    94 /* 94	 ^ */ ,
    95 /* 95	 _ */ ,
    96 /* 96	 ` */ ,
    65 /* 97	 a */ ,
    66 /* 98	 b */ ,
    67 /* 99	 c */ ,
    68 /* 100	 d */ ,
    69 /* 101	 e */ ,
    70 /* 102	 f */ ,
    71 /* 103	 g */ ,
    72 /* 104	 h */ ,
    73 /* 105	 i */ ,
    74 /* 106	 j */ ,
    75 /* 107	 k */ ,
    76 /* 108	 l */ ,
    77 /* 109	 m */ ,
    78 /* 110	 n */ ,
    79 /* 111	 o */ ,
    80 /* 112	 p */ ,
    81 /* 113	 q */ ,
    82 /* 114	 r */ ,
    83 /* 115	 s */ ,
    84 /* 116	 t */ ,
    85 /* 117	 u */ ,
    86 /* 118	 v */ ,
    87 /* 119	 w */ ,
    88 /* 120	 x */ ,
    89 /* 121	 y */ ,
    90 /* 122	 z */ ,
    123 /* 123	 { */ ,
    124 /* 124	 | */ ,
    125 /* 125	 } */ ,
    126 /* 126	 ~ */ ,
    127 /* 127	DEL */ ,
    138 /* 128	Adieresis */ ,
    140 /* 129	Aring */ ,
    141 /* 130	Ccedilla */ ,
    142 /* 131	Eacute */ ,
    150 /* 132	Ntilda */ ,
    159 /* 133	Odieresis */ ,
    134 /* 134	Udieresis */ ,
    135 /* 135	aacute */ ,
    136 /* 136	agrave */ ,
    137 /* 137	acircumflex */ ,
    138 /* 138	adieresis */ ,
    139 /* 139	atilda */ ,
    140 /* 140	aring */ ,
    141 /* 141	ccedilla */ ,
    142 /* 142	eacute */ ,
    143 /* 143	egrave */ ,
    144 /* 144	ecircumflex */ ,
    145 /* 145	edieresis */ ,
    146 /* 146	iacute */ ,
    147 /* 147	igrave */ ,
    148 /* 148	icircumflex */ ,
    149 /* 149	idieresis */ ,
    150 /* 150	ntilda */ ,
    151 /* 151	oacute */ ,
    152 /* 152	ograve */ ,
    153 /* 153	ocircumflex */ ,
    154 /* 154	odieresis */ ,
    155 /* 155	otilda */ ,
    156 /* 156	uacute */ ,
    157 /* 157	ugrave */ ,
    158 /* 158	ucircumflex */ ,
    159 /* 159	udieresis */ ,
    160 /* 160	daggar */ ,
    161 /* 161	ring */ ,
    162 /* 162	cent */ ,
    163 /* 163	sterling */ ,
    164 /* 164	section */ ,
    165 /* 165	bullet */ ,
    166 /* 166	paragraph */ ,
    167 /* 167	germandbls */ ,
    168 /* 168	registered */ ,
    169 /* 169	copyright */ ,
    170 /* 170	trademark */ ,
    171 /* 171	acute */ ,
    172 /* 172	dieresis */ ,
    173 /* 173	notequal */ ,
    190 /* 174	AE */ ,
    191 /* 175	Oslash */ ,
    176 /* 176	infinity */ ,
    177 /* 177	plusminus */ ,
    178 /* 178	lessequal */ ,
    179 /* 179	greaterequal */ ,
    180 /* 180	yen */ ,
    181 /* 181	mu */ ,
    182 /* 182	delta */ ,
    183 /* 183	Sigma */ ,
    185 /* 184	Pi */ ,
    185 /* 185	pi */ ,
    186 /* 186	intergral */ ,
    187 /* 187	ordfeminine */ ,
    188 /* 188	ordmasculine */ ,
    189 /* 189	Omega */ ,
    190 /* 190	ae */ ,
    191 /* 191	oslash */ ,
    192 /* 192	questiondown */ ,
    193 /* 193	exclamdown */ ,
    194 /* 194	not */ ,
    195 /* 195	radical */ ,
    196 /* 196	florin */ ,
    197 /* 197	aprox */ ,
    182 /* 198	Delta */ ,
    199 /* 199	guillemotleft */ ,
    200 /* 200	guillemotright */ ,
    201 /* 201	ellipsis */ ,
    202 /* 202	 */ ,
    136 /* 203	Agrave */ ,
    139 /* 204	Atilda */ ,
    155 /* 205	Otilda */ ,
    207 /* 206	OE */ ,
    207 /* 207	oe */ ,
    208 /* 208	endash */ ,
    209 /* 209	emdash */ ,
    210 /* 210	quotedblleft */ ,
    211 /* 211	quotedblright */ ,
    212 /* 212	quoteleft */ ,
    213 /* 213	quoteright */ ,
    214 /* 214	divide */ ,
    215 /* 215	diamond */ ,
    216 /* 216	ydieresis */ ,
    216 /* 217	Ydieresis */ ,
    218 /* 218	fraction */ ,
    219 /* 219	currency */ ,
    220 /* 220	guilsinglleft */ ,
    221 /* 221	guilsinglright */ ,
    222 /* 222	fi */ ,
    223 /* 223	fl */ ,
    224 /* 224	daggardbl */ ,
    225 /* 225	periodcentered */ ,
    226 /* 226	quotesinglbase */ ,
    227 /* 227	quotedblbase */ ,
    228 /* 228	perthousand */ ,
    137 /* 229	Acircumflex */ ,
    144 /* 230	Ecircumflex */ ,
    135 /* 231	Aaccute */ ,
    145 /* 232	Edieresis */ ,
    143 /* 233	Egrave */ ,
    146 /* 234	Iaccute */ ,
    148 /* 235	Icircumflex */ ,
    149 /* 236	Idieresis */ ,
    147 /* 237	Igrave */ ,
    151 /* 238	Oaccute */ ,
    153 /* 239	Ocircumflex */ ,
    240 /* 240	apple */ ,
    152 /* 241	Ograve */ ,
    156 /* 242	Uaccute */ ,
    158 /* 243	Ucircumflex */ ,
    157 /* 244	Ugrave */ ,
    245 /* 245	dotlessi */ ,
    246 /* 246	circumflex */ ,
    247 /* 247	tilda */ ,
    248 /* 248	macron */ ,
    249 /* 249	breve */ ,
    250 /* 250	dotaccent */ ,
    251 /* 251	ring */ ,
    252 /* 252	cedilla */ ,
    253 /* 253	hungarumlaut */ ,
    254 /* 254	ogonek */ ,
    255 /* 255	caron */ ,
};

/*
 * strdiacasecmp()
 * 
 * perform "diacritically in-sensative" (i.e. case + foreign char insensative)
 * comparison.  This is an analog to strcmp();
 */

int strdiacasecmp (unsigned char *s1, unsigned char *s2)
{
    while (cmap[*s1] == cmap[*s2++]) {
	if (*s1++ == '\0') {
	    return (0);
	}
    }
    return (cmap[*s1] - cmap[*--s2]);
}

/*
 * strndiacasecmp()
 * 
 * perform "diacritically in-sensative" (i.e. case + foreign char insensative)
 * comparison with a max length.  This is an analog to strncmp();
 */

int strndiacasecmp (unsigned char *s1, unsigned char *s2, int n)
{
    while (--n >= 0 && cmap[*s1] == cmap[*s2++]) {
	if (*s1++ == '\0') {
	    return (0);
	}
    }
    return (n < 0 ? 0 : cmap[*s1] - cmap[*--s2]);
}
