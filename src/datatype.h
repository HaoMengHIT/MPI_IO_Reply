#ifndef DATATYPE_H
#define DATATYPE_H

#include <tr1/unordered_map>
#include <map>
#include <list>
#include <iostream>
#include <vector>

using namespace std;

//typedef tr1::unordered_map<string, string> str_hmap; // K
typedef map<string, string> str_hmap;	// K

//TODO: ATTENTION!!!   str_hmap_list is actually a vector! That's to support the [] operation. Should change the name later
typedef vector<str_hmap> str_hmap_list; // T

int data_print(str_hmap_list *all_data, ostream & out = cout);

// NOTE: You should use const reference as argument, so that it can accept rvalue as parameter.
ostream& operator<<(ostream& os, const str_hmap& sh);

// I think it's far more convenient to deal with it in the preprocess section
//bool operator==(const str_hmap &a, const str_hmap &b);

#endif // DATATYPE_H
