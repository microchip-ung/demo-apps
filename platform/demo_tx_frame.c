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


//#include <packet_api.h>
#include <l2proto_api.h>
#include <vtss_common_os.h>
#include <string.h>
#include <conf_api.h>
#include <misc_api.h>
#include "demo_api.h"

#include "demo.h"
#include "demo_trace.h"
#include "packet_api.h"


#undef  DEMO_PACKET_TX_CB


static u8 frame[64] = {
  0x00, 0x01, 0xc1, 0x11, 0x22, 0x33, // Destination MAC address
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Source MAC address
  0x00, 0x40,                         // Type/length field
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06
};


#define DEMO_MAC_ADDRESS_LEN 6


// --- This function lookup the MAC address of port 'port_no'
//     and insert it into frame pointed at by 'p'.
static mesa_rc update_src_mac(mesa_port_no_t port_no, void* p, mesa_mac_t dmac)
{
    vtss_isid_t     isid;
    mesa_port_no_t  switch_port;
    u8              sys_mac_addr[DEMO_MAC_ADDRESS_LEN];

    // --- Map the 'port_no' to internal switch ID 'isid' and port
    //     on that switch 'switch_port'
    if (l2port2port(port_no, &isid, &switch_port) == FALSE) {
      return MESA_RC_ERROR;
    }

    // --- Get the system MAC address
    (void)conf_mgmt_mac_addr_get(sys_mac_addr, 0);
    
    // --- Put source port MAC address into frame. The port MAC address is
    //     the system MAC address plus the port number counting from 1.
    misc_instantiate_mac(&frame[6], sys_mac_addr, switch_port - VTSS_PORT_NO_START + 1);

    memcpy(p, &dmac.addr[0], 6);
    
    return MESA_RC_OK;
}


// --- This function is an example of how to
//     transmit a frame.
//     This function is called from ICLI.
//     The ICLI commands are like this (look in demo.icli):
//     # configure terminal
//     (config)# interface gi 1/8
//     (config-if)# demotx
//

static mesa_rc demo_os_tx_method_1(mesa_port_no_t port_no, mesa_mac_t dmac)
{
    mesa_rc rc;
    void *p;
    vtss_common_framelen_t length=64;
    vtss_common_bufref_t context;

    T_IG(TRACE_GRP_TX, "port_no=%d", port_no + 1);

    // (1) --- Allocate PDU
    p = vtss_os_alloc_xmit(port_no, length, &context);
    if (!p) {
      T_WG(TRACE_GRP_TX, "Failed to allocate TX buffer");
      return MESA_RC_ERROR;
    }

    // (2) --- Copy prototype of frame into 'p'
    memcpy(p, frame, length);

    // (3) --- Put in the source MAC address.
    rc =  update_src_mac(port_no, p, dmac);
    if (rc) return rc;

    // (4) --- And sent frame.
    rc = vtss_os_xmit(port_no, p, length, context);
        
    return rc;
}


#ifdef  DEMO_PACKET_TX_CB
static void demo_tx_done_cb(void *context, /* Callback defined */
                            packet_tx_done_props_t *props)  /* Tx Done status   */
{
    T_IG(TRACE_GRP_TX, "demo_tx_done_cb called context=%p\n", context);
    T_IG(TRACE_GRP_TX,"props.frm_ptr[]=%p %p\n", props->frm_ptr[0], props->frm_ptr[1]);

    if (props->frm_ptr[0]) packet_tx_free(props->frm_ptr[0]);
    if (props->frm_ptr[1]) packet_tx_free(props->frm_ptr[1]);
}
#endif

/*
 * If swtch==FALSE, then a frame with destination MAC address 'dmac' is sent on
 * port 'port_no'.
 * If swtch==TRUE, then the frame is switched and 'port_no- is ignored.
 */
static mesa_rc demo_packet_tx_method_2(mesa_port_no_t port_no, mesa_mac_t dmac, int swtch)
{
    mesa_rc rc;
    u8 *p;
    vtss_common_framelen_t length=64;
    packet_tx_props_t tx_props;

    T_IG(TRACE_GRP_TX, "port_no=%d", port_no + 1);

    // (1) --- Allocate PDU
    p = packet_tx_alloc(length);
    if (!p) {
      T_WG(TRACE_GRP_TX, "Failed to allocate TX buffer");
      return MESA_RC_ERROR;
    }

    T_IG(TRACE_GRP_TX, "p=%p\n", p);

    // (2) --- Copy prototype of frame into 'p'
    memcpy(p, frame, length);

    // (3) --- Put in the source MAC address, and destination MAC 'dmac' into the frame to send.
    rc =  update_src_mac(port_no, p, dmac);
    if (rc) return rc;

    // (4) --- Setup parameters for the TX function.
    packet_tx_props_init(&tx_props);

    tx_props.packet_info.modid  = VTSS_MODULE_ID_DEMO;
    tx_props.packet_info.frm = (u8 *)p;
    tx_props.packet_info.len = length;
    tx_props.tx_info.cos        = 7;
    tx_props.tx_info.tag.vid    = 1; 
    tx_props.tx_info.masquerade_port = VTSS_PORT_NO_NONE;
    
    tx_props.tx_info.dst_port_mask = VTSS_BIT64(port_no); // Not used if 'switch_frm' is true.
    tx_props.tx_info.switch_frm = swtch ? TRUE : FALSE;


    // (4.1) --- If a tx_done_cb function is not defined, then the system will take
    //           care of releasing resources. Otherwise it is up to the tx_dome-cb
    //           funciton to do that.
#ifdef DEMO_PACKET_TX_CB
    tx_props.packet_info.tx_done_cb = demo_tx_done_cb;
    tx_props.packet_info.tx_done_cb_context =(void*)0x12345678;
#endif

    T_IG(TRACE_GRP_TX, "switch_frm=%d", tx_props.tx_info.switch_frm);

    rc = packet_tx(&tx_props);

    return rc;
}



mesa_rc demo_tx_frame(mesa_port_no_t port_no, mesa_mac_t dmac, int method, int swtch)
{
    mesa_rc rc;

    if (method) {
        T_IG(TRACE_GRP_TX, "Run packet_tx() method 2\n");
        rc = demo_packet_tx_method_2(port_no, dmac, swtch);
    } else {
        T_IG(TRACE_GRP_TX, "Run vtss_os_tx() method 1\n");
        rc = demo_os_tx_method_1(port_no, dmac);
    }

    return rc;
}
