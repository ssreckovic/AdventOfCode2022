/*
  Day 8 : Treetop Tree House
  Given a grid of numbers 0-9, with each number representing a tree's height (0 shortest, 9 tallest)

  Part 1: Count how many trees are visible from outside of the grid. A tree is considered visible if
    all other trees between it and an edge of the grid are shorter than it.
  Part 2: Find the highest scenic score of the trees, scenic score is calculated by product of the number
    of trees that can be seen in each direction.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

struct Position {
  int x, y;
};

struct Tree {
  int height;
  bool isVisible;
};

void readInput (std::vector<std::vector<Tree>> &forest, std::string &filename)
{
  std::ifstream file(filename);
  std::string line;
  int lineCount = 0;

  while (std::getline(file, line))
  {
    std::vector<Tree> treeLine;

    for (char i : line)
    {
      Tree tempTree = {
        (int)i - 48, // convert ascii of the character into the actual numerical value
        false
      };
      treeLine.push_back(tempTree); 
    }

    forest.push_back(treeLine);
  }
}

// Part 1
void checkTreeVisibility (Tree &tree, int &highestPreviousTree, int &count)
{
  if (tree.isVisible == false)
  {
    if (tree.height > highestPreviousTree)
    {
      // std::cout << "Visible: " << tree.height << std::endl;
      highestPreviousTree = tree.height;
      tree.isVisible = true;

      count += 1;
    }
  }
  else
  {
    if (tree.height > highestPreviousTree) {
      highestPreviousTree = tree.height;
    }
  }
}

int countForestVisibility (std::vector<std::vector<Tree>> &forest)
{
  int visibleCount = 0;

  for (int i = 0; i < forest.size(); i++) // loop through all rows
  {
    int highestPreviousLeft = -1;
    int highestPreviousRight = -1;

    for (int j = 0; j < forest[i].size(); j++)
    {
      // Check row from left to right
      checkTreeVisibility(forest[i][j], highestPreviousLeft, visibleCount);
      // Check row from right to left
      checkTreeVisibility(forest[i][(forest[i].size() - 1) - j], highestPreviousRight, visibleCount);
    }
  }

  for (int i = 0; i < forest[0].size(); i++) // loop through all columns
  {
    int highestPreviousAbove = -1;
    int highestPreviousBelow = -1;

    for (int j = 0; j < forest.size(); j++)
    {
      // Check row from top to bottom
      checkTreeVisibility(forest[j][i], highestPreviousAbove, visibleCount);
      // Check row from bottom to top
      checkTreeVisibility(forest[(forest.size() - 1) - j][i], highestPreviousBelow, visibleCount);
    }
  }

  return visibleCount;
}

int calculateTreeScore(const Tree &tree, const Position pos, const std::vector<std::vector<Tree>> &forest)
{
  std::vector<Position> directions = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 }};
  int score = 1;

  for (auto& dir : directions)
  {
    int x = pos.x + dir.x;
    int y = pos.y + dir.y;
    int treeCount = 0;

    while (x >= 0 && x < forest[0].size() && y >= 0 && y < forest.size())
    {
      treeCount++;

      if (forest[x][y].height >= tree.height)
      {
        break;
      }

      x += dir.x;
      y += dir.y;
    }

    score *= treeCount;
  }

  return score;
}

// Part 2
int findBestScenicTree (const std::vector<std::vector<Tree>> &forest)
{
  int bestScenicScore = 0;

  for (int i = 1; i < forest.size(); i++)
  {
    for (int j = 1; j < forest[i].size(); j++)
    {
      Position p{ i, j };
      int score = calculateTreeScore(forest[i][j], p, forest);

      if (score > bestScenicScore)
      {
        bestScenicScore = score;
      }
    }
  }

  return bestScenicScore;
}


int main (int argc, char** argv)
{
  std::string filename(argv[1]);
  std::vector<std::vector<Tree>> forest;

  readInput(forest, filename);

  int visibleTrees = countForestVisibility(forest);
  int bestScore = findBestScenicTree(forest);
  std::cout << visibleTrees << std::endl;
  std::cout << bestScore << std::endl;

  return 0;
}