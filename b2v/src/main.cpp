/*
 * Google Summer Of Code
 * Project: Real-time vectorization of brain-atlases
 * Mentoring Organization: International Neuroinformatics Coordinating Facility
 * Mentors: Rembrandt Bakker, Piotr Majka
 * Student: Dhruv Kohli
 * blog: https://rtvba.weebly.com
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include "bitmap.h"
#include "util.h"

int main(int argc, char **argv)
{
	std::string filename = "";
	std::string outFileName = "";
	double toleranceCurve = 0;
	double toleranceLine = 0;
	std::string color;
	pixel bgColor;
	bool bgColorProvided = false;

	//parsing arguments
	int i = argc;
	while(i > 1)
	{
		std::string temp(argv[i - 1]);
		if(temp == "-i")
		{
			if(argc != i)
				filename = argv[i];
			else
			{
				std::cout << "invalid input file" << std::endl;
				exit(1);
			}
		}
		else if(temp == "-o")
		{
			if(argc != i)
				outFileName = argv[i];
			else
			{
				std::cout << "invalid input file" << std::endl;
				exit(1);
			}
		}
		else if(temp == "-t")
		{
			if(argc != i)
				toleranceCurve = atoi(argv[i]);
			else
			{
				std::cout << "invalid tolerance for Curve" << std::endl;
				exit(1);
			}
		}
		else if(temp == "-s")
		{
			if(argc != i)
				toleranceLine = atoi(argv[i]);
			else
			{
				std::cout << "invalid tolerance for Line" << std::endl;
				exit(1);
			}
		}
		else if(temp == "-c")
		{
			if(argc != i)
			{
				color = argv[i];
				if(color.substr(0, 1) == "#")
				{
					bgColor = hexToRGB(color);
				}
				else if(color.substr(0, 3) == "rgb")
				{
					bgColor = parseRGB(color);
				}
				else
				{
					std::cout << "invalid background color: Either #RRGGBB or rgb(R,G,B) must be provided" << std::endl;
					exit(1);
				}
				bgColorProvided = true;
			}
			else
			{
				std::cout << "invalid background color: Either #RRGGBB or rgb(R,G,B) must be provided" << std::endl;
				exit(1);
			}
		}
		else if(temp == "-h")
		{
			std::cout << "b2v: Transforms bitmaps to vector graphics\n\nusage: " << ROOT_DIR << "/bin/b2v [-h] -i BITMAP_SRC [-o SVG_DEST] [-t FIT_TOLERANCE_CURVE] [-s FIT_TOLERANCE_LINE] [-c \"#RRGGBB\"] [-c \"rgb(R,G,B)\"]\n\ndescription\n\narguments: \n-h, --help \n\t\t show this help message and exit\n-i BITMAP_SRC \n\t\t Path to input PNG Bitmap image\n-o SVG_DEST \n\t\t Destination of output SVG with desired name of file(default=ouput.svg)\n-t FIT_TOLERANCE_CURVE \n\t\t Fitting tolerance for curve(default=0)\n-s FIT_TOLERANCE_LINE \n\t\t Fitting tolerance for line(default=0)\n-c \"#RRGGBB\" OR \"rgb(R,G,B)\" \n\t\t Background Color rgb values in base 16 or 10(default=based on median of four corner points of input image)" << std::endl;
			exit(1);
		}
		i--;
	}

	if(toleranceCurve < 0)
	{
		std::cout << "invalid tolerance for curve: must be greater than zero" << std::endl;
		exit(1);
	}

	if(toleranceLine < 0)
	{
		std::cout << "invalid tolerance for line: must be greater than zero" << std::endl;
		exit(1);
	}

	if(outFileName == "")
	{
		outFileName = "ouput.svg";
	}

	if(filename == "")
	{
		std::cout << "input file not provided: use -i filename.png" << std::endl;
		exit(1);
	}

	Bitmap *inputBitmap = new Bitmap(filename, bgColor, bgColorProvided);
	inputBitmap->processImage(toleranceCurve, toleranceLine);
	inputBitmap->writeOuputSVG(outFileName);
	
	return 0;
}

