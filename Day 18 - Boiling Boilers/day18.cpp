#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>

const int MATRIX_SIZE = 21;

struct ThreeDPoint
{
  int x, y, z;

  ThreeDPoint operator+(const ThreeDPoint &p) const 
  {
    return { x + p.x, y + p.y, z + p.z };
  }
};

struct Cube
{
  ThreeDPoint pos;
};

struct Space
{
  bool isEmpty;
  bool visited;
  bool isExternal;
};

const std::vector<ThreeDPoint> DIRECTION_VECTORS = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};

void readInput(std::vector<Cube> &cubes, const std::string &filename)
{
  std::ifstream file(filename);
  std::string line;

  while (std::getline(file, line))
  {
    Cube cube;
    std::stringstream ss (line);
    std::string coord;

    std::getline(ss, coord, ',');
    cube.pos.x = std::stoi(coord);
    std::getline(ss, coord, ',');
    cube.pos.y = std::stoi(coord);
    std::getline(ss, coord, ',');
    cube.pos.z = std::stoi(coord);

    cubes.push_back(cube);
  }
}

std::vector<std::vector<std::vector<int>>> buildThreeDMatrix (std::vector<Cube> &cubes)
{
  std::vector<std::vector<std::vector<int>>> matrix (MATRIX_SIZE, std::vector<std::vector<int>>(MATRIX_SIZE, std::vector<int>(MATRIX_SIZE, 0)));
  try
  {
    for (auto& cube : cubes)
    {
      matrix.at(cube.pos.x).at(cube.pos.y).at(cube.pos.z) = 1;
    }  
  }
  catch(const std::out_of_range& e)
  {
    std::cerr << "Matrix Creation out of range: " << e.what() << '\n';
    exit(1);
  }

  return matrix;
}

std::vector<std::vector<std::vector<Space>>> buildSpaceMatrix(const std::vector<Cube> &cubes, const std::vector<std::vector<std::vector<int>>> &cubeMatrix)
{
  std::vector<std::vector<std::vector<Space>>> spaceMatrix (
    MATRIX_SIZE, std::vector<std::vector<Space>>(MATRIX_SIZE, std::vector<Space>(MATRIX_SIZE, { true, false, false })));

  try
  {
    for (auto& cube : cubes)
    {
      if(cubeMatrix.at(cube.pos.x).at(cube.pos.y).at(cube.pos.z) == 1)
      {
        spaceMatrix.at(cube.pos.x).at(cube.pos.y).at(cube.pos.z).isEmpty = false;
      }
    }  
  }
  catch(const std::out_of_range& e)
  {
    std::cerr << "Space Matrix Creation out of range: " << e.what() << '\n';
    exit(1);
  }

  return spaceMatrix;
}

// Count the open faces of a given cube
int countSurfaceFaces (const Cube &cube, const std::vector<std::vector<std::vector<int>>> &matrix)
{
  int count = 0;

  try
  {
    for (auto& dir : DIRECTION_VECTORS)
    {
      ThreeDPoint current = cube.pos + dir;
      if (current.x < 0 || current.y < 0 || current.z < 0)
      {
        count++;
        continue;
      }

      if (matrix.at(current.x).at(current.y).at(current.z) == 0)
      {
        count++;
      }
    } 
  }
  catch(const std::out_of_range& e)
  {
    std::cerr << "Face Count out of range: " << e.what() << '\n';
    exit(1);
  }

  return count;
}

// Part 1: Find the surface area of all open cube faces
void findSurfaces (std::vector<Cube> &cubes)
{
  std::vector<std::vector<std::vector<int>>> cubeMatrix = buildThreeDMatrix(cubes);
  int faceCount = 0;

  for (auto& cube : cubes)
  {
    faceCount += countSurfaceFaces(cube, cubeMatrix);
  }

  std::cout << "Number of open faces = " << faceCount << std::endl;
}

// Return whether a given 3D position is on the edge of a 3D matrix.
bool onMatrixEdge (const ThreeDPoint &pos, ThreeDPoint matrixMaximums)
{
  return (pos.x < 0 || pos.x >= matrixMaximums.x) || (pos.y < 0 || pos.y >= matrixMaximums.y) || (pos.z < 0 || pos.z >= matrixMaximums.z);
}

// Check is a given position is external or internal to the lava block. If the position has not been visited yet
// flood the open space until you find and edge OR there are no more spots to be visited. Returning true if an edge is found.
bool isExternalSpace (const ThreeDPoint &pos, std::vector<std::vector<std::vector<Space>>> &spaceMatrix)
{
  if (spaceMatrix[pos.x][pos.y][pos.z].visited)
  {
    return spaceMatrix[pos.x][pos.y][pos.z].isExternal;
  }
  
  bool foundExternal = false;
  std::queue<ThreeDPoint> sq;
  std::queue<ThreeDPoint> visitedPoints;
  sq.push(pos);

  try
  {
    while(!sq.empty())
    {
      ThreeDPoint tdp = sq.front();
      sq.pop();
      spaceMatrix[tdp.x][tdp.y][tdp.z].visited = true;

      for (auto& dir : DIRECTION_VECTORS)
      {
        ThreeDPoint next = tdp + dir;
        // Check for edges
        if (onMatrixEdge(next, { (int)spaceMatrix.size(), (int)spaceMatrix[0].size(), (int)spaceMatrix[0][0].size() }))
        {
          foundExternal = true;
          continue;
        }

        // Check if the spot we are looking is a block of obsidean or has been visited already.
        if (!spaceMatrix.at(next.x).at(next.y).at(next.z).isEmpty || spaceMatrix.at(next.x).at(next.y).at(next.z).visited)
        {
          continue;
        }

        // Push to the queue and mark visited reguardless of whether we have found an edge or not
        // this is so we can propegate through all of the connected empty space.
        sq.push(next);
        spaceMatrix.at(next.x).at(next.y).at(next.z).visited = true;

        if (foundExternal)
        {
          spaceMatrix.at(next.x).at(next.y).at(next.z).isExternal = true;
        }
        else
        {
          visitedPoints.push(next);
        }
      }
    }

    if (foundExternal)
    {
      // Set all the points we visited before finding the edge to 'true' because they are external too
      while (!visitedPoints.empty())
      {
        ThreeDPoint vp = visitedPoints.front();
        visitedPoints.pop();
        spaceMatrix.at(vp.x).at(vp.y).at(vp.z).isExternal = true;
      }
    }
  }
  catch (const std::out_of_range &oor)
  {
    std::cerr << "External Space out of range: " << oor.what() << std::endl;
  }

  return foundExternal;
}

// Given a cube, count and return the open external faces of the cube 
int countExternalSurfaceFaces(const Cube &cube, const std::vector<std::vector<std::vector<int>>> &matrix, std::vector<std::vector<std::vector<Space>>> &spaceMatrix)
{
  int count = 0;

  try
  {
    for (auto& dir : DIRECTION_VECTORS)
    {
      
      ThreeDPoint current = cube.pos + dir;
      if (onMatrixEdge(current, { (int)spaceMatrix.size(), (int)spaceMatrix[0].size(), (int)spaceMatrix[0][0].size() }))
      {
        count++;
        continue;
      }
      Space sp = spaceMatrix.at(current.x).at(current.y).at(current.z);

      if (sp.isEmpty && sp.isExternal)
      {
        count++;
        continue;
      }

      if (sp.isEmpty && isExternalSpace(current, spaceMatrix))
      {
        count++;
      }
    } 
  }
  catch(const std::out_of_range& e)
  {
    std::cerr << " New Face Count out of range: " << e.what() << '\n';
    exit(1);
  }

  return count;
}

// Part 2: Find the 'surface area' of only the open external faces (not internal as well)
void findExternalSurfaces (std::vector<Cube> &cubes)
{
  std::vector<std::vector<std::vector<int>>> cubeMatrix = buildThreeDMatrix(cubes);
  std::vector<std::vector<std::vector<Space>>> spaceMatrix = buildSpaceMatrix(cubes, cubeMatrix);
  int faceCount = 0;

  for (auto& cube : cubes)
  {
    faceCount += countExternalSurfaceFaces(cube, cubeMatrix, spaceMatrix);
  }
  std::cout << "Number of external open faces = " << faceCount << std::endl;

}

int main (int argc, char** argv)
{
  std::string filename (argv[1]);
  std::vector<Cube> cubes;

  readInput(cubes, filename);

  // Part 1
  // findSurfaces(cubes);
  // Part 2
  findExternalSurfaces(cubes);
}