#ifndef __UTIL_H__
#define __UTIL_H__ 1

typedef unsigned char uchar;
typedef unsigned int uint;

#include <vector>
#include "GraphicsGems.h" 
#include <bitset>

struct Curve
{
	Curve* reverse;
	std::vector<Point2> pt;
	Point2 start;
	Point2 end;
};

struct Line
{
    uint start;
    uint end;
    std::vector<uint> path;
};

struct pixel
{
	uchar r;
	uchar g;
	uchar b;
};

struct Region
{
	std::vector< std::vector<Curve*> > closedPath;
	std::vector<uint> curveNum;
	pixel col;
};


struct ImageMatrix
{
	pixel **pixMap;
	uint height;
	uint width;
};
 
// A structure to represent an adjacency list
struct AdjList
{
    std::vector<uint> node;  		//ids' of nodes adjacent to this node
    uint id;						//id of node
    uint x;							//Actual x coordinate of node in image
   	uint y;							//Actual y coordinate of node in image
    std::bitset<1> isUsedUp;		//is node used in line segment formation
    std::bitset<1> isCntrlPoint;	//is node a control point
    std::bitset<1> isIslandPoint;	//is node an island point
   	std::vector<uint> adjRegion;	//Adjacent region codes
};

//returns 1 if pixels have same rgb values else 0
int ifEqualPixel(pixel, pixel);

//The image argument has width * height RGBA pixels or width * height * 4 bytes
void encodeOneStep(const char*, std::vector<uchar>&, uint, uint);
#endif