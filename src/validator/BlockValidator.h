// Copyright (c) 2021-2022 aybehrouz <behrouz_ayati@yahoo.com>. All rights
// reserved. This file is part of the C++ implementation of the Argennon smart
// contract Execution Environment (AscEE).
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

#ifndef NODE_BLOCK_VALIDATOR_H
#define NODE_BLOCK_VALIDATOR_H

#include "RequestScheduler.h"
#include "BlockLoader.h"
#include "storage/PageCache.h"

namespace argennon::ave {

class BlockValidator {
public:
    BlockValidator(
            asa::PageCache& cache,
            BlockLoader& blockLoader,
            int workersCount = -1
    );

    bool conditionalValidate(const BlockInfo& current, const BlockInfo& previous);

private:
    ascee::runtime::Executor executor;
    asa::PageCache& cache;
    BlockLoader& blockLoader;
    int workersCount = -1;

    void loadRequests(RequestScheduler& scheduler);

    void buildDependencyGraph(RequestScheduler& scheduler);

    void executeRequests(RequestScheduler& scheduler);
};

} // namespace argennon::ave
#endif //NODE_BLOCK_VALIDATOR_H
