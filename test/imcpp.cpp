#include "imcpp.h"

void imcpp::print(void)
{
	cout << "Int-" << imint << endl;
	cout << "Str-" << imstr << endl;
	cout << "Double-" << imdouble << endl;

	map<string, string>::iterator it;
	cout << "Map-" << endl;
	for (it=immap.begin(); it!=immap.end(); it++) {
		cout << it->first << ":" << it->second << endl;
	}
}

void create_imcpp(imcpp **i)
{
	(*i) = new imcpp(1, "xxx", 20);
}

void c_print(imcpp *i)
{
	i->print();
}
