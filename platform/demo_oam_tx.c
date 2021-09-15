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

#include "demo.h"
#include "demo_api.h"
#include "demo_oam.h"
#include "demo_util.h"
#include <packet_api.h>

#if defined(VTSS_FEATURE_OAM)


// Parameters set by demo_oam_tx() that specify:
// 1)  which OAM frame that shall be transmitted by the CPU (ft)
// 2)  the ISDX 'tx_isdx' of the injected frame. This makes the
//     frame hit the apropiate rule in the chip that point at the VOE
//     in question, so that VOE can update statistics in the frame
//     before it is transmitted.
// 3)  whether the MEP is an Up- or DownMEP. This is necessary to
//     know since the injection point in the switch is different
//     for the two cases
//
static int   ft;
static int   tx_isdx;
static BOOL  upmep;

static int uni_port = DEMO_OAM_UNI_PORT;
static int nni_port = DEMO_OAM_NNI_PORT;


static vtss_handle_t demo_oam_tx_thread_handle;
static vtss_thread_t   demo_oam_tx_thread_state;
static char         demo_oam_tx_thread_stack[THREAD_DEFAULT_STACK_SIZE];
static vtss_flag_t   demo_oam_tx_thread_flag;

static void* demo_tx_done_context = (void*)0xBEEF;
static void  demo_tx_done(void *context, packet_tx_done_props_t *props)
{
  return;
}



// When this function is called, then a actual OAM frame is transmitted
//
static vtss_rc demo_oam_tx_generic(u8* frame, size_t size, vtss_packet_oam_type_t type, size_t pdu_offset)
{
  vtss_rc rc;
  packet_tx_props_t tx_props;
  u8* buffer;

  static int prio = 0;

  // (1) --- Allocate TX buffer, and put the frame into it.
  buffer = packet_tx_alloc(size);
  memcpy(buffer, frame, size);

  // (2) --- Construct metadata for the frame
  packet_tx_props_init(&tx_props);
  
  tx_props.packet_info.modid              = VTSS_MODULE_ID_DEMO;
  tx_props.packet_info.frm[0]             = buffer;
  tx_props.packet_info.len[0]             = size;
  tx_props.packet_info.tx_done_cb         = demo_tx_done;
  tx_props.packet_info.tx_done_cb_context = demo_tx_done_context;

  tx_props.tx_info.oam_type               = type;
  tx_props.tx_info.dst_port_mask          = (1<<nni_port); // In this example we have only one NNI port
  tx_props.tx_info.switch_frm             = upmep ? TRUE : FALSE; // Note: UpMEP OAM frames must be switched
  tx_props.tx_info.cos                    = prio;
  tx_props.tx_info.cosid                  = prio;
  tx_props.tx_info.dp                     = 0;
  tx_props.tx_info.tag.pcp                = prio;
  tx_props.tx_info.isdx                   = tx_isdx; // Transmit ISDX

  // For DownMEP the 'masquerade_port' must be set to NONE. But for
  // UpMEP the residence port, i.e. the 'uni_port' in this example must be specified.
  //
  tx_props.tx_info.masquerade_port        = upmep ? uni_port : VTSS_PORT_NO_NONE;

  tx_props.tx_info.tag.vid                = 1234;

  // For a DownMEP:
  //  When the CPU shall inject a message PDU, then the pipeline point
  //  shall be set to VTSS_PACKET_PIPELINE_PT_REW_IN_VOE. 
  // For a UpMEP: (VTSS_PACKET_PIPELINE_PT_ANA_OU_VOE)
  //
#ifdef VTSS_FEATURE_PACKET_PIPELINE_PT   // Apply to Jaguar2.

  tx_props.tx_info.pipeline_pt = upmep ? VTSS_PACKET_PIPELINE_PT_ANA_OU_VOE : 
                                         VTSS_PACKET_PIPELINE_PT_REW_IN_VOE;

#endif

  // --- First byte after OAM etype 0x8902 in the frame. In this example we
  //     have 2 MAC addresses (2*6), 2 VLAN tags (2*4), and then the 0x8902 (2).
  tx_props.tx_info.pdu_offset        = pdu_offset;


  // (3) --- Transmit frame.
  rc = packet_tx(&tx_props);
  if (rc) return rc;

  // --- If you want to loop through all priorities then enable this code.
  //     E.g. for CCM-LM, one must inject CCM frames on a given priority
  //     in order to get statistics for that priority.
  //     If the code below is not active, then only statistics for priority=0
  //     will be transmitted.

#if 0 
  ++prio;
  if (prio==8) prio=0;
#endif

  return rc;
}

// --- The 'ft' is then index into frame_list[] so that the requested frame is sent.
//

static vtss_rc tx(void)
{
  struct frame_list *p = &frame_list[upmep ? 1 : 0][ft];

  return demo_oam_tx_generic(p->frame, p->size, p->type, p->pdu_offset);
}


// --- This thread will call demo_oam_tx() once every second when signaled to.
//
static void demo_oam_tx_thread(vtss_addrword_t data)
{
  vtss_rc rc;
  vtss_flag_value_t flags;

  for (;;) {

    ft = 0;

    // --- Wait here until we get the signal DEMO_FLAG_OAM_TX, which is an instruction to start
    do {
      flags = vtss_flag_wait(&demo_oam_tx_thread_flag, 0xfffffff, VTSS_FLAG_WAITMODE_OR_CLR);
    } while ( !( flags & DEMO_FLAG_OAM_TX) );
    
    printf("Transmit OAM frames\n");

    // --- Now we are running
    
    do {
      // Transmit an OAM frame with 1 sec interval, and turn off when signaled again.
      rc = tx();
      flags = vtss_flag_timed_wait(&demo_oam_tx_thread_flag, 0xfffffff, VTSS_FLAG_WAITMODE_OR_CLR, vtss_current_time() + 100);
    } while ( !( flags & DEMO_FLAG_OAM_TX) && !rc);

    printf("\nStop transmitting OAM frames\n");
  }
}


// --- This function is used to turn the OAM transmission on and off
//     with e.g. the command:
//     (config)# demo oam down ccm-lm  
//
void demo_oam_tx(int type, int tx_isdx_, BOOL upmep_)
{
  struct frame_list *p;

  if (type>=0 && type<sizeof(frame_list[0])/sizeof(frame_list[0][0])) {

    p = &frame_list[upmep_ ? 1 : 0][type];

    if (p->frame) {
      if (!ft) { ft = type; tx_isdx = tx_isdx_; upmep = upmep_;}
      vtss_flag_setbits(&demo_oam_tx_thread_flag, DEMO_FLAG_OAM_TX);
    } else {
      printf("OAM %s is not implemented ft=%d upmep=%s\n",  p->name, type, upmep_ ? "yes" : "no");
    }

  } else {
    printf("Internal error\n");
  }
}


// --- This function is called from demo.c::demo_init() and create a thread
//     and other mechanisms needed in order to have the CPU transmit OAM
//     frames on a perodic basis.
//
vtss_rc demo_oam_tx_init(vtss_init_data_t *data)
{
    switch (data->cmd) {

    case INIT_CMD_INIT:

        // --- Create a flag to indicate whether the OAM frame
        //     in question should be generated periodicly.
        vtss_flag_init(&demo_oam_tx_thread_flag);

        // --- Create a thread to do this periodic transmission
        vtss_thread_create(VTSS_THREAD_PRIO_HIGH,
                          demo_oam_tx_thread,
                          0,
                          "demo OAM TX thread",
                          demo_oam_tx_thread_stack,
                          sizeof(demo_oam_tx_thread_stack),
                          &demo_oam_tx_thread_handle,
                          &demo_oam_tx_thread_state);

        break;

    case INIT_CMD_START:
        /* vtss_thread_resume(demo_oam_tx_thread_handle); */
        break;

    default:
      // All other cases we do not care about.
      // See demo.c for a complete list.
      break;
    }

    return VTSS_RC_OK;
}

#else

// VTSS_FEATURE_OAM is not defined

vtss_rc demo_oam_tx_init(vtss_init_data_t *data)
{
  return VTSS_RC_OK;
}
 
void demo_oam_tx(int type, int isdx)
{
  printf("Not supported since VTSS_FEATURE_OAM is not defined\n");
}

#endif


#if 0
void direct_to_api(void)
{
  vtss_rc rc;
  vtss_packet_tx_info_t tx_info;
  vtss_packet_tx_ifh_t  tx_ifh;

  rc = vtss_packet_tx_info_init(inst, &tx_info);
  
  tx_info.isdx = tx_isdx;
  tx_info.switch_frm = TRUE;
  tx_info.pdu_offset = 14;
  tx_info.pipeline_pt = ANA_IN_VOE;
  tx_info.masquerade_port = uni_port;
  tx_info.oam_type = VTSS_PACKET_OAM_TYPE_CCM;

  rc = vtss_packet_tx_hdr_compile(inst, &tx_info, &tx_ifh);

  rc = vtss_packet_tx_frame(inst, &tx_ifh, frame, length);
 
}
#endif
