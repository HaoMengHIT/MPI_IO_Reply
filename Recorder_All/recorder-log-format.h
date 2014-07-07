/*
 *  (C) 2009 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef __RECORDER_LOG_FORMAT_H
#define __RECORDER_LOG_FORMAT_H

#ifndef RECORDER_CONFIG_H
    #error Must define RECORDER_CONFIG_H
#endif

#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#ifdef HAVE_INTTYPES_H
	#include <inttypes.h>
#endif

#if !defined PRId64 || defined(PRI_MACROS_BROKEN)
#ifndef __WORDSIZE
#error failed to detect PRId64 or word size
#endif
# undef PRId64
#if __WORDSIZE == 64
# define PRId64 "ld"
#else
# define PRId64 "lld"
#endif
#endif
#if !defined PRIu64 || defined(PRI_MACROS_BROKEN)
#ifndef __WORDSIZE
#error failed to detect PRId64 or word size
#endif
# undef PRIu64
#if __WORDSIZE == 64
# define PRIu64 "lu"
#else
# define PRIu64 "llu"
#endif
#endif

#endif /* __RECORDER_LOG_FORMAT_H */
