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

#ifndef NODE_BLOCK_VALIDATOR_H
#define NODE_BLOCK_VALIDATOR_H

#include <executor/Executor.h>
#include <loader/BlockLoader.h>

namespace node::validator {

class BlockValidator {
public:
    BlockValidator(ascee::runtime::BlockLoader& blockLoader,
                   ascee::runtime::PageLoader& pageLoader) : blockLoader(blockLoader), cache(pageLoader) {}

    void conditionalValidate(const ascee::runtime::Block& current, const ascee::runtime::Block& previous);

private:
    ascee::runtime::Executor executor;
    ascee::runtime::heap::PageCache cache;
    ascee::runtime::BlockLoader& blockLoader;

    void loadRequests(ascee::runtime::RequestScheduler& scheduler);

    void loadMemoryAccessMap(ascee::runtime::RequestScheduler& scheduler);

    void executeRequests(ascee::runtime::RequestScheduler& scheduler, int workersCount = -1);
};

} // namespace node::validator
#endif //NODE_BLOCK_VALIDATOR_H