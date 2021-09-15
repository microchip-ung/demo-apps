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

// VTSS_FEATURE_FDMA
// VTSS_OPT_FDMA = 1
// VTSS_OPT_FDMA_VER = 3

#if defined(VTSS_FEATURE_FDMA) && VTSS_OPT_FDMA
/* FDMA available and user-enabled. */

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

#if defined(VTSS_ARCH_JAGUAR_1)
  #define VTSS_FDMA_CH_CNT          8 /**< Number of DMA Channels.                                                */
  #define VTSS_PHYS_PORT_CNT       32 /**< Number of Port Modules, excluding CPU Port Module.                     */
  #define VTSS_FDMA_DCB_SIZE_BYTES 24 /**< Number of bytes in a DCB.                                              */
  #define VTSS_FDMA_HDR_SIZE_BYTES 52 /**< Max(XTR_HDR_SZ, INJ_HDR_SZ). Worst-case is XTR: 24 bytes for IFH + 4 bytes for queue number + 24 bytes for Rx super-prio artificial IFH).
                                       * In FDMA API v3+, also injection requires 52 bytes: 24 bytes for IFH + 4 bytes for VLAN tag + 24 bytes for a signature IFH used for multicast injections on 48-ported */
#endif /* defined(VTSS_ARCH_JAGUAR_1) */

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
#if defined(VTSS_ARCH_LUTON26) || defined(VTSS_ARCH_JAGUAR_1) || defined(VTSS_ARCH_SERVAL) || defined(VTSS_ARCH_JAGUAR_2)
  #define VTSS_FDMA_MAX_DATA_PER_DCB_BYTES         16380
#endif /* LUTON26/JAGUAR_1/SERVAL */

#define VTSS_FDMA_MIN_FRAME_SIZE_BYTES             64

#define VTSS_FDMA_MIN_DATA_PER_INJ_SOF_DCB_BYTES (VTSS_FDMA_HDR_SIZE_BYTES)
#define VTSS_FDMA_MIN_DATA_PER_XTR_SOF_DCB_BYTES (VTSS_FDMA_HDR_SIZE_BYTES)
#define VTSS_FDMA_MIN_DATA_PER_NON_SOF_DCB_BYTES    1
#define VTSS_FDMA_MAX_FRAME_SIZE_BYTES          10000

#if !defined(VTSS_OS_DCACHE_LINE_SIZE_BYTES)
#define VTSS_OS_DCACHE_LINE_SIZE_BYTES 32 /**< The number of bytes one DCache-line is made up of. */
#endif

#if !defined(VTSS_OS_COMPILER_ATTRIBUTE_ALIGNED)
#define VTSS_OS_COMPILER_ATTRIBUTE_ALIGNED(x) __attribute ((aligned(x))) /**< In some special cases, it is of utmost importance that a certain variable has a certain memory alignment. Applications for this is e.g. placing variables on cache-line boundaries.  */
#endif

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

#if (defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC))
/** \brief Select which AFI to use.
 *
 * On platforms that only support either an FDMA-based or a switch-core-based AFI,
 * select the type it supports or VTSS_FDMA_AFI_TYPE_AUTO, which will resort to
 * the one it supports.
 *
 * On platforms that support both AFI types, selecting VTSS_FDMA_AFI_TYPE_AUTO
 * will cause the FDMA to look at FDMA-only properties, that is, whether
 * frame counting or sequence numbering is enabled, and if so choose the FDMA-based,
 * otherwise it will choose the switch-core based.
 */
typedef enum {
     VTSS_FDMA_AFI_TYPE_AUTO = 0, /**< The FDMA driver chooses an appropriate frame injector. */
     VTSS_FDMA_AFI_TYPE_FDMA,     /**< Tell FDMA driver to use the FDMA-based AFI.            */
     VTSS_FDMA_AFI_TYPE_SWC,      /**< Tell FDMA driver to use the switch-core-based AFI.     */
} vtss_fdma_afi_type_t;
#endif /* (defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC)) */

/**
 * \brief FDMA Injection Properties.
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
     *   Jaguar1: Y
     *   Serval : Y
     */
    void (*pre_cb)(void *ctxt1, void *ctxt2, vtss_fdma_list_t *list);

#if defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC)
    /**
     * Number of times this frame will be injected per second.
     *
     \if defined(VTSS_FEATURE_AFI_FDMA)
     * For FDMA-based AFI, the following applies:
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
     \endif
     * There is a number of restrictions on AFI frames:
     *   - #pre_cb must be NULL,
     *   - vtss_packet_tx_info_t::switch_frm must be FALSE,
     *   - vtss_packet_tx_info_t::dst_port_mask can at most have one bit set, i.e. the frame cannot be multicast,
     *   - vtss_packet_tx_info_t::cos must not be 8 (i.e. super-prio not supported),
     *   - vtss_packet_tx_info_t::tx_vstax_hdr must be VTSS_PACKET_TX_VSTAX_NONE,
     *   - The frame must be held in one single DCB.
     *
     * The memory that contains the actual frame that vtss_fdma_tx() is called
     * with will not be releasable until the AFI frame is cancelled (see vtss_fdma_afi_cancel()).
     *
     *   Luton26: Y
     *   Jaguar1: Y
     *   Serval : Y
     */
    u32 afi_fps;
#endif /* defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC) */

#if defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC)
    /**
     * If the platform supports both FDMA-based and switch-core-based
     * AFI, the application must have a way to select one or the other
     * when requesting injection of an AFI frame.
     *
     * Currently any given platform only supports one or the other, so there's
     * no reason to set it to anything but VTSS_FDMA_AFI_TYPE_AUTO.
     */
    vtss_fdma_afi_type_t afi_type;
#endif /* defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC) */

#if defined(VTSS_FEATURE_AFI_FDMA)
    /**
     * Indicates whether this frame is subject to counting.
     *
     * It is only used when #afi_fps is > 0.
     *
     * The number of frames injected is returned in the call to tx_done_cb()
     * once the frame is cancelled. Interim counters can be obtained through
     * vtss_fdma_list_t::afi_frm_cnt.
     *
     * Enabling counting has some side effects, because the counting must
     * be done in S/W. First of all, to avoid overloading the CPU with
     * interrupts, the FDMA driver makes sure that the frame is repeated a number
     * of times to only get interrupts every, say, 50 ms. This requires an awful lot
     * of DCBs, which will be dynamically allocated by the FDMA driver.
     *
     * Validity:
     *   Luton26: Y
     *   Jaguar1: Y
     *   Serval : Y (TBD)
     */
    BOOL afi_enable_counting;
#endif /* defined(VTSS_FEATURE_AFI_FDMA) */

#if defined(VTSS_FEATURE_AFI_FDMA)
    /**
     * Indicates whether this frame is subject to sequence number updating.
     *
     * It is only used when #afi_fps is > 0.
     *
     * The offset within the frame to update is set with #afi_sequence_number_offset.
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
     *   Jaguar1: Y
     *   Serval : Y (TBD)
     */
    BOOL afi_enable_sequence_numbering;
#endif /* defined(VTSS_FEATURE_AFI_FDMA) */

#if defined(VTSS_FEATURE_AFI_FDMA)
    /**
     * This field indicates the zero-based byte-offset within the frame to increment by one.
     *
     * It is only used when #afi_enable_sequence_numbering is TRUE.
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
     *   Jaguar1: Y
     *   Serval : Y (TBD)
     */
    u16 afi_sequence_number_offset;
#endif /* defined(VTSS_FEATURE_AFI_FDMA) */

} vtss_fdma_tx_info_t;

// If this is not defined already by the Switch-core-based AFI, define one for the FDMA-based.
#if !defined(VTSS_AFI_FPS_MAX)
    #define VTSS_AFI_FPS_MAX (1000000)  /**< Maximum number of frames to inject per second using FDMA-based AFI */
#endif /* !defined(VTSS_AFI_FPS_MAX) */


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
     * Luton26 and Jaguar note: At most one channel may be configured for AFI use.
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
     *   Jaguar : Y - for 48-ported, only.
     *   Luton26: N
     *   Serval : N
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
     *   Jaguar : Y
     *   Luton26: Y, one-hot (exactly one bit set)
     *   Serval : Not used. Implicitly set through the channel number.
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
     *   Jaguar1: Y
     *   Serval : Y (TBD)
     */
     u32 ccm_quotient_max;

    /**
     * <B>AFI:</B>\n
     * The following defines the maximum number of different frequencies (all multiples of
     * each other) that can run on the same channel.
     *
     * Validity:
     *   Luton26: Y
     *   Jaguar1: Y
     *   Serval : Y (TBD)
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
     *   Jaguar1: Y
     *   Serval : Y (TBD)
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
     * Enable FDMA driver.
     *
     * The FDMA driver can be started and stopped with the use of this member.
     *
     * If the FDMA driver has once been enabled, it can be paused by setting #enable
     * to FALSE.
     */
    BOOL enable;

    /**
     * Rx MTU.
     * The maximum frame length (including FCS) the FDMA will pass on to the application.
     * Typically, an application will set this to 1518 + (4 * max number of VLAN tags).
     *
     * Well, in fact, it indicates the number of frame data bytes associated with
     * one DCB. See also #rx_allow_multiple_dcbs.
     *
     * It can be set to 0 to free all memory allocated by the FDMA driver, but if
     * set to a non-zero value, it must be at or greater than 64.
     */
    u32 rx_mtu;

    /**
     * Rx buffer count.
     * The number of Rx buffers to allocate.
     * Since the FDMA consists of a number of one or more Rx channels,
     * the allocated buffers will be spread evenly across the Rx channels
     * required on a given platform.
     */
    u32 rx_buf_cnt;

    /**
     * The FDMA driver allocates its own software DCBs, to obtain correct
     * alignment of the associated hardware DCBs. It uses VTSS_OS_MALLOC()
     * for this. DCBs are therefore owned by the FDMA driver.
     *
     * However, the associated data area, where frame data gets received into,
     * can either be owned by the FDMA driver or the application.
     *
     * If the application wishes to manage the frame data memory, it must
     * set #rx_alloc_cb() to a non-NULL value.
     * If the application leaves all the frame data memory management to the
     * FDMA driver, it must set #rx_alloc_cb() to NULL.
     *
     * During initialization, the FDMA driver allocates #rx_buf_cnt DCBs
     * and checks whether the application will allocate the corresponding
     * frame data, or it should do that itself. If #rx_alloc_cb() is NULL,
     * the FDMA driver will call VTSS_OS_MALLOC() to allocate the corresponding
     * frame data. Otherwise it will call #rx_alloc_cb() to get it allocated.
     *
     * DCBs are the glue between between the FDMA driver and the application,
     * in the sense that once the FDMA has received a frame, it passes a
     * pointer to the DCB to the application's rx_cb() callback. At this point
     * the application has three options:
     * 1) Pass the DCB further up the food chain to higher parts of the
     *    application, and once it has been handled, feed it back to the FDMA
     *    driver with a call to vtss_fdma_dcb_release().
     *    In this case, the rx_cb() function must return NULL.
     * 2) Detach the frame data from the DCB, set the DCB's alloc_ptr to NULL,
     *    and return the DCB back to the FDMA driver through the return value of
     *    rx_cb().
     * 3) If - for some reason - the application chooses not to pass the frame
     *    further up the food chain in the call to rx_cb(), it may simply return
     *    the DCB as is from rx_cb(), without altering the alloc_ptr member.
     *
     * Whether or not the DCB is returned directly as a return value from rx_cb()
     * or returned through a call to vtss_fdma_dcb_release(), the alloc_ptr
     * member of the DCB indicates whether the frame data has been absorbed by the
     * application of not, as follows:
     * A) If alloc_ptr == NULL when the DCB comes back, the FDMA assumes that the
     *    frame data has been absorbed by the application. In that case, it
     *    will ask the application to re-allocate the alloc_ptr member by calling
     *    #rx_alloc_cb() (which must therefore be non-NULL).
     * B) If alloc_ptr is non-NULL when the DCB comes back to the FDMA driver,
     *    the FDMA driver will just re-initialize the DCB and not call the
     *    #rx_alloc_cb() function.
     *
     * rx_alloc_cb() takes three arguments ([IN] params are seen from the
     * application's point of view.
     * \param sz   [IN]      Number of bytes to allocate.
     * \param list [INOUT]   A NULL-terminated list of DCBs to get allocated
     *                       frame data for. It may consist of more than one
     *                       DCB if the application has chosen to support
     *                       reception of frames fragmented over multiple DCBs
     *                       (#rx_allow_multiple_dcbs) and during initialization.
     *                       The application must fill in the alloc_ptr and possibly
     *                       also the "user" members of the DCB.
     * \param mem_flags [IN] This is a bitwise OR of the vtss_mem_flags_t,
     *                       enumeration, which tells the application how to
     *                       allocate the memory.
     *
     * If - upon return from the rx_alloc_cb() function, the DCB's alloc_ptr member
     * is still NULL, one of two things will happen:
     *  i) If it happens during initialization (vtss_fdma_cfg()), the function
     *     call will fail, and the FDMA will not be started.
     * ii) If it happens at runtime (rx_cb()/vtss_fdma_dcb_release()), the DCB
     *     will be put in a special list that can only be released if the FDMA
     *     driver gets restarted.
     *
     * The rx_alloc_cb() will be invoked for every DCB it should supply frame
     * data for, so don't use the ->next member.
     */
    void (*rx_alloc_cb)(u32 sz, vtss_fdma_list_t *const list, u32 mem_flags);

    /**
     * Don't strip VLAN tag.
     *
     * The switch hardware does not strip VLAN tags from the frames prior
     * to received by software.
     *
     * The FDMA driver can be configured to strip VLAN tags from frames received
     * on ports where the VLAN tag matches the port's VLAN tag setup. This is useful
     * for, e.g. IP stacks that don't expect VLAN tags inside the frames.
     * The recommendation is to set this to FALSE.
     */
    BOOL rx_dont_strip_vlan_tag;

    /**
     * Don't re-insert VLAN tag.
     *
     * This is obsolete and is ignored by the FDMA code.
     */
    BOOL rx_dont_reinsert_vlan_tag;

    /**
     * If a frame is received with a VLAN tag TPID that does not match the port's setup,
     * it can be dropped (DCB gets recycled) by the FDMA driver, by setting
     * #rx_allow_vlan_tag_mismatch to FALSE.
     */
    BOOL rx_allow_vlan_tag_mismatch;

    /**
     * Rather than indicating the maximum frame size, #rx_mtu indicates the maximum
     * number of frame data bytes that can go into one DCB's data area.
     * If the application can handle a frame spanning more than one DCB, it can set
     * #rx_allow_multiple_dcbs to TRUE, in which case the rx_cb() callback function
     * may be invoked with a list of DCBs (the SOF DCB's next pointer is non-NULL).
     *
     * In this way, the application may support jumbo frames without having to allocate
     * jumbo frame size bytes to each DCB.
     */
    BOOL rx_allow_multiple_dcbs;

    /**
     * The address of the callback function invoked by the FDMA driver code
     * when a frame arrives. The callback function is invoked once per frame.
     *
     * The function is invoked in the same context as the application invoked
     * vtss_fdma_irq_handler().
     *
     * The parameters to the callback function are as follows:\n
     *   -# cntxt: The cntxt passed to vtss_fdma_irq_handler().
     *   -# list:  NULL-terminated list of software DCBs making up the frame. Depending
     *             on the frame size one or more DCBs are used. The last is always
     *             NULL-terminated. The FDMA has filled in the following fields (see
     *             definition of vtss_fdma_list_t structure for deatils):
     *               (\@dcb), \@*data, \@act_len, (\@next).
     *             Once delivered to the Packet Module, the FDMA will not use the
     *             list again. The FDMA can, however, be handed back a (new) list of
     *             software DCBs by returning the list when the rx_cb() returns.
     *             Alternatively, use the vtss_fdma_dcb_release() function.
     *   - Expected return value from rx_cb():\n
     *             See discussion under #rx_alloc_cb.
     */
    vtss_fdma_list_t *(*rx_cb)(void *cntxt, vtss_fdma_list_t *list);

    /**
     * Tx buffer count.
     * The number of Tx DCBs to allocate.
     */
    u32 tx_buf_cnt;

    /**
     * The address of the callback function invoked by the FDMA driver code
     * when a frame has been transmitted. The callback function is invoked once per frame.
     *
     * The function is invoked in the same context as the application invoked
     * vtss_fdma_irq_handler().
     *
     * The parameters to the callback function are as follows:\n
     *   -# cntxt: The cntxt passed to vtss_fdma_irq_handler().
     *   -# list:  NULL-terminated list of software DCBs making up the frame.
     *             The last is always NULL-terminated.
     *   -# rc:    Return code. VTSS_RC_OK on success, otherwise failed to transmit.
     *
     * The application must extract the required information from the DCB list (e.g. through
     * the DCB's user field) prior to returning from this function, because the FDMA driver
     * will recycle the DCBs upon return.
     *
     * In the event that the DCBs were originally coming from extraction, the FDMA driver
     * returns the DCBs back to extxraction.
     */
    void (*tx_done_cb)(void *cntxt, struct tag_vtss_fdma_list *list, vtss_rc rc);

#if defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC)
    /**
     * AFI buffer count.
     * The number of AFI DCBs to allocate.
     */
    u32 afi_buf_cnt;

    /**
     * The address of the callback function invoked by the FDMA driver code
     * when an AFI frame has been cancelled. The callback function is invoked once per frame.
     *
     * The function is invoked in the same context as the application invoked
     * vtss_fdma_irq_handler().
     *
     * The parameters to the callback function are as follows:\n
     *   -# cntxt: The cntxt passed to vtss_fdma_irq_handler().
     *   -# list:  NULL-terminated list of software DCBs making up the frame.
     *             The last is always NULL-terminated.
     *   -# rc:    Return code. VTSS_RC_OK always.
     *
     * The application must extract the required information from the DCB list (e.g. through
     * the DCB's user field) prior to returning from this function, because the FDMA driver
     * will recycle the DCBs upon return.
     *
     */
    void (*afi_done_cb)(void *cntxt, struct tag_vtss_fdma_list *list, vtss_rc rc);
#endif /* defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC) */
#else /* VTSS_OPT_FDMA_VER */
#if defined(VTSS_FEATURE_VSTAX)
    /**
     * <B>XTR/INJ:</B>\n
     *   One-hot mask, where the bit set indicates the physical
     *   stack port A. More than one bit may be set if the stack
     *   port is part of an aggregation.
     *   OBSOLETE. NOT USED ANYMORE.
     *
     * Validity:
     *   Luton26: N
     *   Jaguar1: Y
     *   Serval : N
     */
    u64 stack_port_a_mask;

    /**
     * <B>XTR/INJ:</B>\n
     *   One-hot mask, where the bit set indicates the physical
     *   stack port B. More than one bit may be set if the stack
     *   port is part of an aggregation.
     *   OBSOLETE. NOT USED ANYMORE
     *
     * Validity:
     *   Luton26: N
     *   Jaguar1: Y
     *   Serval : N
     */
    u64 stack_port_b_mask;

    /**
     * <B>INJ:</B>\n
     *   In dual-chip solutions we need to know the chip on which
     *   stack port A is located, i.e. which chip stack_port_a_mask
     *   pertains to.
     *   OBSOLETE. NOT USED ANYMORE
     *
     * Validity:
     *   Luton26: N
     *   Jaguar1: Y
     *   Serval : N
     */
    vtss_chip_no_t stack_port_a_chip;

    /**
     * <B>INJ:</B>\n
     *   In dual-chip solutions we need to know the chip on which
     *   stack port B is located, i.e. which chip stack_port_b_mask
     *   pertains to.
     *   OBSOLETE. NOT USED ANYMORE
     *
     * Validity:
     *   Luton26: N
     *   Jaguar1: Y
     *   Serval : N
     */
    vtss_chip_no_t stack_port_b_chip;
#else
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
     *   Jaguar1: N
     *   Serval : N
     */
    u32 dummy;
#endif /* defined(VTSS_FEATURE_VSTAX) */
} vtss_fdma_cfg_t;

/**
 * In order to survive e.g. broadcast storms, the FDMA driver incorporates
 * a poor man's policing/throttling scheme.
 *
 * The idea is to check upon every frame reception whether the CPU Rx queue
 * on which the frame was received has exceeded its limit, and if so,
 * suspend extraction from the queue for a period of time.
 *
 * The FDMA has no notion of time, so this requires a little help from
 * the application.
 * To take advantage of the feature, the application must first call
 * vtss_fdma_throttle_cfg_set() with an appropriate configuration, and
 * then call vtss_fdma_throttle_tick() on a regular, application-defined basis,
 * e.g. 10 times per second.
 *
 * The throttle tick takes care of re-opening the queue after the suspension
 * period elapses.
 *
 \endif
 \if VTSS_OPT_FDMA_VER >= 3
 * This feature is controlled per extraction queue, and if
 * vtss_fdma_throttle_cfg_t::frm_limit_per_tick[xtr_qu] is 0 and
 * vtss_fdma_throttle_Cfg_t::byte_limit_per_tick[xtr_qu] is 0 this
 * feature is disabled for that queue.
 *
 * If either vtss_fdma_throttle_cfg_t::frm_limit_per_tick[xtr_qu] or
 * vtss_fdma_throttle_cfg_t::byte_limit_per_tick[xtr_qu] is non-zero,
 * the feature is enabled, and in case a queue gets suspended, it will be
 * suspended for the remainder of the tick period PLUS whatever is
 * specified with vtss_fdma_throttle_cfg_t::suspend_tick_cnt.
 \endif
 *
 * Notice that once an extraction queue gets disabled, that extraction queue
 * will no longer be a source of interrupts. The feature will only affect
 * extraction queues for which it is enabled.
 *
 * Once disabling an extraction queue, the remaining frames in the queue
 * will be read out, after which the queue will be silent. When re-enabling, it
 * will be fresh frames that come in.
 *
 * Be aware that on Lu26, the trick to disable a queue involves directing the
 * frames to the second CPU port (physical #27). If your application uses two CPU ports,
 * then throttling will have unexpected side-effects.
 *
 * There is no need to call vtss_fdma_throttle_tick() unless throttling is
 * enabled for at least one extraction queue.
 *
 * If throttling is enabled for at least one queue, and vtss_fdma_throttle_tick()
 * is *not* called, you risk that an extraction queue will get disabled and
 * never re-enabled again.
 *
 * Validity:
 *   Luton26: Y
 *   Jaguar1: Y
 *   Serval1: Y
 */
typedef struct {

    /**
     * Controls - per extraction queue - the maximum number of frames
     * extracted between two calls to vtss_fdma_throttle_tick() without
     * suspending extraction from that queue.
     *
     * If 0, frame count throttling is disabled for that extraction queue.
     */
    u32 frm_limit_per_tick[VTSS_PACKET_RX_QUEUE_CNT];

    /**
     * Controls - per extraction queue - the maximum number of bytes
     * extracted between two calls to vtss_fdma_throttle_tick() without
     * suspending extraction from that queue.
     *
     * If 0, byte count throttling is disabled for that extraction queue.
     */
    u32 byte_limit_per_tick[VTSS_PACKET_RX_QUEUE_CNT];

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


/**
 * \brief Uninitialize FDMA.
 *
 * Rarely used. Use only if you want to make a controlled shut-down of the
 * FDMA. Disables all FDMA channels and interrupts, and clears pending
 * interrupts.
 *
 * - Call context:\n
 *   Thread
 *
 * - Re-entrant:\n
 *   No
 *
 * \param inst [IN]: Target instance reference.
 *
 * \return
 *   VTSS_RC_OK on success.\n
 *   VTSS_RC_ERROR if a supplied parameter was erroneous.
 */
vtss_rc vtss_fdma_uninit(const vtss_inst_t inst);

/**
 * \brief Configure FDMA.
 *
 * Call this function before any other FDMA API function.
 *
 * - Call context:\n
 *   Thread
 *
 * - Re-entrant:\n
 *   No
 *
 * \param inst [IN]: Target instance reference.
 * \param cfg  [IN]: FDMA configuration.
 *
 * \return
 *   VTSS_RC_OK on success.\n
 *   VTSS_RC_ERROR if a supplied parameter was erroneous.
 */
vtss_rc vtss_fdma_cfg(const vtss_inst_t inst, const vtss_fdma_cfg_t *const cfg);


/**
 * \brief Release DCBs.
 *
 * This function returns DCBs (injection, extraction, or AFI) back to the
 * FDMA driver.
 *
 * This is useful in these situations:
 *   Extraction:
 *      If frame data memory is completely managed by the FDMA
 *      driver (rx_alloc_cb() == NULL), then the application may
 *      choose to pass the frame as is to higher levels of software,
 *      i.e. with zero-copy. Once it is handled, the DCBs must be
 *      returned to the FDMA driver with a call to this function.
 *      If frame data memory is managed by the application (
 *      rx_alloc_cb() != NULL), then the application should return
 *      the list of DCBs it was invoked with in the rx_cb() callback
 *      handler (with the alloc_ptr set to NULL if the frame itself
 *      is passed to higher levels of software).
 *
 *   Injection (both normal and AFI):
 *      This is rarely useful, and the only situation where it makes
 *      sense to return injection DCBs to the FDMA driver is when
 *      it has allocated the DCBs (using vtss_fdma_dcb_get()) and
 *      then decides not to use them in a call to vtss_fdma_inj()
 *      afterwards.
 *
 * - Call context:\n
 *   Thread
 *
 * - Re-entrant:\n
 *   Yes
 *
 * \param inst [IN]: Target instance reference.
 * \param list [IN]: The list of DCBs to be returned to the application.
 *
 * \return
 *   VTSS_RC_OK on success.\n
 *   VTSS_RC_ERROR if a supplied parameter was erroneous.
 */
vtss_rc vtss_fdma_dcb_release(const vtss_inst_t      inst,
                                    vtss_fdma_list_t *const list);

/**
 * \brief Inject a frame.
 *
 * This function takes a NULL-terminated list of software DCBs making up one
 * frame and injects it using the tx info properties given by props.
 *
 * The DCBs must be obtained using the vtss_fdma_dcb_get() call.
 *
 * Once the frame is injected, the configured tx_done_cb() function will be
 * called. Once this function returns, the DCBs that the callback function
 * was invoked with are no longer available to the application.
 *
 * Upon invokation of tx_done_cb(), the application can read additional data
 * from the frame's SOF DCB. The fields filled in by the FDMA driver are
 *   sw_tstamp, afi_frm_cnt, and afi_seq_number.
 *
 * Upon tx_done_cb() the FDMA driver may have modified the actual frame data, and
 * thereby the frm_ptr that was set during the call to vtss_fdma_inj().
 *
 * - Call context:\n
 *   Thread
 *
 * - Re-entrant:\n
 *   Yes
 *
 * \param inst [IN]:      Target instance reference.
 * \param list [IN]:      The list of software DCBs making up the frame. Only the frm_ptr
 *                        and act_len members must be filled in. The user member is useful for
 *                        additional data required by the application to identify the frame that
 *                        is being injected upon the tx_done_cb() callback.
 * \param fdma_info [IN]: Info specifically for use by the FDMA driver. The structure may
 *                        be allocated on the stack, since vtss_fdma_inj() doesn't keep
 *                        a pointer to it after the call returns. Initialize with a call to
 *                        vtss_fdma_tx_info_init() prior to filling it in.
 * \param tx_info   [IN]: Pointer to a structure that contains properties on how to inject
 *                        the frame. Must be initialized with a call to vtss_packet_tx_info_init()
 *                        before assigning it. The structure may be allocated on the stack, since vtss_fdma_inj()
 *                        doesn't keep a pointer to it after the call returns.
 *
 * \return
 *   VTSS_RC_OK on success.\n
 *   VTSS_RC_ERROR on any kind of error. In this case, the DCBs are already returned
 *   to the FDMA driver.
 */
vtss_rc vtss_fdma_tx(const vtss_inst_t           inst,
                           vtss_fdma_list_t      *list,
                           vtss_fdma_tx_info_t   *const fdma_info,
                           vtss_packet_tx_info_t *const tx_info);

/** \brief Initialize a vtss_fdma_tx_info_t structure
 *
 * \param inst      [IN]: Target instance
 * \param fdma_info [IN]: Structure to initialize
 *
 * \return VTSS_RC_OK unless fdma_info == NULL.
 */
vtss_rc vtss_fdma_tx_info_init(const vtss_inst_t inst,
                                     vtss_fdma_tx_info_t *const fdma_info);

#if (defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC))
/** \brief Cancel an ongoing AFI transmission.
 *
 * The frame is identified by a pointer to the frame
 * previously used in list->frm_ptr in the call to
 * vtss_fdma_tx().
 *
 * Once the cancellation has occurred, the
 * afi_tx_done callback will be called.
 *
 * \param inst    [IN]: Target instance
 * \param frm_ptr [IN]: Frame pointer.
 *
 * \return VTSS_RC_ERROR if no such frame was found. VTSS_RC_OK
 * if cancelled successfully.
 */
vtss_rc vtss_fdma_afi_cancel(const vtss_inst_t inst, const u8 *const frm_ptr);
#endif /* (defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC))  */

#if defined(VTSS_FEATURE_AFI_FDMA)
/** \brief Get an interim frame count for a given periodically injected frame.
 *
 * The frame is identified by a pointer to the frame
 * previously used in list->frm_ptr in the call to
 * vtss_fdma_tx(). To get a count != 0, counting must
 * be enabled with fdma_info.afi_enable_counting.
 *
 * \param inst    [IN]: Target instance
 * \param frm_ptr [IN]: Frame pointer.
 * \param frm_cnt [OUT]: Frame counter.
 *
 * \return VTSS_RC_ERROR if no such frame was found. VTSS_RC_OK
 * if successful.
 */
vtss_rc vtss_fdma_afi_frm_cnt(const vtss_inst_t inst, const u8 *const frm_ptr, u64 *const frm_cnt);
#endif /* defined(VTSS_FEATURE_AFI_FDMA) */

/**
 * \brief DCB type identifying a DCB
 */
typedef enum {
    VTSS_FDMA_DCB_TYPE_XTR, /**< The DCB is an extraction DCB. Not needed by application.            */
    VTSS_FDMA_DCB_TYPE_INJ, /**< The DCB is an injection DCB. Needed in call to vtss_fdma_dcb_get(). */
#if defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC)
    VTSS_FDMA_DCB_TYPE_AFI, /**< The DCB is an AFI DCB. Needed in call to vtss_fdma_dcb_get().       */
#endif /* defined(VTSS_FEATURE_AFI_FDMA) || defined(VTSS_FEATURE_AFI_SWC) */
} vtss_fdma_dcb_type_t;

/**
 * \brief Get one or more DCBs suitable for frame injection.
 *
 * The FDMA API is the owner/maintainer of all DCBs. In order to be able to inject
 * a frame, the application must request DCBs from the FDMA driver and fill in
 * appropriate fields (see description under vtss_fdma_inj()) prior to calling
 * vtss_fdma_inj().
 *
 * One or more DCBs may be requested in one go.
 * If the application regrets that it has requested the DCBs, it may call
 * vtss_fdma_dcbs_release() to hand them back to the FDMA driver.
 *
 * If more than one DCB is requested, the FDMA driver will hook them together
 * with the DCBs' next field. The last DCB's next field will be set to NULL
 * by the FDMA driver.
 *
 * Special DCBs are required for AFI frames, so in addition to the number of
 * DCBs, also a dcb_type parameter must be provided. The dcb_type must only be
 * set to VTSS_FDMA_DCB_TYPE_INJ or VTSS_FDMA_DCB_TYPE_AFI.
 *
 * If the requested number of DCBs are not available of the specified type,
 * the function returns VTSS_RC_INCOMPLETE. It is up to the application to
 * implement the logic that can facilitate waiting for DCBs, if it is required
 * that a given frame must be transmitted. It could, e.g. wait for a semaphore
 * in the thread that calls vtss_fdma_inj() and signal that semaphore whenever
 * the tx_done_cb() gets invoked.
 *
 * - Call context:\n
 *   Thread
 *
 * - Re-entrant:\n
 *   Yes
 *
 * \param inst     [IN]:  Target instance reference.
 * \param dcb_cnt  [IN]:  Number of DCBs.
 * \param dcb_type [IN]:  DCB type requested.
 * \param list     [OUT]: Pointer receiving a pointer to the first DCB.
 *
 * \return
 *   VTSS_RC_OK on success.\n
 *   VTSS_RC_ERROR if a supplied parameter was erroneous.\n
 *   VTSS_RC_INCOMPLETE if dcb_cnt DCBs aren't available.
 */
vtss_rc vtss_fdma_dcb_get(const vtss_inst_t          inst,
                                u32                  dcb_cnt,
                                vtss_fdma_dcb_type_t dcb_type,
                                vtss_fdma_list_t     **list);


/**
 * \brief Get current throttle configuration.
 *
 * Returns the current throttling configuration.
 *
 * - Call context:\n
 *   Any
 *
 * - Re-entrant:\n
 *   Yes, but doesn't really make sense.
 *
 * \param inst [IN]:  Target instance reference.
 * \param cfg  [OUT]: Pointer to structure that receives the current throttling configuration.
 *
 * \return
 *   VTSS_RC_OK on success, VTSS_RC_ERROR if channel indicated by ch
 *   is not configured for extraction.
 */
vtss_rc vtss_fdma_throttle_cfg_get(const vtss_inst_t inst, vtss_fdma_throttle_cfg_t *const cfg);

/**
 * \brief Configure throttling
 *
 * Configure throttling.
 * See vtss_fdma_throttle_cfg_t for a description of the throttle feature
 * and the use of this function.
 *
 * - Call context:\n
 *   Any
 *
 * - Re-entrant:\n
 *   Yes, but doesn't really make sense.
 *
 * \param inst [IN]: Target instance reference.
 * \param cfg  [IN]: New throttling configuration.
 *
 * \return
 *   VTSS_RC_OK on success, VTSS_RC_ERROR if channel indicated by ch
 *   is not configured for extraction.
 */
vtss_rc vtss_fdma_throttle_cfg_set(const vtss_inst_t inst, const vtss_fdma_throttle_cfg_t *const cfg);

/**
 * \brief Generate throttle tick.
 *
 * See vtss_fdma_throttle_cfg_t for a description of the throttle feature
 * and the use of this function.
 *
 * - Call context:\n
 *   Any
 *
 * - Re-entrant:\n
 *   Yes, but doesn't really make sense to call this function from more than one thread.
 *
 * \param inst [IN]: Target instance reference.
 *
 * \return
 *   VTSS_RC_OK - always.
 */
vtss_rc vtss_fdma_throttle_tick(const vtss_inst_t inst);





/**
 * \brief Clear FDMA statistics.
 *
 * - Call context:\n
 *   Thread
 *
 * - Re-entrant:\n
 *   Yes
 *
 * \param inst [IN]: Target instance reference.
 *
 * \return
 *   VTSS_RC_OK on success.\n
 *   VTSS_RC_ERROR if supplied parameter was erroneous.
 */
vtss_rc vtss_fdma_stats_clr(const vtss_inst_t inst);

/**
 * \brief FDMA Interrupt Handler.
 *
 * This function is the heart-beat of all FDMA silicon communication.
 * The driver code enables interrupts on the FDMA silicon and expects this
 * function to be called whenver the FDMA generates interrupts.
 * The function is not re-entrant, and it must *not* be interrupted by
 * other non-interrupt code. This function does not call any of the
 * VTSS_OS_INTERRUPT_DISABLE() or VTSS_OS_SCHEDULER_LOCK() macros.
 *
 * The function first checks for extraction channels interrupting and
 * calls back the relevant xtr_cb() functions with one frame at a time.
 * The function calls back for every frame that the FDMA has extxracted
 * since last call.
 * Then it checks for injection completion, takes care of retransmission
 * if that is needed, and calls back the relevant inj_post_cb() functions
 * for every frame that has been injected.
 *
 * This means that both xtr_cb() and inj_post_cb() callback functions are called
 * from the same context as vtss_fdma_irq_handler() is called. BEWARE!!
 *
 * To better understand what is required by the caller of this function,
 * let's branch on the two basic options:
 *
 * - Interrupt Driven Operation:\n
 *   If interrupts are supported, it is important that the FDMA interrupt
 *   is disabled by the "top half" or ISR if it finds out that the
 *   interrupt is for the FDMA. If the OS supports top and bottom halves
 *   like Linux (a.k.a. ISRs and DSRs in eCos), it is recommended to call
 *   vtss_fdma_irq_handler() in the bottom half/DSR rather than in the
 *   top half/ISR. In the bottom half/DSR case, it is ensured that no other
 *   bottom halves/DSRs or threads can preempt the handler. Once the handler
 *   returns, the caller should clear and re-enable top-level FDMA interrupts.
 *   In this scenario VTSS_OPT_FDMA_IRQ_CONTEXT should be defined to 0,
 *   causing the thread code (all other vtss_fdma_XXX() functions) to use the
 *   VTSS_OS_SCHEDULER_LOCK()/UNLOCK() functions.
 *
 *   If the OS doesn't support top/bottom halves, but still supports
 *   interrupt handling, vtss_fdma_irq_handler() may be called directly
 *   from the ISR, which should still start by disabling top-level FDMA interrupts,
 *   then call the handler, and finally clear and re-enable them. In this scendario
 *   VTSS_OPT_FDMA_IRQ_CONTEXT be defined to 1. This causes the thread code
 *   (all other vtss_fdma_XXX() functions) to use the
 *   VTSS_OS_INTERRUPT_DISABLE()/RESTORE() functions whenever it is using or
 *   updating state members also used by the interrupt handler.
 *
 * - Polled Operation:\n
 *   Since the FDMA driver code reads registers directly in the FDMA silicon
 *   to figure out which channels are interrupting, and since it allows for
 *   "no channels want the CPU's attention", it is possible to use
 *   a polled approach rather than an interrupt driven approach. In a single-
 *   threaded application you can call it once in the round-robin trip,
 *   because it cannot be interrupted by any other code. In a multi-threaded
 *   application you will have to disable the scheduler before calling it,
 *   and re-enable it afterwards. This ensures that no other functions
 *   can call e.g. vtss_fdma_inj() while servicing the "interrupt". It is
 *   not good enough to disable interrupts, because the code calls macros
 *   like "output this debug message to the console", which may end up in
 *   the OS kernel, which in turn may schedule the vtss_fdma_irq_handler()
 *   function out in favor of another thread.
 *
 * - Call context:\n
 *   OS-Specific. See description below.
 *
 * - Re-entrant:\n
 *   <B>NO!!!</B>
 *
 * \param inst [IN]:  Target instance reference.
 * \param cntxt [IN]: A user-defined argument, which is passed to the inj_post_cb()
 *                    and xtr_cb() callback functions. Rarely needed.
 *
 * \return
 *   VTSS_RC_OK on success.\n
 *   VTSS_RC_ERROR if \@inst parameter was erroneous.
 */
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
