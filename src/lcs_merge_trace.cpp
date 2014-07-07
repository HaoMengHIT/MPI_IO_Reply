#include "ts.h"

#include <time.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <sys/stat.h>
#include "preprocess.h"


using namespace std;

// preprocess time
struct timespec pp_time;
// merge time
struct timespec merge_time;
// post process time
struct timespec post_time;

// well, I know it's ugly. Don't laugh. -_-!
int display_shortest_edit_script(ostream& fout, int k, int d, int x, vector<vector<int>*>& storage, str_hmap_list& a, str_hmap_list& b, str_hmap_list& b_aux);

// TODO: test version, find edit script of strings, final ver should read MPI traces properly
int find_shortest_edit(ostream& fout, str_hmap_list& a, str_hmap_list& b, str_hmap_list& a_aux, str_hmap_list& b_aux)
{
	const int N = a.size() - 1;
	const int M = b.size() - 1;
	const int MAX = N + M;

	// v[k] stores the furthest reaching D-path's row index(x) in diagonal k(means k=x-y)
	vector<int> v(2*MAX+1);

	// all indexes of vector should have the form of v[???+zero]
	const int zero = MAX;

	vector<vector<int>*> storage(MAX);

	v[zero+1] = 0;

	int x, y;
	for (int d=0; d<=MAX; d++) {
		for (int k=-d; k<=d; k+=2){
			if (k==-d || (v[k-1+zero] < v[k+1+zero] && k!=d))
				x = v[k+1+zero];
			else
				x = v[k-1+zero] + 1;

			y = x - k;

			while (x<N && y<M && a[x+1] == b[y+1]) {
				x++;
				y++;
			}
			v[k+zero] = x;

			//cout << "diagonal k=" << k << " with depth d=" << d << " stop at (" << x << "," << y << ")" << endl;

			if (x>=N && y>=M) {
				display_shortest_edit_script(fout, k, d, x, storage, a, b, b_aux);
				return d;
			}
		}

		// store the current vector
		storage[d] = new vector<int>(2*d+1);

		vector<int>::iterator iter_begin, iter_end;
		iter_begin = v.begin() + zero - d;
		iter_end = v.begin() + zero + d + 1;
		copy(iter_begin, iter_end, (*storage[d]).begin());

		for (int i=0; i<2*d+1; i++)
			//cout << (*storage[d])[i] << " ";
			;
	//	cout << endl; 

		//copy((*storage[d]).begin(), (*storage[d]).end(), ostream_iterator<int>(cout, " "));
		//cout << endl;
	}
	return -1;
}

int display_shortest_edit_script(ostream& fout, int k, int d, int value, vector<vector<int>*>& storage, str_hmap_list& a, str_hmap_list& b, str_hmap_list& b_aux)
{
	//cout << endl;
	//cout << "Display here." << endl;
	//cout << "k:" << k << " d:" << d << " value:" << value << endl;

	if (d == 0) {
        if (value > 0)
            fout << "keep " << value << endl;
		return 0;
    }

	int x = value;
	int y = x - k;

    // the length of snake
    int keep = 0;

	// strip the snake
	do {
		value = x;
		//cout << "k:" << k << " d:" << d << " value:" << value << endl;

		int middle = d-1;
	
		if ((*storage[d-1])[k+1+middle] == value) {
			display_shortest_edit_script(fout, k+1, d-1, value, storage, a, b, b_aux);

			//cout << "vectical edge from (" << value << "," << (value - (k+1)) << ") to " << "(" << value << "," << value-k << ")" << endl; 
            // insert unique element in string B
            fout << "insert " << b[value-k] << b_aux[value-k] << endl;

            // output snake
            if (keep > 0)
                fout << "keep " << keep << endl;

			break;
		}
		else if ((*storage[d-1])[k-1+middle] == value-1) {
			display_shortest_edit_script(fout, k-1, d-1, value-1, storage, a, b, b_aux);
			//cout << "horizontal edge from (" << value-1 << "," << value-1-(k-1) << ") to " << "(" << value << "," << value-k << ")" << endl;
			//fout << "delete " << value-1 << endl;
            fout << "delete 1" << endl;

            if (keep > 0)
                fout << "keep " << keep << endl;

            break;
		}
		else {
			//cout << "strip snake" << endl;
            		keep++;
		}
		
	} while(a[x--] == b[y--]);

	return 0;
}

// merge adjacent del
void post_process(const char* temp_filename, const char* filename)
{
    ifstream fin(temp_filename);
    ofstream fout(filename, ios::out);

    if (!fout) {
    	cout << "Error writting merged file!" << endl;
	return;
    }

    string line;
    string last = "";

    // number of delete
    int nd = 0;
    
    while (getline(fin, line)) {
        if (line.find("d") != 0) {
            if (last != "") {
                fout << last << endl;
		}

            last = line;
            continue;   // not del
        }

        if (last.find("d") != 0) {
            if (last != "") {
                fout << last << endl;
		}

            nd = 1;
            last = line;
        }
        else {
            nd++;
            last = "delete " + std::to_string(nd);
        }
        //cout << line << endl;
    }
    fout << last << endl;
    cout << "Closing file..." << endl;
    fout.close();
}

int lcs_merge_two(str_hmap_list& la, string rdir, string wdir, int procs, int i, str_hmap_list& a_aux)
{
    // Note: the first element in la and lb must be NULL(or sth like it), because the algorithm will ignore the first element
   struct timespec pp_b = recorder_wtime();
   Preprocess<str_hmap_list, str_hmap> ppb(rdir+"compressed_log."+std::to_string(i), procs, i);
   ppb.run();	
   //ppb.data_print();
   str_hmap_list& lb = ppb.get_data();
   str_hmap_list& b_aux = ppb.get_auxiliary();
   struct timespec pp_e = recorder_wtime();
   pp_time += pp_e - pp_b;

   struct timespec merge_b = recorder_wtime();
   const char* temp_filename = "/tmp/lcs_diff_output_temp";
   ofstream fout(temp_filename);
   string s_filename = wdir + "merged_lcs." + to_string(i);
   const char * filename = s_filename.c_str();
   cout << "filename --> " << filename << endl;

   int r = find_shortest_edit(fout, la, lb, a_aux, b_aux);

   cout << "The edit distance between logs is " << r << endl;
   struct timespec merge_e = recorder_wtime();
   merge_time += merge_e - merge_b;

   struct timespec post_b = recorder_wtime();
   post_process(temp_filename, filename);
   cout << "filename --> " << filename << endl;
   struct timespec post_e = recorder_wtime();
   post_time += post_e - post_b;

   return 0;
}

int main(int argc, char* argv[])
{
	if (argc != 4) {
		cout << "useage:"<< argv[0]<<" NUM INPUT_DIR OUTPUT_DIR" << endl;
		return 1;
	}

	string num(argv[1]);
   int logs = stoi(num);
   string rdir_str(argv[2]);
   string wdir_str(argv[3]);
   if(rdir_str.back()!='/') rdir_str.push_back('/');
   if(wdir_str.back()!='/') wdir_str.push_back('/');

   struct stat wdir_st;
   if(stat(argv[3], &wdir_st)){
      mkdir(argv[3], 0755);
      stat(argv[3], &wdir_st);
   }
   if(! (wdir_st.st_mode & S_IFDIR) ){
      perror("output target is not a dir:");
      return errno;
   }

	struct timespec begin = recorder_wtime();
	struct timespec pp_b = recorder_wtime();
	Preprocess<str_hmap_list, str_hmap> ppa(rdir_str+"compressed_log.0", logs, 0);
	ppa.run();	
    //ppa.data_print();
    str_hmap_list& la = ppa.get_data();
    str_hmap_list& a_aux = ppa.get_auxiliary();
    	struct timespec pp_e = recorder_wtime();
	pp_time += pp_e - pp_b;

	for (int i=1; i<logs; i++) {
		// merge log 0 and log i
		lcs_merge_two(la, rdir_str, wdir_str, logs, i, a_aux);
	}
	
	// copy log.0
	ofstream out0(wdir_str + "merged_lcs.0");
	ppa.data_print(out0);

	struct timespec end = recorder_wtime();
	struct timespec cost = end - begin;

	printf("LCS merge cost %ld.%09ld second\n", cost.tv_sec, cost.tv_nsec);
	printf("Preprocess logs cost %ld.%09ld second\n", pp_time.tv_sec, pp_time.tv_nsec);
	printf("Merge logs cost %ld.%09ld second\n", merge_time.tv_sec, merge_time.tv_nsec);
	printf("Post process cost %ld.%09ld second\n", post_time.tv_sec, post_time.tv_nsec);

	return 0;
}
