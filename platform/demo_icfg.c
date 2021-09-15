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

#ifdef VTSS_SW_OPTION_ICFG
#include "icfg_api.h"


// --- This example on ICFG illustrates how the running-config is generated.
//     The running-config is show by means of the ICLI command:
//     # show running-config
//
//     (Below we'll use the term 'mode', which is as defined in AN-1104 table 6)
//
//     In the example below the function demo_icfg_query_func_config() is called
//     when output for the (config) mode is generated.
//
//     And the demo_icfg_query_func_per_interface() is called in the (config-if)
//     mode for each possible interface.
//
//     The system figurs out for which mode the functions below shall be
//     called by means of there registration ID. For the first the registration ID
//     it is VTSS_ICFG_DEMO and for the second it is VTSS_ICFG_DEMO_INTERFACE_CONFIG.
//     These numbers must be defined in vtss_appl/icfg/icfg_api.h.
//
//     (If you are using tags in your editor then search for vtss_icfg_ordering_t
//      in order to get to the right location fast.)
//
//     Since VTSS_ICFG_DEMO come before VTSS_ICFG_INTERFACE_ETHERNET_BEGIN, it apply 
//     to the (config) mode. More precisly: since it is not between an XXX_BEGIN and XXX_END.
//
//     And since VTSS_ICFG_DEMO_INTERFACE_CONFIG is between 
//     VTSS_ICFG_INTERFACE_ETHERNET_BEGIN and VTSS_ICFG_INTERFACE_ETHERNET_END
//     it apply to the (config-if) mode.
//
// --- Exercise 1: Create VTSS_ICFG_DEMO and VTSS_ICFG_DEMO_INTERFACE_CONFIG as described
//                 above in icfg_api.h.
//
// --- Exercise 2: Add an icfg, similar to the ones below, that generate some text, e.g.,
//                 "vitesse rules" in some of the other submodes, e.g., vlan config.
//                 Look in the type of req and figure out for which vlan the call apply.
//                 You can put this number in the response so that you generate
//                 "vitesse rules <number>".

static vtss_rc demo_icfg_query_func_vlanconfig(const vtss_icfg_query_request_t *req,
					       vtss_icfg_query_result_t *result)
{
  vtss_rc rc;

  rc = vtss_icfg_printf(result, "vitesse rules %d\n", req->instance_id.vlan);

  return rc;
}

static vtss_rc demo_icfg_query_func_config(const vtss_icfg_query_request_t *req,
					   vtss_icfg_query_result_t *result)
{
  vtss_rc rc;

  rc = vtss_icfg_printf(result, "demo\n");

  return rc;
}


static vtss_rc demo_icfg_query_func_per_interface(const vtss_icfg_query_request_t *req,
						  vtss_icfg_query_result_t *result)
{
    vtss_rc rc = VTSS_RC_OK;
    BOOL enable;

    const icli_switch_port_range_t *plist = &req->instance_id.port;

    u16 isid = plist->isid;

    u16 portBegin = plist->begin_iport;
    u16 portEnd = portBegin + plist->port_cnt;
    u16 iport;

    for (iport = portBegin; iport < portEnd; ++iport) {

      // The value of enable in this example reflect whether the icli
      // command:
      // (config-if)# demo
      // or
      // (config-if)# no demo
      // shall be given. If the no demo is considered default, then it
      // shall not be outputted unless req->all_defaults is true.

      // Just an example
      enable = (iport==portBegin ? TRUE : FALSE);

      
        if (enable  || req->all_defaults) {
	  // Note the indenting space. This is added in order to make the output
	  // of show running-config look nice. Run show running-config and you will
	  // get the idea.
            rc = vtss_icfg_printf(result, " %sdemoif\n", enable ? "" : "no ");
        }

    }

    return rc;
}




//--- vtss_icfg_ordering_t
void demo_register_icfg(void)
{
  (void)vtss_icfg_query_register(VTSS_ICFG_DEMO,
				 "DEMO",
				 demo_icfg_query_func_config);


  (void)vtss_icfg_query_register(VTSS_ICFG_DEMO_INTERFACE_CONFIG,
				 "DEMO",
				 demo_icfg_query_func_per_interface);

  (void)vtss_icfg_query_register(VTSS_ICFG_DEMO_INTERFACE_VLANCONFIG,
				 "DEMO",
				 demo_icfg_query_func_vlanconfig);

}

#else

void demo_register_icfg(void)
{

}

#endif
