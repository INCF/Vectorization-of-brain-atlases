#include "svg.h"
#include <fstream>
#include <string>

SVG::SVG(uint h, uint w)
{
	imageHeight = (h-4)*0.5;
	imageWidth = (w-4)*0.5;
}

SVG::~SVG()
{

}

/*
 * Writes an svg with curves only.
 * Both forward and reverse curve are written and the resultant
 * svg is expected with only red curves which overlap black curves
 * NOTE: This method will be called only if _EVAL_4_ is defined
 */
void SVG::writeDisjointLineSegments(std::vector<Curve> &v)
{
	std::string path = ROOT_DIR;
	std::ofstream ofsTest6((path + "/check/eval_4_borders.svg").c_str(), std::ofstream::out);

	ofsTest6 << "<svg height=\"" << imageHeight << "\" width=\"" << imageWidth << "\">" << std::endl << "<g>" << std::endl;
	//white backgoround
	ofsTest6 << "<rect width=\"" << imageWidth << "\" height=\"" << imageHeight << "\" fill=\"#ffffff\" stroke-width=\"0\" /> " << std::endl;

	//iterate over all curves
	for(uint i = 0; i < v.size(); ++i)
	{
		ofsTest6 << "<path fill=\"none\" stroke=\"black\" stroke-width=\"0.5\" d =\" ";

		//print forward curve path
		for(uint j = 0;j < v[i].pt.size(); ++j)
		{
			if(j == 0)
			{
				ofsTest6 << "M" << v[i].start.x << " " << v[i].start.y << " C";
			}
			else
				ofsTest6 << v[i].pt[j].x << "," << v[i].pt[j].y << " ";
		}
		ofsTest6 << " \" />" << std::endl;
	}

	//iterate over all curves
	for(uint i = 0; i < v.size(); ++i)
	{
		ofsTest6 << "<path fill=\"none\" stroke=\"red\" stroke-width=\"0.5\" d =\" ";

		//print reverse curve path
		for(uint j = 0;j < v[i].reverse->pt.size(); ++j)
		{
			if(j == 0)
			{
				ofsTest6 << "M" << v[i].reverse->start.x << " " << v[i].reverse->start.y << " C";
			}
			else
				ofsTest6 << v[i].reverse->pt[j].x << "," << v[i].reverse->pt[j].y << " ";
		}
		ofsTest6 << " \" />" << std::endl;
	}

	ofsTest6 << "</g>" << std::endl << "</svg>" << std::endl;

	ofsTest6.close();
}

/*
 * Writes Final SVG corresponding to original image
 * Arguments: Vector of regions in the image and output File name
 */
void SVG::writeFinalOutput(std::vector<Region> &rgn, std::string outFileName)
{
	std::ofstream ofsFinal(outFileName.c_str(), std::ofstream::out);
	ofsFinal << "<svg height=\"" << imageHeight << "\" width=\"" << imageWidth << "\">" << std::endl << "<g>" << std::endl;
	
	// Ouput final svg paths with bezier curves only
	for(uint i = 0; i < rgn.size(); ++i)
	{
		//No printing of empty paths
		if(rgn[i].closedPath.empty())
			continue;
		if(i != 0)
		{
			if(rgn[i].closedPath.size() == 1)
				ofsFinal << "<path  fill=\"" << RGBToHex((uint)rgn[i].col.r, (uint)rgn[i].col.g, (uint)rgn[i].col.b) << "\" stroke-width=\"0\" stroke=\"" << RGBToHex((uint)rgn[i].col.r, (uint)rgn[i].col.g, (uint)rgn[i].col.b) << "\" d = \"";
			else
				ofsFinal << "<path  fill-rule=\"evenodd\" fill=\"" << RGBToHex((uint)rgn[i].col.r, (uint)rgn[i].col.g, (uint)rgn[i].col.b) << "\" stroke-width=\"0\" stroke=\"" << RGBToHex((uint)rgn[i].col.r, (uint)rgn[i].col.g, (uint)rgn[i].col.b) << "\" d = \"";	
		}
		else
			ofsFinal << "<path fill=\"none\" d =\" ";

		//iterate over the closedpaths surrounding the region
		for(uint j = 0; j < rgn[i].closedPath.size(); ++j)
		{
			//iterate over the curves in a closed path
			for(uint k = 0; k < rgn[i].closedPath[j].size(); ++k)
			{
				//iterate over the points in the curve
				for(uint m = 0; m < rgn[i].closedPath[j][k]->pt.size(); ++m)
				{						
					if(m == 0)
					{
						//print starting point of first curve in a closed path
						if(k == 0)
							ofsFinal << "M" << rgn[i].closedPath[j][k]->start.x << " " << rgn[i].closedPath[j][k]->start.y << " C";
					}
					else
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