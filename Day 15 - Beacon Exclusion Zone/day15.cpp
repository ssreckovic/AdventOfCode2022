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

  bool operator!=(const Point &rhs) const{
      return (x != rhs.x) || (y != rhs.y);
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

std::vector<Sensor> findSensorsIntersectingWithRow(std::vector<Sensor> &sensorSet, const int rowNum)
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

// Find the amount of a given row that is covered. Had a better solution before that was way off
// but using the set works, even if it slows it down significantly.
int findRowCoverage(std::vector<Sensor> &sensorSet, const int rowNum)
{
  std::vector<Sensor> intersectingSensors = findSensorsIntersectingWithRow(sensorSet, rowNum);
  std::unordered_set<int> coveredIndicies;
  std::unordered_set<Point, pointHash> beaconCount;

  for (auto& sensor : intersectingSensors)
  {
    int triangleHalfBaseWidth = sensor.beaconDistance - sensor.yDistToMysteryRow;

    for (int i = (-1 * triangleHalfBaseWidth); i <= triangleHalfBaseWidth; i++)
    {
      coveredIndicies.insert(sensor.position.x + i);
    }

    if (sensor.nearestBeacon.y == rowNum)
    {
      beaconCount.insert(sensor.nearestBeacon);
    }
  }
  int total = coveredIndicies.size() - beaconCount.size();
  std::cout << "Total Indicides covered: " << total << std::endl;

  return coveredIndicies.size();

}

std::vector<Point> findPerimeterPoints(Sensor &sensor, const int maxDimension)
{
  std::vector<Point> perimeter;

  for (int i = (-1 * (sensor.beaconDistance + 1)); i <= (sensor.beaconDistance + 1); i++)
  {
    int yPos = (sensor.beaconDistance + 1) - abs(i);

    if (sensor.position.x + i < 0 || sensor.position.x + i > maxDimension)
    {
      continue;
    }

    if (sensor.position.y + yPos <= maxDimension) 
    {
      perimeter.push_back({ sensor.position.x + i, sensor.position.y + yPos });
    }

    // yPos != 0 Avoid duplicates
    if (sensor.position.y - yPos >= 0)
    {
      perimeter.push_back({ sensor.position.x + i, sensor.position.y - yPos });
    }
  }

  return perimeter;
}

bool hasPointSensorIntersection(Point &point, std::vector<Sensor> &sensorSet, Sensor &currentSensor)
{
  for (auto& sensor : sensorSet)
  {
    if (sensor.position != currentSensor.position && sensor.position.dist(point) <= sensor.beaconDistance)
    {
      return true;
    }
  }

  return false;
}

Point findHiddenBeacon (std::vector<Sensor> &sensorSet, const int maxDimension)
{
  // Loop through all sensors and find the points just on the outside of the sensor range
  // Check those points against other sensors to see if there is any intersection
  // If there is no intersection that is the hidden beacon

  for (auto& sensor : sensorSet)
  {
    std::vector<Point> outsidePerimeterPoints = findPerimeterPoints(sensor, maxDimension);

    for(auto& pt : outsidePerimeterPoints)
    {
      bool hasIntersection = hasPointSensorIntersection(pt, sensorSet, sensor);

      if (!hasIntersection)
      {
        return pt;
      }
    }
  }

  return {0,0};
}

int main (int argc, char** argv)
{
  std::string filename(argv[1]);
  std::vector<Sensor> sensorSet;
  MinMaxPair xRange = readInput(sensorSet, filename);

  std::cout << "Part 1" << std::endl;
  // int rowNum = 10; // For sample input
  int rowNum = 2000000;
  findRowCoverage(sensorSet, rowNum);
  
  //Part 2
  std::cout << "Part 2" << std::endl;
  // int maxDimension = 21; // For sample input
  int maxDimension = 4000001;
  Point p = findHiddenBeacon(sensorSet, maxDimension);
  std::cout << "This number is too big to show apperantly: " << std::endl;
  std::cout << p.x << "," << p.y << std::endl;
  std::cout << "Tuning Frequency: " << p.x * 4 << "000000 + " << p.y << std::endl;

  return 0;
}