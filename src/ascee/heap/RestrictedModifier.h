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

#ifndef ARGENNON_HEAP_MODIFIER_H
#define ARGENNON_HEAP_MODIFIER_H

#include <exception>
#include <cstring>
#include <utility>
#include <vector>
#include <arg/primitives.h>
#include <arg/info.h>
#include "Chunk.h"
#include "util/PrefixTrie.hpp"
#include "util/FixedOrderedMap.hpp"

namespace argennon::ascee::runtime {

class RestrictedModifier {
public:
    template<typename T>
    inline
    T load(uint32 offset, uint32 index = 0) { return getAccessBlock(offset).read<T>(currentVersion, index); }

    template<typename T, int h>
    inline
    T loadVarUInt(const util::PrefixTrie<T, h>& trie, uint32 offset, uint32 index = 0, int32* n = nullptr) {
        return getAccessBlock(offset).readVarUInt(trie, currentVersion, index, n);
    }

    template<typename T, int h>
    inline
    T loadIdentifier(const util::PrefixTrie<T, h>& trie, uint32 offset, uint32 index = 0, int32* n = nullptr) {
        return getAccessBlock(offset).readIdentifier(trie, currentVersion, index, n);
    }

    template<typename T>
    inline
    void store(uint32 offset, const T& value, uint32 index = 0) {
        getAccessBlock(offset).write<T>(currentVersion, index, value);
    }

    template<typename T>
    inline
    void addInt(uint32 offset, T value) { getAccessBlock(offset).addInt<T>(currentVersion, value); }

    template<typename T, int h>
    inline
    int storeVarUInt(const util::PrefixTrie<T, h>& trie, uint32 offset, T value) {
        return getAccessBlock(offset).writeVarUInt(trie, currentVersion, value);
    }

    void loadChunk(long_id localID);

    void loadChunk(long_id accountID, long_id localID);

    void loadContext(long_id appID);

    void restoreVersion(int16_t version);

    int16_t saveVersion();

    void writeToHeap();

    bool isValid(uint32 offset, uint32 size) {
        // we need to make sure that the access block is defined. Otherwise, scheduler can not guarantee that
        // isValid is properly parallelized with chunk resizing requests. Also, we need to make sure that an access
        // block with the proper size is defined at the offset. We should throw an exception if the block is not
        // defined because that's a violation of proper resource declaration.
        if (!getAccessBlock(offset).defined(size)) {
            throw std::out_of_range("isValid: access block not defined");
        }
        // we can't use getChunkSize() here
        return offset + size <= currentChunk->size.read<int32>(currentVersion, 0);
    }

    int32 getChunkSize();

    void updateChunkSize(uint32 newSize);

private:
    class AccessBlock {
    public:
        AccessBlock() = default;

        AccessBlock(AccessBlock&&) = default;

        AccessBlock(const AccessBlock&) = delete;

        AccessBlock(const Chunk::Pointer& heapLocation, int32 size, BlockAccessInfo::Access accessType);

        [[nodiscard]]
        bool defined(uint32 requiredSize) const {
            return size >= requiredSize && !accessType.denies(BlockAccessInfo::Access::Operation::check);
        }

        [[nodiscard]] inline
        int32 getSize() const { return size; }

        template<typename T, int h>
        inline
        T readIdentifier(const util::PrefixTrie<T, h>& trie, int16_t version, uint32 index, int32* n = nullptr) {
            return trie.readPrefixCode(prepareToRead(version, index, size), n, size);
        }

        template<typename T, int h>
        inline
        T readVarUInt(const util::PrefixTrie<T, h>& trie, int16_t version, uint32 index, int32* n = nullptr) {
            return trie.decodeVarUInt(prepareToRead(version, index, size), n, size);
        }

        template<typename T>
        inline
        T read(int16_t version, uint32 index) {
            auto content = prepareToRead(version, index, sizeof(T));

            T ret{};
            memcpy((byte*) &ret, content, sizeof(T));
            // Here we return the result by value. This is not bad for performance. Compilers usually implement
            // return-by-value using pass-by-pointer. (a.k.a NRVO optimization)
            return ret;
        }

        template<typename T, int h>
        inline
        int writeVarUInt(const util::PrefixTrie<T, h>& trie, int16_t version, uint32 index, T value) {
            int len;
            auto code = trie.encodeVarUInt(value, &len);
            trie.writeBigEndian(prepareToWrite(version, index, len), code, len);
            return len;
        }


        template<typename T>
        inline
        void write(int16_t version, uint32 index, const T& value) {
            memcpy(prepareToWrite(version, index, sizeof(value)), (byte*) &value, sizeof(value));
        }

        template<typename T>
        inline
        void addInt(int16_t version, T value) {
            static_assert(std::is_integral<T>::value);
            if (sizeof(T) != size) throw std::out_of_range("addInt size");
            if (accessType.denies(BlockAccessInfo::Access::Operation::int_add)) {
                throw std::out_of_range("block is not additive");
            }
            syncTo(version);
            T current;
            if (versionList.empty()) current = 0;
            else memcpy((byte*) &current, versionList.back().getContent(), sizeof(T));
            current += value;
            ensureExists(version);
            memcpy(versionList.back().getContent(), (byte*) &current, sizeof(T));
        }

        void wrToHeap(Chunk* chunk, int16_t version, int32 maxWriteSize);

    private:
        struct Version {
            const int16_t number;
            std::unique_ptr<byte[]> content;

            inline byte* getContent() { return content.get(); } // NOLINT(readability-make-member-function-const)

            Version(int16_t version, int32 size) : number(version), content(std::make_unique<byte[]>(size)) {}
        };

        Chunk::Pointer heapLocation;
        int32 size = 0;
        BlockAccessInfo::Access accessType{BlockAccessInfo::Access::Type::read_only};;
        std::vector<Version> versionList;

        void syncTo(int16_t version);

        bool ensureExists(int16_t version);

        byte* prepareToRead(int16_t version, uint32 offset, uint32 readSize);

        byte* prepareToWrite(int16_t version, uint32 offset, uint32 writeSize);
    };

    typedef util::FixedOrderedMap<int32, AccessBlock> AccessTableMap;
public:
    class ChunkInfo {
        friend class RestrictedModifier;

    public:
        enum class ResizingType {
            expandable, shrinkable, read_only, non_accessible
        };

        /**
         *
         * @param chunk
         * @param resizingType
         * @param sizeBound If resizingType is expandable this value determines the upper bound of the size. When
         * it is shrinkable, it determines the lower bound of the chunk size. When @p resizingType is not @p shrinkable
         * or @p expandable this value does not matter and will be ignored.
         * @param sortedAccessedOffsets
         * @param accessInfoList
         */
        ChunkInfo(Chunk* chunk, ResizingType resizingType, int32 sizeBound,
                  std::vector<int32> sortedAccessedOffsets,
                  const std::vector<BlockAccessInfo>& accessInfoList);

        ChunkInfo(ChunkInfo&&) = default;

        ChunkInfo(const ChunkInfo&) = delete;

        [[nodiscard]] int32 getInitialSize() const {
            // initial sizeChunk will not be modified as long as wrToHeap function is not called, all changes are only
            // made to the version array. Therefor we can obtain the initial size this way.
            return initialSize;
        }

    private:
        AccessTableMap accessTable;
        AccessBlock size;
        /// When the size AccessBlock is writable and newSize > 0 the chunk can only be expanded and the value of
        /// newSize indicates the upper bound of the settable size. If newSize <= 0 the chunk is only shrinkable and
        /// the magnitude of newSize indicates the lower bound of the chunk's new size.
        /// When the size AccessBlock is not writable, newSize != 0 indicates that the size block is readable and
        /// newSize == 0 indicates that the size block is not accessible.
        const int32 sizeBound = 0;
        Chunk* ptr{};
        int32 initialSize;
        ResizingType resizing;

        static std::vector<AccessBlock> toBlocks(
                Chunk* chunk,
                const std::vector<int32>& offsets,
                const std::vector<BlockAccessInfo>& accessInfoList
        );
    };

    typedef util::FixedOrderedMap<long_long_id, ChunkInfo> ChunkMap64;

    RestrictedModifier(std::vector<long_id> apps, std::vector<ChunkMap64> chunkMaps) :
            appsAccessMaps(std::move(apps), std::move(chunkMaps)) {}

    RestrictedModifier() = default;

    RestrictedModifier(const RestrictedModifier&) = delete;

    RestrictedModifier(RestrictedModifier&&) = delete;

    inline
    AccessBlock& getAccessBlock(uint32 offset) {
        if (currentChunk == nullptr) throw std::out_of_range("chunk is not loaded");
        try {
            return currentChunk->accessTable.at(int32(offset));
        } catch (const std::out_of_range&) {
            throw std::out_of_range("no access block is defined at offset: " + std::to_string(offset));
        }
    }

private:
    typedef util::FixedOrderedMap<long_id, ChunkMap64> AppMap;

    int16_t currentVersion = 0;
    ChunkInfo* currentChunk = nullptr;
    ChunkMap64* chunks = nullptr;
    AppMap appsAccessMaps;
};


} // namespace argennon::ascee::runtime::heap
#endif // ARGENNON_HEAP_MODIFIER_H
