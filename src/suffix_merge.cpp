#include "ts.h"
#include <time.h>
#include "suffix_tree_edit.h"
#include "datatype.h"
#include "preprocess.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;


// preprocess time
struct timespec pp_time;
// build suffix time
struct timespec build_tree_time;
// merge time
struct timespec merge_time;

int find_match(SuffixTree& base, string prefix, int procs, int rank)
{
	struct timespec pp_time2_b = recorder_wtime();
	Preprocess<str_hmap_list, str_hmap> pp_right(prefix + string("log.") + to_string(rank), procs, rank, true);
	pp_right.run();
	str_hmap_list& right = pp_right.get_data();
	struct timespec pp_time2_e = recorder_wtime();
	pp_time += (pp_time2_e - pp_time2_b);

	struct timespec m_time_b = recorder_wtime();
	ofstream fout("st/suffix_merged", ios::app);
	fout << rank << "-info--------" << endl;
	
	int begin = 0, end = 0, limit = right.size();
	int pos;
	int sub_begin;

	int length, final_pos, begin_pos;

	typename str_hmap_list::iterator r_iter = right.begin();
	++r_iter;	// skip the empty element
	
	while (r_iter != right.end()) {
		typename str_hmap_list::iterator pre_iter = r_iter;
		r_iter = base.inc_search(r_iter, &final_pos);

		if (r_iter == pre_iter) {
			fout << "insert " << *r_iter << endl;
			++r_iter;
		}
		else {
			length = r_iter - pre_iter;
			begin_pos = final_pos - length + 1;
			fout << "keep " << begin_pos << " " << final_pos << endl;
		}
	}
	fout.close();
	struct timespec m_time_e = recorder_wtime();
	merge_time += (m_time_e - m_time_b);

	return 0;
}

int main(int argc, char* argv[])
{
	if (argc != 3) {
		cout << "Usage: ./trace_tree NUM_OF_LOGS DIR_OF_LOGS" << endl;
		return 1;
	}

	cout << "the merged log will be located in st/suffix_merged" << endl;
	string prefix(argv[2]);
	int log_num = stoi(argv[1]);

	const char *dir = "st";
	mkdir(dir, 0777);

	struct timespec begin_all = recorder_wtime();

	struct timespec pp_time1_b = recorder_wtime();
	Preprocess<str_hmap_list, str_hmap> pp_base(prefix + "log.0", log_num, 0, true);
	pp_base.run();
	str_hmap_list& base = pp_base.get_data();
	str_hmap_list& aux = pp_base.get_auxiliary();
	struct timespec pp_time1_e = recorder_wtime();
	pp_time += (pp_time1_e - pp_time1_b);

	ofstream fout("st/suffix_merged");
	pp_base.data_print(fout);

	struct timespec b_time_b = recorder_wtime();
	cout << "Begining" << endl;
	SuffixTree st(base,aux);

	cout << "Constructing Suffix tree" << endl;
	st.construct();
	struct timespec b_time_e = recorder_wtime();
	build_tree_time += b_time_e - b_time_b;
	
	cout << "_____________________________________________________" << endl;

//	st.print_tree();

	for (int i=1; i<log_num; i++)
		find_match(st, prefix, log_num, i);
	
	struct timespec end_all = recorder_wtime();
	struct timespec cost = end_all - begin_all;

	printf("Suffix merge cost %ld.%09ld second\n", cost.tv_sec, cost.tv_nsec);
	printf("Preprocess logs cost %ld.%09ld second\n", pp_time.tv_sec, pp_time.tv_nsec);
	printf("Build suffix tree cost %ld.%09ld second\n", build_tree_time.tv_sec, build_tree_time.tv_nsec);
	printf("Merge cost %ld.%09ld second\n", merge_time.tv_sec, merge_time.tv_nsec);


	return 0;
}
