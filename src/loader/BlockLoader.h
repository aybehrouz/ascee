// Copyright (c) 2021 aybehrouz <behrouz_ayati@yahoo.com>. All rights reserved.
// This file is part of the C++ implementation of the Argennon smart contract
// Execution Environment (AscEE).
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU Affero General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License
// for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#ifndef ASCEE_BLOCK_LOADER_H
#define ASCEE_BLOCK_LOADER_H

#include <vector>
#include <string>
#include <unordered_set>
#include "argc/primitives.h"

namespace ascee::runtime {

struct AppRequestRawData {
    using IdType = int_fast32_t;
    IdType id;
    long_id calledAppID;
    std::string httpRequest;
    int_fast32_t gas;
    std::vector<long_id> appAccessList;
    std::unordered_set<int_fast32_t> stackSizeFailures;
    std::unordered_set<int_fast32_t> cpuTimeFailures;
    std::unordered_set<IdType> adjList;
    Digest headerDigest;
};

struct AccessBlock {
    int32 offset;
    int32 size;
    AppRequestRawData::IdType reqID;
    bool writable;
};

struct PageAccessType {
    full_id id;
    bool isWritable;
};

struct Block {
    int_fast64_t blockNumber;
};

class BlockLoader {
public:
    void loadBlock(const Block& b) {};

    AppRequestRawData loadRequest(AppRequestRawData::IdType id) {};

    std::vector<long_id> getAppAccessList() {};

    std::vector<long_id> getChunkAccessList(long_id appID) {};

    /// it must check that the list is sorted
    const std::vector<AccessBlock> getBlockAccessList(long_id appID, long_id chunkID) {};

    int_fast32_t getNumOfRequests() {};

    std::vector<PageAccessType> getRequiredPages() {};
};

} // namespace ascee::runtime
#endif // ASCEE_BLOCKLOADER_H
