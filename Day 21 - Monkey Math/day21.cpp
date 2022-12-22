#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <stack>

struct MonkeyMath 
{
  long long value;
  bool hasValue;
  std::string name;
  std::string lhs;
  std::string rhs;
  char mathOperator;
};

struct MathOperation 
{
  long long value;
  bool isLhs;
  char operation;
};

struct MysteryTracker
{
  bool foundTarget;
  char side;
  long long value;
};


MonkeyMath parseMonkey (std::string &line)
{
  std::stringstream ss(line);
  std::string word;
  MonkeyMath mm;

  std::getline(ss, word, ' ');
  word.pop_back(); // get rid of : at end of name
  mm.name = word;

  std::getline(ss, word, ' ');

  if (word.size() < 4 || std::isdigit(word[0]))
  {
    mm.hasValue = true;
    mm.value = std::stoi(word); 

    return mm;
  }

  mm.hasValue = false;
  mm.lhs = word;

  std::getline(ss, word, ' ');
  mm.mathOperator = word[0];

  std::getline(ss, word, ' ');
  mm.rhs = word;

  return mm;
}

void readInput(std::vector<MonkeyMath> &monkeySet, std::string &filename)
{
  std::ifstream file(filename);
  std::string line;

  while(std::getline(file, line))
  {
    MonkeyMath monk = parseMonkey(line);
    monkeySet.push_back(monk);
  }
}

void setMapValues (std::map<std::string, MonkeyMath> &map, const std::vector<MonkeyMath> &monkeySet)
{
  for (auto &monkey : monkeySet)
  {
    map.insert({ monkey.name, monkey });
  }
}

// Find the index of the object with name "root"
int findRootIndex (const std::vector<MonkeyMath> &monkeySet)
{
  for (int i = 0; i < monkeySet.size(); i++)
  {
    if (monkeySet[i].name == "root")
    {
      return i;
    }
  }

  return 0;
}

long long useMonkeyOperator (long long lhs, long long rhs, char mathOperator)
{
  if (mathOperator == '+') { return lhs + rhs; }
  else if (mathOperator == '-') { return lhs - rhs; }
  else if (mathOperator == '*') { return lhs * rhs; }
  else if (mathOperator == '/') { return lhs / rhs; }

  return 0;
}

// Find and return the value of the given monkey (either by value or calculating it based on 'child' values and an operator)
long long findMonkeyValue (MonkeyMath &monkey, std::map<std::string, MonkeyMath> &monkeyMap)
{
  if (monkey.hasValue)
  {
    return monkey.value;
  }

  long long lhs = findMonkeyValue(monkeyMap.at(monkey.lhs), monkeyMap);
  long long rhs = findMonkeyValue(monkeyMap.at(monkey.rhs), monkeyMap);

  long long val = useMonkeyOperator(lhs, rhs, monkey.mathOperator);
  // std:: cout << monkey.name << " = " << monkey.lhs << "(" << lhs << ") " << monkey.mathOperator << " " << monkey.rhs << "(" << rhs << ") == " << val << std::endl;

  monkey.hasValue = true;
  
  return val;
}


// Given a mathematical operation, the value applied to that operation and which side of the operation the value is 
// on, determine the inverse operation and add it to the top of a given stack.
void addInverseOperation (long long value, char initialOperation, bool isLhs, std::stack<MathOperation> &operationStack)
{
  MathOperation op;

  op.value = value;

  // apply inverse operation
  if (initialOperation == '+') { 
    op.operation = '-'; 
    op.isLhs = true;
  }
  else if (initialOperation == '-') { 
    op.operation = '+'; 
    op.isLhs = isLhs;

    if (!isLhs)
    {
      // if the negative is on the right side, flip it so (value - X = (target) => value - (target) = X)
      // Where X is what we are solving for, and target is what the equality is being compared against
      op.operation = '-';
    }
  }
  else if (initialOperation == '*') { 
    op.operation = '/'; 
    op.isLhs = true;
  }
  else if (initialOperation == '/') { 
    op.operation = '*'; 
    op.isLhs = isLhs;

    // If the unknown is the divisor we must 'rearrange' the equation so that it is no longer there. so (value / X = (target) becomes)
    // (value / (target) = X). 
    // Where X is what we solve for, and target is what the equality is being compared against.
    if (!isLhs)
    {
      op.operation = '/';
    }
  }

  operationStack.push(op);
}

// Run through the set of calculations recursively, similar to 'findMonkeyValue' except if the target 'humn' is found
// start building a stack of inverted operations on it.
MysteryTracker reverseFind (MonkeyMath &monkey, std::map<std::string, MonkeyMath> &monkeyMap, std::stack<MathOperation> &operationStack)
{

  MysteryTracker mt;

  if (monkey.name == "humn")
  {
    mt.foundTarget = true;
    mt.value = monkey.value;
    return mt;
  }

  if (monkey.hasValue)
  {
    mt.value = monkey.value;
    mt.foundTarget = false;
    return mt;
  }

  MysteryTracker lhs = reverseFind(monkeyMap.at(monkey.lhs), monkeyMap, operationStack);
  MysteryTracker rhs = reverseFind(monkeyMap.at(monkey.rhs), monkeyMap, operationStack);
  MysteryTracker val;
  val.foundTarget = false;
  val.value = useMonkeyOperator(lhs.value, rhs.value, monkey.mathOperator);


  if (lhs.foundTarget && !rhs.foundTarget)
  {
    addInverseOperation(rhs.value, monkey.mathOperator, true, operationStack);
    val.foundTarget = true;
  }

  if (rhs.foundTarget && !lhs.foundTarget)
  {
    addInverseOperation(lhs.value, monkey.mathOperator, false, operationStack);
    val.foundTarget = true;
  }

  monkey.hasValue = true;
  
  return val;
}

// Part 1: Find the value of the root element.
void findRootValue (std::vector<MonkeyMath> &monkeySet)
{
  std::map<std::string, MonkeyMath> monkeyMap;
  setMapValues(monkeyMap, monkeySet);

  long long rootVal = 0;
  rootVal =  findMonkeyValue(monkeyMap.at("root"), monkeyMap);
  std::cout << "Part 1 answer: " << rootVal << std::endl;
}

// Part 2: Find the value of 'humn' that results in root : lhr == rhs to be true
void findHiddenEqualityValue(std::vector<MonkeyMath> &monkeySet)
{
  std::map<std::string, MonkeyMath> monkeyMap;
  std::stack<MathOperation> operationStack;
  setMapValues(monkeyMap, monkeySet);
  int rootIndex = findRootIndex(monkeySet);

  // Find the value of 'humn' and build the stack of inverse operations to be applied to it
  MysteryTracker rootValLeft;
  MysteryTracker rootValRight;
  rootValLeft = reverseFind(monkeyMap.at(monkeySet[rootIndex].lhs), monkeyMap, operationStack);
  rootValRight = reverseFind(monkeyMap.at(monkeySet[rootIndex].rhs), monkeyMap, operationStack);

  long long rootVal;
  if (rootValRight.foundTarget)
  {
    rootVal = rootValLeft.value;
  }
  else if (rootValLeft.foundTarget) 
  {
    rootVal = rootValRight.value;
  }

  // Run through the stack and complete the calculations
  while (!operationStack.empty())
  {
    MathOperation op = operationStack.top();
    operationStack.pop();

    if (op.isLhs)
    {
      rootVal = useMonkeyOperator(rootVal, op.value, op.operation);
      // std::cout << "X " << op.operation << " " << op.value << " = " << rootVal << std::endl;
    }
    else
    {
      rootVal = useMonkeyOperator(op.value, rootVal, op.operation);
      // std::cout << op.value << " " << op.operation << " X" << " = " << rootVal << std::endl;
    }
  }


  std::cout << "Part 2 answer: " << rootVal << std::endl;
}

int main (int argc, char** argv)
{
  std::string filename(argv[1]);
  std::vector<MonkeyMath> monkeys;

  readInput(monkeys, filename);
  
  // Part 1
  // findRootValue(monkeys);

  // Part 2
  findHiddenEqualityValue(monkeys);

  return 0;

}