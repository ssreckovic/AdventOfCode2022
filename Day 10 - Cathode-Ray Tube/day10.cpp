#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
struct command
{
  int value, cycles;
  std::string cmd;
};

std::vector<command> readInput (std::string &filename)
{
  std::ifstream file(filename);
  std::string line;
  std::vector<command> commands;

  while (std::getline(file, line))
  {
    std::stringstream linestream(line);
    std::string cmdWord;
    std::string value;

    command instruction;
    
    if (line[0] == 'a')
    {
      std::getline(linestream, cmdWord, ' ');
      std::getline(linestream, value, ' ');
      
      instruction.value = std::stoi(value);
      instruction.cycles = 2;
      instruction.cmd = cmdWord;
    }
    else
    {
      std::getline(linestream, cmdWord);

      instruction.value = 0;
      instruction.cycles = 1;
      instruction.cmd = cmdWord;
    }

    commands.push_back(instruction);
  }

  return commands;
}

std::vector<int> calculateInterestingSignals(std::vector<command> &commandList)
{
  const int MAX_CYCLE = 221;
  int cmdIdx = 0;
  int commandExecutionCycle = commandList[0].cycles + 1;

  int cycle = 1;
  int registerX = 1;
  
  int interestingCycle = 20;
  int commandSize = commandList.size();

  std::vector<int> interestingSignals;

  while (cmdIdx < commandSize && cycle < MAX_CYCLE)
  {
    if (cycle == commandExecutionCycle)
    {
      registerX += commandList[cmdIdx].value;
      // std::cout << cmdIdx << "(" << cycle << ") End Command" << std::endl;
      cmdIdx++;
      commandExecutionCycle = cycle + commandList[cmdIdx].cycles;
      // std::cout << cmdIdx << "(" << cycle << ") Start Command: " << commandList[cmdIdx].cmd << " - " << commandList[cmdIdx].value << std::endl; 
    }


    if (cycle == interestingCycle)
    {
      // std::cout << "Cycle:" << cycle << "  Value: " << registerX << std::endl;
      interestingSignals.push_back(cycle * registerX);
      interestingCycle += 40;
    }

    cycle++;
  }

  return interestingSignals;
}

void drawSignals (std::vector<command> &commandList)
{
  const int MAX_CYCLE = 241;
  int cmdIdx = 0;
  int commandExecutionCycle = commandList[0].cycles + 1;

  int cycle = 1;
  int spriteCenter = 2;

  int signalLineBreak = 40;
  int commandSize = commandList.size();

  std::vector<int> interestingSignals;

  while (cmdIdx < commandSize && cycle < MAX_CYCLE)
  {
    if (cycle == commandExecutionCycle)
    {
      spriteCenter += commandList[cmdIdx].value;
      cmdIdx++;
      commandExecutionCycle = cycle + commandList[cmdIdx].cycles;
    }

    if ((cycle - 1) % 40 + 1 >= spriteCenter - 1 && (cycle - 1) % 40 + 1 <= spriteCenter + 1)
    {
      std::cout << '#';
    }
    else
    {
      std::cout << '.';
    }


    if (cycle == signalLineBreak)
    {
      std::cout << std::endl;
      signalLineBreak += 40;
    }

    cycle++;
  }
}

int main (int argc, char** argv)
{
  std::string filename(argv[1]);

  std::vector<command> commandList = readInput(filename);

  // Part 1
  std::vector<int> interestingSignalValues = calculateInterestingSignals(commandList);

  // Part 2
  drawSignals(commandList);

  int sum = 0;
  for (auto& signal : interestingSignalValues)
  {
    // std::cout << signal << std::endl;
    sum += signal;
  }
  std::cout << "Part 1 Answer: " << sum << std::endl;
  return 0;
}