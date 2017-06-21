/***************************************************************
 * [no] ignore-hw [local-loopback ]
 *
 */
EOLS    (ci_ignore_hw_eol, serial_ignore_hw_command, 0);

KEYWORD_ID (ci_ignore_hw_local_loopback, ci_ignore_hw_eol, NONE,
	    OBJ(int,1), IGNORE_LOCAL_LOOPBACK,
	    "local-loopback", "DCE ignores the local loopback serial signal",
	    PRIV_CONF);

ASSERT (ci_ignore_hw_local_loopback_test, ci_ignore_hw_local_loopback, NONE,
        (csb->interface->hwptr->serial_flags & SERIAL_FLAGS_CAN_IGNORE_LOCAL_LOOPBACK));

KEYWORD (ci_ignore_hw, ci_ignore_hw_local_loopback_test, NONE,
         "ignore-hw", "ignore a serial signal", PRIV_CONF);

ASSERT (ci_ignore_hw_test, ci_ignore_hw, ALTERNATE,
        (csb->interface->hwptr->status & IDB_SERIAL) &&
        (csb->interface->hwptr->serial_flags & (SERIAL_FLAGS_CAN_IGNORE_LOCAL_LOOPBACK)));

#undef  ALTERNATE
#define ALTERNATE       ci_ignore_hw_test
