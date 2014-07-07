#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include "datatype.h"
#include "preprocess.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <sys/stat.h>


using namespace std;


int shrink = 20;

void output_func(string func, map<string, vector<string> >& func_info, str_hmap_list& l, int i, ostream& fout, int procs, int rank);

int construct_trace(string infile_prefix, string outfile_prefix, str_hmap_list& la, str_hmap_list& a_aux, int i)
{
	cerr << la.size() << "-----" << a_aux.size() << endl;
	if (a_aux.size() == la.size()+1)
		a_aux.erase(a_aux.begin());

	string c_filename = outfile_prefix + to_string(i);
	cerr << "output to " << c_filename << endl;
	ofstream fout(c_filename.c_str());

	string m_filename = infile_prefix + to_string(i);
	ifstream fin(m_filename);

	int pos = 1;
	int copy;
	string line;
	while (getline(fin, line)) {
		vector<string> substrs;
		// Hell, boost is soooo convenient! Timesaver!
		boost::split(substrs, line, boost::is_any_of(" \t"));
		
		switch(substrs[0][0])
		{
		case 'k':
			cerr << "k -- " << line << endl;
			copy = stoi(substrs[1]);

			for (int i=0; i<copy; i++) {
				fout <<	la[pos] << a_aux[pos] << endl;
				cerr << pos << "-" << la[pos];
				pos++;
			}
			break;
		case 'd':
			cerr << "d -- " << line << endl;
			pos += stoi(substrs[1]);
			break;
		case 'i':
			cerr << "i -- " << line << endl;
			//boost::trim_left(line, boost::is_any_of("insert "))
			line = line.substr(7);
			fout << line << endl;
			break;
		default:
			cerr << "can't recognize this!" << endl;
		}
	}
	fin.close();
	fout.close();
	
	return 0;
}

int gen_code_head(ostream& fout)
{
	fout << "#define _GNU_SOURCE // for clock_gettime, must be placed at the head of file!" << endl;
	fout << "#include <stdio.h>" << endl;
	fout << "#include <time.h>" << endl;
	fout << "#include <sys/time.h>" << endl;
	fout << "#include <mpi.h>" << endl << endl;
	fout << "#define AS 300000" << endl;
	fout << "#define BUFS 1024*1024*100" << endl << "#define HANDLES 100000" << endl << endl;
	fout << "\tstruct timespec begin, end, dur;" << endl;
	fout << "\tMPI_Datatype datatype_r[HANDLES], array_of_types_arraystore_r[HANDLES];" << endl;
	fout << "\tMPI_File fh_r[HANDLES];" << endl;
	fout << "\tMPI_Request request_r[HANDLES], array_of_requests_arraystore_r[HANDLES];" << endl;
	fout << "\tMPI_Status status_r, array_of_statuses_r[HANDLES];" << endl;

	fout << "\tint array_of_gsizes_arraystore_r[AS], array_of_distribs_arraystore_r[AS], array_of_dargs_arraystore_r[AS], array_of_psizes_arraystore_r[AS], array_of_sizes_arraystore_r[AS], array_of_subsizes_arraystore_r[AS], array_of_starts_arraystore_r[AS], array_of_blocklengths_arraystore_r[AS];" << endl;
	fout << "\tMPI_Aint array_of_displacements_arraystore_r[AS];" << endl;
	fout << "\tMPI_Offset f_offset_r, offset_dis_r;" << endl;
	fout << "\tint f_count_r, procs_r, myrank_r;" << endl;
	fout << "\tint size_r, rank_r;" << endl;
	fout << "\tchar Isendbuf_r[200][1024*1024], Irecvbuf_r[200][1024*1024];" << endl;
	fout << "\tchar sendbuf_r[BUFS], recvbuf_r[BUFS], buf_r[BUFS], buffer_r[BUFS];" << endl << endl;

	fout << "int main(int argc,char *argv[])" << endl;
	fout << "{" << endl;
	fout << "\tlong int i, j, k, r, in_loop;" << endl;
	fout << "\tMPI_Init(&argc,&argv);" << endl;
	fout << "\tclock_gettime(CLOCK_MONOTONIC, &begin);" << endl;
	fout << "\tMPI_Comm_size(MPI_COMM_WORLD,&procs_r);" << endl;
	fout << "\tMPI_Comm_rank(MPI_COMM_WORLD,&myrank_r);" << endl << endl;
	fout << "\tprintf(\"Process %d running...\\n\", myrank_r);" << endl << endl;

	return 0;
}

int gen_code_tail(ostream& fout)
{

	fout << "\tprintf(\"Process %d finished\\n\", myrank_r);" << endl << endl;
	fout << "\tclock_gettime(CLOCK_MONOTONIC, &end);\n" << endl;
	fout << "\tdur.tv_sec = end.tv_sec - begin.tv_sec;\n" << endl;
	fout << "\tdur.tv_nsec = end.tv_nsec - begin.tv_nsec + dur.tv_sec * 1000000000;\n";
	fout << "\tprintf(\"Process %d cost %ld.%09ld time\\n\", myrank_r, dur.tv_sec, dur.tv_nsec);\n" << endl << endl;
	fout << "\tMPI_Finalize();" << endl;
	fout << "\treturn 0;" << endl;
	fout << "}" << endl;
}

long get_compute_time(string tm, double ratio)
{
	vector<string> substrs;
	boost::split(substrs, tm, boost::is_any_of("."));

	if (substrs.size() != 2) {
		cerr << "time info error!" << endl;
		cerr << "tm1:" << tm;
		cerr << endl;
		return 1;
	}
	long sec = stol(substrs[0]);
	long nsec = stol(substrs[1]);

	nsec += sec * 1000000000;

	nsec = ratio * (double)nsec;

	return nsec;
}

int gen_code_body(ostream& fout, string file_prefix, map<string, vector<string>>& func_info, int procs, int rank, double ratio)
{	
	string filename = file_prefix + to_string(rank);
	cerr << "gen - " << filename << endl;

	ifstream fin(filename.c_str());
	Preprocess<str_hmap_list, str_hmap> pp(filename, procs, rank);
	pp.run();
	str_hmap_list &l = pp.get_data();
    	str_hmap_list &aux = pp.get_auxiliary();

	fout << "\tif (myrank_r == " << rank << ") {" << endl;

	bool outmost = true;
	int out_label = -1;
	vector<int> loop_end(l.size(), 0);
	vector<int> repetitions, actual_len;


	for (unsigned int i=0; i<l.size(); i++) {
		if (l[i].find("func") == l[i].end() || l[i]["func"] == "")
			continue;
		
		repetitions.clear();
		actual_len.clear();

		if (aux[i].find("tm1") == aux[i].end())
			continue;
		string compute_time = aux[i]["tm1"];
		long nsec = get_compute_time(compute_time, ratio);

		// long int i
		fout << "\t\tfor(i=0; i<" << nsec << "; i++); /* Simulate compute time */" << endl << endl;

		typename str_hmap_list::iterator iter;
		if (l[i].find("repetition") != l[i].end()) {
			vector<string> substrs;
			boost::split(substrs, l[i]["repetition"], boost::is_any_of("-"));
			
			vector<string> in_loop_time;
			boost::split(in_loop_time, l[i]["in_loop_time"], boost::is_any_of("-"));

			int rep_all = 0;
			rep_all = 1;
			for (int k=0; k<substrs.size(); k++) {
				if (substrs[k] != "") {
					int rep_time = stoi(substrs[k]);
					if (outmost && (k == 0))
						rep_time /= shrink;
					if (rep_time == 0)
						rep_time = 1;
					rep_all *= rep_time;
					fout << "\t\tfor(int j=0; j<" << rep_time << "; j++) {" << endl;
					long nsec = get_compute_time(in_loop_time[k], ratio);
					fout << "\t\tif (j>0)" << endl;
					fout << "\t\t\tfor(int k=0; k<" << nsec << "; k++); /* Simulate compute time in loop */" << endl << endl;
					if (k == 0) {
						//fout << "\t\t\tfor(int k=0; k<" << nsec/rep_all << "; k++); /* Simulate compute time in loop */" << endl << endl;
					}
				}
			}
			substrs.clear();

			boost::split(substrs, l[i]["actual_len"], boost::is_any_of("-"));
			cerr << substrs.size() << endl;
			for (int k=0; k<substrs.size(); k++) {
				if (substrs[k] != "")
					loop_end[i+stoi(substrs[k])-1]++;
				if (k == 0 && outmost == true)
					out_label = i+stoi(substrs[k])-1;
				// check loop_end after the function outputed
			}
			outmost = false;
		}

		if (l[i].find("MARK") != l[i].end()) {
			fout << "offset_dis_r = " << l[i]["offset"] << " - f_offset_r;" << endl;
			fout << "f_offset_r = " << l[i]["offset"] << ";" << endl;
		}
		else if (l[i].find("offset") != l[i].end())
			fout << "f_offset_r += offset_dis_r;" << endl;

		string func = l[i]["func"];
		
		output_func(func, func_info, l, i, fout, procs, rank);

		while (loop_end[i]--)
			fout << "\t\t}" << endl;
		if (i == out_label)
			outmost = true;
	}
	
	fout << "\t}\n" << endl;	// for if rank == xx
	return 0;
}

void output_func(string func, map<string, vector<string> >& func_info, str_hmap_list& l, int i, ostream& fout, int procs, int rank) 
{
		if (func == "")
			return;
		if (boost::starts_with(func, "MPI_Comm"))
			return;	//TODO
		fout << "\t\t" << func << '(';

	//	copy(func_info[func].begin()+1, func_info[func].end(), ostream_iterator<string>(fout, ", " ));
	//	fout << *(func_info[func].end()-1) << ");" << endl;
		typename vector<string>::iterator v_it;
		// TODO: figure out why should I minus 1 here 
		for (v_it = func_info[func].begin()+1; v_it<func_info[func].end()-1; v_it++) {
			string parameter = *v_it;	// para
			string argument = l[i][*v_it];	// value of para

			if (boost::starts_with(parameter, "array") && parameter != "array_of_statuses") {
				vector<string> substrs;
				boost::split(substrs, argument, boost::is_any_of("-"));

				int size = stoi(substrs[0]);
				stringstream ss;
				ss << "(";
				for (int i=0; i<size; i++) {
					ss << parameter << "_arraystore_r[" << i <<"] = ";
					if (parameter == "array_of_types") 
						ss << "datatype_r[" << substrs[i+1] << "],";
					else if (parameter == "array_of_requests")
						ss << "request_r[" << substrs[i+1] << "], ";
					else
						ss << substrs[i+1] << ",";
				}
				//if (parameter == "array_of_requests")
				//	ss << "&" <<  parameter  << "_arraystore[0])";
				//else
				ss << parameter << "_arraystore_r)";
				argument = ss.str();
			}
			if (parameter == "array_of_statuses")
				argument += "_r";
			if (parameter.find("buf") != string::npos) {
				if (func == "MPI_Isend")
					argument = "Isendbuf_r[" + l[i]["request"] + "]";
				else if (func == "MPI_Irecv")
					argument = "Irecvbuf_r[" + l[i]["request"] + "]";
				else 
					argument += "_r";
			}
		
			if (parameter == "comm" && boost::starts_with(argument, "MPI") == false)
				argument = "MPI_COMM_WORLD";
			//if (parameter == "comm" && boost::starts_with(argument, "MPI") == false)
			//	argument = "comm[" + argument+ "]";
			//else if (parameter == "newcomm" && boost::starts_with(argument, "MPI") == false)
			//	argument = "&newcomm[" + argument + "]";
			else if (parameter == "op")
				argument = "MPI_MAX";
			else if (parameter == "fh") {
				argument = "fh_r" + string("[") + argument + "]";
				if (func == "MPI_File_open" || func == "MPI_File_close")
					argument = "&(" + argument + ")";
			}
			else if (parameter == "request")
				argument = "&request_r" + string("[") + argument + "]";
			else if ((parameter == "datatype" || parameter == "etype" || parameter == "filetype" || parameter == "oldtype" || parameter == "newtype")) {
				if (boost::starts_with(argument, "MPI") == 0) {
					argument = "datatype_r[" + string(argument) + "]";	// TODO
					if (parameter == "newtype" || func == "MPI_Type_commit")
						argument = "&" + argument;
				}
				else if(argument == "MPI_UNSIGNED_INT")
					argument = "MPI_INT";
			}


			else  if (parameter == "offset") {
				if (l[i].find("MARK") == l[i].end()) 
					argument = "f_offset_r";
			}
			else if (parameter == "tag" || parameter == "sendtag" || parameter == "recvtag" )
				argument = "1";
			else if (parameter == "size" || parameter == "rank") {
				if (func == "MPI_Comm_rank" || func == "MPI_Comm_size")
					argument = "&" + argument + "_r";
			}
			else if (parameter == "status")
				argument = "&" + argument + "_r";
			else if (parameter == "filename" || parameter == "datarep")
				argument = "\"" + argument + "\"";
			else if (parameter == "info")
				argument = "MPI_INFO_NULL";
			else if (parameter == "dest" || parameter == "source") {
				int value = stoi(argument);

				//if (rank % 2 == 1)	// avoid deadlock
				//	value = procs - value;

				//value += rank;
				if (value >= procs)
					value -= procs;


				argument = to_string(value);
			}

			if (argument.size() == 0) {
				cerr << "Para " << parameter << " have no value!" << endl;
			}
			fout << argument;

			if (v_it != func_info[func].end()-2)
				fout << ", ";
		}
		fout << ");" << endl << endl;
}
		

int get_func_info(const char* filename, map<string, vector<string>>& func_info)
{
	ifstream fin(filename);

	if (!fin) {
		cerr << "Can't open function info file!" << endl;
		return 1;
	}

	string line;
	while (getline(fin, line)) {
		vector<string> substrs;
		boost::split(substrs, line, boost::is_any_of(" \t\n"));

		func_info.insert(make_pair(substrs[0], substrs));
	}

	return 0;
}

double get_ratio(string ratiofile)
{
	ifstream fin(ratiofile.c_str());

	string s;
	getline(fin, s);

	vector<string> substrs;
	boost::split(substrs, s, boost::is_any_of("="));
	double ratio_inverse = stod(substrs[1]);

	ofstream fout("replay_ratio");
	fout << ratio_inverse << endl;

	return 1/ratio_inverse;
}

int main(int argc, char* argv[])
{
	if (argc != 7) {
		cerr << "Usage: ./replay <num> <scale> <single aux file> <func_info> <merged_log_dir> <construct_log_dir> " << endl;
		return 1;
	}
   int logs = stoi(argv[1]);
	shrink = stoi(argv[2]);
   const char* func_info_str = argv[4];
   if(access(func_info_str, F_OK)){
      perror("Couldn't open func_info file:");
      return -1;
   }

   string rdir_str(argv[5]);
   string wdir_str(argv[6]);
   if(rdir_str.back() !='/') rdir_str.push_back('/');
   if(wdir_str.back() !='/') wdir_str.push_back('/');

   struct stat wdir_st;
   if(stat(wdir_str.c_str(), &wdir_st)){
      mkdir(wdir_str.c_str(), 0755);
      stat(wdir_str.c_str(), &wdir_st);
   }
   if(! (wdir_st.st_mode & S_IFDIR) ){
      perror("output target is not a dir:");
      return errno;
   }


	Preprocess<str_hmap_list, str_hmap> ppa(rdir_str + "merged_lcs.0", logs, 0);
	ppa.run();	
    //ppa.data_print();
   str_hmap_list& la = ppa.get_data();
	str_hmap_list& a_aux = ppa.get_auxiliary();

	// func name and parameters
	map<string, vector<string>> func_info;
	if (get_func_info(func_info_str, func_info)) {
		cerr << "Func info missing?" << endl;
		return 1;
	}

	for (typename map<string, vector<string>>::iterator it=func_info.begin(); it!=func_info.end(); ++it) {
		cerr << it->first << endl;
		copy(it->second.begin(), it->second.end(), ostream_iterator<string>(cerr, "---"));
		cerr << endl;
	}

	cerr << "func_info?" << endl;

   ostream& code_file = std::cout;
	string infile_prefix = rdir_str + "merged_lcs.";
	string outfile_prefix = wdir_str = "log.";

	double ratio = get_ratio(argv[3]);
	gen_code_head(code_file);
	// infile!
	gen_code_body(code_file, infile_prefix, func_info, logs, 0, ratio);

	for (int i=1; i<logs; i++) {
		construct_trace(infile_prefix, outfile_prefix, la, a_aux, i);
		gen_code_body(code_file, outfile_prefix, func_info, logs, i, ratio);
	}
	gen_code_tail(code_file);
	
	return 0;
}
