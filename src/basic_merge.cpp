#include "ts.h"
#include <time.h>
#include "datatype.h"
#include "preprocess.h"
#include <iostream>
#include <fstream>

using namespace std;


// preprocess time
struct timespec pp_time;
// merge time
struct timespec merge_time;

int find_match(str_hmap_list& base, string prefix, int procs, int rank)
{
	struct timespec pp_time2_b = recorder_wtime();
	Preprocess<str_hmap_list, str_hmap> pp_right(prefix + string("log.") + to_string(rank), procs, rank, true);
	pp_right.run();
	str_hmap_list& right = pp_right.get_data();
	struct timespec pp_time2_e = recorder_wtime();
	pp_time += (pp_time2_e - pp_time2_b);

	struct timespec m_time_b = recorder_wtime();
	ofstream fout("basic_merged");
	
	int common = 0;
	int m = 260;	// lookahead length 
	int li, ri;
	for (int pl=0, pr=0; pl<base.size() && pr<right.size(); pl++, pr++) {
		//cout << pl  << " " << pr << endl;
		if (base[pl] == right[pr]) {
			common++;
			continue;
		}
		if (common > 0)
			fout << "keep " << common << endl;
		common = 0;
		
		int pls;
		for (pls = pl; pls<pl+m && pls<base.size(); pls++) {
			if (base[pls] == right[pr])
				break;
		}
		li = pls - pl;
		int prs;
		for (prs = pr; prs<pr+m && prs<base.size(); prs++) {
			if (base[pl] == right[prs])
				break;
		}
		ri = prs - pr;
		if (li == m && ri == m) {
			fout << "insert " << right[pr] << endl;
		}
		else if (li < ri) {
			fout << "delete " << pls-pl << endl;
			pl = pls - 1;
			pr--;
		}
		else {
			for (int i=pr; i<prs && i<right.size(); i++) {
				fout << "insert " << right[i] << endl;
			}
			pr = prs - 1;
			pl--;
		}

		li = ri = 0;

	}
	if (common > 0)
		fout << "keep " << common << endl;
	struct timespec m_time_e = recorder_wtime();
	merge_time += m_time_e - m_time_b;

	return 0;
}

int main()
{
	string prefix("../trace_log_data/wzzhang_IOR_042805/");

	struct timespec begin_all = recorder_wtime();

	struct timespec pp_time1_b = recorder_wtime();
	Preprocess<str_hmap_list, str_hmap> pp_base(prefix + "log.0", 256, 0, true);
	pp_base.run();
	str_hmap_list& base = pp_base.get_data();
	struct timespec pp_time1_e = recorder_wtime();
	pp_time += (pp_time1_e - pp_time1_b);

	ofstream fout("IORsample");
	//pp_base.data_print(fout);

	cout << "_____________________________________________________" << endl;

//	st.print_tree();

	for (int i=1; i<256; i++)
		find_match(base, prefix, 256, i);
	
	struct timespec end_all = recorder_wtime();
	struct timespec cost = end_all - begin_all;

	printf("Basic merge cost %ld.%09ld second\n", cost.tv_sec, cost.tv_nsec);
	printf("Preprocess logs cost %ld.%09ld second\n", pp_time.tv_sec, pp_time.tv_nsec);
	printf("Merge cost %ld.%09ld second\n", merge_time.tv_sec, merge_time.tv_nsec);


	return 0;
}
