#ifndef DATATYPE_H
#define DATATYPE_H

#include <tr1/unordered_map>
#include <map>
#include <list>

using namespace std;

//typedef tr1::unordered_map<string, string> str_hmap; // K
typedef map<string, string> str_hmap;	// K
typedef list<str_hmap> str_hmap_list; // T

#endif // DATATYPE_H
