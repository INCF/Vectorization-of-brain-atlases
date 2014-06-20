//main.cpp
/*
 * Google Summer Of Code
 * Project: Real-time vectorization of brain-atlases
 * Mentoring Organization: International Neuroinformatics Coordinating Facility
 * Mentors: Rembrandt Bakker, Piotr Majka
 * Student: Dhruv Kohli
 * blog: https://rtvba.weebly.com
 */

/*
 * License info
 */

#include "info.h"
#include "bitmap.h"

int main(int argc, char **argv)
{
	Info inputInfo;

	//parse arguments to main
	inputInfo.parseInputArg(argc, argv);

	//create bitmap with info provided provided by the user
	Bitmap inputBitmap(&inputInfo);

	//process and write output
	inputBitmap.processImage();
	inputBitmap.writeOuputSVG();
	
	return 0;
}