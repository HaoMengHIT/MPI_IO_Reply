/*
 *  (C) 2012 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#define _XOPEN_SOURCE 500

#include "recorder-runtime-config.h"

#include <stdlib.h>
#include <stdio.h>

#include <mpi.h>
#include "recorder.h"
#include "recorder-dynamic.h"

#ifdef RECORDER_PRELOAD

#include <dlfcn.h>
#include "simple_compress.h"

// make it compatible with c++
extern int recorder_mpi_initialize(int*, char***);
extern void recorder_shutdown(int);
extern SimpleCompress *sc;

#define RECORDER_FORWARD_DECL(name,ret,args) \
  ret (*__real_ ## name)args = NULL;

#include "decl_list.def"

#undef RECORDER_FORWARD_DECL

#define RECORDER_FORWARD_DECL(name, ret, args) \
	typedef ret (* TYPE_##name)args;

#include "decl_list.def"

#undef RECORDER_FORWARD_DECL

int resolve_mpi_symbols()
{	
#define RECORDER_FORWARD_DECL(func, unused1, unused2) \
    __real_ ## func = (TYPE_##func)dlsym(RTLD_NEXT, #func); \
    if (!(__real_ ## func)) { \
        fprintf(stderr, "Darshan failed to map symbol: %s\n", #func); \
    }

#include "decl_list.def"
#undef RECORDER_FORWARD_DECL
}

#endif

int MPI_Init(int *argc, char ***argv)
{
    int ret;

#ifdef RECORDER_PRELOAD
    resolve_mpi_symbols();
#endif

    ret = RECORDER_MPI_CALL(PMPI_Init)(argc, argv);
    if(ret != MPI_SUCCESS)
    {
        return(ret);
    }

    recorder_mpi_initialize(argc, argv);

    return(ret);
}

int MPI_Init_thread (int *argc, char ***argv, int required, int *provided)
{
    int ret;

    ret = RECORDER_MPI_CALL(PMPI_Init_thread)(argc, argv, required, provided);
    if (ret != MPI_SUCCESS)
    {
        return(ret);
    }

    recorder_mpi_initialize(argc, argv);

    return(ret);
}

int MPI_Finalize(void)
{
    int ret;

        recorder_shutdown(0);
    ret = RECORDER_MPI_CALL(PMPI_Finalize)();

    return(ret);
}
