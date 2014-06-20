#include "info.h"
#include <cstdlib>
#include "util.h"
#include <iostream>

Info::Info()
{
	//default values
	inputFileName = "";
	outFileName = "";
	toleranceCurve = 2;
	toleranceLine = 0;
	bgColor.r = bgColor.g = bgColor.b = 0;
	bgColorProvided = false;
	turdSize = 10;
	switchNoisy = false;
	medianBlurKernelSize = 1; // means 3X3 i.e (2k+1)X(2k+1)
	KMeansMaxIters = 50;
	numClusters = 8;
}	

Info::~Info()
{
	inputFileName.clear();
	outFileName.clear();
	color.clear();
}

void Info::parseInputArg(int argc, char **argv)
{
	//parsing arguments
	int i = argc;
	while(i > 1)
	{
		std::string temp(argv[i - 1]);
		if(temp == "-i" || temp == "--input-file")
		{
			if(argc != i)
				inputFileName = argv[i];
			else
			{
				std::cout << "invalid input file: use -h, --help for usage information" << std::endl;
				exit(1);
			}
		}
		else if(temp == "-o" || temp == "--output-file")
		{
			if(argc != i)
				outFileName = argv[i];
			else
			{
				std::cout << "invalid output file: use -h, --help for usage information" << std::endl;
				exit(1);
			}
		}
		else if(temp == "-t" || temp == "--curve-tolerance")
		{
			if(argc != i)
				toleranceCurve = atoi(argv[i]);
			else
			{
				std::cout << "invalid tolerance for Curve: use -h, --help for usage information" << std::endl;
				exit(1);
			}
		}
		else if(temp == "-s" || temp == "--line-tolerance")
		{
			if(argc != i)
				toleranceLine = atoi(argv[i]);
			else
			{
				std::cout << "invalid tolerance for Line: use -h, --help for usage information" << std::endl;
				exit(1);
			}
		}
		else if(temp == "-r" || temp == "--turd-size")
		{
			if(argc != i)
				turdSize = atoi(argv[i]);
			else
			{
				std::cout << "invalid turd size: use -h, --help for usage information" << std::endl;
				exit(1);
			}
		}
		else if(temp == "-m" || temp == "--median-blur-kernel-size")
		{
			if(argc != i)
				medianBlurKernelSize = atoi(argv[i]);
			else
			{
				std::cout << "invalid median blur kernel size: use -h, --help for usage information" << std::endl;
				exit(1);
			}
		}
		else if(temp == "-n" || temp == "--num-cluster")
		{
			if(argc != i)
				numClusters = atoi(argv[i]);
			else
			{
				std::cout << "invalid number of clusters: use -h, --help for usage information" << std::endl;
				exit(1);
			}
		}
		else if(temp == "-l" || temp == "--max-iters")
		{
			if(argc != i)
				KMeansMaxIters = atoi(argv[i]);
			else
			{
				std::cout << "invalid max number of kmeans iterations: use -h, --help for usage information" << std::endl;
				exit(1);
			}
		}
		else if(temp == "-c" || temp == "--bg-color")
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
					std::cout << "invalid background color: use -h, --help for usage information" << std::endl;
					exit(1);
				}
				bgColorProvided = true;
			}
			else
			{
				std::cout << "invalid background color: use -h, --help for usage information" << std::endl;
				exit(1);
			}
		}
		else if(temp == "-h" || temp == "--help")
		{
			std::cout << "b2v: Transforms bitmaps to vector graphics\n\nUsage:b2v [options] [filename...]\nDescription\n\nArguments: \n-h, --help \n\t\t show this help message and exit\n-i, --input-file <filename> \n\t\t read input PNG Bitmap image from this file\n-o, --output-file <filename> \n\t\t write output to this file(default=ouput.svg)\n-t, --curve-tolerance <n>  \n\t\t Bezier Curve Fitting tolerance(default=2)\n-s, --line-tolerance <n> \n\t\t Line Fitting tolerance(default=0)\n-c, --bg-color <\"#RRGGBB\"> OR <\"rgb(R,G,B)\"> \n\t\t Background Color in base 16 or 10(default=based on median of colors of four corner points of input image)\n-z, --noisy \n\t\t Required flag to switch on processing for noisy input image(default off)\n-m, --median-blur-kernel-size <n> \n\t\t Kernel size for median blur(default=3)\n-n, --num-cluster <n> \n\t\t number of Clusters/Colors in posterized image(default=8)\n-l, --max-iters <n> \n\t\t maximum number of iterations in kmeans(posterization)(default=50)\n-r, --turdSize <n> \n\t\t Turdsize = threshold number of pixels in an unwanted region(default=10)" << std::endl;
			exit(1);
		}
		else if(temp == "-z" || temp == "--noisy")
		{
			switchNoisy = true;
			std::cout << "Noisy Switched On" << std::endl;
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

	if(turdSize < 0)
	{
		std::cout << "invalid turd size: must be greater than zero" << std::endl;
		exit(1);
	}

	if(medianBlurKernelSize < 0 || medianBlurKernelSize%2 != 1)
	{
		std::cout << "invalid median blur kernel size: must be greater than zero and odd number" << std::endl;
		exit(1);
	}
	else
	{
		medianBlurKernelSize = (int)((medianBlurKernelSize - 1) * 0.5);
	}

	if(numClusters < 0)
	{
		std::cout << "invalid number of clusters: must be greater than zero" << std::endl;
		exit(1);
	}

	if(KMeansMaxIters < 0)
	{
		std::cout << "invalid max number of kmeans iterations: must be greater than zero" << std::endl;
		exit(1);
	}

	if(outFileName == "")
	{
		outFileName = "ouput.svg";
	}

	if(inputFileName == "")
	{
		std::cout << "input file not provided: use -h, --help for usage information" << std::endl;
		exit(1);
	}
}