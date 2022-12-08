#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

// Read in directory info
// Build tree of files
// Traverse tree from the bottom summing up values and then counting the folders with size < limit
struct tree_node
{
  int size;
  bool isDirectory;
  std::string name;
  std::vector<tree_node> children;
  struct tree_node *parent;
};

int findMinimumViableDirectory (tree_node &tree, int memoryToRemove)
{
  int min = tree.size;

  for (auto &child : tree.children)
  {
    if (child.isDirectory && child.size < min && child.size >= memoryToRemove)
    {
      int tempMin = findMinimumViableDirectory(child, memoryToRemove);

      if (tempMin >= memoryToRemove)
      {
        min = tempMin;
      }
    }
  }

  return min;
}

void printTree (tree_node &tree, int depth = 0)
{
  for (int i = 0; i < depth; i++)
  {
    std::cout << "   -";
  }
  std::cout << tree.name << " :: " << tree.size << std::endl;
  
  for (auto& child : tree.children)
  {
    printTree(child, depth + 1);
  }
}

int sumTreeValues (tree_node &tree, int &sumOfLowNums)
{
  int sum = 0;

  for (auto& child : tree.children)
  {
    sum += sumTreeValues(child, sumOfLowNums);
  }


  tree.size = sum + tree.size;

  if (tree.isDirectory && tree.size <= 100000)
  {
    sumOfLowNums += tree.size;
  }

  return tree.size;
}

void parseCommand (const std::string command, tree_node *&currentDir, tree_node *root)
{
  std::stringstream commandstream(command);
  std::string artifact;

  std::getline(commandstream, artifact, ' '); // skip passed the $ at the beginning of the line
  std::getline(commandstream, artifact, ' ');

  if (artifact == "cd")
  {
    std::getline(commandstream, artifact, ' ');

    if (artifact == "..")
    {
      if (currentDir->parent == nullptr)
      {
        std::cerr << "Attempted to access parent of root" << std::endl;
        exit(-1);
      }

      currentDir = currentDir->parent;
    }
    else if (artifact == "/")
    {
      currentDir = root;
    }
    else 
    {
      for (auto& child : currentDir->children)
      {
        if (child.name == artifact)
        {
          currentDir = &child;
          break;
        }
      }
    }

  }
  else if (artifact == "ls")
  {
    // we don't really need to do anything
  }
}

void createChildNode (const std::string line, tree_node &currentDir)
{
  std::stringstream linestream(line);
  std::string word;
  std::string name;

  std::getline(linestream, word, ' ');
  std::getline(linestream, name, ' ');

  tree_node childNode;
  childNode.parent = &currentDir;
  childNode.name = name;

  if (word == "dir") // is a directory
  {
    childNode.size = 0;
    childNode.isDirectory = true;
  }
  else // is a file
  {
    childNode.isDirectory = false;
    childNode.size = std::stoi(word);
  }

  currentDir.children.push_back(childNode);
}

void readInput (tree_node &treeRoot, const std::string &filename)
{
  std::ifstream file(filename);
  std::string line;
  tree_node *currentDir = &treeRoot;

  while (std::getline(file, line))
  {
    /*
    Options for first word are: 
      - $ {command} for a command
        - cd {dirname or / or ..}
        - ls lists all folders and files in the current directory
      - dir {dirname} for definining a folder/directory
      - {number} {filename} for defining a file
    */

    if (line[0] == '$') // read in command
    {
      parseCommand(line, currentDir, &treeRoot);
    } 
    else
    {
      createChildNode(line, *currentDir);
    }
  }
}

int main (int argc, char** argv)
{
  std::string filename(argv[1]);
  tree_node treeRoot;
  treeRoot.size = 0;
  treeRoot.name = "/";
  treeRoot.parent = nullptr;

  readInput(treeRoot, filename);

  int sumOfLowNums = 0;
  sumTreeValues(treeRoot, sumOfLowNums);
  printTree(treeRoot);

  std::cout << sumOfLowNums << std::endl;

  // part 2
  int totalDiskSpace = 70000000;
  int requiredSpace = 30000000;
  int memoryToRemove = treeRoot.size - (totalDiskSpace - requiredSpace);

  int val = findMinimumViableDirectory(treeRoot, memoryToRemove);
  std::cout << val << std::endl;
  return 0;
}