#include <iostream>
#include <cstdio>
#include <cstdlib>

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
	Path* closedPath;
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

int main()
{
	int numPaths;
	int pathLength;
	Path *myPaths;

	scanf("%d", &numPaths);
	myPaths = (Path *)malloc(sizeof(numPaths) * sizeof(Path));

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
				scanf("%lf%lf", &myPaths[i].c[j][k].x, &myPaths[i].c[j][k].y);
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





	for(int i = 0; i < numPaths; ++i)
	{
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
	}

	cout << endl;

	for(int i = 0; i < numPaths; ++i)
	{
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
	}

	return 0;
}
