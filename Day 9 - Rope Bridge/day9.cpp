#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <map>
#include <math.h>
#include <algorithm>

struct point
{
    point() : x(0), y(0){}
    point(int x, int y) : x(x), y(y){}

    bool operator==(const point &other) const{
        return (x == other.x) && (y == other.y);
    }

    point operator+(const point &other) const {
      return { (x + other.x) , (y + other.y) }; 
    }

    point operator-(const point &other) const {
      return { x - other.x, y - other.y };
    }

    void move(const point &dir)
    {
      x += dir.x == 0 ? dir.x : dir.x / abs(dir.x);
      y += dir.y == 0 ? dir.y : dir.y / abs(dir.y);
    }

    int x;
    int y;
};


struct pointHash 
{
public:
	size_t operator()(const point p) const 
  {
		return std::hash<int>()(p.x) ^ std::hash<int>()(p.y);
	}
};

struct Action 
{
  char dir;
  int distance;
};


std::vector<Action> readInput (std::string &filename)
{
  std::ifstream file(filename);
  std::string line;
  std::vector<Action> actions;

  while(std::getline(file, line))
  {
    std::stringstream linestream(line);
    std::string word;

    Action action;

    std::getline(linestream, word, ' ');
    action.dir = word[0];

    std::getline(linestream, word);
    action.distance = std::stoi(word);

    actions.push_back(action);
  }

  return actions;
}

// Part 2
int findPositionsNthTail (int numSegments, std::vector<Action> &actionSet) 
{
  int numPositions = 0;
  std::map<char, point> dirToVector = { {'U', { 0, 1 }}, {'D', { 0, -1 }}, {'L', { -1, 0 }}, {'R', { 1, 0 }} };
  std::unordered_set<point, pointHash> nthTailPoints;
  std::vector<point> ropePositions;

  point headPos = { 0, 0 };
  point lastMove = { 0, 0 };

  for (int i = 0; i < numSegments; i++)
  {
    ropePositions.push_back({ 0, 0 });
  }

  nthTailPoints.insert({ 0, 0 });

  for (auto& action : actionSet) 
  {
    point directionVec = dirToVector.at(action.dir);

    for (int i = 0; i < action.distance; i++)
    {
      ropePositions[0] = ropePositions[0] + directionVec;

      for (int j = 1; j < numSegments; j++)
      {
        point distance = ropePositions[j - 1] - ropePositions[j];

        if (abs(distance.x) > 1 || abs(distance.y) > 1)
        {
          ropePositions[j].move(distance); // This function uses just the direction of the vector and not the scalar values.
        }  
      }
      nthTailPoints.insert(ropePositions[numSegments - 1]);
      nthTailPoints.insert(ropePositions[numSegments - 1]);
    }
  }

  return nthTailPoints.size();
}

int main (int argc, char** argv)
{
  std::string filename(argv[1]);
  std::vector<Action> actionSet;

  actionSet = readInput(filename);

  int part1 = findPositionsNthTail(2, actionSet);
  int part2 = findPositionsNthTail(10, actionSet);
  std::cout << part1 << std::endl;
  std::cout << part2 << std::endl;
}