
#include "argc/types.h"
#include "argc/functions.h"

using namespace argennon;
using namespace ascee;
using namespace argc;

DEF_ARGC_DISPATCHER {
    STRING(dummy, "dummy");
    STRING_BUFFER(all, 2048);

    // time out
    append_str(response, dummy);
    invoke_dispatcher(40, 10, response, request);
    append_str(all, response);

    // division zero
    append_str(response, dummy);
    invoke_dispatcher(40, 20, response, request);
    append_str(all, response);

    // self call: App not found
    append_str(response, dummy);
    invoke_dispatcher(40, 19, response, request);
    append_str(all, response);

    // App not declared
    append_str(response, dummy);
    invoke_dispatcher(40, 11, response, request);
    append_str(all, response);

    // segmentation fault
    invoke_dispatcher(40, 21, response, request);
    append_str(all, response);

    clear_buffer(response);
    append_str(response, all);
    STRING(end, " all called...");
    append_str(response, end);
    return HTTP_OK;
}
