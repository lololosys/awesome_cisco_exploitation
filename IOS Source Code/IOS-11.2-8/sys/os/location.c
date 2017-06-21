/* $Id: location.c,v 3.1.2.1 1996/04/16 19:09:58 rchandra Exp $
 * $Source: /release/112/cvs/Xsys/os/location.c,v $
 *------------------------------------------------------------------
 * System location support
 *
 * December 1995, Dave Katz
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: location.c,v $
 * Revision 3.1.2.1  1996/04/16  19:09:58  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  09:39:31  dkatz
 * Add placeholder.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "location.h"

/*
 * System location
 *
 * Longitude and latitude are stored as centiseconds of arc.  South and West
 * are negative.
 *
 * Altitude is stored in feet.  Negative altitudes are rare but possible.
 */

static int system_longitude;		/* Longitude in centiseconds */
static int system_latitude;		/* Latitude in centiseconds */
static int system_altitude;		/* Altitude in feet */


/*
 * location_set
 *
 * Set the location of the system
 *
 * Longitude and latitude are in centiseconds of arc;  altitude is in feet
 */
void location_set (int longitude, int latitude, int altitude)
{
    system_longitude = longitude;
    system_latitude = latitude;
    system_altitude = altitude;
}

/*
 * location_get
 *
 * Get the location of the system
 */
void location_get (int *longitude, int *latitude, int *altitude)
{
    *longitude = system_longitude;
    *latitude = system_latitude;
    *altitude = system_altitude;
}

/*
 * location_centisecs_to_dmsf
 *
 * Convert centiseconds to degrees, minutes, seconds, and hundredths of
 * seconds.
 *
 * The degrees maintain the sign of the direction (South and West are
 * negative);  all other quantities are positive.
 */
void location_centisecs_to_dmsf (int location, int *degrees, ulong *minutes,
				 ulong *seconds, ulong *hundredths)
{
    ulong local_loc;

    /* Undo the sign first. */

    if (location < 0) {
	local_loc = -location;
    } else {
	local_loc = location;
    }

    *hundredths = local_loc % CENTISECS_PER_SECOND;
    local_loc /= CENTISECS_PER_SECOND;
    *seconds = local_loc % ARC_SECONDS_PER_MINUTE;
    local_loc /= ARC_SECONDS_PER_MINUTE;
    *minutes = local_loc % MINUTES_PER_DEGREE;
    *degrees = local_loc / MINUTES_PER_DEGREE;
    if (location < 0)
	*degrees = -(*degrees);
}

/*
 * show_location
 *
 * Display the system location
 */
void show_location (parseinfo *csb)
{
    int degrees;
    ulong minutes, seconds, hundredths;
    int latitude, longitude, altitude;
    char direction;

    /* Get the location. */

    location_get(&longitude, &latitude, &altitude);

    /* If not set, return. */

    if (!longitude && !latitude && !altitude) /* Not in the Atlantic! */
	return;

    location_centisecs_to_dmsf(latitude, &degrees, &minutes, &seconds,
			       &hundredths);
    direction = 'N';
    if (degrees < 0) {
	degrees = -degrees;
	direction = 'S';
    }
    printf("\nLatitude %d%c%02d'%02d.%02d\", ", degrees, direction, minutes,
	   seconds, hundredths);

    location_centisecs_to_dmsf(longitude, &degrees, &minutes, &seconds,
			       &hundredths);
    direction = 'E';
    if (degrees < 0) {
	degrees = -degrees;
	direction = 'W';
    }
    printf("Longitude %d%c%02d'%02d.%02d\", ", degrees, direction, minutes,
	   seconds, hundredths);

    printf("Altitude %d feet", altitude);
}

