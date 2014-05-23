#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>

using namespace std;

struct Point
{
	double x, y;
};

struct Path
{
	Path* reverse;
	Point **c;
	Point start;
	Point end;
	int length;
};

struct Region
{
	int *pathNum;
	int numSurroundPaths;
	int r;
	int g;
	int b;
	Path** closedPath;
};

double cubicBezierValue(double t, double P0, double P1, double P2, double P3)
{
	return (1-t)*(1-t)*(1-t)*P0 + 3*(1-t)*(1-t)*t*P1 + 3*(1-t)*t*t*P2 + t*t*t*P3; 
}

void reverseCurve(Point* resCp1, Point* resCp2, Point start, Point cp1, Point cp2, Point end)
{
	double b1_x = cubicBezierValue(1.0 / 3.0, start.x, cp1.x, cp2.x, end.x);
	double b1_y = cubicBezierValue(1.0 / 3.0, start.y, cp1.y, cp2.y, end.y);
	double b2_x = cubicBezierValue(2.0 / 3.0, start.x, cp1.x, cp2.x, end.x);
	double b2_y = cubicBezierValue(2.0 / 3.0, start.y, cp1.y, cp2.y, end.y);

	resCp1->x = 1.5*(2.0*b1_x - b2_x) + (1.0/3.0)*start.x - (5.0/6.0)*end.x;
	resCp1->y = 1.5*(2.0*b1_y - b2_y) + (1.0/3.0)*start.y - (5.0/6.0)*end.y;

	resCp2->x = 1.5*(2.0*b2_x - b1_x) + (1.0/3.0)*end.x - (5.0/6.0)*start.x;
	resCp2->y = 1.5*(2.0*b2_y - b1_y) + (1.0/3.0)*end.y - (5.0/6.0)*start.y;
}

void reverseCurve2(Point* resCp1, Point* resCp2, Point start, Point cp1, Point cp2, Point end)
{
	resCp1->x = cp1.x + (2.0/9.0) * (end.x - start.x);
	resCp1->y = cp1.y + (2.0/9.0) * (end.y - start.y);

	resCp2->x = cp2.x - (4.0/9.0)*(end.x - start.x);
	resCp2->y = cp2.y - (4.0/9.0)*(end.y - start.y);
}

void reverseCurve3(Point* resCp1, Point* resCp2, Point start, Point cp1, Point cp2, Point end)
{
	resCp1->x = cp2.x;
	resCp1->y = cp2.y;
	
	resCp2->x = cp1.x;
	resCp2->y = cp1.y;	
}

Path* reversePath(Path* x)
{
	int len = x->length;
	Path *p = NULL;
	p = (Path *)malloc(sizeof(Path));
	p->length = len;
	p->reverse = x;
	p->c = (Point**) malloc(sizeof(Point*) * len);

	for(int i = 0; i < len; ++i)
	{
		p->c[i] = (Point*)malloc(sizeof(Point) * 3);
	}

	if(len == 1)
	{
		p->start = x->end;
		p->end = x->start;
		Point resultCp1, resultCp2;
		reverseCurve3(&resultCp1, &resultCp2, x->start, x->c[0][0], x->c[0][1], x->c[0][2]);
		p->c[0][0].x = resultCp1.x;
		p->c[0][1].x = resultCp2.x;
		p->c[0][2].x = x->c[0][0].x;
		p->c[0][0].y = resultCp1.y;
		p->c[0][1].y = resultCp2.y;
		p->c[0][2].y = x->c[0][0].y;
		return p;
	}

	p->start = x->end;
	p->end = x->start;

	for(int i = 0; i < len; ++i)
	{
		if(i == 0)
		{
			Point resultCp1, resultCp2;
			reverseCurve3(&resultCp1, &resultCp2, x->start, x->c[0][0], x->c[0][1], x->c[0][2]);
			p->c[len - 1][0].y = resultCp1.y;
			p->c[len - 1][1].y = resultCp2.y;
			p->c[len - 1][2].y = x->start.y;
			p->c[len - 2][2].y = x->c[0][2].y;
			p->c[len - 1][0].x = resultCp1.x;
			p->c[len - 1][1].x = resultCp2.x;
			p->c[len - 1][2].x = x->start.x;
			p->c[len - 2][2].x = x->c[0][2].x;
		}
		else if(i == len - 1)
		{
			Point resultCp1, resultCp2;
			reverseCurve3(&resultCp1, &resultCp2, x->c[len - 2][2], x->c[len - 1][0], x->c[len - 1][1], x->c[len - 1][2]);
			p->c[0][0].y = resultCp1.y;
			p->c[0][1].y = resultCp2.y;
			p->c[0][2].y = x->c[len-2][2].y;
			p->start.y = x->c[len-1][2].y;
			p->c[0][0].x = resultCp1.x;
			p->c[0][1].x = resultCp2.x;
			p->c[0][2].x = x->c[len-2][2].x;
			p->start.x = x->c[len-1][2].x;
		}
		else
		{
			Point resultCp1, resultCp2;
			reverseCurve3(&resultCp1, &resultCp2, x->c[i-1][2], x->c[i][0], x->c[i][1], x->c[i][2]);
			p->c[len - 1 - i][0].y = resultCp1.y;
			p->c[len - 1 - i][1].y = resultCp2.y;
			p->c[len - 1 - i][2].y = x->c[i-1][2].y;
			p->c[len - 2 - i][2].y = x->c[i][2].y;
			p->c[len - 1 - i][0].x = resultCp1.x;
			p->c[len - 1 - i][1].x = resultCp2.x;
			p->c[len - 1 - i][2].x = x->c[i-1][2].x;
			p->c[len - 2 - i][2].x = x->c[i][2].x;
		}
	}
	return p;
}

int getNextPathIndex(int *a, int len, Point b, Path *path_list, int numPaths)
{
	int val = 1000;
	int ind = -1;
	int i;
	// 0 OR 1 pixel near check
	for(i = 0; i < len; ++i)
	{
		if(a[i] != -1)
		{
			if(a[i] >= numPaths)
			{
				cout << "ERROR -----" << endl;
			}
			else
			{
				int myX = abs(path_list[a[i]].start.x - b.x);
				int myY = abs(path_list[a[i]].start.y - b.y);
				if(myX + myY < val)
				{
					val = myX + myY;
					ind = i;
				}

				myX = abs(path_list[a[i]].end.x - b.x);
				myY = abs(path_list[a[i]].end.y - b.y);
				if(myX + myY < val)
				{
					val = myX + myY;
					ind = i;
				}
			}
		}
	}
	
	return ind;
}

int sameOrRevDirection(Path *path_list, int focus, Point b)
{
	int val1;
	int val2;
	val1 = abs(path_list[focus].start.x - b.x) + abs(path_list[focus].start.y - b.y);
	val2 = abs(path_list[focus].end.x - b.x) + abs(path_list[focus].end.y - b.y);

	if(val1 < val2)
		return 1;
	else
		return 0;
}

int checkSame(Point a, Point b)
{
	if(a.x == b.x && a.y == b.y)
		return 1;
	else
		return 0;
}

void processRegion(Region *region, Path *path_list, int numPaths)
{
	int counter = region->numSurroundPaths;
	int pushed = 0;
	int focus = 0;
	int revFlag = 0;
	while(counter > 0)
	{
		if(pushed == 0)
		{
			if(region->pathNum[pushed] >= numPaths)
				cout << "Some Problem" << endl;
			region->closedPath[pushed] = &path_list[region->pathNum[focus]];
			region->pathNum[pushed] = -1;  // -1 IMPLIES USED UP PATH
			counter--;
			pushed++;	
			revFlag = 0;		
		}
		else
		{
			// If initial point becomes equal  to final point return ... 
			// This will prevent interaction with islands
			// Also reduce the number of surrounding paths
			if(checkSame(region->closedPath[0]->start, region->closedPath[pushed - 1]->end))
			{
				region->numSurroundPaths = pushed;
				return;
			}

			focus = getNextPathIndex(region->pathNum, region->numSurroundPaths, (revFlag == 0)?(region->closedPath[pushed - 1]->end):(region->closedPath[pushed - 1]->end), path_list, numPaths);
			if(focus == -1)
				cout << "ERROR" << endl;
			else
			{
				int temp = region->pathNum[focus];
				region->pathNum[focus] = -1;
				focus = temp;
				
				if(sameOrRevDirection(path_list, focus, (revFlag == 0)?(region->closedPath[pushed - 1]->end):(region->closedPath[pushed - 1]->end)))
				{
					region->closedPath[pushed] = &path_list[focus];
					counter--;
					pushed++;
					revFlag = 0;
				}
				else
				{
					region->closedPath[pushed] = path_list[focus].reverse;
					counter--;
					pushed++;
					revFlag = 1;
				}
			}
		}
	}
}

int main()
{
	int numPaths;
	int pathLength;
	int imageHeight;
	int imageWidth;
	Path *myPaths = NULL;

	scanf("%d", &imageHeight);
	scanf("%d", &imageWidth);

	scanf("%d", &numPaths);
	myPaths = (Path*)malloc(numPaths * sizeof(Path));

	for(int i = 0; i < numPaths; ++i)
	{	
		scanf("%d", &pathLength);
		myPaths[i].length = pathLength;
		myPaths[i].c = (Point**)malloc(sizeof(Point*) * pathLength);
		for(int j = 0;j < pathLength; ++j)
		{
			myPaths[i].c[j] = (Point*)malloc(sizeof(Point) * 3);
			for(int k = 0; k < 3; ++k)
			{
				double myX, myY;
				scanf("%lf%lf", &myX, &myY);
				myPaths[i].c[j][k].x = myX;
				myPaths[i].c[j][k].y = myY;
				if(j == 0 && k == 0)
				{
					myPaths[i].start.x = myPaths[i].c[0][0].x;
					myPaths[i].start.y = myPaths[i].c[0][0].y;
				}
				if(j == pathLength - 1 && k == 2)
				{
					myPaths[i].end.x = myPaths[i].c[pathLength -1][2].x;
					myPaths[i].end.y = myPaths[i].c[pathLength -1][2].y;
				}

			}
		}
		myPaths[i].reverse = (Path*)malloc(sizeof(Path));
		myPaths[i].reverse = reversePath(&myPaths[i]);
	}


	int numRegions;
	scanf("%d", &numRegions);

	Region *myRegions = (Region*)malloc(numRegions * sizeof(Region));

	for(int i = 0; i < numRegions; ++i)
	{
		int surrPaths;
		scanf("%d", &surrPaths);
		myRegions[i].numSurroundPaths = surrPaths;
		myRegions[i].pathNum = (int*)malloc(surrPaths * sizeof(int));
		myRegions[i].closedPath = (Path**)malloc(surrPaths * sizeof(Path*));

		for(int j = 0; j < surrPaths; ++j)
		{
			int temp;
			scanf("%d", &temp);
			myRegions[i].pathNum[j] = temp;
		}
		int red, blue, green;
		scanf("%d%d%d", &red, &green, &blue);
		myRegions[i].r = red;
		myRegions[i].g = green;
		myRegions[i].b = blue;

		processRegion(&myRegions[i], myPaths, numPaths);
	}

cout << "<!DOCTYPE html>" << endl << "<html>" << endl << "<body>" << endl << "<svg height=\"" << imageHeight << "\" width=\"" << imageWidth << "\">" << endl << "<g>" << endl;

	// Ouput final svg paths with bezier curves only

	for(int i = 0; i < numRegions; ++i)
	{
		if(i != 0)
			cout << "<path  style=\"fill:rgb(" << myRegions[i].r << "," << myRegions[i].g << "," << myRegions[i].b << "); stroke-width:1; stroke:rgb(" << myRegions[i].r << "," << myRegions[i].g << "," << myRegions[i].b << ") \" d = \"";
		else
			cout << "<path fill=\"none\" d =\" ";
		for(int j = 0; j < myRegions[i].numSurroundPaths; ++j)
		{
			for(int k = 0; k < myRegions[i].closedPath[j]->length; ++k)
			{
				for(int l = 0; l < 3; ++l)
				{
					if(j == 0)
					{
						if(k == 0 && l == 0)
						{
							cout << "M" << myRegions[i].closedPath[j]->start.x << " " << myRegions[i].closedPath[j]->start.y << " C";
						}
					}
					else
					{
						if(k == 0 && l == 0)
						{
							//cout << "L" << myRegions[i].closedPath[j]->start.x << " " << myRegions[i].closedPath[j]->start.y << " C";
						}
					}
					cout << myRegions[i].closedPath[j]->c[k][l].x << "," << myRegions[i].closedPath[j]->c[k][l].y << " ";
				}
			}
		}
		cout << " Z\" />" << endl;
		cout << endl;
	}


/* Only Lines
	
	for(int i = 0; i < numRegions; ++i)
	{
		if(i != 0)
			cout << "<path  style=\"fill:rgb(" << myRegions[i].r << "," << myRegions[i].g << "," << myRegions[i].b << "); stroke-width:1; stroke:rgb(" << myRegions[i].r << "," << myRegions[i].g << "," << myRegions[i].b << ") \" d = \"";
		else
			cout << "<path fill=\"none\" d =\" ";
		for(int j = 0; j < myRegions[i].numSurroundPaths; ++j)
		{
			for(int k = 0; k < myRegions[i].closedPath[j]->length; ++k)
			{
				for(int l = 0; l < 3; ++l)
				{
					if(j == 0)
					{
						if(k == 0 && l == 0)
						{
							cout << "M" << myRegions[i].closedPath[j]->start.x << " " << myRegions[i].closedPath[j]->start.y << " L";
						}
					}
					else
					{
						if(k == 0 && l == 0)
						{
							//cout << "L" << myRegions[i].closedPath[j]->start.x << " " << myRegions[i].closedPath[j]->start.y << " C";
						}
					}
					if(l == 2)
					cout << myRegions[i].closedPath[j]->c[k][l].x << "," << myRegions[i].closedPath[j]->c[k][l].y << " L";
				}
			}
		}
		cout << " Z\" />" << endl;
		cout << endl;
	}
	*/
cout << "</g>" << endl << "</svg>" << endl << "</body>" << endl << "</html>" << endl;

/*
	for(int i = 0; i < numPaths; ++i)
	{
		cout << "<path fill=\"none\" stroke=\"black\" stroke-width=\"1\" d =\" ";
		for(int j = 0;j < myPaths[i].length; ++j)
		{
			for(int k = 0; k < 3; ++k)
			{
				if(j == 0 && k == 0)
				{
					cout << "M" << myPaths[i].start.x << " " << myPaths[i].start.y << " C";
				}
				cout << myPaths[i].c[j][k].x << "," << myPaths[i].c[j][k].y << " ";
			}
		}
		cout << " \" />" << endl;
	}


	for(int i = 0; i < numPaths; ++i)
	{
		cout << "<path fill=\"none\" stroke=\"red\" stroke-width=\"1\" d =\" ";
		for(int j = 0;j < myPaths[i].length; ++j)
		{
			for(int k = 0; k < 3; ++k)
			{
				if(j == 0 && k == 0)
				{
					cout << "M" << myPaths[i].reverse->start.x << " " << myPaths[i].reverse->start.y << " C";
				}
				cout << myPaths[i].reverse->c[j][k].x << "," << myPaths[i].reverse->c[j][k].y << " ";
			}
		}
		cout << " \" />" << endl;
	}

*/
	


	return 0;
}
