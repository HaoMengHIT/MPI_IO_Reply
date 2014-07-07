#include "datatype.h"

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

/*
inline bool noncompare(const string& para)
{
	if (para == "info" || para == "status" || para == "request" || para == "tm1" || para == "tm2" || para == "fh") {
		cout << "Bypassed para " << para << endl;
		return true;
	}

	return false;
}

bool operator==(const str_hmap &a, const str_hmap &b)
{
	cout << "~~~~~~~~~~~~~~~~~" << endl;
	// This function is comparing map instead of hash_map as the name implies...
	typename str_hmap::const_iterator iter_a, iter_b;

	if (a.size() != b.size()) {
		cout << "Different size" << endl;
		return false;
	}

	for (iter_a=a.begin(), iter_b=b.begin(); iter_a!=a.end() || iter_b!=b.end(); ++iter_a, ++iter_b) {
		if (iter_a->first != iter_b->first)	// parameter differs
		{
			cout << "left is " << iter_a->first << " but right is " << iter_b->first << endl;
			return false;
		}

		if (noncompare(iter_a->first))
			continue;

		if (iter_a->second != iter_b->second) {
			cout << iter_a->first << " not identical" << endl;
			return false;
		}
	}
	cout << "Matched!" << endl;
	cout << endl << endl;

	return true;
}
*/
