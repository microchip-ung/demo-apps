#include "demo_api.h"
#include <vtss_api.h>


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




#if 1 
vtss_rc demo_fdma_api_(void)
{
  vtss_rc rc;
  vtss_fdma_tx_info_t fdma_info;

  vtss_fdma_list_t      list;
  vtss_packet_tx_info_t tx_info;

  rc = vtss_fdma_tx_info_init(inst, &fdma_info);

  rc = vtss_fdma_tx(inst, &list, &fdma_info, &tx_info);


  return rc;
}
#endif


#if 0

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


static void demo_fdma_rx_alloc_cb(u32 sz, vtss_fdma_list_t *const list, u32 mem_flags)
{
  vtss_fdma_list_t *next = list;

  for (next = list; next; next = next_next) {
    next->alloc_ptr = alloc(sz);
  }
}


// This function configure, i.e. call vtss_fdma_cfg() with
// parameters that the MCSS application normally do.
//
vtss_rc demo_fdma_init()
{
  vtss_rc rc;
  vtss_fdma_cfg_t fdma_cfg;
vtss_fdma_cfg_t
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

  // (1.1) --- Callback that is called whenever a RX frames
  //           has been received.
  //
  fdma_cfg.rx_cb = demo_fdma_rx_cb;

  // (2) --- TX related configuration
  //
  fdma_cfg.tx_buf_cnt = 312;

  // (2.1) --- Callback that is called whenever a TX frames
  //           has actually been transmitted.
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


static void  *demo_fdma_pre_tx_cntxt1 = 0xDEAD;
static void  *demo_fdma_pre_tx_cntxt2 = 0xBEEF;
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
#endif


typedef struct tag_vtss_fdma_list {

  u8 *frm_ptr;
  u32 act_len;

  void *alloc_ptr;

  vtss_packet_rx_info_t *rx_info;

  VTSS_OS_TIMESTAMP_TYPE sw_tstamp;

  void *user;

#if defined(VTSS_FEATURE_AFI_FDMA)
    u64 afi_frm_cnt;
    u32 afi_seq_number;
#endif /* defined(VTSS_FEATURE_AFI_FDMA) */
    struct tag_vtss_fdma_list *next;
} vtss_fdma_list_t;
#endif

 extern vtss_fdma_cfg_t cfg_torben;
void demo_dump_fdma_cfg(void)
{

  printf("fdma_cfg.enable = ", cfg_torben.enable);

  // (1) --- RX related configuration
  printf("fdma_cfg.rx_mtu =  %d\n", cfg_torben.rx_mtu);
  printf("fdma_cfg.rx_buf_cnt = %d\n", cfg_torben.rx_buf_cnt);

  printf("fdma_cfg.rx_alloc_cb = %p\n", cfg_torben.rx_alloc_cb);

  printf("fdma_cfg.rx_dont_strip_vlan_tag = %d\n", cfg_torben.rx_dont_strip_vlan_tag);
  printf("fdma_cfg.rx_dont_reinsert_vlan_tag = %d\n", cfg_torben.rx_dont_reinsert_vlan_tag);
  printf("fdma_cfg.rx_allow_vlan_tag_mismatch = %d\n", cfg_torben.rx_allow_vlan_tag_mismatch);
  printf("fdma_cfg.rx_allow_multiple_dcbs = %d\n", cfg_torben.rx_allow_multiple_dcbs);

  printf("fdma_cfg.rx_cb = %p\n", cfg_torben.rx_cb);

  // (2) --- TX related configuration
  printf("fdma_cfg.tx_buf_cnt = %d\n", cfg_torben.tx_buf_cnt);
  printf("fdma_cfg.tx_done_cb = %d\n", cfg_torben.tx_done_cb);

  // (3) --- AFI related configuration
#if 0
#if defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC) || 0
  cfg_torben.afi_buf_cnt = 10;
  cfg_torben.afi_done_cb = afi_done_cb;
#endif
#endif
 
}


#if 0
#if VTSS_OPT_FDMA_VER >= 3
/**
 * \brief FDMA Injection Properties.
 */
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

#endif

