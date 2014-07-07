#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include "preprocess.h"
#include "suffix_tree_edit.h"
#include "ts.h"

using namespace std;

struct timespec preprocess_time;
struct timespec compress_time;
struct timespec all_time;

int main(int argc, char* argv[])
{
	if (argc != 3) {
		cout << "Usage: ./compress LOGNAME OUTPUTFILE" << endl;
		return 1;
	}
	struct timespec all_b = recorder_wtime();

	string filename(argv[1]);
	ofstream fout(argv[2]);

	struct timespec b = recorder_wtime();
	Preprocess<str_hmap_list, str_hmap> pp(filename, 4, 0, true);
	pp.run();
	struct timespec e = recorder_wtime();
	preprocess_time += e - b;

	str_hmap_list& l = pp.get_data();
	str_hmap_list& tm = pp.get_auxiliary();
//	pp.data_print(cout);

	b = recorder_wtime();
	SuffixTree st(l, tm);
	st.construct();
//	st.print_tree();

	st.dfs();
//	st.dfs_num_print();

	st.bfs_find_repetition();
	st.output_compressed_log(fout);
	e = recorder_wtime();
	struct timespec all_e = recorder_wtime();

	compress_time += e - b;
	all_time += all_e - all_b;

	printf("Preprocess time = %ld.%.9ld\nCompress time = %ld.%.9ld\nAll time = %ld.%.9ld\n", preprocess_time, compress_time, all_time.tv_sec, all_time.tv_nsec);

	return 0;
}
