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
#include "util.h"

int main(int argc, char **argv)
{
	Info inputInfo;
	inputInfo.parseInputArg(argc, argv);

	Bitmap inputBitmap(inputInfo.inputFileName, inputInfo.bgColor, inputInfo.bgColorProvided);
	if(inputInfo.switchNoisy)
		inputBitmap.removeNoise(inputInfo.turdSize, inputInfo.medianBlurKernelSize, inputInfo.numClusters, inputInfo.KMeansMaxIters);
	inputBitmap.processImage(inputInfo.toleranceCurve, inputInfo.toleranceLine);
	inputBitmap.writeOuputSVG(inputInfo.outFileName);
	
	return 0;
}

