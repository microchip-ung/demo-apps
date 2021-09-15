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

/* Description:

   The code in this file basically is the code from demo_evc_j2.c where comments
   and other not relevant stuff has been removed.
   The setup is bassically an EVC that accept c-tagged frams with VID 23 on all UNI 
   ports. In this setup these UNI ports are port 1 and 2 as listed in 'uni_port_list'.
   The atypically thing with this EVC is, that it does not have an NNI, which one
   would usually have. But that is not really necessary - just normally the case.

   When calling demo_vlan_evc(), the ports listed in 'uni_port_list' will accept
   frames with C-tag 23.

   An EVC is created in create_evc() which has no NNI port, and the UNI ports
   as listed in 'uni_port_list' as configured to ROOT role.
   The EVC VID, which is the tag added when frames are forwarded on an NNI port
   is not really used since no NNI port exist. So it is just set to 23.
   The EVC IVID is the VID the switch used internal. So that is also set to 23
   also. It could be anything - as long as it does not collide with other VIDs.
   Therefore we might just as well use 23. There is no need to make it more
   complicated than necessary.

   After this ECE rules are created for the UNI ports that will match C-tagged
   frames with VID 23. This is done in create_ece().

   Finaly we configure in create_vlan() that frames classified to IVID shall be accepted
   on the uni_port_list ports.

   The function demo_vlan_evc_counters() print all counters related to this setup.

*/



#include <vtss/appl/types.h>
#include <vtss_api.h>

#include "demo_api.h"
#include "demo.h"
#include "demo_trace.h"

#if defined(VTSS_FEATURE_EVC) && defined(VTSS_FEATURE_EVC_PORT_EVC)

static const vtss_inst_t         inst = NULL;
static vtss_evc_id_t             evc_id = 1;
static vtss_qos_egress_map_id_t  egress_map_id = 1;
static vtss_qos_ingress_map_id_t ingress_map_id = 5;

static vtss_port_no_t uni_port_list[] = {1-1,2-1};


static vtss_rc create_evc(vtss_vid_t VID)
{
  int  i;
  vtss_rc          rc;
  vtss_evc_conf_t  evc_conf;
  vtss_evc_port_evc_conf_t evc_port_conf;
  vtss_port_no_t   uni_port;

  bzero(&evc_conf, sizeof(evc_conf));
  evc_conf.learning = TRUE;

  // (1) --- The EVC S-tag is not really used, so we set it to the
  //         same value as the VLAN tag we are configuring.
  //
  evc_conf.network.pb.vid = VID;

  // (2) --- We classify to the same VID as the VLAN tag we are configuring,
  //         since otherwise we will take up unnecessay many vlaues in the VID space.
  //
  evc_conf.network.pb.ivid = VID;

  evc_conf.network.pb.policy_no = VTSS_ACL_POLICY_NO_NONE;

#if 0  
  evc_conf.network.pb.tx_map = egress_map_id;
  evc_conf.network.pb.rx_map = ingress_map_id;
#endif
  
  rc = vtss_evc_add(inst, evc_id, &evc_conf);

  // (3) --- We do not want any NNI 
  //
  /* rc = vtss_evc_port_evc_conf_get(inst, evc_id, nni_port, &evc_port_conf); */
  /* evc_port_conf.role = VTSS_EVC_PORT_ROLE_NNI; */
  /* rc = vtss_evc_port_evc_conf_set(inst, evc_id, nni_port, &evc_port_conf); */

  // (4) -- Configure port 'uni_port' to be an UNI port with Root role
  //
  for (i=0; i<sizeof(uni_port_list)/sizeof(uni_port_list[0]); ++i) {
    uni_port = uni_port_list[i];

    rc = vtss_evc_port_evc_conf_get(inst, evc_id, uni_port, &evc_port_conf);
    evc_port_conf.role = VTSS_EVC_PORT_ROLE_ROOT;
    rc = vtss_evc_port_evc_conf_set(inst, evc_id, uni_port, &evc_port_conf);
  }

  return rc;
}


static vtss_rc create_ece(vtss_vid_t VID)
{
  vtss_rc          rc;
  int i;
  vtss_port_no_t   uni_port;

  vtss_ece_type_t     ece_type = VTSS_ECE_TYPE_ANY;
  vtss_ece_t          ece_conf;
  const vtss_ece_id_t ece_id = 0;

  rc = vtss_ece_init(inst, ece_type, &ece_conf);
  if (rc) return rc;

  ece_conf.id=1;

  // (1) --- Match any c-tagged frame with vid 'VID' on the ports
  //         listed in 'uni_port_list'.
  ece_conf.key.type = VTSS_ECE_TYPE_ANY;

  ece_conf.key.tag.tagged = VTSS_VCAP_BIT_1;
  ece_conf.key.tag.s_tagged = VTSS_VCAP_BIT_0;
  ece_conf.key.tag.vid.vr.v.value = VID;
  ece_conf.key.tag.vid.vr.v.mask = 0xfff;
  
  for (i=0; i<sizeof(uni_port_list)/sizeof(uni_port_list[0]); ++i) {
    uni_port = uni_port_list[i];
    ece_conf.key.port_list[uni_port] = VTSS_ECE_PORT_ROOT;
    printf("ECE: uni_port=%d\n", uni_port);
  }

  ece_conf.action.dir = VTSS_ECE_DIR_BOTH;
  ece_conf.action.evc_id=evc_id;

  rc = vtss_ece_add(inst, ece_id, &ece_conf);

  return rc;
}


static vtss_rc create_vlan(vtss_vid_t   vid)
{
  vtss_rc      rc;
  BOOL         members[VTSS_PORT_ARRAY_SIZE];

  int i;
  vtss_port_no_t   uni_port;
  
  rc = vtss_vlan_port_members_get(inst, vid, members);
  if (rc) return rc;
  
  for (i=0; i<sizeof(uni_port_list)/sizeof(uni_port_list[0]); ++i) {
    uni_port = uni_port_list[i];
    members[uni_port] = TRUE;
  }    

  rc = vtss_vlan_port_members_set(inst, vid, members);
  return rc;
}


// --- Create rule for VLAN 23 on ports listed in 'uni_port_list'
//
void demo_vlan_evc(void)
{
  vtss_rc rc;
  vtss_vid_t VID = 23; 

  rc = create_vlan(VID);

  rc = create_evc(VID);
  rc = create_ece(VID);
}

// --- Get all statistics related to VLAN 23 - or rather EVC 'evc_id'.
//
void demo_vlan_evc_counters(void)
{
  vtss_rc rc;
  int i;
  vtss_port_no_t       uni_port;
  vtss_cosid_t         cosid;
  vtss_evc_counters_t  counters;

  for (i=0; i<sizeof(uni_port_list)/sizeof(uni_port_list[0]); ++i) {
    uni_port = uni_port_list[i];

    printf("Port %d:\n", uni_port+1);
    for (cosid=0; cosid<VTSS_COSIDS; ++cosid) {
      rc = vtss_evc_cosid_counters_get(inst, evc_id, uni_port, cosid, &counters);

      printf("Cosid=%d         frames     bytes\n", (int)cosid);
      printf("rx_green   :  %10.10llu %10.10llu\n", counters.rx_green.frames, counters.rx_green.bytes);
      printf("rx_yellow  :  %10.10llu %10.10llu\n", counters.rx_yellow.frames, counters.rx_yellow.bytes);
      printf("rx_red     :  %10.10llu %10.10llu\n", counters.rx_red.frames, counters.rx_red.bytes);
      printf("rx_discard :  %10.10llu %10.10llu\n", counters.rx_discard.frames, counters.rx_discard.bytes);
      printf("tx_green   :  %10.10llu %10.10llu\n", counters.tx_green.frames, counters.tx_green.bytes);
      printf("tx_yellow  :  %10.10llu %10.10llu\n", counters.tx_yellow.frames, counters.tx_yellow.bytes);
      printf("tx_discard :  %10.10llu %10.10llu\n", counters.tx_discard.frames, counters.tx_discard.bytes);
    }
  }
}


#else
void demo_vlan_evc(void)
{
  printf("Only supported for CEService\n");
}
void demo_vlan_evc_counters(void)
{
  printf("Only supported for CEService\n");
}

#endif
