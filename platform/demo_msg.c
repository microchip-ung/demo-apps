/*

  Vitesse Switch API software.

 Copyright (c) 2002-2013 Vitesse Semiconductor Corporation "Vitesse". All
 Rights Reserved.

 Unpublished rights reserved under the copyright laws of the United States of
 America, other countries and international treaties. Permission to use, copy,
 store and modify, the software and its source code is granted. Permission to
 integrate into other products, disclose, transmit and distribute the software
 in an absolute machine readable format (e.g. HEX file) is also granted.  The
 source code of the software may not be disclosed, transmitted or distributed
 without the written permission of Vitesse. The software and its source code
 may only be used in products utilizing the Vitesse switch products.

 This copyright notice must appear in any copy, modification, disclosure,
 transmission or distribution of the software. Vitesse retains all ownership,
 copyright, trade secret and proprietary rights in the software.

 THIS SOFTWARE HAS BEEN PROVIDED "AS IS," WITHOUT EXPRESS OR IMPLIED WARRANTY
 INCLUDING, WITHOUT LIMITATION, IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR USE AND NON-INFRINGEMENT.

*/

#include <msg_api.h>
#include <port_api.h>
#include "demo_api.h"

#include "demo.h"
#include "demo_trace.h"


#define DEMO_MESSAGE_SOMETHING 0
#define DEMO_MESSAGE_ELSE      1


struct demo_msg_t {
    u32 msg_id;
};


// --- A function that can be used for sending messages to
//     the demo module and all, any or local switch depending
//     on isid, see code.
void demo_msg_tx(vtss_isid_t isid)
{
    switch_iter_t sit;

    if ( !(isid < VTSS_ISID_END)) {
        T_WG(TRACE_GRP_MSG, "Invalid ISID %d. Must be less that %d\n", isid, VTSS_ISID_END);
        return;
    }

    
    // --- Send the messages as specified bu isid to other switched.
    //     If isid = VTSS_ISID_GLOBAL = 17, then message is sent to all switched.
    //     If isid = VTSS_ISID_LOCAL = 0, then the message is sent to this switch only.
    //     If isid = 1,...,16, then message is sent to the switch with that number.

    if (isid == VTSS_ISID_LOCAL) {

        // (1) --- Allocated message buffer. Note, that when it is
        //         sent by msg_tx() below, we do not own it any more,
        //         and therefore not ours to release again.
      struct demo_msg_t *msg = (struct demo_msg_t*)VTSS_MALLOC(sizeof(struct demo_msg_t));
        if (msg == NULL) {
            T_EG(TRACE_GRP_MSG, "Allocation failed.\n");
            return;
        }

        // (2) --- Put something useful in the message buffer
        msg->msg_id = DEMO_MESSAGE_SOMETHING;

        // (3) --- And send the message
        msg_tx(VTSS_MODULE_ID_DEMO, isid, msg, sizeof(*msg));

        T_DG(TRACE_GRP_MSG, "Has sent message to isid=%d", isid);

    } else {

        (void)switch_iter_init(&sit, isid, SWITCH_ITER_SORT_ORDER_ISID);
	while (switch_iter_getnext(&sit)) {

	    // (1) --- Allocated message buffer. Note, that when it is
	    //         sent by msg_tx() below, we do not own it any more,
	    //         and therefore not ours to release again.
	    struct demo_msg_t *msg = (struct demo_msg_t*)VTSS_MALLOC(sizeof(struct demo_msg_t));
	    if (msg == NULL) {
	        T_EG(TRACE_GRP_MSG, "Allocation failed.\n");
	        return;
	    }

	    // (2) --- Put something useful in the message buffer
	    msg->msg_id = DEMO_MESSAGE_SOMETHING;

	    // (3) --- And send the message
	    msg_tx(VTSS_MODULE_ID_DEMO, sit.isid, msg, sizeof(*msg));

	    T_DG(TRACE_GRP_MSG, "Has sent message to isid=%d", sit.isid);
	}
    }
}


// --- The message handler
//
static BOOL demo_msg_rx(void  *contxt,
                        const void *const rx_msg,
                        const size_t len,
                        const vtss_module_id_t modid,
                        u32 isid)
{
    const struct demo_msg_t *const msg = ( const struct demo_msg_t *const )rx_msg;

    T_DG(TRACE_GRP_MSG, "Has received mdg_id=%d isid=%d modid=%d\n", msg->msg_id, isid, modid);


    DEMO_PLATFORM_CRIT_ENTER();
    // something that e.g. is shared by this thread and demo_thread

    DEMO_PLATFORM_CRIT_EXIT();

    return TRUE;
}



// --- This function register the receiver function demo_msg_rx()
//     with the msg module.

//
void demo_register_msg(void)
{
    msg_rx_filter_t filter;
    vtss_rc         rc;

    bzero(&filter, sizeof(filter));

    // --- When a message desinated for the demo
    //     module (specified by filter.modid), then
    //     call handler demo_msg_rx().
    filter.cb = demo_msg_rx;
    filter.modid = VTSS_MODULE_ID_DEMO;

    rc = msg_rx_filter_register(&filter);

    if (rc != VTSS_RC_OK) {
        // Failed. Do something.
        T_EG(TRACE_GRP_MSG, "Demo panic");
    }
}
