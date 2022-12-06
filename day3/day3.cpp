#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

void readInput(std::vector<std::string> &rucksacks, std::string &filename)
{
  std::ifstream file(filename);
  std::string line;

  while(std::getline(file, line))
  {
    rucksacks.push_back(line);
  }
}

void readInput(std::vector<std::vector<std::string>> &elfGroups, std::string &filename)
{
  std::ifstream file(filename);
  std::string line;

  std::vector<std::string> rucksacks;
  int count = 0;

  while(std::getline(file, line))
  {
    rucksacks.push_back(line);
    count++;

    if (count % 3 == 0 && count != 0)
    {
      elfGroups.push_back(rucksacks);
      rucksacks.clear();
    }

  }

}

char findCommonCharacter(std::string &s1, std::string &s2)
{
  std::map<char, bool> viewedChars;

  for (char& c : s1)
  {
    viewedChars.insert( std::pair<char, bool> (c, false));
  }

  for (char& c : s2)
  {
    if (viewedChars.count(c))
    {
      return c;
    }
  }

  return 'a';
}

char findCommonCharacter(std::string &s1, std::string &s2, std::string &s3)
{
  std::map<char, int> viewedCharCountMap;

  for (char& c : s1)
  {
    viewedCharCountMap.insert( std::pair<char, int>(c, 1) );
  }

  for (char& c : s2)
  {
    if (viewedCharCountMap[c])
    {
      viewedCharCountMap[c]++;
    }
  }

  for (char c : s3)
  {
    if (viewedCharCountMap[c] >= 2)
    {
      return c;
    }
  }

  return 'a';
}

int main (int argc, char** argv)
{
  std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  std::map<char, int> alphabetValues;
  std::string filename(argv[1]);

  std::vector<std::string> rucksacks;
  std::vector<std::vector<std::string>> elfGroups;

  for (int i = 0; i < alphabet.size(); i++)
  {
    alphabetValues.insert(std::pair<char, int> (alphabet[i], i + 1));
  }

  // readInput(rucksacks, filename);
  readInput(elfGroups, filename);

  int prioritySum = 0;
  // for(auto& sack : rucksacks)
  // {
  //   std::cout << sack << std::endl;
  //   // std::cout << sack.substr(0, sack.size() / 2) << std::endl;
  //   // std::cout << sack.substr(sack.size() / 2, sack.size()) << std::endl;
  //   std::string firstHalf = sack.substr(0, sack.size() / 2);
  //   std::string secondHalf = sack.substr(sack.size() / 2, sack.size());
  //   prioritySum += alphabetValues.at(findCommonCharacter(firstHalf, secondHalf));
  // }

  for (auto& group : elfGroups)
  {
    char val = findCommonCharacter(group[0], group[1], group[2]);
    prioritySum += alphabetValues.at(val);
    // prioritySum += alphabetValues.at(findCommonCharacter(group[0], group[1], group[2]));
  }

  std::cout << "Final Score: " << prioritySum << std::endl;
}