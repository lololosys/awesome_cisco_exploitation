/* $Id: latas.h,v 3.2 1995/11/17 17:33:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/latas.h,v $
 *------------------------------------------------------------------
 * $Log: latas.h,v $
 * Revision 3.2  1995/11/17  17:33:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:25:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    latas		define lat advertised service structure

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/
#ifndef LATAS_H
#define LATAS_H		0		/* Indicate file has been read	*/

/*----------------------------------------------------------------------
    Define advertised service flags.
 -----------------------------------------------------------------------*/
#define AS_OFFERED	0x01		/* Service is multicast		*/
#define AS_DYNAMIC	0x02		/* Service has a dynamic rating	*/
#define AS_CLIENT	0x04		/* Dynamic rating from client	*/
#define AS_ENABLED	0x20		/* Service connections enabled	*/
#define AS_QUEUED	0x40		/* Service can be queued	*/

#define AS_RATING	255		/* Maximum rating allowed       */

/*----------------------------------------------------------------------
    Define the advertised service structure.
 -----------------------------------------------------------------------*/
    typedef struct as
    {
	struct as AS_VIEW(as_link[2]);	/* Link to next, last service	*/
	aid_type as_aid;		/* Service identification	*/
	byte	 as_flags;		/* Service flag mask		*/
	byte	 as_rating;		/* Static service rating	*/
	byte	 as_service[SV_STR];	/* Service name  string		*/
	byte	 as_ident[ID_STR];	/* Service ident string		*/
	byte	 as_password[SV_STR];	/* Password string		*/
#if LE_OBJECTS
	int	as_cur_objects;		/* Current number objects
					   associated with service	*/
	int	as_last_object;		/* Last object used		*/
	byte	as_ob_map[(AS_OBJECTS-1)/8+1];
					/* Defined object bit map	*/
#elif CME_SLAT
	int	as_cur_ports;		/* Current number ports
					   associated with service	*/
	int	as_last_port;		/* Last CME port used		*/
	byte	as_port_map[(CM_MAX_PORTS-1)/8+1];
					/* Defined port bit map		*/
#endif
	int	as_rotary;	/* Associated rotary group */
	char	*as_command;	/* Pointer to autocommand */
    } as_node, AS_VIEW(as_ptr);
#endif
