#ifdef __cplusplus
#include <iostream>
#include <string>
#include <map>

using namespace std;

class imcpp{
private:
	map<string, string> immap;
	int imint;
	string imstr;
	double imdouble;

public:
	imcpp(int i, string str, double d)
		: imint(i), imstr(str), imdouble(d)
	{
		immap.insert(pair<string, string>(string(to_string(imint)), imstr));
		immap.insert(pair<string, string>(imstr, string(to_string(imdouble))));
	}

	void print(void);
};

extern "C"{
#endif // __cplusplus 

typedef struct imcpp imcpp;
void create_imcpp (imcpp **i);
void c_print(imcpp* i);

#ifdef __cplusplus
}
#endif // __cplusplus
