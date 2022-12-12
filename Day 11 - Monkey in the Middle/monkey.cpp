#include <iostream>
#include "monkey.h"

Monkey::Monkey() {};
Monkey::Monkey(std::queue<uint64_t> item, char operation, int operationRhs, int testValue, int commonModulo)
      : items(item),
        operation(operation),
        operationRhs(operationRhs),
        testValue(testValue),
        commonModulo(commonModulo)
      {};


void Monkey::addItem (uint64_t item)
{
  items.push(item);
}

void Monkey::runRound (int partNum)
{
  while (!items.empty()) 
  {
    // Monkey inspects an item
    uint64_t chosenItem = items.front();
    inspectCount++;
    items.pop();

    // Worry level is increased
    if (operation == '+')      { chosenItem += operationRhs; }
    else if (operation == '*') 
    { 
      if (operationRhs == 0) {
        chosenItem *= chosenItem;
      }
      else
      {
        chosenItem *= operationRhs; 
      }
    }

    // Monkey gets bored with item, Worry level decreased
    /**************************Division by 3 only happens in part 1********************************/
    if (partNum == 1)
    {
      chosenItem /= 3;
    }
    else if (partNum == 2)
    {
      chosenItem = chosenItem % commonModulo;
    }
    // Choose which monkey to throw it to
    if ( chosenItem % testValue == 0 )
    {
      m_trueMonkey->addItem(chosenItem);
    }
    else 
    {
      m_falseMonkey->addItem(chosenItem);
    }
    
  }
}

void Monkey::setMonkeys(Monkey *trueMonkey, Monkey *falseMonkey)
{
  m_falseMonkey = falseMonkey;
  m_trueMonkey = trueMonkey;
}

const void Monkey::print()
{
  std::queue copy = items;


  std::cout << "Monkey div: " << testValue;
  // std::cout << "Operation: " << operation << " RHS:" << operationRhs;
  // std::cout << "  Throw Options: " << m_trueMonkey->testValue << "  False " << m_falseMonkey->testValue; 
  std::cout << "  Items: ";
  while (!copy.empty())
  {
    std::cout << copy.front() << ",";
    copy.pop();
  }

  std::cout << std::endl;
}