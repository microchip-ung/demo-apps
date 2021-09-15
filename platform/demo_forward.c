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
#include <packet_api.h>
#include <acl_api.h>

#include "demo.h"
#include "demo_api.h"
#include "demo_trace.h"


static vtss_rc forward_frame_to_cpu_method1(BOOL enable); 
static vtss_rc forward_frame_to_cpu_method2(BOOL enable);
static vtss_rc forward_frame_to_cpu_method3(BOOL enable);


// --- In order for packets with MAC 01:80:C2:00:00:2F (see dmac in demo_packet.c)
//     to be forwarded to the CPU, some bits in the chip ANA:PORT:CPU_FWD_GARP_CFG
//     must be set.
//
void debug_forwarding_control(BOOL enable, int method)
{
    vtss_rc               rc;

    T_DG(TRACE_GRP_FORWARD, "demo_forwarding_control enable=%d", (int)enable);

    // --- Take the API lock
    vtss_appl_api_lock();


    switch (method) {
    case 1:
      rc = forward_frame_to_cpu_method1(enable);
      T_DG(TRACE_GRP_FORWARD, "forward_frame_to_cpu_method1() (%s)", error_txt(rc));
      break;

    case 2:
      rc = forward_frame_to_cpu_method2(enable);
      T_DG(TRACE_GRP_FORWARD, "forward_frame_to_cpu_method2() (%s)", error_txt(rc));
      break;

    case 3:
      rc = forward_frame_to_cpu_method3(enable);
      T_DG(TRACE_GRP_FORWARD, "forward_frame_to_cpu_method3() (%s)", error_txt(rc));
      break;

    default:
      T_WG(TRACE_GRP_FORWARD, "debug_forwarding_control() option %d not defined", method);
    }


    // --- and release the API lock again.
    vtss_appl_api_unlock();
}


// --- In order for packets with MAC 01:80:C2:00:00:2F (see dmac in demo_packet.c)
//     to be forwarded to the CPU, some bits in the chip ANA:PORT:CPU_FWD_GARP_CFG
//     must be set.
//
static vtss_rc forward_frame_to_cpu_method1(BOOL enable)
{
    vtss_rc rc;
    mesa_packet_rx_conf_t conf;

    bzero(&conf, sizeof(conf));

    // (1) --- Read the configuration
    rc = mesa_packet_rx_conf_get(NULL, &conf);

    if (rc != VTSS_OK) {
        T_EG(TRACE_GRP_FORWARD, "Failed to get packet_rx_conf (%s)", error_txt(rc));
	return VTSS_RC_ERROR;
    }

    // (2) --- Change configuration and write it back
    //        0x2f is the last byte of dmac in demo_packet.c
    conf.reg.garp_cpu_only[0x2f & 0xf] = enable;
    rc = mesa_packet_rx_conf_set(NULL, &conf);


    if (rc != VTSS_OK) {
        T_EG(TRACE_GRP_FORWARD, "Failed to set packet_rx_conf (%s)", error_txt(rc));
    }

    return rc;
}


static const u8  dmac[6] = {0x00, 0x01, 0xc1, 0x11, 0x22, 0x33};
static const mesa_vid_t  vid  = 1;

// --- Here we register specific on destination MAC 00:01:C1:11:22:33 (see dmac in demo_packet.c)
//     to be forwarded to the CPU, some bits in the chip ANA:PORT:CPU_FWD_GARP_CFG
//     must be set.
//
static vtss_rc forward_frame_to_cpu_method2(BOOL enable)
{
    vtss_rc rc;

    int i;
    mesa_mac_table_entry_t entry;


    // (1) --- Clear everything in entry. Set the
    //         destination MAC address and VID.
    bzero(&entry, sizeof(entry));

    entry.vid_mac.vid = vid;
    for (i=0; i<6; ++i) {
        entry.vid_mac.mac.addr[i] = dmac[i]; 
    }

    // --- Frames shall go to the CPU, and
    //     the entry shall be static.
    entry.copy_to_cpu = TRUE;
    entry.locked      = TRUE;


    // (2) --- Add or delete entry from MAC table.
    if (enable) {

      rc = mesa_mac_table_add(NULL, &entry);

    } else {

      rc = mesa_mac_table_del(NULL, &entry.vid_mac);

    }


    if (rc != VTSS_OK) {
        T_EG(TRACE_GRP_FORWARD, "mesa_mac_table_add() failed (%s)", error_txt(rc));
    }

    return rc;
}

#if 1
static vtss_rc forward_frame_to_cpu_method3(BOOL enable)
{
    vtss_rc rc = VTSS_RC_OK;

    mesa_ace_type_t type;
    mesa_ace_t ace;

    // Lets make an ethernet match, and initialyze 'ace'.
    type = MESA_ACE_TYPE_ETYPE;

    rc = mesa_ace_init(NULL, type, &ace);
    if (rc) return rc;

#if 0
    // Set relevant fields.
    ace.frame.etype.dmac =
    ace.frame.etype.smac =
    ace.frame.etype.etype =
    ace.frame.etype.data =

    // and make it effective
    rc = mesa_ace_add(NULL, id, &ace);
#endif

    return rc;
}




#else

static vtss_rc forward_frame_to_cpu_method3(BOOL enable)
{
    vtss_rc rc = VTSS_RC_OK;
    acl_entry_conf_t conf;

    // (1) --- IPv4 entry
    if (acl_mgmt_ace_init(VTSS_ACE_TYPE_IPV4, &conf) != VTSS_OK) {
        return rc;
    }

    conf.id = 1;//DHCP_HELPER_BOOTPS_ACE_ID;

    // (2) --- Port for which this apply
#if defined(VTSS_FEATURE_ACL_V2)
    conf.action.port_action = VTSS_ACL_PORT_ACTION_FILTER;
    memset(conf.action.port_list, 0, sizeof(conf.action.port_list));
#else
    conf.action.permit = FALSE;
#endif /* VTSS_FEATURE_ACL_V2 */

    conf.action.force_cpu = TRUE;
    conf.action.cpu_once = FALSE;
    conf.isid = VTSS_ISID_LOCAL;

    // (3) --- match 192.168.x.x
    conf.frame.ipv4.dip.value=0xC0A80000;
    conf.frame.ipv4.dip.mask =0xFFFF0000;

    if (acl_mgmt_ace_add(ACL_USER_NAT, ACL_MGMT_ACE_ID_NONE, &conf) != VTSS_OK) {
      //        T_D("Add DHCP helper reserved ACE (BOOTPS) fail.\n");
    }

    return rc;
}


#endif
