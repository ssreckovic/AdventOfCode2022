#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>


struct Position
{
  long long x, y;

  Position operator+(const Position &b) const
  {
    return { x + b.x, y + b.y };
  }

  Position operator-(const Position &b) const
  {
    return { x - b.x, y - b.y };
  }

  bool operator==(const Position &b) const
  {
    return ( x == b.x && y == b.y );
  }

  bool operator!=(const Position &b) const
  {
    return (x != b.x || y != b.y);
  }

  bool operator<(const Position &a) const
  {
    return x < a.x || (x == a.x && y < a.y);
  }
};

struct RockShape
{
  Position originPosition;
  std::vector<Position> relativePoints;
  int height;
};

Position leftDiff = {-1,0};
Position rightDiff = {1,0};

struct DroppingBlock
{
  RockShape shape;
  Position origin;

  void pushSideways (int jetDirection, const std::map<Position, char> &rockChamber)
  {
    // std::cout << "Dir : " << jetDirection << " place: L->" << leftBounds << "R->" << rightBounds << std::endl;
    Position dir = jetDirection == -1 ? leftDiff: rightDiff;

    for (auto& blockPoint : shape.relativePoints)
    {
      Position current = origin + blockPoint + dir;

      if (current.x < 0 || current.x > 6 || rockChamber.count( current ) > 0)
      {
        return;
      }
    }
    // std::cout << "Move " << jetDirection << std::endl;
    origin.x += jetDirection;
  }
};

struct RockEpoch
{
  int rockType;
  int jet;

  bool operator==(const RockEpoch &re) const 
  {
    return rockType == re.rockType && jet == re.jet;
  }

  bool operator<(const RockEpoch &re) const
  {
    return rockType < re.rockType || (rockType == re.rockType && jet < re.jet);
  }
};

struct EpochInfo 
{
  long long top;
  long long numRocks;;
  int seenBeforeCount;
};

std::vector<RockShape> buildRockShapes ()
{
  // Four different rock fall patterns
  /* Where '#' is a rock
    s1      s2     s3     s4   s5
    ####    .#.    ..#    #    ##
            ###    ..#    #    ##
            .#.    ###    #
                          #
  */
  std::vector<RockShape> shapes;
  RockShape s1 = { { 2, 0 }, {{ 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 }}, 1 };
  RockShape s2 = { { 3, 0 }, {{ 0, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 }, { 0, 2 }}, 3 }; 
  RockShape s3 = { { 2, 0 }, {{ 0, 0 }, { 1, 0 }, { 2, 0 }, { 2, 1 }, { 2, 2 }}, 3 };
  RockShape s4 = { { 2, 0 }, {{ 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 }}, 4 };
  RockShape s5 = { { 2, 0 }, {{ 0, 0 }, { 0, 1 }, { 1, 1 }, { 1, 0 }}, 2 };

  shapes.push_back(s1);
  shapes.push_back(s2);
  shapes.push_back(s3);
  shapes.push_back(s4);
  shapes.push_back(s5);

  return shapes;
}

void readInput (std::vector<int> &jetDirectionSet, std::string filename)
{
  std::ifstream file(filename);
  std::string line;

  std::getline(file, line);

  for (auto& ch : line)
  {
    if (ch == '>')
    {
      jetDirectionSet.push_back(1);
    }
    else 
    {
      jetDirectionSet.push_back(-1);
    }
  }
}

void printChamber( const std::map<Position, char> &rockChamber, int highestPosition)
{
  for(int i = highestPosition; i >= 0; i--)
  {
    for (int j = 0; j < 7; j++)
    {
      if (rockChamber.count({j, i}) > 0)
      {
        std::cout << '#';
      }
      else 
      {
        std::cout << '.';
      }
    }
    std::cout << std::endl;
  }
}

bool hasCollided (const DroppingBlock &block, const std::map<Position, char> &rockChamber)
{
  if (block.origin.y - 1 < 0)
  {
    return true;
  }

  for (auto& blockPoint : block.shape.relativePoints)
  {
    if (rockChamber.count({ block.origin.x + blockPoint.x, block.origin.y + blockPoint.y - 1 }) > 0)
    {
      return true;
    }
  }

  return false;
}

void placeRock (DroppingBlock &block, std::map<Position, char> &rockChamber)
{
  for (auto& blockPoint : block.shape.relativePoints)
  {
    rockChamber[{block.origin.x + blockPoint.x, block.origin.y + blockPoint.y}] = '#';
  }
}

void simulateRockfall (const std::vector<int> &jetDirectionSet, std::vector<RockShape> &rockShapes, int numRocks)
{
  std::map<Position, char> rockChamber;

  long long highestPosition = 0;
  int jet = 0;

  for (int i = 0; i < numRocks; i++)
  {
    RockShape shape = rockShapes[i % rockShapes.size()];
    DroppingBlock block = { shape, { shape.originPosition.x, highestPosition + 3 } };
    
    bool atRest = false;
    while (!atRest)
    {
      block.pushSideways(jetDirectionSet[jet], rockChamber);
      jet = (jet + 1) % jetDirectionSet.size();

      if (highestPosition < block.origin.y)
      {
        block.origin.y--;
        continue;
      }

      if (hasCollided(block, rockChamber))
      {
        placeRock(block, rockChamber);
        atRest = true;
      }
      else
      {
        block.origin.y--;
      }
    }

    highestPosition = std::max(highestPosition, block.origin.y + block.shape.height);
  }
  
  // printChamber(rockChamber, highestPosition);
  std::cout << "Highest Position: " << highestPosition;
}

void hugeNumberRockFall (const std::vector<int> &jetDirectionSet, std::vector<RockShape> &rockShapes, long long hugeNumRocks)
{
  std::map<Position, char> rockChamber;
  std::map<RockEpoch, EpochInfo> seen;
  bool cycleNotFound = true;
  long long absoluteTop = 0; // Used to store the value after the cycle is found, top is then added to this at the end.
  long long top = 0;
  int jet = 0;
  int iterations = 0;
  long long numRocks = 0;
  
  while (numRocks < hugeNumRocks)
  {
    int rockIdx = numRocks % rockShapes.size();
    DroppingBlock block = { rockShapes[rockIdx], { rockShapes[rockIdx].originPosition.x, top + 3 } };

    bool atRest = false;
    while (!atRest)
    {
      block.pushSideways(jetDirectionSet[jet], rockChamber);
      jet = (jet + 1) % jetDirectionSet.size();

      if (top < block.origin.y)
      {
        block.origin.y--;
        continue;
      }

      if (hasCollided(block, rockChamber))
      {
        placeRock(block, rockChamber);
        atRest = true;
      }
      else
      {
        block.origin.y--;
      }  
    }

    top = std::max(top, block.origin.y + block.shape.height);

    if (cycleNotFound)
    {
      // checking for cycles
      if (seen.count({ rockIdx, jet }) == 0)
      {
        seen.insert({ { rockIdx, jet }, { top, numRocks, 1 }});
      }
      else
      {
        if (seen[{ rockIdx, jet }].seenBeforeCount == 2)
        {
          long long deltaRocks = numRocks - seen[{ rockIdx, jet }].numRocks;
          long long deltaTop = top - seen[{ rockIdx, jet }].top;
          long long numRepeats = (hugeNumRocks - numRocks) / deltaRocks;
          numRocks += numRepeats * deltaRocks;
          // absoluteTop is used instead of top so that we don't have to worry about updating any map values
          // we can just continue as if top is the same as before, and run the last few iterations, then add top and abosoluteTop 
          // at the very end.
          absoluteTop = numRepeats * deltaTop;

          cycleNotFound = false;
        }
        seen[{ rockIdx, jet }].seenBeforeCount++;
        seen[{ rockIdx, jet }].top = top;
        seen[{ rockIdx, jet }].numRocks = numRocks;
      }
    }
    
    numRocks++;
    iterations++;
  }
  absoluteTop += top;

  std::cout << "Iterations: " << iterations << std::endl;
  std::cout << "Height of the huge tower of rocks is: " << absoluteTop + top << std::endl;
}

int main (int argc, char** argv)
{
  std::string filename(argv[1]);
  std::vector<int> jetDirectionSet;
  std::vector<RockShape> rockShapes = buildRockShapes();
  int numRocksToDrop = argc == 3 ? std::atoi(argv[2]) : 2022;
  long long bigRocks = 1000000000000;
  readInput(jetDirectionSet, filename);

  // simulateRockfall(jetDirectionSet, rockShapes, numRocksToDrop);
  hugeNumberRockFall(jetDirectionSet, rockShapes, bigRocks);
}