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


//#include "mscc/ethernet/switch/api.h"
#include "demo_api.h"




#ifdef VTSS_FEATURE_EVC_PORT_EVC

#include <vtss_api.h>

static int nni_port = 2-1;
static mesa_inst_t  inst = NULL;


vtss_rc create_evc(void)
{
  mesa_rc          rc;
  mesa_evc_id_t    evc_id = 1;
  mesa_evc_conf_t  evc_conf;

  // (1) --- Create an EVC


  // (1.1) --- The evc_conf must be setup. Start by making sure that the config structure is "clean".
  //
  bzero(&evc_conf, sizeof(evc_conf));

  // (1.2) --- The EVC may be associated with a policer. Some special values are defined
  //           in mesa_evc_api.h.
  //           Valid values are: 0,...,MESA_EVC_POLICERS-1, 
  //                             MESA_EVC_POLICER_ID_DISCARD,
  //                             MESA_EVC_POLICER_ID_NONE.
  //
  //           In this example we choose to have no policer associated with the EVC.
  //
  evc_conf.policer_id = MESA_EVC_POLICER_ID_NONE;

  // (1.3) --- No learning.
  evc_conf.learning = FALSE;

  // (1.4) --- Select which port should be NNI port for this EVC. Here we select to
  //           have only one NNI port, namely 'nni_port'
  //
  if ( !(nni_port<VTSS_PORT_ARRAY_SIZE) ) return VTSS_RC_ERROR; // Error: NNI port is not in the vlaid range.

  evc_conf.network.pb.nni[nni_port] = TRUE;

  // (1.5) --- Select the S-VID that this EVC shall use
  //
  evc_conf.network.pb.vid = 1234;

  // (1.6) --- The ivid is the VID the switch chip use internally to differentiate traffic for this
  //           EVC from other traffic. Most often the vid (above) and ivid is set to the same number.
  //           But that is not a requirement.
  //
  evc_conf.network.pb.ivid = 1234;

  // (1.7) --- Finally add this to the API.
  rc = mesa_evc_add(inst, evc_id, &evc_conf);

  return rc;
}

vtss_rc create_ece()
{
  vtss_rc          rc;

  // (2) --- Create an ECE / UNI
    mesa_ece_type_t     ece_type = MESA_ECE_TYPE_ANY;
    mesa_ece_t          ece_conf;
    const mesa_ece_id_t ece_id = 0;

    int                 uni_port = 0;

    rc = mesa_ece_init(inst, ece_type, &ece_conf);
    if (rc) return rc;

    ece_conf.id=1;

    // (2.2) --- The key determines which frames are subject to this ECE.

    // (2.3) --- The key.port_list specify which port ingress frames should
    //           come from. In this example we choose to have only one ingress
    // port namely 'uni_port'. Any number of ports can be specified.

    if ( !(uni_port<VTSS_PORT_ARRAY_SIZE) ) return VTSS_RC_ERROR; // Error: UNI port is not in the valid range.
    ece_conf.key.port_list[uni_port] = VTSS_ECE_PORT_ROOT;

    // (2.4) --- In this example we choose to filter on IPv4 frames, i.e., the key.type is set to
    //           MESA_ECE_TYPE_IPV4. This means that key.frame.ipv4 must also be setup correctly.
    //           Different switch chips support different vlaues of key.type. Basically if
    //           key.type = MESA_ECE_TYPE_XX, then key.frame.xx must also be set. The only exception
    //           is if key.type = MESA_ECE_TYPE_ANY.
    //
    ece_conf.key.type = MESA_ECE_TYPE_LLC;//MESA_ECE_TYPE_IPV4; // 5tupple_ip4

    // Lets say we dont care about any field in the IPv4 packet except for 
    // the source IP address, where we want addresses 10.11.X.X
    ece_conf.key.frame.ipv4.sip.value = 0x0a0b0000;
    ece_conf.key.frame.ipv4.sip.mask  = 0xffff0000;

    ece_conf.key.tag.tagged = MESA_VCAP_BIT_1;
    ece_conf.key.tag.vid.vr.v.value = 110;
    ece_conf.key.inner_tag.tagged = MESA_VCAP_BIT_0;
    ece_conf.key.inner_tag.vid.vr.v.value = 111;

    // In the datasheet for Serval1, it is stated, that three lookups can be
    // performed in the IS1. From our perspective, the first two is used for
    // ECE.
    ece_conf.key.lookup = 0;



    // So now they key specify, that ingress packets on port 'uni_port' that are
    // IPv4 frames with source IP address 10.11.X.X is subject to this rule.

    // [One thing we have not mentioned, and which will clutter up the picture at this point
    //  is, that the ingress port has to be configured to generate a key that conform with this
    //  key. We'll come back to that later.]

    // The next thing to do, is to configure what shall happen with frames that 
    // have the property above, i.e., we shall configure the action.

    // When an ECE is configured, we normally have the UNI->NNI direction or the
    // ingress rules on the UNI in mind. That was what we were thinking of, when
    // we created the key above.
    // But in the end we want a bidirectional connection. So we also want frames
    // that ingress on the NNI(s) of the EVC to egress the UNI.
    // [We might like these frames to conform to the key. I.e. in our examle, 
    //  a frames with S-VID=1234 that ingress on port 'nni_port' should
    //  egress on port 'uni_port' if it is an IPv4 frame with destination IP 
    //  address 10.11.X.X. But that is not possible.]
    // Therefore we tell the API to automatically generate the apropiated rule
    // in the NNI->UNI direction:
    //
    ece_conf.action.dir = MESA_ECE_DIR_BOTH;

    // When we do this, we implictily say, that our specification is an UNI->NNI
    // specification.

    // We can fine tune the rule generation some more. Normally we want, as already
    // mentioned, a rule to take care of the UNI->NNI direction, also calles the RX direction
    // and the NNI->UNI direction, i.e. the TX direction. So two rules can be made.
    // We can choose to do so, or we can specify that only the RX rule should be setup
    // or the TX rule should. This is done with action.rule which can be MESA_ECE_RULE_BOTH,
    // MESA_ECE_RULE_RX or MESA_ECE_RULE_TX.
    // In our example we setup both rules:
    // 
    ece_conf.action.rule = MESA_ECE_RULE_BOTH;

    ece_conf.action.evc_id=1;
    ece_conf.action.policer_id = MESA_EVC_POLICER_ID_NONE;


    // In order to complete this discussion, we could have set action.dir MESA_ECE_DIR_UNI_TO_NNI or
    // MESA_ECE_DIR_NNI_TO_UNI.
    // In the first case, we do not have to learn anything new, i.e. the key is specified with
    // the UNI in mind and the UNI->NNI rule will be configured just like before.
    // It does not matter if  action.rule is MESA_ECE_RULE_RX or MESA_ECE_RULE_BOTH. But is
    // action.rule is MESA_ECE_RULE_TX, then no rule will be configured since there is no
    // reverse direction in the unidirctionsl set.

    rc = mesa_ece_add(inst, ece_id, &ece_conf);

    return rc;
}

vtss_rc create_ece2()
{
  vtss_rc          rc;

  // (2) --- Create an ECE / UNI
    mesa_ece_type_t     ece_type = MESA_ECE_TYPE_ANY;
    mesa_ece_t          ece_conf;
    const mesa_ece_id_t ece_id = 0;

    int                 uni_port = 0;

    rc = mesa_ece_init(inst, ece_type, &ece_conf);
    if (rc) return rc;

    ece_conf.id=1;

    if ( !(uni_port<VTSS_PORT_ARRAY_SIZE) ) return VTSS_RC_ERROR; // Error: UNI port is not in the valid range.
    ece_conf.key.port_list[uni_port] = VTSS_ECE_PORT_ROOT;

    ece_conf.key.type = MESA_ECE_TYPE_LLC;//MESA_ECE_TYPE_IPV4; // 5tupple_ip4

    ece_conf.key.tag.tagged = MESA_VCAP_BIT_1;
    ece_conf.key.tag.vid.vr.v.value = 110;
    ece_conf.key.tag.vid.vr.v.mask = 0x3ff;
    ece_conf.key.inner_tag.tagged = MESA_VCAP_BIT_0;
    ece_conf.key.inner_tag.vid.vr.v.value = 111;
    ece_conf.key.inner_tag.vid.vr.v.mask = 0x3fe;

    ece_conf.key.lookup = 0;
    ece_conf.action.dir = MESA_ECE_DIR_NNI_TO_UNI;//MESA_ECE_DIR_BOTH;

    ece_conf.action.rule = MESA_ECE_RULE_BOTH;

    ece_conf.action.evc_id=1;
    ece_conf.action.policer_id = MESA_EVC_POLICER_ID_NONE;


    ece_conf.action.outer_tag.enable = TRUE;
    ece_conf.action.outer_tag.vid = 0x123;

    ece_conf.action.inner_tag.type = MESA_ECE_INNER_TAG_C;
    ece_conf.action.inner_tag.vid = 0x124;

    rc = mesa_ece_add(inst, ece_id, &ece_conf);

    return rc;
}


vtss_rc config_port_key()
{
  vtss_rc rc = VTSS_RC_OK;

#if defined(VTSS_FEATURE_EVC_PORT)

  vtss_port_no_t       port_no=0;
  mesa_evc_port_conf_t port_conf;

  // (1) --- Read the port configuration since we only want
  //         to change part of it.
  //

  rc = mesa_evc_port_conf_get(inst, port_no, &port_conf);

  // (2) --- Possible values for the port_conf.key_type is:
  //    VTSS_VCAP_KEY_TYPE_NORMAL
  //    VTSS_VCAP_KEY_TYPE_DOUBLE_TAG
  //    VTSS_VCAP_KEY_TYPE_IP_ADDR
  //    VTSS_VCAP_KEY_TYPE_MAC_IP_ADDR
  //
  // In the Serval1 data-sheet, there are 6 different IS1 keys (see section 3.7.3).
  // When a packet is received on a port a key is generated for that packet.
  // Which key that is generated is determined by the type of the packet,
  // i.e., IPv4, IPv6 or something else.
  // On a port basis it is configured which type of key that shall be generated
  // in each of these 3 cases (see section 6.1.8.3).
  //
  // In our example we are interested in the case where a IPv4 packet is received.
  // In this case one of 4 possibilities exist namely:
  //  - NORMAL, 7TUPLE, 5TUPLE_IP4 or DBL_VID
  // Since we want to filter on the source IP address, we must chose a key type
  // that contain that information. Looking in the datasheet it turns out, that
  // DBL_VID does not contain SIP. So that one can not be used, i.e. we can not
  // use VTSS_VCAP_KEY_TYPE_DOUBLE_TAG for the key_type.
  //
  // For NORMAL the datasheet (3.7.3) tell us, that this key contain the source IP
  // address. That is true if we have not configured the port to put the destination
  // IP address in the key instead. This is configured by key.dmac_dip = FALSE.
  // If we do that, then the key will work.
  //
  // For IP_ADDR, the key will be 5TUPLE_IP4 which will work
  // For MAC_IP_ADDR, the key will be 7TUPLE which will also work 
  //
  // One important thing to note is, that the key generation configured
  // on the port should be compatible with the key configuration in the
  // ECE. E.g. if the key 5TUPLE_IP4 is generated on the port, then only
  // entries with key type 5TUPLE_IP4 in the IS1   
  //
  port_conf.key_type = MESA_VCAP_KEY_TYPE_NORMAL;
  port_conf.dmac_dip = FALSE;

  rc = mesa_evc_port_conf_set(inst, port_no, &port_conf);
#endif
  return rc;
}

static vtss_rc create_vlan()
{
  mesa_rc      rc;
  mesa_vid_t   vid = 1234;
  //  BOOL         members[VTSS_PORT_ARRAY_SIZE];
  mesa_port_list_t members;
  mesa_bool_t val;
  
  rc = mesa_vlan_port_members_get(inst, vid, &members);
  if (rc) return rc;

  // Set port 1 (UNI) and port 2 (NNI to be member of VLAN 'vid'.
#if 0 //tf does not seem to work
  mesa_port_list_set(&members, 0, val);
  mesa_port_list_set(&members, 1, val);
#endif

  rc = mesa_vlan_port_members_set(inst, vid, &members);
  return rc;
}


static void  print_evc_counter(mesa_ece_id_t  ece_id,
                         vtss_port_no_t port_no,
                         mesa_evc_counters_t  *c)
{
  printf("For ece_id=%d, port_no=%d\n", ece_id, port_no);
  printf("  _______________frames_______bytes__\n");
  printf("  rx_green:  %10llu  %10llu\n",c->rx_green.frames,c->rx_green.bytes);
  printf("  rx_yellow: %10llu  %10llu\n",c->rx_yellow.frames,c->rx_yellow.bytes);
  printf("  rx_red:    %10llu  %10llu\n",c->rx_red.frames,c->rx_red.bytes);
  printf("  rx_discard:%10llu  %10llu\n",c->rx_discard.frames,c->rx_discard.bytes);
  printf("  tx_discard:%10llu  %10llu\n",c->tx_discard.frames,c->tx_discard.bytes);
  printf("  tx_green:  %10llu  %10llu\n",c->tx_green.frames,c->tx_green.bytes);
  printf("  tx_yellow: %10llu  %10llu\n",c->tx_yellow.frames,c->tx_yellow.bytes);
}


static vtss_rc clear_ece_counter()
{
  vtss_rc rc;
  mesa_evc_id_t  evc_id = 0;
  vtss_port_no_t port_no;

  port_no = 0;
  rc = mesa_evc_counters_clear(inst, evc_id, port_no);
  if (rc) return rc;

  port_no = 1;
  rc = mesa_evc_counters_clear(inst, evc_id, port_no);

  return rc;
}


static vtss_rc get_ece_counter()
{
  vtss_rc rc;
  mesa_ece_id_t  ece_id = 1;
  vtss_port_no_t port_no;
  mesa_evc_counters_t  counters;


  port_no = 0;
  rc = mesa_ece_counters_get(inst, ece_id, port_no, &counters);
  if (rc) return rc;
  print_evc_counter(ece_id, port_no, &counters);

  port_no = 1;
  rc = mesa_ece_counters_get(inst, ece_id, port_no, &counters);
  if (rc) return rc;
  print_evc_counter(ece_id, port_no, &counters);

  return rc;
}



void demo_evc_main()
{
  vtss_rc rc;

  // Create EVC object
  rc = create_evc();
  if (rc) printf("EVC setup failed\n");

  // Configure UNI port to generate appropiated key
  rc = config_port_key();
  if (rc) printf("Port setup failed\n");

  // Insert rule in IS1 for assigning packets to EVC
  rc = create_ece2();
  if (rc) printf("ECE setup failed\n");

  // Configure UNI and NNI ports to be member of VID 1234
  rc =  create_vlan();
  if (rc) printf("VLAN setup failed\n");
}



void demo_evc_main_ece_clear(vtss_port_no_t port_no)
{
  vtss_rc rc;

  rc =  clear_ece_counter();
  if (rc) printf("Failed to clear ECE counter\n");
}



void demo_evc_main_ece_statistics(vtss_port_no_t port_no)
{
  vtss_rc rc;

  rc =  get_ece_counter();
  if (rc) printf("Failed to get ECE counter\n");
}


#else

void demo_evc_main(void)
{
  printf("VTSS_FEATURE_EVC_PORT_EVC is not defined\n");
}

void demo_evc_main_ece_clear(mesa_port_no_t port_no)
{
  printf("VTSS_FEATURE_EVC_PORT_EVC is not defined\n");
}

void demo_evc_main_ece_statistics(mesa_port_no_t port_no)
{
  printf("VTSS_FEATURE_EVC_PORT_EVC is not defined\n");
}

#endif
