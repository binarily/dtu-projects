/* Copyright (c) 1997-2017, FenixOS Developers
   All Rights Reserved.

   This file is subject to the terms and conditions defined in
   file 'LICENSE', which is part of this source code package.
 */

/*! \file stdint.h This file holds definitions of integer types with a specific
     bit length. */

#ifndef _STDINT_H_
#define _STDINT_H_

typedef signed char             int8_t;
typedef signed short int        int16_t;
typedef signed int              int32_t;
typedef signed long long int    int64_t;

typedef int8_t                  int_least8_t;
typedef int16_t                 int_least6_t;
typedef int32_t                 int_least32_t;
typedef int64_t                 int_least64_t;

typedef int8_t                  int_fast8_t;
typedef int32_t                 int_fast16_t;
typedef int32_t                 int_fast32_t;
typedef int64_t                 int_fast64_t;

typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
typedef unsigned int            uint32_t;
typedef unsigned long long int  uint64_t;

typedef uint8_t                 uint_least8_t;
typedef uint16_t                uint_least16_t;
typedef uint32_t                uint_least32_t;
typedef uint64_t                uint_least64_t;

typedef uint8_t                 uint_fast8_t;
typedef uint32_t                uint_fast16_t;
typedef uint32_t                uint_fast32_t;
typedef uint64_t                uint_fast64_t;

typedef int32_t                 intptr_t;
typedef uint32_t                uintptr_t;

typedef int64_t                 intmax_t;
typedef uint64_t                uintmax_t;

/*! \todo Find a better place for these typedefs. Putting it here is not
    consistent with C99.
 */
typedef int32_t                 sig_atomic_t;
typedef uint32_t                size_t;

/* \warning This is a type which does not exist in C99. */
typedef uint32_t                usig_atomic_t;

# define INT8_MIN               (-INT8_MAX-1)
# define INT16_MIN              (-INT16_MAX-1)
# define INT32_MIN              (-INT32_MAX-1)
# define INT64_MIN              (-INT64_MAX-1)

# define INT8_MAX               (127)
# define INT16_MAX              (32767)
# define INT32_MAX              (2147483647L)
# define INT64_MAX              (9223372036854775807LL)

# define UINT8_MAX              (255)
# define UINT16_MAX             (65535)
# define UINT32_MAX             (4294967295U)
# define UINT64_MAX             (18446744073709551615UL)

# define INT_FAST8_MIN          INT8_MIN
# define INT_FAST16_MIN         INT32_MIN
# define INT_FAST32_MIN         INT32_MIN
# define INT_FAST64_MIN         INT64_MIN

# define INT_FAST8_MAX          INT8_MAX
# define INT_FAST16_MAX         INT32_MAX
# define INT_FAST32_MAX         INT32_MAX
# define INT_FAST64_MAX         INT64_MAX

# define UINT_FAST8_MAX         UINT8_MAX
# define UINT_FAST16_MAX        UINT32_MAX
# define UINT_FAST32_MAX        UINT32_MAX
# define UINT_FAST64_MAX        UINT64_MAX

# define INT_LEAST8_MIN         INT8_MIN
# define INT_LEAST16_MIN        INT16_MIN
# define INT_LEAST32_MIN        INT32_MIN
# define INT_LEAST64_MIN        INT64_MIN

# define INT_LEAST8_MAX         INT8_MAX
# define INT_LEAST16_MAX        INT16_MAX
# define INT_LEAST32_MAX        INT32_MAX
# define INT_LEAST64_MAX        INT64_MAX

# define UINT_LEAST8_MAX        UINT8_MAX
# define UINT_LEAST16_MAX       UINT16_MAX
# define UINT_LEAST32_MAX       UINT32_MAX
# define UINT_LEAST64_MAX       UINT64_MAX

# define INTPTR_MIN             INT32_MIN
# define INTPTR_MAX             INT32_MAX
# define UINTPTR_MAX            UINT32_MAX

# define SIG_ATOMIC_MIN         INT32_MIN
# define SIG_ATOMIC_MAX         INT32_MAX
# define USIG_ATOMIC_MAX        UINT32_MAX

# define INT_ATOMIC_MAX_BITLENGTH (32)

#endif
