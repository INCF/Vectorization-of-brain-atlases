#include <iostream>
#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <sstream>  

using namespace std;

int main()
{
	int i = 0;
	string pre = "/home/dhruv/bitmap2vector/input_atlas/atlas_";
	string suff = ".png";
	string name;
	string input_name;

	for(int i = 0; i < 500; ++i)
	{
		name = "";
		input_name = "";
		stringstream ss, ss2;
		ss << pre << setfill('0') << setw(3) << i << suff;
		ss2 << "atlas_" << setfill('0') << setw(3) << i;
		ss >> name;
		cout << name << endl;
		ifstream f(name.c_str());
	    if (f.good()) 
	    {
	    	f.close();
	    	ss2 >> input_name;
	    	input_name = "./main.sh " + input_name + " 1";
	    	int j = system ("chmod +x main.sh");
	    	j = system(input_name.c_str());
	    }
	}
	return 0;
}