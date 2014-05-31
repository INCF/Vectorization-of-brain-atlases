#include "graph.h"
#include "debug.h"
#include <fstream>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <bitset>
#include <string>

std::vector<Point2> *ptStore = new std::vector<Point2>; //stores bezier points as they are genereted/streamed

//LOOP: length of the loop from a control point to itself to make it to be considered as line segment
#define LOOP 10
//LIMIT: length of the segment to be considered for either DouglasPeuckar simplification or jump simplification
#define LIMIT 10
//JUMP: Number of pixels to be left while walking over the line segment
#define JUMP 5
//GAP: Net absolute change in the x and y direction of two pixels to be considered for a closed loop
#define GAP 6
//For douglas pecker algorithm
#define EPSILON 1


void DrawBezierCurve(int n, BezierCurve curve)
{
    for(int i = 0; i < n; ++i)
    {
    	Point2* temp = new Point2[1];
    	temp->x = curve[i].x;
    	temp->y = curve[i].y;
        ptStore->push_back(*temp);
    }
}

//Constructor
Graph::Graph(uint h, uint w, std::vector<pixel> &v, ImageMatrix *x)
{
	V = 0;
	imageHeight = h;
	imageWidth = w;
	outputSVG = new SVG(imageHeight, imageWidth);

	for(uint i = 0; i < v.size(); ++i)
	{
		Region *tempRegion = new Region[1];
		tempRegion->col = v[i];
		region.push_back(*tempRegion);
	}

	pop = x;
}

//Destructor
Graph::~Graph()
{
	for(uint i = 0; i < vertex.size(); ++i)
	{
		vertex[i].node.clear();
		vertex[i].adjRegion.clear();
	}
	vertex.clear();

	for(uint i = 0; i < lineSeg.size(); ++i)
	{
		lineSeg[i].path.clear();
	}
	lineSeg.clear();

	for(uint i = 0; i < islandLineSeg.size(); ++i)
	{
		islandLineSeg[i].path.clear();
	}
	islandLineSeg.clear();

	delete[] outputSVG;
}

//Adds a vertex to the graph with specified parameters set
void Graph::addVertex(uint id, uint x, uint y, bool isUsedUp = 0, bool isCntrlPoint = 0, bool isIslandPoint = 0)
{
	AdjList *list = new AdjList[1];
	list->id = id;
	list->x = x;
	list->y = y;
	list->isUsedUp.set(0,isUsedUp);
	list->isCntrlPoint.set(0,isCntrlPoint);
	list->isIslandPoint.set(0,isIslandPoint);
	vertex.push_back(*list);
	V = V + 1;	
}

// Adds an edge to graph
void Graph::addEdge(uint src, uint dest)
{
    // Add an edge from src to dest.  A new node is added to the adjacency
    // list of src.
    vertex[src].node.push_back(dest);
}

//Adds the code of adjacent region to the adjacent region vector of source vertex
void Graph::addRegion(uint src, uint ind)
{
	vertex[src].adjRegion.push_back(ind);
}

void Graph::setControlPoint(uint a, bool b)
{
	vertex[a].isCntrlPoint.set(0,b);
}

void Graph::setIslandPoint(uint a, bool b)
{
	vertex[a].isIslandPoint.set(0,b);
}

void Graph::formLineSegments()
{
	//Pass 1: finds line segments from control point to control point
	//Pass 2: finds islands and assign one point of that island as representative by setting isIslandPt
	for(int pass = 1; pass <= 3; ++pass)
	{
		for(uint i = 0; i < V; ++i)
		{
			if((vertex[i].isCntrlPoint.test(0) && pass == 1) || (!vertex[i].isUsedUp.test(0) && (pass == 2||pass==3)))
			{
				if(pass == 1)
					vertex[i].isUsedUp.set(0, true);
				else
					vertex[i].isIslandPoint.set(0, true);

				for(uint j = 0; j < vertex[i].node.size(); ++j)
				{
					Line *temp = new Line[1];
					if(!vertex[vertex[i].node[j]].isUsedUp.test(0))
					{
						vertex[vertex[i].node[j]].isUsedUp.set(0, true);
						moveToNode(temp->path, vertex[i].node[j], i, i, pass, 1);
						if(!temp->path.empty())
						{
							temp->path.push_back(vertex[i].node[j]);
							temp->path.push_back(i);
							std::reverse(temp->path.begin(), temp->path.end());
							temp->start = temp->path.front();
							temp->end = temp->path.back();
							if(pass == 1)
								lineSeg.push_back(*temp);
							else 
								islandLineSeg.push_back(*temp);
						}
					}
				}
				if(pass == 2)
					vertex[i].isUsedUp.set(0, true);
			}
		}
	}

	std::string path = ROOT_DIR;
	#ifdef _TEST_5_
	std::ofstream ofsTest5((path + "/check/test_5_cpp.txt").c_str(), std::ofstream::out);
	ofsTest5 << lineSeg.size() + islandLineSeg.size() << std::endl;

	ofsTest5 << "Line segments: " << std::endl;

	for(uint i = 0; i < lineSeg.size(); ++i)
	{
		ofsTest5 << lineSeg[i].path.size() << std::endl;
		for(uint j = 0; j < lineSeg[i].path.size(); ++j)
		{
			ofsTest5 << lineSeg[i].path[j] << " ";
		}
		ofsTest5 << std::endl;
	}

	ofsTest5 << "IsLand Line segments: " << std::endl;

	for(uint i = 0; i < islandLineSeg.size(); ++i)
	{
		ofsTest5 << islandLineSeg[i].path.size() << std::endl;
		for(uint j = 0; j < islandLineSeg[i].path.size(); ++j)
		{
			ofsTest5 << islandLineSeg[i].path[j] << " " ;
		}
		ofsTest5 << std::endl;
	}

	ofsTest5.close();
	#endif

	#ifdef _EVAL_3_
	//generate some image
	std::vector<unsigned char> image;
	image.resize(imageWidth * imageHeight * 4);
	for(uint y = 0; y < imageHeight; y++)
	{
		for(uint x = 0; x < imageWidth; x++)
		{
			image[4 * imageWidth * y + 4 * x + 0] = pop->pixMap[y][x].r;
			image[4 * imageWidth * y + 4 * x + 1] = pop->pixMap[y][x].g;
			image[4 * imageWidth * y + 4 * x + 2] = pop->pixMap[y][x].b;
			image[4 * imageWidth * y + 4 * x + 3] = 255;
		}
	}

	for(uint i = 0; i < lineSeg.size(); ++i)
	{
		for(uint j = 0; j < lineSeg[i].path.size(); ++j)
		{
			image[4 * imageWidth * vertex[lineSeg[i].path[j]].x + 4 * vertex[lineSeg[i].path[j]].y + 0] = 0;
			image[4 * imageWidth * vertex[lineSeg[i].path[j]].x + 4 * vertex[lineSeg[i].path[j]].y + 1] = 0;
			image[4 * imageWidth * vertex[lineSeg[i].path[j]].x + 4 * vertex[lineSeg[i].path[j]].y + 2] = 0;
			image[4 * imageWidth * vertex[lineSeg[i].path[j]].x + 4 * vertex[lineSeg[i].path[j]].y + 3] = 255;			
		}
	}

	for(uint i = 0; i < islandLineSeg.size(); ++i)
	{
		for(uint j = 0; j < islandLineSeg[i].path.size(); ++j)
		{
			image[4 * imageWidth * vertex[islandLineSeg[i].path[j]].x + 4 * vertex[islandLineSeg[i].path[j]].y + 0] = 255;
			image[4 * imageWidth * vertex[islandLineSeg[i].path[j]].x + 4 * vertex[islandLineSeg[i].path[j]].y + 1] = 0;
			image[4 * imageWidth * vertex[islandLineSeg[i].path[j]].x + 4 * vertex[islandLineSeg[i].path[j]].y + 2] = 0;
			image[4 * imageWidth * vertex[islandLineSeg[i].path[j]].x + 4 * vertex[islandLineSeg[i].path[j]].y + 3] = 255;			
		}
	}

	encodeOneStep((path + "/check/eval_3_lineSeg_beforeprocess.png").c_str(), image, imageWidth, imageHeight);
	image.clear();
	#endif

}

void Graph::moveToNode(std::vector<uint> &v, uint to, uint from, uint startedFrom, uint pass, uint len)
{
	uint ind;
	bool emptyFlag = true;
	if(pass == 2 && to == startedFrom)
	{
		v.push_back(to);
	}
	else if(pass == 1 && checkCntrlPtAdj(to, startedFrom) && ((!vertex[ind = getAdjCntrlPtInd(to, startedFrom)].isUsedUp.test(0)) || (len > LOOP && ind == startedFrom)))
	{
		v.push_back(ind);
	}
	else
	{
		for(uint i = 0; i < vertex[to].node.size(); ++i)
		{
			std::vector<uint> temp;
			if(equalSideRegions(to, vertex[to].node[i]) && !vertex[vertex[to].node[i]].isUsedUp.test(0) && !isAdjToFrom(from, vertex[to].node[i]) && vertex[to].node[i] != from)
			{
				vertex[vertex[to].node[i]].isUsedUp.set(0, true);
				len++;
				moveToNode(temp, vertex[to].node[i], to, startedFrom, pass, len);
				if(!temp.empty())
				{
					v.assign(temp.begin(), temp.end());
					if(vertex[to].node[i] != startedFrom)
						v.push_back(vertex[to].node[i]);
					//else no need to add because starting point already pushed in
					if(vertex[v[0]].isCntrlPoint.test(0))
						break;
					emptyFlag = false;
				}
				else if(pass == 2)
				{
					vertex[vertex[to].node[i]].isUsedUp.set(0, false);
				}
			}
		}
		if(emptyFlag && pass == 3 && len > LOOP)
		{
			if((abs(vertex[startedFrom].x - vertex[to].x) + abs(vertex[startedFrom].y - vertex[to].y)) <= GAP)
			{
				v.push_back(to);
				std::cout << "PASS 3" << std::endl;
			}
		}
	}
}

bool Graph::checkCntrlPtAdj(uint to, uint startedFrom)
{
	for(uint i = 0; i < vertex[to].node.size(); ++i)
	{
		if(vertex[vertex[to].node[i]].isCntrlPoint.test(0))
			return true;
	}
	return false;
}

uint Graph::getAdjCntrlPtInd(uint to, uint startedFrom)
{
	std::vector<uint> v;
	for(uint i = 0; i < vertex[to].node.size(); ++i)
	{
		if(vertex[vertex[to].node[i]].isCntrlPoint.test(0))
			v.push_back(vertex[to].node[i]);
	}
	if(v.size() == 1)
		return v[0];
	else
	{
		for(uint i = 0; i < v.size(); ++i)
		{
			if(v[i] != startedFrom)
				return v[i];
		}
	}
	//Will never come at this point
	std::cout << "Something BAD happened while retrieving control point at line segment formation step" << std::endl;
	exit(1);
}

bool Graph::equalSideRegions(uint a, uint b)
{
	if(vertex[a].adjRegion.size() != vertex[b].adjRegion.size())
		return false;

	std::sort(vertex[a].adjRegion.begin(), vertex[a].adjRegion.end());
	std::sort(vertex[b].adjRegion.begin(), vertex[b].adjRegion.end());

	for(uint i = 0; i < vertex[a].adjRegion.size(); ++i)
	{
		if(vertex[a].adjRegion[i] != vertex[b].adjRegion[i])
			return false;
	}

	return true;
}

bool Graph::isAdjToFrom(uint from, uint dest)
{
	for(uint i = 0; i < vertex[from].node.size(); ++i)
	{
		if(vertex[from].node[i] == dest)
			return true;
	}
	return false;
}

void Graph::preprocessLineSegments()
{
	
	//Preprocess : Take points at an appropriate interval depending on the length of line segment OR DouglasPeucker
#if 1

	std::vector<uint> tempLineSeg;
	for(uint i = 0; i < lineSeg.size(); ++i)
	{
		tempLineSeg.clear();
		if(lineSeg[i].path.size() > LIMIT)// if line length is above 6 then only go in and preprocess 
		{
			tempLineSeg.push_back(lineSeg[i].path[0]);
			tempLineSeg.push_back(lineSeg[i].path[1]);
			for(uint j = 2; j < lineSeg[i].path.size() - 2; j = j + 1 + JUMP)
			{
				tempLineSeg.push_back(lineSeg[i].path[j]);
			}
			tempLineSeg.push_back(lineSeg[i].path[lineSeg[i].path.size() - 2]);
			tempLineSeg.push_back(lineSeg[i].path[lineSeg[i].path.size() - 1]);

			lineSeg[i].path.clear();
			lineSeg[i].path.assign(tempLineSeg.begin(), tempLineSeg.end());
		}
	}

	for(uint i = 0; i < islandLineSeg.size(); ++i)
	{
		tempLineSeg.clear();
		if(islandLineSeg[i].path.size() > LIMIT)// if line length is above 5 then only go in and preprocess 
		{
			tempLineSeg.push_back(islandLineSeg[i].path[0]);
			tempLineSeg.push_back(islandLineSeg[i].path[1]);
			for(uint j = 2; j < islandLineSeg[i].path.size() - 2; j = j + 1 + JUMP)
			{
				tempLineSeg.push_back(islandLineSeg[i].path[j]);
			}
			tempLineSeg.push_back(islandLineSeg[i].path[islandLineSeg[i].path.size() - 2]);
			tempLineSeg.push_back(islandLineSeg[i].path[islandLineSeg[i].path.size() - 1]);

			islandLineSeg[i].path.clear();
			islandLineSeg[i].path.assign(tempLineSeg.begin(), tempLineSeg.end());
		}
	}

	tempLineSeg.clear();

	#endif
	#if 0

	std::vector<uint> tempLineSeg;
	for(uint i = 0; i < lineSeg.size(); ++i)
	{
		tempLineSeg.clear();
		if(lineSeg[i].path.size() > LIMIT)// if line length is above 5 then only go in and preprocess 
		{
			tempLineSeg = DouglasPeucker(lineSeg[i].path,EPSILON);

			lineSeg[i].path.clear();
			lineSeg[i].path.push_back(lineSeg[i].start);
			lineSeg[i].path.insert(lineSeg[i].path.begin() + 1, tempLineSeg.begin(), tempLineSeg.end());
			lineSeg[i].path.push_back(lineSeg[i].end);
		}
	}

	for(uint i = 0; i < islandLineSeg.size(); ++i)
	{
		tempLineSeg.clear();
		if(islandLineSeg[i].path.size() > LIMIT)// if line length is above 5 then only go in and preprocess 
		{
			tempLineSeg.push_back(islandLineSeg[i].path[0]);
			tempLineSeg.push_back(islandLineSeg[i].path[1]);
			for(uint j = 2; j < islandLineSeg[i].path.size() - 2; j = j + 1 + JUMP)
			{
				tempLineSeg.push_back(islandLineSeg[i].path[j]);
			}
			tempLineSeg.push_back(islandLineSeg[i].path[islandLineSeg[i].path.size() - 2]);
			tempLineSeg.push_back(islandLineSeg[i].path[islandLineSeg[i].path.size() - 1]);

			islandLineSeg[i].path.clear();
			islandLineSeg[i].path.assign(tempLineSeg.begin(), tempLineSeg.end());
		}
	}

	tempLineSeg.clear();
#endif

	#ifdef _EVAL_3_
	//generate some image
	std::string path = ROOT_DIR;
	std::vector<unsigned char> image;
	image.resize(imageWidth * imageHeight * 4);
	for(uint y = 0; y < imageHeight; y++)
	{
		for(uint x = 0; x < imageWidth; x++)
		{
			image[4 * imageWidth * y + 4 * x + 0] = pop->pixMap[y][x].r;
			image[4 * imageWidth * y + 4 * x + 1] = pop->pixMap[y][x].g;
			image[4 * imageWidth * y + 4 * x + 2] = pop->pixMap[y][x].b;
			image[4 * imageWidth * y + 4 * x + 3] = 255;
		}
	}

	for(uint i = 0; i < lineSeg.size(); ++i)
	{
		for(uint j = 0; j < lineSeg[i].path.size(); ++j)
		{
			image[4 * imageWidth * vertex[lineSeg[i].path[j]].x + 4 * vertex[lineSeg[i].path[j]].y + 0] = 0;
			image[4 * imageWidth * vertex[lineSeg[i].path[j]].x + 4 * vertex[lineSeg[i].path[j]].y + 1] = 0;
			image[4 * imageWidth * vertex[lineSeg[i].path[j]].x + 4 * vertex[lineSeg[i].path[j]].y + 2] = 0;
			image[4 * imageWidth * vertex[lineSeg[i].path[j]].x + 4 * vertex[lineSeg[i].path[j]].y + 3] = 255;			
		}
	}

	for(uint i = 0; i < islandLineSeg.size(); ++i)
	{
		for(uint j = 0; j < islandLineSeg[i].path.size(); ++j)
		{
			image[4 * imageWidth * vertex[islandLineSeg[i].path[j]].x + 4 * vertex[islandLineSeg[i].path[j]].y + 0] = 255;
			image[4 * imageWidth * vertex[islandLineSeg[i].path[j]].x + 4 * vertex[islandLineSeg[i].path[j]].y + 1] = 0;
			image[4 * imageWidth * vertex[islandLineSeg[i].path[j]].x + 4 * vertex[islandLineSeg[i].path[j]].y + 2] = 0;
			image[4 * imageWidth * vertex[islandLineSeg[i].path[j]].x + 4 * vertex[islandLineSeg[i].path[j]].y + 3] = 255;			
		}
	}

	encodeOneStep((path + "/check/eval_3_lineSeg_afterprocess.png").c_str(), image, imageWidth, imageHeight);
	image.clear();
	#endif

}

std::vector<uint> Graph::DouglasPeucker(std::vector<uint> &v, double epsilon)
{
	double dmax = 0;
	uint index = 0;
	for(uint i = 2; i < v.size() - 1; ++i)
	{
		double d = shortestDistanceToSegment(v[i], v.front(), v.back());
		if(d > dmax)
		{
			index = i;
			dmax = d;
		}
	}

	std::vector<uint> v3;

	if(dmax > epsilon)
	{
		std::vector<uint> v1(v.begin(), v.begin() + index);
		std::vector<uint> v2(v.begin() + index, v.end());
		v3 = DouglasPeucker(v1, epsilon);
		std::vector<uint> v4 = DouglasPeucker(v2, epsilon);
		v3.insert(v3.end(), v4.begin(), v4.end());
	}
	else
	{
		v3.push_back(v.front());
		v3.push_back(v.back());
	}
	return v3;
}

double Graph::shortestDistanceToSegment(uint i, uint j, uint k)
{
	double x1 = vertex[i].x;
	double y1 = vertex[i].y;
	double x2 = vertex[j].x;
	double y2 = vertex[j].y;
	double x3 = vertex[k].x;
	double y3 = vertex[k].y;
	if(x2 == x3)
		return abs(x1 - x2);

	double m = (y3 - y2) / (x3 - x2);
	double c = y2 - m * x2;

	return abs((y1 - m * x1 - c) / (sqrt(1 + m * m)));
}

void Graph::formCurves()
{
	tolerance = 0;
	for(uint i = 0; i < lineSeg.size(); ++i)
	{
		Point2 *d = new Point2[lineSeg[i].path.size() + 2];
		d[0].x = vertex[lineSeg[i].path[0]].y;
		d[0].y = vertex[lineSeg[i].path[0]].x;
		for(uint j = 0; j < lineSeg[i].path.size(); ++j)
		{
			d[j+1].x = vertex[lineSeg[i].path[j]].y;
			d[j+1].y = vertex[lineSeg[i].path[j]].x;
		}
		d[lineSeg[i].path.size()+1].x = vertex[lineSeg[i].path[lineSeg[i].path.size()-1]].y;
		d[lineSeg[i].path.size()+1].y = vertex[lineSeg[i].path[lineSeg[i].path.size()-1]].x;

		Curve *tempCurve = new Curve[1];
		tempCurve->reverse = new Curve[1];
		FitCurve(d, lineSeg[i].path.size() + 2, tolerance);
		tempCurve->pt.assign(ptStore->begin(), ptStore->end());
		tempCurve->start = ptStore->front();
		tempCurve->end = ptStore->back();
		ptStore->clear();
		tempCurve->reverse = reverseCurve(tempCurve);
		curve.push_back(*tempCurve);
	}

	for(uint i = 0; i < islandLineSeg.size(); ++i)
	{
		Point2 *d = new Point2[islandLineSeg[i].path.size() + 2];
		d[0].x = vertex[islandLineSeg[i].path[0]].y;
		d[0].y = vertex[islandLineSeg[i].path[0]].x;

		for(uint j = 0; j < islandLineSeg[i].path.size(); ++j)
		{
			d[j+1].x = vertex[islandLineSeg[i].path[j]].y;
			d[j+1].y = vertex[islandLineSeg[i].path[j]].x;
		}
		d[islandLineSeg[i].path.size()+1].x = vertex[islandLineSeg[i].path[islandLineSeg[i].path.size()-1]].y;
		d[islandLineSeg[i].path.size()+1].y = vertex[islandLineSeg[i].path[islandLineSeg[i].path.size()-1]].x;

		Curve *tempCurve = new Curve[1];
		tempCurve->reverse = new Curve[1];
		FitCurve(d, islandLineSeg[i].path.size() + 2, tolerance);
		tempCurve->pt.assign(ptStore->begin(), ptStore->end());
		tempCurve->start = ptStore->front();
		tempCurve->end = ptStore->back();
		ptStore->clear();
		tempCurve->reverse = reverseCurve(tempCurve);
		curve.push_back(*tempCurve);
	}

	#ifdef _TEST_6_
	std::string path = ROOT_DIR;
	std::ofstream ofsTest6((path + "/check/test_6_cpp.txt").c_str(), std::ofstream::out);
	ofsTest6 << curve.size() << std::endl;

	for(uint i = 0; i < curve.size(); ++i)
	{
		ofsTest6 << curve[i].pt.size() << std::endl;
		ofsTest6 << curve[i].start.x << " " << curve[i].start.y << std::endl;
		ofsTest6 << curve[i].end.x << " " << curve[i].end.y << std::endl;
		for(uint j = 0; j < curve[i].pt.size(); ++j)
		{
			ofsTest6 << curve[i].pt[j].x << " " << curve[i].pt[j].y << std::endl;
		}
	}

	ofsTest6.close();
	#endif

	#ifdef _EVAL_1_
	outputSVG->writeDisjointLineSegments(curve);
	#endif
}

Curve* Graph::reverseCurve(Curve *x)
{
	Curve *p =  new Curve[1];
	p->reverse = x;

	p->start = x->end;
	p->end = x->start;

	if(x->pt.size() < 3)
		std::cout << "MAYBE AN ERROR" << std::endl;

	for(int i = x->pt.size() - 2; i >= 0; --i)
	{
		p->pt.push_back(x->pt[i]);
	}
	p->pt.push_back(x->start);

	return p;
}

void Graph::assignCurveNumToRegion()
{
	for(uint i = 0; i < lineSeg.size(); ++i)
	{
		for(uint j = 0; j < lineSeg[i].path.size(); ++j)
		{
			if(!vertex[lineSeg[i].path[j]].isCntrlPoint.test(0))
			{
				if(vertex[lineSeg[i].path[j]].adjRegion.size() == 2)
				{
					region[vertex[lineSeg[i].path[j]].adjRegion[0]].curveNum.push_back(i);
					region[vertex[lineSeg[i].path[j]].adjRegion[1]].curveNum.push_back(i);
					break;
				}
				else
				{
					std::cout << "Something wrong: Not a COntrol Point but still 3 or more adjacent regions" << std::endl;
				}
			}
		}
	}
	for(uint i = 0; i < islandLineSeg.size(); ++i)
	{
		for(uint j = 0; j < islandLineSeg[i].path.size(); ++j)
		{
			if(vertex[islandLineSeg[i].path[j]].adjRegion.size() == 2)
			{
				region[vertex[islandLineSeg[i].path[j]].adjRegion[0]].curveNum.push_back(i + lineSeg.size());
				region[vertex[islandLineSeg[i].path[j]].adjRegion[1]].curveNum.push_back(i + lineSeg.size());
				break;
			}
			else
			{
				std::cout << "Something wrong: Not a Control Point but still 3 or more adjacent regions" << std::endl;
			}
		}
	}

	#ifdef _TEST_7_
	std::string path = ROOT_DIR;
	std::ofstream ofsTest7((path + "/check/test_7_cpp.txt").c_str(), std::ofstream::out);
	ofsTest7 << "No. of regions" << region.size() << std::endl;

	for(uint i = 0; i < region.size(); ++i)
	{
		ofsTest7 << "No. of curves assigned to region " << i << " : " << region[i].curveNum.size() << std::endl;
		for(uint j = 0; j < region[i].curveNum.size(); ++j)
		{
			ofsTest7 << region[i].curveNum[j] << " ";
		}
		ofsTest7 << std::endl;
	}

	ofsTest7.close();
	#endif
}

void Graph::processRegions()
{
	for(uint i = 0; i < region.size(); ++i)
	{
		assignClosedPaths(region[i]);
	}
}

void Graph::writeOuput(std::string outFileName)
{
	outputSVG->writeFinalOutput(region, outFileName);
}

void Graph::assignClosedPaths(Region &rgn)
{
	bool firstTime = true;
	int focus = 0;
	std::vector<Curve*> *tempPath;
	bool initiallyEmpty = rgn.curveNum.empty();

	while(!rgn.curveNum.empty())
	{
		if(firstTime)
		{
			tempPath = new std::vector<Curve*>();
			tempPath->clear();
			focus = rgn.curveNum.back();
			rgn.curveNum.pop_back();
			tempPath->push_back(&curve[focus]);
			firstTime = false;
		}
		else
		{
			if(tempPath->front()->start.x == tempPath->back()->end.x && tempPath->front()->start.y == tempPath->back()->end.y)
			{
				rgn.closedPath.push_back(*tempPath);
				firstTime = true;
			}
			else
			{
				focus = getNextPathIndex(rgn.curveNum, tempPath->back()->end);
				if(focus == -1)
					std::cout << "focus = - 1 i.e. empty still in :ERROR" << std::endl;
				else
				{
					//Erasing the corresponding curve number
					uint temp = rgn.curveNum[focus];
					rgn.curveNum.erase(rgn.curveNum.begin() + focus);
					focus = temp;

					if(ifForwardDirection(focus, tempPath->back()->end))
					{
						tempPath->push_back(&curve[focus]);
					}
					else
					{
						tempPath->push_back(curve[focus].reverse);
					}
				}
			}
		}
	}	
	if(!initiallyEmpty)
		rgn.closedPath.push_back(*tempPath);
}

int Graph::getNextPathIndex(std::vector<uint> &v, Point2 b)
{
	double val = 1000;  //No distance will exceed this disatance
	int ind = -1;

	for(uint i = 0; i < v.size(); ++i)
	{
		double myX = abs(curve[v[i]].start.x - b.x);
		double myY = abs(curve[v[i]].start.y - b.y);
		if(myX + myY < val)
		{
			val = myX + myY;
			ind = i;
		}

		myX = abs(curve[v[i]].end.x - b.x);
		myY = abs(curve[v[i]].end.y - b.y);
		if(myX + myY < val)
		{
			val = myX + myY;
			ind = i;
		}
	}
	
	return ind;
}

bool Graph::ifForwardDirection(int focus, Point2 b)
{
	double val1;
	double val2;
	val1 = abs(curve[focus].start.x - b.x) + abs(curve[focus].start.y - b.y);
	val2 = abs(curve[focus].end.x - b.x) + abs(curve[focus].end.y - b.y);

	if(val1 < val2)
		return true;
	else
		return false;
}