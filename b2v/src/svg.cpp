#include "svg.h"
#include <fstream>

SVG::SVG(uint h, uint w)
{
	imageHeight = h;
	imageWidth = w;
}

SVG::~SVG()
{

}

void SVG::writeDisjointLineSegments(std::vector<Curve> &v)
{
	std::ofstream ofsTest6("evaluation/eval_1_cpp.svg", std::ofstream::out);

	ofsTest6 << "<svg height=\"" << imageHeight << "\" width=\"" << imageWidth << "\">" << std::endl << "<g>" << std::endl;


	for(uint i = 0; i < v.size(); ++i)
	{
		ofsTest6 << "<path fill=\"none\" stroke=\"black\" stroke-width=\"0.5\" d =\" ";
		for(uint j = 0;j < v[i].pt.size(); ++j)
		{
			if(j == 0)
			{
				ofsTest6 << "M" << v[i].start.x << " " << v[i].start.y << " C";
			}
			ofsTest6 << v[i].pt[j].x << "," << v[i].pt[j].y << " ";
		}
		ofsTest6 << " \" />" << std::endl;
	}

	for(uint i = 0; i < v.size(); ++i)
	{
		ofsTest6 << "<path fill=\"none\" stroke=\"red\" stroke-width=\"0.5\" d =\" ";
		for(uint j = 0;j < v[i].reverse->pt.size(); ++j)
		{
			if(j == 0)
			{
				ofsTest6 << "M" << v[i].reverse->start.x << " " << v[i].reverse->start.y << " C";
			}
			ofsTest6 << v[i].reverse->pt[j].x << "," << v[i].reverse->pt[j].y << " ";
		}
		ofsTest6 << " \" />" << std::endl;
	}

	ofsTest6 << "</g>" << std::endl << "</svg>" << std::endl;

	ofsTest6.close();
}

void SVG::writeFinalOutput(std::vector<Region> &rgn, std::string outFileName)
{
	std::ofstream ofsFinal(outFileName.c_str(), std::ofstream::out);
	ofsFinal << "<svg height=\"" << imageHeight << "\" width=\"" << imageWidth << "\">" << std::endl << "<g>" << std::endl;

	// Ouput final svg paths with bezier curves only
	for(uint i = 0; i < rgn.size(); ++i)
	{
		if(i != 0)
		{
			if(rgn[i].closedPath.size() == 1)
				ofsFinal << "<path  style=\"fill:rgb(" << (int)rgn[i].col.r << "," << (int)rgn[i].col.g << "," << (int)rgn[i].col.b << "); stroke-width:0; stroke:rgb(" << (int)rgn[i].col.r << "," << (int)rgn[i].col.g << "," << (int)rgn[i].col.b << ") \" d = \"";
			else
				ofsFinal << "<path  style=\"fill-rule:evenodd;fill:rgb(" << (int)rgn[i].col.r << "," << (int)rgn[i].col.g << "," << (int)rgn[i].col.b << "); stroke-width:0; stroke:rgb(" << (int)rgn[i].col.r << "," << (int)rgn[i].col.g << "," << (int)rgn[i].col.b << ") \" d = \"";	
		}
		else
			ofsFinal << "<path fill=\"none\" d =\" ";

		for(uint j = 0; j < rgn[i].closedPath.size(); ++j)
		{
			for(uint k = 0; k < rgn[i].closedPath[j].size(); ++k)
			{
				for(uint m = 0; m < rgn[i].closedPath[j][k]->pt.size(); ++m)
				{						
					if(k == 0 && m == 0)
					{
						ofsFinal << "M" << rgn[i].closedPath[j][k]->start.x << " " << rgn[i].closedPath[j][k]->start.y << " C";
					}					
					ofsFinal << rgn[i].closedPath[j][k]->pt[m].x << "," << rgn[i].closedPath[j][k]->pt[m].y << " ";
				}
			}
		}
		ofsFinal << " Z\" />" << std::endl;
		ofsFinal << std::endl;
	}
	ofsFinal << "</g>" << std::endl << "</svg>" << std::endl;

	ofsFinal.close();
}