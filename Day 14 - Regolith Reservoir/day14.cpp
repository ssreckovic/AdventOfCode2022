#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

const int INDEX_REDUCTION_FACTOR = 490; // Given coordinates are of the range 490 - 580ish. This is used to reduce that to 1-90
const int INFINITE_OFFSET = 5000;

// Full size example -> for the moment hard coded
const int CAVE_HEIGHT_LARGE = 250; // 'Infinite Height'
const int CAVE_WIDTH_LARGE = 100;
// Test example size
const int CAVE_HEIGHT_SMALL = 50; // 'Infinite Height'
const int CAVE_WIDTH_SMALL = 20;


struct Point 
{
  int x, y;

  Point operator-(const Point &rhs) const
  {
    return { x - rhs.x, y - rhs.y };
  }

  Point operator+(const Point &rhs) const
  {
    return { x + rhs.x, y + rhs.y };
  }

  void operator+=(const Point &rhs)
  {
    x += rhs.x;
    y += rhs.y;
  }

  bool operator!=(const Point &rhs) const
  {
    return ( x != rhs.x ) || ( y != rhs.y );
  }

  Point unitVector()
  {
    int xDiv = abs(x);
    int yDiv = abs(y);

    if (xDiv == 0) { xDiv = 1; }
    if (yDiv == 0) { yDiv = 1; }

    return { x / xDiv, y / yDiv };
  }
};

// Extract all sets of points from the given string, return a vector of those points.
std::vector<Point> parseRockPoints (const std::string &line)
{
  std::vector<Point> rockPoints;
  std::stringstream ss(line);
  std::string word;

  while(std::getline(ss, word, ' '))
  {
    if (word != "->")
    {
      std::stringstream ws(word);
      std::string xCoord;
      std::string yCoord;

      std::getline(ws, xCoord, ',');
      std::getline(ws, yCoord, ',');

      rockPoints.push_back({ std::stoi(xCoord), std::stoi(yCoord) });
    }
  }

  return rockPoints;
}

// Given a set of points draw 'lines' of rock ('#') between those two points on the given matrix map of the cave.
void drawRockLine(std::vector<std::vector<char>> &caveMap, const std::vector<Point> &rockPoints, int &maxHeight, const int partNum)
{
  // Offset needed for part 2, to put the structure of lines
  // in the center of the 'Infinite' offset
  int offset = 0;
  if (partNum == 2)
  {
    offset = INFINITE_OFFSET;
  }

  for (int i = 0; i < rockPoints.size() - 1; i++)
  {
    Point rockVector = rockPoints[i + 1] - rockPoints[i];
    Point direction = rockVector.unitVector();
    Point posCount = { 0, 0 };

    // Find the maximum height of a point on this line.
    // Used to determine to global maximum of height in the cave.
    if (rockPoints[i].y > maxHeight)
    {
      maxHeight = rockPoints[i].y;
    }
    else if (rockPoints[i + 1].y > maxHeight)
    {
      maxHeight = rockPoints[i + 1].y;
    }

    // Loop between the two given points and change all points to be rock ('#') instead of air.
    while (posCount != (rockVector + direction))
    {
      Point pt = rockPoints[i] + posCount;
      caveMap[pt.y][(pt.x % INDEX_REDUCTION_FACTOR) + offset] = '#';

      posCount += direction;
    }
  }
}

// Draw a horizontal line of rock ('#') across the given row index.
void drawHorizontalLine (std::vector<std::vector<char>> &caveMap, const int rowIndex)
{
  for (int i = 0; i < caveMap[rowIndex].size(); i++)
  {
    caveMap[rowIndex][i] = '#';
  }
}

// Read from the file. Create a map based on that input.
void readInput (std::vector<std::vector<char>> &caveMap, const std::string &filename, const int partNum)
{
  std::ifstream file(filename);
  std::string line;
  int globalMax = 0;
  while (std::getline(file, line))
  {
    int localMax = 0;
    std::vector<Point> rockPoints = parseRockPoints(line);
    drawRockLine(caveMap, rockPoints, localMax, partNum);

    if (localMax > globalMax) 
    {
      globalMax = localMax;
    }
  }

  // Draw the 'infinite' line that is used in part 2.
  if (partNum == 2)
  {
    drawHorizontalLine(caveMap, globalMax + 2);
  }
}

// Print a given matrix of chars.
void printMap (std::vector<std::vector<char>> &caveMap)
{
  for(auto& line : caveMap)
  {
    for (auto& sq : line)
    {
      std::cout << sq;
    }
    std::cout << std::endl;
  }
}

// Simulate dropping sand through our given cavemap
void runSand (std::vector<std::vector<char>> &caveMap, Point &origin)
{
  int stableSandCount = 0;
  bool hitVoid = false;

  while (!hitVoid)
  {
    // Drop one sand
    Point sand = origin;

    while (sand.y < caveMap.size() - 1)
    {
      if (caveMap[sand.y + 1][sand.x] == '.')
      {
        sand.y++;
      }
      else if (caveMap[sand.y + 1][sand.x - 1] == '.')
      {
        sand.y++;
        sand.x--;
      }
      else if (caveMap[sand.y + 1][sand.x + 1] == '.')
      {
        sand.y++;
        sand.x++;
      }
      else
      {
        caveMap[sand.y][sand.x] = 'o';
        stableSandCount++;
        break;
      }
    }

    if (sand.y == 0)
    {
      std::cout << "DONE -> we built to top! ";
      hitVoid = true;
    }
    if (sand.y >= caveMap.size() - 1)
    {
      std::cout << "DONE -> we hit the void: " << sand.x << "," << sand.y << std::endl;
      hitVoid = true;
    }

  } 

  std::cout << "Stable sand at end: " << stableSandCount << std::endl;
}

int main (int argc, char** argv)
{
  std::string filename(argv[1]);
  Point sandOrigin = { 500 % INDEX_REDUCTION_FACTOR, 0 };
  int width, height;
  int additionalWidth = 0;

  // Change this number to run as part 1 or 2.
  int partNum = 1;

  if (filename == "input.txt")
  {
    width = CAVE_WIDTH_LARGE;
    height = CAVE_HEIGHT_LARGE;
  }
  else
  {
    width = CAVE_WIDTH_SMALL;
    height = CAVE_HEIGHT_SMALL;
  }

  if (partNum == 2)
  {
    // used to make the width of the ground 2*100000 + CAVE_WIDTH_
    additionalWidth = INFINITE_OFFSET;
  }

  sandOrigin.x += additionalWidth;
  width += 2 * additionalWidth;

  std::vector<std::vector<char>> caveMap(height, std::vector<char>(width, '.'));
  caveMap[sandOrigin.y][sandOrigin.x] = '+';

  readInput(caveMap, filename, partNum);
  runSand(caveMap, sandOrigin);


  printMap(caveMap);
}