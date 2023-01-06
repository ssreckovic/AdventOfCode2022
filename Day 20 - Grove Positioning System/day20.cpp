#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

enum Direction { Left, Right };

struct Node
{
  long long value;
  Node* next;
  Node* prev;

  Node () {};
  Node(Node* b) {
    value = b->value;
    next = b->next;
    prev = b->prev;
  }
};

void printLinkedList (Node* start, int length, long long valueMultiplier)
{
  int count = 0;
  Node* node = start;

  while (count < length)
  {
    std::cout << (node->value * valueMultiplier ) << "(" << ((node->value * valueMultiplier ) % (length - 1)) << ") -> ";
    node = node->next;
    count++;
  }
  std::cout << std::endl;
}

void buildLinkedList(std::vector<Node> &nodeOrder)
{
  int idx = 1;

  while (idx < nodeOrder.size() - 1)
  {
    nodeOrder[idx].next = &nodeOrder[idx + 1];
    nodeOrder[idx].prev = &nodeOrder[idx - 1];
    idx++;
  }

  nodeOrder[0].next = &nodeOrder[1];
  nodeOrder[0].prev = &nodeOrder[nodeOrder.size() - 1];
  nodeOrder[nodeOrder.size() - 1].next = &nodeOrder[0];
  nodeOrder[nodeOrder.size() - 1].prev = &nodeOrder[nodeOrder.size() - 2];
}

void readInput (std::vector<Node> &nodeOrder, const std::string &filename)
{
  std::ifstream file(filename);
  std::string line;
  int idx = 0;
  while(std::getline(file, line))
  {
    Node current;
    current.value = std::stoi(line);
    nodeOrder.push_back(current);
  }

  buildLinkedList(nodeOrder);
}

Node* findNodeValue (std::vector<Node> &nodeOrder, int target)
{
  for (auto &node : nodeOrder)
  {
    if (node.value == target)
    {
      return &node;
    }
  }

  return NULL;
}

Node* traverseList (Node* start, long long travelDistance, int listSize)
{
  Node* current = start;
  Node* traversalNode = new Node(start);
  long long movement = (travelDistance % (listSize - 1));

  while (movement != 0)
  {
    if (movement < 0)
    {
      traversalNode = traversalNode->prev;
      movement++;
    }
    else // movement > 0 
    {
      traversalNode = traversalNode->next;
      movement--;
    }
  }

  return traversalNode;
}

void getSolutionValues (Node* start, int listLength, long long valueMultiplier)
{
  std::vector<int> targets = { 1000, 2000, 3000 };
  Node* tar;
  long long total = 0;

  // printLinkedList(start, listLength, valueMultiplier);

  for (auto &target : targets )
  {
    tar = traverseList(start, target, listLength);
    total += (tar->value * valueMultiplier);
    std::cout << target << " index is = " << (tar->value * valueMultiplier) << std::endl;
  }
 
  std::cout << "Final Value is " << total << std::endl;
  delete tar;
}

void moveNode (Node* start, Node* end, Direction dir)
{
  Node* endNext = end->next;
  Node* endPrev = end->prev;
  Node* startNext = start->next;
  Node* startPrev = start->prev;

  // Make the nodes adjacent to the place you just left point to each other
  startNext->prev = startPrev;
  startPrev->next = startNext;

  if (dir == Left)
  {
    start->next = end;
    start->prev = endPrev;
    end->prev = start;
    endPrev->next = start;
  }
  else if (dir == Right)
  {
    start->next = endNext;
    start->prev = end;
    end->next = start;
    endNext->prev = start;
  }
}

void mixList(std::vector<Node> &nodeOrder, long long valueMultiplier = 1, int mixingMultiplier = 1)
{
  Node* start = findNodeValue(nodeOrder, 0);
  Node* traversalNode;
  int listSize = nodeOrder.size();

  for (int i = 0; i < (listSize * mixingMultiplier); i++)
  {
    Node* current = &nodeOrder[i % listSize];
    traversalNode = traverseList(current, (current->value * valueMultiplier), listSize);

    if ((current->value * valueMultiplier) % (listSize - 1) == 0)
    {
      continue;
    }

    if (current->value < 0)
    {
      moveNode(current, traversalNode, Left);
    }
    else
    {
      moveNode(current, traversalNode, Right);
    }
  }
  
  getSolutionValues(start, listSize, valueMultiplier);
}

int main (int argc, char** argv)
{
  std::string filename(argv[1]);
  std::vector<Node> nodeOrder;

  readInput(nodeOrder, filename);
  // Part 1
  // mixList(nodeOrder);
  // Part 2
  long long valueMultiplier = 811589153;
  mixList(nodeOrder, valueMultiplier, 10);
}