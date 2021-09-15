/*

 Vitesse Switch Software.

 Copyright (c) 2002-2015 Vitesse Semiconductor Corporation "Vitesse". All
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

/**
 * \file
 * \brief Frame DMA API
 */

#ifndef _VTSS_FDMA_API_H_
#define _VTSS_FDMA_API_H_

#include <vtss/api/types.h>



#if defined(VTSS_FEATURE_FDMA) && VTSS_OPT_FDMA
/* FDMA available and user-enabled. */

// Avoid including vtss_os.h in newer FDMA versions, because it contains
// info to the API, and not info from the API.

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Frame DMA channel number.
 * Channels are numbered in range [0; VTSS_FDMA_CH_CNT[.
 */
typedef i32 vtss_fdma_ch_t; /* Must be a signed integer, or some loops will not work anymore. */

/*
 * Fundamental constants
 */
#if defined(VTSS_ARCH_LUTON26)
  #define VTSS_FDMA_CH_CNT          8 /**< Number of DMA Channels.                             */
  #define VTSS_PHYS_PORT_CNT       26 /**< Number of Port Modules, excluding CPU Port Module.  */
  #define VTSS_FDMA_DCB_SIZE_BYTES 24 /**< Number of bytes in a DCB.                           */
  #define VTSS_FDMA_HDR_SIZE_BYTES 16 /**< Max(XTR_HDR_SZ, INJ_HDR_SZ). Worst-case is INJ (8 for IFH, vtss_fdma_inj_props_t::ptp_action != 0 => +4 for timestamp. \@switch_frm == TRUE => +4 for VLAN tag) */
#endif /* defined(VTSS_ARCH_LUTON26) */

#if defined(VTSS_ARCH_SERVAL)
  #define VTSS_FDMA_CH_CNT          4 /**< Number of DMA Channels. Fixed layout                */
  #define VTSS_PHYS_PORT_CNT       11 /**< Number of Port Modules, excluding CPU Port Module.  */
  #define VTSS_FDMA_DCB_SIZE_BYTES 16 /**< Number of bytes in a DCB.                           */
  #define VTSS_FDMA_HDR_SIZE_BYTES 20 /**< Max(XTR_HDR_SZ, INJ_HDR_SZ). Worst-case is INJ (16 for IFH + 4 for VLAN tag (if \@switch_frm == TRUE) */
#endif /* defined(VTSS_ARCH_SERVAL) */

#if defined(VTSS_ARCH_JAGUAR_2)
  #define VTSS_FDMA_CH_CNT         10 /**< Number of DMA Channels. Fixed layout */
  #define VTSS_FDMA_DCB_SIZE_BYTES 16 /**< Number of bytes in a DCB.            */
  #define VTSS_FDMA_HDR_SIZE_BYTES 32 /**< Max(XTR_HDR_SZ, INJ_HDR_SZ). Worst-case is INJ (28 for IFH + 4 for VLAN tag (if \@switch_frm == TRUE) */
#endif /* defined(VTSS_ARCH_JAGUAR_2) */

/*
 * These do not appear in the Doxygen docs...
 *
 * Unfortunately, there are a few bugs in the FDMA silicon. These are cir-
 * cumvented in S/W, unless the relevant VTSS_FDMA_IGNORE_GNATS_xxxx
 * are defined (The xxxx refers to a number in Vitesse's internal bug-tracking
 * system, GNATS).
 * The main-purpose for having these in the code is to be able to ignore the
 * GNATS to provoke the error during testing. Normally these should *not* be
 * defined when compiling vtss_fdma.c.
 *
 * VTSS_FDMA_IGNORE_GNATS_5299:
 *   When receiving a frame and the frame uses one single DCB, and the DCB's
 *   data area is X bytes, and the frame size is in the interval [X-3; X],
 *   then the DCB's SOF flag is not set by the FDMA silicon. By defining
 *   this, the FDMA driver code will check for the SOF flag and assert that
 *   it is set when expecting a SOF.
 *
 * VTSS_FDMA_IGNORE_GNATS_5355:
 *   This GNATS states that the sizeof(non-EOFs' data area) must be a multiple
 *   of 4. This is relevant to injection only, and means that if a frame is
 *   split across several DCBs, then all but the last DCB's data area must have
 *   a size that is a multiple of 4. Define this if you want to allow user-
 *   code to inject invalid-sized DCBs.
 *
 * VTSS_FDMA_IGNORE_GNATS_5376:
 *   This GNATS states that the minimum size of the SOF DCB's data area must
 *   be sizeof(IFH+CMD)+1 byte. This is relevant to injection only, and means
 *   that if a frame is split across several DCBs, then the first DCB's data
 *   area's size must be at least IFH+CMD+1 byte large. Define this if you
 *   want to allow user-code to inject invalid-sized SOF DCB data areas.
 *
 * VTSS_FDMA_IGNORE_GNATS_5418
 *   When transmitting a frame from the CPU port module using the DO_ANALYZE
 *   flag, then the frame is looped back to the CPU, even if the ANA::SRCMASKS[CPU_PM]
 *   doesn't include the CPU Port Module itself. So to avoid frames from being
 *   looped back to the application, this should *not* be defined. In that
 *   case, the FDMA driver code will check if the frame was transmitted from
 *   the CPU itself (it checks the IFH's source port field), and count and
 *   discard it if so. Define this, if you don't want the driver code to do
 *   this check.
 */

/**
 * \def VTSS_FDMA_MIN_DATA_PER_INJ_SOF_DCB_BYTES
 *   Defines the minimum size in bytes of an injection SOF-DCB's associated
 *   data area.
 *
 * \def VTSS_FDMA_MIN_DATA_PER_XTR_SOF_DCB_BYTES
 *   Defines the minimum size in bytes of an extraction SOF-DCB's associated
 *   data area. Since every DCB can become a SOF DCB, this value also defines
 *   the minimum size that the user must allocate per DCB.
 *
 * \def VTSS_FDMA_MIN_DATA_PER_NON_SOF_DCB_BYTES
 *   Defines the minimum size in bytes of an injection/extraction non-SOF-
 *   DCB's associated data area.
 *
 * \def VTSS_FDMA_MAX_DATA_PER_DCB_BYTES
 *   For both injection and extraction, the maximum number of bytes that
 *   one single DCB's associated data area can refer to. If you need to
 *   inject or extract larger frames, use multiple DCBs.
 *
 * \def VTSS_FDMA_MIN_FRAME_SIZE_BYTES
 *   The minimum allowed frame size (excluding IFH and CMD fields, but including FCS) in bytes.
 *   This is defined for legacy reasons. The FDMA will automatically adjust any frame below
 *   the minimum ethernet size to the minimum ethernet size before transmission.
 *
 * \def VTSS_FDMA_MAX_FRAME_SIZE_BYTES
 *   The maximum allowed total frame size (excluding IFH and CMD fields) in
 *   bytes.
 */
#if defined(VTSS_ARCH_LUTON26) || defined(VTSS_ARCH_SERVAL) || defined(VTSS_ARCH_JAGUAR_2)
  #define VTSS_FDMA_MAX_DATA_PER_DCB_BYTES         16380
#endif /* LUTON26/SERVAL/JAGUAR_2 */

#define VTSS_FDMA_MIN_FRAME_SIZE_BYTES             64

#define VTSS_FDMA_MIN_DATA_PER_NON_SOF_DCB_BYTES    1
#define VTSS_FDMA_MAX_FRAME_SIZE_BYTES          10000

#if !defined(VTSS_OS_DCACHE_LINE_SIZE_BYTES)
#define VTSS_OS_DCACHE_LINE_SIZE_BYTES 32 /**< The number of bytes one DCache-line is made up of. */
#endif

#if !defined(VTSS_OS_COMPILER_ATTRIBUTE_ALIGNED)
#define VTSS_OS_COMPILER_ATTRIBUTE_ALIGNED(x) __attribute ((aligned(x))) /**< In some special cases, it is of utmost importance that a certain variable has a certain memory alignment. Applications for this is e.g. placing variables on cache-line boundaries.  */
#endif

#if VTSS_OS_DCACHE_LINE_SIZE_BYTES <= VTSS_FDMA_DCB_SIZE_BYTES
#error "Internal Error. Rewrite unused member of vtss_fdma_list_t to support H/W DCBs bigger than VTSS_OS_DCACHE_LINE_SIZE_BYTES"
#endif /* VTSS_OS_DCACHE_LINE_SIZE_BYTES <= VTSS_FDMA_DCB_SIZE_BYTES */

/**
 * \brief Software DCB structure.
 *
 * The following structure defines a software DCB. Software DCBs can be linked
 * together to form a list of software DCBs. This is the fundamental data
 * structure used to transfer information between a user-level application and
 * the FDMA driver code.
 *
 * The structure holds the actual hardware DCB needed by the FDMA silicon,
 * a pointer to the associated data area, and other properties such as frame
 * length and allocation length.
 *
 * The structure is used for both injection and extraction, but some of its
 * members' meaning change slightly in the two cases. The exact interpretation
 * is shown as comments inside the definition below.
 */
typedef struct tag_vtss_fdma_list {
    /**
     * <B>XTR/INJ/AFI:</B>\n
     *   Reserved by vtss_fdma.c. The FDMA itself only requires this field to be 4-byte aligned, but
     *   if the dcache is enabled, it's safe to have it cache-line aligned, i.e. 32-byte aligned,
     *   especially if you allocate an array of vtss_fdma_list_t items, because both the CPU and
     *   the FDMA may update two adjacent items simultaneously, and because the cache invalidate
     *   function typically writes back the start cache line if it's not cache-line aligned.
     */
    u8 hw_dcb[VTSS_FDMA_DCB_SIZE_BYTES] VTSS_OS_COMPILER_ATTRIBUTE_ALIGNED(VTSS_OS_DCACHE_LINE_SIZE_BYTES);

    /**
     * Always make the H/W DCB take up a full cache line, or invalidating it might
     * also invalidate any data written to subsequent field in this structure.
     */
    u8 unused[VTSS_OS_DCACHE_LINE_SIZE_BYTES - VTSS_FDMA_DCB_SIZE_BYTES]; // Works as long as this gives a positive number.
    /**
     * <B>XTR:</B>\n
     *   VIRTUAL ADDRESS. Must point to a pre-allocated area of alloc_len bytes. Must be 32-bit aligned. Set by user.\n
     * <B>INJ/AFI:</B>\n
     *   VIRTUAL ADDRESS. Must point to a pre-allocated area of act_len bytes. For SOF DCB,
     *   it must point to the IFH. Need only be byte-aligned. Set by user.
     */
    u8 *data;
    /**
     * <B>XTR/INJ/AFI:</B>\n
     *   VIRTUAL ADDRESS. This points to the first byte of the XTR/INJ IFH. Set by FDMA driver.
     *   This is useful for architectures with variable-length XTR/INJ headers
     *   or in cases where the reserved space in front of the packet is made
     *   such that the same data area can be used for both injection and
     *   extraction (VTSS_OPT_FDMA_VER >= 2).
     */
    u8 *ifh_ptr;

    /**
     * <B>XTR/INJ/AFI:</B>\n
     *   VIRTUAL ADDRESS. This points to the first byte of the frame. Set by FDMA driver.\n
     *   For SOF DCBs, this corresponds to the first byte of the DMAC. For non-SOF DCBs
     *   it points to the first byte of the continued frame.
     *   This is useful for architectures with variable-length XTR/INJ headers
     *   or in cases where the reserved space in front of the packet is made
     *   such that the same data area can be used for both injection and
     *   extraction (VTSS_OPT_FDMA_VER >= 2).
     *   The actual size of the IFH can be found by subtracting ifh_ptr from frm_ptr.
     */
    u8 *frm_ptr;

    /**
     * <B>XTR:</B>\n
     *   Holds the number of bytes extracted into #ifh_ptr. In case of a fragmented frame, the receiver of the frame
     *   must sum up all the act_len in the list of fragments to obtain the full frame size.
     *   For the SOF DCB, act_len includes the size of the IFH and possible start gap, and for EOF DCB,
     *   also the size of the FCS and possible end gap. Set by FDMA driver.\n
     * <B>INJ/AFI:</B>\n
     *   The number of bytes held in associated data area. For the SOF DCB, it includes the size of the IFH
     *   and CMD fields, and for the EOF DCB, also the size of the FCS. Set by user.
     */
    u32 act_len;


    /**
     * <B>XTR:</B>\n
     *   The number of allocated bytes in ->data. Set by user.\n
     * <B>INJ:</B>\n
     *   Not used.
     * <B>AFI:</B>\n
     *   Internally used by the FDMA driver to hold various information.\n
     */
    u32 alloc_len;

    /**
     * <B>XTR:</B>\n
     *   The FDMA driver code time-stamps the packet when the vtss_fdma_irq_handler()
     *   gets invoked based on an extraction interrupt.\n
     * <B>INJ:</B>\n
     *   The FDMA driver code time-stamps the packet when the vtss_fdma_irq_handler()
     *   gets invoked based on an injection interrupt.\n.
     * <B>AFI:</B>\n
     *   Unused.
     * The FDMA driver is agnostic to the time stamp format, and it's up to
     * the platform header (vtss_os.h) to define appropriate types and functions
     * for obtaining the time stamp.
     */
    VTSS_OS_TIMESTAMP_TYPE timestamp;

    /**
     * <B>XTR/INJ/AFI:</B>\n
     *   A pointer to any user data. Set by user and used only by the user. The FDMA code doesn't touch nor uses it.
     */
    void *user;

    // V3+: Part of vtss_fdma_cfg_t (tx_done_cb())
    /**
     * <B>XTR:</B>\n
     *   Not used (a per-channel callback function is configured instead).\n
     * <B>INJ:</B>\n
     *   Callback function called when a frame has been transmitted. Must be set by user in SOF list item.
     *   Not used for non-SOF items, since the callback is only called once per frame.\n
     *     - cntxt:   The value of the parameter passed into the OS-specific call to vtss_fdma_irq_handler().\n
     *     - list:    The list of software-DCBs constituting the frame that has just been injected.\n
     *     - ch:      The FDMA channel that this frame was injected through.\n
     *     - dropped: When FALSE, the frame was successfully transmitted, otherwise an error occurred. Errors can
     *                only occur if injecting to a front port and the INJ_WR_DELAY is not set high
     *                enough, or if the front port is in flow control mode. See also vtss_fdma_inj_flow_ctrl_cfg_set().
     * <B>AFI:</B>\n
     *   Callback function called when a AFI frame has been successfully cancelled transmission. It allows the application
     *   to free the associated memory (DCBs and frame data) once the FDMA is done using it.
     *   The parameters passed in the call to the callback function are the same as the ones passed
     *   for injection (see above). The \@dropped parameter will always be FALSE for AFI, though.
     */
    void (*inj_post_cb)(void *cntxt, struct tag_vtss_fdma_list *list, vtss_fdma_ch_t ch, BOOL dropped);

#if defined(VTSS_FEATURE_AFI_SWC)
    /**
     * <B>XTR/INJ:</B>\n
     *    Not used.\n
     * <B>AFI:</B>
     *   Indicates whether this is a switch-core-based periodically
     *   injected frame. If #afi_id == VTSS_AFI_ID_NONE, it's not
     *   AFI-based.
     */
    vtss_afi_id_t afi_id;
#endif /* defined(VTSS_FEATURE_AFI_SWC)

#if defined(VTSS_FEATURE_AFI_FDMA)
    /**
     * <B>XTR/INJ</B>\n
     *   Not used.
     * <B>AFI</B>\n
     *   Internal parameter. Holds the number of frames to increment #ccm_frm_cnt by for
     *   each interrupt, when the AFI channel is counting.
     *
     * Validity:
     *   Luton26: Y
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u32 afi_frm_incr;

    /**
     * <B>XTR/INJ</B>\n
     *   Not used.
     * <B>AFI</B>\n
     *   Internal parameter. Holds the number of frames to increment #ccm_frm_cnt by
     *   after an interrupt. The reason for this parameter is that the increment may
     *   change if the AFI channel in question will serve a different list of frames
     *   (because an existing is cancelled or a new frame is added) after the interrupt.
     *   The interrupt handler will then update #afi_frm_incr with the value of
     *   #afi_frm_incr_next.
     *
     * Validity:
     *   Luton26: Y
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u32 afi_frm_incr_next;

    /**
     * <B>XTR/INJ</B>\n
     *   Not used.
     * <B>AFI</B>\n
     *   Internal parameter. Temporary that holds the next number of repetitions
     *   of this frame. If all resources required are available, this number will
     *   be copied to #afi_frm_incr_next.
     *
     * Validity:
     *   Luton26: Y
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u32 afi_frm_incr_temp;

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
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u64 ccm_frm_cnt;

    /**
     * <B>XTR/INJ</B>\n
     *   Not used.
     * <B>AFI</B>\n
     *   Holds the next sequence number to put in a given frame.
     *   Updated repeatedly by S/W when AFI sequence numbering is enabled.
     *
     * Validity:
     *   Luton26: Y
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u32 ccm_seq_number;

    /**
     * <B>XTR/INJ:</B>\n
     *   Not used.
     * <B>AFI:</B>\n
     *   Internally used to link AFI frames while adding or deleting AFI frames from a
     *   given AFI-enabled channel. This is in order not to disturb the interrupt handler
     *   in this process, so that interrupts can be kept enabled while updating
     *   the new list of DCBs. Worst case, it takes around 150 ms to create a list with
     *   e.g. 300,000 frames per second that require frame counting and sequence numbering.
     *   It's unacceptable to have interrupts diabled (or the scheduler locked) for such
     *   a long time, since existing frames will not be counted and sequence-number updated then.
     *
     * Validity:
     *   Luton26: Y
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    struct tag_vtss_fdma_list *afi_next;

#endif /* defined(VTSS_FEATURE_AFI_FDMA) */

    /**
     * <B>XTR:</B>\n
     *   Points to the next entry in the list or NULL if it's the last. Set by user on initialization of list.
     *   Continuously updated by vtss_fdma.c afterwards.\n
     * <B>INJ:</B>\n
     *   Points to the next fragment of the frame and set by user on a per-frame basis. Last fragment of a frame
     *   must set ->next to NULL. Once handed to vtss_fdma.c, the driver code takes over.
     * <B>AFI:</B>\n
     *   Must be NULL (AFI frames must be contained in one fragment (due to injection from multiple GPDMA channels
     *   into the same injection group)).
     *   Internally the FDMA driver uses it to link multiple user AFI frames onto the same AFI channel.
     */
    struct tag_vtss_fdma_list *next;
} vtss_fdma_list_t;


// If this is not defined already by the Switch-core-based AFI, define one for the FDMA-based.
#if !defined(VTSS_AFI_FPS_MAX)
    #define VTSS_AFI_FPS_MAX (1000000)  /**< Maximum number of frames to inject per second using FDMA-based AFI */
#endif /* !defined(VTSS_AFI_FPS_MAX) */

/**
 * \brief FDMA Injection Properties.
 *
 * Properties on how to inject a frame using the vtss_fdma_inj() function.
 * All members map or relate to fields in the injected frame's injection header
 * fields.
 *
 * In the following, \@list refers to the list argument passed to the
 * vtss_fdma_inj() function.
 *
 * The application must always memset() this structure
 * to zero before calling vtss_fdma_inj().
 */
typedef struct {
    /**
     * Any user data. Is passed in a call to \@pre_cb().
     */
    void *pre_cb_ctxt1;

    /**
     * Any user data. Is paased in a call to \@pre_cb().
     */
    void *pre_cb_ctxt2;

    /**
     * Callback function called just before the frame is handed over to the FDMA H/W. If NULL, no callback
     * will occur.
     * The called back function may change frame data, nothing else.
     * The called back function *MUST* execute fast and without waiting for synchronization primitives,
     * i.e. no waits are allowed. When called back, interrupts may be disabled.
     * Not used for non-SOF items, since the callback is only called once per frame.\n
     *
     * Implementation notes:
     *   Due to shuffling of beginning-of-frame-fields, this will not work for the following architectures
     *   under the specified circumstances:
     *   Luton26: This will not work for switched frames.
     *   Serval : This will not work for switched frames.
     *
     * Parameters:
     *   - ctxt1:     The value of \@pre_cb_ctxt1.\n
     *   - ctxt2:     The value of \@pre_cb_ctxt2.\n
     *   - list:      Pointer to the SOF item.
     * Validity:
     *   Luton26: Y
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    void (*pre_cb)(void *ctxt1, void *ctxt2, vtss_fdma_list_t *list);

#if defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC)
    /**
     * Number of times this frame will be injected per second.
     *
     * This field must be non-zero if injected on a channel configured for AFI.
     * If injected on a non-AFI channel, this field is ignored.
     *
     * Instead of specifying a specific channel, you may also use a special
     * channel called VTSS_FDMA_CCM_CH_AUTO. Doing so will cause the
     * FDMA driver to attempt to find a suitable channel (pre-configured
     * for AFI) for the injection. See thorough description under
     * 'VTSS_FDMA_CCM_CH_AUTO'.
     *
     * If instead a specific AFI channel is selected, that channel's
     * current frame frequencies must be multiples of this frame's
     * frequency (or vice versa, depending on which number is higher),
     * and there must be room for this new frame, and the frequencies of the
     * currently running frames must not be farther apart than a factor of
     * vtss_fdma_ch_cfg_t::ccm_quotient_max for the call to succeed.
     *
     * Note that there is a limited bandwidth out of the DDR2 SDRAM,
     * through the Shared Bus, and into the FDMA, which also depends on
     * the CPU load. This means that there's an upper limit on the number
     * of frames you can inject per second. Not only does this depend
     * on the frame sizes and their frequencies, but also on H/W and
     * the application's CPU usage (since the CPU shares the Shared Bus
     * with the FDMA). The FDMA driver will not attempt to check that the
     * requested, accumulated bandwidth is greater than this limit, so
     * it's up to the application to determine what makes sense to inject.
     *
     * Valid values when injecting to an AFI channel or using VTSS_FDMA_CCM_CH_AUTO
     * is [1; VTSS_FDMA_CCM_FPS_MAX].
     *
     * There is a number of restrictions on frames injected on a AFI channel:
     *   - #pre_cb must be NULL,
     *   - #switch_frm must be FALSE,
     *   - #port_mask can at most have one bit set, i.e. the frame cannot be multicast,
     *   - #qos_class must not be 8 (i.e. super-prio not supported),
     *   - \@contains_stack_hdr must be FALSE,
     *   - The frame must be held in one single DCB.
     *
     * The vtss_fdma_list_t that vtss_fdma_inj() is called with will not
     * be releasable until the AFI frame is cancelled (see #ccm_cancel) below.
     * Neither will the memory that contains the actual frame. This suggests that AFI frames
     * and associated DCBs should be statically allocated, and for the sake
     * of warm-start, in a place that is not used by the boot loader or
     * overwritten during application initialization.
     *
     *   Luton26: Y
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u32 ccm_fps;
#endif /* defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC) */

#if defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC)
    /**
     * This field is used to indicate whether to start a new periodical AFI
     * frame injection or cancel an ongoing injection.
     *
     * Encoding:
     *  FALSE: Start a new periodical injection. The rest of the properties in
     *         this structure must be set according to the description in
     *         #ccm_fps.
     *  TRUE:  Cancel an ongoing periodic injection. All other fields in
     *         this structure are ignored when this is set, but the
     *         channel number and DCB list in the call to vtss_fdma_inj()
     *         must be identical to the original injection (well, you may
     *         set the channel number to VTSS_FDMA_CCM_CH_AUTO even if
     *         you specified an exact channel number in the original call).
     *         Once the frame is completely removed from the list of
     *         frames currently being injected, the list's inj_post_cb()
     *         function will be called - if non-NULL.
     *
     * This field is ignored unless the channel with which vtss_fdma_inj()
     * is called is configured as a AFI channel or is VTSS_FDMA_CCM_CH_AUTO.
     *
     *   Luton26: Y
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    BOOL ccm_cancel;
#endif /* defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC) */

#if defined(VTSS_FEATURE_AFI_FDMA)
    /**
     * Indicates whether this frame is subject to counting.
     *
     * It is only used when #ccm_fps is > 0.
     *
     * The number of frames injected is returned in the call to inj_post_cb()
     * once the frame is cancelled. Interim counters can be obtained through
     * vtss_fdma_list_t::ccm_frm_cnt.
     *
     * Enabling counting has some side effects, because the counting must
     * be done in S/W. First of all, to avoid overloading the CPU with
     * interrupts, the FDMA driver makes sure that the frame is repeated a number
     * of times to only get interrupts every, say, 50 ms. This requires an awful lot
     * of DCBs, which will be dynamically allocated by the FDMA driver.
     *
     * Validity:
     *   Luton26: Y
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    BOOL ccm_enable_counting;
#endif /* defined(VTSS_FEATURE_AFI_FDMA) */

#if defined(VTSS_FEATURE_AFI_FDMA)
    /**
     * Indicates whether this frame is subject to sequence number updating.
     *
     * It is only used when #ccm_fps is > 0.
     *
     * The offset within the frame to update is set with #ccm_sequence_number_offset.
     *
     * Enabling sequence numbering has some side effects, because the frame updates must
     * be done in S/W. First of all, to avoid overloading the CPU with
     * interrupts, the frame will be copied a number of times to
     * only get interrupts every, say, 50 ms. This requires dynamic memory allocation
     * within the FDMA driver.
     * Let's take an example: Suppose you wish to send 64-byte frames @ 200 Mbps and
     * have these frames sequence numbered. Such frames will give
     *    200,000,000 / (8 * 64) ~= 400,000 frames per second.
     * In other words, the temporal spacing between these frames is 2.5 microseconds,
     * which - to get to a 50 ms interrupt rate - means that the frames must be copied
     * 50000 / 2.5 = 20000 times. With overhead, one 64-byte frame actually requires
     * 88 bytes, so the total amount of (dynamic) memory required for this operation
     * is 1.76 MBytes (not taking into account the DCBs, which are also dynamically
     * allocated within the driver).
     * In fact, you have to double this number, because the driver is made in such a
     * way that the H/W is not stopped while updating the sequence numbers. Instead,
     * the FDMA driver makes a circular list consisting of two 50 ms half-circles,
     * so that S/W can update one half while the H/W is injecting the other half.
     *
     * SO BE CAREFUL WHEN YOU DESIGN THE APPLICATION!
     *
     * Validity:
     *   Luton26: Y
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    BOOL ccm_enable_sequence_numbering;
#endif /* defined(VTSS_FEATURE_AFI_FDMA) */

#if defined(VTSS_FEATURE_AFI_FDMA)
    /**
     * This field indicates the zero-based byte-offset within the frame to increment by one.
     *
     * It is only used when #ccm_enable_sequence_numbering is TRUE.
     *
     * The initial value of the sequence number is whatever the frame you inject
     * contains at that offset.
     *
     * The FDMA driver assumes a 4-byte, any-aligned, network-ordered (big endian) value,
     * which wraps to 0 at 0xFFFFFFFF.
     *
     * Needless to say that the full 4-byte value must be contained within the frame's length.
     *
     * Validity:
     *   Luton26: Y
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u16 ccm_sequence_number_offset;
#endif /* defined(VTSS_FEATURE_AFI_FDMA) */

    /**
     * As the name implies, if this is TRUE, the FDMA driver
     * code will not modify the IFH before frame injection. The various
     * members of this structure will therefore not be used.
     * This is primarily thought for debugging, or for adding extra
     * features not supported by the API.
     * Normally, you would set this to FALSE.
     */
    BOOL dont_touch_ifh;

    /**
     * If FALSE, the analyzer processing is skipped for this frame,
     * and the destination port set is specified with #port_mask.
     * frame will be passed through the analyzer to find the
     * destination port set. The analyzer looks up the DMAC in the
     * MAC table and forwards based on its findings. The frame may therefore
     * get flooded.
     *
     * If FALSE, the destination port set must be specified with #port_mask.
     * Validity:
     *   Luton26: Y
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     *
     * On Luton26 + Serval: If FALSE the rewriter still uses the tag information for rewriting actions.
     */
    BOOL switch_frm;

    /**
     * This field provides the destination port set onto which to send
     * the frame and may thus be used to multicast the same frame on
     * multiple front ports in one go.
     * The field is only used if #switch_frm is FALSE.
     *
     * The width of the field is VTSS_PHYS_PORT_CNT. The mask thus contains
     * a bit for every front port, where bit 0 in the mask corresponds
     * to physical front port number 0, etc.
     * Some architectures allow for looping a frame back to the CPU on
     * a specific CPU extraction queue, but this is not currently
     * supported.
     * Notice: If #chip_no is 1, this mask can only have one bit set.
     * Validity:
     *   Luton26: Y
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u64  port_mask;

    /**
     * This field provides the destination chip (in a multi-chip solution)
     * that the frame should be injected onto.
     * It is only used in multi-chip solutions, and only
     * if #switch_frm is FALSE.
     * If #chip_no == 1, #port_mask must have exactly one bit set.
     *
     * Valid values: [0; 1]
     * Validity:
     *   Luton26: N
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    vtss_chip_no_t chip_no;

    /**
     * The 12-bit VLAN ID that the frame is injected on.
     *
     * #switch_frm == TRUE:
     *   The frame's broadcast domain is this VID.
     *   It is not possible to specify PCP and DEI.
     *   The frame may or may not be tagged on egress depending on the port's
     *   untagged VID (set). If it gets tagged, the port's setup determines
     *   the VLAN Ethertype.
     *
     * #switch_frm == FALSE:
     *   If #tpid is non-zero, a VLAN tag with TPID = #tpid,
     *   VID = #vlan & 0xFFF, and PCP = #qos_class is inserted in
     *   the frame prior to transmission. CFI will become 0.
     *
     * Validity:
     *   Luton26: Y
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     *
     * On Luton26 and Serval:
     *   The VLAN is physically inserted into the frame data in
     *   order to be able to inject on this VLAN. This means
     *   that the user's frame data will be modified. This is
     *   not reversible. The additional space needed for this
     *   is reflected in VTSS_FDMA_INJ_HDR_SIZE_BYTES.
     */
    u16 vlan; // 12 LSBits valid.

    /**
     * If not VTSS_ISDX_NONE, it will be used in ES0 lookups instead of the frame's classified VID.
     * Ignored when #switch_frm is TRUE.
     *
     * Validity:
     *   Luton26: N
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    vtss_isdx_t isdx;

   /**
     * The DP value is inserted into injection header to become drop precedence of the frame.
     * Ignored when #switch_frm is TRUE.
     *
     * Validity:
     *   Luton26: N
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    vtss_dp_level_t dp;

     /**
     * The PCP value is inserted into injection header to become classified PCP of the frame.
     * Only relevant if 'switch_frm' is FALSE
     *
     * Validity:
     *   Luton26: N
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u8 pcp;

   /**
     * The logical port number of the masquerade port.
     * When masquerading, the frame will be handled as if it was received by the
     * ingress port specified in #masquerade_port. Its value will not be used unless
     * #switch_frm is TRUE.
     *
     * When maquerading, the FDMA driver may or may not insert a VLAN tag into the
     * frame prior to transmission. This is determined by #vlan.
     *
     * Set to VTSS_PORT_NO_NONE to disable masquerading.
     *
     * Validity:
     *   Luton26: N
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    vtss_port_no_t masquerade_port;

   /**
     * VLAN Tag Protocol Identifier.
     *
     * When #tpid != 0 and #switch_frm == FALSE, a VLAN tag
     * will be inserted into the frame prior to transmission.
     * The tag will be composed as TPID = #tpid, VID = #vlan & 0xFFF,
     * and PCP = #qos_class. CFI will become 0.
     *
     * Validity:
     *   Luton26: Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    vtss_etype_t tpid;

    /**
     * The frame's Precision Time Protocol action.
     * See #vtss_packet_ptp_action_t for the enumeration.
     * Ignored when #switch_frm is TRUE.
     *
     * When != VTSS_PACKET_PTP_ACTION_NONE, the #ptp_timestamp and #ptp_id fields must be considered.
     *
     * Validity:
     *   Luton26: Y - except for enumeration VTSS_PACKET_PTP_ACTION_ORIGIN_TIMESTAMP.
     *   Serval : Y - except for enumeration VTSS_PACKET_PTP_ACTION_ONE_AND_TWO_STEP.
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    vtss_packet_ptp_action_t ptp_action;

    /**
     * The PTP identifier used for two-step PTP actions.
     * The CPU can only use from IDs 0 through 3.
     *
     * Used when #ptp_action == VTSS_PACKET_PTP_ACTION_TWO_STEP.
     *
     * Validity:
     *   Luton26: Y
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u8 ptp_id;

    /**
     * Holds the PTP timestamp indicating when the injection started.
     * The rewriter can then calculate a residence time based on this and the
     * frame's transmission timestamp.
     *
     * Used when #ptp_action is != VTSS_PACKET_PTP_ACTION_NONE.
     *
     * Validity:
     *   Luton26: Y
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u32 ptp_timestamp;

    /**
     * Latch timestamp into a switch core register when the frame
     * is transmitted. This register can then be read-out by S/W
     * to obtain the actual transmission time for that frame.
     * Encoding:
     *   0: Don't latch timestamp.
     *   1: Latch timestamp into register 0.
     *   2: Latch timestamp into register 1.
     *   3: Latch timestamp into register 2.
     *
     * Validity:
     *   Luton26: N
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u32 latch_timestamp;

    /**
     * The aggregation code that this frame will use. The 4-bit number
     * maps directly to the corresponding field in the IFH, but is
     * only set if the frame is being switched (the #switch_frm member
     * is non-zero) and the destination port number is VTSS_CPU_PM_NUMBER.
     * Validity:
     *   Luton26: N
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u8 aggr_code;

    /**
     * The QoS class that this frame will be transmitted on.
     * This is a number in range [0; 8]. Note that even if the frame
     * contains a stack header (contains_stack_hdr == TRUE), this field
     * will have to be set to 8 if the frame must be transmitted on
     * super-prio. The SUPER_PRIO field within the stack header determines
     * further transmission (i.e. on the receiving switch, and further in
     * the system).
     *
     * If you set it to '8', the frame will be transmitted with super-priority,
     * i.e. with even higher priority than the highest priority achievable for
     * normal data traffic. This is not a valid setting if #switch_frm == TRUE.
     *
     * Luton26 and Serval notes:
     *   A value of '8' (super priority) will be converted to a value of '7'.
     *
     * Validity:
     *   Luton26: Y
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u8 qos_class;


    /**
     * OAM type.
     *
     * Only used if #ptp_action is zero.
     * See vtss_packet_oam_type_t for a description.
     *
     * Validity:
     *   Luton26: N
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    vtss_packet_oam_type_t oam_type;
} vtss_fdma_inj_props_t;

/**
 * \brief Channel usage.
 *
 * A given FDMA channel can either be used for extraction or injection.
 */
typedef enum {
    VTSS_FDMA_CH_USAGE_UNUSED, /**< The channel is not currently in use.                               */
    VTSS_FDMA_CH_USAGE_XTR,    /**< The channel is used/supposed to be used for frame extraction.      */
    VTSS_FDMA_CH_USAGE_INJ,    /**< The channel is used/supposed to be used for frame injection.       */
#if defined(VTSS_FEATURE_AFI_FDMA)
    VTSS_FDMA_CH_USAGE_CCM,    /**< The channel is used/supposed to be used for period frame injection */
#endif /* defined(VTSS_FEATURE_AFI_FDMA) */
} vtss_fdma_ch_usage_t;

#if (defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC))
    /**
     * Use this channel number in calls to vtss_fdma_inj() to
     * ask the FDMA driver to find a suitable channel pre-configured
     * for periodic transmission to inject the frame through.
     *
     * The driver will first search through the channel list for
     * channels that currently use the same frame frequency or a multiple
     * of the frame frequency given by vtss_fdma_inj_props_t::ccm_fps.
     * For this channel to be considered, the frame frequencies of the currently
     * running frames must not be more than a factor of vtss_fdma_ch_cfg_t::ccm_quotient_max
     * apart from the one attempted injected.
     *
     * If such a channel is found, that channel will be used. If it then turns
     * out that there's not room for more AFI frames on this channel, the call will fail.
     *
     * If no suitable channel is found, the driver will search for an AFI channel that
     * is not currently in use and use that one.
     *
     * Otherwise the call to vtss_fdma_inj() will fail.
     */
    #define VTSS_FDMA_CCM_CH_AUTO -1
#endif /* (defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC))

/**
 * \brief Channel configuration structure.
 *
 * The following structure defines the parameters needed to configure
 * a DMA channel.
 * The DMA channel can either be used for frame extraction, frame injection,
 * or period frame injection.
 * This is controlled by the #usage parameter.
 *
 * The interpretation and validity of the remaining fields depend on the
 * #usage parameter.
 *
 * Note: For future compatibility, all applications must memset() this structure to
 * 0 before filling it in.
 */
typedef struct {
    /**
     * <B>XTR/INJ/AFI:</B>\n
     * Indicates whether this channel is used for extraction, injection, or
     * periodic injection.
     * Luton26 note: At most one channel may be configured for AFI use.
     * To disable a channel, set this member to VTSS_FDMA_CH_USAGE_UNUSED.
     */
    vtss_fdma_ch_usage_t usage;

#if defined(VTSS_FEATURE_PACKET_GROUPING)
    /**
     * <B>XTR:</B>\n
     * The extraction group that this channel serves.
     * Need only be set if cfg->chip_no == 1.
     * Valid values are [0; VTSS_PACKET_RX_GRP_CNT[
     *
     * <B>INJ/AFI:</B>\n
     * Unused.
     *
     * Validity:
     *   Luton26: N
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    vtss_packet_rx_grp_t xtr_grp;

    /**
     * <B>XTR:</B>\n
     * Unused.
     *
     * <B>INJ:</B>\n
     * A mask containing the injection groups that this channel serves.
     * A channel may serve more than one injection group. On some architectures
     * a given injection group serves a given type of service (e.g. super-
     * priority injection, switched injection, front-port directed injection,
     * etc.). On such architectures, the actual injection group to use is
     * conveyed in the call to vtss_fdma_inj().
     *
     * At least one bit must be set in the mask. The most significant bit that
     * can be set is VTSS_PACKET_TX_GRP_CNT - 1.
     *
     * <B>AFI:</B>\n
     * A mask containing the injection group that the AFI channel serves.
     * This is a one-hot mask, that is, exactly one bit must be set, and
     * the most significant bit that can be set is VTSS_PACKET_TX_GRP_CNT - 1.
     *
     * Validity:
     *   Luton26: Y, one-hot (exactly one bit set)
     *   Serval : Not used. Implicitly set through the channel number.
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u32 inj_grp_mask;
#endif /* VTSS_FEATURE_PACKET_GROUPING */

    /**
     * <B>XTR:</B>\n
     * A linked, NULL-terminated list of software DCBs, into which the FDMA
     * extract frames. One frame may take one or more DCBs, depending on the
     * size of the associated data area. The following fields of each list
     * item must be pre-initialized by the Packet Module (see definition of
     * vtss_fdma_list_t structure for details):
     * \@data, \@alloc_len, (user), and \@next.
     *
     * <B>INJ:</B>\n
     * Unused.
     *
     * <B>AFI:</B>\n
     * Unused. Must be NULL. The FDMA driver allocates DCBs on its own.
     * This approach is chosen because it's very hard to pre-determine
     * a good number of DCBs that must be allocated, since it depends on
     * whether the application uses frame counting or sequence numbering
     * or not, and whether it may add or cancel frames so fast that both
     * a running, a pending, and a new list of frames must be built up.
     */
    vtss_fdma_list_t *list;

    /**
     * <B>XTR:</B>\n
     * The address of the callback function invoked by the FDMA driver code
     * when a frame arrives. The callback function is invoked once per frame.
     *   - Call context:\n
     *     DSR (the same as what invokes vtss_fdma_irq_handler().
     *   - The parameters to the callback function are as follows:\n
     *      -# cntxt: The value passed in the Packet Module's call to vtss_fdma_irq_handler().
     *      -# list:  NULL-terminated list of software DCBs making up the frame. Depending
     *                on the frame size one or more DCBs are used. The last is always
     *                NULL-terminated. The FDMA has filled in the following fields (see
     *                definition of vtss_fdma_list_t structure for deatils):
     *                (\@dcb), \@*data, \@act_len, (\@next).
     *                Once delivered to the Packet Module, the FDMA will not use the
     *                list again. The FDMA can, however, be handed back a (new) list of
     *                software DCBs by returning the list when the xtr_cb() returns.
     *                Alternatively, use the vtss_fdma_xtr_add_dcbs() function.
     *      -# qu:    The extraction queue that this frame was extracted from. This
     *                may be useful if all extraction callback functions map to the
     *                same function.
     *      - Expected return value from xtr_cb():\n
     *        Non-NULL if the Packet Module wishes to give the FDMA new DCBs to extract to.
     *        In the case where the Packet Module handles the frame directly, it could as well
     *        pass back the list that xtr_cb() was called with right away. In the case where
     *        the frame is passed up to higher levels, like an IP stack, the Packet Module
     *        will probably return NULL in the call to xtr_cb() and provide a replacement
     *        list at a later point in time with a call to vtss_fdma_xtr_add_dcbs().
     *
     * <B>INJ/AFI:</B>\n
     * Unused.
     */
     vtss_fdma_list_t *(*xtr_cb)(void *cntxt, vtss_fdma_list_t *list, vtss_packet_rx_queue_t qu);

    /**
     * <B>XTR/INJ/AFI:</B>\n
     * Controls the channel priority. Valid values are [0; 7].
     * The higher value the higher priority. Everytime the DMA H/W
     * needs to find the next channel to serve, it selects -
     * amongst the pending channels - the channel with the highest
     * priority. Two or more channels may have the same priority,
     * in which case the DMA H/W grants the lowest-numbered channel
     * access.
     * Note: If using the deprecated vtss_fdma_inj_cfg() and
     * vtss_fdma_xtr_cfg() functions, the channel priority will
     * be the same as the channel number.
     */
    int  prio;

    /**
     * <B>XTR</B>\n
     * In a dual-chip solution, this controls the chip to extract from.
     * For single chip solutions, this field must be set to 0.
     * For dual-chip solutions, this field must be set to 0 for extraction
     * from the primary chip, and in this case, the xtr_grp must match
     * the channel number.
     * For dual-chip solutions, this field must be set to 1 for extraction
     * from the secondary chip, and in this case, the xtr_grp need not
     * match the channel number, because - at the time of writing - the
     * channel number isn't really used for anything in that case, because
     * the secondary chip's frames have to be read out manually because they
     * can't be auto-transferred to the primary chip's CPU extraction queues
     * without losing the CPU Rx queue number that it was stored in on the
     * secondary chip.
     *
     * <B>INJ/AFI:</B>\n
     * Unused.
     */
    vtss_chip_no_t chip_no;

#if defined(VTSS_FEATURE_AFI_FDMA)
    /**
     * Maximum quotient allowed on a given AFI channel.
     */
     #define VTSS_FDMA_CCM_QUOTIENT_MAX 3

    /**
     * <B>AFI:</B>\n
     * Controls the channel's maximum frequency quotient.
     * A given AFI channel can handle several frame frequencies provided they
     * are multiples of each other.
     * This configuration option determines the maximum frequency qoutient two frames
     * running on the same channel can have.
     * If for instance, it's set to 2 and the first frame is transmitted with 50 fps,
     * then a subsequent frame can be transmitted with a requested frequency of
     * 25, 50, or 100 fps, only.
     * To allow only one frequency on the channel, set it to 1.
     *
     * Valid values are in the range [1; VTSS_FDMA_CCM_QUOTIENT_MAX].
     *
     * Validity:
     *   Luton26: Y
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
     u32 ccm_quotient_max;

    /**
     * <B>AFI:</B>\n
     * The following defines the maximum number of different frequencies (all multiples of
     * each other) that can run on the same channel.
     *
     * Validity:
     *   Luton26: Y
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    #define VTSS_FDMA_CCM_FREQ_LIST_LEN 5

    /**
     * <B>AFI:</B>\n
     * The following defines the maximum number of frames per second that can be used
     * in vtss_fdma_inj_props_t's fps member.
     * There is absolutely no guarantee that the actual H/W architecture can reach
     * this number of frames per second.
     *
     * Validity:
     *   Luton26: Y
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
     #define VTSS_FDMA_CCM_FPS_MAX VTSS_AFI_FPS_MAX
#endif /* defined(VTSS_FEATURE_AFI_FDMA) */
} vtss_fdma_ch_cfg_t;

/**
 * \brief FDMA configuration structure.
 *
 * The following structure defines the parameters needed to configure
 * the FDMA in general.
 *
 * Note: For future compatibility, all applications must memset() this structure to
 * 0 before filling it in.
 */
typedef struct {
    /**
     * <B>XTR/INJ:</B>\n
     *   This is just a dummy, that is not used for anything, but which
     *   must be here to make the structure non-empty when stacking is
     *   not defined.
     *   It can safely be removed once other non-stacking parameters are
     *   added to this structure.
     *
     * Validity:
     *   Luton26: N
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u32 dummy;
} vtss_fdma_cfg_t;

typedef struct {
    /**
     * Controls - per extraction queue - the maximum number of frames
     * extracted between two calls to vtss_fdma_throttle_tick() without
     * suspending extraction from that queue.
     *
     * If 0, throttling is disabled for that extraction queue.
     */
    u32 limit_per_tick[VTSS_PACKET_RX_QUEUE_CNT];



    /**
     * Controls - per extraction queue - the number of invocations of
     * vtss_fdma_throttle_tick() that must happen before an extraction queue
     * that has been disabled, gets re-enabled.
     *
     * For instance,
     *   a value of 0 means: re-enable the extraction queue on the next tick.
     *   a value of 1 means: re-enable the extraction queue two ticks from when it was suspended.
     */
    u32 suspend_tick_cnt[VTSS_PACKET_RX_QUEUE_CNT];
} vtss_fdma_throttle_cfg_t;

vtss_rc vtss_fdma_init(const vtss_inst_t inst);
vtss_rc vtss_fdma_uninit(const vtss_inst_t inst);
vtss_rc vtss_fdma_cfg(const vtss_inst_t inst, const vtss_fdma_cfg_t *const cfg);
vtss_rc vtss_fdma_ch_cfg(const vtss_inst_t        inst,
                         const vtss_fdma_ch_t     ch,
                         const vtss_fdma_ch_cfg_t *const cfg);
vtss_rc vtss_fdma_xtr_add_dcbs(const vtss_inst_t      inst,
                               const vtss_fdma_ch_t   xtr_ch,
                                     vtss_fdma_list_t *const list);
vtss_rc vtss_fdma_inj(const vtss_inst_t           inst,
                            vtss_fdma_list_t      *list,
                      const vtss_fdma_ch_t        ch,
                      const u32                   len,
                      const vtss_fdma_inj_props_t *const props);
vtss_rc vtss_fdma_throttle_cfg_get(const vtss_inst_t inst, const vtss_fdma_ch_t ch, vtss_fdma_throttle_cfg_t *const cfg);
vtss_rc vtss_fdma_throttle_cfg_set(const vtss_inst_t inst, const vtss_fdma_ch_t ch, const vtss_fdma_throttle_cfg_t *const cfg);
vtss_rc vtss_fdma_throttle_tick(const vtss_inst_t inst);
vtss_rc vtss_fdma_xtr_ch_from_list(const vtss_inst_t inst, const vtss_fdma_list_t *const list, vtss_fdma_ch_t *const ch); /* Can be used to get the channel number that the frame pointed to by list was extracted from. */

typedef struct {
    /**
     * The physical port number on which the frame was received.
     * Validity:
     *   Luton26: Y
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    vtss_phys_port_no_t src_port;
    /**
     * TRUE when #src_port contains valid information, FALSE otherwise.
     */
    BOOL                src_port_decoded;

    /**
     * The chip on which the frame was received (dual chip solutions)
     * Validity:
     *   Luton26: Y (always 0)
     *   Serval : Y (always 0)
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    vtss_chip_no_t chip_no;
    /**
     * TRUE when #chip_no contains valid information, FALSE otherwise.
     */
    BOOL           chip_no_decoded;

    /**
     * If set, frame was received with a tag that was removed.
     * Validity:
     *   Luton26: N
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    BOOL was_tagged;
    /**
     * TRUE when #was_tagged contains valid information, FALSE otherwise.
     */
    BOOL was_tagged_decoded;

    /**
     * The tag information's associcated Tag Protocol Identifier.
     * The definitions are:
     *   0: C-Tag: EtherType = 0x8100.
     *   1: S-Tag: EtherType = 0x88A8 or custom value.
     * Validity:
     *   Luton26: Y
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u8   tag_type;
    /**
     * TRUE when #tag_type contains valid information, FALSE otherwise.
     */
    BOOL tag_type_decoded;

    /**
     * The 12-bit VLAN ID from the VLAN classification.
     * Validity:
     *   Luton26: Y
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u16  vid;
    /**
     * TRUE when #vid contains valid information, FALSE otherwise.
     */
    BOOL vid_decoded;

    /**
     * The 10-bit ISDX from IS1 classification.
     * Validity:
     *   Luton26: N
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u16  isdx;
    /**
     * TRUE when #isdx contains valid information, FALSE otherwise.
     */
    BOOL isdx_decoded;

    /**
     * The frame's 1-bit CFI/DEI field from the VLAN classification.
     * Validity:
     *   Luton26: Y
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    BOOL dei;
    /**
     * TRUE when #dei contains valid information, FALSE otherwise.
     */
    BOOL dei_decoded;

    /**
     * The frame's 3-bit uprio/PCP field from the VLAN classification.
     * Validity:
     *   Luton26: Y
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u8   pcp;
    /**
     * TRUE when #pcp contains valid information, FALSE otherwise.
     */
    BOOL pcp_decoded;

    /**
     * The reason for copying or redirecting the frame to the CPU.
     * The definitions are:
     *   000: CPU_NORMAL
     *   001: CPU_ONLY_RSVD
     *   010: CPU_COPY_IPMC
     *   011: Reserved
     *   100: CPU_ONLY_IGMP
     *   101: CPU_ONLY_MLD
     *   110: Reserved
     *   111: CPU_COPY_ACL
     * Validity:
     *   Luton26: N
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u8   frame_type;
    /**
     * TRUE when #frame_type contains valid information, FALSE otherwise.
     */
    BOOL frame_type_decoded;

    /**
     * If set, the frame's SMAC is subject to learning.
     * Validity:
     *   Luton26: N
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    BOOL learn;
    /**
     * TRUE when #learn contains valid information, FALSE otherwise.
     */
    BOOL learn_decoded;

    /**
     * The CPU extraction queue number when forwarding the frame
     * to the CPU port module.
     * Validity:
     *   Luton26: Y
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u16  xtr_qu;
    /**
     * TRUE when #xtr_qu contains valid information, FALSE otherwise.
     */
    BOOL xtr_qu_decoded;

    /**
     * CPU extraction queue mask (one bit per CPU extraction queue).
     * Each bit implies te frame was subject to CPU forwarding to the
     * specific queue.
     * Validity:
     *   Luton26: Y
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u32  xtr_qu_mask;
    /**
     * TRUE when #xtr_qu_mask contains valid information, FALSE otherwise.
     */
    BOOL xtr_qu_mask_decoded;

    /**
     * The frame's classified DSCP value.
     * Luton26:
     *   If the frame is hardware timestamped (frame has hit a rule in IS2
     *   with PTP_ENA), the DSCP field contains the timestamp identifier
     *   provided by the analyzer.
     *
     * Validity:
     *   Luton26: Y
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u8   dscp;
    /**
     * TRUE when #dscp contains valid information, FALSE otherwise.
     */
    BOOL dscp_decoded;

    /**
     * Two-step PTP Tx timestamp identifier (6 bits).
     *
     * Validity:
     *   Luton26: N
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u32 tstamp_id;
    /**
     * TRUE when #tstamp_id contains valid information, FALSE otherwise.
     */
    BOOL tstamp_id_decoded;


    /**
     * The frame's ingress timestamp.
     *
     * Serval:
     *   Two-step PTP frames have the 32-bit Rx timestamp saved in this field.
     *
     * Validity:
     *   Luton26: N
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u32  tstamp;
    /**
     * TRUE when #tstamp contains valid information, FALSE otherwise.
     */
    BOOL tstamp_decoded;

    /**
     * Various un-decodable OAM info. Decoding of the OAM info
     * field from the extraction header requires accompanying
     * frame info, and is therefore saved as an opaque type,
     * letting it be up to the application to decode it based on
     * description in the datasheet.
     *
     * Serval:
     *   This corresponds to the contents of the REW_VAL field
     *   in the extraction header. oam_info_decoded = TRUE when
     *   REW_OP[2:0] == 4. Only the 32 lsbits of #oam_info are used.
     *
     * Validity:
     *   Luton26: N
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u64  oam_info;
    /**
     * TRUE when #oam_info contains valid information, FALSE otherwise.
     */
    BOOL oam_info_decoded;

    /**
     *   If #acl_hit is set, this value is the entry number of the rule
     *   hit in IS2. If both IS2 lookups hit a rule which copy the frame
     *   to the CPU, then the second lookup's entry number is used.
     *
     *   For Serval, this is the combined ACL_ID action coming out of the
     *   two IS2 look-ups.
     * Validity:
     *   Luton26: Y
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u32  acl_idx;
    /**
     * TRUE when #acl_idx contains valid information, FALSE otherwise.
     */
    BOOL acl_idx_decoded;

    /**
     * SFlow type. Indicates if this is copied to the CPU due to Rx or Tx SFlow.
     *
     * Encoding - only valid if sflow_type_decoded == TRUE:
     *  0: Rx SFlow.
     *  1: Tx SFlow.
     *
     *  If #sflow_type_decoded == TRUE, the Rx or Tx port that gave rise to this is given by #sflow_port.
     *
     * Validity:
     *   Luton26: Y
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
     u8   sflow_type;
    /**
     * TRUE when #sflow_type contains valid information, FALSE otherwise.
     */
     BOOL sflow_type_decoded;

    /**
     * SFlow port.
     * Indicates the SFlow Rx or Tx port number that caused this frame to be copied to the CPU.
     * #sflow_port_decoded can only be set if #sflow_type_decoded == TRUE.
     * Validity:
     *   Luton26: Y
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u32  sflow_port;
    /**
     * TRUE when #sflow_port contains valid information, FALSE otherwise.
     */
    BOOL sflow_port_decoded;

    /**
     * SFlow frame fwd count.
     * Indicates the total number of frames transmitted. This is normally an 18-bit counter, but if
     * ANA_AC:PS_COMMON_CFG.SFLOW_SMPLD_ID_IN_STAMP_ENA is set to 0, then you may obtain a 24-bit counter
     * by concatenating the 6 LSbits of #sflow_port with the 18 LSbits of #sflow_fwd_cnt.
     * Validity:
     *   Luton26: N
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u32  sflow_fwd_cnt;
    /**
     * TRUE when #sflow_fwd_cnt contains valid information, FALSE otherwise.
     */
    BOOL sflow_fwd_cnt_decoded;

    /**
     * SFlow sampled frames count.
     * Indicates the number of SFlow sampled frames.
     * Validity:
     *   Luton26: N
     *   Serval : N
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u32  sflow_sample_cnt;
    /**
     * TRUE when #sflow_sample_cnt contains valid information, FALSE otherwise.
     */
    BOOL sflow_sample_cnt_decoded;

    /**
     * Set if frame has hit a rule in IS2, which copies the frame to the
     * CPU (IS2 actions CPU_COPY_ENA or HIT_ME_ONCE). #acl_idx contains
     * the IS2 entry number.
     * For Serval, the #acl_idx contains the combined ACL_ID action of
     * the rules hit in IS2.
     * Validity:
     *   Luton26: Y
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    BOOL acl_hit;
    /**
     * TRUE when #acl_hit contains valid information, FALSE otherwise.
     */
    BOOL acl_hit_decoded;

    /**
     * The frame's drop precedence (DP) level after policing.
     * Validity:
     *   Luton26: Y
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u8   dp;
    /**
     * TRUE when #dp contains valid information, FALSE otherwise.
     */
    BOOL dp_decoded;

    /**
     * The source MAC address learning action triggered by the frame.
     * 0: No learning.
     * 1: Learning of a new entry.
     * 2: Updating of an already learned unlocked entry.
     * 3: Updating of an already learned locked entry.
     * Validity:
     *   Luton26: Y
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u8   lrn_flags;
    /**
     * TRUE when #lrn_flags contains valid information, FALSE otherwise.
     */
    BOOL lrn_flags_decoded;

    /**
     * The frame's classified QoS class.
     * Validity:
     *   Luton26: Y
     *   Serval : Y
     *   Jaguar2: N
     *   Serval2: N
     *   ServalT: N
     */
    u8   qos_class;
    /**
     * TRUE when #qos_class contains valid information, FALSE otherwise.
     */
    BOOL qos_class_decoded;


} vtss_fdma_xtr_props_t;

vtss_rc vtss_fdma_xtr_hdr_decode(const vtss_inst_t           inst,
                                 const vtss_fdma_list_t      *const list,
                                       vtss_fdma_xtr_props_t *const xtr_props);
vtss_rc vtss_fdma_inj_mirror_port_get(const vtss_inst_t inst, vtss_chip_no_t *const chip_no, vtss_phys_port_no_t *const phys_port);
vtss_rc vtss_fdma_inj_mirror_port_set(const vtss_inst_t inst, vtss_chip_no_t chip_no, vtss_phys_port_no_t phys_port);
vtss_rc vtss_fdma_irq_handler(const vtss_inst_t inst, void *const cntxt);

#ifdef __cplusplus
}
#endif
#endif /* defined(VTSS_FEATURE_FDMA) && VTSS_OPT_FDMA */
#endif /* _VTSS_FDMA_API_H_ */
/******************************************************************************
 *
 * End of file
 *
 *****************************************************************************/




vtss_rc vtss_fdma_init(const vtss_inst_t inst);
vtss_rc vtss_fdma_uninit(const vtss_inst_t inst);
vtss_rc vtss_fdma_cfg(const vtss_inst_t inst, const vtss_fdma_cfg_t *const cfg);
vtss_rc vtss_fdma_ch_cfg(const vtss_inst_t        inst,
                         const vtss_fdma_ch_t     ch,
                         const vtss_fdma_ch_cfg_t *const cfg);
vtss_rc vtss_fdma_xtr_add_dcbs(const vtss_inst_t      inst,
                               const vtss_fdma_ch_t   xtr_ch,
                                     vtss_fdma_list_t *const list);
vtss_rc vtss_fdma_inj(const vtss_inst_t           inst,
                            vtss_fdma_list_t      *list,
                      const vtss_fdma_ch_t        ch,
                      const u32                   len,
                      const vtss_fdma_inj_props_t *const props);
vtss_rc vtss_fdma_throttle_cfg_get(const vtss_inst_t inst, const vtss_fdma_ch_t ch, vtss_fdma_throttle_cfg_t *const cfg);
vtss_rc vtss_fdma_throttle_cfg_set(const vtss_inst_t inst, const vtss_fdma_ch_t ch, const vtss_fdma_throttle_cfg_t *const cfg);
vtss_rc vtss_fdma_throttle_tick(const vtss_inst_t inst);
vtss_rc vtss_fdma_xtr_ch_from_list(const vtss_inst_t inst, const vtss_fdma_list_t *const list, vtss_fdma_ch_t *const ch); /* Can be used to get the channel number that the frame pointed to by list was extracted from. */
vtss_rc vtss_fdma_xtr_hdr_decode(const vtss_inst_t           inst,
                                 const vtss_fdma_list_t      *const list,
                                       vtss_fdma_xtr_props_t *const xtr_props);
vtss_rc vtss_fdma_inj_mirror_port_get(const vtss_inst_t inst, vtss_chip_no_t *const chip_no, vtss_phys_port_no_t *const phys_port);
vtss_rc vtss_fdma_inj_mirror_port_set(const vtss_inst_t inst, vtss_chip_no_t chip_no, vtss_phys_port_no_t phys_port);
vtss_rc vtss_fdma_irq_handler(const vtss_inst_t inst, void *const cntxt);
