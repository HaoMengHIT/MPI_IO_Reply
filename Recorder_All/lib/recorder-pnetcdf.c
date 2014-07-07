/*
 *  (C) 2009 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "recorder-runtime-config.h"
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "mpi.h"
#include "recorder.h"

#ifdef RECORDER_PRELOAD
#define __USE_GNU
#include <dlfcn.h>
#include <stdlib.h>

#define RECORDER_FORWARD_DECL(name,ret,args) \
  ret (*__real_ ## name)args = NULL;

#define RECORDER_DECL(__name) __name

#define MAP_OR_FAIL(func) \
    if (!(__real_ ## func)) \
    { \
        __real_ ## func = dlsym(RTLD_NEXT, #func); \
        if(!(__real_ ## func)) { \
            fprintf(stderr, "Darshan failed to map symbol: %s\n", #func); \
            exit(1); \
        } \
    }

#else

#define RECORDER_FORWARD_DECL(name,ret,args) \
  extern ret __real_ ## name args;

#define RECORDER_DECL(__name) __wrap_ ## __name

#define MAP_OR_FAIL(func)

#endif

RECORDER_FORWARD_DECL(ncmpi_create, int, (MPI_Comm comm, const char *path, int cmode, MPI_Info info, int *ncidp));
RECORDER_FORWARD_DECL(ncmpi_open, int, (MPI_Comm comm, const char *path, int omode, MPI_Info info, int *ncidp));
RECORDER_FORWARD_DECL(ncmpi_close, int, (int ncid));

static struct recorder_file_runtime* recorder_file_by_ncid(int ncid);

int RECORDER_DECL(ncmpi_create)(MPI_Comm comm, const char *path,
    int cmode, MPI_Info info, int *ncidp)
{
    int ret;
    struct recorder_file_runtime* file;
    char* tmp;
    int comm_size;
    int hash_index;

    MAP_OR_FAIL(ncmpi_create);

    ret = __real_ncmpi_create(comm, path, cmode, info, ncidp);
    return(ret);
}

int RECORDER_DECL(ncmpi_open)(MPI_Comm comm, const char *path,
    int omode, MPI_Info info, int *ncidp)
{
    int ret;
    struct recorder_file_runtime* file;
    char* tmp;
    int comm_size;
    int hash_index;

    MAP_OR_FAIL(ncmpi_open);

    ret = __real_ncmpi_open(comm, path, omode, info, ncidp);

    return(ret);

}

int RECORDER_DECL(ncmpi_close)(int ncid)
{
    struct recorder_file_runtime* file;
    int hash_index;
    int tmp_ncid = ncid;
    int ret;

    MAP_OR_FAIL(ncmpi_close);

    ret = __real_ncmpi_close(ncid);

    return(ret);

}
