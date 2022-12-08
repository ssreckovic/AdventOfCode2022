#include <string>
#include <iostream>
#include <fstream>

void readInput (std::string &packet, std::string &filename)
{
  std::ifstream file(filename);
  std::getline(file, packet);
}

// This function checks the given sequence of characters to see if there are any duplicate characters
// Returns the index of the first instance of a duplicate character,
// Returns -1 if there were no duplicate characters
int checkValuePacketStart ( const std::string &sequence )
{

  for (int i = 0; i < sequence.size(); i++)
  {
    for (int j = i + 1; j < sequence.size(); j++)
    {
      if (sequence[i] == sequence[j])
      {
        return i;
      }
    }
  }

  return -1;
}

int findPacketStartMarker (int packetIndicatorLength, std::string &packet)
{

  // Loop through entire length of packet
  // From starting index: read in 4 characters, one at a time
  /*
    m
    m j
    m j q
    m j q q

    grab 1st char: compare against 2, 3, 4, if yes continue, if no move first index to n + 1, where n is index of 1st char
    grab 2nd char: compare against 3, 4     if yes continue, if no move first index to n + 2, where n is index of 1st char
    grab 3rd char: compare against 4        if yes continue, if no move first index to n + 3, where n is index of 1st char
  */

  int marker = -1;
  int index = 0;


  while (index < packet.size() && marker == -1)
  {
    std::string sequence = packet.substr(index, packetIndicatorLength);
    int val = checkValuePacketStart(sequence);

    if (val == -1) 
    {
      std::cout << sequence << std::endl;
      return index + packetIndicatorLength;
    }

    index = index + val + 1;
  }
  
  return marker;
}

int main (int argv, char** argc) 
{
  std::string filename(argc[1]);
  std::string packet;

  readInput(packet, filename);

  // int packetStartMarker = findPacketStartMarker(4, packet);
  int packetStartMarker = findPacketStartMarker(14, packet);
  std::cout << "Result: " << packetStartMarker << std::endl;

  return 0;
}