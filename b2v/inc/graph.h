#ifndef __GRAPH_H__
#define __GRAPH_H__ 1

#include "util.h"
#include "svg.h"
#include <vector>
#include <string>

typedef Point2 *BezierCurve;

/*
A class of Graph using adjacency list representation
that adds vertices dynamically i.e. vertices (white pixels) are
added as they are discovered.
*/
class Graph
{
	private:
		uint V;	//Real time number of vertices (Not TOTAL until all vertices are added)
		std::vector<AdjList> vertex;
		std::vector<Line> lineSeg;
		std::vector<Line> islandLineSeg;
		std::vector<Curve> curve;
		std::vector<Region> region;
		double tolerance;
		SVG *outputSVG;
		uint imageHeight;
		uint imageWidth;
		ImageMatrix *pop;
	public:
		Graph(uint, uint, std::vector<pixel>&, ImageMatrix*);
		~Graph(); 
		void addVertex(uint, uint, uint, bool, bool, bool);
		void addEdge(uint, uint);
		void addRegion(uint, uint);
		void setControlPoint(uint, bool);
		void setIslandPoint(uint, bool);
		void formLineSegments();
		void moveToNode(std::vector<uint>&, uint, uint, uint, uint, uint);
		bool checkCntrlPtAdj(uint, uint);
		uint getAdjCntrlPtInd(uint, uint);
		bool equalSideRegions(uint, uint);
		bool isAdjToFrom(uint, uint);
		void preprocessLineSegments();
		void formCurves();
		Curve* reverseCurve(Curve*);
		void assignCurveNumToRegion();
		void processRegions();
		void assignClosedPaths(Region&);
		int getNextPathIndex(std::vector<uint>&, Point2);
		bool ifForwardDirection(int, Point2);
		std::vector<uint> DouglasPeucker(std::vector<uint>&, double);
		double shortestDistanceToSegment(uint, uint, uint);
		void writeOuput(std::string);
};

void FitCurve(Point2 *d, int nPts,double error);

void DrawBezierCurve(int n, BezierCurve curve);

#endif