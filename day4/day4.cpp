#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

void readInput(std::vector<std::vector<int>> &assignmentPairs, std::string &filename)
{
  std::ifstream file(filename);
  std::string line;

  while(std::getline(file, line))
  {
    std::stringstream linestream(line);
    std::string assignmentRange;
    std::vector<int> assignments;

    while(std::getline(linestream, assignmentRange, ','))
    {
      // std::cout << assignmentVal << std::endl;
      //first and third character of assignmentVal is the number range
      std::stringstream assignmentStream(assignmentRange);
      std::string value;
      while(std::getline(assignmentStream, value, '-'))
      {
        assignments.push_back(std::stoi(value));
      }
    }

    assignmentPairs.push_back(assignments);
    assignments.clear();
  }
}

// Function takes the pair of assignments (represented as vector of 4 ints)
// and returns a boolean value of whether the assignments completely overlap or not
bool checkCompleteOverlap (std::vector<int> &assignmentPair)
{
  if ((assignmentPair[0] <= assignmentPair[2] && assignmentPair[1] >= assignmentPair[3])
        || (assignmentPair[2] <= assignmentPair[0] && assignmentPair[3] >= assignmentPair[1]))
  {
    return true;
  }

  return false;
}

bool checkPartialOverlap (std::vector<int> &pair)
{
  if (pair[0] > pair[3] || pair[2] > pair[1])
  {
    return false;
  }

  return true;
}

int main (int argv, char** argc)
{
  std::string filename(argc[1]);
  std::vector<std::vector<int>> assignmentPairs;

  readInput(assignmentPairs, filename);

  int overlapCount = 0;
  int partialCount = 0;
  for (auto& pair : assignmentPairs)
  {
    bool isCompleteOverlap = checkCompleteOverlap(pair);
    bool isPartialOverlap = checkPartialOverlap(pair);
    // std::cout << " : " << isPartialOverlap << std::endl;
    overlapCount += isCompleteOverlap;
    partialCount += isPartialOverlap;

    if (!isPartialOverlap)
    {
      std::cout << pair[0] << "," << pair[1] << " :: " << pair[2] << "," << pair[3] << std::endl;
    }
  }

  std::cout << overlapCount << std::endl;
  std::cout << partialCount << std::endl;

}