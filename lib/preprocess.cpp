#include "preprocess.h"


//template <typename T, typename K>
Preprocess::Preprocess(string filename_para, int procs_para, int rank_para, bool first_para)
{
	first = first_para;
	filename = filename_para;
	rank = rank_para;
	procs = procs_para;
	state = 1;
	counter = 0;
	barrier = 0;
	argument_maps.clear();
	last_io = "INI";
	last_offset = 0;
	offset_dis = 0;
	argument_maps.insert(make_pair("fh", map<string, unsigned int>()));
	argument_maps.insert(make_pair("datatype", map<string, unsigned int>()));
	argument_maps.insert(make_pair("request", map<string, unsigned int>()));
	argument_maps.insert(make_pair("comm", map<string, unsigned int>()));
	std::cerr << "File " << filename << std::endl;
}

//template <typename T, typename K>
void Preprocess::changefile(string filename_para)
{
	filename = filename_para;
}

// TODO: should have an output parameter here
int Preprocess::run()
{
	fstream fin(filename.c_str());
	if (!fin) {
		std::cerr << "Unable to open input file:" << filename << std::endl;
		return 1;
	}
	std::string ReadLine;
	int lineno = 0;

	std::cerr << "Running" << std::endl;
	str_hmap empty;
	all_data.push_back(empty);
	auxiliary_data.push_back(empty);
	
	while (getline(fin,ReadLine))
	{
		lineno++;
		extract_data_from_single_line(ReadLine, lineno);
	}

	return 0;
}

//template<typename T, typename K>
unsigned int Preprocess::build_match(string& key, string& value, bool free)
{
	typename map<string,map<string, unsigned int> >::iterator outer_it;
	if ((outer_it = argument_maps.find(key)) == argument_maps.end())
		return 0;
	
	typename map<string, unsigned int>::iterator inner_it;
	if ((inner_it = (outer_it->second).find(value)) == (outer_it->second).end()) {
		if (free == true) {
			cerr << "Freeing unallocated handle!" << endl;
			cerr << "Current parameter: " << key << endl;
			cerr << "Current Handle: " << value << endl;
			cerr << "Handle(argument) Map:" << endl;

			for (inner_it = outer_it->second.begin(); inner_it!=outer_it->second.end(); ++inner_it)
				cerr << inner_it->first << "->" << inner_it->second << "\t";

			cerr << endl;
			exit(1);
		}
		unsigned int num = outer_it->second.size() + 1;
		outer_it->second.insert(make_pair(value, num));
	}

	unsigned int result = (outer_it->second)[value];
	if (free)
		outer_it->second.erase(inner_it);

	return result;
}

//template<typename T, typename K>
bool Preprocess::check_io(string& func_name, string& offset) 
{
	unsigned long off = stoul(offset);
	if (last_io != func_name || off == 0) {
		last_io = func_name;
		offset_dis = off - last_offset;
		last_offset = off;
		return false;
	}
	if (off - last_offset != offset_dis) {
		offset_dis = off - last_offset;
		last_offset = off;
		return false;
	}
	last_offset = off;
	return true;

}

// analyze each line, note this funtion is data-specific
//template<typename T, typename K>
int Preprocess::extract_data_from_single_line(std::string & line, int pos)
{
	std::string key, value, temp;
	str_hmap cur_func;
	str_hmap auxiliary;

	if (line == "Start tracing..." || line == "Outputing all data...")
		return 0;

	// extract function's name, parameters, etc
	for(int cur_para_begin = 0,
			cur_para_end = line.find(' ', cur_para_begin); 

		cur_para_end >= 0; 
		
		cur_para_begin = cur_para_end+1,cur_para_end = line.find(' ', cur_para_begin)){
		
		while (line[cur_para_begin] == ' ')
			cur_para_begin++;
		temp.assign(line, cur_para_begin, cur_para_end-cur_para_begin);
		size_t split_pos = temp.find('=');

		if (split_pos >= 0) {
			value.assign(temp, split_pos+1, std::string::npos);
			key.assign(temp, 0, split_pos);
		}
		else
			value.clear();

		// ignore timestamps and nullify info, status, request
		if (key == "tm1" || key == "tm2" || key == "tm1_first") {
			auxiliary.insert(pair<std::string, std::string>(key, value));
			continue;
		}
		else if (key == "info" || key == "op" || key == "status" || key == "array_of_statuses") 
			value = key;
		else if (key == "tag" || key == "sendtag" || key == "recvtag")
			value = "MPI_ANY_TAG";

		else if ((key == "array_of_types" || key == "array_of_requests") && first == true) {
			vector<string> substrs;
			boost::split(substrs, value, boost::is_any_of("-"));

			std::stringstream ss;
			string new_value;

			int size = stoi(substrs[0]);
			ss << substrs[0] << "-";
			string temp_key;
			bool free = false;

			if (key == "array_of_types")
				temp_key = "datatype";
			else {	// MPI_Waitall
				temp_key = "request";
				free = true;
			}
			string temp_value;
			for (int i=1; i<=size; i++) {
				temp_value = substrs[i];
				unsigned int v = build_match(temp_key, temp_value, free);
				ss << v << "-";
			}		
			ss >> new_value;
			value = new_value;
		}

		if (first == true && (key == "dest" || key == "source")) {
			int temp_value = std::stoi(value);
			temp_value -= rank;
			//if (temp_value < 0)
			//	temp_value += procs;
/*
			string func_name = cur_func["func"];
			if (boost::starts_with(func_name, "MPI_I")) {
				int result = temp_value;
				int ret = 0;
				if (state)
					ret = reorder(func_name, key, temp_value, pos, &result);
				if (ret)
					temp_value = result;
			}
*/
			value = std::to_string(temp_value);
		}

		// TODO: remove it later
		// func name should be already inserted to cur_func here?
		else if (first == true && (key == "comm" || key == "newcomm" || key == "fh" || key == "request" || key == "datatype" || key == "etype" || key == "filetype" || key == "newtype" || key == "oldtype") && value.find("MPI") != 0) {
			bool free = false;
			if (cur_func["func"] == "MPI_File_close" || cur_func["func"] == "MPI_Type_free" || cur_func["func"] == "MPI_Wait") 
				free = true;
			string temp_key = key;
			if (temp_key == "etype" || temp_key == "filetype" || temp_key == "oldtype" || temp_key == "newtype")
				temp_key = "datatype";
			else if (temp_key == "newcomm")
				temp_key = "comm";
			unsigned int assigned_value = build_match(temp_key, value, free);
			value = std::to_string(assigned_value);
		}
		else if (first == true && (key == "offset") && (cur_func["func"].find("read_at") != string::npos || cur_func["func"].find("write_at"))) {
			bool flag = false;
			flag = true;
			
			flag = check_io(cur_func["func"], value);
			
			//cout << last_io <<  " " << last_offset << " " <<  offset_dis << endl;
			if (flag)
				value = to_string(0);
			else
				cur_func.insert(make_pair("MARK", "startingpoint"));
		}
		cur_func.insert(pair<std::string, std::string>(key, value));
	}

	// TODO: sample
	if (auxiliary.empty())
		return 1;

	if (first == true && cur_func["func"] == "MPI_Waitall") {
		state = 0;
		counter = (procs-1) * 4 * 2;
		barrier = pos;
	}
	else if (state == 4 || state ==0) {
		counter--;
		if (counter == (procs-1) * 4 * 2 - 4 + 1)
			state = 4;
		else if (counter == 0)
			state = 1;
	}
	if (auxiliary.find("tm1") == auxiliary.end())
		return 1;
	if (cur_func.find("func") == cur_func.end())
		return 1;
	if (auxiliary["tm1"] == "")
		return 1;
	if (cur_func["func"] == "")
		return 1;

	all_data.push_back(cur_func);
	auxiliary_data.push_back(auxiliary);

	return 0;
}


int data_print(const str_hmap_list& log, const str_hmap_list& aux, ostream & out)
{
   const str_hmap_list& all_data = log;
   const str_hmap_list& auxiliary_data = aux;
   bool has_aux = log.size() == aux.size();

	out << "Outputing all data..." << std::endl;
	if (all_data.empty()) {
		out << "No data so far" << std::endl;
		return 1;
	}

	typename str_hmap_list::const_iterator titor, titor2;
	for(titor=all_data.begin(), titor2=auxiliary_data.begin(); titor!=all_data.end(); ++titor, ++titor2) {
		typename str_hmap::const_iterator kitor;
		for (kitor=titor->begin(); kitor!=titor->end(); ++kitor){
			out << kitor->first << "=" << kitor->second << ' ';
		}

      if(has_aux){
         for (kitor=titor2->begin(); kitor!=titor2->end(); ++kitor)
            out << kitor->first << "=" << kitor->second << ' ';
      }

		out << std::endl;
	}

	return 0;
}

int data_print(const str_hmap_list& log, ostream & out)
{
   str_hmap_list empty;
   return data_print(log, empty, out);
}
