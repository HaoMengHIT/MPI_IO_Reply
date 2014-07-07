/*
 *  (C) 2012 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef __RECORDER_DYNAMIC_H
#define __RECORDER_DYNAMIC_H

#ifdef RECORDER_PRELOAD

#include <dlfcn.h>
#include <stdlib.h>

#define RECORDER_MPI_CALL(func) __real_ ## func

#define RECORDER_FORWARD_DECL(name,ret,args) \
  extern ret (*__real_ ## name)args;

#include "decl_list.def"

#undef RECORDER_FORWARD_DECL

#else

#define RECORDER_MPI_CALL(func) func

#endif

#endif
