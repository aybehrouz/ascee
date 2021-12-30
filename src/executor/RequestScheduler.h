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

#ifndef ASCEE_EXEC_SCHEDULER_H
#define ASCEE_EXEC_SCHEDULER_H

#include <argc/types.h>
#include <util/BlockingQueue.h>
#include <unordered_set>
#include <cassert>
#include "heap/PageCache.h"
#include "heap/Modifier.h"
#include "FailureManager.h"
#include "loader/BlockLoader.h"
#include "loader/AppLoader.h"
#include <atomic>
#include <utility>

namespace ascee::runtime {



struct AppRequest {
    using IdType = AppRequestRawData::IdType;
    IdType id;
    long_id calledAppID;
    std::string httpRequest;
    int_fast32_t gas;
    heap::Modifier modifier;
    std::unordered_map<long_id, dispatcher_ptr> appTable;
    FailureManager failureManager;
    Digest digest;
};

struct AppResponse {
    AppRequest::IdType reqID;
    int statusCode;
    std::string response;
};

class RequestScheduler;

class DagNode {
public:
    int_fast32_t decrementInDegree() {
        assert(inDegree > 0);
        return --inDegree;
    }

    void incrementInDegree() { ++inDegree; }

    AppRequest& getAppRequest() {
        return request;
    }

    int_fast32_t getInDegree() const {
        return inDegree;
    }

    auto& adjacentNodes() {
        return adjList;
    }

    bool isAdjacent(AppRequest::IdType other) const {
        return adjList.contains(other);
    }

    explicit DagNode(AppRequestRawData&& data, const RequestScheduler* scheduler);

private:
    AppRequest request;
    const std::unordered_set<AppRequest::IdType> adjList;
    std::atomic<int_fast32_t> inDegree = 0;
};

/// works fine even if the graph is not a dag and contains loops
class RequestScheduler {
public:
    AppRequest* nextRequest();

    void submitResult(const AppResponse& result);

    void findCollisions(long_id appID, long_id chunkID);

    /// this function is thread-safe as long as all used `id`s are distinct
    auto& requestAt(long id);

    /// this function is thread-safe as long as all used `id`s are distinct
    void addRequest(long id, AppRequestRawData&& data);

    void buildDag();

    explicit RequestScheduler(int_fast32_t totalRequestCount, heap::PageCache::ChunkIndex& heapIndex);

    [[nodiscard]]
    heap::Modifier buildModifierFor(AppRequest::IdType requestID) const {
        return buildModifier(memAccessMaps[requestID]);
    }

    [[nodiscard]]
    heap::Modifier buildModifier(const AppRequestRawData::MemAccessMapType& rawAccessMap) const;

private:
    heap::PageCache::ChunkIndex& heapIndex;
    std::atomic<int_fast32_t> count;
    BlockingQueue<DagNode*> zeroQueue;
    std::unique_ptr<std::unique_ptr<DagNode>[]> nodeIndex;
    std::vector<AppRequestRawData::MemAccessMapType> memAccessMaps;
    std::unique_ptr<AppRequestRawData::MemAccessMapType> sortedAccessBlocks;

    void registerDependency(AppRequest::IdType u, AppRequest::IdType v);

};

} // namespace ascee::runtime
#endif // ASCEE_EXEC_SCHEDULER_H
