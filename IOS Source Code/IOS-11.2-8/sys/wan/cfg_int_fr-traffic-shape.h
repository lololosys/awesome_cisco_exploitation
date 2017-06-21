/* $Id: cfg_int_fr-traffic-shape.h,v 3.1.2.4 1996/06/08 00:01:17 fowler Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_fr-traffic-shape.h,v $
 *------------------------------------------------------------------
 * Frame Relay Traffic Shaping
 *
 * March 1996, Charles Poe
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_fr-traffic-shape.h,v $
 * Revision 3.1.2.4  1996/06/08  00:01:17  fowler
 * CSCdi59356:  All frame relay subinterface tests are failing - could be
 *              a broadcast
 *              Fix broadcast nvgen
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/05/29  06:34:56  fowler
 * CSCdi58766:  Link errors in 11.2 build
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/17  21:13:42  ccpoe
 * CSCdi57877:  Frame relay traffic shaping breaks modular images
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/15  05:54:34  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/26  05:47:50  ccpoe
 * placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * frame-relay class <class-name>
 * no frame-relay class <class-name>
 */
EOLS	(ci_FR_int_class_eol, fr_class_command, FR_INT_CLASS);

STRING	(ci_FR_int_class_name, ci_FR_int_class_eol, no_alt,
	 OBJ(string, 1), "map class name");

NVGENS	(ci_FR_int_class_nvgen, ci_FR_int_class_name,
	 fr_class_command, FR_INT_CLASS);

KEYWORD	(ci_FR_int_class, ci_FR_int_class_nvgen, ALTERNATE,
	    "class", "Define a map class on the interface",
	 PRIV_CONF|PRIV_SUBIF);
/***************************************************************
 * frame-relay broadcast-queue size rate num_packets
 * no frame-relay broadcast-queue 
*/
EOLS	(ci_FR_bq_eol, fr_traffic_command, FR_BQ);

/*
 * function that co-works w/ NUMBER_FUNC macro to define the current
 * range for max. packets/S broadcasts transmission.
 */
static void ci_FR_bq_pkts_range (parseinfo *csb, uint *lower, uint *upper)
{
    if (csb && csb->interface && csb->interface->hwptr &&
	(csb->interface->hwptr->status & IDB_HSSI)) {
	*lower = 1;
	*upper = 10000;
    } else {
	*lower = 1;
	*upper = 999;
    }
}

NUMBER_FUNC (ci_FR_bq_pkts, ci_FR_bq_eol, no_alt, OBJ(int,3), 
        ci_FR_bq_pkts_range,
        "Max. packets/S broadcasts transmission");

/*
 * function that co-works w/ NUMBER_FUNC macro to define the current
 * range for byte rate per sec. for broadcasts transmission.
 */
static void ci_FR_bq_rate_range (parseinfo *csb, uint *lower, uint *upper)
{
    if (csb && csb->interface && csb->interface->hwptr &&
	(csb->interface->hwptr->status & IDB_HSSI)) {
	*lower = 1000;
	*upper = 7000000;
    } else {
	*lower = 1000;
	*upper = 1000000;
    }
}

NUMBER_FUNC  (ci_FR_bq_rate, ci_FR_bq_pkts, no_alt, OBJ(int,2), 
         ci_FR_bq_rate_range, 
	 "Byte rate per sec. for broadcasts transmission");

NUMBER	(ci_FR_bq_size, ci_FR_bq_rate, no_alt,
	 OBJ(int,1), 1, 65535, "Queue size for broadcasts");

NOPREFIX (ci_FR_bq_noprefix, ci_FR_bq_size, ci_FR_bq_eol);

NVGENS	(ci_FR_bq_nvgen, ci_FR_bq_noprefix,
	 fr_traffic_command, FR_BQ);

KEYWORD	(ci_FR_bq, ci_FR_bq_nvgen, ci_FR_int_class,
	    "broadcast-queue", "Define a broadcast queue and transmit rate",
	 PRIV_CONF);



/***************************************************************
 * frame-relay traffic-shaping
 * no frame-relay traffic-shaping
 */
EOLS	(ci_FR_traffic_eol, fr_traffic_command, FR_TRAFFIC_SWITCH);
    
NVGENS	(ci_FR_traffic_nvgen, ci_FR_traffic_eol,
	 fr_traffic_command, FR_TRAFFIC_SWITCH);

KEYWORD	(ci_FR_traffic, ci_FR_traffic_nvgen, ci_FR_bq,
	 "traffic-shaping", "Enable Frame Relay Traffic Shaping",
	 PRIV_CONF);

ASSERT(ci_FR_traffic_mgt, ci_FR_traffic, ALTERNATE,
       is_frame_relay(csb->interface->hwptr));

#undef ALTERNATE
#define ALTERNATE	ci_FR_traffic_mgt
