#ifndef DATATYPE_H
#define DATATYPE_H

#include <map>
#include <list>
#include <iostream>
#include <vector>

typedef std::map<std::string, std::string> str_hmap;	// K Type

//TODO: ATTENTION!!!   str_hmap_list is actually a vector! That's to support the [] operation. Should change the name later
typedef std::vector<str_hmap> str_hmap_list; // T Type

// print str_hmap_list data to out
int data_print(str_hmap_list *all_data, std::ostream & out = std::cout);

// NOTE: You should use const reference as argument, so that it can accept rvalue as parameter.
std::ostream& operator<<(std::ostream& os, const str_hmap& sh);

#endif // DATATYPE_H
