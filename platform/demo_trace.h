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

#ifndef __DEMO_TRACE_H__
#define __DEMO_TRACE_H__

#include <vtss_module_id.h>

// --- The VTSS_MODULE_ID_DEMO must be defined in mgmt/include/vtss_module_id.h.
//     Search for xxrp in that file to see what changes must be done.

#define VTSS_TRACE_MODULE_ID VTSS_MODULE_ID_DEMO
#define VTSS_TRACE_GRP_DEFAULT 0
#define TRACE_GRP_PLATFORM     1
#define TRACE_GRP_PACKET       2
#define TRACE_GRP_MSG          3
#define TRACE_GRP_L2PROTO      4
#define TRACE_GRP_FORWARD      5
#define TRACE_GRP_CRIT         6
#define TRACE_GRP_WEB          7
#define TRACE_GRP_TX           8
#define TRACE_GRP_SOCKET       9
#define TRACE_GRP_CNT         10


#endif
