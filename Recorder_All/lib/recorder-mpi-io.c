/* 
 *This file is generated automaticaly by wrapper generator.
 * define neccessary variables below
 */

double tm1, tm2;
int ret, result_len;
char comm_name[100], etype_name[100], filetype_name[100], datatype_name[100];
#define BUFFER_SIZE 4096
const int threshold = BUFFER_SIZE * 0.9;
int bytes, written_bytes;
char rec_buffer[4096];

/*
 *  (C) 2009 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#define _XOPEN_SOURCE 500
#define _GNU_SOURCE /* for tdestroy() */

#include "recorder-runtime-config.h"

#include <stdio.h>
#ifdef HAVE_MNTENT_H
#include <mntent.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <zlib.h>
#include <assert.h>
#include <search.h>

#include "mpi.h"
#include "recorder.h"
#include "recorder-dynamic.h"

extern char* __progname;
FILE *__recorderfh;

double recorder_wtime(void)
{
    return ((int) time(NULL) * 1.0);
}

void recorder_mpi_initialize(int *argc, char ***argv)
{
    int nprocs;
    int rank;

    RECORDER_MPI_CALL(PMPI_Comm_size)(MPI_COMM_WORLD, &nprocs);
    RECORDER_MPI_CALL(PMPI_Comm_rank)(MPI_COMM_WORLD, &rank);

    char *logfile_name;
    char *logdir_name;
    logfile_name=(char*)malloc(PATH_MAX);
    logdir_name=(char*)malloc(PATH_MAX);
    char cuser[L_cuserid] = {0};
    cuserid(cuser);

    sprintf(logdir_name,"%s_%s",cuser, __progname);
    int status;
    status = mkdir(logdir_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    sprintf(logfile_name,"%s/log.%d",logdir_name,rank);
    __recorderfh=fopen(logfile_name,"w");

    printf(" logfile_name %s ,recorderfh %d\n",logfile_name,__recorderfh);

    free(logfile_name);
    free(logdir_name);

    return;
}

void recorder_shutdown(int timing_flag)
{
    fclose(__recorderfh);

    return;
}
int MPI_File_close(MPI_File *fh)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_close)(fh);
	tm2 = recorder_wtime();
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_close(fh=%d)\n",tm1, tm2, fh);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_set_size(MPI_File fh,MPI_Offset size)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_set_size)(fh, size);
	tm2 = recorder_wtime();
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_set_size(fh=%d, size=%d)\n",tm1, tm2, fh, size);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_iread_at(MPI_File fh,MPI_Offset offset,void *buf,int count,MPI_Datatype datatype,__D_MPI_REQUEST *request)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_iread_at)(fh, offset, buf, count, datatype, request);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_iread_at(fh=%d, offset=%d, buf=*buf, count=%d, datatype=%s, request=%d)\n",tm1, tm2, fh, offset, count, datatype_name, request);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_iread(MPI_File fh,void  *buf,int  count,MPI_Datatype  datatype,__D_MPI_REQUEST  *request)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_iread)(fh, buf, count, datatype, request);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_iread(fh=%d, buf=*buf, count=%d, datatype=%s, request=%d)\n",tm1, tm2, fh, count, datatype_name, request);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_iread_shared(MPI_File fh,void *buf,int count,MPI_Datatype datatype,__D_MPI_REQUEST *request)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_iread_shared)(fh, buf, count, datatype, request);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_iread_shared(fh=%d, buf=*buf, count=%d, datatype=%s, request=%d)\n",tm1, tm2, fh, count, datatype_name, request);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_iwrite_at(MPI_File fh,MPI_Offset offset,void *buf,int count,MPI_Datatype datatype,__D_MPI_REQUEST *request)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_iwrite_at)(fh, offset, buf, count, datatype, request);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_iwrite_at(fh=%d, offset=%d, buf=*buf, count=%d, datatype=%s, request=%d)\n",tm1, tm2, fh, offset, count, datatype_name, request);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_iwrite(MPI_File fh,void *buf,int count,MPI_Datatype datatype,__D_MPI_REQUEST *request)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_iwrite)(fh, buf, count, datatype, request);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_iwrite(fh=%d, buf=*buf, count=%d, datatype=%s, request=%d)\n",tm1, tm2, fh, count, datatype_name, request);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_iwrite_shared(MPI_File fh,void *buf,int count,MPI_Datatype datatype,__D_MPI_REQUEST *request)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_iwrite_shared)(fh, buf, count, datatype, request);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_iwrite_shared(fh=%d, buf=*buf, count=%d, datatype=%s, request=%d)\n",tm1, tm2, fh, count, datatype_name, request);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_open(MPI_Comm comm,char *filename,int amode,MPI_Info info,MPI_File *fh)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_open)(comm, filename, amode, info, fh);
	tm2 = recorder_wtime();
PMPI_Comm_get_name(comm, comm_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_open(comm=%s, filename=%s, amode=%d, info=%d, fh=%d)\n",tm1, tm2, comm_name, filename, amode, info, fh);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_read_all_begin(MPI_File fh,void *buf,int count,MPI_Datatype datatype)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_read_all_begin)(fh, buf, count, datatype);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_read_all_begin(fh=%d, buf=*buf, count=%d, datatype=%s)\n",tm1, tm2, fh, count, datatype_name);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_read_all(MPI_File fh,void *buf,int count,MPI_Datatype datatype,MPI_Status *status)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_read_all)(fh, buf, count, datatype, status);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_read_all(fh=%d, buf=*buf, count=%d, datatype=%s, status=%d)\n",tm1, tm2, fh, count, datatype_name, status);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_read_at_all(MPI_File fh,MPI_Offset offset,void *buf,int count,MPI_Datatype datatype,MPI_Status *status)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_read_at_all)(fh, offset, buf, count, datatype, status);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_read_at_all(fh=%d, offset=%d, buf=*buf, count=%d, datatype=%s, status=%d)\n",tm1, tm2, fh, offset, count, datatype_name, status);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_read_at_all_begin(MPI_File fh,MPI_Offset offset,void *buf,int count,MPI_Datatype datatype)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_read_at_all_begin)(fh, offset, buf, count, datatype);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_read_at_all_begin(fh=%d, offset=%d, buf=*buf, count=%d, datatype=%s)\n",tm1, tm2, fh, offset, count, datatype_name);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_read_at(MPI_File fh,MPI_Offset offset,void *buf,int count,MPI_Datatype datatype,MPI_Status *status)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_read_at)(fh, offset, buf, count, datatype, status);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_read_at(fh=%d, offset=%d, buf=*buf, count=%d, datatype=%s, status=%d)\n",tm1, tm2, fh, offset, count, datatype_name, status);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_read(MPI_File fh,void *buf,int count,MPI_Datatype datatype,MPI_Status *status)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_read)(fh, buf, count, datatype, status);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_read(fh=%d, buf=*buf, count=%d, datatype=%s, status=%d)\n",tm1, tm2, fh, count, datatype_name, status);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_read_ordered_begin(MPI_File fh,void *buf,int count,MPI_Datatype datatype)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_read_ordered_begin)(fh, buf, count, datatype);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_read_ordered_begin(fh=%d, buf=*buf, count=%d, datatype=%s)\n",tm1, tm2, fh, count, datatype_name);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_read_ordered(MPI_File fh,void *buf,int count,MPI_Datatype datatype,MPI_Status *status)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_read_ordered)(fh, buf, count, datatype, status);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_read_ordered(fh=%d, buf=*buf, count=%d, datatype=%s, status=%d)\n",tm1, tm2, fh, count, datatype_name, status);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_read_shared(MPI_File fh,void *buf,int count,MPI_Datatype datatype,MPI_Status *status)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_read_shared)(fh, buf, count, datatype, status);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_read_shared(fh=%d, buf=*buf, count=%d, datatype=%s, status=%d)\n",tm1, tm2, fh, count, datatype_name, status);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_set_view(MPI_File fh,MPI_Offset disp,MPI_Datatype etype,MPI_Datatype filetype,char *datarep,MPI_Info info)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_set_view)(fh, disp, etype, filetype, datarep, info);
	tm2 = recorder_wtime();
PMPI_Type_get_name(etype, etype_name, &result_len);
PMPI_Type_get_name(filetype, filetype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_set_view(fh=%d, disp=%d, etype=%s, filetype=%s, datarep=%s, info=%d)\n",tm1, tm2, fh, disp, etype_name, filetype_name, datarep, info);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_sync(MPI_File fh)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_sync)(fh);
	tm2 = recorder_wtime();
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_sync(fh=%d)\n",tm1, tm2, fh);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_write_all_begin(MPI_File fh,void *buf,int count,MPI_Datatype datatype)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_write_all_begin)(fh, buf, count, datatype);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_write_all_begin(fh=%d, buf=*buf, count=%d, datatype=%s)\n",tm1, tm2, fh, count, datatype_name);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_write_all(MPI_File fh,void *buf,int count,MPI_Datatype datatype,MPI_Status *status)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_write_all)(fh, buf, count, datatype, status);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_write_all(fh=%d, buf=*buf, count=%d, datatype=%s, status=%d)\n",tm1, tm2, fh, count, datatype_name, status);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_write_at_all_begin(MPI_File fh,MPI_Offset offset,void *buf,int count,MPI_Datatype datatype)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_write_at_all_begin)(fh, offset, buf, count, datatype);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_write_at_all_begin(fh=%d, offset=%d, buf=*buf, count=%d, datatype=%s)\n",tm1, tm2, fh, offset, count, datatype_name);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_write_at_all(MPI_File fh,MPI_Offset offset,void *buf,int count,MPI_Datatype datatype,MPI_Status *status)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_write_at_all)(fh, offset, buf, count, datatype, status);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_write_at_all(fh=%d, offset=%d, buf=*buf, count=%d, datatype=%s, status=%d)\n",tm1, tm2, fh, offset, count, datatype_name, status);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_write_at(MPI_File fh,MPI_Offset offset,void *buf,int count,MPI_Datatype datatype,MPI_Status *status)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_write_at)(fh, offset, buf, count, datatype, status);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_write_at(fh=%d, offset=%d, buf=*buf, count=%d, datatype=%s, status=%d)\n",tm1, tm2, fh, offset, count, datatype_name, status);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_write(MPI_File fh,void *buf,int count,MPI_Datatype datatype,MPI_Status *status)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_write)(fh, buf, count, datatype, status);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_write(fh=%d, buf=*buf, count=%d, datatype=%s, status=%d)\n",tm1, tm2, fh, count, datatype_name, status);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_write_ordered_begin(MPI_File fh,void *buf,int count,MPI_Datatype datatype)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_write_ordered_begin)(fh, buf, count, datatype);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_write_ordered_begin(fh=%d, buf=*buf, count=%d, datatype=%s)\n",tm1, tm2, fh, count, datatype_name);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_write_ordered(MPI_File fh,void *buf,int count,MPI_Datatype datatype,MPI_Status *status)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_write_ordered)(fh, buf, count, datatype, status);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_write_ordered(fh=%d, buf=*buf, count=%d, datatype=%s, status=%d)\n",tm1, tm2, fh, count, datatype_name, status);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_File_write_shared(MPI_File fh,void *buf,int count,MPI_Datatype datatype,MPI_Status *status)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_File_write_shared)(fh, buf, count, datatype, status);
	tm2 = recorder_wtime();
PMPI_Type_get_name(datatype, datatype_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_File_write_shared(fh=%d, buf=*buf, count=%d, datatype=%s, status=%d)\n",tm1, tm2, fh, count, datatype_name, status);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_Comm_rank(MPI_Comm comm,int *rank)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_Comm_rank)(comm, rank);
	tm2 = recorder_wtime();
PMPI_Comm_get_name(comm, comm_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_Comm_rank(comm=%s, rank=*rank)\n",tm1, tm2, comm_name);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

int MPI_Comm_size(MPI_Comm comm,int *size)
{
	tm1 = recorder_wtime();
	ret = RECORDER_MPI_CALL(PMPI_Comm_size)(comm, size);
	tm2 = recorder_wtime();
PMPI_Comm_get_name(comm, comm_name, &result_len);
	bytes = sprintf(rec_buffer + written_bytes, "tm1=%.5f tm2=%.5f MPI_Comm_size(comm=%s, size=*size)\n",tm1, tm2, comm_name);
	written_bytes += bytes - 1;
	if (written_bytes > threshold) {
		fputs(rec_buffer, __recorderfh);
		written_bytes = 0;
}

	return ret;
}

