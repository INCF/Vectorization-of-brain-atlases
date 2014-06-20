/*
 * Google Summer Of Code
 * Project: Real-time vectorization of brain-atlases
 * Mentoring Organization: International Neuroinformatics Coordinating Facility
 * Mentors: Rembrandt Bakker, Piotr Majka
 * Student: Dhruv Kohli
 * blog: https://rtvba.weebly.com
 */

#include "info.h"
#include "bitmap.h"

int main(int argc, char **argv)
{
	Info inputInfo;
	inputInfo.parseInputArg(argc, argv);

	Bitmap inputBitmap(&inputInfo);
	inputBitmap.processImage();
	inputBitmap.writeOuputSVG();
	
	return 0;
}