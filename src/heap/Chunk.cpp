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

#include <cstring>
#include "Chunk.h"

using namespace ascee;

const Chunk::Pointer Chunk::null = Chunk::Pointer(nullptr);
Chunk* const Chunk::transient = new Chunk();

/// mew chunks always have a size of zero. Their size usually should be changed through the pointer obtainable by
/// getSizePointer function.
/// The chunk will be zero initialized. This is important to make sure that smart contracts
/// behave deterministically and validators will agree on the result of executing a smart contact.
Chunk::Chunk(int size) : content(new byte[size]()) {}

int32 Chunk::getsize() const {
    return chunkSize;
}

bool Chunk::isTransient() const {
    return !bool(content);
}

Chunk::Pointer Chunk::getContentPointer(int32 offset) {
    return Pointer(content.get() + offset);
}

Chunk::Pointer Chunk::getSizePointer() {
    return Pointer(reinterpret_cast<byte*>(&chunkSize));
}

void Chunk::expandSpace(int extra) {
    // check new size
    auto* newContent = new byte[chunkSize + extra];
    memcpy(newContent, content.get(), chunkSize);
    content = std::unique_ptr<byte[]>(newContent);
}
