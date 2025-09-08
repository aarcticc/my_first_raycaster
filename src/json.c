/* Local json.c to compile the bundled single-header json implementation.
   The full implementation is provided in include/json.h as weak inline
   definitions. Including it from a .c file will emit the symbols into the
   binary so we don't depend on an external libjson-c. */

#include "json.h"

/* Nothing else required; the header provides the implementation via
   json_weak definitions. */
