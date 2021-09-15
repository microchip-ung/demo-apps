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

#include <vtss_trace_api.h>
#include "demo_trace.h"


// --- For details on trace see vtss_appl/util/vtss_trace_api.h
//
//     # platform debug allow // Only need once, and enable the 'debug' command.
//     # debug trace module level demo
//     Module      Group       Level    Timestamp  Usec  Ring Buf  IRQ  Description
//     ----------  ----------  -------  ---------  ----  --------  ---  -----------
//     demo        default     warning  yes        no    no        no   Demo module (platform)
//                 platform    warning  yes        no    no        no   Platform calls
//                 packet      warning  yes        no    no        no   Packet calls
//                 msg         warning  yes        no    no        no   Msg calls
//                 l2proto     warning  yes        no    no        no   l2proto calls
//                 forward     warning  yes        no    no        no   forward control calls
//                 crit        warning  yes        no    no        no   Critical regions
//                 web         warning  yes        no    no        no   Web application
//
//     To change the level of group 'forward' to 'debug' do:
//
//     # debug trace module level demo forward debug
//
//     Then T_DG(TRACE_GRP_FORWARD, "Something"); will be printed
//
//     E.g. in the ICLI do:
//     # configure terminal
//     (config)# demo
//     D demo/forward 00:13:33 11/debug_forwarding_control#45: demo_forwarding_control enable=1
//     (config)#
//
//
// --- The lists below define a trace context called 'demo', which
//     has a number of groups like 'default', platform', packet', etc.
//
//     Compare these lists with the output in the comment above.
//
#if (VTSS_TRACE_ENABLED)

static vtss_trace_reg_t trace_reg = {
    VTSS_TRACE_MODULE_ID,
    "demo",
    "Demo module (platform)"
};

static vtss_trace_grp_t trace_grps[TRACE_GRP_CNT] = {
  /* VTSS_TRACE_GRP_DEFAULT */ {
        "default",
        "Default (demo platform)",
        VTSS_TRACE_LVL_WARNING,
        VTSS_TRACE_FLAGS_TIMESTAMP
    },
  /* [TRACE_GRP_PLATFORM] */ {
        "platform",
        "Platform calls",
        VTSS_TRACE_LVL_WARNING,
        VTSS_TRACE_FLAGS_TIMESTAMP
    },
  /* [TRACE_GRP_PACKET] */ {
        "packet",
        "Packet calls",
        VTSS_TRACE_LVL_WARNING,
        VTSS_TRACE_FLAGS_TIMESTAMP
    },
  /* [TRACE_GRP_MSG] */ {
        "msg",
        "Msg calls",
        VTSS_TRACE_LVL_WARNING,
        VTSS_TRACE_FLAGS_TIMESTAMP
    },
  /* [TRACE_GRP_L2PROTO] */ {
        "l2proto",
        "l2proto calls",
        VTSS_TRACE_LVL_WARNING,
        VTSS_TRACE_FLAGS_TIMESTAMP
    },
  /* [TRACE_GRP_FORWARD] */ {
        "forward",
        "forward control calls",
        VTSS_TRACE_LVL_WARNING,
        VTSS_TRACE_FLAGS_TIMESTAMP
    },
  /* [TRACE_GRP_CRIT] */ {
        "crit",
        "Critical regions",
        VTSS_TRACE_LVL_WARNING,
        VTSS_TRACE_FLAGS_TIMESTAMP
    },
  /* [TRACE_GRP_WEB] */ {
        "web",
        "Web application",
        VTSS_TRACE_LVL_WARNING,
        VTSS_TRACE_FLAGS_TIMESTAMP
    },
  /* [TRACE_GRP_TX] */ {
        "tx",
        "frame tx function",
        VTSS_TRACE_LVL_WARNING,
        VTSS_TRACE_FLAGS_TIMESTAMP
    },
  /* [TRACE_GRP_SOCKET] */ {
        "socket",
        "server - client",
        VTSS_TRACE_LVL_WARNING,
        VTSS_TRACE_FLAGS_TIMESTAMP
    },
};

// --- Install function for these traces. Called for demo.c::demo_init()
//
void demo_register_trace(void)
{
    VTSS_TRACE_REG_INIT(&trace_reg, trace_grps, TRACE_GRP_CNT);
    VTSS_TRACE_REGISTER(&trace_reg);
}

#else

void demo_register_trace(void)
{
}

#endif /* VTSS_TRACE_ENABLED */

