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

void findRowCoverage(std::vector<Sensor> &sensorSet, const MinMaxPair &xRange, const int rowNum)
{
  std::vector<Sensor> intersectingSensors = findIntersectingSensors(sensorSet, rowNum);
  std::unordered_set<int> coveredIndecies;
  std::unordered_set<Point, pointHash> beaconCount;

  // int beaconOffset = 0;
  for (auto& sensor : intersectingSensors)
  {
    int triangleHalfBaseWidth = sensor.beaconDistance - sensor.yDistToMysteryRow;

    for (int i = (-1 * triangleHalfBaseWidth); i <= triangleHalfBaseWidth; i++)
    {
      coveredIndecies.insert(sensor.position.x + i);
    }

    if (sensor.nearestBeacon.y == rowNum)
    {
      std::cout << sensor.nearestBeacon.x << "," << sensor.nearestBeacon.y << std::endl;
      beaconCount.insert(sensor.nearestBeacon);
    }
  }
  std::cout << beaconCount.size() << std::endl;
  int total = coveredIndecies.size() - beaconCount.size();
  std::cout << coveredIndecies.size() << std::endl;
  std::cout << total << std::endl;
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

int main (int argc, char** argv)
{
  std::string filename(argv[1]);
  std::vector<Sensor> sensorSet;
  MinMaxPair xRange = readInput(sensorSet, filename);

  // int rowNum = 10;
  int rowNum = 2000000;
  findRowCoverage(sensorSet, xRange, rowNum);
  
  // for (auto &sensor : sensorSet)
  // {
  //   printSensor(sensor);
  // }

}