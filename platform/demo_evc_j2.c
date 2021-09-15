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

#include "demo_api.h"
#include "demo_util.h"

#ifdef VTSS_FEATURE_EVC_PORT_EVC



#define DEMO_ENCAP
#undef  DEMO_EVC2

static vtss_port_no_t nni_port = 2-1;
static vtss_port_no_t uni_port = 1-1;

static  vtss_inst_t   inst = NULL;
static  vtss_evc_id_t evc_id = 1;
static  vtss_evc_id_t evc_id2 = 2;

static vtss_qos_egress_map_id_t  egress_map_id = 1;
static vtss_qos_ingress_map_id_t ingress_map_id = 5;


vtss_rc create_evc(void)
{
  vtss_rc          rc;
  vtss_evc_conf_t  evc_conf;
  vtss_evc_port_evc_conf_t evc_port_conf;

  // (1) --- Create an EVC


  // (1.1) --- The evc_conf must be setup. Start by making sure that the config structure is "clean".
  //
  bzero(&evc_conf, sizeof(evc_conf));

  // (1.2) --- In the J2 architecture, and EVC is not configured in this place
  //           in contrast to e.g. Serval1


  // (1.3) --- No learning.
  //
  evc_conf.learning = FALSE;

  // (1.4) --- Select which port should be NNI port for this EVC. Here we select to
  //           have only one NNI port, namely 'nni_port'
  //
  //           For J2 use the vtss_evc_port_evc_conf_*() for adding NNI ports to an EVC.
  //           This is different for Serval-1

  // (1.5) --- Select the S-VID that this EVC shall use
  //
  evc_conf.network.pb.vid = 1234;

  // (1.6) --- The ivid is the VID the switch chip use internally to differentiate traffic for this
  //           EVC from other traffic. Most often the vid (above) and ivid is set to the same number.
  //           But that is not a requirement.
  //
  evc_conf.network.pb.ivid = 1234;

  // (1.7) --- For J2 there is also a vid and ivid for leafs
  //  evc_conf.network.pb.leaf_vid = 1235;
  //  evc_conf.network.pb.leaf_ivid = 1235;

  // (1.8) --- J2: Assign a policy to frames received on NNI in order to hit ACL rule.
  //
  evc_conf.network.pb.policy_no = 12;

  // (1.9) --- J2: Assign a egress map for NNI port in the EVC. The actual map is setup 
  //           in create_egress_map(). The map is setup so that the colour of the
  //           egress frame us swapped, i.e. green is yellow and yellow is green.
  //
  evc_conf.network.pb.tx_map = egress_map_id;

  // (1.10) --- J2: An ingress map can also be configured.
  //
  evc_conf.network.pb.rx_map = ingress_map_id;

  // (1.11) --- Finally add this to the API.
  //
  rc = vtss_evc_add(inst, evc_id, &evc_conf);

  // (1.12) -- Configure port 'nni_port' to be an NNI port 
  //
  rc = vtss_evc_port_evc_conf_get(inst, evc_id, nni_port, &evc_port_conf);
  evc_port_conf.role = VTSS_EVC_PORT_ROLE_NNI;
  rc = vtss_evc_port_evc_conf_set(inst, evc_id, nni_port, &evc_port_conf);

  // (1.13) -- Configure port 'uni_port' to be an UNI port with Root role
  //
  rc = vtss_evc_port_evc_conf_get(inst, evc_id, uni_port, &evc_port_conf);
  evc_port_conf.role = VTSS_EVC_PORT_ROLE_ROOT;

/* #ifdef DEMO_ENCAP */
/*   // Set encap id */
/*   evc_port_conf.encap_enable = TRUE; */
/*   evc_port_conf.encap_id = 1; */
/* #endif */

  rc = vtss_evc_port_evc_conf_set(inst, evc_id, uni_port, &evc_port_conf);

  return rc;
}


vtss_rc create_evc2(void)
{
  vtss_rc          rc;
  vtss_evc_conf_t  evc_conf;
  vtss_evc_port_evc_conf_t evc_port_conf;

  // (1) --- Create an EVC


  // (1.1) --- The evc_conf must be setup. Start by making sure that the config structure is "clean".
  //
  bzero(&evc_conf, sizeof(evc_conf));

  // (1.2) --- In the J2 architecture, and EVC is not configured in this place
  //           in contrast to e.g. Serval1


  // (1.3) --- No learning.
  //
  evc_conf.learning = FALSE;

  // (1.4) --- Select which port should be NNI port for this EVC. Here we select to
  //           have only one NNI port, namely 'nni_port'
  //
  //           For J2 use the vtss_evc_port_evc_conf_*() for adding NNI ports to an EVC.
  //           This is different for Serval-1

  // (1.5) --- Select the S-VID that this EVC shall use
  //
  evc_conf.network.pb.vid = 1234+1;

  // (1.6) --- The ivid is the VID the switch chip use internally to differentiate traffic for this
  //           EVC from other traffic. Most often the vid (above) and ivid is set to the same number.
  //           But that is not a requirement.
  //
  evc_conf.network.pb.ivid = 1234+1;

  // (1.7) --- For J2 there is also a vid and ivid for leafs
  //  evc_conf.network.pb.leaf_vid = 1235;
  //  evc_conf.network.pb.leaf_ivid = 1235;

  // (1.8) --- J2: Assign a policy to frames received on NNI in order to hit ACL rule.
  //
  evc_conf.network.pb.policy_no = 12;

  // (1.9) --- J2: Assign a egress map for NNI port in the EVC. The actual map is setup 
  //           in create_egress_map(). The map is setup so that the colour of the
  //           egress frame us swapped, i.e. green is yellow and yellow is green.
  //
  evc_conf.network.pb.tx_map = egress_map_id;

  // (1.10) --- J2: An ingress map can also be configured.
  //
  evc_conf.network.pb.rx_map = ingress_map_id;

  // (1.11) --- Finally add this to the API.
  //
  rc = vtss_evc_add(inst, evc_id2, &evc_conf);

  // (1.12) -- Configure port 'nni_port' to be an NNI port 
  //
  rc = vtss_evc_port_evc_conf_get(inst, evc_id2, nni_port, &evc_port_conf);
  evc_port_conf.role = VTSS_EVC_PORT_ROLE_NNI;
  rc = vtss_evc_port_evc_conf_set(inst, evc_id2, nni_port, &evc_port_conf);

  // (1.13) -- Configure port 'uni_port' to be an UNI port with Root role
  //
  rc = vtss_evc_port_evc_conf_get(inst, evc_id2, uni_port, &evc_port_conf);
  evc_port_conf.role = VTSS_EVC_PORT_ROLE_ROOT;

#ifdef DEMO_ENCAP
  // Set encap id
  evc_port_conf.encap_enable = TRUE;
  evc_port_conf.encap_id = 1;
#endif

  rc = vtss_evc_port_evc_conf_set(inst, evc_id2, uni_port, &evc_port_conf);

  return rc;
}


vtss_rc create_ece()
{
  vtss_rc          rc;

  // (2) --- Create an ECE / UNI
  vtss_ece_type_t     ece_type = VTSS_ECE_TYPE_ANY;
  vtss_ece_t          ece_conf;
  const vtss_ece_id_t ece_id = 0;

  rc = vtss_ece_init(inst, ece_type, &ece_conf);
  if (rc) return rc;


    ece_conf.id=1;

    // (2.2) --- The key determines which frames are subject to this ECE.

    // (2.3) --- The key.port_list specify which port ingress frames should
    //           come from. In this example we choose to have only one ingress
    // port namely 'uni_port'. Any number of ports can be specified.

    if ( !(uni_port<VTSS_PORT_ARRAY_SIZE) ) return VTSS_RC_ERROR; // Error: UNI port is not in the valid range.
    ece_conf.key.port_list[uni_port] = VTSS_ECE_PORT_ROOT;
    /* ece_conf.key.port_list[nni_port] = VTSS_ECE_PORT_ROOT; */

    // (2.4) --- In this example we choose to filter on IPv4 frames, i.e., the key.type is set to
    //           VTSS_ECE_TYPE_IPV4. This means that key.frame.ipv4 must also be setup correctly.
    //           Different switch chips support different vlaues of key.type. Basically if
    //           key.type = VTSS_ECE_TYPE_XX, then key.frame.xx must also be set. The only exception
    //           is if key.type = VTSS_ECE_TYPE_ANY.
    //

#define EX1
#undef EX2
#undef EX3

#ifdef EX1

    // Ex.1: Assign any frames received at the UNI to the EVC
    ece_conf.key.type = VTSS_ECE_TYPE_ANY;

#endif

#ifdef EX2

    // Ex. 2: Assign IPv4 frames with SIP=10.11.*.* and one VLAN tag
    ece_conf.key.type = VTSS_ECE_TYPE_IPV4;

    // Lets say we dont care about any field in the IPv4 packet except for 
    // the source IP address, where we want addresses 10.11.X.X
    ece_conf.key.frame.ipv4.sip.value = 0x0a0b0000;
    ece_conf.key.frame.ipv4.sip.mask  = 0xffff0000;

    // ...and the shall have one VLAN tag and VID=110
    ece_conf.key.tag.tagged = VTSS_VCAP_BIT_1;
    ece_conf.key.tag.s_tagged = VTSS_VCAP_BIT_0;
    ece_conf.key.tag.vid.vr.v.value = 110;
    ece_conf.key.tag.vid.vr.v.mask = 0xfff;
    ece_conf.key.inner_tag.tagged = VTSS_VCAP_BIT_0;
    ece_conf.key.inner_tag.s_tagged = VTSS_VCAP_BIT_0;
    ece_conf.key.inner_tag.vid.vr.v.value = 111;
    ece_conf.key.inner_tag.vid.vr.v.mask = 0x0;

#endif

#ifdef EX3

    // Ex. 3: Assign IPv4 frames with SIP=10.11.*.* and one VLAN tag
    ece_conf.key.type = VTSS_ECE_TYPE_ANY;

    ece_conf.key.tag.tagged = VTSS_VCAP_BIT_1;
    ece_conf.key.tag.s_tagged = VTSS_VCAP_BIT_1;
    ece_conf.key.tag.dei = VTSS_VCAP_BIT_1;
    ece_conf.key.tag.vid.vr.v.value = 1234;
    ece_conf.key.tag.vid.vr.v.mask = 0xfff;
    ece_conf.key.inner_tag.tagged = VTSS_VCAP_BIT_0;
    /* ece_conf.key.inner_tag.s_tagged = VTSS_VCAP_BIT_0; */
    /* ece_conf.key.inner_tag.vid.vr.v.value = 111; */
    /* ece_conf.key.inner_tag.vid.vr.v.mask = 0x0; */

#endif


    // So now the key specify, that ingress packets on port 'uni_port' that are
    // IPv4 frames with source IP address 10.11.X.X is subject to this rule.

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

    ece_conf.action.dir = VTSS_ECE_DIR_BOTH;
    /* ece_conf.action.dir = VTSS_ECE_DIR_NNI_TO_UNI; */
    /* ece_conf.action.dir = VTSS_ECE_DIR_UNI_TO_NNI; */

    ece_conf.action.policy_no=13;

    // When we do this, we implictily say, that our specification is an UNI->NNI
    // specification.

    // We can fine tune the rule generation some more. Normally we want, as already
    // mentioned, a rule to take care of the UNI->NNI direction, also calles the RX direction
    // and the NNI->UNI direction, i.e. the TX direction. So two rules can be made.
    // We can choose to do so, or we can specify that only the RX rule should be setup
    // or the TX rule should. This is done with action.rule which can be VTSS_ECE_RULE_BOTH,
    // VTSS_ECE_RULE_RX or VTSS_ECE_RULE_TX.
    // In our example we setup both rules:
    // 
    // J2: Not applicable

    ece_conf.action.evc_id=evc_id;

    // ece_conf.action.policer_id = VTSS_EVC_POLICER_ID_NONE;
    // J2: Policers are specified with vtss_evc_cosid_policer_conf_*()

    // In order to complete this discussion, we could have set action.dir VTSS_ECE_DIR_UNI_TO_NNI or
    // VTSS_ECE_DIR_NNI_TO_UNI.
    // In the first case, we do not have to learn anything new, i.e. the key is specified with
    // the UNI in mind and the UNI->NNI rule will be configured just like before.
    // It does not matter if  action.rule is VTSS_ECE_RULE_RX or VTSS_ECE_RULE_BOTH. But if
    // action.rule is VTSS_ECE_RULE_TX, then no rule will be configured since there is no
    // reverse direction in the unidirctionsl set.

    rc = vtss_ece_add(inst, ece_id, &ece_conf);

    return rc;
}

vtss_rc create_ece2()
{
  vtss_rc          rc;

  // (2) --- Create an ECE / UNI
  vtss_ece_type_t     ece_type = VTSS_ECE_TYPE_ANY;
  vtss_ece_t          ece_conf;
  const vtss_ece_id_t ece_id2 = 0;

  rc = vtss_ece_init(inst, ece_type, &ece_conf);
  if (rc) return rc;


    ece_conf.id=1+1;

    // (2.2) --- The key determines which frames are subject to this ECE.

    // (2.3) --- The key.port_list specify which port ingress frames should
    //           come from. In this example we choose to have only one ingress
    // port namely 'uni_port'. Any number of ports can be specified.

    if ( !(uni_port<VTSS_PORT_ARRAY_SIZE) ) return VTSS_RC_ERROR; // Error: UNI port is not in the valid range.
    /* ece_conf.key.port_list[uni_port] = VTSS_ECE_PORT_ROOT; */
    ece_conf.key.port_list[nni_port] = VTSS_ECE_PORT_ROOT;

    // (2.4) --- In this example we choose to filter on IPv4 frames, i.e., the key.type is set to
    //           VTSS_ECE_TYPE_IPV4. This means that key.frame.ipv4 must also be setup correctly.
    //           Different switch chips support different vlaues of key.type. Basically if
    //           key.type = VTSS_ECE_TYPE_XX, then key.frame.xx must also be set. The only exception
    //           is if key.type = VTSS_ECE_TYPE_ANY.
    //


    // Ex. 3: Assign IPv4 frames with SIP=10.11.*.* and one VLAN tag
    ece_conf.key.type = VTSS_ECE_TYPE_ANY;

    ece_conf.key.tag.tagged = VTSS_VCAP_BIT_1;
    ece_conf.key.tag.s_tagged = VTSS_VCAP_BIT_0;
    ece_conf.key.tag.dei = VTSS_VCAP_BIT_ANY;
    ece_conf.key.tag.vid.vr.v.value = 1234;
    ece_conf.key.tag.vid.vr.v.mask = 0xfff;
    ece_conf.key.inner_tag.tagged = VTSS_VCAP_BIT_0;

    // So now the key specify, that ingress packets on port 'uni_port' that are
    // IPv4 frames with source IP address 10.11.X.X is subject to this rule.

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

    /* ece_conf.action.dir = VTSS_ECE_DIR_BOTH; */
    ece_conf.action.dir = VTSS_ECE_DIR_NNI_TO_UNI;
    /* ece_conf.action.dir = VTSS_ECE_DIR_UNI_TO_NNI; */

    ece_conf.action.policy_no=13;

    // When we do this, we implictily say, that our specification is an UNI->NNI
    // specification.

    // We can fine tune the rule generation some more. Normally we want, as already
    // mentioned, a rule to take care of the UNI->NNI direction, also calles the RX direction
    // and the NNI->UNI direction, i.e. the TX direction. So two rules can be made.
    // We can choose to do so, or we can specify that only the RX rule should be setup
    // or the TX rule should. This is done with action.rule which can be VTSS_ECE_RULE_BOTH,
    // VTSS_ECE_RULE_RX or VTSS_ECE_RULE_TX.
    // In our example we setup both rules:
    // 
    // J2: Not applicable

    ece_conf.action.evc_id=evc_id2;
    ece_conf.action.pop_tag = VTSS_ECE_POP_TAG_1;

    // ece_conf.action.policer_id = VTSS_EVC_POLICER_ID_NONE;
    // J2: Policers are specified with vtss_evc_cosid_policer_conf_*()

    // In order to complete this discussion, we could have set action.dir VTSS_ECE_DIR_UNI_TO_NNI or
    // VTSS_ECE_DIR_NNI_TO_UNI.
    // In the first case, we do not have to learn anything new, i.e. the key is specified with
    // the UNI in mind and the UNI->NNI rule will be configured just like before.
    // It does not matter if  action.rule is VTSS_ECE_RULE_RX or VTSS_ECE_RULE_BOTH. But if
    // action.rule is VTSS_ECE_RULE_TX, then no rule will be configured since there is no
    // reverse direction in the unidirctionsl set.

    rc = vtss_ece_add(inst, ece_id2, &ece_conf);

    return rc;
}


vtss_rc create_qos()
{
  vtss_rc rc;

  vtss_qos_ingress_map_key_t ikey;
  vtss_qos_ingress_map_t     imap;

  int i_pcp, i_dei;

  ikey = VTSS_QOS_INGRESS_MAP_KEY_PCP_DEI;

  rc = vtss_qos_ingress_map_init(inst, ikey, &imap);
  if (rc) return rc;

  imap.id = 1; // 0, ..., 255

  // The map.key has been set by vtss_qos_ingress_map_init(), so do not do that again.

  // All the map.action.cos, .dpl, .pcp, .dei, .dscp, .cosid, .path are false by default
  // (from vtss_qos_ingress_init_map(), so we do only set the fields that need to be true.

  imap.action.dpl   = TRUE; // Replace the classified DPL

  // Since key = VTSS_QOS_INGRESS_MAP_KEY_PCP_DEI, then map.maps.pcp_dei must be configured.
  // and since map.dpl = TRUE the map.maps.pcp_dei[].dpl must be configured.
  for (i_pcp=0; i_pcp<8; ++i_pcp) {
    for (i_dei=0; i_dei<2; ++i_dei) {
      imap.maps.pcp_dei[i_pcp][i_dei].dpl = i_dei ? TRUE : FALSE;
    }
  }

  // --- Egress
  vtss_qos_egress_map_key_t ekey;
  vtss_qos_egress_map_t     emap;

  ekey = VTSS_QOS_EGRESS_MAP_KEY_COSID;

  rc = vtss_qos_egress_map_init(inst, ekey, &emap);
  emap.id = 1; //  [ VTSS_QOS_EGRESS_MAP_ID_START(0) ; VTSS_QOS_EGRESS_MAP_ID_END(512) [

  rc = vtss_qos_egress_map_add(inst, &emap);
  return rc;
}


static vtss_rc create_vlan()
{
  vtss_rc      rc;
  vtss_vid_t   vid = 1234;
  BOOL         members[VTSS_PORT_ARRAY_SIZE];

  rc = vtss_vlan_port_members_get(inst, vid, members);
  if (rc) return rc;

  // Set port 1 (UNI) and port 2 (NNI to be member of VLAN 'vid'.
  members[0]=members[1]=TRUE;

  rc = vtss_vlan_port_members_set(inst, vid, members);
  return rc;
}

static vtss_rc create_vlan2()
{
  vtss_rc      rc;
  vtss_vid_t   vid = 1235;
  BOOL         members[VTSS_PORT_ARRAY_SIZE];

  rc = vtss_vlan_port_members_get(inst, vid, members);
  if (rc) return rc;

  // Set port 1 (UNI) and port 2 (NNI to be member of VLAN 'vid'.
  members[0]=members[1]=TRUE;

  rc = vtss_vlan_port_members_set(inst, vid, members);
  return rc;
}


void print_policer(const vtss_evc_id_t     evc_id,
                   const vtss_port_no_t    port_no,
                   const vtss_cosid_t      cosid)
{
  static const char* tf[2]={"false", "true"};
  vtss_rc rc;
  vtss_evc_policer_conf_t conf;

  rc = vtss_evc_cosid_policer_conf_get(inst, evc_id, port_no, cosid, &conf);

  printf("type=%d\n", conf.type);
  printf("enable=%s\n", tf[conf.enable]);
  printf("cm=%s\n", tf[conf.cm]);
  printf("cf=%s\n", tf[conf.cf]);
  printf("line_rate=%s\n", tf[conf.line_rate]);
  printf("cir=%dkbps\n", conf.cir);
  printf("cbs=%dbytes\n", conf.cbs);
  printf("eir=%dkpgs\n", conf.eir);
  printf("ebs=%dbytes\n", conf.ebs);
}

//extern u32 torben;
//extern u32 torben_level;
//extern u64 torben_rate;

vtss_rc create_policer()
{
  vtss_rc rc;

  vtss_port_no_t    port_no = uni_port;
  vtss_cosid_t      cosid=0;
  vtss_evc_policer_conf_t conf;

  print_policer(evc_id,port_no, cosid);

  for (cosid=0; cosid<VTSS_COSIDS; ++cosid) {
    rc = vtss_evc_cosid_policer_conf_get(inst, evc_id, port_no, cosid, &conf);

    conf.type = VTSS_POLICER_TYPE_MEF;
    conf.enable = TRUE;
    conf.cm = TRUE; // Colour Mode, i.e., colour aware
    conf.cf = FALSE;
    conf.line_rate = FALSE;
    conf.cir = 1000*(VTSS_COSIDS-cosid);  // = 1Mbps * (cosid+1)
    conf.cbs = 4096;
    conf.eir = 2000*(VTSS_COSIDS-cosid);  // = 2Mbps * (cosid+1)
    conf.ebs = 4096;

    rc = vtss_evc_cosid_policer_conf_set(inst, evc_id, port_no, cosid, &conf);
    //    printf("cosid=%d, torben=%d rate=%llu level=%u\n", cosid, torben, torben_rate, torben_level);

  }

  for (cosid=0; cosid<VTSS_COSIDS; ++cosid) {
    printf("cosid=%d\n", cosid);
    print_policer(evc_id,port_no, cosid);
  }

  return rc;
}

vtss_rc create_encap()
{
  vtss_rc rc;

  vtss_evc_encap_id_t    id;
  vtss_evc_encap_conf_t  conf;

  id = 1;

  rc = vtss_evc_encap_conf_get(inst, id, &conf);
  if (rc) return rc;


  conf.vid = 2;


  rc = vtss_evc_encap_conf_set(inst, id, &conf);

  return rc;
}

vtss_rc create_ingress_map()
{
  vtss_rc rc;
  vtss_qos_ingress_map_key_t key;
  vtss_qos_ingress_map_t     map;
  int i_pcp, i_dei;

  key = VTSS_QOS_INGRESS_MAP_KEY_PCP_DEI;

  rc = vtss_qos_ingress_map_init(inst, key, &map);
  if (rc) return rc;

  // (1) --- Setup the mapping for id=1. Only the dei bit is affected.
  map.id = ingress_map_id;
  map.action.dei = TRUE; // All other fields will not be used.

#if 1
  // --- The cos is actually the qos or Classified QoS. You can enable this code
  //     if you want to see the effect.
  //     Below the mapping is set so that cos ( or QoS) is set to the PCP of the
  //     frame. So if this table is used for a given frame, then QoS class of the
  //     frame will be as the PCP of the frame specify - no matter what was
  //     configured on the port with vtss_qos_port_conf_set().
  //
  map.action.cos = TRUE;
#endif

  map.action.cosid = TRUE;

  for (i_pcp=0; i_pcp<8; ++i_pcp)
    for (i_dei=0; i_dei<2; ++i_dei) {
      map.maps.pcp_dei[i_pcp][i_dei].dei = i_dei%1 ? TRUE : FALSE;
      map.maps.pcp_dei[i_pcp][i_dei].cos = 7-i_pcp;
      map.maps.pcp_dei[i_pcp][i_dei].cosid = i_pcp;

      if (i_pcp==0)       map.maps.pcp_dei[i_pcp][i_dei].cosid = 1;
      if (i_pcp==1)       map.maps.pcp_dei[i_pcp][i_dei].cosid = 0;

    }

  // (2) --- Add this ingress mapping
  rc = vtss_qos_ingress_map_add(inst, &map);
  return rc;
}


vtss_rc create_egress_map()
{
  vtss_rc rc;
  vtss_qos_egress_map_key_t key;
  vtss_qos_egress_map_t     map;
  int i_cosid, i_dpl;

  key = VTSS_QOS_EGRESS_MAP_KEY_COSID_DPL;

  rc = vtss_qos_egress_map_init(inst, key, &map);
  if (rc) return rc;

  // (1) --- Setup the mapping for id=1. Only the dei bit is affected.
  map.id = egress_map_id;
  map.action.dei = TRUE; // All other fields will not be used.

  for (i_cosid=0; i_cosid<8; ++i_cosid)
    for (i_dpl=0; i_dpl<4; ++i_dpl) {
      map.maps.cosid_dpl[i_cosid][i_dpl].dei = i_dpl==0 ? FALSE : TRUE; // Map yellow to green and vica verse.
      map.maps.cosid_dpl[i_cosid][i_dpl].pcp = i_cosid;
    }

  // (2) --- Add this egress mapping
  rc = vtss_qos_egress_map_add(inst, &map);
  return rc;
}



vtss_rc create_qos_port(void)
{
  vtss_rc  rc;
  vtss_port_no_t  port_no;
  vtss_qos_port_conf_t  conf;

  port_no = uni_port;

  rc = vtss_qos_port_conf_get(inst, port_no, &conf);
  if (rc) return rc;

  // --- For the UNI port set the tag_class_enable to TRUE.
  //     so that the colour of the packet, i.e. the dei bit
  //     is taken into account for the policer, which
  //     is configure elsewhere in create_policer().
  conf.tag_class_enable = TRUE;

  rc = vtss_qos_port_conf_set(inst, port_no, &conf);

  return rc;
}


void print_counters(vtss_evc_counters_t *c)
{
  printf("rx_green   %10llu frames  %10llu bytes\n", c->rx_green.frames,   c->rx_green.bytes);
  printf("rx_yellow  %10llu frames  %10llu bytes\n", c->rx_yellow.frames,  c->rx_yellow.bytes);
  printf("rx_red     %10llu frames  %10llu bytes\n", c->rx_red.frames,     c->rx_red.bytes);
  printf("rx_discard %10llu frames  %10llu bytes\n", c->rx_discard.frames, c->rx_discard.bytes);
  printf("tx_discard %10llu frames  %10llu bytes\n", c->tx_discard.frames, c->tx_discard.bytes);
  printf("tx_green   %10llu frames  %10llu bytes\n", c->tx_green.frames,   c->tx_green.bytes);
  printf("tx_yellow  %10llu frames  %10llu bytes\n", c->tx_yellow.frames,  c->tx_yellow.bytes);
}


void demo_evc_main(void)
{
  vtss_rc rc;

  demo_printf("nni_port = %d\n", nni_port);
  demo_printf("uni_port = %d\n", uni_port);
  demo_printf("evc_id = %d\n", evc_id);

#ifdef DEMO_EVC2
  demo_printf("evc_id2 = %d\n", evc_id2);
#endif


  create_encap();

  // Create EVC object
  rc = create_evc();
  if (rc) printf("EVC setup failed\n");

#ifdef DEMO_EVC2
  rc = create_evc2();
  if (rc) printf("EVC2 setup failed\n");
#endif

#if 0
  // Configure UNI port to generate appropiated key
  // J2: keys are hardcoded for now.
  rc = config_port_key();
  if (rc) printf("Port setup failed\n");
#endif

  // Insert rule in IS1 for assigning packets to EVC
  rc = create_ece();
  if (rc) printf("ECE setup failed\n");

#ifdef DEMO_EVC2
  rc = create_ece2();
  if (rc) printf("ECE2 setup failed\n");
#endif

  // Configure UNI and NNI ports to be member of VID 1234
  rc =  create_vlan();
  if (rc) printf("VLAN setup failed\n");

#ifdef DEMO_EVC2
  rc =  create_vlan2();
  if (rc) printf("VLAN2 setup failed\n");
#endif

  // Configure a police
  rc = create_policer();
  if (rc) printf("Policer setup failed\n");

#if 1
  rc = create_ingress_map();
  if (rc) printf("Ingress maps failed\n");

  rc = create_egress_map();
  if (rc) printf("Egress maps failed\n");
#endif

  rc = create_qos_port();
  if (rc) printf("Ingress port qos\n");
}


// ---Debug
extern "C" {
void demo_vtss_ece_add(const vtss_inst_t   inst,
                     const vtss_ece_id_t ece_id,
                       const vtss_ece_t    *const ece);
}

void demo_vtss_ece_add(const vtss_inst_t   inst,
                     const vtss_ece_id_t ece_id,
                     const vtss_ece_t    *const ece)
{
  static const char* port_type[2]={"NONE","ROOT"};
  static const char* ece_type[]={"VTSS_ECE_TYPE_ANY",
#if defined(VTSS_ARCH_SERVAL)
                           "VTSS_ECE_TYPE_ETYPE",
                           "VTSS_ECE_TYPE_LLC",
                           "VTSS_ECE_TYPE_SNAP",
#endif /* VTSS_ARCH_SERVAL */
                           "VTSS_ECE_TYPE_IPV4",
                           "VTSS_ECE_TYPE_IPV6"};



  int i;

  printf("ece_id=%u\n", ece_id);
  printf("ece=(id=%u ,key=[port_list=[", ece->id);
  for (i=0; i<sizeof(ece->key.port_list)/sizeof(ece->key.port_list[0]); ++i) {
    printf("%d:%s,", i, port_type[ece->key.port_list[i]]);
  }

  printf("], type=%s", ece_type[ece->key.type]);
  printf("]\n");

} 



//
// For EVC given by 'evc_id', clear all counters for all cos IDs on port 'port_no'.
//
void demo_evc_main_ece_clear(vtss_port_no_t port_no)
{
  vtss_rc rc;
  vtss_cosid_t   cosid;

  for (cosid=0; cosid<8; ++cosid) {
    rc = vtss_evc_cosid_counters_clear(inst, evc_id, port_no, cosid);
    if (rc) printf("Error: EVC counters for (evc,port,cosid)=(%d,%d,%d) clear\n", evc_id, port_no, cosid);
  }
}



//
// For EVC given by 'evc_id', dump all counters for all cos IDs on port 'port_no'.
//
void demo_evc_main_ece_statistics(vtss_port_no_t port_no)
{
  vtss_rc  rc;
  vtss_evc_counters_t  counters;
  vtss_cosid_t   cosid;

  for (cosid=0; cosid<8; ++cosid) {
    rc = vtss_evc_cosid_counters_get(inst, evc_id, port_no, cosid, &counters);
    if (rc) {
      printf("Error: EVC counters for (evc,port,cosid)=(%d,%d,%d)\n", evc_id, port_no, cosid);
    } else {
      printf("EVC counters for (evc,port,cosid)=(%d,%d,%d)\n", evc_id, port_no, cosid);
      print_counters(&counters);
    }
  }
}

#else

void demo_evc_main(void)
{
  printf("VTSS_FEATURE_EVC_PORT_EVC is not defined\n");
}

void demo_evc_main_ece_clear(vtss_port_no_t port_no)
{
  printf("VTSS_FEATURE_EVC_PORT_EVC is not defined\n");
}

void demo_evc_main_ece_statistics(vtss_port_no_t port_no)
{
  printf("VTSS_FEATURE_EVC_PORT_EVC is not defined\n");
}

#endif
