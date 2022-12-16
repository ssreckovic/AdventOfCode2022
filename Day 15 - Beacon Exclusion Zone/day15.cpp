#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_set>

struct Point 
{
  int x, y;

  bool operator==(const Point &rhs) const{
      return (x == rhs.x) && (y == rhs.y);
  }

  int dist (Point &p)
  {
    return abs(x - p.x) + abs(y - p.y);
  }
};

struct Sensor
{
  Point position;
  Point nearestBeacon;
  int beaconDistance;
  int yDistToMysteryRow;
};

struct MinMaxPair
{
  int min, max;
};

struct pointHash 
{
public:
	size_t operator()(const Point p) const 
  {
		return std::hash<int>()(p.x) ^ std::hash<int>()(p.y);
	}
};

void printSensor (Sensor &sensor)
{
  std::cout << "Sensor @ (" << sensor.position.x << "," << sensor.position.y << ") : ";
  std::cout << "Beacon @ (" << sensor.nearestBeacon.x << "," << sensor.nearestBeacon.y << ") : ";
  std::cout << "Distance to Beacon: " << sensor.beaconDistance << " :: ";
  std::cout << "Intersecting Distance to row is " << sensor.yDistToMysteryRow << std::endl;
}

void printMatrix(std::vector<std::vector<char>> &matrix)
{
  for (int i = 0; i < matrix.size(); i++)
  { 
    if (i < 10) { std::cout << i << "   "; }
    else if (i < 100) { std::cout << i << "  "; }
    else if (i < 1000) { std::cout << i << " "; }
    
    for (auto &ch : matrix[i])
    {
      std::cout << ch;
    }
    std::cout << std::endl;
  }
}

Sensor parseSensorInput (std::string &inputLine, int &minX, int &maxX)
{
  std::stringstream ss(inputLine);
  std::string word;

  Sensor sensor;
  bool isSensorPoint = true;

  while (std::getline(ss, word, ' '))
  {
    if (word[0] == 'x')
    {
      word.pop_back();
      int x = std::stoi(word.substr(2, word.size() - 2));
      std::getline(ss, word, ' ');
      int y = std::stoi(word.substr(2, word.size() - 2));

      if (x < minX) { minX = x; }
      else if (x > maxX) { maxX = x; }

      if (isSensorPoint)
      {
        sensor.position = { x, y };
        isSensorPoint = false;
      }
      else 
      {
        sensor.nearestBeacon = { x, y };
      }
    }
  }

  sensor.beaconDistance = sensor.position.dist(sensor.nearestBeacon);

  return sensor;
}

MinMaxPair readInput (std::vector<Sensor> &sensorSet, std::string &filename)
{
  std::ifstream file(filename);
  std::string line;

  int minXVal = 200000000;
  int maxXVal = 0;

  while(std::getline(file, line))
  {
    Sensor sens = parseSensorInput(line, minXVal, maxXVal);
    sensorSet.push_back(sens);
  }

  return { minXVal, maxXVal };
}

std::vector<Sensor> findIntersectingSensors(std::vector<Sensor> &sensorSet, const int rowNum)
{
  std::vector<Sensor> intersectingSensors;
  for (auto& sensor : sensorSet)
  {
    int intersectionDistance = abs(rowNum - sensor.position.y);

    if (intersectionDistance <= sensor.beaconDistance)
    {
      sensor.yDistToMysteryRow = intersectionDistance;
      intersectingSensors.push_back(sensor);
    }
  }
  return intersectingSensors;
}

int findMissingIndex(std::unordered_set<int> &coveredIndicies, const int maxValue)
{
  for (int i = 1; i < maxValue; i++)
  {
    if (coveredIndicies.count(i) == 0)
    {
      return i;
    }
  }
  // for (const auto& elem : coveredIndicies)
  // {
  //   std::cout << elem << std::endl;
  // }
  return 0;
}

int findRowCoverage(std::vector<Sensor> &sensorSet, const int rowNum, const int maxValue) // const MinMaxPair &xRange,
{
  std::vector<Sensor> intersectingSensors = findIntersectingSensors(sensorSet, rowNum);
  std::unordered_set<int> coveredIndicies;
  // std::unordered_set<Point, pointHash> beaconCount;

  for (auto& sensor : intersectingSensors)
  {
    int triangleHalfBaseWidth = sensor.beaconDistance - sensor.yDistToMysteryRow;

    for (int i = (-1 * triangleHalfBaseWidth); i <= triangleHalfBaseWidth; i++)
    {
      if (sensor.position.x + i > 0 && sensor.position.x + i <= maxValue)
      {
        coveredIndicies.insert(sensor.position.x + i);
      }
    }

    // if (sensor.nearestBeacon.y == rowNum)
    // {
    //   beaconCount.insert(sensor.nearestBeacon);
    // }
  }
  // int total = coveredIndicies.size();// - beaconCount.size();
  // std::cout << "Total Indecides covered: " << total << std::endl;
  if (coveredIndicies.size() < maxValue)
  {
    std::cout << "Looking for index" << std::endl;
    return findMissingIndex(coveredIndicies, maxValue);
  }
  return coveredIndicies.size();

  // Solution below seemed to be off slightly even though it is faster
  
  // std::vector<char> mysteryRow((xRange.max - xRange.min + 1), '.');
  // int rowCoverage = 0;
  // std::cout << xRange.max << " , " << xRange.min << std::endl;
  // std::cout << "Size of row: " <<  mysteryRow.size() << std::endl;
  // for (auto &sensor : intersectingSensors)
  // {
  //   if (sensor.nearestBeacon.y == rowNum)
  //   {
  //     mysteryRow[sensor.nearestBeacon.x + 2] = 'B';
  //   }

  //   int triangleHalfBaseWidth = sensor.beaconDistance - sensor.yDistToMysteryRow;

  //   for (int i = (-1 * triangleHalfBaseWidth); i <= triangleHalfBaseWidth; i++)
  //   {
  //     int index = (sensor.position.x - (xRange.min)) + i;

  //     if (index >= mysteryRow.size()) { continue; }

  //     if (mysteryRow[index] == '.')
  //     {
  //       mysteryRow[index] = '#';
  //       rowCoverage++;
  //     }
  //   }
  // }
  // for (auto& i : mysteryRow)
  // {
  //   std::cout << i;
  // }
  // std::cout << std::endl;
  
  // std::cout << rowCoverage << std::endl;
}


void findMissingBeacon(std::vector<Sensor> &sensorSet, int dimension)
{
  for (int i = 0; i < dimension + 1; i++)
  {
    if (i == 100000 || i == 200000 || i == 500000 || i == 1000000 || i == 2000000)
    {
      std::cout << "Count :" << i << std::endl;
    }
    int val = findRowCoverage(sensorSet, i, dimension);

    if (val < dimension)
    {
      std::cout << "Done! " << val << "," << i << std::endl;
      break;
    }
  }
}
// void setSensorCoverage(std::vector<std::vector<char>> &coverageMatrix, const Sensor &sensor)
// {
//   for (int i = sensor.beaconDistance; i >= (-1 * sensor.beaconDistance); i--)
//   {
//     int halfWidth = sensor.beaconDistance - abs(i); 

//     for (int j = (-1 * halfWidth); j <= halfWidth; j++)
//     {
//       int xPos = sensor.position.x + j;
//       int yPos = sensor.position.y + i;
//       if ( xPos >= coverageMatrix[0].size() || yPos >= coverageMatrix.size() || xPos < 0 || yPos < 0 ) { continue; }
  
//       coverageMatrix[yPos][xPos] = '#';
//     }
//   }
//   coverageMatrix[sensor.position.y][sensor.position.x] = 'S';
// }

// Point findDot (std::vector<std::vector<char>> &coverageMatrix)
// {
//   for (int i = 0; i < coverageMatrix.size(); i++)
//   {
//     for (int j = 0; j < coverageMatrix[0].size(); j++)
//     {
//       if (coverageMatrix[i][j] == '.')
//       {
//         return { j, i };
//       }
//     }
//   }

//   return { 0 , 0};
// }

// void coverEntireMap (std::vector<Sensor> &sensorSet, const int maxDimension)
// {
//   std::cout << "Start Coverage" << std::endl;
//   std::vector<std::vector<char>> coverageMatrix (maxDimension, std::vector<char>(maxDimension ,'.'));

//   std::cout << "Done Initializing" << std::endl;
//   for (int i = 0; i < sensorSet.size(); i++)
//   {
//     setSensorCoverage(coverageMatrix, sensorSet[i]);
//     std::cout << "Done sensor " << i << "/" << sensorSet.size() << std::endl;
//   }
//   std::cout << "Finding Dot" << std::endl;
//   Point dot = findDot(coverageMatrix);

//   std::cout << dot.x << "," << dot.y << std::endl;
//   // printMatrix(coverageMatrix);
// }

int main (int argc, char** argv)
{
  std::string filename(argv[1]);
  std::vector<Sensor> sensorSet;
  MinMaxPair xRange = readInput(sensorSet, filename);

  // int rowNum = 10;
  // int rowNum = 2000000;
  // findRowCoverage(sensorSet, xRange, rowNum);
  // int maxDimension = 21;
  int maxDimension = 4000000 + 1;
  findMissingBeacon(sensorSet, maxDimension);
  // coverEntireMap(sensorSet, maxDimension);
  
  // for (auto &sensor : sensorSet)
  // {
  //   printSensor(sensor);
  // }

}