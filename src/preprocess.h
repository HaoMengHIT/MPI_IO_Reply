#ifndef _PREPROCESS_H_
#define _PREPROCESS_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <tr1/unordered_map>
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
//template <typename T, typename K>
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
	int reorder(string func, string key, int temp_value, int pos, int* result);
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

	// return 0 if repetition found, 
	// -1 if function name unmatch,
	// 1 if offset unmatch,
	// 2 if other paras unmatch
//	int consecutive_op(K & cur_func);	No longer needed

	// compress the obvious consecutive op on large file
//	int push_with_simple_compress(K & cur_func);	No longer needed

public:
	Preprocess(string filename_para, int nprocs_para, int rank_para, bool first_para = false);
//	~Preprocess();

	// return 0 if success
	void changefile(string file_name_para);
	int run();
	// TODO: why the adding of const will cause error?
	int data_print(ostream & out = cerr);
	int data_print_pure(ostream & out = cerr);
	T & get_data(void);
	T & get_auxiliary(void) { return auxiliary_data; }
};

// Important! The implementation and definition of TEMPLATE CLASS MUST put in the same FILE!!!
#endif // _PREPROCESS_H
