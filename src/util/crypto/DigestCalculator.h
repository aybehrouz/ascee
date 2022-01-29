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

#ifndef ARGENNON_DIGEST_CALCULATOR_H
#define ARGENNON_DIGEST_CALCULATOR_H

#include <cstdint>
#include <cstddef>

namespace argennon::util {

struct Digest {
    int x = 0;
public:
    bool operator==(const Digest& rhs) const {
        return x == rhs.x;
    }

    bool operator!=(const Digest& rhs) const {
        return !(rhs == *this);
    }
};

class DigestCalculator {
public:
    void append(uint8_t* binary, size_t len) {};

    template<typename T>
    auto& operator<<(const T& value) {
        return *this;
    }

    Digest CalculateDigest() { return {}; };
private:
    int x;
};

} // namespace argennon::ascee::runtime
#endif // ARGENNON_DIGEST_CALCULATOR_H
