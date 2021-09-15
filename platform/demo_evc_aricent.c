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


static int nni_port = 2-1;


static vtss_rc create_evc(void)
{
  vtss_rc          rc;
  vtss_inst_t      inst = NULL;
  vtss_evc_id_t    evc_id = 1;
  vtss_evc_conf_t  evc_conf;

  // (1) --- Create an EVC


  // (1.1) --- The evc_conf must be setup. Start by making sure that the config structure is "clean".
  //
  bzero(&evc_conf, sizeof(evc_conf));

  // (1.2) --- The EVC may be associated with a policer. Some special values are defined
  //           in vtss_evc_api.h.
  //           Valid values are: 0,...,VTSS_EVC_POLICERS-1, 
  //                             VTSS_EVC_POLICER_ID_DISCARD,
  //                             VTSS_EVC_POLICER_ID_NONE.
  //
  //           In this example we choose to have no policer associated with the EVC.
  //
  evc_conf.policer_id = VTSS_EVC_POLICER_ID_NONE;

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
  rc = vtss_evc_add(inst, evc_id, &evc_conf);

  return rc;
}

static vtss_rc create_ece()
{
  vtss_rc          rc;

  // (2) --- Create an ECE / UNI
  vtss_ece_type_t     ece_type = VTSS_ECE_TYPE_ANY;
  vtss_ece_t          ece_conf;
  const vtss_ece_id_t ece_id = 0;

  int                 uni_port = 0;
  vtss_inst_t         inst = NULL;

  rc = vtss_ece_init(inst, ece_type, &ece_conf);
  if (rc) return rc;

  ece_conf.id=1;

  if ( !(uni_port<VTSS_PORT_ARRAY_SIZE) ) return VTSS_RC_ERROR; // Error: UNI port is not in the valid range.
  ece_conf.key.port_list[uni_port] = VTSS_ECE_PORT_ROOT;

  ece_conf.key.type = VTSS_ECE_TYPE_LLC;

  // Match C-VLAN = 110
  ece_conf.key.tag.tagged = VTSS_VCAP_BIT_1;
  ece_conf.key.tag.vid.vr.v.value = 110;
  ece_conf.key.tag.vid.vr.v.mask = 0xfff;

  ece_conf.key.lookup = 0;
  ece_conf.action.dir  = VTSS_ECE_DIR_BOTH;
  ece_conf.action.rule = VTSS_ECE_RULE_BOTH;

  ece_conf.action.evc_id=1;
  ece_conf.action.policy_no = 42;

  rc = vtss_ece_add(inst, ece_id, &ece_conf);

  return rc;
}


static vtss_rc create_ACL()
{
  vtss_rc rc;
  vtss_inst_t      inst = NULL;
  vtss_ace_type_t  type = VTSS_ACE_TYPE_ANY;
  vtss_ace_t       ace;
  int i;

  rc = vtss_ace_init(inst, type, &ace);
  if (rc) return rc;

  ace.id = 1;
  ace.lookup = 0; // First lookup, as alternative to second.

  // We do not care on which port the frame might have been received.
  for (i=0; i<VTSS_PORT_ARRAY_SIZE; ++i)
    ace.port_list[i] = TRUE;

  // Dont care if the frame is service frame
  ace.isdx_enable =  FALSE;
  ace.isdx_disable =  FALSE;

  // Match a service frame with policy 42.
  ace.policy.value = 42;
  ace.policy.mask = 0xff;

  rc = vtss_ace_add(inst, VTSS_ACE_ID_LAST, &ace);

  return rc;
}


static vtss_rc config_port_key()
{
  vtss_rc rc;

  vtss_inst_t          inst = NULL;
  vtss_port_no_t       port_no=0;
  vtss_evc_port_conf_t port_conf;

  // (1) --- Read the port configuration since we only want
  //         to change part of it.
  //
  rc = vtss_evc_port_conf_get(inst, port_no, &port_conf);
  if (rc) return rc;

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
  port_conf.key_type = VTSS_VCAP_KEY_TYPE_NORMAL;
  port_conf.dmac_dip = FALSE;

  rc = vtss_evc_port_conf_set(inst, port_no, &port_conf);

  return rc;
}

static vtss_rc create_vlan()
{
  vtss_rc      rc;
  vtss_inst_t  inst = NULL;
  vtss_vid_t   vid = 1234;
  BOOL         members[VTSS_PORT_ARRAY_SIZE];

  rc = vtss_vlan_port_members_get(inst, vid, members);
  if (rc) return rc;

  // Set port 1 (UNI) and port 2 (NNI to be member of VLAN 'vid'.
  members[0]=members[1]=TRUE;

  rc = vtss_vlan_port_members_set(inst, vid, members);
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
  rc = create_ece();
  if (rc) printf("ECE setup failed\n");

  // Insert rule in IS2 that match frames that did match 
  // the IS1 rule insrted above
  rc = create_ACL();
  if (rc) printf("ACL setup failed\n");

  // Configure UNI and NNI ports to be member of VID 1234
  rc =  create_vlan();
  if (rc) printf("VLAN setup failed\n");
}
