#include "demo_jsontype.h"
#include <string.h>

static demo_json_a_type_t x={1,"xxx"};

vtss_rc demo_json_a_type_get(demo_json_a_type_t  *const v)
{
  printf("get %d %s\n", v->SomeNumber, v->SomeString);

  v->SomeNumber = x.SomeNumber;
  memcpy(v->SomeString, x.SomeString, 20);

  return 0;
}


  // (3) --- The set function for type in (1).
vtss_rc demo_json_a_type_set(const demo_json_a_type_t  *const v)
{
  printf("set %d %s\n", v->SomeNumber, v->SomeString);

  x.SomeNumber = v->SomeNumber;
  memcpy(x.SomeString, v->SomeString, 20);

  return 0;
}

