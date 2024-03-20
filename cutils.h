/*
 * C utilities
 *
 * Copyright (c) 2017 Fabrice Bellard
 * Copyright (c) 2018 Charlie Gordon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef CUTILS_H
#define CUTILS_H

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#if defined(_WIN32)
#include <windows.h>
#endif
#if defined(_MSC_VER)
#include <winsock2.h>
#include <malloc.h>
#define alloca _alloca
#define ssize_t ptrdiff_t
#endif
#if defined(__APPLE__)
#include <malloc/malloc.h>
#elif defined(__linux__) || defined(__ANDROID__) || defined(__CYGWIN__)
#include <malloc.h>
#elif defined(__FreeBSD__)
#include <malloc_np.h>
#endif
#if !defined(_WIN32)
#include <errno.h>
#include <pthread.h>
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#  define likely(x)       (x)
#  define unlikely(x)     (x)
#  define force_inline __forceinline
#  define no_inline __declspec(noinline)
#  define __maybe_unused
#  define __attribute__(x)
#  define __attribute(x)
#  include <intrin.h>
static void *__builtin_frame_address(unsigned int level) {
    return (void *)((char*)_AddressOfReturnAddress() - sizeof(int *) - level * sizeof(int *));
}
#else
#  define likely(x)       __builtin_expect(!!(x), 1)
#  define unlikely(x)     __builtin_expect(!!(x), 0)
#  define force_inline inline __attribute__((always_inline))
#  define no_inline __attribute__((noinline))
#  define __maybe_unused __attribute__((unused))
#endif

// https://stackoverflow.com/a/6849629
#undef FORMAT_STRING
#ifdef _MSC_VER
# if _MSC_VER >= 1400
#  include <sal.h>
#  if _MSC_VER > 1400
#   define FORMAT_STRING(p) _Printf_format_string_ p
#  else
#   define FORMAT_STRING(p) __format_string p
#  endif
# endif
#endif /* _MSC_VER */
#ifndef FORMAT_STRING
# define FORMAT_STRING(p) p
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#include <math.h>
#define INF      INFINITY
#define NEG_INF  (-INFINITY)
#else
#define INF      (1.0/0.0)
#define NEG_INF  (-1.0/0.0)
#endif

#define xglue(x, y) x ## y
#define glue(x, y) xglue(x, y)
#define stringify(s)    tostring(s)
#define tostring(s)     #s

#ifndef offsetof
#define offsetof(type, field) ((size_t) &((type *)0)->field)
#endif
#ifndef countof
#define countof(x) (sizeof(x) / sizeof((x)[0]))
#ifndef endof
#define endof(x) ((x) + countof(x))
#endif
#endif
#ifndef container_of
/* return the pointer of type 'type *' containing 'ptr' as field 'member' */
#define container_of(ptr, type, member) ((type *)((uint8_t *)(ptr) - offsetof(type, member)))
#endif

#if !defined(_MSC_VER) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define minimum_length(n)  static n
#else
#define minimum_length(n)  n
#endif

typedef int BOOL;

#ifndef FALSE
enum {
    FALSE = 0,
    TRUE = 1,
};
#endif

void pstrcpy(char *buf, size_t buf_size, const char *str);
char *pstrcat(char *buf, size_t buf_size, const char *s);
int strstart(const char *str, const char *val, const char **ptr);
int has_suffix(const char *str, const char *suffix);

static inline uint8_t is_be(void) {
    union {
        uint16_t a;
        uint8_t  b;
    } u = { 0x100 };
    return u.b;
}

static inline int max_int(int a, int b)
{
    if (a > b)
        return a;
    else
        return b;
}

static inline int min_int(int a, int b)
{
    if (a < b)
        return a;
    else
        return b;
}

static inline uint32_t max_uint32(uint32_t a, uint32_t b)
{
    if (a > b)
        return a;
    else
        return b;
}

static inline uint32_t min_uint32(uint32_t a, uint32_t b)
{
    if (a < b)
        return a;
    else
        return b;
}

static inline int64_t max_int64(int64_t a, int64_t b)
{
    if (a > b)
        return a;
    else
        return b;
}

static inline int64_t min_int64(int64_t a, int64_t b)
{
    if (a < b)
        return a;
    else
        return b;
}

/* WARNING: undefined if a = 0 */
static inline int clz32(unsigned int a)
{
#if defined(_MSC_VER) && !defined(__clang__)
    // XXX: unsigned int _lzcnt_u32(unsigned int a)
    unsigned long index;
    _BitScanReverse(&index, a);
    return 31 - index;
#else
    return __builtin_clz(a);
#endif
}

/* WARNING: undefined if a = 0 */
static inline int clz64(uint64_t a)
{
#if defined(_MSC_VER) && !defined(__clang__)
    // XXX: unsigned int _lzcnt_u64(unsigned __int64 a)
    unsigned long index;
    _BitScanReverse64(&index, a);
    return 63 - index;
#else
    return __builtin_clzll(a);
#endif
}

/* WARNING: undefined if a = 0 */
static inline int ctz32(unsigned int a)
{
#if defined(_MSC_VER) && !defined(__clang__)
    // XXX: unsigned int _tzcnt_u32 (unsigned int a)
    unsigned long index;
    _BitScanForward(&index, a);
    return index;
#else
    return __builtin_ctz(a);
#endif
}

/* WARNING: undefined if a = 0 */
static inline int ctz64(uint64_t a)
{
#if defined(_MSC_VER) && !defined(__clang__)
    // XXX: unsigned int _tzcnt_u64 (unsigned __int64 a)
    unsigned long index;
    _BitScanForward64(&index, a);
    return index;
#else
    return __builtin_ctzll(a);
#endif
}

static inline uint64_t get_u64(const uint8_t *tab)
{
    uint64_t v;
    memcpy(&v, tab, sizeof(v));
    return v;
}

static inline int64_t get_i64(const uint8_t *tab)
{
    int64_t v;
    memcpy(&v, tab, sizeof(v));
    return v;
}

static inline void put_u64(uint8_t *tab, uint64_t val)
{
    memcpy(tab, &val, sizeof(val));
}

static inline uint32_t get_u32(const uint8_t *tab)
{
    uint32_t v;
    memcpy(&v, tab, sizeof(v));
    return v;
}

static inline int32_t get_i32(const uint8_t *tab)
{
    int32_t v;
    memcpy(&v, tab, sizeof(v));
    return v;
}

static inline void put_u32(uint8_t *tab, uint32_t val)
{
    memcpy(tab, &val, sizeof(val));
}

static inline uint32_t get_u16(const uint8_t *tab)
{
    uint16_t v;
    memcpy(&v, tab, sizeof(v));
    return v;
}

static inline int32_t get_i16(const uint8_t *tab)
{
    int16_t v;
    memcpy(&v, tab, sizeof(v));
    return v;
}

static inline void put_u16(uint8_t *tab, uint16_t val)
{
    memcpy(tab, &val, sizeof(val));
}

static inline uint32_t get_u8(const uint8_t *tab)
{
    return *tab;
}

static inline int32_t get_i8(const uint8_t *tab)
{
    return (int8_t)*tab;
}

static inline void put_u8(uint8_t *tab, uint8_t val)
{
    *tab = val;
}

#ifndef bswap16
static inline uint16_t bswap16(uint16_t x)
{
    return (uint16_t)((x >> 8) | (x << 8));
}
#endif

#ifndef bswap32
static inline uint32_t bswap32(uint32_t v)
{
    return ((v & 0xff000000) >> 24) | ((v & 0x00ff0000) >>  8) |
        ((v & 0x0000ff00) <<  8) | ((v & 0x000000ff) << 24);
}
#endif

#ifndef bswap64
static inline uint64_t bswap64(uint64_t v)
{
    return ((v & ((uint64_t)0xff << (7 * 8))) >> (7 * 8)) |
        ((v & ((uint64_t)0xff << (6 * 8))) >> (5 * 8)) |
        ((v & ((uint64_t)0xff << (5 * 8))) >> (3 * 8)) |
        ((v & ((uint64_t)0xff << (4 * 8))) >> (1 * 8)) |
        ((v & ((uint64_t)0xff << (3 * 8))) << (1 * 8)) |
        ((v & ((uint64_t)0xff << (2 * 8))) << (3 * 8)) |
        ((v & ((uint64_t)0xff << (1 * 8))) << (5 * 8)) |
        ((v & ((uint64_t)0xff << (0 * 8))) << (7 * 8));
}
#endif

static inline void inplace_bswap16(uint8_t *tab) {
    put_u16(tab, bswap16((uint16_t)get_u16(tab)));
}

static inline void inplace_bswap32(uint8_t *tab) {
    put_u32(tab, bswap32(get_u32(tab)));
}

/* XXX: should take an extra argument to pass slack information to the caller */
typedef void *DynBufReallocFunc(void *opaque, void *ptr, size_t size);

typedef struct DynBuf {
    uint8_t *buf;
    size_t size;
    size_t allocated_size;
    BOOL error; /* true if a memory allocation error occurred */
#if SIZE_MAX > UINT_MAX
    int pad;    /* prevent alignment warning */
#endif
    DynBufReallocFunc *realloc_func;
    void *opaque; /* for realloc_func */
} DynBuf;

void dbuf_init2(DynBuf *s, void *opaque, DynBufReallocFunc *realloc_func);
static inline void dbuf_init(DynBuf *s) { dbuf_init2(s, NULL, NULL); }
int dbuf_realloc(DynBuf *s, size_t new_size);
int dbuf_write(DynBuf *s, size_t offset, const void *data, size_t len);
int dbuf_put(DynBuf *s, const void *data, size_t len);
int dbuf_put_self(DynBuf *s, size_t offset, size_t len);
int dbuf_putc(DynBuf *s, uint8_t c);
int dbuf_putstr(DynBuf *s, const char *str);
static inline int dbuf_put_u16(DynBuf *s, uint16_t val)
{
    return dbuf_put(s, (uint8_t *)&val, 2);
}
static inline int dbuf_put_u32(DynBuf *s, uint32_t val)
{
    return dbuf_put(s, (uint8_t *)&val, 4);
}
static inline int dbuf_put_u64(DynBuf *s, uint64_t val)
{
    return dbuf_put(s, (uint8_t *)&val, 8);
}
__attribute__((format(printf, 2, 3)))
int dbuf_printf(DynBuf *s, FORMAT_STRING(const char *fmt), ...);
extern int (*dbuf_vprintf_fun)(DynBuf *s, const char *fmt, va_list ap);
void dbuf_free(DynBuf *s);
static inline BOOL dbuf_error(DynBuf *s) {
    return s->error;
}
static inline void dbuf_set_error(DynBuf *s)
{
    s->error = TRUE;
}

/*---- UTF-8 and UTF-16 handling ----*/

#define UTF8_CHAR_LEN_MAX 4

enum {
    UTF8_PLAIN_ASCII  = 0,  // 7-bit ASCII plain text
    UTF8_NON_ASCII    = 1,  // has non ASCII code points (8-bit or more)
    UTF8_HAS_16BIT    = 2,  // has 16-bit code points
    UTF8_HAS_NON_BMP1 = 4,  // has non-BMP1 code points, needs UTF-16 surrogate pairs
    UTF8_HAS_ERRORS   = 8,  // has encoding errors
};
int utf8_scan(const char *buf, size_t len, size_t *plen);
size_t utf8_encode_len(uint32_t c);
size_t utf8_encode(uint8_t *buf, uint32_t c);
uint32_t utf8_decode_len(const uint8_t *p, size_t max_len, const uint8_t **pp);
uint32_t utf8_decode(const uint8_t *p, const uint8_t **pp);
size_t utf8_decode_buf8(uint8_t *dest, size_t dest_len, const char *src, size_t src_len);
size_t utf8_decode_buf16(uint16_t *dest, size_t dest_len, const char *src, size_t src_len);
size_t utf8_encode_buf8(char *dest, size_t dest_len, const uint8_t *src, size_t src_len);
size_t utf8_encode_buf16(char *dest, size_t dest_len, const uint16_t *src, size_t src_len);

static inline BOOL is_surrogate(uint32_t c)
{
    return (c >> 11) == (0xD800 >> 11); // 0xD800-0xDFFF
}

static inline BOOL is_hi_surrogate(uint32_t c)
{
    return (c >> 10) == (0xD800 >> 10); // 0xD800-0xDBFF
}

static inline BOOL is_lo_surrogate(uint32_t c)
{
    return (c >> 10) == (0xDC00 >> 10); // 0xDC00-0xDFFF
}

static inline uint32_t get_hi_surrogate(uint32_t c)
{
    return (c >> 10) - (0x10000 >> 10) + 0xD800;
}

static inline uint32_t get_lo_surrogate(uint32_t c)
{
    return (c & 0x3FF) | 0xDC00;
}

static inline uint32_t from_surrogate(uint32_t hi, uint32_t lo)
{
    return 65536 + 1024 * (hi & 1023) + (lo & 1023);
}

static inline int from_hex(int c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    else
        return -1;
}

static inline uint8_t is_lower_ascii(uint8_t c) {
    return c >= 'a' && c <= 'z';
}

static inline uint8_t is_upper_ascii(uint8_t c) {
    return c >= 'A' && c <= 'Z';
}

static inline uint8_t to_lower_ascii(uint8_t c) {
    return c >= 'A' && c <= 'Z' ? c + ('a' - 'A') : c;
}

static inline uint8_t to_upper_ascii(uint8_t c) {
    return c >= 'a' && c <= 'z' ? c - ('a' - 'A') : c;
}

extern char const digits36[36];
extern char const digits36_upper[36];
size_t u32toa(char buf[minimum_length(11)], uint32_t n);
size_t i32toa(char buf[minimum_length(12)], int32_t n);
size_t u64toa(char buf[minimum_length(21)], uint64_t n);
size_t i64toa(char buf[minimum_length(22)], int64_t n);
size_t u32toa_radix(char buf[minimum_length(33)], uint32_t n, unsigned int base);
size_t i32toa_radix(char buf[minimum_length(34)], int32_t n, unsigned base);
size_t u64toa_radix(char buf[minimum_length(65)], uint64_t n, unsigned int base);
size_t i64toa_radix(char buf[minimum_length(66)], int64_t n, unsigned int base);

void rqsort(void *base, size_t nmemb, size_t size,
            int (*cmp)(const void *, const void *, void *),
            void *arg);

int64_t js__gettimeofday_us(void);
uint64_t js__hrtime_ns(void);

static inline size_t js__malloc_usable_size(const void *ptr)
{
#if defined(__APPLE__)
    return malloc_size(ptr);
#elif defined(_WIN32)
    return _msize((void *)ptr);
#elif defined(__linux__) || defined(__ANDROID__) || defined(__CYGWIN__) || defined(__FreeBSD__)
    return malloc_usable_size((void *)ptr);
#else
    return 0;
#endif
}

/* Cross-platform threading APIs. */

#if !defined(EMSCRIPTEN) && !defined(__wasi__)

#if defined(_WIN32)
#define JS_ONCE_INIT INIT_ONCE_STATIC_INIT
typedef INIT_ONCE js_once_t;
typedef CRITICAL_SECTION js_mutex_t;
typedef CONDITION_VARIABLE js_cond_t;
#else
#define JS_ONCE_INIT PTHREAD_ONCE_INIT
typedef pthread_once_t js_once_t;
typedef pthread_mutex_t js_mutex_t;
typedef pthread_cond_t js_cond_t;
#endif

void js_once(js_once_t *guard, void (*callback)(void));

void js_mutex_init(js_mutex_t *mutex);
void js_mutex_destroy(js_mutex_t *mutex);
void js_mutex_lock(js_mutex_t *mutex);
void js_mutex_unlock(js_mutex_t *mutex);

void js_cond_init(js_cond_t *cond);
void js_cond_destroy(js_cond_t *cond);
void js_cond_signal(js_cond_t *cond);
void js_cond_broadcast(js_cond_t *cond);
void js_cond_wait(js_cond_t *cond, js_mutex_t *mutex);
int js_cond_timedwait(js_cond_t *cond, js_mutex_t *mutex, uint64_t timeout);

#endif /* !defined(EMSCRIPTEN) && !defined(__wasi__) */

#endif  /* CUTILS_H */
