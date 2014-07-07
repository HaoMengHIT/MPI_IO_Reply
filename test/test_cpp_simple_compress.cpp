#include "simple_compress.h"
#include <fstream>

using namespace std;

int main()
{
	SimpleCompress sc("sample_log");

	fstream fin("sample_input");
	string readLine;
	int lineno = 1;
	while (getline(fin, readLine)) {
		cout << "Line: " << lineno << endl;
		sc.read_func(readLine);
		sc.write_or_compress();
		lineno++;
	}

	return 0;
}
