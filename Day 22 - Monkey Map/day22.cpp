#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>

struct MapLine
{
  int startIndex;
  int endIndex;
  int lineLength;
};
struct Instruction
{
  int distance;
  char rotation;
};

struct Point 
{
  int x, y;

  Point operator+(const Point a)
  {
    return { x + a.x, y + a.y };
  }
  Point operator-(const Point a)
  {
    return { x - a.x, y - a.y };
  }
};
struct CubeEdge
{
  
};

struct CubeFace 
{
  int topIdx;
  int leftIdx;
  int rightIdx;
  int bottomIdx;
};


void parseInstructions (std::string instructionLine, std::vector<Instruction> &instructionSet)
{
  int idxStart = 0;
  int len = 0;
  char dir = 'F';

  for (int i = 0; i < instructionLine.size(); i++)
  {
    if (instructionLine[i] == 'R' || instructionLine[i] == 'L')
    {
      instructionSet.push_back({ std::stoi(instructionLine.substr(idxStart, len)), dir });
      idxStart = i + 1;
      len = 0;
      dir = instructionLine[i];
    }
    else 
    {
      len++;
    }
  }
  
  instructionSet.push_back({ std::stoi(instructionLine.substr(idxStart, len)), dir});
}

void readInput (std::vector<std::vector<char>> &monkeyMap, std::vector<MapLine> &lineInfo, 
  std::vector<Instruction> &instructionSet, const std::string &filename)
{
  std::ifstream file(filename);
  std::string line;
  int lineCount = 0;

  while (std::getline(file, line))
  {

    if (std::isdigit(line[0]))
    {
      parseInstructions(line, instructionSet);
      break; 
    }

    MapLine ml = { -1, -1, 0 };
    ml.endIndex = line.size() - 1;
    std::vector<char> mapLine;

    for (int i = 0; i < line.size(); i++)
    {
      mapLine.push_back(line[i]);

      if (ml.startIndex == -1 && line[i] != ' ')
      {
        ml.startIndex = i;
      }

      if (ml.startIndex != -1)
      {
        ml.lineLength++;
      }
    }

    if (ml.startIndex != -1)
    {
      lineInfo.push_back(ml);
      monkeyMap.push_back(mapLine);
    }
  }
}

Point checkForWraparound (Point &nextPosition, Point &direction, const std::vector<MapLine> &lineInfo, const std::vector<std::vector<char>> &monkeyMap)
{
  Point resolvedPosition = nextPosition;

  if (direction.y == 0)
  {
    // wrap around in x direction
    if (nextPosition.x > lineInfo[nextPosition.y].endIndex)
    {
      resolvedPosition.x = lineInfo[nextPosition.y].startIndex;
    }
    else if (nextPosition.x < lineInfo[nextPosition.y].startIndex)
    {
      resolvedPosition.x = lineInfo[nextPosition.y].endIndex;
    }
  }      
  else if (direction.x == 0)
  {
    // Moving took us out of bounds of the map, or out of bounds in the local area
    if (nextPosition.y < 0 || nextPosition.y > monkeyMap.size() || (nextPosition.x > lineInfo[nextPosition.y].endIndex || nextPosition.x < lineInfo[nextPosition.y].startIndex))
    {
      // move in opposite direction until you run into an edge, that point is where the wrap around occurs
      // unless that point is a wall, in which case you stay at the last point before the wrap around
      Point tempPos = nextPosition - direction;
      while (tempPos.y >= 0 && tempPos.x >= lineInfo[tempPos.y].startIndex && tempPos.x <= lineInfo[tempPos.y].endIndex)
      {
        tempPos = tempPos - direction;
      }

      tempPos = tempPos + direction;
      if (monkeyMap[tempPos.y][tempPos.x] != '#')
      {
        resolvedPosition = tempPos;
      }
      else
      {
        resolvedPosition = nextPosition - direction;
      }
      
    }
  }

  return resolvedPosition;
}

int traverseMap(std::vector<std::vector<char>> &monkeyMap, const std::vector<MapLine> &lineInfo, const std::vector<Instruction> &instructionSet)
{
  std::vector<Point> directions = { { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 }}; // Right, Down, Left, Up
  std::vector<char> dirChar = { '>', 'v', '<', '^' };
  Point currentPos = { lineInfo[0].startIndex, 0 };
  int dirIndex = 0;

  for (auto& instruction : instructionSet)
  {
    int count = 0;

    // Use direction input to change the direction vector used.
    if (instruction.rotation == 'R') { dirIndex = (dirIndex + 1) % 4; }
    else if (instruction.rotation == 'L') { dirIndex = (dirIndex - 1 + 4) % 4; }


    while (count < instruction.distance)
    {
      
      Point nextPosition = currentPos + directions[dirIndex];
      nextPosition = checkForWraparound(nextPosition, directions[dirIndex], lineInfo, monkeyMap);
      

      char nextPositionVal = monkeyMap[nextPosition.y][nextPosition.x];
      
      if (nextPositionVal == '#')
      {
        break;
      }

      if (nextPositionVal != ' ' || nextPositionVal != '#') // could be . < > v ^
      {
        // can move to next position
        monkeyMap[nextPosition.y][nextPosition.x] = dirChar[dirIndex];
        currentPos = nextPosition;
      }

      count++;
    }
  }

  int finalVal = 1000 * (currentPos.y + 1) + 4 * (currentPos.x + 1) +  dirIndex;
  return finalVal;
}

// Transform flat map to a series of cube faces
std::vector<std::vector<std::vector<char>>> buildCube (std::vector<std::vector<char>> &monkeyMap, std::vector<MapLine> &lineInfo)
{
  std::vector<std::vector<std::vector<char>>> cubeMap;

  return cubeMap;
}


int traverseCubeMap(std::vector<std::vector<char>> &monkeyMap, const std::vector<MapLine> &lineInfo, const std::vector<Instruction> &instructionSet)
{
  std::vector<Point> directions = { { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 }}; // Right, Down, Left, Up
  std::vector<char> dirChar = { '>', 'v', '<', '^' };
  Point currentPos = { lineInfo[0].startIndex, 0 };
  int dirIndex = 0;

  for(auto& line : lineInfo)
  {
    std::cout << line.startIndex << " to " << line.endIndex << " dist: " << line.lineLength << std::endl;
  }
  // std::vector<std::vector<std::vector<char>> monkeyCube = buildCube(monkeyMap, lineInfo);
  return 0;
}

int main (int argc, char** argv)
{
  std::string filename(argv[1]);
  std::vector<std::vector<char>> monkeyMap;
  std::vector<MapLine> lineInfo;
  std::vector<Instruction> instructionSet;

  readInput(monkeyMap, lineInfo, instructionSet, filename); 
  
  // int secretPassword = traverseMap(monkeyMap, lineInfo, instructionSet);
  // std::cout << "Password: " << secretPassword << std::endl;

  int secondSecretPassword = traverseCubeMap(monkeyMap, lineInfo, instructionSet);
  std::cout << "Password: " << secondSecretPassword << std::endl;
  // for (int i = 0; i < 10; i++)
  // {
  //   // std::cout << lineInfo[i].startIndex << " to " << lineInfo[i].endIndex << " len " << lineInfo[i].lineLength << "   ";
  //   for (auto& ch : monkeyMap[i])
  //   {
  //     std::cout << ch;
  //   }

  //   std::cout << std::endl;
  // }

  // for (auto & inst : instructionSet)
  // {
  //   std::cout << inst.distance << "->" << inst.rotation << std::endl;
  // }
}