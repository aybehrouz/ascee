#include <argc/functions.h>
#include "argc/types.h"


#include "stdio.h"

void foo() {
    foo();
}


int dispatcher(string_t request) {
    int ret = invoke_dispatcher(255, 1, request);
    /*  string_t req = STRING("Hi!");
      foo();
      return 0;*/
    // printf("woohoo called!!!\n");
    //while(1);
    //foo();
    //return invoke_dispatcher(250, 1, req);
};
