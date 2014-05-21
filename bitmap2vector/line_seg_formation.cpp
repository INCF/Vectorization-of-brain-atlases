#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

struct Lines
{
    int start;
    int end;
    vector<int> path;
};

// A structure to represent a node in adjacency list
struct AdjListNode
{
    int dest;
    int weight;
    struct AdjListNode* next;
};
 
// A structure to represent an adjacency liat
struct AdjList
{
    struct AdjListNode *head;  // pointer to head node of list
    int numAdjacentVertices;
    int isCntrlPoint;
    int isUsedUp;
    int id;
    int numAdjRegions;
    int isIslandPoint;
    int *adjRegion;
};
 
// A structure to represent a graph. A graph is an array of adjacency lists.
// Size of array will be V (number of vertices in graph)
struct Graph
{
    int V;
    struct AdjList* array;
};
 
// A utility function to create a new adjacency list node
struct AdjListNode* newAdjListNode(int dest, int weight)
{
    struct AdjListNode* newNode =
            (struct AdjListNode*) malloc(sizeof(struct AdjListNode));
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}
 
// A utility function that creates a graph of V vertices
struct Graph* createGraph(int V)
{
    struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph));
    graph->V = V;
 
    // Create an array of adjacency lists.  Size of array will be V
    graph->array = (struct AdjList*) malloc(V * sizeof(struct AdjList));
 
     // Initialize each adjacency list as empty by making head as NULL
    for (int i = 0; i < V; ++i)
        graph->array[i].head = NULL;
 
    return graph;
}
 
// Adds an edge to graph
void addEdge(struct Graph* graph, int src, int dest, int weight, int directed)
{
    // Add an edge from src to dest.  A new node is added to the adjacency
    // list of src.  The node is added at the begining
    struct AdjListNode* newNode = newAdjListNode(dest, weight);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;
 
    if(directed == 0)
    {
    // Since graph is undirected, add an edge from dest to src also
    newNode = newAdjListNode(src, weight);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
    }
}

// Calculates out-degree of a vertex
//// Note : out-degree can be calculated
//// quickly at the time of scanning itself
int outDeg(struct Graph *graph, int queryVertex)
{
    struct AdjListNode* node;
    int count=0;

    node = graph->array[queryVertex].head;

    while(node!=NULL)
    {
        ++count;
        node = node->next;
    }
    return count;
}

// Calculates in-degree of a vertex
//// Note : in-degree can be calculated
//// quickly at the time of scanning itself
int inDeg(struct Graph *graph, int queryVertex)
{
    struct AdjListNode* node;
    int count=0;

    for(int i = 0; i < graph->V; ++i)
    {
        node = graph->array[i].head;
        while(node!=NULL)
        {
            if(node->dest == queryVertex)
                ++count;
            node = node->next;
        }
    }
    return count;
}

// Prints the edges of the graph
void printGraph(struct Graph *graph)
{
    struct AdjListNode* node;

    for(int i = 0; i < graph->V; ++i)
    {
        printf("Out-Degree of vertex %d = %d\nIn-Degree of vertex %d = %d\n", i, outDeg(graph, i), i, inDeg(graph,i));
        node = graph->array[i].head;
        while(node != NULL)
        {
            printf("%d to %d with weight %d\n", i, node->dest, node->weight);
            node = node->next;
        }
    }
}

int check_from_adj(Graph *graph, int from, int dest)
{
    if(!graph->array[from].isCntrlPoint)
        return 0;
    AdjListNode* node = graph->array[from].head;
    while(node!=NULL)
    {
        if(node->dest == dest)
            return 1;
        node = node->next;
    }
    return 0;
}

int check_cntrl_adj(Graph *graph, int from)
{
    AdjListNode* node = graph->array[from].head;
    while(node!=NULL)
    {
        if(graph->array[node->dest].isCntrlPoint)
            return 1;
        node = node->next;
    }
    return 0;
}

int get_adj_cntrl_pt(Graph *graph, int from)
{
    AdjListNode* node = graph->array[from].head;
    while(node!=NULL)
    {
        if(graph->array[node->dest].isCntrlPoint)
            return node->dest;
        node = node->next;
    }
    cout << "Some Problem" << endl;
    return 0;
}

int check_same_side_regions(Graph *graph, int a, int b)
{
    if(graph->array[a].numAdjRegions != graph->array[b].numAdjRegions)
        return 0;
    
    sort(graph->array[a].adjRegion, graph->array[a].adjRegion + graph->array[a].numAdjRegions);
    sort(graph->array[b].adjRegion, graph->array[b].adjRegion + graph->array[b].numAdjRegions);
    for(int i = 0; i < graph->array[a].numAdjRegions; ++i)
    {
        if(graph->array[a].adjRegion[i] != graph->array[b].adjRegion[i])
            return 0;
    }
    
    return 1;
}

void move_to_node(Graph* graph, int to, vector<int>& temp, int from)
{
    // Check if there is an adjacent control point node which is not used up 
    // if yes then catch it and move to it
    // else perform the usual procedure
    int ind;
    if(check_cntrl_adj(graph, to) && !graph->array[ind = get_adj_cntrl_pt(graph, to)].isUsedUp)
    {
        temp.push_back(to);
    }
    else
    {
        AdjListNode *node = graph->array[to].head;
        while(node != NULL)
        {
            vector<int> temp2;
            if(check_same_side_regions(graph, to, node->dest) && !graph->array[node->dest].isUsedUp && !check_from_adj(graph, from, node->dest))
            {
                graph->array[node->dest].isUsedUp = 1;
                move_to_node(graph, node->dest, temp2, to);
                if(!temp2.empty())
                {
                    temp.assign(temp2.begin(), temp2.end());
                    temp.push_back(node->dest);
                    if(graph->array[temp[0]].isCntrlPoint)
                        break;
                }
            }
            node = node->next;   
        }
    }
    
}

void move_to_island_node(Graph *graph, int to, vector<int>& temp, int from, int startedFrom)
{
    if(to == startedFrom)
    {
        temp.push_back(to);
    }
    else
    {
        AdjListNode *node = graph->array[to].head;
        while(node != NULL)
        {
            vector<int> temp2;
            if(!graph->array[node->dest].isUsedUp && !check_from_adj(graph, from, node->dest) && node->dest != from)
            {
                graph->array[node->dest].isUsedUp = 1;
                move_to_island_node(graph, node->dest, temp2, to, startedFrom);
                if(!temp2.empty())
                {
                    temp.assign(temp2.begin(), temp2.end());
                        
                    if(graph->array[temp[0]].isCntrlPoint)
                        break;
                    else
                        temp.push_back(node->dest);
                }
            }
            node = node->next;   
        }
    }
}

 // Driver program to test above functions
int main()
{
    int numVertices, directed, numControlPoints;
    
    scanf("%d", &numVertices);
    struct Graph* graph = createGraph(numVertices);

    scanf("%d", &numControlPoints);
    int *cntrlPt = (int*)malloc(numControlPoints * sizeof(int));
    int counter = 0;

    directed = 1;

    for(int i = 0; i < numVertices; ++i)
    {
        int cntrl, num, numRegions;
        scanf("%d", &numRegions);
        graph->array[i].numAdjRegions = numRegions;
        graph->array[i].adjRegion = (int*) malloc(sizeof(int) * numRegions);
        scanf("%d%d", &cntrl, &num);
        if(cntrl)
        {
            cntrlPt[counter++] = i;
            graph->array[i].isUsedUp = 0;
        }
        else
        {
            graph->array[i].isUsedUp = 0;
        }
        graph->array[i].isCntrlPoint = cntrl;
        graph->array[i].numAdjacentVertices = num;
        graph->array[i].id = i;
        graph->array[i].isIslandPoint = 0;
        for(int j = 0; j < num; ++j)
        {
            int dest;
            scanf("%d", &dest);
            addEdge(graph, i, dest-1, 1, directed);
        } 
        for(int k = 0; k < numRegions; ++k)
        {
            int regionNum;
            scanf("%d", &regionNum);
            graph->array[i].adjRegion[k] = regionNum;
        }    
    }

    

    // Forming line segments
   
    int lineCounter = 1;

    

    for(int i = 0; i < numControlPoints; ++i)
    {
        graph->array[cntrlPt[i]].isUsedUp = 1;
        AdjListNode *node = graph->array[cntrlPt[i]].head;
        //cout << "main control point : " << cntrlPt[i] << endl;
        while(node != NULL)
        {
            vector<int> temp2;
            if(!graph->array[node->dest].isUsedUp)
            {//cout <<  "main : " << node->dest << endl;
                graph->array[node->dest].isUsedUp = 1;
                move_to_node(graph, node->dest, temp2, cntrlPt[i]);
                if(!temp2.empty())
                {
                    //cout << "pushed" << endl;
                    temp2.push_back(node->dest);
                    temp2.push_back(cntrlPt[i]);
                    
                    cout << "line_seg(" << lineCounter++ << ").a = [";
                    for(int j = temp2.size() - 1; j >= 0; --j)
                    {
                        cout << temp2[j] << " ";
                    }
                    cout << "];";
                    cout << endl;
                    
                   // line_seg[lineCounter].path.assign(temp.begin(), temp.end());
                   // line_seg[lineCounter].path.push_back(node->dest);
                   // line_seg[lineCounter].path.push_back(cntrlPt[i]);
                  //  lineCounter++;
                   // line_seg = (Lines*)realloc(line_seg, (lineCounter+1)*sizeof(Lines));
                }
            }
            node = node->next;
        }
        graph->array[cntrlPt[i]].isUsedUp = 0;
    }

    int islandLineCounter = 1;
    // Form Island segments with same start and end
    for(int i = 0; i < numVertices; ++i)
    {
        if(!graph->array[i].isUsedUp)
        {
            graph->array[i].isIslandPoint = 1;
            AdjListNode *node = graph->array[i].head;
            while(node != NULL)
            {
                vector<int> temp;
                if(!graph->array[node->dest].isUsedUp)
                {
                    graph->array[node->dest].isUsedUp = 1;
                    move_to_island_node(graph, node->dest, temp, i, i);
                    if(!temp.empty())
                    {
                        //cout << "pushed" << endl;
                        temp.push_back(node->dest);
                        temp.push_back(i);
                        
                        cout << "island_line_seg(" << islandLineCounter++ << ").a = [";
                        for(int j = temp.size() - 1; j >= 0; --j)
                        {
                            cout << temp[j] << " ";
                        }
                        cout << "];";
                        cout << endl;
                    }
                }
                node = node->next;

            }
            graph->array[i].isUsedUp = 1;
        }
    }   
    

/*

    // Print All Line segments
    for(int i = 0; i < lineCounter + 1; ++i)
    {
        for(int j = 0; j < line_seg[i].path.size(); ++j)
        {
            cout << line_seg[i].path[j] << " ";
        }
        cout << endl;
    }
*/
    free(graph->array);
    free(graph);

	return 0;
}