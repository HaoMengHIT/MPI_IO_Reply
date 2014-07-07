MPI_IO_replay
=============

Replay MPI I/O operation according to the input trace file.

Build
------

.. code:: bash

   $ mkdir build; cd build;
   $ cmake ..
   $ make

.. note::
   Important File

   The ``Recorder_All/func_list`` defines the wrapper function to process

Run
----

output trace:

.. code:: bash

   $ LD_PRELOAD=Recorder_All/librecorder-compress.so mpirun -np [num] [prog]
   $ RECORDER_OUTDIR=<override-dir> LD_PRELOAD=Recorder_All/librecorder-compress.so mpirun -np [num] [prog]
   
compress trace:

.. code:: bash

   $ src/compress <single log file> <output log file>
   
merge traces:

.. code:: bash

   $ src/lcs_trace <log_num> <compressed_log_dir> <output_dir>

compressed_log_dir's filename should keep as compressed_log.*, then it would
write output into ``<output_dir>`` folder.
   
replay to a c code:

.. code:: bash

   $ src/replay <num> <scale> <single aux file> <func_info> <merged_log_dir> <construct_log_dir> 2>/dev/null 1>temp.c

recompile the temp.c:

.. code:: bash

   $ mpicc -std=c99 -O0 temp.c 
