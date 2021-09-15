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

mesa_rc create_acl(void)
{
  mesa_rc rc;
  mesa_inst_t      inst = NULL;
  mesa_ace_type_t  type = MESA_ACE_TYPE_ETYPE;
  mesa_ace_t       ace;

  rc = mesa_ace_init(inst, type, &ace);
  if (rc) return rc;

  ace.id = 1;

#if defined(VTSS_ARCH_LUTON26) || defined(VTSS_ARCH_SERVAL)
#if defined(VTSS_FEATURE_QOS_POLICER_DLB)
  ace.action.evc_policer_id = 15;
  ace.action.evc_police = TRUE;
#endif
#endif


#if 0
  ace.isdx_enable =  FALSE;
  ace.isdx_disable = TRUE;
#endif

#ifdef VTSS_FEATURE_ACL_V2
  ace.port_list[0] = TRUE;
#endif
  
#if 1
  ace.vlan.vid.value=1234;//512;
  ace.vlan.vid.mask=0xfff;

  ace.type = MESA_ACE_TYPE_ETYPE;
  ace.frame.etype.etype.value[0] = 0x89;
  ace.frame.etype.etype.value[1] = 0x02;
  ace.frame.etype.etype.mask[0]  = 0xff;
  ace.frame.etype.etype.mask[1]  = 0xff;

#endif
  rc = mesa_ace_add(inst, MESA_ACE_ID_LAST, &ace);
  return rc;

}

extern "C" {
  extern void create_acl_main(void);
}

void create_acl_main(void)
{
  mesa_rc rc;

  printf("This is Serval-2\n");
  rc = create_acl();
  if (rc) printf("Demo: ACL create failed rc=%d\n", rc);
}
