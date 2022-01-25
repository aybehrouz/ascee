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

#ifndef ARGENNON_HEAP_PAGE_H
#define ARGENNON_HEAP_PAGE_H

#include "arg/primitives.h"
#include <map>
#include <vector>
#include <memory>
#include "heap/Chunk.h"

namespace argennon::asa {
/**
 * When a page contains migrants, its native chunk can not be migrated. We need to check that in this class.
 *
 * If the page does not have any migrants, its
 * native can be migrated and the page will be converted into a special <<moved>> page in the merkle tree. However,
 * we don't need to implement that here.
 */
class Page {
    using Chunk = ascee::runtime::Chunk;
public:
    class Delta {

    };

    explicit Page(int64_fast blockNumber) : version(blockNumber) {
        native = std::make_unique<Chunk>();
    }

    /**
     *
     * @param id
     * @param migrant must be a pointer to a chunk allocated on heap. The page will take the ownership of the pointer.
     */
    void addMigrant(full_id id, Chunk* migrant) {
        if (!migrants.try_emplace(id, migrant).second) {
            throw BlockError("migrant already exists");
        }
    };

    byte* getDigest() { return nullptr; };

    [[nodiscard]]
    int64_fast getBlockNumber() const {
        return version;
    }

    void applyDelta(Delta delta, int64_fast blockNumber) {
        version = blockNumber;
    }

    void setWritableFlag(bool writable) {
        native->setWritable(writable);
        for (const auto& pair: migrants) {
            pair.second->setWritable(writable);
        }
    }

    void removeDelta(Delta delta) {

    }

    [[nodiscard]]
    Chunk* getNative() {
        return native.get();
    }

    [[nodiscard]]
    const std::map<full_id, std::unique_ptr<Chunk>>& getMigrants() {
        return migrants;
    }

    Chunk* extractMigrant(full_id id) {
        try {
            auto ret = migrants.at(id).release();
            migrants.erase(id);
            return ret;
        } catch (const std::out_of_range&) {
            throw BlockError("is not a migrant");
        }
    }

    Chunk* extractNative() {
        if (!migrants.empty()) throw BlockError("migrating native chunk of a page containing migrants");
        return native.release();
    }

private:
    int64_fast version = 0;
    bool writableFlag = false;
    std::unique_ptr<Chunk> native;
    std::map<full_id, std::unique_ptr<Chunk>> migrants;
};

} // namespace argennon::asa
#endif // ARGENNON_HEAP_PAGE_H
