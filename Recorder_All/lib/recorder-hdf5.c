/*
 *  (C) 2009 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "recorder-runtime-config.h"
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <limits.h>
#include "mpi.h"
#include "recorder.h"
#include "hdf5.h"       // HUONGLUU: add include hdf5 in order to define wrapped hdf5 functions

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

// HUONGLUU: need to define function ID here
char *func_list[] = {
"H5Fcreate", // File interface
"H5Fopen",
"H5Fclose",
"H5Gclose",// Group interface
"H5Gcreate2",
"H5Gget_objinfo",
"H5Giterate",
"H5Gopen2",
"H5Dclose",// Dataset interface
"H5Dcreate1",
"H5Dcreate2",
"H5Dget_create_plist",
"H5Dget_space",
"H5Dget_type",
"H5Dopen2",
"H5Dread",
"H5Dwrite",
"H5Sclose",// Dataspace interface
"H5Screate",
"H5Screate_simple",
"H5Sget_select_npoints",
"H5Sget_simple_extent_dims",
"H5Sget_simple_extent_npoints",
"H5Sselect_elements",
"H5Sselect_hyperslab",
"H5Sselect_none",
"H5Tclose", // Datatype interface
"H5Tcopy",
"H5Tget_class",
"H5Tget_size",
"H5Tset_size",
"H5Aclose",//Attribute interface
"H5Acreate2",
"H5Aget_name",
"H5Aget_num_attrs",
"H5Aget_space",
"H5Aget_type",
"H5Aopen",
"H5Aopen_idx",
"H5Aopen_name",
"H5Aread",
"H5Awrite",
"H5Pclose",// Property List interface
"H5Pcreate",
"H5Pget_chunk",
"H5Pget_mdc_config",
"H5Pset_alignment",
"H5Pset_chunk",
"H5Pset_dxpl_mpio",
"H5Pset_fapl_core",
"H5Pset_fapl_mpio",
"H5Pset_fapl_mpiposix",
"H5Pset_istore_k",
"H5Pset_mdc_config",
"H5Pset_meta_block_size",
"H5Lexists",// Link interface
"H5Lget_val",
"H5Literate",
"H5Oclose",// Object interface
"H5Oget_info",
"H5Oget_info_by_name",
"H5Oopen",
"ENDLIST"
};

int get_func_id(const char* func_name){
    int i=0;
    while (func_list[i] != "ENDLIST") {
        if (func_list[i] == func_name)
            return i;
        else
            i++;
    }
    return -1;
}
/* HUONGLUU: start - Define HDF5 functions of interest */
/* NOTE: using HDF5 1.8 version */

/* File Interface */
RECORDER_FORWARD_DECL(H5Fcreate, hid_t, (const char *filename, unsigned flags, hid_t create_plist, hid_t access_plist));
RECORDER_FORWARD_DECL(H5Fopen, hid_t, (const char *filename, unsigned flags, hid_t access_plist));
RECORDER_FORWARD_DECL(H5Fclose, herr_t, (hid_t file_id));

/* Group Interface */
RECORDER_FORWARD_DECL(H5Gclose, herr_t, (hid_t group_id));
RECORDER_FORWARD_DECL(H5Gcreate2, hid_t, (hid_t loc_id, const char *name, hid_t lcpl_id, hid_t gcpl_id, hid_t gapl_id));
RECORDER_FORWARD_DECL(H5Gget_objinfo, herr_t, (hid_t loc_id, const char *name, hbool_t follow_link, H5G_stat_t *statbuf));
RECORDER_FORWARD_DECL(H5Giterate, int, (hid_t loc_id, const char *name, int *idx, H5G_iterate_t operator, void *operator_data));
RECORDER_FORWARD_DECL(H5Gopen2, hid_t, (hid_t loc_id, const char * name, hid_t gapl_id));

/* Dataset Interface  */
RECORDER_FORWARD_DECL(H5Dclose,herr_t,(hid_t dataset_id));
RECORDER_FORWARD_DECL(H5Dcreate1, hid_t, (hid_t loc_id, const char *name, hid_t type_id, hid_t space_id, hid_t dcpl_id));
RECORDER_FORWARD_DECL(H5Dcreate2, hid_t, (hid_t loc_id, const char *name, hid_t dtype_id, hid_t space_id, hid_t lcpl_id, hid_t dcpl_id, hid_t dapl_id));
RECORDER_FORWARD_DECL(H5Dget_create_plist, hid_t, (hid_t dataset_id));
RECORDER_FORWARD_DECL(H5Dget_space, hid_t, (hid_t dataset_id));
RECORDER_FORWARD_DECL(H5Dget_type, hid_t, (hid_t dataset_id));
RECORDER_FORWARD_DECL(H5Dopen2, hid_t, (hid_t loc_id, const char *name, hid_t dapl_id));
RECORDER_FORWARD_DECL(H5Dread, herr_t, (hid_t dataset_id, hid_t mem_type_id, hid_t mem_space_id, hid_t file_space_id, hid_t xfer_plist_id, void *buf));
RECORDER_FORWARD_DECL(H5Dwrite, herr_t, (hid_t dataset_id, hid_t mem_type_id, hid_t mem_space_id, hid_t file_space_id, hid_t xfer_plist_id, const void * buf));

/* Dataspace Interface */
RECORDER_FORWARD_DECL(H5Sclose, herr_t, (hid_t space_id));

RECORDER_FORWARD_DECL(H5Screate, hid_t, (H5S_class_t type));

RECORDER_FORWARD_DECL(H5Screate_simple, hid_t, (int rank, const hsize_t * current_dims, const hsize_t * maximum_dims));
RECORDER_FORWARD_DECL(H5Sget_select_npoints, hssize_t, ( hid_t space_id ));
RECORDER_FORWARD_DECL(H5Sget_simple_extent_dims, int, (hid_t space_id, hsize_t *dims, hsize_t *maxdims ));
RECORDER_FORWARD_DECL(H5Sget_simple_extent_npoints, hssize_t, ( hid_t space_id ));
RECORDER_FORWARD_DECL(H5Sselect_elements, herr_t, ( hid_t space_id, H5S_seloper_t op, size_t num_elements, const hsize_t *coord ));
RECORDER_FORWARD_DECL(H5Sselect_hyperslab, herr_t, (hid_t space_id, H5S_seloper_t op, const hsize_t *start, const hsize_t *stride, const hsize_t *count, const hsize_t *block ));
RECORDER_FORWARD_DECL(H5Sselect_none,herr_t, (hid_t space_id));

/* Datatype Interface */

RECORDER_FORWARD_DECL(H5Tclose, herr_t, ( hid_t dtype_id ));
RECORDER_FORWARD_DECL(H5Tcopy, hid_t, ( hid_t dtype_id ));
RECORDER_FORWARD_DECL(H5Tget_class, H5T_class_t, ( hid_t dtype_id ));
RECORDER_FORWARD_DECL(H5Tget_size, size_t, ( hid_t dtype_id ));
RECORDER_FORWARD_DECL(H5Tset_size, herr_t, ( hid_t dtype_id, size_t size ));

/* Attribute Interface */

RECORDER_FORWARD_DECL(H5Aclose, herr_t,(hid_t attr_id));
RECORDER_FORWARD_DECL(H5Acreate2, hid_t, ( hid_t loc_id, const char *attr_name, hid_t type_id, hid_t space_id, hid_t acpl_id, hid_t aapl_id));
RECORDER_FORWARD_DECL(H5Aget_name, ssize_t, (hid_t attr_id, size_t buf_size, char *buf ));
RECORDER_FORWARD_DECL(H5Aget_num_attrs, int, ( hid_t loc_id ) );
RECORDER_FORWARD_DECL(H5Aget_space, hid_t, (hid_t attr_id));
RECORDER_FORWARD_DECL(H5Aget_type, hid_t, (hid_t attr_id));
RECORDER_FORWARD_DECL(H5Aopen, hid_t, ( hid_t obj_id, const char *attr_name, hid_t aapl_id ));
RECORDER_FORWARD_DECL(H5Aopen_idx, hid_t, ( hid_t loc_id, unsigned int idx ));
RECORDER_FORWARD_DECL(H5Aopen_name, hid_t, ( hid_t loc_id, const char *name ));
RECORDER_FORWARD_DECL(H5Aread, herr_t, (hid_t attr_id, hid_t mem_type_id, void *buf ));
RECORDER_FORWARD_DECL(H5Awrite, herr_t, (hid_t attr_id, hid_t mem_type_id, const void *buf ));

/* Property List Interface */
RECORDER_FORWARD_DECL(H5Pclose, herr_t, (hid_t plist ));
RECORDER_FORWARD_DECL(H5Pcreate, hid_t, ( hid_t cls_id ));
RECORDER_FORWARD_DECL(H5Pget_chunk, int, (hid_t plist, int max_ndims, hsize_t * dims ));
RECORDER_FORWARD_DECL(H5Pget_mdc_config, herr_t, (hid_t plist_id, H5AC_cache_config_t *config_ptr));
RECORDER_FORWARD_DECL(H5Pset_alignment, herr_t, (hid_t plist, hsize_t threshold, hsize_t alignment ));
RECORDER_FORWARD_DECL(H5Pset_chunk, herr_t, (hid_t plist, int ndims, const hsize_t * dim ));
RECORDER_FORWARD_DECL(H5Pset_dxpl_mpio, herr_t, ( hid_t dxpl_id, H5FD_mpio_xfer_t xfer_mode ));
RECORDER_FORWARD_DECL(H5Pset_fapl_core, herr_t, ( hid_t fapl_id, size_t increment, hbool_t backing_store ));
RECORDER_FORWARD_DECL(H5Pset_fapl_mpio, herr_t, ( hid_t fapl_id, MPI_Comm comm, MPI_Info info ));
RECORDER_FORWARD_DECL(H5Pset_fapl_mpiposix, herr_t, ( hid_t fapl_id, MPI_Comm comm, hbool_t use_gpfs_hints ));
RECORDER_FORWARD_DECL(H5Pset_istore_k, herr_t, (hid_t plist, unsigned ik ));
RECORDER_FORWARD_DECL(H5Pset_mdc_config, herr_t, (hid_t plist_id, H5AC_cache_config_t *config_ptr));
RECORDER_FORWARD_DECL(H5Pset_meta_block_size, herr_t, ( hid_t fapl_id, hsize_t size ));

/* Link Interface */
RECORDER_FORWARD_DECL(H5Lexists, htri_t, ( hid_t loc_id, const char *name, hid_t lapl_id ));
RECORDER_FORWARD_DECL(H5Lget_val, herr_t, ( hid_t link_loc_id, const char *link_name, void *linkval_buff, size_t size, hid_t lapl_id ));
RECORDER_FORWARD_DECL(H5Literate, herr_t, ( hid_t group_id, H5_index_t index_type, H5_iter_order_t order, hsize_t *idx, H5L_iterate_t op, void *op_data ));

/* Object Interface */
RECORDER_FORWARD_DECL(H5Oclose, herr_t, ( hid_t object_id ));
RECORDER_FORWARD_DECL(H5Oget_info, herr_t, ( hid_t object_id, H5O_info_t *object_info ));
RECORDER_FORWARD_DECL(H5Oget_info_by_name, herr_t, ( hid_t loc_id, const char *object_name, H5O_info_t *object_info, hid_t lapl_id ));
RECORDER_FORWARD_DECL(H5Oopen, hid_t, ( hid_t loc_id, const char *name, hid_t lapl_id ));


/* HUONGLUU: end - Define HDF5 functions of interest */


static struct recorder_file_runtime* recorder_file_by_hid(int hid);


/* HUONGLUU: start - Implement wrapped versions of functions */

hid_t RECORDER_DECL(H5Fcreate)(const char *filename, unsigned flags,
    hid_t create_plist, hid_t access_plist)
{
    hid_t ret;
    #ifndef DISABLE_HDF5_TRACE
    double tm1,tm2;

    MAP_OR_FAIL(H5Fcreate);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Fcreate(%s %d %d %d)\n",tm1, filename, flags, create_plist, access_plist);
    }
    #endif
    #endif

    ret = __real_H5Fcreate(filename, flags, create_plist, access_plist);

    /*
    tm2 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f> \n", ret, tm2-tm1);
    }
    */

    return(ret);
}

hid_t RECORDER_DECL(H5Fopen)(const char *filename, unsigned flags,
    hid_t access_plist)
{
    hid_t ret;
    double tm1,tm2;

    MAP_OR_FAIL(H5Fopen);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Fopen(%s,%d,%d)\n",tm1, filename,flags,access_plist);
    }
    #endif

    ret = __real_H5Fopen(filename, flags, access_plist);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f> \n", ret, tm2-tm1);
    }
    */

    return(ret);
}

herr_t RECORDER_DECL(H5Fclose)(hid_t file_id)
{
    herr_t ret;
    double tm1,tm2;

    MAP_OR_FAIL(H5Fclose);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Fclose(%d)\n",tm1,file_id);
    }
    #endif

    ret = __real_H5Fclose(file_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

// Group Interface
herr_t RECORDER_DECL(H5Gclose)(hid_t group_id)
{
    herr_t ret;
    double tm1,tm2;

    MAP_OR_FAIL(H5Gclose);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Gclose(%d)\n",tm1,group_id);
    }
    #endif

    ret = __real_H5Gclose(group_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

hid_t RECORDER_DECL(H5Gcreate2)(hid_t loc_id, const char *name, hid_t lcpl_id, hid_t gcpl_id, hid_t gapl_id)
{
    hid_t ret;
    double tm1,tm2;

    MAP_OR_FAIL(H5Gcreate2);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Gcreate2(%d,%s,%d,%d,%d)\n",tm1,loc_id,name,lcpl_id,gcpl_id,gapl_id);
    }
    #endif

    ret = __real_H5Gcreate2(loc_id,name,lcpl_id,gcpl_id,gapl_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}


herr_t RECORDER_DECL(H5Gget_objinfo)(hid_t loc_id, const char *name, hbool_t follow_link, H5G_stat_t *statbuf)
{
    herr_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Gget_objinfo);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Gget_objinfo(%d,%s,%d,statbuf)\n",tm1, loc_id, name, follow_link);
    }
    #endif

    ret = __real_H5Gget_objinfo(loc_id,name, follow_link,statbuf);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

int RECORDER_DECL(H5Giterate)(hid_t loc_id, const char *name, int *idx, H5G_iterate_t operator, void *operator_data)
{
    int ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Giterate);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Giterate(%d,%s,%d,%d, %d)\n",tm1,loc_id,name,idx, operator, operator_data);
    }
    #endif

    ret = __real_H5Giterate(loc_id,name,idx, operator, operator_data);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}
hid_t RECORDER_DECL(H5Gopen2)(hid_t loc_id, const char * name, hid_t gapl_id)
{
    hid_t ret;
    double tm1,tm2,duration;
    MAP_OR_FAIL(H5Gopen2);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Gopen2(%d,%s,%d)\n",tm1,loc_id,name,gapl_id);
    }
    #endif

    ret = __real_H5Gopen2(loc_id,name,gapl_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

// Dataset interface
herr_t RECORDER_DECL(H5Dclose)(hid_t dataset_id){
    herr_t ret;
    double tm1,tm2,duration;

    MAP_OR_FAIL(H5Dclose);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Dclose(%d)\n",tm1,dataset_id);
    }
    #endif

    ret = __real_H5Dclose(dataset_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

hid_t RECORDER_DECL(H5Dcreate1)(hid_t loc_id, const char *name, hid_t type_id, hid_t space_id, hid_t dcpl_id)
{
    hid_t ret;
    double tm1,tm2,duration;

    MAP_OR_FAIL(H5Dcreate1);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Dcreate1(%d,%s,%d,%d,%d)\n",tm1,loc_id,name,type_id,space_id,dcpl_id);
    }
    #endif

    ret = __real_H5Dcreate1(loc_id,name,type_id,space_id,dcpl_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return ret;
}

hid_t RECORDER_DECL(H5Dcreate2)(hid_t loc_id, const char *name, hid_t dtype_id, hid_t space_id, hid_t lcpl_id, hid_t dcpl_id, hid_t dapl_id)
{
    hid_t ret;
    double tm1,tm2;

    MAP_OR_FAIL(H5Dcreate2);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();

    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Dcreate2(%d,%s,%d,%d,%d,%d,%d)\n",tm1,loc_id,name,dtype_id,space_id,lcpl_id,dcpl_id,dapl_id);
    }
    #endif

    ret = __real_H5Dcreate2(loc_id,name,dtype_id,space_id,lcpl_id,dcpl_id,dapl_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return ret;
}

hid_t RECORDER_DECL(H5Dget_create_plist)(hid_t dataset_id)
{
    hid_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Dget_create_plist);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Dget_create_plist(%d)\n",tm1,dataset_id);
    }
    #endif

    ret = __real_H5Dget_create_plist(dataset_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return ret;
}

hid_t RECORDER_DECL(H5Dget_space)(hid_t dataset_id)
{
    hid_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Dget_space);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Dget_space(%d)\n",tm1,dataset_id);
    }
    #endif

    ret = __real_H5Dget_space(dataset_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

hid_t RECORDER_DECL(H5Dget_type)(hid_t dataset_id)
{
    hid_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Dget_type);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Dget_type(%d)\n",tm1,dataset_id);
    }
    #endif

    ret = __real_H5Dget_type(dataset_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

hid_t RECORDER_DECL(H5Dopen2)(hid_t loc_id, const char *name, hid_t dapl_id)
{
    hid_t ret;
    double tm1,tm2;

    MAP_OR_FAIL(H5Dopen2);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Dopen2(%d,%s,%d)\n",tm1,loc_id,name,dapl_id);
    }
    #endif

    ret = __real_H5Dopen2(loc_id,name,dapl_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}


herr_t RECORDER_DECL(H5Dread)(hid_t dataset_id, hid_t mem_type_id, hid_t mem_space_id, hid_t file_space_id, hid_t xfer_plist_id, void *buf)
{
    herr_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Dread);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    hssize_t npoints = H5Sget_select_npoints(mem_space_id);
    size_t size_of_data_type = H5Tget_size(mem_type_id);
    long long total_size_read = npoints * size_of_data_type;
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Dread(%d,%d,%d,%d,%d,%lld)\n",tm1,dataset_id,mem_type_id,mem_space_id,file_space_id,xfer_plist_id, total_size_read);
    }
    #endif

    ret = __real_H5Dread(dataset_id,mem_type_id,mem_space_id,file_space_id,xfer_plist_id, buf);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

herr_t RECORDER_DECL(H5Dwrite)(hid_t dataset_id, hid_t mem_type_id, hid_t mem_space_id, hid_t file_space_id, hid_t xfer_plist_id, const void *buf)
{
    herr_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Dwrite);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    /*hssize_t npoints = H5Sget_select_npoints(mem_space_id);
    size_t size_of_data_type = H5Tget_size(mem_type_id);
    long long total_size_written = npoints * size_of_data_type;*/
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Dwrite(%d,%d,%d,%d,%d)\n",tm1,dataset_id,mem_type_id,mem_space_id,file_space_id,xfer_plist_id);
    }
    #endif

    ret = __real_H5Dwrite(dataset_id,mem_type_id,mem_space_id,file_space_id,xfer_plist_id, buf);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

// Dataspace interface
herr_t RECORDER_DECL(H5Sclose)(hid_t space_id){
    herr_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Sclose);

    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Sclose(%d)\n",tm1,space_id);
    }
    #endif

    ret = __real_H5Sclose(space_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

hid_t RECORDER_DECL(H5Screate)(H5S_class_t type){
    hid_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Screate);

    tm1 = recorder_wtime();
    ret = __real_H5Screate(type);
    /*
    tm2 = recorder_wtime();

    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Screate(%d) = %d <%.5f>\\n",tm1,type,ret,tm2-tm1);
    }
    */

    return(ret);
}

hid_t RECORDER_DECL(H5Screate_simple)(int rank, const hsize_t * current_dims, const hsize_t * maximum_dims){
    hid_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Screate_simple);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
            fprintf(__recorderfh,"%.5f H5Screate_simple(%d,current_dims,maximum_dims)\n",tm1, rank);
    }
    #endif

    ret = __real_H5Screate_simple(rank,current_dims,maximum_dims);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    #if 0
    if (__recorderfh != NULL) {
        if(current_dims != NULL && maximum_dims != NULL) {
            char* cur_dims_str = (char *) malloc(sizeof(char) * 128);
            char* max_dims_str = (char *) malloc(sizeof(char) * 128);
            char* tmp1 = (char *) malloc(sizeof(char) * 128);
            char* tmp2 = (char *) malloc(sizeof(char) * 128);
            int i;

            char null_str[] = "";
            strcpy(cur_dims_str, null_str);
            strcpy(max_dims_str, null_str);
            for(i = 0; i < rank; i++) {
                sprintf(tmp1, "%d", current_dims[i]);
                strcat(cur_dims_str, tmp1);
                sprintf(tmp2, "%d", maximum_dims[i]);
                strcat(max_dims_str, tmp2);
                if(i != rank - 1) {
                    strcat(cur_dims_str, ",");
                    strcat(max_dims_str, ",");
                }
            }
            free(cur_dims_str);
            free(max_dims_str);
            free(tmp1);
            free(tmp2);

            fprintf(__recorderfh,"%.5f H5Screate_simple(%d,%d,%d) = %d <%.5f>\\n",tm1, rank, current_dims[0], maximum_dims[0], ret, tm2-tm1);

        }
        else if(current_dims != NULL) {
            char* cur_dims_str = (char *) malloc(sizeof(char) * 128);
            char* tmp1 = (char *) malloc(sizeof(char) * 128);
            int i;

            char null_str[] = "";
            strcpy(cur_dims_str, null_str);
            for(i = 0; i < rank; i++) {
                sprintf(tmp1, "%d", current_dims[i]);
                strcat(cur_dims_str, tmp1);
                if(i != rank - 1) {
                    strcat(cur_dims_str, ",");
                }
            }
            free(cur_dims_str);
            free(tmp1);
            /*
            fprintf(__recorderfh,"%.5f H5Screate_simple(%d,%d,NULL) = %d <%.5f>\\n",tm1, rank, current_dims[0], ret, tm2-tm1);

        }
        else {
            fprintf(__recorderfh,"%.5f H5Screate_simple(%d,NULL,NULL) = %d <%.5f>\\n",tm1, rank, ret, tm2-tm1);
        }
        */
    }
    #endif

    return(ret);
}

hssize_t RECORDER_DECL(H5Sget_select_npoints)(hid_t space_id){
    hssize_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Sget_select_npoints);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Sget_select_npoints(%d)\n",tm1,space_id);
    }
    #endif

    ret = __real_H5Sget_select_npoints(space_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return ret;
}

int RECORDER_DECL(H5Sget_simple_extent_dims)(hid_t space_id, hsize_t *dims, hsize_t *maxdims ){
    int ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Sget_simple_extent_dims);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Sget_simple_extent_dims(%d,%d,%d)\n",tm1,space_id, dims,maxdims);
    }
    #endif

    ret = __real_H5Sget_simple_extent_dims(space_id, dims,maxdims);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

hssize_t RECORDER_DECL(H5Sget_simple_extent_npoints)( hid_t space_id ){
    hssize_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Sget_simple_extent_npoints);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Sget_simple_extent_npoints(%d)\n",tm1,space_id);
    }
    #endif

    ret = __real_H5Sget_simple_extent_npoints(space_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return ret;
}

herr_t RECORDER_DECL(H5Sselect_elements)( hid_t space_id, H5S_seloper_t op, size_t num_elements, const hsize_t *coord){
    herr_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Sselect_elements);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Sselect_elements(%d,%d,%d,%d)\n",tm1,space_id,op, num_elements, coord);
    }
    #endif

    ret = __real_H5Sselect_elements(space_id, op, num_elements, coord);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

herr_t RECORDER_DECL(H5Sselect_hyperslab)(hid_t space_id, H5S_seloper_t op, const hsize_t *start, const hsize_t *stride, const hsize_t *count, const hsize_t *block ){
    herr_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Sselect_hyperslab);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Sselect_hyperslab(%d,%d,start,stride,count,block)\n",tm1,space_id, op);
    }
    #endif

    ret = __real_H5Sselect_hyperslab(space_id, op, start, stride, count, block);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */


    #if 0
    // The start, stride, count, and block arrays must be the same size as the rank of the dataspace.
    int rank = H5Sget_simple_extent_ndims(space_id);

    if (__recorderfh != NULL)
    {
        char* start_str = (char *) malloc(sizeof(char) * 128);
        char* stride_str = (char *) malloc(sizeof(char) * 128);
        char* count_str = (char *) malloc(sizeof(char) * 128);
        char* block_str = (char *) malloc(sizeof(char) * 128);
        char* tmp = (char *) malloc(sizeof(char) * 128);
        int i;

        char null_str[] = "";
        char zero_str[] = "0";
        strcpy(start_str, null_str);
        strcpy(stride_str, null_str);
        strcpy(count_str, null_str);
        strcpy(block_str, null_str);
        for(i = 0; i < rank; i++) {
            strcpy(tmp, zero_str);
            if(start != NULL)
                sprintf(tmp, "%d", start[i]);
            strcat(start_str, tmp);

            strcpy(tmp, zero_str);
            if(stride != NULL)
                sprintf(tmp, "%d", stride[i]);
            strcat(stride_str, tmp);

            strcpy(tmp, zero_str);
            if(count != NULL)
                sprintf(tmp, "%d", count[i]);
            strcat(count_str, tmp);

            strcpy(tmp, zero_str);
            if(block != NULL)
                sprintf(tmp, "%d", block[i]);
            strcat(block_str, tmp);

            if(i != rank - 1) {
                strcat(start_str, ",");
                strcat(stride_str, ",");
                strcat(count_str, ",");
                strcat(block_str, ",");
            }
        }

        fprintf(__recorderfh,"%.5f H5Sselect_hyperslab(%d,%d,%s,%s,%s,%s) = %d <%.5f>\\n",tm1,space_id, op, start_str, stride_str, count_str, block_str, ret, tm2-tm1);
    }
    #endif

    return(ret);
}

herr_t RECORDER_DECL(H5Sselect_none)(hid_t space_id){
    herr_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Sselect_none);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Sselect_none(%d)\n",tm1,space_id);
    }
    #endif

    ret = __real_H5Sselect_none(space_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

/* Datatype Interface */
herr_t RECORDER_DECL(H5Tclose)( hid_t dtype_id ) {
    herr_t ret;
    double tm1,tm2;

    MAP_OR_FAIL(H5Tclose);

    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Tclose(%d)\n",tm1,dtype_id);
    }
    #endif

    ret = __real_H5Tclose(dtype_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

hid_t RECORDER_DECL(H5Tcopy)( hid_t dtype_id ){
    hid_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Tcopy);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Tcopy(%d)\n",tm1,dtype_id);
    }
    #endif

    ret = __real_H5Tcopy(dtype_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

H5T_class_t RECORDER_DECL(H5Tget_class)( hid_t dtype_id ){
    H5T_class_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Tget_class);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Tget_class(%d)\n",tm1,dtype_id);
    }
    #endif

    ret = __real_H5Tget_class(dtype_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

size_t RECORDER_DECL(H5Tget_size)( hid_t dtype_id ){
    size_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Tget_size);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Tget_size(%d)\n",tm1,dtype_id);
    }
    #endif

    ret = __real_H5Tget_size(dtype_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return ret;
}

herr_t RECORDER_DECL(H5Tset_size)( hid_t dtype_id, size_t size ){
    herr_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Tset_size);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Tset_size(%d,%d)\n",tm1,dtype_id,size);
    }
    #endif

    ret = __real_H5Tset_size(dtype_id,size);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

/* Attribute Interface */
herr_t RECORDER_DECL(H5Aclose)(hid_t attr_id){
    herr_t ret;
    double tm1,tm2,duration;
    MAP_OR_FAIL(H5Aclose);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Aclose(%d)\n",tm1,attr_id);
    }
    #endif

    ret = __real_H5Aclose(attr_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

hid_t RECORDER_DECL(H5Acreate2)( hid_t loc_id, const char *attr_name, hid_t type_id, hid_t space_id, hid_t acpl_id, hid_t aapl_id)
{
    hid_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Acreate2);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Acreate2(%d,%s,%d,%d,%d,%d)\n",tm1,loc_id, attr_name, type_id, space_id, acpl_id, aapl_id);
    }
    #endif

    ret = __real_H5Acreate2(loc_id, attr_name, type_id, space_id, acpl_id, aapl_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}


ssize_t RECORDER_DECL(H5Aget_name)(hid_t attr_id, size_t buf_size, char *buf ){
    ssize_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Aget_name);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Aget_name(%d,%d,%s)\n",tm1,attr_id, buf_size, buf);
    }
    #endif

    ret = __real_H5Aget_name(attr_id, buf_size, buf);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

int RECORDER_DECL(H5Aget_num_attrs)( hid_t loc_id ){
    int ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Aget_num_attrs);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Aget_num_attrs(%d)\n",tm1,loc_id);
    }
    #endif

    ret = __real_H5Aget_num_attrs(loc_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}



hid_t RECORDER_DECL(H5Aget_space)(hid_t attr_id){
    hid_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Aget_space);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Aget_space(%d)\n",tm1,attr_id);
    }
    #endif

    ret = __real_H5Aget_space(attr_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

hid_t RECORDER_DECL(H5Aget_type)(hid_t attr_id){
    hid_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Aget_type);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Aget_type(%d)\n",tm1,attr_id);
    }
    #endif

    ret = __real_H5Aget_type(attr_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

hid_t RECORDER_DECL(H5Aopen)( hid_t obj_id, const char *attr_name, hid_t aapl_id ){
    hid_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Aopen);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Aopen(%d,%s,%d)\n",tm1,obj_id, attr_name, aapl_id);
    }
    #endif

    ret = __real_H5Aopen(obj_id, attr_name, aapl_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

hid_t RECORDER_DECL(H5Aopen_idx)( hid_t loc_id, unsigned int idx ){
    hid_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Aopen_idx);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Aopen_idx(%d,%d)\n",tm1,loc_id, idx);
    }
    #endif

    ret = __real_H5Aopen_idx(loc_id, idx);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}


hid_t RECORDER_DECL(H5Aopen_name)( hid_t loc_id, const char *name ){
    hid_t ret;
    double tm1,tm2;

    MAP_OR_FAIL(H5Aopen_name);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Aopen_name(%d,%s)\n",tm1,loc_id, name);
    }
    #endif

    ret = __real_H5Aopen_name(loc_id,name);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

herr_t RECORDER_DECL(H5Aread)(hid_t attr_id, hid_t mem_type_id, void *buf ){
    herr_t ret;
    double tm1, tm2;
    MAP_OR_FAIL(H5Aread);

    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Aread(%d,%d,%d)\n",tm1,attr_id, mem_type_id, buf);
    }
    #endif

    ret = __real_H5Aread(attr_id, mem_type_id, buf);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

herr_t RECORDER_DECL(H5Awrite)(hid_t attr_id, hid_t mem_type_id,const void *buf ){
    herr_t ret;
    double tm1, tm2;

    MAP_OR_FAIL(H5Awrite);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Awrite(%d,%d,%d)\n",tm1,attr_id, mem_type_id, buf);
    }
    #endif

    ret = __real_H5Awrite(attr_id, mem_type_id, buf);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

/* Property List Interface */
herr_t RECORDER_DECL(H5Pclose)(hid_t plist){
    herr_t ret;
    double tm1, tm2;
    MAP_OR_FAIL(H5Pclose);

    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Pclose(%d)\n",tm1,plist);
    }
    #endif

    ret = __real_H5Pclose(plist);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

hid_t RECORDER_DECL(H5Pcreate)(hid_t cls_id){
    hid_t ret;
    double tm1, tm2;
    MAP_OR_FAIL(H5Pcreate);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Pcreate(%d)\n",tm1,cls_id);
    }
    #endif

    ret = __real_H5Pcreate(cls_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

int RECORDER_DECL(H5Pget_chunk)(hid_t plist, int max_ndims, hsize_t * dims){
    int ret;
    double tm1, tm2;
    MAP_OR_FAIL(H5Pget_chunk);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Pget_chunk(%d,%d,%d)\n",tm1,plist, max_ndims, dims);
    }
    #endif

    ret = __real_H5Pget_chunk(plist, max_ndims, dims);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

herr_t RECORDER_DECL(H5Pget_mdc_config)(hid_t plist_id, H5AC_cache_config_t *config_ptr){
    herr_t ret;
    double tm1, tm2;
    MAP_OR_FAIL(H5Pget_mdc_config);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Pget_mdc_config(%d,config_ptr)\n",tm1, plist_id /*,config_ptr,*/);
    }
    #endif

    ret = __real_H5Pget_mdc_config(plist_id, config_ptr);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

herr_t RECORDER_DECL(H5Pset_alignment)(hid_t plist, hsize_t threshold, hsize_t alignment ){
    herr_t ret;
    double tm1, tm2;
    MAP_OR_FAIL(H5Pset_alignment);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();

    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Pset_alignment(%d,%d,%d)\n",tm1,plist, threshold, alignment);
    }
    #endif

    ret = __real_H5Pset_alignment(plist, threshold, alignment);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

herr_t RECORDER_DECL(H5Pset_chunk)(hid_t plist, int ndims, const hsize_t * dim){
    herr_t ret;
    double tm1, tm2;
    MAP_OR_FAIL(H5Pset_chunk);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Pset_chunk(%d,%d,%d)\n",tm1,plist, ndims, ndims * sizeof(hsize_t));
    }
    #endif

    ret = __real_H5Pset_chunk(plist, ndims, dim);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

herr_t RECORDER_DECL(H5Pset_dxpl_mpio)(hid_t dxpl_id, H5FD_mpio_xfer_t xfer_mode){
    herr_t ret;
    double tm1, tm2;
    MAP_OR_FAIL(H5Pset_dxpl_mpio);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Pset_dxpl_mpio(%d,%d)\n",tm1,dxpl_id,xfer_mode);
    }
    #endif

    ret = __real_H5Pset_dxpl_mpio(dxpl_id,xfer_mode);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

herr_t RECORDER_DECL(H5Pset_fapl_core)(hid_t fapl_id, size_t increment, hbool_t backing_store){
    herr_t ret;
    double tm1, tm2;
    MAP_OR_FAIL(H5Pset_fapl_core);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Pset_fapl_core(%d,%d,%d)\n",tm1,fapl_id, increment, backing_store);
    }
    #endif

    ret = __real_H5Pset_fapl_core(fapl_id, increment, backing_store);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

herr_t RECORDER_DECL(H5Pset_fapl_mpio)(hid_t fapl_id, MPI_Comm comm, MPI_Info info){
    herr_t ret;
    double tm1, tm2;
    MAP_OR_FAIL(H5Pset_fapl_mpio);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Pset_fapl_mpio(%d,%d,%d)\n",tm1,fapl_id,comm, info);
    }
    #endif

    ret = __real_H5Pset_fapl_mpio(fapl_id,comm, info);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

herr_t RECORDER_DECL(H5Pset_fapl_mpiposix)( hid_t fapl_id, MPI_Comm comm, hbool_t use_gpfs_hints){
    herr_t ret;
    double tm1, tm2;
    MAP_OR_FAIL(H5Pset_fapl_mpiposix);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Pset_fapl_mpiposix(%d,%d,%d)\n",tm1,fapl_id,comm, use_gpfs_hints);
    }
    #endif

    ret = __real_H5Pset_fapl_mpiposix(fapl_id, comm, use_gpfs_hints);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

herr_t RECORDER_DECL(H5Pset_istore_k)(hid_t plist, unsigned ik){
    herr_t ret;
    double tm1, tm2;
    MAP_OR_FAIL(H5Pset_istore_k);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Pset_istore_k(%d,%d)\n",tm1,plist, ik);
    }
    #endif

    ret = __real_H5Pset_istore_k(plist, ik);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

herr_t RECORDER_DECL(H5Pset_mdc_config)(hid_t plist_id, H5AC_cache_config_t *config_ptr){
    herr_t ret;
    double tm1, tm2;
    MAP_OR_FAIL(H5Pset_mdc_config);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Pset_mdc_config(%d,%d)\n",tm1,plist_id, config_ptr);
    }
    #endif

    ret = __real_H5Pset_mdc_config(plist_id, config_ptr);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

herr_t RECORDER_DECL(H5Pset_meta_block_size)(hid_t fapl_id, hsize_t size){
    herr_t ret;
    double tm1, tm2;
    MAP_OR_FAIL(H5Pset_meta_block_size);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Pset_meta_block_size(%d,%d)\n",tm1,fapl_id,size);
    }
    #endif

    ret = __real_H5Pset_meta_block_size(fapl_id,size);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

/* Link Interface */
htri_t RECORDER_DECL(H5Lexists)(hid_t loc_id, const char *name, hid_t lapl_id){
    herr_t ret;
    double tm1, tm2;
    MAP_OR_FAIL(H5Lexists);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Lexists(%d,%s,%d)\n",tm1,loc_id, name, lapl_id);
    }
    #endif

    ret = __real_H5Lexists(loc_id, name, lapl_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

herr_t RECORDER_DECL(H5Lget_val)(hid_t link_loc_id, const char *link_name, void *linkval_buff, size_t size, hid_t lapl_id ){
    herr_t ret;
    double tm1, tm2;
    MAP_OR_FAIL(H5Lget_val);
    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Lget_val(%d,%s,%d,%d,%d)\n",tm1,link_loc_id, link_name, linkval_buff, size, lapl_id);
    }
    #endif

    ret = __real_H5Lget_val(link_loc_id, link_name, linkval_buff, size, lapl_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

herr_t RECORDER_DECL(H5Literate)(hid_t group_id, H5_index_t index_type, H5_iter_order_t order, hsize_t *idx, H5L_iterate_t op, void *op_data){
    herr_t ret;
    double tm1, tm2;
    MAP_OR_FAIL(H5Literate);

    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Literate(%d,%d,%d,%d,%d,%d)\n",tm1,group_id, index_type,order, idx, op, op_data);
    }
    #endif

    ret = __real_H5Literate(group_id, index_type,order, idx, op, op_data);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

/* Object Interface */
herr_t RECORDER_DECL(H5Oclose)(hid_t object_id){
    herr_t ret;
    double tm1,tm2;

    MAP_OR_FAIL(H5Oclose);

    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Oclose(%d)\n",tm1,object_id);
    }
    #endif

    ret = __real_H5Oclose(object_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

herr_t RECORDER_DECL(H5Oget_info)(hid_t object_id, H5O_info_t *object_info){
    herr_t ret;
    double tm1,tm2;

    MAP_OR_FAIL(H5Oget_info);

    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Oget_info(%d,%d)\n",tm1,object_id,object_info);
    }
    #endif

    ret = __real_H5Oget_info(object_id, object_info);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */
        return(ret);

}

herr_t RECORDER_DECL(H5Oget_info_by_name)(hid_t loc_id, const char *object_name, H5O_info_t *object_info, hid_t lapl_id){
    herr_t ret;
    double tm1,tm2;
    MAP_OR_FAIL(H5Oget_info_by_name);

    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
    if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Oget_info_by_name(%d,%s,%d,%d)\n",tm1,loc_id, object_name, object_info, lapl_id);
    }
    #endif

    ret = __real_H5Oget_info_by_name(loc_id, object_name, object_info, lapl_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

hid_t RECORDER_DECL(H5Oopen)(hid_t loc_id, const char *name, hid_t lapl_id){
    herr_t ret;
    double tm1,tm2;

    MAP_OR_FAIL(H5Oopen);

    #ifndef DISABLE_HDF5_TRACE
    tm1 = recorder_wtime();
if (__recorderfh != NULL) {
        fprintf(__recorderfh,"%.5f H5Oopen(%d,%s,%d)\n",tm1,loc_id,name, lapl_id);
    }
    #endif

    ret = __real_H5Oopen(loc_id,name, lapl_id);

    /*
    tm2 = recorder_wtime();
    if(__recorderfh != NULL) {
        fprintf(__recorderfh, "= %d <%.5f>\n", ret, tm2-tm1);
    }
    */

    return(ret);
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=8 sts=4 sw=4 expandtab
 */
