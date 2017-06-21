/* $Id: lexfast.h,v 3.2 1995/11/17 17:43:29 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lex/lexfast.h,v $
 *------------------------------------------------------------------
 * lexfast.h -- LEX fastswitching definitions
 *
 * Feb. 1994, Andy Harvey
 *
 * Copyright (c) 1994-1995 by Cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lexfast.h,v $
 * Revision 3.2  1995/11/17  17:43:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:41:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LEXFAST_H__
#define __LEXFAST_H__

#define ROUND(value)	(((value) + 1) & 0xFFFE)
#define WORDS(value)	((value) >> 1)

/*
 * For use in fastswitch routines with input lex output xxx.
 */
#define LEX_ARPA_ENCAPBYTES	(input->encsize + ETHER_ARPA_ENCAPBYTES)
#define LEX_ETALK_ENCAPBYTES	(input->encsize + ETALK_ENCAPBYTES)

#define LEX_ARPA_OFFSET	(LEX_RECEIVE_OFFSET + WORDS(ETHER_ARPA_ENCAPBYTES))
#define LEX_ISO1_OFFSET	(LEX_RECEIVE_OFFSET + WORDS(ETHER_SAP_ENCAPBYTES))
#define LEX_ISO2_OFFSET (LEX_RECEIVE_OFFSET + WORDS(ETHER_SNAP_ENCAPBYTES))

/*
 * IP specific defines.
 */
#define LEX_IP_OFFSET 		LEX_ARPA_OFFSET
#define LEX_IP_HEADER_LEN	20

/*
 * IPX specific defines.
 */
#define LEX_IPX_HEADER_LEN	30

/*
 * Offsets for use in lex input fastswitching routines and macros.
 */

#define LEXARPA_ARPA_OFFSET 	(LEX_ARPA_OFFSET - WORDS(ETHER_ARPA_ENCAPBYTES))
#define LEXARPA_ISO1_OFFSET 	(LEX_ARPA_OFFSET - WORDS(ROUND(ETHER_SAP_ENCAPBYTES)))
#define LEXARPA_ISO2_OFFSET 	(LEX_ARPA_OFFSET - WORDS(ETHER_SNAP_ENCAPBYTES))
#define LEXARPA_FDDI1_OFFSET 	(LEX_ARPA_OFFSET - WORDS(FDDI_SAP_ENCAPBYTES))
#define LEXARPA_FDDI2_OFFSET 	(LEX_ARPA_OFFSET - WORDS(ROUND(FDDI_SNAP_ENCAPBYTES)))
#define LEXARPA_TR1_OFFSET 	(LEX_ARPA_OFFSET - WORDS(ROUND(TRING_SAP_ENCAPBYTES)))
#define LEXARPA_TR2_OFFSET 	(LEX_ARPA_OFFSET - WORDS(TRING_SNAP_ENCAPBYTES))

#define LEXISO1_ARPA_OFFSET 	(LEX_ISO1_OFFSET - WORDS(ETHER_ARPA_ENCAPBYTES))
#define LEXISO1_ISO1_OFFSET 	(LEX_ISO1_OFFSET - WORDS(ETHER_SAP_ENCAPBYTES))
#define LEXISO1_ISO2_OFFSET 	(LEX_ISO1_OFFSET - WORDS(ETHER_SNAP_ENCAPBYTES))
#define LEXISO1_FDDI1_OFFSET 	(LEX_ISO1_OFFSET - WORDS(FDDI_SAP_ENCAPBYTES))
#define LEXISO1_FDDI2_OFFSET 	(LEX_ISO1_OFFSET - WORDS(FDDI_SNAP_ENCAPBYTES))
#define LEXISO1_TR1_OFFSET 	(LEX_ISO1_OFFSET - WORDS(TRING_SAP_ENCAPBYTES))
#define LEXISO1_TR2_OFFSET 	(LEX_ISO1_OFFSET - WORDS(TRING_SNAP_ENCAPBYTES))

#define LEXISO2_ARPA_OFFSET	(LEX_ISO2_OFFSET - WORDS(ETHER_ARPA_ENCAPBYTES))
#define LEXISO2_ISO1_OFFSET	(LEX_ISO2_OFFSET - WORDS(ROUND(ETHER_SAP_ENCAPBYTES)))
#define LEXISO2_ISO2_OFFSET 	(LEX_ISO2_OFFSET - WORDS(ETHER_SNAP_ENCAPBYTES))
#define LEXISO2_FDDI1_OFFSET 	(LEX_ISO2_OFFSET - WORDS(FDDI_SAP_ENCAPBYTES))
#define LEXISO2_FDDI2_OFFSET 	(LEX_ISO2_OFFSET - WORDS(ROUND(FDDI_SNAP_ENCAPBYTES)))
#define LEXISO2_TR1_OFFSET 	(LEX_ISO2_OFFSET - WORDS(ROUND(TRING_SAP_ENCAPBYTES)))
#define LEXISO2_TR2_OFFSET 	(LEX_ISO2_OFFSET - WORDS(TRING_SNAP_ENCAPBYTES))

/*
 * The constants below are used when fastswitching when the lex
 * interface is the output interface.
 */
#define ARPA_LEXARPA_OFFSET 	(ARPA_IPX_OFFSET - WORDS(path->length))
#define ARPA_LEXISO1_OFFSET 	(ARPA_IPX_OFFSET - WORDS(ROUND(path->length)))
#define ARPA_LEXISO2_OFFSET 	(ARPA_IPX_OFFSET - WORDS(path->length))

#define ISO1_LEXARPA_OFFSET 	(ISO1_IPX_OFFSET - WORDS(path->length))
#define ISO1_LEXISO1_OFFSET 	(ISO1_IPX_OFFSET - WORDS(path->length))
#define ISO1_LEXISO2_OFFSET 	(ISO1_IPX_OFFSET - WORDS(path->length))

#define ISO2_LEXARPA_OFFSET 	(ISO2_IPX_OFFSET - WORDS(path->length))
#define ISO2_LEXISO1_OFFSET 	(ISO2_IPX_OFFSET - WORDS(ROUND(path->length)))
#define ISO2_LEXISO2_OFFSET 	(ISO2_IPX_OFFSET - WORDS(path->length))

#define LEXARPA_LEXARPA_OFFSET 	(LEX_ARPA_OFFSET - WORDS(path->length))
#define LEXARPA_LEXISO1_OFFSET 	(LEX_ARPA_OFFSET - WORDS(ROUND(path->length)))
#define LEXARPA_LEXISO2_OFFSET 	(LEX_ARPA_OFFSET - WORDS(path->length))

#define LEXISO1_LEXARPA_OFFSET 	(LEX_ISO1_OFFSET - WORDS(path->length))
#define LEXISO1_LEXISO1_OFFSET 	(LEX_ISO1_OFFSET - WORDS(path->length))
#define LEXISO1_LEXISO2_OFFSET 	(LEX_ISO1_OFFSET - WORDS(path->length))

#define LEXISO2_LEXARPA_OFFSET 	(LEX_ISO2_OFFSET - WORDS(path->length))
#define LEXISO2_LEXISO1_OFFSET 	(LEX_ISO2_OFFSET - WORDS(ROUND(path->length)))
#define LEXISO2_LEXISO2_OFFSET 	(LEX_ISO2_OFFSET - WORDS(path->length))

#define FDDI1_LEXARPA_OFFSET	 (FDDI_ISO1_IPX_OFFSET - WORDS(path->length))
#define FDDI1_LEXISO1_OFFSET	 (FDDI_ISO1_IPX_OFFSET - WORDS(path->length))
#define FDDI1_LEXISO2_OFFSET	 (FDDI_ISO1_IPX_OFFSET - WORDS(path->length))

#define FDDI2_LEXARPA_OFFSET	 (FDDI_ISO2_IPX_OFFSET - WORDS(path->length))
#define FDDI2_LEXISO1_OFFSET	 (FDDI_ISO2_IPX_OFFSET - WORDS(ROUND(path->length)))
#define FDDI2_LEXISO2_OFFSET	 (FDDI_ISO2_IPX_OFFSET - WORDS(path->length))

#define TR1_LEXARPA_OFFSET	 (TR_ISO1_IPX_OFFSET - WORDS(path->length))
#define TR1_LEXISO1_OFFSET	 (TR_ISO1_IPX_OFFSET - WORDS(path->length))
#define TR1_LEXISO2_OFFSET	 (TR_ISO1_IPX_OFFSET - WORDS(path->length))

#define TR2_LEXARPA_OFFSET	 (TR_ISO2_IPX_OFFSET - WORDS(path->length))
#define TR2_LEXISO1_OFFSET	 (TR_ISO2_IPX_OFFSET - WORDS(ROUND(path->length)))
#define TR2_LEXISO2_OFFSET	 (TR_ISO2_IPX_OFFSET - WORDS(path->length))

#define HDLC_LEXARPA_OFFSET	 (HDLC_IPX_OFFSET - WORDS(path->length))
#define HDLC_LEXISO1_OFFSET	 (HDLC_IPX_OFFSET - WORDS(ROUND(path->length)))
#define HDLC_LEXISO2_OFFSET	 (HDLC_IPX_OFFSET - WORDS(path->length))

#define FR_IETF_LEXARPA_OFFSET	 (FR_IETF_IPX_OFFSET - WORDS(path->length))
#define FR_IETF_LEXISO1_OFFSET	 (FR_IETF_IPX_OFFSET - WORDS(ROUND(path->length)))
#define FR_IETF_LEXISO2_OFFSET	 (FR_IETF_IPX_OFFSET - WORDS(path->length))

#define AIP_LEXARPA_OFFSET	 ((lineoff) - WORDS(path->length))
#define AIP_LEXISO1_OFFSET	 ((lineoff) - WORDS(ROUND(path->length)))
#define AIP_LEXISO2_OFFSET	 ((lineoff) - WORDS(path->length))

#endif
