/*
 *  (C) 2009 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "recorder-runtime-config.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include <search.h>
#include <assert.h>
#define __USE_GNU
#include <pthread.h>

#include "recorder.h"

#ifndef HAVE_OFF64_T
typedef int64_t off64_t;
#endif

extern char* __progname_full;

#ifdef RECORDER_PRELOAD
#define __USE_GNU
#include <dlfcn.h>
#include <stdlib.h>

#define RECORDER_FORWARD_DECL(name,ret,args) \
  ret (*__real_ ## name)args = NULL;

#define RECORDER_TYPE(name, ret, args) \
	typedef ret (* TYPE_##name)args;


#define MAP_OR_FAIL(func) \
    __real_ ## func = (TYPE_##func)dlsym(RTLD_NEXT, #func); \
    if (!(__real_ ## func)) { \
        fprintf(stderr, "Darshan failed to map symbol: %s\n", #func); \
	}

#define RECORDER_DECL(__name) __name

#define RECORDER_MPI_CALL(func) __real_ ## func

extern double (*__real_PMPI_Wtime)(void);

#else

#define RECORDER_FORWARD_DECL(name,ret,args) \
  extern ret __real_ ## name args;

#define RECORDER_DECL(__name) __wrap_ ## __name

#define MAP_OR_FAIL(func)

#define RECORDER_MPI_CALL(func) func

#endif

RECORDER_FORWARD_DECL(creat, int, (const char* path, mode_t mode));
RECORDER_FORWARD_DECL(creat64, int, (const char* path, mode_t mode));
RECORDER_FORWARD_DECL(open, int, (const char *path, int flags, ...));
RECORDER_FORWARD_DECL(open64, int, (const char *path, int flags, ...));
RECORDER_FORWARD_DECL(close, int, (int fd));
RECORDER_FORWARD_DECL(write, ssize_t, (int fd, const void *buf, size_t count));
RECORDER_FORWARD_DECL(read, ssize_t, (int fd, void *buf, size_t count));
RECORDER_FORWARD_DECL(lseek, off_t, (int fd, off_t offset, int whence));
RECORDER_FORWARD_DECL(lseek64, off64_t, (int fd, off64_t offset, int whence));
RECORDER_FORWARD_DECL(pread, ssize_t, (int fd, void *buf, size_t count, off_t offset));
RECORDER_FORWARD_DECL(pread64, ssize_t, (int fd, void *buf, size_t count, off64_t offset));
RECORDER_FORWARD_DECL(pwrite, ssize_t, (int fd, const void *buf, size_t count, off_t offset));
RECORDER_FORWARD_DECL(pwrite64, ssize_t, (int fd, const void *buf, size_t count, off64_t offset
));
RECORDER_FORWARD_DECL(readv, ssize_t, (int fd, const struct iovec *iov, int iovcnt));
RECORDER_FORWARD_DECL(writev, ssize_t, (int fd, const struct iovec *iov, int iovcnt));
RECORDER_FORWARD_DECL(__fxstat, int, (int vers, int fd, struct stat *buf));
RECORDER_FORWARD_DECL(__fxstat64, int, (int vers, int fd, struct stat64 *buf));
RECORDER_FORWARD_DECL(__lxstat, int, (int vers, const char* path, struct stat *buf));
RECORDER_FORWARD_DECL(__lxstat64, int, (int vers, const char* path, struct stat64 *buf));
RECORDER_FORWARD_DECL(__xstat, int, (int vers, const char* path, struct stat *buf));
RECORDER_FORWARD_DECL(__xstat64, int, (int vers, const char* path, struct stat64 *buf));
RECORDER_FORWARD_DECL(mmap, void*, (void *addr, size_t length, int prot, int flags, int fd, off_t offset));
RECORDER_FORWARD_DECL(mmap64, void*, (void *addr, size_t length, int prot, int flags, int fd, off64_t offset));
RECORDER_FORWARD_DECL(fopen, FILE*, (const char *path, const char *mode));
RECORDER_FORWARD_DECL(fopen64, FILE*, (const char *path, const char *mode));
RECORDER_FORWARD_DECL(fclose, int, (FILE *fp));
RECORDER_FORWARD_DECL(fread, size_t, (void *ptr, size_t size, size_t nmemb, FILE *stream));
RECORDER_FORWARD_DECL(fwrite, size_t, (const void *ptr, size_t size, size_t nmemb, FILE *stream));
RECORDER_FORWARD_DECL(fseek, int, (FILE *stream, long offset, int whence));
RECORDER_FORWARD_DECL(fsync, int, (int fd));
RECORDER_FORWARD_DECL(fdatasync, int, (int fd));

RECORDER_TYPE(creat, int, (const char* path, mode_t mode));
RECORDER_TYPE(creat64, int, (const char* path, mode_t mode));
RECORDER_TYPE(open, int, (const char *path, int flags, ...));
RECORDER_TYPE(open64, int, (const char *path, int flags, ...));
RECORDER_TYPE(close, int, (int fd));
RECORDER_TYPE(write, ssize_t, (int fd, const void *buf, size_t count));
RECORDER_TYPE(read, ssize_t, (int fd, void *buf, size_t count));
RECORDER_TYPE(lseek, off_t, (int fd, off_t offset, int whence));
RECORDER_TYPE(lseek64, off64_t, (int fd, off64_t offset, int whence));
RECORDER_TYPE(pread, ssize_t, (int fd, void *buf, size_t count, off_t offset));
RECORDER_TYPE(pread64, ssize_t, (int fd, void *buf, size_t count, off64_t offset));
RECORDER_TYPE(pwrite, ssize_t, (int fd, const void *buf, size_t count, off_t offset));
RECORDER_TYPE(pwrite64, ssize_t, (int fd, const void *buf, size_t count, off64_t offset));
			RECORDER_TYPE(readv, ssize_t, (int fd, const struct iovec *iov, int iovcnt));
			RECORDER_TYPE(writev, ssize_t, (int fd, const struct iovec *iov, int iovcnt));
			RECORDER_TYPE(__fxstat, int, (int vers, int fd, struct stat *buf));
			RECORDER_TYPE(__fxstat64, int, (int vers, int fd, struct stat64 *buf));
			RECORDER_TYPE(__lxstat, int, (int vers, const char* path, struct stat *buf));
			RECORDER_TYPE(__lxstat64, int, (int vers, const char* path, struct stat64 *buf));
			RECORDER_TYPE(__xstat, int, (int vers, const char* path, struct stat *buf));
			RECORDER_TYPE(__xstat64, int, (int vers, const char* path, struct stat64 *buf));
			RECORDER_TYPE(mmap, void*, (void *addr, size_t length, int prot, int flags, int fd, off_t offset));
			RECORDER_TYPE(mmap64, void*, (void *addr, size_t length, int prot, int flags, int fd, off64_t offset));
			RECORDER_TYPE(fopen, FILE*, (const char *path, const char *mode));
			RECORDER_TYPE(fopen64, FILE*, (const char *path, const char *mode));
			RECORDER_TYPE(fclose, int, (FILE *fp));
			RECORDER_TYPE(fread, size_t, (void *ptr, size_t size, size_t nmemb, FILE *stream));
			RECORDER_TYPE(fwrite, size_t, (const void *ptr, size_t size, size_t nmemb, FILE *stream));
			RECORDER_TYPE(fseek, int, (FILE *stream, long offset, int whence));
			RECORDER_TYPE(fsync, int, (int fd));
			RECORDER_TYPE(fdatasync, int, (int fd));


static int recorder_mem_alignment = 1;

/* these are paths that we will not trace */
static char* exclusions[] = {
"/etc/",
"/dev/",
"/usr/",
"/bin/",
"/boot/",
"/lib/",
"/opt/",
"/sbin/",
"/sys/",
"/proc/",
NULL
};

static double posix_wtime(void);

int RECORDER_DECL(close)(int fd)
{
    struct recorder_file_runtime* file;
    int hash_index;
    int tmp_fd = fd;
    double tm1, tm2;
    int ret;

    MAP_OR_FAIL(close);
    #ifndef DISABLE_POSIX_TRACE
    tm1 = recorder_wtime();

    size_t len = 256;
    char actual_filename[len];
    sprintf(actual_filename, "stdout");
    if(fd != 1) {
        char filename[len];
        sprintf(filename, "/proc/self/fd/%d", fd);
        readlink(filename, actual_filename, len);
    }
    if(__recorderfh != NULL)
        fprintf(__recorderfh,"%.5f close(%s) \n", tm1, actual_filename);
    #endif

    ret = __real_close(fd);
    tm2 = recorder_wtime();

    return(ret);
}

int RECORDER_DECL(fclose)(FILE *fp)
{
    struct recorder_file_runtime* file;
    int hash_index;
    int tmp_fd = fileno(fp);
    double tm1, tm2;
    int ret;

    MAP_OR_FAIL(fclose);
    #ifndef DISABLE_POSIX_TRACE
    tm1 = recorder_wtime();

    if(__recorderfh != NULL)
        fprintf(__recorderfh,"%.5f fclose(fp) \n", tm1);
    #endif

    ret = __real_fclose(fp);
    tm2 = recorder_wtime();

    return(ret);
}


int RECORDER_DECL(fsync)(int fd)
{
    int ret;
    struct recorder_file_runtime* file;
    double tm1, tm2;

    MAP_OR_FAIL(fsync);
    #ifndef DISABLE_POSIX_TRACE
    tm1 = recorder_wtime();
    size_t len = 256;
    char actual_filename[len];
    sprintf(actual_filename, "stdout");
    if(fd != 1) {
        char filename[len];
        sprintf(filename, "/proc/self/fd/%d", fd);
        readlink(filename, actual_filename, len);
    }
    if(__recorderfh != NULL)
        fprintf(__recorderfh,"%.5f fsync(%s) \n", tm1, actual_filename);

    #endif

    ret = __real_fsync(fd);
    tm2 = recorder_wtime();


    if(ret < 0)
        return(ret);

    return(ret);
}

int RECORDER_DECL(fdatasync)(int fd)
{
    int ret;
    struct recorder_file_runtime* file;
    double tm1, tm2;

    MAP_OR_FAIL(fdatasync);
    #ifndef DISABLE_POSIX_TRACE
    tm1 = recorder_wtime();
    size_t len = 256;
    char actual_filename[len];
    sprintf(actual_filename, "stdout");
    if(fd != 1) {
        char filename[len];
        sprintf(filename, "/proc/self/fd/%d", fd);
        readlink(filename, actual_filename, len);
    }
    if(__recorderfh != NULL)
        fprintf(__recorderfh,"%.5f fdatasync(%s) \n", tm1, actual_filename);

    #endif

    ret = __real_fdatasync(fd);
    tm2 = recorder_wtime();
    if(ret < 0)
        return(ret);

    return(ret);
}


void* RECORDER_DECL(mmap64)(void *addr, size_t length, int prot, int flags,
    int fd, off64_t offset)
{
    void* ret;
    struct recorder_file_runtime* file;

    MAP_OR_FAIL(mmap64);

    ret = __real_mmap64(addr, length, prot, flags, fd, offset);
    if(ret == MAP_FAILED)
        return(ret);

    return(ret);
}


void* RECORDER_DECL(mmap)(void *addr, size_t length, int prot, int flags,
    int fd, off_t offset)
{
    void* ret;
    struct recorder_file_runtime* file;

    MAP_OR_FAIL(mmap);

    ret = __real_mmap(addr, length, prot, flags, fd, offset);
    if(ret == MAP_FAILED)
        return(ret);

    return(ret);
}

int RECORDER_DECL(creat)(const char* path, mode_t mode)
{
    int ret;
    double tm1, tm2;

    MAP_OR_FAIL(creat);
    #ifndef DISABLE_POSIX_TRACE
    tm1 = recorder_wtime();

    if(__recorderfh != NULL)
        fprintf(__recorderfh,"%.5f creat(%s, %d) \n", tm1, path, mode);
    #endif

    ret = __real_creat(path, mode);
    tm2 = recorder_wtime();

    return(ret);
}

int RECORDER_DECL(creat64)(const char* path, mode_t mode)
{
    int ret;
    double tm1, tm2;

    MAP_OR_FAIL(creat64);
    #ifndef DISABLE_POSIX_TRACE
    tm1 = recorder_wtime();
    #endif
    ret = __real_creat64(path, mode);
    tm2 = recorder_wtime();

    return(ret);
}

int RECORDER_DECL(open64)(const char* path, int flags, ...)
{
    int mode = 0;
    int ret;
    double tm1, tm2;

    MAP_OR_FAIL(open64);

    if (flags & O_CREAT)
    {
        va_list arg;
        va_start(arg, flags);
        mode = va_arg(arg, int);
        va_end(arg);

        tm1 = recorder_wtime();
        ret = __real_open64(path, flags, mode);
        tm2 = recorder_wtime();
    }
    else
    {
        tm1 = recorder_wtime();
        ret = __real_open64(path, flags);
        tm2 = recorder_wtime();
    }

    return(ret);
}

int RECORDER_DECL(open)(const char *path, int flags, ...)
{
    int mode = 0;
    int ret;
    double tm1, tm2;

    MAP_OR_FAIL(open);

    if (flags & O_CREAT)
    {
        va_list arg;
        va_start(arg, flags);
        mode = va_arg(arg, int);
        va_end(arg);

        #ifndef DISABLE_POSIX_TRACE
        tm1 = recorder_wtime();
        if(__recorderfh != NULL)
            fprintf(__recorderfh,"%.5f open(%s, %d, %d) \n", tm1, path, flags, mode);
        #endif

        ret = __real_open(path, flags, mode);
        tm2 = recorder_wtime();
    }
    else
    {
        tm1 = recorder_wtime();
        if(__recorderfh != NULL)
            fprintf(__recorderfh,"%.5f open(%s, %d) \n", tm1, path, flags);
        ret = __real_open(path, flags);
        tm2 = recorder_wtime();
    }

    return(ret);
}

FILE* RECORDER_DECL(fopen64)(const char *path, const char *mode)
{
    FILE* ret;
    int fd;
    double tm1, tm2;

    MAP_OR_FAIL(fopen64);
    tm1 = recorder_wtime();
    ret = __real_fopen64(path, mode);
    tm2 = recorder_wtime();
    if(ret == 0)
        fd = -1;
    else
        fd = fileno(ret);

    return(ret);
}

FILE* RECORDER_DECL(fopen)(const char *path, const char *mode)
{
    FILE* ret;
    int fd;
    double tm1, tm2;

    MAP_OR_FAIL(fopen);
    #ifndef DISABLE_POSIX_TRACE
    tm1 = recorder_wtime();
    if(__recorderfh != NULL)
        fprintf(__recorderfh,"%.5f fopen(%s) \n", tm1, path);

    #endif
    ret = __real_fopen(path, mode);
    tm2 = recorder_wtime();
    if(ret == 0)
        fd = -1;
    else
        fd = fileno(ret);

    return(ret);
}

int RECORDER_DECL(__xstat64)(int vers, const char *path, struct stat64 *buf)
{
    int ret;
    struct recorder_file_runtime* file;
    double tm1, tm2;

    MAP_OR_FAIL(__xstat64);

    tm1 = recorder_wtime();
    ret = __real___xstat64(vers, path, buf);
    tm2 = recorder_wtime();
    if(ret < 0 || !S_ISREG(buf->st_mode))
        return(ret);

    return(ret);
}

int RECORDER_DECL(__lxstat64)(int vers, const char *path, struct stat64 *buf)
{
    int ret;
    struct recorder_file_runtime* file;
    double tm1, tm2;

    MAP_OR_FAIL(__lxstat64);

    tm1 = recorder_wtime();
    ret = __real___lxstat64(vers, path, buf);
    tm2 = recorder_wtime();
    if(ret < 0 || !S_ISREG(buf->st_mode))
        return(ret);

    return(ret);
}

int RECORDER_DECL(__fxstat64)(int vers, int fd, struct stat64 *buf)
{
    int ret;
    struct recorder_file_runtime* file;
    double tm1, tm2;

    MAP_OR_FAIL(__fxstat64);

    tm1 = recorder_wtime();
    ret = __real___fxstat64(vers, fd, buf);
    tm2 = recorder_wtime();
    if(ret < 0 || !S_ISREG(buf->st_mode))
        return(ret);

    return(ret);
}


int RECORDER_DECL(__xstat)(int vers, const char *path, struct stat *buf)
{
    int ret;
    struct recorder_file_runtime* file;
    double tm1, tm2;

    MAP_OR_FAIL(__xstat);

    tm1 = recorder_wtime();
    ret = __real___xstat(vers, path, buf);
    tm2 = recorder_wtime();
    if(ret < 0 || !S_ISREG(buf->st_mode))
        return(ret);

    return(ret);
}

int RECORDER_DECL(__lxstat)(int vers, const char *path, struct stat *buf)
{
    int ret;
    struct recorder_file_runtime* file;
    double tm1, tm2;

    MAP_OR_FAIL(__lxstat);

    tm1 = recorder_wtime();
    ret = __real___lxstat(vers, path, buf);
    tm2 = recorder_wtime();
    if(ret < 0 || !S_ISREG(buf->st_mode))
        return(ret);

    return(ret);
}

int RECORDER_DECL(__fxstat)(int vers, int fd, struct stat *buf)
{
    int ret;
    struct recorder_file_runtime* file;
    double tm1, tm2;

    MAP_OR_FAIL(__fxstat);

    tm1 = recorder_wtime();
    ret = __real___fxstat(vers, fd, buf);
    tm2 = recorder_wtime();
    if(ret < 0 || !S_ISREG(buf->st_mode))
        return(ret);

    return(ret);
}

ssize_t RECORDER_DECL(pread64)(int fd, void *buf, size_t count, off64_t offset)
{
    ssize_t ret;
    int aligned_flag = 0;
    double tm1, tm2;

    MAP_OR_FAIL(pread64);
    #ifndef DISABLE_POSIX_TRACE
    if((unsigned long)buf % recorder_mem_alignment == 0)
        aligned_flag = 1;

    tm1 = recorder_wtime();

    if(__recorderfh != NULL)
        fprintf(__recorderfh,"%.5f pread64(%d, void *buf, %d, %d) \n",tm1, fd, count, offset);
    #endif
    ret = __real_pread64(fd, buf, count, offset);
    tm2 = recorder_wtime();

    return(ret);
}

ssize_t RECORDER_DECL(pread)(int fd, void *buf, size_t count, off_t offset)
{
    ssize_t ret;
    int aligned_flag = 0;
    double tm1, tm2;

    MAP_OR_FAIL(pread);
    if((unsigned long)buf % recorder_mem_alignment == 0)
        aligned_flag = 1;

    #ifndef DISABLE_POSIX_TRACE
    tm1 = recorder_wtime();
    if(__recorderfh != NULL)
        fprintf(__recorderfh,"%.5f pread(%d,void *buf, %d, %d) \n",tm1, fd, count, offset);
    #endif

    ret = __real_pread(fd, buf, count, offset);
    tm2 = recorder_wtime();

    return(ret);
}


ssize_t RECORDER_DECL(pwrite)(int fd, const void *buf, size_t count, off_t offset)
{
    ssize_t ret;
    int aligned_flag = 0;
    double tm1, tm2;

    MAP_OR_FAIL(pwrite);
    if((unsigned long)buf % recorder_mem_alignment == 0)
        aligned_flag = 1;

    #ifndef DISABLE_POSIX_TRACE
    tm1 = recorder_wtime();

    if(__recorderfh != NULL)
        fprintf(__recorderfh,"%.5f pwrite(%d, const void *buf, %d, %d) \n",tm1, fd, count, offset);
    #endif
    ret = __real_pwrite(fd, buf, count, offset);
    tm2 = recorder_wtime();


    return(ret);
}

ssize_t RECORDER_DECL(pwrite64)(int fd, const void *buf, size_t count, off64_t offset)
{
    ssize_t ret;
    int aligned_flag = 0;
    double tm1, tm2;

    MAP_OR_FAIL(pwrite64);

    if((unsigned long)buf % recorder_mem_alignment == 0)
        aligned_flag = 1;

    #ifndef DISABLE_POSIX_TRACE
    tm1 = recorder_wtime();
    if(__recorderfh != NULL)
        fprintf(__recorderfh,"%.5f pwrite64(%d, const void *buf, %d, %d) \n", tm1, fd, count, offset);
    #endif
    ret = __real_pwrite64(fd, buf, count, offset);
    tm2 = recorder_wtime();

    return(ret);
}

ssize_t RECORDER_DECL(readv)(int fd, const struct iovec *iov, int iovcnt)
{
    ssize_t ret;
    int aligned_flag = 1;
    int i;
    double tm1, tm2;

    MAP_OR_FAIL(readv);

    for(i=0; i<iovcnt; i++)
    {
        if(((unsigned long)iov[i].iov_base % recorder_mem_alignment) != 0)
            aligned_flag = 0;
    }

    #ifndef DISABLE_POSIX_TRACE
    tm1 = recorder_wtime();
    if(__recorderfh != NULL)
        fprintf(__recorderfh,"%.5f readv(%d, iov, %d) \n",tm1, fd, iovcnt);
    #endif
    ret = __real_readv(fd, iov, iovcnt);
    tm2 = recorder_wtime();

    return(ret);
}

ssize_t RECORDER_DECL(writev)(int fd, const struct iovec *iov, int iovcnt)
{
    ssize_t ret;
    int aligned_flag = 1;
    int i;
    double tm1, tm2;

    MAP_OR_FAIL(writev);

    for(i=0; i<iovcnt; i++)
    {
        if(!((unsigned long)iov[i].iov_base % recorder_mem_alignment == 0))
            aligned_flag = 0;
    }

    #ifndef DISABLE_POSIX_TRACE
    tm1 = recorder_wtime();
    if(__recorderfh != NULL)
        fprintf(__recorderfh,"%.5f writev(%d, const iov, %d) \n", tm1, fd, iovcnt);
    #endif
    ret = __real_writev(fd, iov, iovcnt);
    tm2 = recorder_wtime();

    return(ret);
}

size_t RECORDER_DECL(fread)(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t ret;
    int aligned_flag = 0;
    double tm1, tm2;

    MAP_OR_FAIL(fread);

    if((unsigned long)ptr % recorder_mem_alignment == 0)
        aligned_flag = 1;

    #ifndef DISABLE_POSIX_TRACE
    tm1 = recorder_wtime();
    if(__recorderfh != NULL)
        fprintf(__recorderfh,"%.5f fread(ptr, %d, %d, %d) \n",tm1, size, nmemb, fileno(stream));

    #endif
    ret = __real_fread(ptr, size, nmemb, stream);
    tm2 = recorder_wtime();

    return(ret);
}

ssize_t RECORDER_DECL(read)(int fd, void *buf, size_t count)
{
    ssize_t ret;
    int aligned_flag = 0;
    double tm1, tm2;

    MAP_OR_FAIL(read);

    if((unsigned long)buf % recorder_mem_alignment == 0)
        aligned_flag = 1;

    tm1 = recorder_wtime();

    #ifndef DISABLE_POSIX_TRACE
    if(__recorderfh != NULL)
        fprintf(__recorderfh,"%.5f read(%d, buf, %d) \n", tm1, fd, count);

    #endif
    ret = __real_read(fd, buf, count);
    tm2 = recorder_wtime();

    return(ret);
}

ssize_t RECORDER_DECL(write)(int fd, const void *buf, size_t count)
{
    ssize_t ret;

    int aligned_flag = 0;
    double tm1, tm2;

    MAP_OR_FAIL(write);

    if((unsigned long)buf % recorder_mem_alignment == 0)
        aligned_flag = 1;

    #ifndef DISABLE_POSIX_TRACE
    tm1 = recorder_wtime();

    size_t len = 256;
    char actual_filename[len];
    sprintf(actual_filename, "stdout");
    if(fd != 1) {
        char filename[len];
        sprintf(filename, "/proc/self/fd/%d", fd);
        readlink(filename, actual_filename, len);
    }
    if(__recorderfh != NULL)
        fprintf(__recorderfh,"%.5f write(%s, void *buf, %d) \n", tm1, actual_filename, count);
    #endif

    ret = __real_write(fd, buf, count);
    tm2 = recorder_wtime();

    return(ret);
}

size_t RECORDER_DECL(fwrite)(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t ret;
    int aligned_flag = 0;
    double tm1, tm2;

    MAP_OR_FAIL(fwrite);

    if((unsigned long)ptr % recorder_mem_alignment == 0)
        aligned_flag = 1;

    #ifndef DISABLE_POSIX_TRACE
    tm1 = recorder_wtime();

    if(__recorderfh != NULL)
        fprintf(__recorderfh,"%.5f fwrite(ptr, %d, %d, %d) \n",tm1, size, nmemb, fileno(stream));
    #endif

    ret = __real_fwrite(ptr, size, nmemb, stream);
    tm2 = recorder_wtime();

    return(ret);
}

off64_t RECORDER_DECL(lseek64)(int fd, off64_t offset, int whence)
{
    off64_t ret;
    struct recorder_file_runtime* file;
    double tm1, tm2;

    MAP_OR_FAIL(lseek64);

    tm1 = recorder_wtime();
    ret = __real_lseek64(fd, offset, whence);
    tm2 = recorder_wtime();

    return(ret);
}

off_t RECORDER_DECL(lseek)(int fd, off_t offset, int whence)
{
    off_t ret;
    struct recorder_file_runtime* file;
    double tm1, tm2;

    MAP_OR_FAIL(lseek);

    #ifndef DISABLE_POSIX_TRACE
    tm1 = recorder_wtime();
    if(__recorderfh != NULL)
        fprintf(__recorderfh,"%.5f lseek(fd, offset, whence) \n", tm1);
    #endif
    ret = __real_lseek(fd, offset, whence);
    tm2 = recorder_wtime();

    return(ret);
}

int RECORDER_DECL(fseek)(FILE *stream, long offset, int whence)
{
    int ret;
    struct recorder_file_runtime* file;
    double tm1, tm2;

    MAP_OR_FAIL(fseek);

    #ifndef DISABLE_POSIX_TRACE
    tm1 = recorder_wtime();

    if(__recorderfh != NULL)
        fprintf(__recorderfh,"%.5f fseek(*stream, offset, whence) \n", tm1);
    #endif

    ret = __real_fseek(stream, offset, whence);
    tm2 = recorder_wtime();

    return(ret);
}

static double posix_wtime(void)
{
    return RECORDER_MPI_CALL(PMPI_Wtime)();
}


double recorder_wtime(void)
{
    return ((int) time(NULL) * 1.0);

    //return 0;
    /*if(!recorder_global_job || recorder_global_job->flags & CP_FLAG_NOTIMING)
    {
        return(0);
    }

    return(posix_wtime());
    */
}
/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
