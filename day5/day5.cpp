#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <deque>

void readInput (std::vector<std::deque<char>> &cargoStacks, std::vector<std::vector<int>> &rearrangementSet, std::string &filename)
{
  std::ifstream file(filename);
  std::string line;
  bool hasInitializedStacks = false;

  while(std::getline(file, line))
  {
    if (!hasInitializedStacks)
    {
      std::deque<char> stack;
      int numStacks = (line.size() + 1) / 4;

      for (int i = 0; i < numStacks; i++)
      {
        cargoStacks.push_back(stack);
      }

      hasInitializedStacks = true;
    }


    if (line == "") { break; }
    if (line[1] == '1') { continue; }

    // index of char of boxes is 1, 5, 9, 13... (index is (n-1) / 4)
    for (int i = 1; i < line.size(); i+= 4)
    {
      if (line[i] != ' ')
      {
        cargoStacks[(i - 1) / 4].push_back(line[i]);
      }
    }
  }

  // Read in the instruction sets, we only care about the 3 integers that are in each line
  // First Int  : number of crates to move
  // Second Int : origin stack
  // Third Int  : destination stack 
  while(std::getline(file, line))
  {
    std::stringstream linestream(line);
    std::string article;
    std::vector<int> instruction;

    while (std::getline(linestream, article, ' '))
    {
      if (article == "move" || article == "from" || article == "to")
      {
        continue;
      }

      instruction.push_back(std::stoi(article));
    }

    rearrangementSet.push_back(instruction);
  }
}

// Part 1
void moveCratesLIFO (int numCrates, std::deque<char> &startStack, std::deque<char> &destStack)
{

  for (int i = 0; i < numCrates; i++)
  {
    char crate = startStack.front();
    startStack.pop_front();

    destStack.push_front(crate);
  }

}

// Part 2
void moveCratesMaintainOrder (int numCrates, std::deque<char> &startStack, std::deque<char> &destStack)
{
  std::deque<char> tempStack;

  for (int i = 0; i < numCrates; i++)
  {
    char crate = startStack.front();
    startStack.pop_front();

    tempStack.push_front(crate);
  }

  for (int j = 0; j < numCrates; j++)
  {
    char crate = tempStack.front();
    tempStack.pop_front();

    destStack.push_front(crate);
  }
}

int main (int argv, char** argc)
{
  std::string filename(argc[1]);
  std::vector<std::deque<char>> cargoStacks;
  std::vector<std::vector<int>> rearrangementSet;

  readInput(cargoStacks, rearrangementSet, filename);

  // Use instructions to move the crates around. 
  for (auto& instruction : rearrangementSet)
  {
    // moveCratesLIFO(instruction[0], cargoStacks[instruction[1] - 1], cargoStacks[instruction[2] - 1]);
    moveCratesMaintainOrder(instruction[0], cargoStacks[instruction[1] - 1], cargoStacks[instruction[2] - 1]);
  }


  // Display Results
  std::cout << "Result: ";

  for (auto& stack : cargoStacks)
  {
    std::cout << stack.front();
  }

  std::cout << std::endl;
}