/*
 *  (C) 2009 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef __RECORDER_H
#define __RECORDER_H

#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <mpi.h>
#include "recorder-log-format.h"

extern FILE* __recorderfh;

#include <time.h>
struct timespec recorder_wtime(void);

#endif /* __RECORDER_H */
