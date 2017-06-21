
xrlatfont.h,1849
#define __XRLATFONT_H__26,851
} lat_font_list_req;lat_font_list_req38,1037
} lat_font_get_prop_req;lat_font_get_prop_req46,1176
} lat_font_get_glyph_req;lat_font_get_glyph_req57,1381
} lat_font_list_rep;lat_font_list_rep65,1523
} lat_font_attr_rep;lat_font_attr_rep71,1620
} lat_font_property_rep;lat_font_property_rep81,1793
} lat_font_glyph_rep;lat_font_glyph_rep88,1914
enum LAT_FONT_PROTOCOL LAT_FONT_PROTOCOL90,1937
    LAT_FONT_VERSION 91,1962
    LAT_FONT_VERSION = 0x01,91,1962
    LAT_FONT_READY 92,1991
    LAT_FONT_READY = 0x02,92,1991
    LAT_FONT_LIST_OPCODE 93,2018
    LAT_FONT_LIST_OPCODE = 0x03,93,2018
    LAT_FONT_GET_PROPERTIES 94,2051
    LAT_FONT_GET_PROPERTIES = 0x04,94,2051
    LAT_FONT_GET_GLYPHS 95,2087
    LAT_FONT_GET_GLYPHS = 0x05,95,2087
    LAT_FONT_DIRECTORY 96,2119
    LAT_FONT_DIRECTORY = 0x0a,96,2119
    LAT_FONT_ATTRIBUTES 97,2150
    LAT_FONT_ATTRIBUTES = 0x0b,97,2150
    LAT_FONT_PROPERTIES 98,2182
    LAT_FONT_PROPERTIES = 0x0c,98,2182
    LAT_FONT_GLYPHS 99,2214
    LAT_FONT_GLYPHS = 0x0d99,2214
enum font_state font_state111,2338
enum font_state { FONT_NOTOPEN 111,2338
enum font_state { FONT_NOTOPEN = 0, FONT_OPEN_WAIT,111,2338
enum font_state { FONT_NOTOPEN = 0, FONT_OPEN_WAIT, FONT_VERSION_WAIT,111,2338
		    FONT_READY_WAIT,112,2409
		    FONT_READY_WAIT, FONT_READY,112,2409
		    FONT_READY_WAIT, FONT_READY, FONT_LIST,112,2409
		    FONT_GET_ATTRIBUTES,113,2455
		    FONT_GET_ATTRIBUTES, FONT_GET_PROPERTIES,113,2455
		    FONT_GET_ATTRIBUTES, FONT_GET_PROPERTIES, FONT_GET_GLYPHS,113,2455
		    FONT_DISCONNECT 114,2520
typedef struct lat_font_context_ lat_font_context_116,2546
} lat_font_context;lat_font_context143,3936
} lat_font_dir_hdr;lat_font_dir_hdr152,4111
#define XRLAT_OUTBUFSIZE 158,4153
#define XRLAT_INBUFSIZE 160,4243
