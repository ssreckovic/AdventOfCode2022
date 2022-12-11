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
      // return { (x + other.x) / (std::max(x + other.x , 1)) , (y + other.y) / (std::max(y + other.y , 1)) }; 
      return { (x + other.x) , (y + other.y) }; 
    }

    float dist (const point &other) const {
      return sqrtf((other.x - x)*(other.x - x) + (other.y - y)*(other.y - y));
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
  std::vector<point> prevMove;

  point headPos = { 0, 0 };
  point lastMove = { 0, 0 };

  for (int i = 0; i < numSegments; i++)
  {
    prevMove.push_back({ 0, 0 });
    ropePositions.push_back({ 0, 0 });
  }

  nthTailPoints.insert({ 0, 0 });

  for (auto& action : actionSet) 
  {
    point directionVec = dirToVector.at(action.dir);

    // std::cout << action.dir << " :: " << action.distance << std::endl;
    for (int i = 0; i < action.distance; i++)
    {
      // Loop through rope segments
      for (int j = 0; j < numSegments; j++)
      {
        if (j == 0)
        {
          // std::cout << "Action : " << action.dir << " " << i << std::endl;
          // std::cout << "Prev POS :: " << ropePositions[j].x << "," << ropePositions[i].y << "   Direciton: " << directionVec.x << "," << directionVec.y << std::endl;
          // std::cout << directionVec.x + ropePositions[j].x << " " << directionVec.y + ropePositions[j].y << std::endl;
          // point temp = {ropePositions[j].x + directionVec.x ,  ropePositions[j].y + directionVec.y};
          // std::cout << temp.x << "," << temp.y<< std::endl;
          ropePositions[j] = ropePositions[j] + directionVec;
          lastMove = directionVec;
          
          // lastMove = prevMove[j];
          // std::cout << "POS :: " << ropePositions[j].x << "," << ropePositions[j].y << std::endl;
          // std::cout << "H : (" << ropePositions[j].x <<  "," << ropePositions[j].y << ")" << std::endl;
        }

        else if (ropePositions[j].dist(ropePositions[j - 1]) >= 2)
        {
          ropePositions[j] = ropePositions[j] + prevMove[j - 1];
          
          prevMove[j] = prevMove[j - 1];
          // prevMove[j - 1] = lastMove;
          point temp = lastMove;
          lastMove = prevMove[j - 1];
          prevMove[j - 1] = temp;
          // std::cout << "(" << ropePositions[j].x <<  "," << ropePositions[j].y << ")" << std::endl;
          // prevMove[j - 1] = lastMove;
          // lastMove = 
          // lastMove = prevMove[j - 1];
          // prevMove[j] = prevMove[j - 1];

          if (j == numSegments - 1)
          {
            // std::cout << "POS :: " << ropePositions[j].x << "," << ropePositions[j].y << std::endl;
            nthTailPoints.insert(ropePositions[j]);
          }
        }
        else 
        {
          // std::cout << prevMove[j].x << " ," << prevMove[j].y << std::endl;
          // prevMove[j] = prevMove[j] + prevMove[j - 1];
          // std::cout << "(" << ropePositions[j].x <<  "," << ropePositions[j].y << ")" << std::endl;
          prevMove[j - 1] = prevMove[j - 1] + lastMove;
          
          lastMove = { 0, 0 };
        }
  
      }
      // std::cout << " " << std::endl;
    }
  }
  numPositions = nthTailPoints.size();
  return numPositions;
}

// Part 1
int findNumUniqueTailPositions (std::vector<Action> &actionSet)
{
  int numPositions = 0;

  point prevMove = { 0, 0 };
  std::map<char, point> dirToVector = { {'U', { 0, 1 }}, {'D', { 0, -1 }}, {'L', { -1, 0 }}, {'R', { 1, 0 }} };
  std::unordered_set<point, pointHash> tailPoints;

  tailPoints.insert({ 0, 0 });

  point headPos = { 0, 0 };
  point tailPos = { 0, 0 };

  // std::cout << (headPos.x + tailPos.x) / (std::max(headPos.x + tailPos.x , 1)) << ", " << (std::max(headPos.x + tailPos.x , 1)) << std::endl;
  for (auto& action : actionSet)
  {
    point directionVec = dirToVector.at(action.dir);
    // std::cout << directionVec.x << " , " << directionVec.y << std::endl;
    for (int i = 0; i < action.distance; i++)
    {
      headPos = headPos + directionVec;

      // check distance between head and tail, if > 2 move the tail
      if (headPos.dist(tailPos) >= 2)
      {
        tailPos = tailPos + prevMove;
        prevMove = directionVec;

        tailPoints.insert(tailPos);
      }
      else
      {
        // when distance < 2, just keep adding the actions together until 
        // the head moves far away and then update tail position with that new vector
        prevMove = prevMove + directionVec;
      }
      // std::cout << "Head: (" << headPos.x << "," << headPos.y << ") :: Tail: (" << tailPos.x << "," << tailPos.y << ")" << std::endl;
    }
  }

  numPositions = tailPoints.size();
  return numPositions;
}

int main (int argc, char** argv)
{
  std::string filename(argv[1]);
  std::vector<Action> actionSet;

  actionSet = readInput(filename);

  int part1 = findNumUniqueTailPositions(actionSet);
  int part2 = findPositionsNthTail(10, actionSet);
  std::cout << part1 << std::endl;
  std::cout << part2 << std::endl;
}