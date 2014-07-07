srcdir = .
prefix = /home/zhangyou/work/Recorder_Optimized/lib
#HDF5_DIR = /usr/
exec_prefix = ${prefix}
libdir = $(DESTDIR)${exec_prefix}/lib
#LDFLAGS = -lhdf5    
#-L${HDF5_DIR}/lib -lhdf5
CC = mpic++
LD = @LD@

RECORDER_LOG_FORMAT = $(srcdir)/./recorder-log-format.h

all: lib/librecorder-compress.so

CFLAGS_SHORTENED = -DRECORDER_CONFIG_H=\"recorder-runtime-config.h\" -I . -I$(srcdir) -I$(srcdir)/../ -I${HDF5_DIR}/include  -D_LARGEFILE64_SOURCE -DRECORDER_PRELOAD

CFLAGS_SHARED = -DRECORDER_CONFIG_H=\"recorder-runtime-config.h\" -I . -I$(srcdir) -I$(srcdir)/../ -I${HDF5_DIR}/include  -D_LARGEFILE64_SOURCE -shared -fpic -DPIC -DRECORDER_PRELOAD

#CFLAGS_SHARED = -DRECORDER_CONFIG_H=\"recorder-runtime-config.h\" -I . -I$(srcdir) -I$(srcdir)/../ -I${HDF5_DIR}/include  -D_LARGEFILE64_SOURCE -shared -fpic -DPIC -DRECORDER_PRELOAD -DDISABLE_POSIX_TRACE

#CFLAGS_SHARED = -DRECORDER_CONFIG_H=\"recorder-runtime-config.h\" -I . -I$(srcdir) -I$(srcdir)/../ -I${HDF5_DIR}/include  -D_LARGEFILE64_SOURCE -shared -fpic -DPIC -DRECORDER_PRELOAD -DDISABLE_POSIX_TRACE -DDISABLE_MPIO_TRACE

LIBS = -lz @LIBBZ2@

lib::
	@mkdir -p $@

MIXED_FLAG=-lstdc++
SHARE=-shared -fPIC -DPIC

lib/simple_compress.o: lib/simple_compress.cpp lib/simple_compress.h | lib
	mpic++ -Wall -O2 -std=c++0x $(SHARE) -c $< -o $@

lib/recorder-mpi-io-compress.o: lib/recorder-mpi-io-compress.c recorder-compress.h recorder-dynamic.h $(recorder_LOG_FORMAT) | lib
	mpic++ -c $(CFLAGS_SHARED) $< -o $@

#lib/recorder-mpi-io-compress.po: lib/recorder-mpi-io-compress.o lib/simple_compress.o | lib
#	$(CC) $(MIXED_FLAG) $(CFLAGS_SHARED) $^ -o $@

lib/recorder-mpi-init-finalize.po: lib/recorder-mpi-init-finalize-compress.c recorder-compress.h recorder-dynamic.h $(recorder_LOG_FORMAT) | lib
	mpic++ $(CFLAGS_SHARED) -c $< -o $@

#lib/recorder-pnetcdf.po: lib/recorder-pnetcdf.c recorder.h $(recorder_LOG_FORMAT) | lib
#	$(CC) $(CFLAGS_SHARED) -c $< -o $@

#lib/recorder-hdf5.po: lib/recorder-hdf5.c recorder.h $(recorder_LOG_FORMAT) | lib
#	$(CC) $(CFLAGS_SHARED) -c $< -o $@

#lib/recorder-posix.po: lib/recorder-posix.c recorder.h $(recorder_LOG_FORMAT) | lib
#	mpic++ $(CFLAGS_SHARED) -c $< -o $@

lib/librecorder-compress.so: lib/recorder-mpi-io-compress.o lib/simple_compress.o lib/recorder-mpi-init-finalize.po 
#lib/recorder-posix.po
	#lib/recorder-posix.po
#lib/librecorder.so: lib/recorder-mpi-io.po lib/recorder-mpi-init-finalize.po lib/recorder-pnetcdf.po lib/recorder-hdf5.po lib/recorder-posix.po
	mpic++ $(CFLAGS_SHARED) $(LDFLAGS) $(MIXED_FLAG) -ldl -o $@ $^ -lpthread -lrt -lz

install:: all
	install -d $(libdir)
	install -m 755 lib/librecorder.so $(libdir)

clean::
	rm -f *.o *.a lib/*.o lib/*.po lib/*.a lib/librecorder-compress.so

distclean:: clean
	rm -f recorder-runtime-config.h
	rm -rf autom4te.cache
