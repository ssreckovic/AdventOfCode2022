#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>

struct Position
{
  int x, y;

  Position operator+(const Position &a) const
  {
    return { x + a.x, y + a.y };
  }

  bool operator<(const Position &a) const
  {
    return x < a.x || (x == a.x && y < a.y);
  }

  bool operator==(const Position &a) const
  {
    return x == a.x && y == a.y;
  }
};

struct Elf 
{
  Position position;
  Position prevPosition;  
  Position proposedDirection;
};

void readInput (std::vector<Elf> &elves, std::map<Position, int> &positionMap, const std::string &filename)
{
  std::ifstream file(filename);
  std::string line;
  int lineNum = 0;
  while (std::getline(file, line))
  {
    for (int i = 0; i < line.size(); i++)
    {
      if (line[i] == '#')
      {
        Position pos = { i , lineNum };
        Elf elf = { pos, pos, { 0, 0 } };
        elves.push_back(elf);
        positionMap.insert({ pos, elves.size() - 1 });
      }
    }

    lineNum++;
  }
}

// Hard coding in the values that must be checked before an elf is allowed to move in that direction
void buildDirectionMap(const std::vector<Position> movementDirs, std::map<Position, std::vector<Position>> &directionMap, const std::vector<Position> &allDirections)
{
  std::vector<Position> checkingDirs = { { -1, -1 }, { 0, -1 }, { 1, -1 }, 
                                         { -1, 0 },             { 1, 0 },
                                         { -1, 1 },  { 0, 1 },  { 1, 1 } };
  directionMap[movementDirs[0]] = { allDirections[0], allDirections[1], allDirections[2] }; // North
  directionMap[movementDirs[1]] = { allDirections[5], allDirections[6], allDirections[7] }; // South
  directionMap[movementDirs[2]] = { allDirections[0], allDirections[3], allDirections[5] }; // West
  directionMap[movementDirs[3]] = { allDirections[2], allDirections[4], allDirections[7] }; // East

}

// Check an elfs surroundings, return whether any elves are found in that area.
bool hasAdjacentElves(const Elf& elf, const std::map<Position, int> &positionMap, const std::vector<Position> &checkingDirs)
{
  for (auto& dir : checkingDirs)
  {
    if (positionMap.count(elf.position + dir) > 0 && positionMap.at({elf.position + dir}) != -1)
    {
      return true;
    }
  }

  return false;
}

// Determine which direction to move the elf based on its surroundings
Position getProposedDirection (const Elf& elf, const std::map<Position, int> &positionMap, const std::map<Position, std::vector<Position>> &directionMap, const std::vector<Position> &movementDirs, int dirStart)
{
  bool hasConflict;

  for (int i = 0; i < movementDirs.size(); i++)
  { 
    hasConflict = false;
    int idx = (i + dirStart) % movementDirs.size();
    std::vector<Position> dirChecks = directionMap.at({movementDirs[idx]});

    for (auto& dir : dirChecks)
    {
      Position p = elf.position + dir;

      if (positionMap.count(p) != 0 && positionMap.at({p}) != -1) 
      {
        hasConflict = true;
        break;
      }
    }

    if (!hasConflict)
    {
      return movementDirs[idx];
    }
  }

  return { 0 , 0 };
}

// Given a vector of indicies, move the elves at those indicies back to their previous positions
void revertElves (std::vector<int> indiciesToRevert, std::vector<Elf> &elves, std::map<Position, int> &positionMap)
{
  for (auto& index : indiciesToRevert)
  {
    // There will likely be duplicates of certain indicies, so if we get one ignore it
    if (positionMap.at((elves[index].position)) == -1)
    {
      continue;
    }
    
    positionMap[elves[index].position] = -1;
    positionMap[elves[index].prevPosition] = index;
    elves[index].position = elves[index].prevPosition;
  }
}

// Run the process of moving the elves based on their proposed direction, and fixing and cases where multiple elves 
// would be moving to a single position.
void moveElves (std::vector<Elf> &elves, std::vector<int> movingElves, std::map<Position, int> &positionMap)
{
  std::vector<int> resetPositions;

  for (auto& elfIdx : movingElves)
  {
    if (elves[elfIdx].proposedDirection.x == 0 && elves[elfIdx].proposedDirection.y == 0)
    {
      continue;
    }

    elves[elfIdx].prevPosition = elves[elfIdx].position;
    Position nextPos = elves[elfIdx].position + elves[elfIdx].proposedDirection;
    elves[elfIdx].proposedDirection = { 0, 0 };

    // Move the elf to the next position
    if (positionMap.count(nextPos) <= 0 || positionMap.at({nextPos}) == -1)
    {
      elves[elfIdx].position = nextPos;
      elves[elfIdx].proposedDirection = { 0, 0 };
      positionMap[elves[elfIdx].prevPosition] = -1;
      positionMap[elves[elfIdx].position] = elfIdx;
    }
    else
    {
      // There is already a value set at this position, so everyone must move back to the previous position
      // The index stored in the map will be used to reset the position of the initial moved elf after
      // all the other elves have moved.
      resetPositions.push_back(positionMap.at(nextPos));
    }
  }

  revertElves(resetPositions, elves, positionMap);
}

// Given a vector of elves, determine the minimum dimensions of the square that would contain all of their positions
Position findMinSquare (const std::vector<Elf> &elves)
{
  int minX = 10000000, maxX = 0;
  int minY = 10000000, maxY = 0;

  for (auto& elf : elves)
  {
    if (elf.position.x < minX)
    {
      minX = elf.position.x;
    }
    else if (elf.position.x > maxX)
    {
      maxX = elf.position.x;
    }

    if (elf.position.y < minY)
    {
      minY = elf.position.y;
    }
    else if (elf.position.y > maxY)
    {
      maxY = elf.position.y;
    }
  }

  return { maxX - minX + 1, maxY - minY + 1};
}

int partOne (std::vector<Elf> &elves, std::map<Position, int> positionMap, int numRounds)
{
  // Manually set all the direction "vectors" 
  std::vector<Position> checkingDirs = { { -1, -1 }, { 0, -1 }, { 1, -1 }, 
                                         { -1, 0 },             { 1, 0 },
                                         { -1, 1 },  { 0, 1 },  { 1, 1 } };
  std::vector<Position> movementDirs = { { 0, -1 }, { 0, 1 }, { -1, 0 }, { 1, 0 } }; // North, South, East, West
  std::map<Position, std::vector<Position>> directionMap;
  buildDirectionMap(movementDirs, directionMap, checkingDirs);

  int movementDirStart = 0;
  
  for (int i = 0; i < numRounds; i++)
  {
    std::vector<int> movingElves;

    // First half of the round
    for (int j = 0; j < elves.size(); j++)
    {
      if (!hasAdjacentElves(elves[j], positionMap, checkingDirs))
      {
        continue;
      }

      // Make a list of the indicies of all the elves that will be moving
      movingElves.push_back(j);
      elves[j].proposedDirection = getProposedDirection(elves[j], positionMap, directionMap, movementDirs, movementDirStart);
    }

    // Second half of the round
    moveElves(elves, movingElves, positionMap);
    movementDirStart++;
  }
  

  Position minCoverageDimensions = findMinSquare(elves);
  std::cout << "min dim: " << minCoverageDimensions.x << "," << minCoverageDimensions.y << std::endl;

  return (minCoverageDimensions.x * minCoverageDimensions.y) - elves.size();
  
}

int partTwo (std::vector<Elf> &elves, std::map<Position, int> &positionMap)
{
  std::vector<Position> checkingDirs = { { -1, -1 }, { 0, -1 }, { 1, -1 }, 
                                         { -1, 0 },             { 1, 0 },
                                         { -1, 1 },  { 0, 1 },  { 1, 1 } };
  std::vector<Position> movementDirs = { { 0, -1 }, { 0, 1 }, { -1, 0 }, { 1, 0 } }; // North, South, East, West
  std::map<Position, std::vector<Position>> directionMap;
  buildDirectionMap(movementDirs, directionMap, checkingDirs);

  int movementDirStart = 0;
  int roundNum = 1;

  while (roundNum < 10000)
  {
    std::vector<int> movingElves;
  
    // First half of the round
    for (int j = 0; j < elves.size(); j++)
    {
      if (!hasAdjacentElves(elves[j], positionMap, checkingDirs))
      {
        continue;
      }

      elves[j].proposedDirection = getProposedDirection(elves[j], positionMap, directionMap, movementDirs, movementDirStart);

      if (elves[j].proposedDirection.x != 0 || elves[j].proposedDirection.y != 0)
      {
        movingElves.push_back(j);
      }
    }

    // No Elves are moving, so stop!
    if (movingElves.size() == 0)
    {
      break;
    }
    moveElves(elves, movingElves, positionMap);
    movementDirStart++;
    roundNum++;
  }
  
  return roundNum;
  
}

int main (int argc, char** argv)
{
  std::string filename(argv[1]);
  std::vector<Elf> elves;
  std::map<Position, int> positionMap;

  readInput(elves, positionMap, filename);

  int numRounds = 10;
  // int finalValue = partOne(elves, positionMap, numRounds);
  // std::cout << "Final Value: " << finalValue << std::endl;

  int numberOfRoundsRun = partTwo(elves, positionMap);
  std::cout << "Number of rounds until zero: " << numberOfRoundsRun;
  
  return 0;
}