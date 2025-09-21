#ifndef STRING_H
#define STRING_H

#include <stddef.h>

/// @brief Compares the first `count` bytes of buffers `lhs` and `rhs`. The
/// comparison is done left-to-right.
///
/// @param lhs The left-hand-side of the comparison. Must be a non-null pointer
///            valid for `count` reads.
/// @param rhs The right-hand-side of the comparison. Must be a non-null pointer
///            valid for `count` reads.
/// @param count The amount of bytes to compare from both buffers.
///
/// @returns The returned value's sign is the sign of the difference between the
/// values of the first pair or bytes that differ in the objects being compared.
///
/// For instance, in the two buffers `[10, 20, 30]` and `[10, 20, 20]` the first
/// pair of different bytes is `(30, 20)` and the result would be `1`, because
/// `30 > 20`. If both buffers are identical, the result will be `0`.
int memcmp(const void *lhs, const void *rhs, size_t count);

/// @brief Copies the value `(unsigned char)ch` into each of the first `count`
///        bytes of the `dest` buffer.
///
/// @param dest The destination pointer. Must be a valid non-null
///             pointer valid for `count` writes.
/// @param ch The data to be copied into the buffer. It will be reinterpreted
///           as a byte with `(unsigned char)ch`
/// @param count The amount of bytes to copy
/// @returns Returns back the `dest` pointer.
void *memset(void *dest, int ch, size_t count);

/// @brief Moves `count` bytes from `src` to `dest`.
///
/// Unlike `memmove`, the buffers may NOT overlap.
///
/// @param dest The destination pointer. Must be a valid non-null
///             pointer valid for `count` writes that does not
///             overlap with `src`.
/// @param src The source pointer. Must be a valid non-null
///            pointer valid for `count` reads that does not
///            overlap with `dst`.
/// @param count The amount of bytes to move
/// @returns Returns back the `dest` pointer
void *memcpy(void *__restrict dest, const void *__restrict src, size_t count);

/// @brief Moves `count` bytes from `src` to `dest`.
///
/// The objects may overlap. Copying takes places as if the bytes
/// were copied to a temporary buffer first.
///
/// @param dest The destination pointer. Must be a valid non-null
///             pointer valid for `count` writes.
/// @param src The source pointer. Must be a valid non-null
///            pointer valid for `count` reads.
/// @param count The amount of bytes to move
/// @returns Returns back the `dest` pointer
void *memmove(void *dest, const void *src, size_t count);

/// @brief Returns the length of a null-terminated string
///
/// @param data The buffer containing string data
/// @returns The length of the string (the position of the null byte)
size_t strlen(const char *data);

#endif