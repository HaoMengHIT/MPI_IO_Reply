#ifndef _PREPROCESS_H_
#define _PREPROCESS_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <map>
#include <list>
#include <vector>
#include <cstdlib>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include "datatype.h"

using namespace std;

/*
 * Read raw trace file and convert.
 */
class Preprocess {
private:
	string filename;

	// the inner maps is used to match arguments like fh and request
	map<string, map<string, unsigned int> > argument_maps;

	// the data structure stores each function's name, paras etc
	str_hmap_list all_data;
	str_hmap_list auxiliary_data;	// for time info or sth else we're interested

	int extract_data_from_single_line(string & line, int pos);

	unsigned int build_match(string& key, string& value, bool first);
	bool check_io(string& func_name, string& offset);

	bool first; // this state should be true if and only if it is reading the raw logs!
	int procs;
	int rank;	// the value of rank relies on the last number of filename!
	int state;	// reorder arg, ugly..
	int counter;
	int barrier;

	string last_io;
	unsigned long last_offset;
	unsigned long offset_dis;


public:
   /* read from file and build memory structure
    * @param filename_para: the log's filename
    * @param nprocs_para: there are totaly how many files
    * @param rank_para: this file's id suffix (rank means the mpi program's
    *    rank, Recorder would write rank id at end of file name, such as log.4.
    *    so rank here is 4)
    * @param raw: reading the uncompressed raw log data.
    */
	Preprocess(string filename_para, int nprocs_para, int rank_para, bool raw = false);

   /* change the file before run */
	void changefile(string file_name_para);

   /* do real process and get data 
    * @return 0 --> success
    *         1 --> failed
    */
	int run();

	const str_hmap_list & get_data(void) const {
      return all_data;
   }
	const str_hmap_list & get_auxiliary(void) const {
      return auxiliary_data; 
   }
};


/* print log data as well as aux data */
int data_print(const str_hmap_list& log, const str_hmap_list& aux, ostream & out = cerr);
/* print only log data */
int data_print(const str_hmap_list& log, ostream & out = cerr);

#endif // _PREPROCESS_H
