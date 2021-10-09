
#include "Heap.h"
// #define NDEBUG
#include <cassert>

#define MAX_VERSION 30000

using namespace ascee;

/*
uint8_t HeapModifier::loadByte(int32_t offset) {
    return accessTable.at(offset & ~0b111).heapLocation[offset & 0b111];
}*/

int16_t Heap::Modifier::saveVersion() {
    if (currentVersion == MAX_VERSION) throw std::out_of_range("version limit reached");
    return currentVersion++;
}

void Heap::Modifier::restoreVersion(int16_t version) {
    if (version > currentVersion || version < 0) throw std::runtime_error("restoring an invalid version");
    currentVersion = version;
}

void Heap::Modifier::loadChunk(short_id_t chunkID) {
    accessTable = chunks32.at(chunkID);
}

void Heap::Modifier::loadChunk(std_id_t chunkID) { accessTable = chunks64.at(chunkID); }

void Heap::Modifier::loadContext(std_id_t appID) {
    chunks32 = appsAccessMaps.at(appID).first;
    chunks64 = appsAccessMaps.at(appID).second;
}

void Heap::Modifier::defineAccessBlock(Pointer heapLocation,
                                       std_id_t app, short_id_t chunk, int32 offset,
                                       int32 size, bool writable) {
    appsAccessMaps[app].first[chunk].emplace(std::piecewise_construct,
                                             std::forward_as_tuple(offset),
                                             std::forward_as_tuple(heapLocation, size, writable));
}

void Heap::Modifier::AccessBlock::syncTo(int16_t version) {
    if (snapshotList.empty() || snapshotList.back()->version < version) return;

    while (snapshotList.back()->version > version) {
        delete snapshotList.back();
        snapshotList.pop_back();
    }

    // restore the snapshot data
    heapLocation.writeBlock(snapshotList.back()->content, size);

    if (snapshotList.back()->version == version) {
        delete snapshotList.back();
        snapshotList.pop_back();
    }
}

void Heap::Modifier::AccessBlock::updateTo(int16_t version) {
    // checks are ordered for having the best performance on average
    if (!snapshotList.empty()) {
        auto snapshotVersion = snapshotList.back()->version;
        assert(snapshotVersion <= version - 1);
        if (snapshotVersion == version - 1) return;
        if (snapshotVersion == -1) throw std::out_of_range("block is not writable");
    }

    if (version <= 0) return;

    snapshotList.push_back(new Snapshot(int16_t(version - 1), size));
    // read heap into the newly created snapshot
    heapLocation.readBlock(snapshotList.back()->content, size);
}

Heap::Modifier::AccessBlock::AccessBlock(Pointer heapLocation, int32 size, bool writable)
        : heapLocation(heapLocation), size(size) {
    if (!writable) {
        snapshotList.push_back(new Snapshot(-1, 0));
    }
}