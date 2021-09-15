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


#include <packet_api.h>
#include <l2proto_api.h>

#include "demo.h"
#include "demo_trace.h"


// --- This is the we will register on, i.e., when a packet is received
//     on the switch with this destination MAC address, it shall be
//     sent to the CPU.
//static const mac_addr_t  dmac = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x2f};
static const u8 dmac[6] = {0x00, 0x01, 0xc1, 0x11, 0x22, 0x33};



// --- This function is called when a packet is received on this switch
//     which the attributes as given in the demo_packet_register() function.
static BOOL demo_packet_rx(void *contxt,
                           const uchar *const frm,
                           const vtss_packet_rx_info_t *const rx_info)
{
    // --- We have received a packet. Forward it to the master by means of then l2proto module

    l2_receive_indication(VTSS_MODULE_ID_DEMO, frm, rx_info->length, rx_info->port_no, rx_info->tag.vid, VTSS_GLAG_NO_NONE);

    T_DG(TRACE_GRP_PACKET, "Received packet");

    // --- When TRUE is returned, then no other subscribers will get this packet.
    return TRUE;
}



// --- This function register on an destination MAC address of LLC type
//     whenever such a packet is received on the switch, it is sent to the CPU
//     and the function demo_packet_rx() is called.
void demo_register_packet(void)
{
    packet_rx_filter_t  rx_filter;
    void               *rx_filter_id;
    vtss_rc            rc;

    // --- Setup packet rx handler for demo module.
    bzero(&rx_filter, sizeof(rx_filter));
    rx_filter.modid = VTSS_MODULE_ID_DEMO;
    rx_filter.cb    = demo_packet_rx;

    // --- Configure which ethernet packet we want.
    memcpy(rx_filter.dmac, dmac, sizeof(rx_filter.dmac));
    rx_filter.match  = PACKET_RX_FILTER_MATCH_DMAC;
    rx_filter.prio   = PACKET_RX_FILTER_PRIO_LOW;
    rx_filter.etype  = 0;
    rx_filter.contxt = NULL;

    // --- Register this with the system.
    rc = packet_rx_filter_register(&rx_filter, &rx_filter_id);

    if (rc != VTSS_RC_OK) {
        T_EG(TRACE_GRP_PACKET, "Failed to register for demo packets (%s)", error_txt(rc));
    }

}
