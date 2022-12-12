#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>

#include "monkey.h"

void readInput (std::vector<Monkey> &monkeys, std::string &filename)
{
  std::ifstream file(filename);
  std::string line;

  std::vector<int> monkeyThrowOptions;

  int commonModulo = 1;
  int monkeyCount = 0;

  while (std::getline(file, line))
  {
    Monkey monk;
    std::vector<uint64_t> itemList;

    while (std::getline(file, line))
    {
      if (line == "")
      {
        monkeyCount++;
        break;
      }

      std::stringstream linestream(line);
      std::string word;

      // Get rid of blank spaces at beginning of lines
      while (std::getline(linestream, word, ' '))
      {
        if (word != "") { break; }
      }


      if (word == "Starting")
      {
        std::getline(linestream, word, ' ');

        while(std::getline(linestream, word, ' '))
        {
          if (word.back() == ',')
          {
            word.pop_back();
          }
          itemList.push_back(std::stoi(word));
        }

      }
      else if (word == "Operation:")
      {
        while (std::getline(linestream, word, ' '))
        {
          if (word[0] == '+' || word[0] == '*') { break; }
        }
        

        monk.operation = word[0];
        std::getline(linestream, word, ' ');

        if (word == "old")
        {
          monk.operationRhs = 0;
        }
        else
        {
          monk.operationRhs = std::stoi(word);
        }
      }
      else if (word == "Test:")
      {
        std::getline(linestream, word, ' '); // divisible
        std::getline(linestream, word, ' '); // by
        std::getline(linestream, word, ' '); // value
        commonModulo *= std::stoi(word);
        monk.testValue = std::stoi(word);
      }
      else
      {
        monkeyThrowOptions.push_back(line[line.size() - 1] - '0');
      }
    }


    for (auto& item : itemList)
    {
      monk.addItem(item);
    }
    monkeys.push_back(monk);
  }

  for(int i = 0; i < monkeyThrowOptions.size(); i+= 2)
  {
    monkeys[i / 2].setMonkeys(&monkeys[monkeyThrowOptions[i]], &monkeys[monkeyThrowOptions[i + 1]]);
    monkeys[i / 2].commonModulo = commonModulo;
  }

}

void runMonkeySimulation (std::vector<Monkey> &monkeys, int rounds, int partNum)
{
  for (int i = 0; i < rounds; i++)
  {
    for (auto& monkey : monkeys)
    {
      monkey.runRound(partNum);
    }

  }

  std::cout << "After Round :" << rounds << " the monkeys are holding items with these worry levels:" << std::endl;

  for (auto& monkey : monkeys)
  {
    monkey.print();
  }
}

void checkInspectionNumbers (std::vector<Monkey> &monkeys)
{
  uint64_t max1 = 0;
  uint64_t max2 = 0;

  for (auto& monkey : monkeys)
  {
    uint64_t temp = 0 ;
    if (monkey.inspectCount > max1)
    {
      temp = max1;
      max1 = monkey.inspectCount;
    }
    else if (monkey.inspectCount > max2)
    {
      max2 = monkey.inspectCount;
    }

    if (temp > max2)
    {
      max2 = temp;
    }
  }

  std::cout << max1 << " " << max2 << std::endl;
  std::cout << "Monkey Business Score: " << max1 * max2 << std::endl;
}

int main (int argc, char** argv)
{
  std::string filename(argv[1]);
  std::vector<Monkey> monkeys;

  readInput(monkeys, filename);

  // Part 1
  // runMonkeySimulation(monkeys, 20, 1);
  // Part 2
  runMonkeySimulation(monkeys, 10000, 2);

  checkInspectionNumbers(monkeys);
  return 0;
}