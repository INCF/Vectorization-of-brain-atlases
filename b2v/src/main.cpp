#include <iostream>
#include <string>
#include <cstdlib>
#include "bitmap.h"

int main(int argc, char **argv)
{
	std::string filename = "";
	std::string outFileName = "";

	switch(argc)
	{
		case 2:
			filename.assign(argv[1]);
			outFileName = "output.svg";
			break;
		case 3:
			filename.assign(argv[1]);
			outFileName.assign(argv[2]);
			break;
		default:
			std::cout << "Usage: " << ROOT_DIR << "/bin/b2v path_to_filename.png [path_to_outputfileName.svg]" << std::endl;
			exit(1);
			break;
	}

	Bitmap *inputBitmap = new Bitmap(filename);
	inputBitmap->processImage();
	inputBitmap->writeOuputSVG(outFileName);
	
	return 0;
}

