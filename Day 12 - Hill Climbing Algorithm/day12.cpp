#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>

const int MAX_NODE_VAL = 1000000;

struct Point
{
  int x, y;
};

struct Node 
{
  int cost;
  char height;
  bool visited;
  Point position;
  std::vector<Node*> neighbours;
};

struct compare
{
  bool operator()(const Node* a, const Node* b)
  {
    return a->cost > b->cost;
  }
};

void readInput (std::vector<std::vector<Node>> &groundMap, Point &start, Point &destination, const std::string &filename)
{
  std::ifstream file(filename);
  std::string line;
  int lineCount = 0;

  while (std::getline(file, line))
  {
    std::vector<Node> mapLine;

    for (int i = 0; i < line.size(); i++)
    {
      Node node = { MAX_NODE_VAL, line[i], false, { i, lineCount } };

      if (line[i] == 'S') 
      {
        node.cost = 0;
        node.height = 'a';
        node.visited = true;
        start.x = i;
        start.y = lineCount;
      }
      else if (line[i] == 'E')
      {
        node.height = 'z';
        destination.x = i;
        destination.y = lineCount;
      }

      mapLine.push_back(node);
    }
    groundMap.push_back(mapLine);
    lineCount++;
  }
}

// Loop through the matrix of nodes and determine which nodes are neighbours, storing those.
void setNeighbours(std::vector<std::vector<Node>> &groundMap, bool reverseClimb)
{
  std::vector<Point> directions = {{1,0}, {-1,0}, {0,1}, {0,-1}};
  for (int i = 0; i< groundMap.size(); i++)
  {
    for (int j = 0; j < groundMap[0].size(); j++)
    {
      for (auto& dir : directions)
      {
        if (i + dir.y < 0 || i + dir.y >= groundMap.size() || j + dir.x < 0 || j + dir.x >= groundMap[0].size())
        {
          continue;
        }

        // Change the rules for climbing up if we are 'reverse climbing'
        if (!reverseClimb)
        {
          // Part 1
          if (abs(groundMap[i + dir.y][j + dir.x].height - groundMap[i][j].height) <= 1 || groundMap[i][j].height > groundMap[i + dir.y][j + dir.x].height)
          {
            groundMap[i][j].neighbours.push_back(&groundMap[i + dir.y][j + dir.x]);
          }
        }
        else
        {
          // Part 2
          if (abs(groundMap[i + dir.y][j + dir.x].height - groundMap[i][j].height) <= 1 || groundMap[i][j].height < groundMap[i + dir.y][j + dir.x].height)
          {
            groundMap[i][j].neighbours.push_back(&groundMap[i + dir.y][j + dir.x]);
          }
        }
      }
    }
  }
}

// Inspect all the current nodes neighbours, updating cost if possible, and pushing to the queue
// if the node has not been visited.
void addNeighbourToQueue(std::priority_queue<Node*, std::vector<Node*>, compare> &pq, Node* currentNode)
{
  
  for (auto neighbour : currentNode->neighbours)
  {
    if (currentNode->cost + 1 < neighbour->cost)
    {
      neighbour->cost = currentNode->cost + 1;
    }

    if (!neighbour->visited)
    {
      pq.push(neighbour);
    }
  }
}

// Calculate the shortest path from a given origin point.
void shortestPathFromOrigin (std::vector<std::vector<Node>> &groundMap, Point &origin)
{
  // create min priority queue to keep the thing with the smallest distance next in queue
  std::priority_queue<Node*, std::vector<Node*>, compare> pq;
  Node originNode = groundMap[origin.y][origin.x];
  

  // std::cout << "Start : " << origin.x << "," << origin.y << " " << originNode.height << std::endl;

  // Initialize the priority queue with origin's neighbours
  for (auto neighbour : originNode.neighbours)
  {
    // std::cout << "Start Neighbours: (" << neighbour->position.x << "," << neighbour->position.y << "):" << neighbour->height << std::endl;
    neighbour->cost = 1;
    pq.push(neighbour);
  }

  // Loop through minimum priority queue, checking all nodes that we have
  // passed across so far
  while(!pq.empty())
  {
    Node* currentNode = pq.top();
    pq.pop();

    if (!currentNode->visited)
    {
      addNeighbourToQueue(pq, currentNode);
      currentNode->visited = true;
    }
  }
}

// Calculate the shortest path from a start point to a destination
int shortestPath (std::vector<std::vector<Node>> &groundMap, Point &start, Point &destination)
{

  shortestPathFromOrigin(groundMap, start);
  
  std::cout << "Done! : ";
  std::cout << groundMap[destination.y][destination.x].height << " " << groundMap[destination.y][destination.x].cost << std::endl;

  return groundMap[destination.y][destination.x].cost;
}

// Find the minimum path from the destination to all points with height 'a'.
int shortestPathFromLowestPoints(std::vector<std::vector<Node>> &groundMap, Point &destination)
{
  int minPath = MAX_NODE_VAL;

  shortestPathFromOrigin(groundMap, destination);

  for (int i = 0; i < groundMap.size(); i++)
  {
    for (int j = 0; j < groundMap[0].size(); j++)
    {
      if (groundMap[i][j].height == 'a' && groundMap[i][j].cost < minPath && groundMap[i][j].cost != 0)
      {
        std::cout << "(" << groundMap[i][j].position.x << "," << groundMap[i][j].position.y << "): " << groundMap[i][j].height << " :: " << groundMap[i][j].cost << std::endl;
        minPath = groundMap[i][j].cost;
      }
    }
  }
  std::cout << "Shortest path from lowest points: " << minPath << std::endl;
  return minPath;
}

int main (int argc, char** argv)
{
  std::string filename(argv[1]);
  std::vector<std::vector<Node>> groundMap;
  Point start = { 0, 0 };
  Point destination = { 0, 0 };
  int partNum = 2;

  readInput(groundMap, start, destination, filename);

  if (partNum == 1)
  {
    setNeighbours(groundMap, false);
    int shortestPathLength = shortestPath(groundMap, start, destination);
  }
  else if (partNum == 2)
  {
    // SetNeighbours needs the boolean flag to know that we are traversing 'downhill' from destination to
    // the low points.
    setNeighbours(groundMap, true);
    int shortestPathLowPoints = shortestPathFromLowestPoints(groundMap, destination);
  }

}