#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

std::map<char, int> SNAFUToDigit = {
  { '2' ,  2 }, 
  { '1' ,  1 }, 
  { '0' ,  0 }, 
  { '-' , -1 }, 
  { '=' , -2 }
};
std::map<int, char> DigitToSNAFU = {
  {  2, '2' }, 
  {  1, '1' }, 
  {  0, '0' }, 
  { -1, '-' }, 
  { -2, '=' }
};

void readInput(std::vector<std::string> &fuelValues, const std::string filename)
{
  std::ifstream file(filename);
  std::string line;

  while(std::getline(file, line))
  {
    fuelValues.push_back(line);
  }
}

long long snafuToDecimal (const std::string &snafuValue)
{
  long long value = 0;
  long long multiplier = 1;
  for (int i = snafuValue.size() - 1; i >= 0; i--)
  {
    long long digitValue = SNAFUToDigit[snafuValue[i]];
    value += digitValue * multiplier;
    multiplier *= 5;
    // std::cout << "Multiplier" << i << ":" << multiplier << std::endl;
  }

  return value;
}

std::string decimalToBase5(long long value)
{
  std::string base5("");
  long long remainder = 0;
  long long quotient = value;

  while (quotient != 0)
  {
    remainder = quotient % 5;
    quotient = quotient / 5;
    base5.insert(0, std::to_string(remainder));
    std::cout << base5 << std::endl;
  }

  return base5;
}

std::string decimalToSnafu (long long value)
{
  std::string base5 = decimalToBase5(value);
  std::string snafu("");
  int carry = 0;

  for (int i = base5.size() - 1; i >= 0; i--)
  {
    int digit = (base5[i] - '0') + carry;

    if (digit > 2)
    {
      carry = 1;
      digit -= 5;
    }
    else
    {
      carry = 0;
    }

    snafu = DigitToSNAFU[digit] + snafu; 
  }

  return snafu;
}

long long sumFuelValues (std::vector<std::string> &fuelValues)
{
  long long sum = 0;

  for (auto& snafuFuel : fuelValues)
  {
    long long decimalFuel = snafuToDecimal(snafuFuel);
    sum += decimalFuel;
  }

  return sum;
}

void bobsConsoleNumber(std::vector<std::string> &fuelValues)
{
  long long fuelSum = sumFuelValues(fuelValues);
  std::cout << "Sum of the fuel values is: " << fuelSum << std::endl;
  std::string consoleInput = decimalToSnafu(fuelSum);
  std::cout << "Bobs console input should be: " << consoleInput << std::endl;
}
int main (int argc, char** argv)
{
  std::string filename(argv[1]);
  std::vector<std::string> fuelValues;

  readInput(fuelValues, filename);
  bobsConsoleNumber(fuelValues);

  return 0;
}