#ifndef SIMPLE_COMPRESS
#define SIMPLE_COMPRESS

#ifdef __cplusplus
#include <iostream>
#include <fstream>
#include <string>
#include "datatype.h"

typedef str_hmap K;

class SimpleCompress {
private:
	//TODO: we can use pointer of K to switch between them, 
	// thus avoiding all copy from cur to last. 
	// This should improve effiency to some extent.
	K last_func;
	K cur_func;
	ofstream os;

	int consecutive_op(K &);
	int write_log(K &);

public:
	SimpleCompress() {};
	SimpleCompress(const char *logname)
	{
		//std::cout << "Open stream " << logname << std::endl;
		os.open(logname);
		os << "Start tracing..." << std::endl;
	}
	// read data into cur_func
	int read_func(string line);

	// compare last_func and cur_func, change repetition times if they match
	// , else write last_func into log, then set last_func as cur_func
	int write_or_compress(void);

	// write last_func to log
	~SimpleCompress();
};

extern "C"{
#endif // __cplusplus
// here it goes C-style funtions
typedef struct SimpleCompress SimpleCompress;

void create_sc_by_filename(SimpleCompress **sc, char *filename);
void delete_sc(SimpleCompress *sc);
int read_func(SimpleCompress *sc, char *line);
int write_or_compress(SimpleCompress *sc);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SIMPLE_COMPRESS
