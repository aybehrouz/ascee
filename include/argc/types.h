#ifndef ASCEE_ARGC_TYPES_H_
#define ASCEE_ARGC_TYPES_H_

#include <stdint.h> // NOLINT(modernize-deprecated-headers)

/// int represents a signed integer with the most efficient size for the platform which MUST NOT be smaller than 32 bits.
typedef uint8_t byte;
typedef uint16_t uint16;
typedef int32_t int32;
typedef int64_t int64;
typedef __int128_t int128;
typedef double float64;
typedef __float128 float128;
typedef uint32_t short_id_t;
typedef uint64_t std_id_t;
typedef __uint128_t full_id_t;
typedef struct String string_t;
typedef struct StringBuffer string_buffer;

typedef int (* dispatcher_ptr_t)(void*, string_t);

struct String {
    const char* content;
    int length;
};

struct StringBuffer {
    char* buffer;
    int maxSize;
    int end;
};

/// HTTP status codes. new costume coded could be defined.
#define HTTP_OK 200
#define BAD_REQUEST 400
#define NOT_FOUND 404
#define INTERNAL_ERROR 500
#define LOOP_DETECTED 508

#endif // ASCEE_ARGC_TYPES_H_
