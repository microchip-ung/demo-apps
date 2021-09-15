/*

 Vitesse Switch Software.

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
#ifndef _DEMO_SERIALIZER_HXX_
#define _DEMO_SERIALIZER_HXX_

#include <vtss/appl/module_id.h>
#include "demo_jsontype.h"
#include "vtss_appl_serialize.hxx"


template <typename T>
void serialize(T &a, demo_json_a_type_t &p)
{
  typename T::Map_t m =
    a.as_map(vtss::tag::Typename("demo_json_a_type_t"));


#ifdef DEMO_LINUX
  //
  // --- Namespace has been change slightly for Linux version
  //
  m.add_leaf(vtss::AsInt(p.SomeNumber),
             vtss::tag::Name("SomeInterestingNumber"),
             vtss::expose::snmp::Status::Current,
             vtss::expose::snmp::OidElementValue(1),
             vtss::expose::snmp::RangeSpec<u32>(2, 5),
             vtss::tag::Description("Example of a number."));

  m.add_leaf(vtss::AsDisplayString(p.SomeString, sizeof(p.SomeString)),
             vtss::tag::Name("SomeCoolString"),
             vtss::expose::snmp::Status::Current,
             vtss::expose::snmp::OidElementValue(2),
             vtss::tag::Description("Example of a DisplayString."));

#else

  m.add_leaf(vtss::AsInt(p.SomeNumber),
             vtss::tag::Name("SomeInterestingNumber"),
             vtss::snmp::Status::Current,
             vtss::snmp::OidElementValue(1),
             vtss::snmp::RangeSpec<u32>(2, 5),
             vtss::tag::Description("Example of a number."));

  m.add_leaf(vtss::AsDisplayString(p.SomeString, sizeof(p.SomeString)),
             vtss::tag::Name("SomeCoolString"),
             vtss::snmp::Status::Current,
             vtss::snmp::OidElementValue(2),
             vtss::tag::Description("Example of a DisplayString."));

#endif
}

namespace vtss { namespace demo {

struct DemoConfig_t {
    typedef expose::ParamList<expose::ParamVal<demo_json_a_type_t *>>  P;

    VTSS_EXPOSE_SERIALIZE_ARG_1(demo_json_a_type_t &i)
    {
        h.argument_properties(tag::Name("config"));
        serialize(h, i);
    }

    VTSS_EXPOSE_GET_PTR(demo_json_a_type_get);
    VTSS_EXPOSE_SET_PTR(demo_json_a_type_set);
};


// end the namespaces
} }


#endif
