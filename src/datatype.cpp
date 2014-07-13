#include "datatype.h"
using namespace std;

int data_print(str_hmap_list *all_data, ostream & out)
{
	out << "Outputing all data..." << std::endl;
	if (all_data->empty()) {
		out << "No data so far" << std::endl;
		return 1;
	}

	str_hmap_list::iterator titor;
	for(titor=all_data->begin(); titor!=all_data->end(); ++titor) {
		str_hmap::iterator kitor;
		for (kitor=titor->begin(); kitor!=titor->end(); ++kitor){
			out << kitor->first << " = " << kitor->second << '\t';
		}
		out << std::endl << std::endl;
	}

	return 0;
}

ostream& operator<<(ostream& os, const str_hmap& sh)
{
	typename str_hmap::const_iterator iter;

	for (iter=sh.begin(); iter!=sh.end(); ++iter) {
//		if (iter->first == string("func"))
			os << iter->first << "=" << iter->second << ' ';
	}
//	os << endl;

	return os;
}

