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

#include <l2proto_api.h>

#include "demo.h"
#include "demo_trace.h"



// --- This function is called when a packet is received on this switch
//     which the attributes as given in the demo_packet_register() function.
//
static void demo_l2_rx(const void *packet, size_t len, mesa_vid_t vid, l2_port_no_t l2port)
{
    u32    *buf;
    size_t total_len = len + (3 * sizeof(u32)); // We reserve two extra u32 for total length and l2 port
    size_t delta;

    T_DG(TRACE_GRP_L2PROTO, "len=%d, sid=%d, l2port=%d", len, vid, l2port);

    DEMO_PLATFORM_CRIT_ENTER();

    // --- Ensure, that buffers, i.e., 'buf' will always be 4 byte alligned.
    if ((delta = total_len % 4)) {
        total_len += 4 - delta;
    }

    if ((buf = (u32 *)vtss_bip_buffer_reserve(&demo_rx_buffer, total_len)) != NULL) {
        buf[0] = total_len;                      // Save the total length first (in bytes)
        buf[1] = l2port;                         // Then the l2 port it came from
        buf[2] = len;
        memcpy(&buf[3], packet, len);            // Then the data
        vtss_bip_buffer_commit(&demo_rx_buffer); // Tell it to the BIP buffer.
    }

    DEMO_PLATFORM_CRIT_EXIT();

    // --- Signal the demo_thread(). See demo.c and look for symbol
    //     DEMO_FLAG_PACKET_RX.
    if (buf) {
        vtss_flag_setbits(&demo_thread_flag, DEMO_FLAG_PACKET_RX);
    }

}


// --- Register function
void demo_register_l2proto(void)
{

    l2_receive_register(VTSS_MODULE_ID_DEMO, demo_l2_rx);

}
