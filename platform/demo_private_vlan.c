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

#include "vtss_api.h"
#include "demo_api.h"

/*
 1) The functions vtss_pvlan_port_members_<get|set>(inst, pvlan, members) will
    get and set which ports (in members[]) are in private vlan 'pvlan'.
    The 'pvlan' is an ID.
    By default all ports are members in private vlan pvlan=0. If two ports
    are menbers of a certain pvlan, then frames can be forwarded between them.
    Therefore all ports are members in pvlan=0, which esentailly means, that
    private vlan is disabled.
    
    Example: We can remove port 4 from pvlan 0 with
    
      pvlan = 0;
      port4 = 4-1;
    
      vtss_pvlan_port_members_get(inst, pvlan, members)
      members[port4]=false;
      vtss_pvlan_port_members_set(inst, pvlan, members)
    
    so now no one can forward to port 4, and port 4 can not forward
    to anyone.
    
    With
      pvlan = 1;
      port3 = 3-1;
      port4 = 4-1;
    
      vtss_pvlan_port_members_get(inst, pvlan, members)
      members[port3] = true;
      members[port4] = true;
      vtss_pvlan_port_members_set(inst, pvlan, members)
    
     we have inserted port 3 and 4 in pvlan 1, which was empty from the start,
     i.e., no oter ports are in pvlan=1. So now port 3 and 4 can talk together.
     Port 3 can talk to anyone, since pvlan=0 allow port 3 to talk to all other ports.
     Port 4 can on the other hand only talk to port 3 via pvlan=1.
    
     In this sense this private vlan is bidirectional.


 2) The functions vtss_apvlan_port_members_<get|set>(inst, port_no, member) work a little
    different. Here we configure that port 'port_no' are allowed to send frames to
    list 'members'. So if you want to prevent port 3 from sending frames to port 4,
    you can do the following:
    
      port3 = 3-1;
      port4 = 4-1;
    
      vtss_apvlan_port_members_get(inst, port3, members)
      members[port4] = false;
      vtss_pvlan_port_members_set(inst, port3, members)

    so we take port 4 out of the list of ports, that port 3 that forward to.

*/

const vtss_inst_t      inst=0;


static vtss_rc demo_pvlan2(vtss_pvlan_no_t  pvlan_no, vtss_pvlan_no_t  pvlan_no2)
{
  vtss_rc  rc;
  BOOL  member[VTSS_PORT_ARRAY_SIZE];
  int  i;
  
  rc = vtss_pvlan_port_members_get(inst, pvlan_no, member);
  if (rc) return rc;
    
  printf("vtss_pvlan_port_members_get(, %d, ", pvlan_no);
  for (i=0; i<VTSS_PORT_ARRAY_SIZE; ++i)
    printf("%d", (int)member[i]);
  printf(")\n");

  if (pvlan_no2<VTSS_PORT_ARRAY_SIZE)
    member[pvlan_no2] = !member[pvlan_no2];

  rc = vtss_pvlan_port_members_set(inst, pvlan_no, member);
  return rc;
}



void demo_pvlan(vtss_pvlan_no_t  pvlan_no, vtss_pvlan_no_t  pvlan_no2)
{
  if (demo_pvlan2(pvlan_no, pvlan_no2))
    printf("Error: demo_pvlan2\n");
}

static vtss_rc demo_apvlan2(vtss_port_no_t port_no, vtss_port_no_t port_no2)
{
  vtss_rc rc;
  BOOL             member[VTSS_PORT_ARRAY_SIZE];
  int i;
  
  rc = vtss_apvlan_port_members_get(inst, port_no, member);
  if (rc) return rc;
    
  printf("vtss_pvlan_port_members_get(, %d, ", port_no);
  for (i=0; i<VTSS_PORT_ARRAY_SIZE; ++i)
    printf("%d", (int)member[i]);
  printf(")\n");

  if (port_no2<VTSS_PORT_ARRAY_SIZE)
    member[port_no2] = !member[port_no2];

  rc = vtss_apvlan_port_members_set(inst, port_no, member);
  return rc;
}



void demo_apvlan(vtss_port_no_t  port_no, vtss_port_no_t  port_no2)
{
  if (demo_apvlan2(port_no, port_no2))
    printf("Error: demo_apvlan2\n");
}


#if 0
vtss_rc vtss_ace_init(const vtss_inst_t      inst,
                      const vtss_ace_type_t  type,
                      vtss_ace_t             *const ace);


/**
 * \brief Add/modify ACE.
 *
 * \param inst [IN]         Target instance reference.
 * \param ace_id_next [IN]  ACE ID of next entry.
 *                          The ACE will be added before the entry with this ID.
 *                          VTSS_ACE_ID_LAST is reserved for inserting last.
 * \param ace [IN]          ACE structure.
 *
 * \return Return code.
 **/
vtss_rc vtss_ace_add(const vtss_inst_t    inst,
                     const vtss_ace_id_t  ace_id_next,
                     const vtss_ace_t     *const ace);
#endif

extern "C"
void print_ace(const vtss_ace_t     *const ace)
{
  int i;
  static const char* name[]={
    "VTSS_ACE_TYPE_ANY",
    "VTSS_ACE_TYPE_ETYPE",
    "VTSS_ACE_TYPE_LLC",
    "VTSS_ACE_TYPE_SNAP",
    "VTSS_ACE_TYPE_ARP",
    "VTSS_ACE_TYPE_IPV4",
    "VTSS_ACE_TYPE_IPV6"
  };
  
  printf("id=%d\n", ace->id);

#if defined(VTSS_ARCH_SERVAL)
  printf("lookup=%d\n", (int)(ace->lookup));
  printf("isdx_enable=%d\n",  (int)(ace->isdx_enable) );
  printf("isdx_disable=%d\n", (int)(ace->isdx_disable) );
#endif /* VTSS_ARCH_SERVAL */

#if defined(VTSS_FEATURE_ACL_V2)
  printf("port_list=");
  for (i=0; i<VTSS_PORT_ARRAY_SIZE; ++i)
    printf("%d", (int)(ace->port_list[i]));
  printf("\n");
#endif /* VTSS_FEATURE_ACL_V2 */

  printf("policy=%d\n",(int)(ace->policy.mask) );
  printf("type=%s\n", name[ace->type]);

}

#if 0
typedef struct
{
    vtss_ace_id_t        id;           /**< ACE ID, must be different from VTSS_ACE_ID_LAST */
#if defined(VTSS_ARCH_SERVAL)
    u8                   lookup;       /**< Lookup, any non-zero value means second lookup */
    BOOL                 isdx_enable;  /**< Use VID value for ISDX value */
    BOOL                 isdx_disable; /**< Match only frames with ISDX zero */
#endif /* VTSS_ARCH_SERVAL */
#if defined(VTSS_FEATURE_ACL_V2)
    BOOL                 port_list[VTSS_PORT_ARRAY_SIZE]; /**< Port list */ 
#endif /* VTSS_FEATURE_ACL_V2 */
    vtss_ace_u8_t        policy;       /**< Policy number */
    vtss_ace_type_t      type;         /**< ACE frame type */
    vtss_acl_action_t    action;       /**< ACE action */
                                   
    vtss_ace_bit_t       dmac_mc;      /**< Multicast DMAC */
    vtss_ace_bit_t       dmac_bc;      /**< Broadcast DMAC */

    vtss_ace_vlan_t      vlan;         /**< VLAN Tag */

    union
    {
        /* VTSS_ACE_TYPE_ANY: No specific fields */
        vtss_ace_frame_etype_t etype;  /**< VTSS_ACE_TYPE_ETYPE */
        vtss_ace_frame_llc_t   llc;    /**< VTSS_ACE_TYPE_LLC */
        vtss_ace_frame_snap_t  snap;   /**< VTSS_ACE_TYPE_SNAP */
        vtss_ace_frame_arp_t   arp;    /**< VTSS_ACE_TYPE_ARP */
        vtss_ace_frame_ipv4_t  ipv4;   /**< VTSS_ACE_TYPE_IPV4 */
        vtss_ace_frame_ipv6_t  ipv6;   /**< VTSS_ACE_TYPE_IPV6 */
    } frame; /**< Frame type specific data */
} vtss_ace_t;

#endif
