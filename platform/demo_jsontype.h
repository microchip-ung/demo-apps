/*

 Vitesse Switch API software.

 Copyright (c) 2002-2014 Vitesse Semiconductor Corporation "Vitesse". All
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


#ifndef _VTSS_DEMO_JSONTYPE_H_
#define _VTSS_DEMO_JSONTYPE_H_

#include <vtss/api/types.h>
#include <vtss/appl/interface.h>
#include <vtss/basics/enum-descriptor.h>

//#ifdef __cplusplus
//extern "C" {
//#endif


  // (1) --- Some type struct
typedef struct {
  u32  SomeNumber;
  i8 SomeString[20];
} demo_json_a_type_t;


// (2) --- The get function for type in (1).
vtss_rc demo_json_a_type_get(
        demo_json_a_type_t  *const v);


  // (3) --- The set function for type in (1).
vtss_rc demo_json_a_type_set(
        const demo_json_a_type_t  *const v);


//#ifdef __cplusplus
//}
//#endif
#endif  // _VTSS_DEMO_JSONTYPE_H_
