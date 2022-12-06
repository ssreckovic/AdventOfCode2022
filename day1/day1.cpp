#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>

bool descendingOrder(int i, int j) { return i > j; }

std::vector<int> readDayInput(const std::string& file) 
{
  std::ifstream source;
  source.open(file);
  std::string line;
  std::vector<int> sums;
  int currentSum;

  while(std::getline(source, line))
  {
    if (line != "")
    {
      currentSum += std::stoi(line);
    }

    if(line == "")
    {
      sums.push_back(currentSum);
      currentSum = 0;
    }
  }

  source.close();
  return sums;
}

int main(int argc, char** argv) 
{
  std::string inputFile(argv[1]);
  std::vector<int> calorieSums = readDayInput(inputFile);

  std::sort(calorieSums.begin(), calorieSums.end(), descendingOrder);
  int sumTopCals = 0;


  for (int i = 0; i < 3; i++) 
  {
    sumTopCals += calorieSums[i];
  }
  std::cout << "Sum Value is: " << sumTopCals << std::endl;

  
  return 0;
}