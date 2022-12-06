#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>

int evaluatePlayerMove (int enemyChoice, int outcome)
{
  if (outcome == 2) // tie
  {
    return enemyChoice;
  }

  if (outcome == 3) // win
  {
    return (enemyChoice % 3) + 1;
  }

  // lose
  if (enemyChoice == 1) 
  {
    return 3;
  }

  return (enemyChoice - 1);
}

int evaluateRPS (int i, int j)
{
  // Where i represents the enemy player, j represents you
  if (i == j)
  {
    // tie!
    return 3;
  }

  if (i - 1 == j || (i - 1 == j % 3))
  {
    // you lose
    return 0;
  }

  // else: you win
  return 6;
}

void readInput(std::vector<std::vector<int>> &battles, std::string &filename, const std::map<char, int> &RPSMap)
{
  std::ifstream file(filename);
  std::string line;

  while(std::getline(file, line))
  {
    if (line == "")
    {
      continue;
    }

    std::vector<int> lineValues;
    std::stringstream lineStream(line);

    char value;
    while(lineStream >> value)
    {
      lineValues.push_back(RPSMap.at(value));
    }

    battles.push_back(lineValues);
  }
}

int main (int argc, char** argv)
{
  std::string filename = argv[1];
  std::map<char, int> RPSMap = { {'A', 1}, {'B', 2}, {'C', 3}, {'X', 1}, {'Y', 2}, {'Z', 3}};
  std::vector<std::vector<int>> battles;
  int winVals [3] = { 0, 3, 6 };

  readInput(battles, filename, RPSMap);

  int score = 0;

  for (auto& battle : battles)
  {
    score += winVals[battle[1] - 1] + evaluatePlayerMove(battle[0], battle[1]);;
  }

  std::cout << "Final Score: " << score << std::endl;
  return 0;
}