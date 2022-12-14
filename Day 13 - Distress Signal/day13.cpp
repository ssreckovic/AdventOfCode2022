#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stack>
#include <algorithm>


/*
  Input: pairs of packets separated by blank lines which are 'out of order'.
    - Data consists of lists and integers.
    - Each list starts with [ and ends with ]
    - List contains zero or more comma-separated values (either lists or integers)
  Goal: Identify how many pairs of packets are in the right order
  Comparing Packets:
    - If values are both integers, lower integer should come first. 
      - If left < right, the packets are in the correct order
      - If left > right, the packets are in the incorrect order
      - If left == right, continue checking the rest of the input

    - If value are both lists, compare the first value of each, then second etc
      - If left list runs out of items first, inputs are in correct order
      - If right list runs out of items first, inputs are in incorrect order
      - If list lengths are the same, and no comparison makes a decision then continue checking rest of the input

    - If exactly one value is an integer, convert the integer to a list which contains that integer as its only value
      then retry the comparison

    0 will represent left > right  (incorrect order)
    1 will represent right > left  (correct order)
    2 will represent left == right (continue)
*/ 

void readInput (std::vector<std::string> &packets, std::string &filename)
{
  std::ifstream file(filename);
  std::string line;
  
  while(std::getline(file, line))
  {
    if (line != "")
    {
      packets.push_back(line);
    }
  }
}

std::vector<std::string> splitPacketList (const std::string &packet)
{
  std::vector<std::string> list;
  int i = 0;
  int bracketCount = 0;
  int commaGroupStart = 0;
  bool insideComma = true;

  while (i < packet.size())
  {
    commaGroupStart = i;
    insideComma = true;

    while (insideComma)
    {
      if (packet[i] == '[') { bracketCount++; }
      if (packet[i] == ']') { bracketCount--; }

      if ((packet[i] == ',' && bracketCount == 0) || i == packet.size())
      {
        list.push_back(packet.substr(commaGroupStart, i - commaGroupStart));
        insideComma = false;
      }

      i++;
    }
  }

  return list;
}

int compareTwoValues (std::string left, std::string right)
{
  int leftVal = std::stoi(left);
  int rightVal = std::stoi(right);

  if (leftVal == rightVal) { return 2; }

  return leftVal < rightVal;
}

int compareTwoValues (int left, int right)
{
  if (left == right) { return 2; }

  return left < right;
}

// Returns: 0 if left > right, 1 if left < right, 2 if left == right
int comparePackets (const std::string &packet1, const std::string &packet2)
{
  // Remove the brackets from the outside of the string and split them into values for that list.
  std::vector<std::string> leftList = splitPacketList(packet1.substr(1, packet1.size() - 2));
  std::vector<std::string> rightList = splitPacketList(packet2.substr(1, packet2.size() - 2));

  int leftLen = leftList.size();
  int rightLen = rightList.size();
  int minLen = 0;

  if (leftLen == 0 || rightLen == 0)
  {
    return compareTwoValues(leftLen, rightLen);
  }

  if (leftLen < rightLen){ minLen = leftLen; }
  else { minLen = rightLen; }

  for (int i = 0; i < minLen; i++)
  {
    if (leftList[i][0] != '[' && rightList[i][0] != '[')
    {
      int result = compareTwoValues(leftList[i], rightList[i]);

      if (result != 2) { return result; }

      continue;
    }

    std::string newLeft = leftList[i];
    std::string newRight = rightList[i];

    if (leftList[i][0] != '[')
    {
      newLeft = std::string("[") + leftList[i] + "]";
    }
    if (rightList[i][0] != '[')
    {
      newRight = std::string("[") + rightList[i] + "]";
    }

    int compResult = comparePackets(newLeft, newRight);

    if (compResult != 2)
    {
      return compResult;
    }
  }

  return compareTwoValues(leftLen, rightLen);
}

bool comparePacketsBool (const std::string &packet1, const std::string &packet2)
{
  return comparePackets(packet1, packet2);
}

void checkPackets (const std::vector<std::string> &packets)
{

  int sumOfIndices = 0;
  for (int i = 0; i < packets.size(); i+= 2)
  {
    int val = comparePackets(packets[i], packets[i + 1]);
    
    if (val == 1)
    {
      sumOfIndices += (i / 2) + 1;
    }
    // std::cout << "Packet Index :" << i/2 + 1<< " has value " << val << std::endl;
  }

  std::cout << "Result: " << sumOfIndices << std::endl;
}

// Part 2: Sort all the packages, with 2 new values added. Determine the index of those newly added
// 'divider packets' after sorted
void sortPackets (std::vector<std::string> &packets)
{
  std::string dividerPack1 = "[[2]]";
  std::string dividerPack2 = "[[6]]";
  packets.push_back(dividerPack1);
  packets.push_back(dividerPack2);

  std::sort(packets.begin(), packets.end(), comparePacketsBool);

  int decoderKey = 1;
  for(int i = 0; i < packets.size(); i++)
  {
    // std::cout << packets[i] << std::endl;

    if (packets[i] == dividerPack1 || packets[i] == dividerPack2)
    {
      decoderKey *= i + 1;
    }
  }

  std::cout << "Decoder Key: " << decoderKey << std::endl;
}


int main (int argc, char** argv)
{
  std::string filename(argv[1]);
  std::vector<std::string> packets;

  readInput(packets, filename);

  // Part 1
  checkPackets(packets);

  // Part 2
  sortPackets(packets);

  return 0;
}