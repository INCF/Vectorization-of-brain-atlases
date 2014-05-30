#ifndef __SVG_H__
#define __SVG_H__ 1

#include <vector>
#include <string>
#include "util.h"

struct Curve;
struct Region;

class SVG
{
	private:
		uint imageHeight;
		uint imageWidth;
	public:
		SVG(uint, uint);
		~SVG();
		void writeDisjointLineSegments(std::vector<Curve>&);
		void writeFinalOutput(std::vector<Region>&, std::string);
};

#endif