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

static vtss_inst_t  inst=NULL;


void demo_qce_create()
{

  vtss_rc rc;

  vtss_qce_type_t  type = VTSS_QCE_TYPE_ETYPE;
  vtss_qce_t       qce;
  
  vtss_qcl_id_t  qcl_id = 1;
  vtss_qce_id_t  qce_id = VTSS_QCE_ID_LAST;

  rc = vtss_qce_init(inst, type, &qce);

  qce.id=1;
  qce.key.port_list[0] = TRUE;
  qce.key.tag.vid.type =  VTSS_VCAP_VR_TYPE_VALUE_MASK;
  qce.key.tag.vid.vr.v.value = 123;
  qce.key.tag.vid.vr.v.mask  = 0xfff;


  qce.action.pcp_dei_enable = TRUE;
  qce.action.pcp = 0x123;

  rc = vtss_qce_add(inst, qcl_id, qce_id, & qce);

}

static char* key_type_name[] = {
  "VTSS_VCAP_KEY_TYPE_NORMAL",
  "VTSS_VCAP_KEY_TYPE_DOUBLE_TAG",
  "VTSS_VCAP_KEY_TYPE_IP_ADDR",
  "VTSS_VCAP_KEY_TYPE_MAC_IP_ADDR"
};



void demo_qce_config_key()
{
  vtss_rc rc;
  vtss_port_no_t  port_no = 0;
  vtss_qos_port_conf_t  qce_conf;

  rc = vtss_qos_port_conf_get(inst, port_no, &qce_conf);


#if 0
  typedef enum {
    VTSS_VCAP_KEY_TYPE_NORMAL,     /**< Half key, SIP only */
    VTSS_VCAP_KEY_TYPE_DOUBLE_TAG, /**< Quarter key, two tags */
    VTSS_VCAP_KEY_TYPE_IP_ADDR,    /**< Half key, SIP and DIP */
    VTSS_VCAP_KEY_TYPE_MAC_IP_ADDR /**< Full key, MAC and IP addresses */
  } vtss_vcap_key_type_t;
#endif


  if (qce_conf.key_type>=VTSS_VCAP_KEY_TYPE_NORMAL || qce_conf.key_type<=VTSS_VCAP_KEY_TYPE_MAC_IP_ADDR) {
    printf("key_type = %s\n", key_type_name[qce_conf.key_type]);
  } else {
    printf("key_type = %d\n", qce_conf.key_type);
  }

  qce_conf.key_type = VTSS_VCAP_KEY_TYPE_IP_ADDR;

  rc = vtss_qos_port_conf_set(inst, port_no, &qce_conf);

}
