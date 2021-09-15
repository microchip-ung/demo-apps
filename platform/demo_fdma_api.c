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
#include <vtss_api.h>


// Description of this example:
//
// First the function  demo_fdma_init()  is called
// to configure the FDMA functionality.
//
// The demo_fdma_send() show how to send a frame.
//
// 




static const vtss_inst_t  inst=NULL;

static void  *demo_fdma_post_tx_cntxt = (void*)0x23571113; // Just some number


#if 0
// Demo frame to be sent, vlan-tag = PCP=3, DEI=0, VID=1234
//
#define TEST_LENGTH (2*6 + 4 + 2 + 10 + 2)
static u32 test_length = TEST_LENGTH - 2;
static const  u8 test_frame[TEST_LENGTH] = { 0x00, 0x01, 0xc1, 0x00, 0x11, 0x22,  
                                             0x00, 0x01, 0xc1, 0x00, 0x11, 0x33,
                                             0x81, 0x00, 0x64, 0xd2,
                                             0x00, 0x0a,
                                             0,1,2,3,4,5,6,7,8,9,
                                             0,0};
#endif




static vtss_fdma_list_t *keep_list = NULL;


// This function is called when a frames is received.
//
static vtss_fdma_list_t *demo_fdma_rx_cb(void *cntxt, vtss_fdma_list_t *list)
{

#ifdef METHOD_1

  // We'll keep the frame, represented by 'list' after this function return.
  // This is indicated by returning NULL. When we do this we'll have to
  // return the frame resources later by calling vtss_fdma_dcb_release(,list).
  //
  keep_list = list;
  return NULL;
  
#elifdef METHOD_2

  // Take the frame buffer by setting 'alloc_ptr' to NULL.
  // If this method is used, the fdma driver will call
  // rx_alloc_cb() in order for the application to create frame
  // buffers and set alloc_prt accordingly. Therefore when using this
  // method, the rx_alloc_cb must be set.

#elifdef METHOD_3
  
  // If this function does not change anything in 'list' and return list,
  // then the resources are back in the fdma driver.
  return list;
#endif

  return 0;
}


static void demo_fdma_tx_done_cb(void *cntxt, vtss_fdma_list_t *list, vtss_rc rc)
{
  // The cntxt is the value of the 2nd parameter that vtss_fdma_irq_handler() has been
  // called with. In the MSCC application, this is just NULL, 

  // The 'list' is the list of DCBs that we allocated earlier with vtss_fdma_dcb_get() in
  // demo_fdma_send(). Now we got it back, and can therefore identify which frames
  // this call is wrt.

  if (list->user == demo_fdma_post_tx_cntxt) {
    printf("Just after to TX frames\n");
  } else {
    printf("Something else has been sent\n");
  }
}


static void demo_fdma_afi_done_cb(void *cntxt, struct tag_vtss_fdma_list *list, vtss_rc rc)
{
  
}


// If the function below is registered with the FDMA, and if the FDMA calls this
// function, then we are supposed to provide a buffer as the code illustrate.
//
static void demo_fdma_rx_alloc_cb(u32 sz, vtss_fdma_list_t *const list, u32 mem_flags)
{
  vtss_fdma_list_t *next = list;

  for (next = list; next; next = next->next) {
    next->alloc_ptr = alloc(sz);
  }
}


// This function configure, i.e. call vtss_fdma_cfg() with
// parameters that the MCSS application normally do.
//
vtss_rc demo_fdma_init(void)
{
  vtss_rc rc;
  vtss_fdma_cfg_t fdma_cfg;

  bzero(&fdma_cfg, sizeof(fdma_cfg));

  fdma_cfg.enable = TRUE;

  // (1) --- RX related configuration
  //
  fdma_cfg.rx_mtu = 1518 + 4*2;
  fdma_cfg.rx_buf_cnt = 416;

  //         Must be defined if METHOD_2 in demo_fdma_rx_cb()
  //         is used.
  //
  fdma_cfg.rx_alloc_cb = NULL; //demo_fdma_rx_alloc_cb;

  fdma_cfg.rx_dont_strip_vlan_tag     = FALSE;
  fdma_cfg.rx_dont_reinsert_vlan_tag  = FALSE;
  fdma_cfg.rx_allow_vlan_tag_mismatch = FALSE;
  fdma_cfg.rx_allow_multiple_dcbs     = FALSE;

  //         Callback that is called whenever a RX frames
  //         has been received.
  //
  fdma_cfg.rx_cb = demo_fdma_rx_cb;

  // (2) --- TX related configuration
  //
  fdma_cfg.tx_buf_cnt = 312;

  //         Callback that is called whenever a TX frames
  //         has actually been transmitted.
  //
  fdma_cfg.tx_done_cb = demo_fdma_tx_done_cb;

  // (3) --- AFI related configuration
  //
#if defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC)
  fdma_cfg.afi_buf_cnt = 10;
  fdma_cfg.afi_done_cb = demo_fdma_afi_done_cb;
#endif

  // (4) --- Configure this
  rc = vtss_fdma_cfg(inst, &fdma_cfg);

  return rc;
}


static void  *demo_fdma_pre_tx_cntxt1 = (void*)0xDEAD; // Just some value
static void  *demo_fdma_pre_tx_cntxt2 = (void*)0xBEEF; // -||-
static void   demo_fdma_pre_tx_callback(void *ctxt1, void *ctxt2, vtss_fdma_list_t *list)
{
  printf("Just about to TX frames\n");
}


// Example of how to send a frame with FDMA.
//
void demo_fdma_send(void)
{
  vtss_rc rc;

  vtss_fdma_list_t *list; // fdma_dcb
  vtss_fdma_tx_info_t    fdma_info;
  vtss_packet_tx_info_t  tx_info;

  u32 dcb_cnt = 1;
  vtss_fdma_dcb_type_t dcb_type = VTSS_FDMA_DCB_TYPE_INJ;

  
  // (1) --- Start by allocating the necessary DCBs
  //         The 'list' point is pointing at the first DCB, and a list of DCBs is returned, then the
  //         ->next is the link. The number of DBCs in the list is given by 'dcb_cnt'.
  //         So if dnb_cnt=1, then list->next=NULL.
  //
  do {
    rc = vtss_fdma_dcb_get(inst, dcb_cnt, dcb_type, &list);
  } while (rc == VTSS_RC_INCOMPLETE);

  //         Hookup the frame to sent
  //
  list->frm_ptr = frame;
  list->act_len = frame_length;

  //         Set optional user pointer, that will be used
  //         then demo_fdma_tx_done_cb() is called.
  //
  list->user = demo_fdma_post_tx_cntxt;
  

  // (2) --- Init the FDMA info struct. 
  //
  rc = vtss_fdma_tx_info_init(inst, &fdma_info);

  //         We have the option to register a callback function that will
  //         be called just before the FDMA transmit the frame.
  //         if pre_cb not changed, i.e. NULL, then it is just not used.
  //           
  fdma_info.pre_cb_ctxt1  =  demo_fdma_pre_tx_cntxt1;
  fdma_info.pre_cb_ctxt2  =  demo_fdma_pre_tx_cntxt1;
  fdma_info.pre_cb        =  demo_fdma_pre_tx_callback;


  // (3) --- Init the Packet info struct. 
  //
  rc = vtss_packet_tx_info_init(inst, &tx_info);

  
  // (4) --- Schedule the frame for transmission
  //
  rc = vtss_fdma_tx(inst, list, &fdma_info, &tx_info);

  
  // (5) --- Now that frame is in the queue for transmission.
  //         Just before the frames is sent, the demo_fdma_pre_tx_callback()
  //         function is called. And when it has been sent then the demo_fdma_tx_done_cb()
  //         is called with the context variable demo_fdma_post_tx_cntxt. 

  return rc;
}



// Example of how to receive a frame with FDMA.
//
// Here is not receive function as such.
// When a frame is received, the callback function
// demo_fdma_rx_cb() that we registeres in demo_fdma_init()
// is called. See the comment in demo_fdma_rx_cb().
//



// --- The list below is the FDMA function available in VERSION = 3

#if 0
vtss_rc vtss_fdma_cfg(const vtss_inst_t inst, const vtss_fdma_cfg_t *const cfg);

vtss_rc vtss_fdma_dcb_release(const vtss_inst_t      inst,
                                    vtss_fdma_list_t *const list);

vtss_rc vtss_fdma_tx(const vtss_inst_t           inst,
                           vtss_fdma_list_t      *list,
                           vtss_fdma_tx_info_t   *const fdma_info,
                           vtss_packet_tx_info_t *const tx_info);

vtss_rc vtss_fdma_tx_info_init(const vtss_inst_t inst,
                                     vtss_fdma_tx_info_t *const fdma_info);

vtss_rc vtss_fdma_afi_cancel(const vtss_inst_t inst, const u8 *const frm_ptr);

vtss_rc vtss_fdma_afi_frm_cnt(const vtss_inst_t inst, const u8 *const frm_ptr, u64 *const frm_cnt);

vtss_rc vtss_fdma_dcb_get(const vtss_inst_t          inst,
                                u32                  dcb_cnt,
                                vtss_fdma_dcb_type_t dcb_type,
                                vtss_fdma_list_t     **list);

vtss_rc vtss_fdma_throttle_cfg_get(const vtss_inst_t inst, vtss_fdma_throttle_cfg_t *const cfg);

vtss_rc vtss_fdma_throttle_cfg_set(const vtss_inst_t inst, const vtss_fdma_throttle_cfg_t *const cfg);

vtss_rc vtss_fdma_throttle_tick(const vtss_inst_t inst);

vtss_rc vtss_fdma_stats_clr(const vtss_inst_t inst);

vtss_rc vtss_fdma_irq_handler(const vtss_inst_t inst, void *const cntxt);



typedef struct {
    void *pre_cb_ctxt1;
    void *pre_cb_ctxt2;

    void (*pre_cb)(void *ctxt1, void *ctxt2, vtss_fdma_list_t *list);

#if defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC)
    u32 afi_fps;
    vtss_fdma_afi_type_t afi_type;
#endif /* defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC) */

#if defined(VTSS_FEATURE_AFI_FDMA)
    BOOL afi_enable_counting;
    BOOL afi_enable_sequence_numbering;
    u16 afi_sequence_number_offset;
#endif /* defined(VTSS_FEATURE_AFI_FDMA) */

} vtss_fdma_tx_info_t;




typedef struct tag_vtss_fdma_list {
    /**
     * <B>XTR:</B>\n
     *   This points to the first byte of the frame. Set by FDMA driver.\n
     *   For SOF DCBs, this corresponds to the first byte of the DMAC. For non-SOF DCBs
     *   it points to the first byte of the continued frame.
     *
     * <B>INJ/AFI:</B>\n
     *   This points to the first byte of the frame. Set by application.
     *   For SOF DCBs, VTSS_FDMA_HDR_SIZE_BYTES of head room must be available
     *   just before the #frm_ptr.
     *   The FDMA driver may update this internally, so don't rely on its value
     *   after injection.
     */
    u8 *frm_ptr;

    /**
     * <B>XTR:</B>\n
     *   Used internally by the FDMA driver (holds length incl. IFH, frame, and FCS).
     * <B>INJ/AFI:</B>\n
     *   The number of frame bytes to be injected from #frm_ptr for this fragment.
     *   For the SOF DCB, it does not include the size of IFH - only true frame data.
     *   for the EOF DCB, it does not include the size of the FCS.
     *   The FDMA driver may update this internally, so don't rely on its value
     *   after injection.
     */
    u32 act_len;

    /**
     * <B>XTR:</B>\n
     *   Pointer to allocated frame + meta data. Either set by
     *   application during calls to rx_alloc_cb() or by the FDMA
     *   driver itself if memory management is entirely handled
     *   by the FDMA driver.
     * <B>INJ/AFI:</B>\n
     *   Not used.
     */
    void *alloc_ptr;

  /**
     * <B>XTR:</B>\n
     *   Pointer to decoded extraction header. The allocation of this
     *   is taken care of by the FDMA driver. Only valid in SOF DCB.
     * <B>INJ/AFI:</B>\n
     *   Not used.
     */
    vtss_packet_rx_info_t *rx_info;

  /**
     * <B>XTR:</B>\n
     *    Unused. In V3+, it's part of the vtss_packet_rx_info_t structure and is called sw_tstamp.
     * <B>INJ:</B>\n
     *   The FDMA driver code time-stamps the packet when the vtss_fdma_irq_handler()
     *   gets invoked based on an injection interrupt.\n.
     * <B>AFI:</B>\n
     *   Unused.
     * The FDMA driver is agnostic to the time stamp format, and it's up to
     * the platform header (vtss_os.h) to define appropriate types and functions
     * for obtaining the time stamp.
     */
    VTSS_OS_TIMESTAMP_TYPE sw_tstamp;

    /**
     * <B>XTR/INJ/AFI:</B>\n
     *   A pointer to any user data. Set by user and used only by the user. The FDMA code doesn't touch nor uses it.
     */
    void *user;


#if defined(VTSS_FEATURE_AFI_FDMA)
    /**
     * <B>XTR/INJ</B>\n
     *   Not used.
     * <B>AFI</B>\n
     *   Output parameter. Holds the number of frames that was actually transmitted.
     *   Updated regularly, but is only 100% correct once the AFI injection is cancelled
     *   and the tx_done_cb() is called.
     *
     * Validity:
     *   Luton26: Y
     *   Jaguar1: Y
     *   Serval : Y (TBD)
     */
    u64 afi_frm_cnt;

    /**
     * <B>XTR/INJ</B>\n
     *   Not used.
     * <B>AFI</B>\n
     *   Holds the next sequence number to put in a given frame.
     *   Updated repeatedly by S/W when AFI sequence numbering is enabled.
     *
     * Validity:
     *   Luton26: Y
     *   Jaguar1: Y
     *   Serval : Y (TBD)
     */
    u32 afi_seq_number;
#endif /* defined(VTSS_FEATURE_AFI_FDMA) */

  struct tag_vtss_fdma_list *next;
} vtss_fdma_list_t;


#endif

