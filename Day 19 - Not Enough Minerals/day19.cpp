#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>

enum Resource { OreR, ClayR, ObsidianR, GeodeR };
struct RobotRequirement 
{
  int oreCost = 0;
  int clayCost = 0;
  int obsidianCost = 0;

  virtual bool hasRequirements(int a) const = 0;
  virtual bool hasRequirements(int a, int b) const = 0;
};
struct Ore : RobotRequirement
{
  bool hasRequirements(int a) const
  {
    return a >= oreCost;
  }
  bool hasRequirements(int a, int b) const { return false; }
};
struct Clay : RobotRequirement
{
  bool hasRequirements(int a) const
  {
    return a >= oreCost;
  }
  bool hasRequirements(int a, int b) const { return false; }
};
struct Obsidian : RobotRequirement
{

  bool hasRequirements(int a) const { return false; }
  bool hasRequirements(int a, int b) const { 
    return a >= oreCost && b >= clayCost; 
  }
};
struct Geode : RobotRequirement
{

  bool hasRequirements(int a) const { return false; }
  bool hasRequirements(int a, int b) const { 
    return a >= oreCost && b >= obsidianCost; 
  }
};

struct Collection
{
  int ore;
  int clay;
  int obsidian;
  int geode;
};

struct Blueprint
{
  int id;
  Ore oreRobot;
  Clay clayRobot;
  Obsidian obsidianRobot;
  Geode geodeRobot;
  Collection robotMax;
};


struct State 
{
  Collection resources;
  Collection robots;
  int minute;
  std::vector<bool> omitChoice;
};

Blueprint parseBlueprint (std::string &line)
{
  std::stringstream ss(line);
  std::string phrase;
  Blueprint bp;

  std::getline(ss, phrase, ' ');
  std::getline(ss, phrase, ':');
  bp.id = std::stoi(phrase);

  int count = 0;
  while (std::getline(ss, phrase, ' '))
  {
    if (phrase == "costs" || phrase == "and")
    { 
      std::getline(ss, phrase, ' ');
      switch (count)
      {
      case 0:
        bp.oreRobot.oreCost = std::stoi(phrase);
        bp.robotMax.ore = bp.oreRobot.oreCost;
        break;
      case 1:
        bp.clayRobot.oreCost = std::stoi(phrase);
        bp.robotMax.ore = bp.robotMax.ore < bp.clayRobot.oreCost ? bp.clayRobot.oreCost : bp.robotMax.ore;
        break;
      case 2:
        bp.obsidianRobot.oreCost = std::stoi(phrase);
        bp.robotMax.ore = bp.robotMax.ore < bp.obsidianRobot.oreCost ? bp.obsidianRobot.oreCost : bp.robotMax.ore;
        break;
      case 3:
        bp.obsidianRobot.clayCost = std::stoi(phrase);
        bp.robotMax.clay = bp.obsidianRobot.clayCost;
        break;
      case 4:
        bp.geodeRobot.oreCost = std::stoi(phrase);
        break;
      case 5:
        bp.geodeRobot.obsidianCost = std::stoi(phrase);
        bp.robotMax.obsidian = bp.geodeRobot.obsidianCost;
        break;
      default:
        break;
      }

      count++;
    }
  }

  return bp;
}

void readInput (std::vector<Blueprint> &blueprints, const std::string &filename)
{
  std::ifstream file(filename);
  std::string line;

  while(std::getline(file, line))
  {
    Blueprint bp = parseBlueprint(line);
    blueprints.push_back(bp);
  }
}

bool canPurchaseRobot (const Collection &resources, const RobotRequirement &req, Resource resourceType)
{
  switch (resourceType)
  {
  case GeodeR:
    return req.hasRequirements(resources.ore, resources.obsidian); 
  case OreR:
    return req.hasRequirements(resources.ore);
  case ClayR:
    return req.hasRequirements(resources.ore);
  case ObsidianR:
    return req.hasRequirements(resources.ore, resources.clay);
  default:
    return false;
  }
}

void purchaseRobot (State &st, const RobotRequirement &req, Resource resourceType)
{
  switch (resourceType)
  {
  case GeodeR:
    st.robots.geode++;
    st.resources.ore -= req.oreCost;
    st.resources.obsidian -= req.obsidianCost;
    break;
  case OreR:
    st.robots.ore++;
    st.resources.ore -= req.oreCost;
    break;
  case ClayR:
    st.robots.clay++;
    st.resources.ore -= req.oreCost;
    break;
  case ObsidianR:
    st.robots.obsidian++;
    st.resources.ore -= req.oreCost;
    st.resources.clay -= req.clayCost;
  default:
    break;
  }
}

State addMinutesResources(const State &oldState)
{
  State newState = oldState;

  newState.resources.ore += newState.robots.ore;
  newState.resources.clay += newState.robots.clay;
  newState.resources.obsidian += newState.robots.obsidian;
  newState.resources.geode += newState.robots.geode;

  return newState;
}

State updateResources(const State &st)
{
  State upSt = addMinutesResources(st);
  upSt.minute++;

  return upSt;
}

State getBuyRobotState(const State &st, const RobotRequirement &robReq, Resource resourceType)
{
  State buySt = updateResources(st);
  purchaseRobot(buySt, robReq, resourceType);

  return buySt;
}

std::vector<State> getPossibleStates (const State &current, const Blueprint &bp, const int currentMaxGeodes, const int totalTime)
{
  std::vector<bool> skippedChoices = { false, false, false, false };
  std::vector<State> possibleStates;
  State copy = current;

  // Calculate the maxiumum theoretical # geodes this state could end with.
  // If it's less than our current max # geodes then we can stop following this path.
  int maxAdditionGeodes = 0;
  int timeLeft = totalTime - copy.minute;
  if (canPurchaseRobot(copy.resources, bp.geodeRobot, GeodeR))
  {
    maxAdditionGeodes = ((timeLeft * copy.robots.geode) + (timeLeft * (timeLeft - 1)) / 2);
  }
  else 
  {
    maxAdditionGeodes = (timeLeft * copy.robots.geode + ((timeLeft - 1) * (timeLeft - 2)) / 2);
  }
  if (maxAdditionGeodes + copy.resources.geode < currentMaxGeodes)
  {
    return possibleStates;
  }

  // Purchase Geode Robot
  if (canPurchaseRobot(copy.resources, bp.geodeRobot, GeodeR))
  {
    possibleStates.push_back(getBuyRobotState(copy, bp.geodeRobot, GeodeR));
    // If we can buy a geode then that is the only choice we will give
    return possibleStates;
  }

  // Could put these 3 checks (and/or 1 above) as a function, but I don't see a way that doesn't mess around
  // what I've set up. Plus its very few lines already.

  // Purchase Ore Robot
  if (copy.robots.ore < bp.robotMax.ore && !copy.omitChoice[OreR] && canPurchaseRobot(copy.resources, bp.oreRobot, OreR))
  {
    possibleStates.push_back(getBuyRobotState(copy, bp.oreRobot, OreR));
    skippedChoices[OreR] = true;
  }

  // Purchase Clay Robot
  if (copy.robots.clay < bp.robotMax.clay && !copy.omitChoice[ClayR] && canPurchaseRobot(copy.resources, bp.clayRobot, ClayR))
  {
    possibleStates.push_back(getBuyRobotState(copy, bp.clayRobot, ClayR));
    skippedChoices[ClayR] = true;
  }

  // Purchase Obsidian Robot
  if (copy.robots.obsidian < bp.robotMax.obsidian && !copy.omitChoice[ObsidianR] && canPurchaseRobot(copy.resources, bp.obsidianRobot, ObsidianR))
  {
    possibleStates.push_back(getBuyRobotState(copy, bp.obsidianRobot, ObsidianR));
    skippedChoices[ObsidianR] = true;
  }

  // Wait 
  State waitState = updateResources(copy);
  waitState.omitChoice = skippedChoices;
  possibleStates.push_back(waitState);

  
  return possibleStates;
}

int breakMaxGeodes (const Blueprint &bp, const int maxMinutes)
{
  State startState = {{ 0, 0, 0, 0 }, { 1, 0, 0, 0 }, 0, { false, false, false, false }};
  std::stack<State> stateStack;
  std::vector<State> visitedStates;
  int maxGeodes = 0;
  long long count = 0;

  stateStack.push(startState);

  while(!stateStack.empty())
  {
    State st = stateStack.top();
    stateStack.pop();
    count++;

    if (st.minute >= maxMinutes)
    {
      updateResources(st);
      if (st.resources.geode > maxGeodes)
      {
        maxGeodes = st.resources.geode;
      }
      continue;
    }
    else
    {
      // Function -> returns vector of states that we have allowed given certain pruning,
      std::vector<State> possibleStates = getPossibleStates(st, bp, maxGeodes, maxMinutes);

      for (auto &st : possibleStates)
      {
        stateStack.push(st);
      }
    }

  }
  std::cout << "Iterations: " << count << std::endl;
  return maxGeodes;
}

void sumBlueprintQuality (const std::vector<Blueprint> &blueprints, const int maxMinutes)
{
  int qualitySum = 0;

  for(auto& blueprint : blueprints)
  {
    int geodes = breakMaxGeodes(blueprint, maxMinutes);
    qualitySum += geodes * blueprint.id;
    std::cout << "Blueprint " << blueprint.id << " cracked " << geodes << " geodes. For a quality of :" << geodes * blueprint.id << std::endl;
  }

  std::cout << "Total Blueprint quality is: " << qualitySum << std::endl;
}

int main (int argc, char** argv)
{
  std::string filename(argv[1]);
  std::vector<Blueprint> blueprints;

  readInput(blueprints, filename);
  // Part 1
  sumBlueprintQuality(blueprints, 24);
  
  // Part 2
  std::vector<Blueprint> remainingPrints = {blueprints[0], blueprints[1]};
  if (blueprints.size() > 2)
  {
    remainingPrints.push_back(blueprints[2]);
  }
  sumBlueprintQuality(remainingPrints, 32);
}