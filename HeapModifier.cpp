//
// Created by aybehrouz on 9/22/21.
//

#include <stdio.h>
#include "HeapModifier.h"

int64 HeapModifier::loadInt64(int32 offset) {
    printf("\n%d-->%d\n", context, offset);
}

void HeapModifier::changeContext(std_id_t appID) {

}

void HeapModifier::saveCheckPoint() {

}

void HeapModifier::RestoreCheckPoint() {

}

void HeapModifier::DiscardCheckPoint() {

}
